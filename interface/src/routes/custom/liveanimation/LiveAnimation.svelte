<!--
   @title     MoonLight
   @file      LiveAnimation.svelte
   @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
   @Authors   https://github.com/MoonModules/MoonLight/commits/main
   @Doc       https://moonmodules.org/MoonLight/moonlight/liveanimation/
   @Copyright © 2025 Github MoonLight Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
-->

<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import LightIcon from '~icons/tabler/bulb';
	import { socket } from '$lib/stores/socket';
	import type { LiveAnimationState } from '$lib/types/models_custom';
	import Spinner from '$lib/components/Spinner.svelte';
	import FileEdit from '$lib/components/custom/FileEdit.svelte';
	import MultiInput from '$lib/components/custom/MultiInput.svelte';

	let state: any = $state({});

	let dataLoaded = false;
	const endPoint = "liveAnimation";

	async function getState() {
		try {
			const response = await fetch('/rest/' + endPoint, {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			// console.log("getState", endPoint);
			handleState(await response.json());
		} catch (error) {
			console.error('Error:', error);
		}
	}

	const handleState = (data: any) => {
		console.log("handleState", data);
		state = data;
		dataLoaded = true;
	};

	onMount(() => {
		// console.log("onMount", endPoint);
		socket.on(endPoint, handleState);
		// getState(); //done in settingscard
	});

	onDestroy(() => {
		// console.log("onDestroy", endPoint);
		socket.off(endPoint, handleState);
	});

	function sendSocket() {
		console.log("sendSocket", endPoint, state);
		if (dataLoaded) 
			socket.sendEvent(endPoint, state)
	}

	async function postState() {
		try {
			const response = await fetch('/rest/' + endPoint, {
				method: 'POST',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(state)
			});
			if (response.status == 200) {
				notifications.success(endPoint + ' updated.', 3000);
				state = await response.json();
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}
</script>

<SettingsCard collapsible={false}>
	{#snippet icon()}
		<LightIcon  class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	{/snippet}
	{#snippet title()}
		<span >Live Animation</span>
	{/snippet}
	{#await getState()}
		<Spinner />
	{:then nothing}
	<div class="w-full">
		<MultiInput property={{name:"lightsOn", type:"checkbox"}} bind:value={state.lightsOn} onChange={sendSocket}></MultiInput>
		<MultiInput property={{name:"Brightness", type:"range", min:0, max:255}} bind:value={state.brightness} onChange={sendSocket}></MultiInput>
		<MultiInput property={{name:"Animation", type:"selectFile", values:state.animations}} bind:value={state.animation} onChange={sendSocket}></MultiInput>
		<MultiInput property={{name:"driverOn", type:"checkbox"}} bind:value={state.driverOn} onChange={sendSocket}></MultiInput>
	</div>
	{/await}
</SettingsCard>
