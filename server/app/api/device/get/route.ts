import { Device_Info } from "@/interface/device";
import { GetDevice } from "@/tool/device";
import { getQuery } from "@/util";

async function GET(request: Request) {
  const { id } = getQuery(request);
  if (!id) return new Response("bad request", { status: 400 });
  const device = GetDevice({ id });
  if (!device) return new Response("not found", { status: 404 });
  const data: Device_Info = {
    id: device.id,
    name: device.name,
  };
  return new Response(JSON.stringify(data), { status: 200 });
}

export { GET };
