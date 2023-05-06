import styled from "styled-components";

function Header() {
  return (
    <HeaderContainer>
      <h1>Boat safety service</h1>
    </HeaderContainer>
  );
}

const HeaderContainer = styled.div`
  display: flex;
  flex-direction: row;
  text-align: center;
  align-items: center;
  justify-content: center;
  height:70px;
`;

export default Header;