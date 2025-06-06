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

		let channelsPerLight:number = header[15];
		let offsetRGB:number = header[16];
		let isPositions:number = header[21];
		
		if (!(isPositions==10)) { //(type == ct_Leds) {
			if (!done) {
				requestLayout(); //ask for positions
				console.log("Monitor.handleMonitor", data);
				done = true;
			}
			clearColors();
			for (let index = 0; index < data.length; index +=channelsPerLight) {
				// colorLed(index/3, data[index]/255, data[index+1]/255, data[index+2]/255);
				const r = data[index + offsetRGB] / 255;
				const g = data[index + 1 + offsetRGB] / 255;
				const b = data[index + 2 + offsetRGB] / 255;
				const a = 1.0; // Full opacity
				colors.push(r, g, b, a);
			}
			updateScene(vertices, colors);
		} else { //layout Change
			// console.log(lights)
			handleLayout(header, data);
		}
	};

	const handleLayout = (header: Uint8Array, positions: Uint8Array) => {
		console.log("Monitor.handleLayout positions", header, positions);

		//rebuild scene
		createScene(el);

		let ledFactor: number = 1;//header[1];
		let ledSize: number = header[23];
		width = header[1] * 256 + header[0];
		height = header[5] * 256 + header[4];
		depth = header[9] * 256 + header[8];

		console.log("Monitor.handleLayout", ledFactor, ledSize, width, height, depth);

		for (let index = 0; index < positions.length; index +=12) {
			// console.log(data[index], data[index+1], data[index+2]);
			//this is weird... where is the first position?
			let x = (positions[index-1] * 256 + positions[index]) / ledFactor;
			let y = (positions[index+3] * 256 + positions[index+4]) / ledFactor;
			let z = (positions[index+7] * 256 + positions[index+8]) / ledFactor;

			//set to -1,1 coordinate system of webGL
			//width -1 etc as 0,0 should be top left, not bottom right
			x = width==1?0:((width-1-x) / (width - 1)) * 2.0 - 1.0;
			y = height==1?0:((height-1-y) / (height - 1)) * 2.0 - 1.0;
			z = depth==1?0:((depth-1-z) / (depth - 1)) * 2.0 - 1.0;

			vertices.push(x, y, z);
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
	 <div>
		<canvas bind:this={el} width="720" height="360"></canvas>
	</div>

</SettingsCard>