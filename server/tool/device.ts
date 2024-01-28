import { join } from "path";
import { existsSync, mkdirSync } from "fs";

import type { Device_Info_full } from "@/interface/device";
import { WriteJSON, ReadJSON, dbPath } from "./fs";

const devicePath = join(dbPath, "device.json");

function AddDeviceFile(id: string) {
  let path = join(dbPath, "device", id);
  if (!existsSync(path)) mkdirSync(path);
  path = join(path, "data.json");
  if (!existsSync(path)) WriteJSON(path, []);
  path = join(dbPath, "device", id, "info.json");
  if (!existsSync(path)) WriteJSON(path, {});
}

function GetDeviceList(): Device_Info_full[] {
  return ReadJSON<Device_Info_full[]>(devicePath);
}

function GetDevice(device: {
  id?: string;
  mac?: string;
}): Device_Info_full | undefined {
  const devices = GetDeviceList();
  const { id, mac } = device;
  if (!id && !mac) return;
  if (mac) return devices.find((device) => device.mac === mac);
  return devices.find((device) => device.id === id);
}

function AddDevice(device: Device_Info_full) {
  const devices = GetDeviceList();
  devices.push(device);
  WriteJSON(devicePath, devices);
  AddDeviceFile(device.id);
}

function DeleteDevice(id: string) {
  const devices = GetDeviceList();
  const index = devices.findIndex((device) => device.id === id);
  if (index === -1) return;
  devices.splice(index, 1);
  WriteJSON(devicePath, devices);
}

function UpdateDevice(device: Device_Info_full) {
  const devices = GetDeviceList();
  const index = devices.findIndex((d) => d.id === device.id);
  if (index === -1) return;
  devices[index] = device;
  WriteJSON(devicePath, devices);
}

function IsDeviceExist(device: { id?: string; mac?: string }): boolean {
  const devices = GetDeviceList();
  const { id, mac } = device;
  if (!id && !mac) return false;
  if (mac && devices.findIndex((device) => device.mac === mac) !== -1)
    return true;
  return devices.findIndex((device) => device.id === id) !== -1;
}

export {
  GetDeviceList,
  GetDevice,
  AddDevice,
  DeleteDevice,
  UpdateDevice,
  IsDeviceExist,
};
