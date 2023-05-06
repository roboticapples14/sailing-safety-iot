import styled from "styled-components";
import { SOSData } from "../types/types";
import { DataContainer, DataText } from "./styles/styles";


function SOS({ sos, mob }: SOSData) {
    let sos_status = "Good";
    let mob_status = "Good";

    if (document.getElementById("sos") != null && document.getElementById("status") != null && document.getElementById("mob") != null) {
        if (sos === 0) {
            sos_status = "Good"
            document.getElementById("sos").style.fontSize = "medium";
        }
        else if (sos === 1) {
            sos_status = "SOS"
            document.getElementById("status").style.backgroundColor = "#dd0000";
            document.getElementById("sos").style.fontSize = "large";
        }
        if (mob === 0) {
            mob_status = "Good"
            document.getElementById("mob").style.fontSize = "medium";
        }
        else if (mob === 1) {
            mob_status = "Man Overboard"
            document.getElementById("status").style.backgroundColor = "#dd0000";
            document.getElementById("mob").style.fontSize = "large";
        }

        if (mob === 0 && sos === 0) {
            document.getElementById("status").style.backgroundColor = "#26547c";
        }
    }

  return (
    <SOSContainer id="status">
        <DataContainer>
        <DataText>

            <div id="sos">
            <b>Boat Status:</b> {sos_status}
            </div>

            <div id="mob">
            <b>Sailor Status:</b> {mob_status}
            </div>

        </DataText>
        </DataContainer>
    </SOSContainer>
  );
}

export const SOSContainer = styled.div`
    width: 80%;
    display: flex;
    flex-wrap:wrap;
    flex-direction: row;
    text-align: center;
    justify-content: space-around;
    margin: auto;
    margin-bottom: 20px;
    padding: 20px;
    border: 1px #fafafa solid;
    border-radius: 10px;
    background-color: #26547c;
    box-shadow: 0px 0px 5px #e67e22;
`;

export const StatusContainer = styled.div`
    display: inline-block;
    margin:auto;
`;

export default SOS;
