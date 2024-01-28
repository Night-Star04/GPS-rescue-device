type GPS_Data = {
  id: string;
  time: number; // timestamp
  lat: number; // latitude
  lng: number; // longitude
  satellites?: number; // Number of satellites in use, 00 - 12
  hdop?: number; // Horizontal Dilution of Precision
  altitude?: number; // Altitude above mean sea level
  geoid?: number; // Height of geoid (mean sea level) above WGS84 ellipsoid
};

export type { GPS_Data };
