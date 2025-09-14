<!--
   @title     MoonBase
   @file      MultiInput.svelte
   @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
   @Authors   https://github.com/MoonModules/MoonLight/commits/main
   @Copyright © 2025 Github MoonLight Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.

   Not w-full!
-->


<script lang="ts">
	import { onDestroy } from 'svelte';
	import FileEdit from '$lib/components/moonbase/FileEdit.svelte';
    import {initCap, getTimeAgo} from '$lib/stores/moonbase_utilities';

    export let property: any;
    export let value: any;
    export let onChange = (event:any) => {console.log(event)};
    export let hasNumber = true;
    export let disabled = false;
    export let step = 1;
    export let changeOnInput:boolean = true;

    //make getTimeAgo reactive
    export let currentTime = Date.now();

    let dragSource: { row: number, col: number } | null = null;

    function handleDragStart(event: DragEvent, row: number, col: number) {
        dragSource = { row, col };
        // Optionally, set drag data for external DnD
        event.dataTransfer?.setData('text/plain', `${row},${col}`);
    }

    function handleDrop(event: DragEvent, targetRow: number, targetCol: number) {
        if (dragSource && property.rows) {
            // Swap the cells
            const temp = property.rows[dragSource.row][dragSource.col];
            property.rows[dragSource.row][dragSource.col] = property.rows[targetRow][targetCol];
            property.rows[targetRow][targetCol] = temp;
            dragSource = null;
        }
    }
    
    let hoverTimeout: any = null;
    let fileContent: any;
    let popupCell: number | null = null;
    let popupX = 0;
    let popupY = 0;

    function handleMouseEnter(cell: number, event: MouseEvent, savedPreset: boolean) {
        hoverTimeout = setTimeout(async () => {
            if (!property.hoverToServer && savedPreset) {
                //open the file with cell
                const response = await fetch(`/rest/file/.config/presets/preset${cell.toString().padStart(2, '0')}.json`);
                if (response.ok) {
                    fileContent = await response.json();
                    console.log(fileContent);
                } else {
                    fileContent = "Failed to load file.";
                }
                popupCell = cell;
            }
            // Offset the popup a bit to the right and down
            popupX = event.clientX + 16;
            popupY = event.clientY + 16;

        }, 1000); 
        // 2 seconds
    }

    function handleMouseLeave() {
        clearTimeout(hoverTimeout);
        popupCell = null;
        fileContent = null;
    }

    //precent onClick when dblClick
    let clickTimeout: any = null;
    let preventClick = false;
</script>

<div>

<label class="label cursor-pointer" for={property.name}>
    <!-- <span class="text-md">{initCap(property.name)}</span> -->
    <span class="mr-4">{initCap(property.name)}</span>  
</label>

