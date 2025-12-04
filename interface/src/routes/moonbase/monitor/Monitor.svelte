<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import {
		clearColors,
		colors,
		vertices,
		createScene,
		updateScene,
		setMatrixDimensions
	} from './monitor';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { socket } from '$lib/stores/socket';
	import ControlIcon from '~icons/tabler/adjustments';

	let el: HTMLCanvasElement;

	let width = -1;
	let height = -1;
	let depth = -1;

	let done = false; //temp to show one instance of monitor data receiced

	//ask the server to run the mapping, the resulting positions are sent by websocket monitor
	const requestLayout = async () => {
		// try {
		const response = await fetch('/rest/monitorLayout', {
			method: 'GET',
			headers: {
				'Content-Type': 'application/json'
			}
		});
		console.log('requestLayout submitted');
		const arrayBuffer = await response.json();
		console.log('requestLayout received', arrayBuffer);
		// } catch (error) {
		// 	console.error('Error:', error);
		// }
	};

	const handleMonitor = (data: Uint8Array) => {
		if (data.length == 37)
			//see ModuleLightsControl.h:243
			handleHeader(data);
		else {
			if (isPositions) {
				handlePositions(data);
				isPositions = false;
			} else handleChannels(data);
		}
	};

	let nrOfLights: number;
	let channelsPerLight: number;
	let offsetRGB: number;
	let offsetWhite: number;
	let isPositions: boolean = false;
	// let offsetRed:number;
	// let offsetGreen:number;
	// let offsetBlue:number;

	const handleHeader = (header: Uint8Array) => {
		console.log('Monitor.handleHeader', header);

		let view = new DataView(header.buffer);

		// let isPositions:number = header[6];
		isPositions = true; //(header[6] >> 0) & 0x3; // bits 0-1
		// offsetRed     = (header[6] >> 2) & 0x3; // bits 2-3
		// offsetGreen   = (header[6] >> 4) & 0x3; // bits 4-5
		// offsetBlue    = (header[6] >> 6) & 0x3; // bits 6-7
		// offsetWhite   = header[13];

		nrOfLights = view.getUint16(12, true); //  header[12] + 256 * header[13];
		channelsPerLight = view.getUint8(19); //header[19];
		offsetRGB = view.getUint8(20); //header[20];
		offsetWhite = view.getUint8(21); //header[21];

		//rebuild scene
		createScene(el);

		// let ledFactor: number = 1;//header[1];
		// let ledSize: number = header[23];
		width = view.getInt32(0, true); //header[0] + 256 * header[1];
		height = view.getInt32(4, true); //header[4] + 256 * header[5];;
		depth = view.getInt32(8, true); //header[8] + 256 * header[9];;

		setMatrixDimensions(width, height);

		// let nrOfLights = header[4] + 256 * header[5];

		console.log(
			'Monitor.handleHeader',
			width,
			height,
			depth,
			nrOfLights,
			channelsPerLight,
			offsetRGB
		);
	};

	const handlePositions = (positions: Uint8Array) => {
		console.log('Monitor.handlePositions', positions);

		for (let index = 0; index < nrOfLights * 3; index += 3) {
			let x = positions[index];
			let y = positions[index + 1];
			let z = positions[index + 2];

			//set to -1,1 coordinate system of webGL
			//width -1 etc as 0,0 should be top left, not bottom right
			x = width == 1 ? 0 : (x / (width - 1)) * 2.0 - 1.0;
			y = height == 1 ? 0 : ((height - 1 - y) / (height - 1)) * 2.0 - 1.0;
			z = depth == 1 ? 0 : ((depth - 1 - z) / (depth - 1)) * 2.0 - 1.0;

			vertices.push(x, y, z);
		}
	};

	const handleChannels = (channels: Uint8Array) => {
		if (!done) {
			requestLayout(); //ask for positions
			console.log('Monitor.handleChannels', channels);
			done = true;
		}
		clearColors();
		//max size supported is 255x255x255 (index < width * height * depth) ... todo: only any of the component < 255
		for (let index = 0; index < nrOfLights * channelsPerLight; index += channelsPerLight) {
			// && index < width * height * depth
			// colorLed(index/3, data[index]/255, data[index+1]/255, data[index+2]/255);
			const r = channels[index + offsetRGB + 0] / 255;
			const g = channels[index + offsetRGB + 1] / 255;
			const b = channels[index + offsetRGB + 2] / 255;
			let w = 0;
			if (offsetWhite != 255) w = channels[index + offsetRGB + 3] / 255; //add white channel if present
			const a = 1.0; // Full opacity
			colors.push(r + w, g + w, b + w, a);
		}

		updateScene(vertices, colors);
	};

	onMount(() => {
		console.log('onMount Monitor');
		socket.on('monitor', handleMonitor);
	});

	onDestroy(() => {
		console.log('onDestroy Monitor');
		socket.off('monitor', handleMonitor);
	});
</script>

<SettingsCard collapsible={false}>
	{#snippet icon()}
		<ControlIcon class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	{/snippet}
	{#snippet title()}
		<span>Monitor</span>
	{/snippet}

	<div class="w-full overflow-x-auto">
		<canvas bind:this={el} width="720" height="360"></canvas>
	</div>
</SettingsCard>
