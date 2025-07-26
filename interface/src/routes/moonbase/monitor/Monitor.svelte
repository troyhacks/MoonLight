<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import {clearColors, colors, vertices, createScene, updateScene } from './monitor';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { socket } from '$lib/stores/socket';
	import ControlIcon from '~icons/tabler/adjustments';

	let el:HTMLCanvasElement;

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
			const arrayBuffer = await response.json();
			console.log("requestLayout", arrayBuffer);
		// } catch (error) {
		// 	console.error('Error:', error);
		// }
	}

	const handleMonitor = (lights: Uint8Array) => {

        const headerLength = 24; // Define the length of the header
        const header = lights.slice(0, headerLength);
        const data = lights.slice(headerLength);

		// let isPositions:number = header[6]; 
		const isPositions   = (header[6] >> 0) & 0x3; // bits 0-1
		const offsetRed     = (header[6] >> 2) & 0x3; // bits 2-3
		const offsetGreen   = (header[6] >> 4) & 0x3; // bits 4-5
		const offsetBlue    = (header[6] >> 6) & 0x3; // bits 6-7

		let nrOfLights = header[4] + 256 * header[5];
		let channelsPerLight:number = header[11];
		let offsetRGB:number = header[12];
		
		if (!(isPositions==2)) { 
			//(type == ct_Leds) {
			if (!done) {
				requestLayout(); //ask for positions
				console.log("Monitor.handleMonitor", data);
				done = true;
			}
			clearColors();
			for (let index = 0; index < nrOfLights * channelsPerLight; index +=channelsPerLight) {
				// colorLed(index/3, data[index]/255, data[index+1]/255, data[index+2]/255);
				const r = data[index + offsetRGB] / 255;
				const g = data[index + 1 + offsetRGB] / 255;
				const b = data[index + 2 + offsetRGB] / 255;
				const a = 1.0; // Full opacity
				colors.push(r, g, b, a);
			}
			updateScene(vertices, colors);
		} else { 
			//layout Change
			// console.log(lights)
			handleLayout(header, data);
		}
	};

	function unpackCoord3DInto3Bytes(xin: number, yin: number, zin:number) {
		// Combine 3 bytes into a 24-bit number
		const packed = (xin << 16) | (yin << 8) | zin;

		const x = (packed >> 13) & 0x7FF; // 11 bits
		const y = (packed >> 5) & 0xFF;   // 8 bits
		const z = packed & 0x1F;          // 5 bits

		return { x, y, z };
	}

	const handleLayout = (header: Uint8Array, positions: Uint8Array) => {
		console.log("Monitor.handleLayout positions", header, positions);

		//rebuild scene
		createScene(el);

		// let ledFactor: number = 1;//header[1];
		// let ledSize: number = header[23];
		width = header[0] + 256 * header[1]; //max 2047
		height = header[2];
		depth = header[3];

		let nrOfLights = header[4] + 256 * header[5];

		console.log("Monitor.handleLayout", nrOfLights, width, height, depth);

		for (let index = 0; index < nrOfLights * 3; index +=3) {
			// console.log(data[index], data[index+1], data[index+2]);
			let position = unpackCoord3DInto3Bytes(positions[index], positions[index+1], positions[index+2])

			//set to -1,1 coordinate system of webGL
			//width -1 etc as 0,0 should be top left, not bottom right
			position.x = width==1?0:(((position.x) / (width - 1)) * 2.0 - 1.0);
			position.y = height==1?0:(((height-1-position.y) / (height - 1)) * 2.0 - 1.0);
			position.z = depth==1?0:(((depth-1-position.z) / (depth - 1)) * 2.0 - 1.0);

			vertices.push(position.x, position.y, position.z);
		}
	}


	onMount(() => {
		console.log("onMount Monitor")
		socket.on("monitor", handleMonitor);

	});

	onDestroy(() => {
		console.log("onDestroy Monitor");
		socket.off("monitor", handleMonitor);
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