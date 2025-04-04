<!--
   @title     MoonBase
   @file      MultiInput.svelte
   @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
   @Authors   https://github.com/ewowi/MoonBase/commits/main
   @Copyright © 2025 Github MoonBase Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com

   Not w-full!
-->


<script lang="ts">
	import FileEdit from '$lib/components/custom/FileEdit.svelte';
	import DragDropList, { VerticalDropZone, reorder, type DropEvent } from 'svelte-dnd-list';
	import Add from '~icons/tabler/circle-plus';
	import { user } from '$lib/stores/user';
	import { page } from '$app/state';
	import Router from '~icons/tabler/router';
	import { slide } from 'svelte/transition';
	import { cubicOut } from 'svelte/easing';
	import Edit from '~icons/tabler/pencil';
	import Delete from '~icons/tabler/trash';
	import MultiInput from '$lib/components/custom/MultiInput.svelte';
	import Array from '$lib/components/custom/Array.svelte';
	import ArrayLight from '$lib/components/custom/ArrayLight.svelte';

    let { property, data, definition, onChange, changeOnInput, value1=$bindable(), value2 } = $props();

    let dataEditable: any = $state({});

    let propertyEditable: string = $state("");

    let showEditor: boolean = $state(false);

    //if no records added yet, add an empty array
    if (data[property.name] == undefined) {
        data[property.name] = [];
    }

    let lastIndex: number = -1;

    let localDefinition: any = $state([]);

    // console.log("Array property", property, data, definition, changeOnInput, data[property.name], value1, value2);
    for (let i = 0; i < definition.length; i++) {
        // console.log("addItem def", propertyName, property)
        if (property.name == definition[i].name) {
            localDefinition = definition[i].n;
            console.log("localDefinition", property.name, definition[i].n)
        }
    }

	function onDrop(propertyName: string, { detail: { from, to } }: CustomEvent<DropEvent>) {
		
		if (!to || from === to) {
			return;
		}

		data[propertyName] = reorder(data[propertyName], from.index, to.index);
		onChange();
		// console.log(onDrop, data[propertyName]);
	}

    function addItem(propertyName: string) {
		propertyEditable = propertyName;
		//set the default values from the definition...
		dataEditable = {};

		//set properties with their defaults
		for (let i = 0; i < localDefinition.length; i++) {
			let property = localDefinition[i];
			if (property.name == propertyName) {
                console.log("addItem def", propertyName, property)
				for (let i=0; i < property.n.length; i++) {
					let propertyN = property.n[i];
					// console.log("propertyN", propertyN)
					dataEditable[propertyN.name] = propertyN.default;
				}
			}
		}
	}

	function handleEdit(propertyName: string, index: number) {
		console.log("handleEdit", propertyName, index, data[propertyName][index])
        if (lastIndex != index) {
            showEditor = true;
            lastIndex = index;
        } else
            showEditor = !showEditor;
        propertyEditable = propertyName;
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
		onChange();
	}

    function nrofDetails(index: number, propertyN: any) {
        if (data[property.name][index][propertyN.name])
            return data[property.name][index][propertyN.name].length
        else return 0;
    }

</script>

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
                onChange();

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
                        {#if propertyN.type == "array"}
                            <div>
                                <div class="font-bold">↓{nrofDetails(index, propertyN)}</div>
                            </div>
                        {:else if propertyN.type != "password"}
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
            {#if propertyN.type != "array"}
                <div>
                    <MultiInput property={propertyN} bind:value={dataEditable[propertyN.name]} onChange={onChange} changeOnInput={changeOnInput}></MultiInput>
                </div>
            {:else if propertyN.type == "array"}
                <label for="{propertyN.name}">{propertyN.name}</label>
                <Array property={propertyN} bind:value1={data[propertyN.name]} value2={data} data={dataEditable} definition={localDefinition} onChange={onChange} changeOnInput={changeOnInput}></Array>
            {/if}
        {/each}
    {/if}