{#if property.ro}
    {#if property.type == "ip"}
        <a 
            href="http://{value}"
            target ="_blank"
        >{value}</a>
    {:else if property.type == "mdnsName"}
        <a 
            href="http://{value}.local"
            target ="_blank"
        >{value}</a>
    {:else if property.type == "coord3D"}
        <span>{value.x}, {value.y}, {value.z}</span> 
    {:else}
        <span>{value}</span> 
    {/if}
{:else}

{#if property.type == "select" || property.type == "selectFile"}
    <select 
        bind:value={value}
        on:change={onChange}
        class="select">
        <slot></slot>
        {#each property.values as value, index}
            <option value={property.type == "selectFile"?value:index}>
                {value}
            </option>
        {/each}
    </select>
    {#if property.type == "selectFile"}
        <FileEdit
            path = {value}
            showEditor = {false}
        />
    {/if}
{:else if property.type == "checkbox"}
    <input 
        type="checkbox"
        class="toggle toggle-primary"
        bind:checked={value}
        on:change={onChange}
    />
{:else if property.type == "range"}
    <div class="flex-row flex items-center space-x-2">
        <!-- range colors: https://daisyui.com/components/range/ 
         on:input: direct response to server
         -->
        <input 
            type="range"
            min={property.min?property.min:0} 
            max={property.max?property.max:255}
            step={step}
            class={"w-full range " + (disabled == false ? (property.color=="Red"?"range-error":(property.color=="Green"?"range-success":"range-primary")) : "range-secondary")}
            {disabled}
            title={property.default}
            bind:value={value}
            on:input={onChange}
        />
        {#if hasNumber}
            <input 
                type="number"
                min={property.min?property.min:0} 
                max={property.max?property.max:255}
                step={step}
                class="input"
                style="height: 2rem; width: 5rem"
                {disabled}
                bind:value={value}
                on:change={onChange}
            />
        {/if}
</div>
{:else if property.type == "textarea"}
    <textarea rows="10" cols="61"
        class="w-full textarea"
        on:change={onChange}
        on:input={(event:any) => {if (changeOnInput) onChange(event)}}
    >{value}</textarea>
{:else if property.type == "file"}
    <input 
        type="file"
        on:change={onChange}
    />
{:else if property.type == "number"}
    <input 
        type="number"
        style="width: {String(property.max || 255).length  + 4}ch"
        min={property.min?property.min:0}
        max={property.max?property.max:255}
        class='input invalid:border-error invalid:border-2'
        bind:value={value}
        on:change={onChange}
        on:input={(event:any) => {if (changeOnInput) onChange(event)}}
    />
{:else if property.type == "text"}
    <input 
        type={property.type}
        class="input invalid:border-error invalid:border-2"
        minlength={property.min?property.min:0}
        maxlength={property.max?property.max:255}
        bind:value={value}
        on:change={onChange}
        on:input={(event:any) => {if (changeOnInput) onChange(event)}}
    />
{:else if property.type == "time"}
    <span>{value} {getTimeAgo(value, currentTime)}</span> 
{:else if property.type == "ip"}
    <input 
        type={property.type}
        class="input invalid:border-error invalid:border-2"
        minlength=3
        maxlength=15
        bind:value={value}
        on:change={onChange}
        on:input={(event:any) => {if (changeOnInput) onChange(event)}}
    />
    <a 
        href="http://{value}"
        target ="_blank"
    >Link</a>
{:else if property.type == "button"}
    <button class="btn btn-primary" type="button" on:click={(event:any) => {if (value==null) value = 1; else value++; onChange(event)}}
    >{property.name}</button
    >
{:else if property.type == "coord3D"}
    <input 
        type=number
        style="width: {String(property.max || 255).length * 2}ch"
        class="input invalid:border-error invalid:border-2"
        min=0
        max=2047
        bind:value={value.x}
        on:change={onChange}
    />
    <input 
        type=number
        style="width: {String(property.max || 255).length * 2}ch"
        class="input invalid:border-error invalid:border-2"
        min=0
        max=255
        bind:value={value.y}
        on:change={onChange}
    />
    <input 
        type=number
        style="width: {String(property.max || 255).length * 2}ch"
        class="input invalid:border-error invalid:border-2"
        min=0
        max=31
        bind:value={value.z}
        on:change={onChange}
    />
{:else if property.type == "pad"}
    <div class="flex flex-col space-y-2">
        {#each Array(Math.ceil((value.count||64) / (property.width||8))) as _, y}
            <div class="flex flex-row space-x-2">
                {#each Array(property.width) as _, x}
                    {#if ((x + y * property.width) < value.count)}
                        <button
                            class="btn btn-square w-{property.size} h-{property.size} text-xl rounded-lg {
                                value.selected == x + y * property.width?`btn-error`:Array.isArray(value.list) && value.list.includes(x + y * property.width) ? `btn-success` : 'btn-primary'}"
                            type="button"
                            draggable="true"
                            on:dragstart={(event) => handleDragStart(event, y, x)}
                            on:dragover|preventDefault
                            on:drop={(event) => handleDrop(event, y, x)}
                            on:click={(event:any) => {
                                preventClick = false;
                                clickTimeout = setTimeout(() => {
                                    if (!preventClick) {
                                        value.select = x + y * property.width;
                                        console.log("click", y, x, value.select);
                                        value.selected = value.select;
                                        value.action = "click";
                                        onChange(event);
                                    }
                                }, 250); 
                                // 250ms is a typical double-click threshold
                            }}
                            on:dblclick={(event:any) => {
                                preventClick = true;
                                clearTimeout(clickTimeout);
                                value.select = x + y * property.width;
                                console.log("dblclick", y, x, value.select);
                                value.action = "dblclick";
                                onChange(event)
                            }}
                            on:mouseenter={(event:any) => {
                                // console.log("mousenter", rowIndex, colIndex, cell, value);
                                if (property.hoverToServer) {
                                    value.select = x + y * property.width;
                                    value.action = "mouseenter";
                                    onChange(event)
                                } else
                                    handleMouseEnter(x + y * property.width, event, value.list.includes(x + y * property.width))
                            }}
                            on:mouseleave={(event:any) => {
                                // console.log("mouseleave", rowIndex, colIndex, cell, value);
                                if (property.hoverToServer) {
                                    value.select = x + y * property.width;
                                    value.action = "mouseleave";
                                    onChange(event)
                                } else
                                    handleMouseLeave()
                            }}
                        >
                            {x + y * property.width}
                            {#if popupCell === x + y * property.width}
                                <div class="fixed z-50 bg-base-200 p-6 rounded shadow-lg mt-2 min-h-0 text-left inline-block min-w-0"
                                    style="left: {popupX}px; top: {popupY}px;"
                                >
                                    <!-- Popup for {cell} -->
                                    {#if fileContent && fileContent.nodes}
                                        {#each fileContent.nodes as node}
                                            {console.log("node.name", node.name)}
                                            <p>{node.name} {node.on?"on":"off"}</p>
                                        {/each}
                                    {/if}
                                </div>
                            {/if}
                        </button>
                    {/if}
                {/each}
            </div>
        {/each}
    </div>
    <!-- btn btn-square btn-primary gives you square, colored buttons (DaisyUI).
flex flex-col and flex flex-row create the grid layout (TailwindCSS).
Adjust space-x-2 and space-y-2 for spacing. -->

{:else}
    <input 
        type={property.type}
        class="input invalid:border-error invalid:border-2"
        bind:value={value}
        on:change={onChange}
    />
{/if}
{/if}

</div>