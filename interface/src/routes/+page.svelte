<script lang="ts">
	import type { PageData } from './$types';
	import logo from '$lib/assets/logo.png';
	import { notifications } from '$lib/components/toasts/notifications';

	interface Props {
		data: PageData;
	}

	let { data }: Props = $props();
</script>

<div class="hero bg-base-100 h-screen">
	<div class="card md:card-side bg-base-200 shadow-primary shadow-2xl">
		<figure class="bg-base-200"><img src={logo} alt="Logo" class="h-auto w-64" /></figure>
		<div class="card-body w-80">
			<h2 class="card-title text-center text-2xl">Welcome to MoonLight</h2>
			<p class="py-6 text-center">
				MoonLight is a fork of ESP32-SvelteKit and add Files, Modules, Light control and much more, ready to flash on ESP32 devices.
				We can be found on 
				<a
					href="https://discord.gg/TC8NSUSCdV"
					class="link"
					target="_blank"
					rel="noopener noreferrer">Discord WLED MM 2D & Audio</a
				>, 
				<a
					href="https://moonmodules.org"
					class="link"
					target="_blank"
					rel="noopener noreferrer">MoonModules.org</a
				> and
				<a
					href="https://reddit.com/r/MoonModules"
					class="link"
					target="_blank"
					rel="noopener noreferrer">Reddit</a
				>.
			</p>
			<a
				class="btn btn-primary"
				href="/system/status"
				onclick={() => notifications.success('You did it!', 1000)}>Start</a
			>
		</div>
	</div>
</div>
