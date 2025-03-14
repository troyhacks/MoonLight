export type FilesState = {
	name: string;
	path: string;
	isFile: boolean;
	size: number;
	time: number;
	contents: string;
	files: FilesState[];
	fs_total: number;
	fs_used: number;
};

export type LiveAnimationState = {
	name: string;
	lightsOn:boolean;
	brightness: number;
	animation: number;
	animations: string[];
	driverOn:boolean;
};
