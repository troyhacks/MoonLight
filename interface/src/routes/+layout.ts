import type { LayoutLoad } from './$types';

// This can be false if you're using a fallback (i.e. SPA mode)
export const prerender = false;
export const ssr = false;

export const load = (async ({ fetch }) => {
	const result = await fetch('/rest/features');
	const item = await result.json();
	const instances = await fetch('/rest/instances');
	const instancesJson = await instances.json();
	return {
		features: item,
		title: 'MoonLight',
		github: 'ewowi/MoonLight',
		copyright: '2025 MoonModules⚖️GPLv3',
		appName: 'MoonLight',
		instances: instancesJson
	};
}) satisfies LayoutLoad;
