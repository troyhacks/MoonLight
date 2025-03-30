<script lang="ts">
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Spinner from '$lib/components/Spinner.svelte';
	import Router from '~icons/tabler/router';
	import Cancel from '~icons/tabler/x';
	import MultiInput from '$lib/components/custom/MultiInput.svelte';
	import { onMount, onDestroy } from 'svelte';
	import { socket } from '$lib/stores/socket';
	import Array from '$lib/components/custom/Array.svelte';

	let definition: any = $state([]);
	let data: any = $state({});

	let showEditor: boolean = $state(false);
	let changed: boolean = $state(false);

	const modeWS: boolean = true; //todo: make this an argument

	async function getState() {

		let moduleName = page.url.searchParams.get('module');

		console.log("getState", '/rest/' + moduleName)

		//load definition
		try {
			const response = await fetch('/rest/' + moduleName + 'Def', {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			definition = await response.json();
			console.log("definition", definition)
		} catch (error) {
			console.error('Error:', error);
		}

		console.log("get data", '/rest/' + moduleName)
		//load data
		try {
			const response = await fetch('/rest/' + moduleName, {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			handleState(await response.json())
			// console.log("data", data)
		} catch (error) {
			console.error('Error:', error);
		}
		return data;
	}

	async function postState() {
		//validation (if needed) here?
		//optional checks if the whole state is correct

		let moduleName = page.url.searchParams.get('module')

		try {
			const response = await fetch('/rest/' + moduleName, {
				method: 'POST',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(data)
			});
			if (response.status == 200) {
				notifications.success('Module settings updated.', 3000);
				data = await response.json();
				changed = false;
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
		showEditor = false;
	}

	function cancelState() {
		getState();
		showEditor = false;
		changed = false;
	}

	function inputChanged() {
		if (modeWS) {
			let moduleName = page.url.searchParams.get('module')||'';
			socket.sendEvent(moduleName, data)
		} else {
			changed = true;
		}
	}

	const handleState = (state: any) => {
		console.log("handleState", state);
		data = state;
		showEditor = false;
	};

	onMount(() => {
		if (modeWS) {
			let moduleName = page.url.searchParams.get('module') || ''
			console.log("onMount", moduleName);
			socket.on(moduleName, handleState);
		}
	});

	onDestroy(() => {
		if (modeWS) {
			let moduleName = page.url.searchParams.get('module') || ''
			console.log("onDestroy", moduleName);
			socket.off(moduleName, handleState);
		}
	});

</script>

<SettingsCard collapsible={false}>
	{#snippet icon()}
		<Router  class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	{/snippet}
	{#snippet title()}
		<span >Module {page.url.searchParams.get('module')}</span>
	{/snippet}

	{#if !page.data.features.security || $user.admin}
		<div class="bg-base-200 shadow-lg relative grid w-full max-w-2xl self-center overflow-hidden">
			{#await getState()}
				<Spinner />
			{:then nothing}
				<!-- <div class="grid w-full grid-cols-1 content-center gap-x-4 px-4 sm:grid-cols-2"> -->
				<div class="relative w-full overflow-visible">
					{#each definition as property}
						{#if property.type != "array"}
							<div>
								<MultiInput property={property} bind:value={data[property.name]} onChange={inputChanged} changeOnInput={!modeWS}></MultiInput>
							</div>
						{:else if property.type == "array"}
							<Array property={property} bind:value1={data[property.name]} value2={data[property.name]} data={data} definition={definition} onChange={inputChanged} changeOnInput={!modeWS}></Array>
						{/if}
					{/each}
				</div>

				{#if !modeWS}
					<div class="divider mb-2 mt-0"></div>
					<div class="mx-4 mb-4 flex flex-wrap justify-end gap-2">
						<button class="btn btn-primary" type="button" onclick={cancelState} disabled={!changed}
							>Cancel</button
						>
						<button class="btn btn-primary" type="button" onclick={postState} disabled={!changed}
							>Save</button
						>
					</div>
				{/if}
			{/await}
		</div>
	{/if}
</SettingsCard>
