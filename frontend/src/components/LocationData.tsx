import { LocationData } from "../types/types";
import { DataContainer, DataText } from "./styles/styles";

function Location({ lat, lng }: LocationData) {
  return (
    <DataContainer>
      <DataText>
        <div>
          <b>GPS:</b> {lat}, {lng}{" "}
        </div>
      </DataText>
    </DataContainer>
  );
}

export default Location;
