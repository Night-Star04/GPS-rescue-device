import { WriteJSON, ReadJSON, dbPath } from "./fs";
import { IsDeviceExist } from "./device";
import { join } from "path";
import type { GPS_Data } from "@/interface/data";

function AddData(id: string, data: GPS_Data) {
  if (!IsDeviceExist({ id })) return;
  const path = join(dbPath, "device", id, "data.json");
  const datas = ReadJSON<GPS_Data[]>(path);
  datas.push(data);
  WriteJSON(path, datas);
}

function GetData(id: string): GPS_Data[] {
  if (!IsDeviceExist({ id })) return [];
  const path = join(dbPath, "device", id, "data.json");
  return ReadJSON<GPS_Data[]>(path);
}

function EditNowData(id: string, data: GPS_Data) {
  if (!IsDeviceExist({ id })) return;
  const path = join(dbPath, "device", id, "info.json");
  WriteJSON(path, data);
}

function GetNowData(id: string): GPS_Data | undefined {
  if (!IsDeviceExist({ id })) return;
  const path = join(dbPath, "device", id, "info.json");
  const datas = ReadJSON<GPS_Data>(path);
  return datas;
}

export { AddData, GetData, EditNowData, GetNowData };
