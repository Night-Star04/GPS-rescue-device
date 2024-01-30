"use client";
import { useEffect, useRef, useState } from "react";
import { Loader } from "@googlemaps/js-api-loader";
import useSWR from "swr";
import { fetcher_json } from "@/util";
import type { Device_Info } from "@/interface/device";
import type { GPS_Data } from "@/interface/data";
import {
  Stack,
  Paper,
  Typography as Text,
  Box,
  Table,
  TableHead,
  TableRow,
  TableCell,
  TableBody,
  Divider,
  FormControl,
  InputLabel,
  MenuItem,
  Select,
} from "@mui/material";

function DataTable(props: { id?: string }) {
  const { id } = props;
  const { data, isLoading } = useSWR(
    id ? `/api/data/get/?id=${id}&mod=list` : null,
    fetcher_json<Array<GPS_Data>>,
    { refreshInterval: 1000 }
  );
  return (
    <>
      {isLoading ? (
        <Text
          variant="h5"
          sx={{
            alignItems: "center",
            justifyContent: "center",
            display: "flex",
            height: "100%",
          }}
        >
          Loading...
        </Text>
      ) : (
        <>
          <Text variant="h5">Data</Text>
          <Table>
            <TableHead>
              <TableRow>
                <TableCell>Time</TableCell>
                <TableCell>Latitude</TableCell>
                <TableCell>Longitude</TableCell>
                <TableCell>Altitude</TableCell>
                <TableCell>Altitude (msl)</TableCell>
                <TableCell>HDOP</TableCell>
                <TableCell>Url</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {data?.map((row) => (
                <TableRow key={row.id}>
                  <TableCell>{new Date(row.time).toUTCString()}</TableCell>
                  <TableCell>{row.lat}</TableCell>
                  <TableCell>{row.lng}</TableCell>
                  <TableCell>{row.altitude}</TableCell>
                  <TableCell>{row.geoid}</TableCell>
                  <TableCell>{row.hdop}</TableCell>
                  <TableCell>
                    <a
                      href={`https://www.google.com/maps/search/?api=1&query=${row.lat},${row.lng}`}
                      target="_blank"
                      rel="noreferrer"
                    >
                      Link
                    </a>
                  </TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
        </>
      )}
    </>
  );
}

const GOOGLE_MAPS_API_KEY = process.env.NEXT_PUBLIC_GOOGLE_MAPS_API_KEY || "";
function Map(props: { position: google.maps.LatLngLiteral }) {
  const { position } = props;
  const mapRef = useRef<HTMLDivElement>(null);
  const [Location, setLocation] = useState<google.maps.LatLngLiteral | null>(
    null
  );
  const [address, setAddress] = useState<string | null>(null);

  useEffect(() => {
    const loader = new Loader({
      apiKey: GOOGLE_MAPS_API_KEY,
      version: "weekly",
      libraries: ["maps", "marker", "geocoding"],
    });

    if (
      position.lat.toFixed(3) === Location?.lat.toFixed(3) &&
      position.lng.toFixed(3) === Location?.lng.toFixed(3)
    )
      return; // prevent re-render

    let map: google.maps.Map;
    let marker: google.maps.Marker;
    loader.importLibrary("maps").then(() => {
      const google = window.google;
      map = new google.maps.Map(mapRef.current!, {
        center: position,
        zoom: 15,
      });
      setLocation(position);
    });
    loader.importLibrary("marker").then(() => {
      const google = window.google;
      marker = new google.maps.Marker({
        position: position,
        map: map,
        title: "Device Location",
      });
    });
    loader.importLibrary("geocoding").then(() => {
      const google = window.google;
      const geocoder = new google.maps.Geocoder();
      geocoder.geocode(
        { location: position, region: "tw" },
        (results, status) => {
          if (status === "OK" && results) {
            setAddress(results[0].formatted_address);
          } else {
            setAddress("Cannot get address");
          }
        }
      );
    });
  });
  return (
    <>
      <Text variant="body1">
        {address ? (
          <a
            href={`https://www.google.com/maps/search/?api=1&query=${address}`}
            target="_blank"
          >
            {address}
          </a>
        ) : (
          "Getting address..."
        )}
      </Text>
      <Text variant="subtitle1" sx={{ pl: 1 }}>
        {position.lat}, {position.lng}
      </Text>
      <div id="map" style={{ height: "49vh" }} ref={mapRef}></div>
    </>
  );
}

type HomeProps = { searchParams: { id?: string } };

function Home(Props: HomeProps) {
  const [deviceID, setDeviceID] = useState<string | undefined>(
    Props.searchParams.id
  );
  const { data: Info, isLoading } = useSWR(
    deviceID ? `/api/device/get/?id=${deviceID}` : null,
    fetcher_json<Device_Info>
  );
  const { data: nowData, isLoading: isLoading2 } = useSWR(
    deviceID ? `/api/data/get/?id=${deviceID}&mod=now` : null,
    fetcher_json<GPS_Data>,
    { refreshInterval: 1000 }
  );
  const { data: DeviceList } = useSWR(
    "/api/device/list/get",
    fetcher_json<Array<Device_Info>>
  );

  return (
    <>
      <Stack direction={{ xs: "column", md: "row" }} spacing={2}>
        <Box sx={{ width: { xs: "100%", md: "25%" } }}>
          <Paper sx={{ p: 2 }} elevation={6}>
            <Text variant="h5">Info</Text>
            <div
              style={{
                display: "flex",
                width: "100%",
                justifyContent: "space-between",
              }}
            >
              <div>
                <Text variant="body1" sx={{ pl: 2 }}>
                  ID: {deviceID || "N/A"}
                </Text>
                <Text variant="body1" sx={{ pl: 2 }}>
                  Name: {Info?.name || "N/A"}
                </Text>
              </div>
              <Divider orientation="vertical" flexItem />
              <div
                style={{
                  display: "flex",
                  justifyContent: "center",
                  alignItems: "center",
                }}
              >
                <Text variant="body1">Device:</Text>
                <FormControl sx={{ m: 1, minWidth: 100 }} size="small">
                  <InputLabel id="Device-ID-select">ID</InputLabel>
                  <Select
                    labelId="Device-ID-select"
                    id="Device-ID-select"
                    label="ID"
                    value={deviceID || ""}
                    onChange={(e) => setDeviceID(e.target.value)}
                  >
                    {DeviceList
                      ? DeviceList.map(({ id, name }) => (
                          <MenuItem key={id} value={id}>
                            {name ? `${name}(${id})` : id}
                          </MenuItem>
                        ))
                      : null}
                  </Select>
                </FormControl>
              </div>
            </div>
          </Paper>
          <Paper sx={{ p: 2, mt: 2 }} elevation={6}>
            <Text variant="h5">Now Location:</Text>
            <div style={{ display: "inline-block" }}>
              {[
                "Time: ",
                "Latitude: ",
                "Longitude: ",
                "Altitude: ",
                "Altitude (msl): ",
                "HDOP: ",
              ].map((item, index) => (
                <Text
                  variant="body1"
                  sx={{ pl: 2 }}
                  key={index}
                  style={{ display: "block" }}
                >
                  {item}
                </Text>
              ))}
            </div>
            <div style={{ display: "inline-block" }}>
              {[
                new Date(nowData?.time as number).toLocaleString(),
                nowData?.lat,
                nowData?.lng,
                nowData?.altitude,
                nowData?.geoid,
                nowData?.hdop,
              ].map((item, index) => (
                <Text
                  variant="body1"
                  sx={{ pl: 2 }}
                  key={index}
                  style={{ display: "block" }}
                >
                  {item || "N/A"}
                </Text>
              ))}
            </div>
          </Paper>
          <Paper sx={{ p: 2, mt: 2 }} elevation={6}>
            <Text variant="h5">Map</Text>
            {isLoading2 ? (
              <Text
                variant="h5"
                sx={{
                  alignItems: "center",
                  justifyContent: "center",
                  display: "flex",
                  height: "100%",
                  minHeight: "50vh",
                }}
              >
                Loading...
              </Text>
            ) : (
              <Map
                position={{
                  lat: nowData?.lat || 0,
                  lng: nowData?.lng || 0,
                }}
              />
            )}
          </Paper>
        </Box>
        <Paper
          sx={{
            p: 2,
            width: { xs: "100%", md: "75%" },
            height: { md: "95vh" },
          }}
          elevation={6}
        >
          <DataTable id={deviceID} />
        </Paper>
      </Stack>
    </>
  );
}

export default Home;
