import type { PageLoad } from './$types';

export const load = (async () => {
	return {
		title: 'Module'
	};
}) satisfies PageLoad;
