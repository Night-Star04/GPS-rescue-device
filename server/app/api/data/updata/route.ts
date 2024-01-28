import type { GPS_Data } from "@/interface/data";
import { GPS_Coverter, UUID } from "@/util";
import { IsDeviceExist } from "@/tool/device";
import { EditNowData } from "@/tool/data";

function Err_response(
  str: string = "bad request",
  status: number = 400
): Response {
  return new Response(str, { status });
}

async function POST(request: Request) {
  if (request.headers.get("content-type") !== "application/json") {
    return new Response("bad request", { status: 400 });
  }

  const {
    id, // Device ID
    state, // GPS Status, 0=Invalid, 1=2D/3D, 2=DGNSS, 4=Fixed RTK, 5=Float RTK, 6=Dead Reckoning
    time, // UTC time of position fix, hhmmss.sss
    lat, // Latitude, ddmm.mmmm
    lat_dir, // N/S Indicator, N=north or S=south
    lng, // Longitude, dddmm.mmmm
    lng_dir, // E/W Indicator, E=east or W=west
    satellites, // Number of satellites in use, 00 - 12
    hdop, // Horizontal Dilution of Precision
    altitude, // Altitude above mean sea level
    geoid, // Height of geoid (mean sea level) above WGS84 ellipsoid
  } = await request.json();
  let err = false;
  if (!id || !state || !time || !lat || !lng) err = true;
  else if (!IsDeviceExist({ id })) err = true;
  else if (typeof state !== "string") err = true;
  else if (typeof time !== "string") err = true;
  else if (typeof lat !== "string") err = true;
  else if (typeof lat_dir !== "string" || !["N", "S", ""].includes(lat_dir))
    err = true;
  else if (typeof lng !== "string") err = true;
  else if (typeof lng_dir !== "string" || !["E", "W", ""].includes(lng_dir))
    err = true;
  else if (satellites && typeof satellites !== "string") err = true;
  else if (hdop && typeof hdop !== "string") err = true;
  else if (altitude && typeof altitude !== "string") err = true;
  else if (geoid && typeof geoid !== "string") err = true;

  if (err) return Err_response();

  const { lat: lat_num, lng: lng_num } = GPS_Coverter(
    lat,
    lat_dir,
    lng,
    lng_dir
  );
  let data: GPS_Data = {
    id: UUID(8),
    time: Date.now(),
    lat: lat_num,
    lng: lng_num,
  };
  if (satellites) {
    const satellites_num = parseInt(satellites);
    if (isNaN(satellites_num) || satellites_num < 0 || satellites_num > 12)
      return Err_response();
    data.satellites = satellites_num;
  }
  if (hdop) {
    const hdop_num = parseFloat(hdop);
    if (isNaN(hdop_num) || hdop_num < 0.5 || hdop_num > 99.99)
      return Err_response();
    data.hdop = hdop_num;
  }
  if (altitude) {
    const altitude_num = parseFloat(altitude);
    if (isNaN(altitude_num) || altitude_num < -9999.9 || altitude_num > 99999.9)
      return Err_response();
    data.altitude = altitude_num;
  }
  if (geoid) {
    const geoid_num = parseFloat(geoid);
    if (isNaN(geoid_num) || geoid_num < -9999.9 || geoid_num > 99999.9)
      return Err_response();
    data.geoid = geoid_num;
  }
  console.log(id);
  EditNowData(id, data);

  return new Response("ok", { status: 200 });
}

export { POST };
