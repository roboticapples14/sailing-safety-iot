import styled from "styled-components";

function UpdateTime({ receivedAt }) {
  return <UpdateContainer>Last update: {receivedAt}</UpdateContainer>;
}

export const UpdateContainer = styled.div`
  height: 30px;
  display: flex;
  flex-direction: row;
  text-align: left;
  align-items: flex-start;
  align-items: center;
  font-size: 10px;
`;

export default UpdateTime;
