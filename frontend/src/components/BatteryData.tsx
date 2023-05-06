import styled from "styled-components";
import { BatteryProps } from "../types/types";
import { DataContainer, DataText } from "./styles/styles";

function Battery({ batteryLevel }: BatteryProps) {
  const findBatteryStatus = () => {
    switch (batteryLevel) {
      case 0:
        return <BatteryLevel color={"#FF7276"}>20%</BatteryLevel>;
      case 1:
        return <BatteryLevel color={"black"}>50%</BatteryLevel>;
      case 2:
        return <BatteryLevel color={"#90EE90"}>85%</BatteryLevel>;
    }
  };

  return (
    <DataContainer>
      <DataText>
        <div>
          <b>Battery level: </b>
          {findBatteryStatus()}
        </div>
      </DataText>
    </DataContainer>
  );
}

const BatteryLevel = styled.span<{ color: string }>`
  color: ${(p) => p.color};
`;

export default Battery;
