<script lang="ts">
	import type { PageData } from './$types';
	import FileManager from './FileManager.svelte';

	interface Props {
		data: PageData;
	}

	let { data }: Props = $props();
</script>

<div class="mx-0 my-1 flex flex-col space-y-4">
	<!-- 🌙 remove sm:mx-8 sm:my-8 so module card closer to monitor card -->
	<FileManager />
</div>
