export interface LocationData {
  lat: number;
  lng: number;
}

export interface SOSData {
  sos: number;
  mob: number;
}

export interface BatteryProps {
  batteryLevel: number;
}

export interface UplinkData {
  acc: number;
  bat: number;
  sos: number;
  mob: number;
  x: number;
  y: number;
  receivedAt?: string,
}