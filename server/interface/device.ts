type Device_Info = {
  id: string;
  name?: string;
};
type Device_Info_full = Device_Info & {
  mac: string;
};

export type { Device_Info, Device_Info_full };
