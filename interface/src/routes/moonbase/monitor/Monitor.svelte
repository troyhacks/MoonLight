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
			console.log("requestLayout submitted");
			const arrayBuffer = await response.json();
			console.log("requestLayout received", arrayBuffer);
		// } catch (error) {
		// 	console.error('Error:', error);
		// }
	}

	const handleMonitor = (data: Uint8Array) => {

		if (data.length == 29)
			handleHeader(data)
		else {
			if (isPositions) {
				handlePositions(data)
				isPositions = false;
			}
			else
				handleChannels(data);
		}
	};

	// function unpackCoord3DInto3Bytes(xin: number, yin: number, zin:number) {
	// 	// Combine 3 bytes into a 24-bit number
	// 	const packed = (xin << 16) | (yin << 8) | zin;

	// 	const x = (packed >> 13) & 0x7FF; // 11 bits
	// 	const y = (packed >> 5) & 0xFF;   // 8 bits
	// 	const z = packed & 0x1F;          // 5 bits

	// 	return { x, y, z };
	// }

	// JavaScript unpacking function
	function unpackCoord3DInto3Bytes(xin: number, yin: number, zin:number) {
		// Bit masks for each mode
		const MODE0_X_MASK = 0x7F;    // 7 bits
		const MODE0_Y_MASK = 0x7F;    // 7 bits
		const MODE0_Z_MASK = 0xFF;    // 8 bits
		
		const MODE1_X_MASK = 0x1FF;   // 9 bits
		const MODE1_Y_MASK = 0x1;     // 1 bit
		const MODE1_Z_MASK = 0xFFF;   // 12 bits
		
		const MODE2_X_MASK = 0x1;     // 1 bit
		const MODE2_Y_MASK = 0x1FF;   // 9 bits
		const MODE2_Z_MASK = 0xFFF;   // 12 bits
		
		const MODE3_X_MASK = 0x1F;    // 5 bits
		const MODE3_Y_MASK = 0x1F;    // 5 bits
		const MODE3_Z_MASK = 0x1F;    // 5 bits
		
		// Combine 3 bytes into a 24-bit number
		const packed = (xin << 16) | (yin << 8) | zin;
		
		// Extract mode from top 2 bits
		const mode = (packed >> 22) & 0x3;
		
		let x, y, z;
		
		switch(mode) {
			case 0: // x=7, y=7, z=8 (max: 2^7=128, 2^7=128, 2^8=256) - for 128x128x1
				x = (packed >> 15) & MODE0_X_MASK;
				y = (packed >> 8) & MODE0_Y_MASK;
				z = packed & MODE0_Z_MASK;
				break;
			case 1: // x=9, y=1, z=12 (max: 2^9=512, 1, 2^12=4096) - for 300x1x1 and 1x1x300
				x = (packed >> 13) & MODE1_X_MASK;
				y = (packed >> 12) & MODE1_Y_MASK;
				z = packed & MODE1_Z_MASK;
				break;
			case 2: // x=1, y=9, z=12 (max: 1, 2^9=512, 2^12=4096) - for 1x300x1
				x = (packed >> 21) & MODE2_X_MASK;
				y = (packed >> 12) & MODE2_Y_MASK;
				z = packed & MODE2_Z_MASK;
				break;
			case 3: // x=5, y=5, z=5 (max: 2^5=32, 2^5=32, 2^5=32) - for 30x30x30
				x = (packed >> 17) & MODE3_X_MASK;
				y = (packed >> 12) & MODE3_Y_MASK;
				z = (packed >> 7) & MODE3_Z_MASK;
				break;
			default:
				throw new Error("Invalid packing mode");
		}
		
		return { x, y, z }; //, mode
	}

	let nrOfLights:number;
	let channelsPerLight:number;
	let offsetRGB:number;
	let isPositions: boolean = false;
	// let offsetRed:number;
	// let offsetGreen:number;
	// let offsetBlue:number;
	// let offsetWhite:number;

	const handleHeader = (header: Uint8Array) => {
		console.log("Monitor.handleHeader", header);

		// let isPositions:number = header[6]; 
		isPositions   = true;//(header[6] >> 0) & 0x3; // bits 0-1
		// offsetRed     = (header[6] >> 2) & 0x3; // bits 2-3
		// offsetGreen   = (header[6] >> 4) & 0x3; // bits 4-5
		// offsetBlue    = (header[6] >> 6) & 0x3; // bits 6-7
		// offsetWhite   = header[13];

		nrOfLights = header[4] + 256 * header[5];
		channelsPerLight = header[11];
		offsetRGB = header[12];

		//rebuild scene
		createScene(el);

		// let ledFactor: number = 1;//header[1];
		// let ledSize: number = header[23];
		width = header[0] + 256 * header[1]; //max 2047
		height = header[2];
		depth = header[3];

		// let nrOfLights = header[4] + 256 * header[5];

		console.log("Monitor.handleHeader", nrOfLights, channelsPerLight, width, height, depth);

	}

	const handlePositions = (positions: Uint8Array) => {
		console.log("Monitor.handlePositions", positions);

		for (let index = 0; index < nrOfLights * 3; index +=3) {
			let position = unpackCoord3DInto3Bytes(positions[index], positions[index+1], positions[index+2])

			//set to -1,1 coordinate system of webGL
			//width -1 etc as 0,0 should be top left, not bottom right
			position.x = width==1?0:(((position.x) / (width - 1)) * 2.0 - 1.0);
			position.y = height==1?0:(((height-1-position.y) / (height - 1)) * 2.0 - 1.0);
			position.z = depth==1?0:(((depth-1-position.z) / (depth - 1)) * 2.0 - 1.0);

			vertices.push(position.x, position.y, position.z);
		}
	}

	const handleChannels = (channels: Uint8Array) => {

		if (!done) {
			requestLayout(); //ask for positions
			console.log("Monitor.handleChannels", channels);
			done = true;
		}
		clearColors();
		for (let index = 0; index < nrOfLights * channelsPerLight; index +=channelsPerLight) {
			// colorLed(index/3, data[index]/255, data[index+1]/255, data[index+2]/255);
			const r = channels[index + offsetRGB + 0] / 255;
			const g = channels[index + offsetRGB + 1] / 255;
			const b = channels[index + offsetRGB + 2] / 255;
			const a = 1.0; // Full opacity
			colors.push(r, g, b, a);
		}

		updateScene(vertices, colors);
	};

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