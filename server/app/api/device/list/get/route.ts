import { Device_Info } from "@/interface/device";
import { GetDeviceList } from "@/tool/device";

function GET(_request: Request) {
  const deviceList = GetDeviceList();
  const data: Array<Device_Info> = deviceList.map((device) => {
    const { id, name } = device;
    return { id, name };
  });

  return new Response(JSON.stringify(data), { status: 200 });
}

export { GET };
