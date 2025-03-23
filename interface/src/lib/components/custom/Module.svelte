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
	import Spinner from '$lib/components/Spinner.svelte';
	import MultiInput from '$lib/components/custom/MultiInput.svelte';

	interface Props {
		endPoint: string;
		children?: import('svelte').Snippet;
	}

	let {
		endPoint ,
		children
	}: Props = $props();

	let state: any = $state({});

	let dataLoaded = false;
	let stateString: string = $state("");

	async function getState() {
		try {
			const response = await fetch('/rest/' + endPoint, {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			console.log("getState", endPoint);
			handleState(await response.json());
		} catch (error) {
			console.error('Error:', error);
		}
	}

	const handleState = (data: any) => {
		console.log("handleState", data);
		state = data;
		stateString = JSON.stringify(state)
		dataLoaded = true;
	};

	onMount(() => {
		console.log("onMount ");
		socket.on(endPoint, handleState);
	});

	onDestroy(() => {
		console.log("onDestroy");
		socket.off(endPoint, handleState);
	});

	export function sendSocket() {
		console.log("sendSocket", state);
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
				notifications.success('state updated.', 3000);
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
		<span >{endPoint}</span>
	{/snippet}
	{#await getState()}
		<Spinner />
	{:then nothing}
	<div class="w-full">
		{#if children == null}
			<MultiInput property={{name:"State", type:"textarea"}} bind:value={stateString} 
				onChange={(event) => {
					console.log("onChange", event.target.value);
			}}>
			</MultiInput>
		{/if}
		<div class="w-full">
			{@render children?.()}
		</div>
	</div>
	{/await}
</SettingsCard>
