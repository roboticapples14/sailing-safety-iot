import RadioButtonGroup from "./components/inputs/RadioButtonGroup";
import { useEffect, useState } from "react";
import "./App.css";
import Map from "./components/Map";
import Header from "./components/Header";
import Location from "./components/LocationData";
import SOS, { SOSContainer } from "./components/SOSData";
import styled from "styled-components";
import { fetch_uplink, send_downlink } from "./utils/dataLoader";
import Battery from "./components/BatteryData";
import WaterData from "./components/WaterData";
import { UplinkData } from "./types/types";
import UpdateTime from "./components/UpdateTime";
import ConfirmationPopup from "./components/ConfirmationPopup";

function App() {
  const commCodes = [
    {
      label: "Come in",
      value: "AA==",
      name: "commCode",
    },
    {
      label: "Storm warning",
      value: "AQ==",
      name: "commCode",
    },
    {
      label: "Strong winds",
      value: "Ag==",
      name: "commCode",
    },
    {
      label: "Boat clearance",
      value: "Aw==",
      name: "commCode",
    },
    {
      label: "Clear Message",
      value: "MA==",
      name: "commCode",
    }
  ];

  const [selectedValue, setSelectedValue] = useState<String>(
    commCodes[0].label
  );
  const [isPopupOpen, setIsPopupOpen] = useState<Boolean>(false);
  const [uplinkData, setUplinkData] = useState<UplinkData>({
    acc: 0,
    bat: 0,
    sos: 0,
    mob: 0,
    x: 0,
    y: 0,
  });

  const codeSelectionHandler = (event: React.ChangeEvent<HTMLInputElement>) => {
    const newValue = event.target.value;
    setIsPopupOpen(false);
    setSelectedValue((prevValue) => {
      // Deselect other radio buttons
      if (prevValue !== newValue) {
        return newValue;
      } else {
        return "";
      }
    });
  };

  useEffect(() => {
    const dataFetch = async () => {
      const data = await fetch_uplink();
      setUplinkData(data);
    };
    // dataFetch();
    const interval = setInterval(() => {
      dataFetch();
    }, 1000);
    return () => clearInterval(interval);
  }, []);

  const sentDownlink = async () => {
    await send_downlink(selectedValue);
    setIsPopupOpen(true);
  };

  function get_message(encoding) {

    if (encoding == "AA==") {
      return "Come in";
    }
    if (encoding == "AQ==") {
      return "Storm warning";
    }
    if (encoding == "Ag==") {
      return "Strong winds";
    }
    if (encoding == "Aw==") {
      return "Boat clearance";
    }
  };

  return (
    <div className="App">
      <Header />
      <SOS sos={uplinkData.sos} mob={uplinkData.mob}/>
      <BoatContainer>
        <div>
          <Location lat={uplinkData.x} lng={uplinkData.y} />
          <WaterData choppinessLevel={uplinkData.acc} />
          <Battery batteryLevel={uplinkData.bat} />
          <ConfirmationPopup
            isPopupOpen={isPopupOpen}
            message={get_message(selectedValue)}
            closePopup={() => setIsPopupOpen(false)}
          />
          <SendDownstreamContainer>
            <form
              onSubmit={(event) => event.preventDefault()}
              id="downstreamForm"
            >
              <RadioButtonGroup
                label="Send message to boat:"
                options={commCodes}
                onChange={(event) => codeSelectionHandler(event)}
              />
              <div>
                <button type="submit" id="sendButton" onClick={sentDownlink}>
                  Send
                </button>
              </div>
            </form>
          </SendDownstreamContainer>
          <UpdateTime receivedAt={uplinkData.receivedAt} />
        </div>

        <Map lat={uplinkData.x} lng={uplinkData.y} />
      </BoatContainer>
    </div>
  );
}

const BoatContainer = styled.div`
  width: 80%;
  display: flex;
  flex-direction: row;
  text-align: center;
  justify-content: space-around;
  margin: auto;
  padding: 20px;
  border: 2px #fafafa solid;
  border-radius: 20px;
  background-color: #26547c;
  box-shadow: 0px 0px 20px #e67e22;
`;

const SendDownstreamContainer = styled.fieldset`
  width: 100%;
  text-align: center;
  margin: 20px auto;
  background-color: #e67e22;
  border-radius: 5px;
`;

export default App;
