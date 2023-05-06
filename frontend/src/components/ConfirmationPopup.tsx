import styled from "styled-components";
import { DataContainer } from "./styles/styles";

function ConfirmationPopup({ isPopupOpen, message, closePopup }) {
  return (
    <PopupContainer>
      {isPopupOpen && (
        <DataContainer>
          <div>
            <b>Message sent:</b> {message}
          </div>
          <Button onClick={() => closePopup()}>OK</Button>
        </DataContainer>
      )}
    </PopupContainer>
  );
}

export const PopupContainer = styled.div`
  height: 30px;
  display: flex;
  flex-direction: row;
  text-align: left;
  align-items: flex-start;
  align-items: center;
`;

export const Button = styled.button`
  margin-left: 10px;
  background: #e67e22;
  color: #fff;
  border: 1px solid #eee;
  border-radius: 20px;
  box-shadow: 1px 1px 1px #eee;
  text-shadow: none;
`;

export default ConfirmationPopup;
