import { UplinkData } from "../types/types"

export default function uplinkParser(data: UplinkData) {
  const x: number = parseFloat(data.x.toString().substring(0, 8))
  const y: number = parseFloat(data.y.toString().substring(0, 8))
  const receivedAt: string = new Date(data.receivedAt).toString()
  return { ...data, x, y, receivedAt }
}
