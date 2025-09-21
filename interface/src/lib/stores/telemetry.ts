import { writable } from 'svelte/store';
import type { RSSI } from '../types/models';
import type { Battery } from '../types/models';
import type { DownloadOTA } from '../types/models';

let telemetry_data = {
	rssi: {
		rssi: 0,
		ssid: '',
		disconnected: true,
		restartNeeded: false, // 🌙 restartNeeded Indicates if the system needs to be restarted
		safeMode: false, // 🌙 safeMode Indicates if the system is in safe mode
		saveNeeded: false, // 🌙 saveNeeded Indicates that changes has been made which need to be saved (or canceled)
		hostName: '' // 🌙 to show in title and statusbar
	},
	battery: {
		soc: -1,
		charging: false,
		voltage: -1, // 🌙
		current: -1, // 🌙
	},
	download_ota: {
		status: 'none',
		progress: 0,
		error: ''
	}
};

function createTelemetry() {
	const { subscribe, set, update } = writable(telemetry_data);

	return {
		subscribe,
		setRSSI: (data: RSSI) => {
			if (!isNaN(Number(data.rssi))) {
				update((telemetry_data) => ({
					...telemetry_data,
					rssi: { rssi: Number(data.rssi), ssid: data.ssid, disconnected: false, safeMode: data.safeMode, restartNeeded: data.restartNeeded, saveNeeded: data.saveNeeded, hostName: data.hostName } // 🌙 variables added
				}));
			} else {
				update((telemetry_data) => ({
					...telemetry_data,
					rssi: { rssi: 0, ssid: data.ssid, disconnected: true, safeMode: data.safeMode, restartNeeded: data.restartNeeded, saveNeeded: data.saveNeeded, hostName: data.hostName } // 🌙  variables added
				}));
			}
			if (data.hostName != '')
				localStorage.setItem('telemetry.rssi.hostName', data.hostName);
		},
		setBattery: (data: Battery) => {
			update((telemetry_data) => ({
				...telemetry_data,
				battery: { soc: data.soc, charging: data.charging, voltage: data.voltage, current: data.current }
			}));
		},
		setDownloadOTA: (data: DownloadOTA) => {
			update((telemetry_data) => ({
				...telemetry_data,
				download_ota: { status: data.status, progress: data.progress, error: data.error }
			}));
		}
	};
}

export const telemetry = createTelemetry();
