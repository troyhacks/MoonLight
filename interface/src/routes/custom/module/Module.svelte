<script lang="ts">
	import { modals } from 'svelte-modals';
	import { slide } from 'svelte/transition';
	import { cubicOut } from 'svelte/easing';
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import { notifications } from '$lib/components/toasts/notifications';
	import DragDropList, { VerticalDropZone, reorder, type DropEvent } from 'svelte-dnd-list';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import ConfirmDialog from '$lib/components/ConfirmDialog.svelte';
	import Spinner from '$lib/components/Spinner.svelte';
	import Router from '~icons/tabler/router';
	import Add from '~icons/tabler/circle-plus';
	import Edit from '~icons/tabler/pencil';
	import Delete from '~icons/tabler/trash';
	import Cancel from '~icons/tabler/x';

	let itemEditable: any = $state({});

	let newItem: boolean = $state(true);
	let showEditor: boolean = $state(false);

	let definition: any = $state([]);
	let itemState: any = $state({});

	async function getState() {

		//load definition
		try {
			const response = await fetch('/rest/moduleDef', {
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

		//load data
		try {
			const response = await fetch('/rest/module', {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			itemState = await response.json();
		} catch (error) {
			console.error('Error:', error);
		}
		return itemState;
	}

	async function postState(data: any) {
		try {
			const response = await fetch('/rest/module', {
				method: 'POST',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(data)
			});
			if (response.status == 200) {
				notifications.success('Module settings updated.', 3000);
				itemState = await response.json();
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}

	function validateState() {
		postState(itemState);
	}

	function validateForm() {
		const propertyName = "nodes"; // to do: make parameter

		let valid = true;

		// Submit JSON to REST API
		if (valid) {
			if (newItem) {
				itemState[propertyName].push(itemEditable);
			} else {
				itemState[propertyName].splice(itemState[propertyName].indexOf(itemEditable), 1, itemEditable);
			}
			addItem(propertyName);
			itemState[propertyName] = [...itemState[propertyName]]; //Trigger reactivity
			showEditor = false;
		}
	}

	function addItem(propertyName: string) {
		newItem = true;
		//set the default values from the definition...
		itemEditable = {};

		//set properties with their defaults
		for (let i = 0; i < definition.length; i++) {
			let property = definition[i];
			console.log("addItem def", propertyName, property)
			if (property.name == propertyName) {
				for (let i=0; i < property.n.length; i++) {
					let propertyN = property.n[i];
					console.log("propertyN", propertyN)
					itemEditable[propertyN.name] = propertyN.default;
				}
			}
		}
	}

	function handleEdit(propertyName: string, index: number) {
		newItem = false;
		showEditor = true;
		itemEditable = itemState[propertyName][index];
	}

	function confirmDelete(propertyName: string, index: number) {
		modals.open(ConfirmDialog, {
			title: 'Delete Item',
			message: 'Are you sure you want to delete this item?',
			labels: {
				cancel: { label: 'Cancel', icon: Cancel },
				confirm: { label: 'Delete', icon: Delete }
			},
			onConfirm: () => {
				// Check if item is currently been edited and delete as well
				if (itemState[propertyName][index].animation === itemEditable.animation) {
					addItem(propertyName);
				}
				// Remove item from array
				itemState[propertyName].splice(index, 1);
				itemState[propertyName] = [...itemState[propertyName]]; //Trigger reactivity
				showEditor = false;
				modals.close();
			}
		});
	}

	function onDrop({ detail: { from, to } }: CustomEvent<DropEvent>) {
		const propertyName = "nodes"; // to do: make parameter
		
		if (!to || from === to) {
			return;
		}

		itemState[propertyName] = reorder(itemState[propertyName], from.index, to.index);
		console.log(onDrop, itemState[propertyName]);
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
		<span >Module</span>
	{/snippet}

	{#if !page.data.features.security || $user.admin}
		<div class="bg-base-200 shadow-lg relative grid w-full max-w-2xl self-center overflow-hidden">
			{#await getState()}
				<Spinner />
			{:then nothing}
				<div class="relative w-full overflow-visible">
					{#each definition as property}
						{#if property.type == "array" && property.name == "nodes"}
							<div class="h-16 flex w-full items-center justify-between space-x-3 p-0 text-xl font-medium">
								{property.name}
							</div>
							<button
								class="btn btn-primary text-primary-content "
								onclick={() => {
									addItem(property.name);
									showEditor = true;
								}}
							>
								<Add class="h-6 w-6" /></button
							>

							<div
								class="overflow-x-auto space-y-1"
								transition:slide|local={{ duration: 300, easing: cubicOut }}
							>
								<DragDropList
									id="items"
									type={VerticalDropZone}
									itemSize={60}
									itemCount={itemState[property.name].length}
									on:drop={onDrop}
									
								>
									{#snippet children({ index })}
																<!-- svelte-ignore a11y_click_events_have_key_events -->
										<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2">
											<div class="mask mask-hexagon bg-primary h-auto w-10 shrink-0">
												<Router class="text-primary-content h-auto w-full scale-75" />
											</div>
											{#each property.n as propertyN}
												<div>
													<div class="font-bold">{itemState[property.name][index][propertyN.name]}</div>
												</div>
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
															confirmDelete(property.name, index);
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
						{/if}
					{/each}
				</div>

				<div class="divider mb-0"></div>
				<div
					class="flex flex-col gap-2 p-0"
					transition:slide|local={{ duration: 300, easing: cubicOut }}
				>
					<form
						class=""
						onsubmit={preventDefault(validateForm)}
						novalidate
					>

						<div class="grid w-full grid-cols-1 content-center gap-x-4 px-4 sm:grid-cols-2">
							{#each definition as property}
								{#if property.type != "array"}
									<div>
										<label class="label" for={property.name}>
											<span class="label-text text-md">{property.name}</span>
										</label>
										<input
											type="text"
											class="input input-bordered invalid:border-error w-full invalid:border-2"
											bind:value={itemState[property.name]}
											id={property.name}
										/>
									</div>
								{:else if property.name == "nodes"}
									{#if showEditor}
										<div class="divider my-0"></div>
										{#each property.n as propertyN}
											<div>
												<label class="label" for={propertyN.name}>
													<span class="label-text text-md">{propertyN.name}</span>
												</label>
												{#if propertyN.type != "select"}
													<input
														type="text"
														class="input input-bordered invalid:border-error w-full invalid:border-2}"
														bind:value={itemEditable[propertyN.name]}
														id={propertyN.name}
													/>
												{:else}
													<select
														class="select select-bordered w-full"
														id="animation"
														bind:value={itemEditable[propertyN.name]}
													>
														{#each propertyN.values as value}
															<option value={value}>
																{value}
															</option>
														{/each}
													</select>
												{/if}
											</div>
										{/each}
									{/if}
								{/if}
							{/each}
						</div>


						<div class="divider mb-2 mt-0"></div>
						<div class="mx-4 mb-4 flex flex-wrap justify-end gap-2">
							<button class="btn btn-primary" type="submit" disabled={!showEditor}
								>{newItem ? 'Add Node' : 'Update Node'}</button
							>
							<button class="btn btn-primary" type="button" onclick={validateState}
								>Apply Settings</button
							>
						</div>
					</form>
				</div>
			{/await}
		</div>
	{/if}
</SettingsCard>
