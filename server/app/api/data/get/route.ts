import { GetData, GetNowData } from "@/tool/data";
import { getQuery } from "@/util";

function Min(a: number, b: number) {
  return a < b ? a : b;
}

function GET_list(id: string, start: number = 0, limit: number = 100) {
  let data = GetData(id);
  if (!data) return new Response("not found", { status: 404 });
  const end = Min(start + limit, data.length);
  data.sort((a, b) => b.time - a.time); // sort by time
  data = data.slice(start, end); // limit
  return new Response(JSON.stringify(data), { status: 200 });
}

function GET_listLegth(id: string) {
  const data = GetData(id);
  if (!data) return new Response("not found", { status: 404 });

  return new Response(JSON.stringify({ length: data.length }), { status: 200 });
}

function GET_now(id: string) {
  const data = GetNowData(id);
  if (!data) return new Response("not found", { status: 404 });
  return new Response(JSON.stringify(data), { status: 200 });
}

function GET(request: Request) {
  const { id, mod, limit, page } = getQuery(request);
  if (!id || !mod) return new Response("bad request", { status: 400 });
  if (limit && isNaN(Number(limit)))
    return new Response("bad request", { status: 400 });
  if (page && isNaN(Number(page)))
    return new Response("bad request", { status: 400 });

  switch (mod) {
    case "list": {
      const Limit = Number(limit || 100);
      const Start = Number(page || 0) * Limit;
      return GET_list(id, Start, Limit);
    }
    case "now":
      return GET_now(id);
    case "listLegth":
      return GET_listLegth(id);
    default:
      return new Response("bad request", { status: 400 });
  }
}

export { GET };
