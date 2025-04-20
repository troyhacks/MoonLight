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
	import { onDestroy } from 'svelte';
	import FileEdit from '$lib/components/custom/FileEdit.svelte';
    import {initCap, getTimeAgo} from '$lib/stores/custom_utilities';

    export let property: any;
    export let value: any;
    export let onChange = (event:any) => {console.log(event)};
    export let hasNumber = true;
    export let disabled = false;
    export let step = 1;
    export let changeOnInput:boolean = true;

    //make getTimeAgo reactive
    export let currentTime = Date.now();
    // Update the dummy variable every second
    const interval = setInterval(() => {
        currentTime = Date.now();
    }, 1000);
    onDestroy(() => {
        clearInterval(interval);
    });

</script>

<label class="label cursor-pointer" for={property.name}>
    <!-- <span class="label-text text-md">{initCap(property.name)}</span> -->
    <span class="mr-4">{initCap(property.name)}</span>
</label>

{#if property.ro && false}
    {#if property.type == "ip"}
        <a 
            href="http://{value}"
            target ="_blank"
        >{value}</a>
    {:else}
        <span>{value}</span> 
    {/if}
{:else}

{#if property.type == "select" || property.type == "selectFile"}
    <select 
        bind:value={value}
        on:change={onChange}
        class="select select-bordered">
        <slot></slot>
        {#each property.values as value}
            <option value={value}>
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
        <!-- range colors: https://daisyui.com/components/range/ -->
        <input 
            type="range"
            min={property.min} 
            max={property.max}
            step={step}
            class={"range " + (disabled == false ? (property.color=="Red"?"range-error":(property.color=="Green"?"range-success":"range-primary")) : "range-secondary")}
            {disabled}
            bind:value={value}
            on:change={onChange}
        />
        {#if hasNumber}
            <input 
                type="number"
                min={property.min} 
                max={property.max}
                step={step}
                class="input input-bordered"
                style="height: 2rem; width: 5rem"
                {disabled}
                bind:value={value}
                on:change={onChange}
            />
        {/if}
</div>
{:else if property.type == "textarea"}
    <textarea rows="10" cols="61"
        class="textarea textarea-bordered"
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
        min={property.min} 
        max={property.max}
        class="input input-bordered invalid:border-error invalid:border-2"
        bind:value={value}
        on:change={onChange}
        on:input={(event:any) => {if (changeOnInput) onChange(event)}}
    />
{:else if property.type == "text"}
    <input 
        type={property.type}
        class="input input-bordered invalid:border-error invalid:border-2"
        minlength={property.min}
        maxlength={property.max}
        bind:value={value}
        on:change={onChange}
        on:input={(event:any) => {if (changeOnInput) onChange(event)}}
    />
{:else if property.type == "time"}
    <span>{value} {getTimeAgo(value, currentTime)}</span> 
{:else if property.type == "ip"}
    <input 
        type={property.type}
        class="input input-bordered invalid:border-error invalid:border-2"
        minlength={property.min}
        maxlength={property.max}
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
{:else}
    <input 
        type={property.type}
        class="input input-bordered invalid:border-error invalid:border-2"
        bind:value={value}
        on:change={onChange}
    />
{/if}
{/if}