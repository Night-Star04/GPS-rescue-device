import { AddDevice, GetDevice } from "@/tool/device";
import { UUID } from "@/util";

async function POST(request: Request) {
  if (request.headers.get("content-type") !== "application/json") {
    return new Response("bad request", { status: 400 });
  }
  const { id } = await request.json();
  if (!id) return new Response("bad request", { status: 400 });
  const device = GetDevice({ mac: id });
  console.log(device);
  if (device) return new Response(device.id, { status: 200 });
  const uuid = UUID(8);
  AddDevice({ id: uuid, mac: id });
  return new Response(uuid, { status: 200 });
}

export { POST };
