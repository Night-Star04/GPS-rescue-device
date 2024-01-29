import { GetData, GetNowData } from "@/tool/data";
import { getQuery } from "@/util";

function GET_list(id: string, limit: number = 100) {
  const data = GetData(id);
  if (!data) return new Response("not found", { status: 404 });
  data.sort((a, b) => b.time - a.time); // sort by time
  data.splice(limit); // limit
  return new Response(JSON.stringify(data), { status: 200 });
}

function GET_now(id: string) {
  const data = GetNowData(id);
  if (!data) return new Response("not found", { status: 404 });
  return new Response(JSON.stringify(data), { status: 200 });
}

function GET(request: Request) {
  const { id, mod, limit } = getQuery(request);
  if (!id || !mod) return new Response("bad request", { status: 400 });
  if (limit && isNaN(Number(limit)))
    return new Response("bad request", { status: 400 });

  switch (mod) {
    case "list":
      return GET_list(id, Number(limit || 100));
    case "now":
      return GET_now(id);
    default:
      return new Response("bad request", { status: 400 });
  }
}

export { GET };
