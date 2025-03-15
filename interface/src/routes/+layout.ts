import type { LayoutLoad } from './$types';

// This can be false if you're using a fallback (i.e. SPA mode)
export const prerender = false;
export const ssr = false;

export const load = (async ({ fetch }) => {
	const result = await fetch('/rest/features');
	const item = await result.json();
	return {
		features: item,
		title: 'ESP32-Svelte-Live-MM',
		github: 'ewowi/ESP32-svelte-live-mm',
		copyright: '2025 theelims, ewowi, hpwit ⚖️ GPL-3.0',
		appName: 'ESP32 Svelte Live MM'
	};
}) satisfies LayoutLoad;
