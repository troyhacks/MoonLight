<!--
   @title     MoonBase
   @file      MultiInput.svelte
   @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
   @Authors   https://github.com/MoonModules/MoonLight/commits/main
   @Copyright © 2025 Github MoonLight Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.

   Not w-full!
   https://tabler.io/icons
-->

<script lang="ts">
	import { onDestroy } from 'svelte';
	import DraggableList from '$lib/components/DraggableList.svelte';
	import Add from '~icons/tabler/circle-plus';
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import { slide } from 'svelte/transition';
	import { cubicOut } from 'svelte/easing';
	import SearchIcon from '~icons/tabler/search';
	import Delete from '~icons/tabler/trash';
	import { initCap, getTimeAgo } from '$lib/stores/moonbase_utilities';

	import EditObject from './EditObject.svelte';
	import { modals } from 'svelte-modals';
	import Grip from '~icons/tabler/grip-vertical';

	let { property, data = $bindable(), definition, onChange, changeOnInput } = $props();

	let dataEditable: any = $state({});

	let propertyEditable: string = $state('');

	//if no records added yet, add an empty array
	if (data[property.name] == undefined) {
		data[property.name] = [];
	}

	let localDefinition: any = $state([]);

	// console.log("Array property", property, data, definition, changeOnInput, data[property.name], value1, value2);
	for (let i = 0; i < definition.length; i++) {
		// console.log("addItem def", propertyName, property)
		if (property.name == definition[i].name) {
			localDefinition = definition[i].n;
			// console.log("localDefinition", property.name, definition[i].n)
		}
	}

	//make getTimeAgo reactive
	let currentTime = $state(Date.now());
	// Update the dummy variable every second
	const interval = setInterval(() => {
		currentTime = Date.now();
	}, 1000);

	onDestroy(() => {
		clearInterval(interval);
	});

	function handleReorder(reorderedItems: any[]) {
		console.log('handleReorder', property.name, reorderedItems);
		data[property.name] = reorderedItems;
		onChange();
	}

	function addItem(propertyName: string) {
		propertyEditable = propertyName;
		//set the default values from the definition...
		dataEditable = {};

		//set properties with their defaults
		for (let i = 0; i < definition.length; i++) {
			let property = definition[i];
			if (property.name == propertyName) {
				console.log('addItem def', propertyName, property);
				for (let i = 0; i < property.n.length; i++) {
					let propertyN = property.n[i];
					// console.log("propertyN", propertyN)
					dataEditable[propertyN.name] = propertyN.default;
				}
			}
		}

		modals.open(EditObject as any, {
			property: property,
			localDefinition: localDefinition,
			title: 'Add ' + initCap(propertyName) + ' #' + (data[propertyName].length + 1),
			dataEditable: dataEditable, //bindable
			onChange,
			changeOnInput
		});
	}

	function handleEdit(propertyName: string, index: number) {
		console.log('handleEdit', propertyName, index, data[propertyName][index]);

		modals.open(EditObject as any, {
			property: property,
			localDefinition: localDefinition,
			title: initCap(propertyName) + ' #' + (index + 1),
			dataEditable: data[propertyName][index], // By reference (bindable)
			onChange,
			changeOnInput
		});
	}

	function deleteItem(propertyName: string, index: number) {
		// Remove item from array
		console.log(propertyName, index, data[propertyName]);
		data[propertyName].splice(index, 1);
		data[propertyName] = [...data[propertyName]]; //Trigger reactivity
		onChange();
	}
</script>

<div class="divider mb-2 mt-0"></div>
<div class="h-16 flex w-full items-center justify-between space-x-3 p-0 text-xl font-medium">
	{initCap(property.name)}
</div>
<div class="relative w-full overflow-visible">
	<!-- <div class="mx-4 mb-4 flex flex-wrap justify-end gap-2"> -->
	<button
		class="btn btn-primary text-primary-content btn-md absolute -top-14 right-0"
		onclick={() => {
			addItem(property.name);

			//add the new item to the data
			data[property.name].push(dataEditable);
			onChange();
		}}
	>
		<Add class="h-6 w-6" /></button
	>
</div>

<div class="overflow-x-auto space-y-1" transition:slide|local={{ duration: 300, easing: cubicOut }}>
	<DraggableList items={data[property.name]} onReorder={handleReorder} class="space-y-2">
		{#snippet children({ item: item, index }: { item: any; index: number })}
			<!-- svelte-ignore a11y_click_events_have_key_events -->
			<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2">
				<Grip class="h-6 w-6 text-base-content/30 cursor-grab flex-shrink-0" />
				<!-- Show the first 3 fields -->
				{#each property.n.slice(0, 3) as propertyN}
					{#if propertyN.type == 'time'}
						<div>
							<div class="font-bold">
								{getTimeAgo(item[propertyN.name], currentTime)}
							</div>
						</div>
					{:else if propertyN.type == 'coord3D'}
						<div>
							<div class="font-bold">
								({item[propertyN.name].x}, {item[propertyN.name].y}, {item[propertyN.name].z})
							</div>
						</div>
					{:else if propertyN.type != 'array' && propertyN.type != 'controls' && propertyN.type != 'password'}
						<div>
							<div class="font-bold">{item[propertyN.name]}</div>
						</div>
					{/if}
				{/each}
				{#each property.n as propertyN}
					{#if propertyN.type == 'array' || propertyN.type == 'controls'}
						<div>
							<div class="font-bold">
								↓{item[propertyN.name] ? item[propertyN.name].length : ''}
							</div>
						</div>
					{/if}
				{/each}
				{#if !page.data.features.security || $user.admin}
					<div class="grow"></div>
					<div class="space-x-0 px-0 mx-0">
						<button
							class="btn btn-ghost btn-sm"
							onclick={() => {
								handleEdit(property.name, index);
							}}
						>
							<SearchIcon class="h-6 w-6" /></button
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
	</DraggableList>
</div>
