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
	import ObjectArray from '$lib/components/custom/ObjectArray.svelte';

    let { property, data, definition, showEditor = false, onChange, changeOnInput, value1=$bindable(), value2 } = $props();

    let dataEditable: any = $state({});

    let propertyEditable: string = $state("");

    console.log("ArrayLight property", property, "data", data, "definition", definition, "value1", value1, "value2", value2, showEditor, changeOnInput, data[property.name]);
    // for (let i = 0; i < definition.length; i++) {
    //     // console.log("addItem def", propertyName, property)
    //     if (property.name == definition[i].name) {
    //         console.log("def[i]", property.name, definition[i].n)
    //     }
    // }

    </script>
