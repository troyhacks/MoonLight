import type { PageLoad } from './$types';

export const load = (async () => {
	return {
		title: 'WiFi Access Point'
	};
}) satisfies PageLoad;
