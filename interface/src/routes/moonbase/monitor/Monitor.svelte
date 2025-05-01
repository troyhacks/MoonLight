<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	// import { addLed, colorLed, createScene } from './monitor';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { socket } from '$lib/stores/socket';
	import ControlIcon from '~icons/tabler/adjustments';

	let el:HTMLCanvasElement;

	let width = -1;
	let height = -1;
	let depth = -1;

	let str = "Monitor";

	let done = false; //temp to show one instance of monitor data receiced

	// enum ChannelType {
	const ct_Leds=0
	// 	ct_LedsRGBW,
	const ct_Position=2
	// 	ct_Channels,
	// 	ct_MovingHead,
	// 	ct_CrazyCurtain,
	// 	ct_count
	// };

	const handleMonitor = (lights: Uint8Array) => {
        const headerLength = 5+12; // Define the length of the header
        const header = lights.slice(0, headerLength);
        const data = lights.slice(headerLength);

		let type:number = header[0];
		
		//fixChange
		if (type == ct_Position) {
			console.log(lights)
			handleLightsDefinition(header, data);
		} else if (type == ct_Leds) {
			if (!done)
				console.log("Monitor.handleMonitor", data);
			for (let index = 0; index < data.length; index +=3) {
				// colorLed(index/3, data[index]/255, data[index+1]/255, data[index+2]/255);
				// str = "("+ data[index] + "," + data[index+1] + "," + data[index+2] + ")," + str;
			}
			done = true;
		}
	};

	const handleLightsDefinition = (header: Uint8Array, positions: Uint8Array) => {
		console.log("Monitor.handleLightsDefinition", header, positions);
		// data.forEach((value, index) => {
    	//     console.log(`Index ${index}: ${value}`);
	    // });

		//rebuild scene
		// createScene(el);

		str = "Header: " + header + "\n";
		str += "Data Size: " + positions.length / 12 + "\n";

		width = 0;
		height = 0;
		depth = 0;
		let ledFactor: number = 1;//header[1];
		let ledSize: number = header[2];
		// data[3]=0; data[4]=0; data[5]=0;

		//parse 1
		for (let index = 0; index < positions.length; index +=12) {
			// console.log(data[index], data[index+1], data[index+2]);
			let x = (positions[index+2] * 256 + positions[index+3]) / ledFactor;
			let y = (positions[index+6] * 256 + positions[index+7]) / ledFactor;
			let z = (positions[index+10] * 256 + positions[index+11]) / ledFactor;

			str += "("+ x + "," + y + "," + z + "),";

			if (x > width) width = x;
			if (y > height) height = y;
			if (z > depth) depth = z;
		}

		width = Math.ceil(width) + 1;
		height = Math.ceil(height) + 1;
		depth = Math.ceil(depth) + 1;
		// console.log("dimensions", width, height, depth);
		// //parse 2
		// for (let index = 0; index < positions.length; index +=3) {
		// 	let x = positions[index] / ledFactor;
		// 	let y = positions[index+1] / ledFactor;
		// 	let z = positions[index+2] / ledFactor;
		// 	// addLed(0.3, x - width/2, y - height/2, z - depth/2);
		// }
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

	<!-- <div class="w-full overflow-x-auto"> -->
	 <div>
		<!-- <canvas bind:this={el}></canvas> -->
		<textarea  class="w-full h-full" readonly>{str}</textarea>
	</div>

</SettingsCard>