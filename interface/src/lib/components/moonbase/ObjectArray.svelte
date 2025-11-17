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
	import { initCap } from '$lib/stores/moonbase_utilities';

	import EditObject from './EditObject.svelte';
	import { modals } from 'svelte-modals';
	import Grip from '~icons/tabler/grip-vertical';
	import MultiInput from './MultiInput.svelte';
	import { isNumber } from 'chart.js/helpers';

	let { property, data = $bindable(), definition, onChange, changeOnInput } = $props();

	let dataEditable: any = $state({});

	// let propertyEditable: string = $state('');

	let searchQuery: string = $state('');
	searchQuery = "!none";

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
		data[property.name] = reorderedItems.map((wrapper: any) => wrapper.item);
		onChange();
	}

	function addItem(propertyName: string) {
		// propertyEditable = propertyName;
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

	// Filter items based on search query - returns array of {item, originalIndex}
	let filteredItems = $derived.by(() => {
		if (!searchQuery.trim()) {
			return data[property.name].map((item: any, index: number) => ({
				item,
				originalIndex: index
			}));
		}

		const isNegated = searchQuery.startsWith('!');
		const query = (isNegated ? searchQuery.slice(1) : searchQuery).toLowerCase().trim();

		if (!query) {
			return data[property.name].map((item: any, index: number) => ({
				item,
				originalIndex: index
			}));
		}

		const filtered = data[property.name]
			.map((item: any, index: number) => ({ item, originalIndex: index }))
			.filter(({ item }: { item: any }) => {
				// Search through the first 3 visible fields
				const matchFound = property.n.slice(0, 3).some((propertyN: any) => {
					let valueStr;

					// Check dropdown - only check the SELECTED option's label, not all options
					if (
						propertyN.values &&
						Array.isArray(propertyN.values) &&
						isNumber(item[propertyN.name])
					) {
						valueStr = propertyN.values[item[propertyN.name]];
					} else {
						valueStr = item[propertyN.name];
					}

					return String(valueStr).toLowerCase().includes(query);
				});

				// If negated (!), return items that DON'T match
				return isNegated ? !matchFound : matchFound;
			});

		return filtered;
	});
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

<!-- Search Filter -->
{#if data[property.name].length > 0}
	<div class="mb-3">
		<div class="relative">
			<SearchIcon class="absolute left-3 top-1/2 -translate-y-1/2 h-5 w-5 text-base-content/50" />
			<input
				type="text"
				bind:value={searchQuery}
				placeholder="Search... (prefix with ! to exclude)"
				class="input input-bordered w-full input-sm"
			/>
			{#if searchQuery}
				<button
					class="btn btn-ghost btn-sm absolute right-1 top-1/2 -translate-y-1/2"
					onclick={() => (searchQuery = '')}
				>
					✕
				</button>
			{/if}
		</div>
		{#if searchQuery.trim()}
			<div class="text-sm text-base-content/60 mt-1 ml-1">
				{filteredItems.length} of {data[property.name].length} items
				{searchQuery.startsWith('!') ? '(excluding matches)' : ''}
			</div>
		{/if}
	</div>
{/if}

<div class="overflow-x-auto space-y-1" transition:slide|local={{ duration: 300, easing: cubicOut }}>
	<DraggableList items={filteredItems} onReorder={handleReorder} class="space-y-2">
		{#snippet children({ item: itemWrapper, index }: { item: any; index: number })}
			<!-- svelte-ignore a11y_click_events_have_key_events -->
			<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2">
				<Grip class="h-6 w-6 text-base-content/30 cursor-grab flex-shrink-0" />
				<!-- Show the first 3 fields -->
				{#each property.n.slice(0, 3) as propertyN}
					{#if propertyN.type != 'array' && propertyN.type != 'controls' && propertyN.type != 'password'}
						<MultiInput
							property={propertyN}
							bind:value={itemWrapper.item[propertyN.name]}
							noPrompts={true}
							onChange={(event) => {
								onChange(event);
							}}
						></MultiInput>
					{/if}
				{/each}
				<!-- Show nr of controls -->
				{#each property.n as propertyN}
					{#if propertyN.type == 'array' || propertyN.type == 'controls'}
						<div>
							<div class="font-bold">
								↓{itemWrapper.item[propertyN.name] ? itemWrapper.item[propertyN.name].length : ''}
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
								handleEdit(property.name, itemWrapper.originalIndex);
							}}
						>
							<SearchIcon class="h-6 w-6" /></button
						>
						<button
							class="btn btn-ghost btn-sm"
							onclick={() => {
								deleteItem(property.name, itemWrapper.originalIndex);
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
