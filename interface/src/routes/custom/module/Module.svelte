<script lang="ts">
	import { slide } from 'svelte/transition';
	import { cubicOut } from 'svelte/easing';
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import { notifications } from '$lib/components/toasts/notifications';
	import DragDropList, { VerticalDropZone, reorder, type DropEvent } from 'svelte-dnd-list';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Spinner from '$lib/components/Spinner.svelte';
	import Router from '~icons/tabler/router';
	import Add from '~icons/tabler/circle-plus';
	import Edit from '~icons/tabler/pencil';
	import Delete from '~icons/tabler/trash';
	import Cancel from '~icons/tabler/x';
	import MultiInput from '$lib/components/custom/MultiInput.svelte';

	let definition: any = $state([]);
	let data: any = $state({});

	let dataEditable: any = $state({});

	let propertyEditable: string = $state("");
	let showEditor: boolean = $state(false);
	let changed: boolean = $state(false);

	async function getState() {

		let params = new URLSearchParams(page.url.search);
		let moduleName = params.get('module') || "module";

		console.log("get definition", '/rest/' + moduleName + 'Def')

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
			data = await response.json();
			console.log("data", data)
		} catch (error) {
			console.error('Error:', error);
		}
		return data;
	}

	async function postState(data: any) {
		let params = new URLSearchParams(page.url.search);
		let moduleName = params.get('module') || "module";

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

	function validateAndSaveState() {
		//optional checks if the whole state is correct

		postState(data);
		changed = false;
	}

	function addItem(propertyName: string) {
		propertyEditable = propertyName;
		//set the default values from the definition...
		dataEditable = {};

		//set properties with their defaults
		for (let i = 0; i < definition.length; i++) {
			let property = definition[i];
			console.log("addItem def", propertyName, property)
			if (property.name == propertyName) {
				for (let i=0; i < property.n.length; i++) {
					let propertyN = property.n[i];
					console.log("propertyN", propertyN)
					dataEditable[propertyN.name] = propertyN.default;
				}
			}
		}
	}

	function inputChanged() {
		changed = true;
	}

	function handleEdit(propertyName: string, index: number) {
		console.log("handleEdit", propertyName, index)
		propertyEditable = propertyName;
		showEditor = true;
		dataEditable = data[propertyName][index];
	}

	function deleteItem(propertyName: string, index: number) {
		// Check if item is currently been edited and delete as well
		if (data[propertyName][index].animation === dataEditable.animation) {
			addItem(propertyName);
		}
		// Remove item from array
		data[propertyName].splice(index, 1);
		data[propertyName] = [...data[propertyName]]; //Trigger reactivity
		showEditor = false;
		changed = true;
	}

	function onDrop(propertyName: string, { detail: { from, to } }: CustomEvent<DropEvent>) {
		
		if (!to || from === to) {
			return;
		}

		data[propertyName] = reorder(data[propertyName], from.index, to.index);
		changed = true;
		console.log(onDrop, data[propertyName]);
	}

    function preventDefault(fn: any) {
		return function (event: any) {
			event.preventDefault();
			fn.call(this, event);
		};
	}
</script>

<SettingsCard collapsible={false}>
	{#snippet icon()}
		<Router  class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	{/snippet}
	{#snippet title()}
		<span >Module {new URLSearchParams(page.url.search).get('module') || 'module'}</span>
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
								<MultiInput property={property} bind:value={data[property.name]} onChange={inputChanged}></MultiInput>
							</div>
						{:else if property.type == "array"}
							<div class="divider mb-2 mt-0"></div>
							<div class="h-16 flex w-full items-center justify-between space-x-3 p-0 text-xl font-medium">
								{property.name}
							</div>
							<div class="relative w-full overflow-visible">
							<!-- <div class="mx-4 mb-4 flex flex-wrap justify-end gap-2"> -->
								<button
									class="btn btn-primary text-primary-content btn-md absolute -top-14 right-0"
									onclick={() => {
										addItem(property.name);
										changed = true;

										//add the new item to the data
										data[property.name].push(dataEditable);
										showEditor = true;
									}}
								>
									<Add class="h-6 w-6" /></button
								>
							</div>

							<div
								class="overflow-x-auto space-y-1"
								transition:slide|local={{ duration: 300, easing: cubicOut }}
							>
								<DragDropList
									id={property.name}
									type={VerticalDropZone}
									itemSize={60}
									itemCount={data[property.name].length}
									on:drop={(event) => {
										onDrop(property.name, event);
									}}
									
								>
									{#snippet children({ index })}
																<!-- svelte-ignore a11y_click_events_have_key_events -->
										<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2">
											<div class="mask mask-hexagon bg-primary h-auto w-10 shrink-0">
												<Router class="text-primary-content h-auto w-full scale-75" />
											</div>
											{#each property.n as propertyN}
												{#if propertyN.type != "password"}
													<div>
														<div class="font-bold">{data[property.name][index][propertyN.name]}</div>
													</div>
												{/if}
											{/each}
											{#if !page.data.features.security || $user.admin}
												<div class="flex-grow"></div>
												<div class="space-x-0 px-0 mx-0">
													<button
														class="btn btn-ghost btn-sm"
														onclick={() => {
															handleEdit(property.name, index);
														}}
													>
														<Edit class="h-6 w-6" /></button
													>
													<button
														class="btn btn-ghost btn-sm"
														onclick={() => {
															deleteItem(property.name, index);
														}}
													>
														<Delete class="text-error h-6 w-6" />
													</button>
												</div>
											{/if}
										</div>
									{/snippet}
								</DragDropList>
							</div>
							{#if showEditor && property.name == propertyEditable}
								<div class="divider my-0"></div>
								{#each property.n as propertyN}
									<div>
										<MultiInput property={propertyN} bind:value={dataEditable[propertyN.name]} onChange={inputChanged}></MultiInput>
									</div>
								{/each}
							{/if}
						{/if}
					{/each}
				</div>

				<div class="divider mb-2 mt-0"></div>
				<div class="mx-4 mb-4 flex flex-wrap justify-end gap-2">
					<button class="btn btn-primary" type="button" onclick={cancelState} disabled={!changed}
						>Cancel</button
					>
					<button class="btn btn-primary" type="button" onclick={validateAndSaveState} disabled={!changed}
						>Save</button
					>
				</div>
			{/await}
		</div>
	{/if}
</SettingsCard>
