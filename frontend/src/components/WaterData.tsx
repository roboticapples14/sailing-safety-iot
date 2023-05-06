import { DataContainer, DataText, WarningText } from "./styles/styles";

interface WaterDataProps {
  choppinessLevel: number;
}

// Water levels are from 0 to 7
const STORM_WARNING = 6;
const WATER_WARNING = 4;

function WaterData({ choppinessLevel }: WaterDataProps) {
  const needToRecharge = () => {
    if (choppinessLevel >= STORM_WARNING) {
      return (
        <WarningText>
          <b>Storm warning</b>
        </WarningText>
      );
    }
    if (choppinessLevel >= WATER_WARNING) {
      return (
        <WarningText>
          <b>Strong wind</b>
        </WarningText>
      );
    }
  };
  return (
    <DataContainer>
      <DataText>
        <div>
          <b>Choppiness level: </b> {choppinessLevel}
        </div>
      </DataText>
      {needToRecharge()}
    </DataContainer>
  );
}

export default WaterData;
