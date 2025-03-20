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

	let showDetails = $state(false);

	let itemEditable: any = $state({});

	let newItem: boolean = $state(true);
	let showEditor: boolean = $state(false);

	let datadefinition: any = $state([]);
	let itemState: any = $state({});

	let itemList: any[] = $state([]);

	async function getState() {

		//load definition
		try {
			const response = await fetch('/rest/moduleDD', {
				method: 'GET',
				headers: {
					Authorization: page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			datadefinition = await response.json();
			// console.log("datadefinition", datadefinition, datadefinition.nodes)
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
		itemList = itemState.nodes;
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
		let valid = true;

		// Submit JSON to REST API
		if (valid) {
			if (newItem) {
				// console.log(itemList, itemEditable.animation, itemEditable.size)
				itemList.push(itemEditable);
			} else {
				itemList.splice(itemList.indexOf(itemEditable), 1, itemEditable);
			}
			addItem();
			itemList = [...itemList]; //Trigger reactivity
			showEditor = false;
		}
	}

	function addItem() {
		newItem = true;
		itemEditable = {
			animation: '',
			size: 0,
		};
	}

	function handleEdit(index: number) {
		newItem = false;
		showEditor = true;
		itemEditable = itemList[index];
	}

	function confirmDelete(index: number) {
		modals.open(ConfirmDialog, {
			title: 'Delete Item',
			message: 'Are you sure you want to delete this item?',
			labels: {
				cancel: { label: 'Cancel', icon: Cancel },
				confirm: { label: 'Delete', icon: Delete }
			},
			onConfirm: () => {
				// Check if item is currently been edited and delete as well
				if (itemList[index].animation === itemEditable.animation) {
					addItem();
				}
				// Remove item from array
				itemList.splice(index, 1);
				itemList = [...itemList]; //Trigger reactivity
				showEditor = false;
				modals.close();
			}
		});
	}

	function onDrop({ detail: { from, to } }: CustomEvent<DropEvent>) {
		if (!to || from === to) {
			return;
		}

		itemList = reorder(itemList, from.index, to.index);
		console.log(onDrop, itemList);
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
		<span >Nodes</span>
	{/snippet}

	{#if !page.data.features.security || $user.admin}
		<div class="bg-base-200 shadow-lg relative grid w-full max-w-2xl self-center overflow-hidden">
			<div class="h-16 flex w-full items-center justify-between space-x-3 p-0 text-xl font-medium">
				Nodes
			</div>
			{#await getState()}
				<Spinner />
			{:then nothing}
				<div class="relative w-full overflow-visible">
					<button
						class="btn btn-primary text-primary-content btn-md absolute -top-14 right-16"
						onclick={() => {
							addItem();
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
							itemCount={itemList.length}
							on:drop={onDrop}
							
						>
							{#snippet children({ index })}
														<!-- svelte-ignore a11y_click_events_have_key_events -->
								<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2">
									<div class="mask mask-hexagon bg-primary h-auto w-10 shrink-0">
										<Router class="text-primary-content h-auto w-full scale-75" />
									</div>
									{#each datadefinition as object}
										{#if object.type == "array"}
											{#each object.n as objectN}
												<div>
													<div class="font-bold">{itemList[index][objectN.name]}</div>
												</div>
											{/each}
										{/if}
									{/each}
									{#if !page.data.features.security || $user.admin}
										<div class="flex-grow"></div>
										<div class="space-x-0 px-0 mx-0">
											<button
												class="btn btn-ghost btn-sm"
												onclick={() => {
													handleEdit(index);
												}}
											>
												<Edit class="h-6 w-6" /></button
											>
											<button
												class="btn btn-ghost btn-sm"
												onclick={() => {
													confirmDelete(index);
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
							{#each datadefinition as object}
								{#if object.type != "array"}
									<div>
										<label class="label" for={object.name}>
											<span class="label-text text-md">{object.name}</span>
										</label>
										<input
											type="text"
											class="input input-bordered invalid:border-error w-full invalid:border-2"
											bind:value={itemState[object.name]}
											id={object.name}
										/>
									</div>
								{:else}
									{#if showEditor}
										<div class="divider my-0"></div>
										{#each object.n as objectN}
											<div>
												<label class="label" for={objectN.name}>
													<span class="label-text text-md">{objectN.name}</span>
												</label>
												{#if objectN.type != "select"}
													<input
														type="text"
														class="input input-bordered invalid:border-error w-full invalid:border-2}"
														bind:value={itemEditable[objectN.name]}
														id={objectN.name}
													/>
												{:else}
													<select
														class="select select-bordered w-full"
														id="animation"
														bind:value={itemEditable.animation}
													>
														{#each objectN.values as value}
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
