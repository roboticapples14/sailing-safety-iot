import axios from "axios";
import uplinkParser from "./uplinkParser";
import { THINGS_STACK_API_KEY } from "../secrets";

axios.defaults.headers.common['Authorization'] = `Bearer ${THINGS_STACK_API_KEY}`;

function get_distance(lat1, lon1, lat2, lon2) {
  const R = 6371e3; // metres
  const φ1 = lat1 * Math.PI/180; // φ, λ in radians
  const φ2 = lat2 * Math.PI/180;
  const Δφ = (lat2-lat1) * Math.PI/180;
  const Δλ = (lon2-lon1) * Math.PI/180;

  const a = Math.sin(Δφ/2) * Math.sin(Δφ/2) +
            Math.cos(φ1) * Math.cos(φ2) *
            Math.sin(Δλ/2) * Math.sin(Δλ/2);
  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));

  const d = R * c; // in metres
  return d
}

export async function fetch_uplink() {
  const url = 'https://eu1.cloud.thethings.network/api/v3/as/applications/iot-34346/packages/storage/uplink_message'
  const { data } = await axios.get(url, { params: { limit: 1, order: "-received_at" } })
  if (data) {
    const payload = data.result.uplink_message.decoded_payload
    const receivedAt = data.result.uplink_message.received_at
    return uplinkParser({ ...payload, receivedAt })
  }
  return {
    acc: 0,
    bat: 0,
    sos: 0,
    mob: 0,
    x: 0,
    y: 0,
    receivedAt: undefined,
  }
}

export async function send_downlink(payload: String) {
  const downlink_body = {
    "downlinks": [{
      "frm_payload": payload,
      "f_port": 1
    }]
  }
  const url = 'https://eu1.cloud.thethings.network/api/v3/as/applications/iot-34346/devices/eui-70b3d57ed005c2c6/down/push'
  const response = await axios.post(url, downlink_body)
  console.log("Downlink send");
  console.log(response);
  console.log(payload);
}
