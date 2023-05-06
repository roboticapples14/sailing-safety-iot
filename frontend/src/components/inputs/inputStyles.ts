import styled from "styled-components";

export enum OurColors {
    // NAVY = "#3e5e72",
    primarycolor = "#00a8ff",
    secondarycolor = "#008dc4",
    accentcolor = "#e67e22",
    textcolor = "#333",
    white = "#FAFAFA"
 };
 

export const Radio = styled.input`
   -webkit-appearance: none;
   appearance: none;
   margin: 0;
   width: 1.5em;
   height: 1.5em;
   border: 2px solid ${OurColors.primarycolor};
   border-radius: 50%;
   ::after {
      content: "";
      display: block;
      border-radius: 50%;
      width: 0.75em;
      height: 0.75em;
      margin: 3px;
   }
   :hover {
    ::after {
       background-color: ${OurColors.primarycolor}
        }
    }
    :focus {
        outline: 2px solid ${OurColors.white};
    }
    :checked {
    ::after {
       background-color: ${OurColors.accentcolor};
    }
    :hover {
       background-color: ${OurColors.secondarycolor};
       border: 2px solid ${OurColors.textcolor};
       ::after {
          background-color: ${OurColors.primarycolor}
       }
    }
 }
`;

export const Label = styled.label<{ disabled?: boolean; }>`
   font-size: 1rem;
   font-weight: 600;
   color: ${OurColors.white};
   font-family: StabilGrotesk, -apple-system, BlinkMacSystemFont,
       "Segoe UI", Roboto, Oxygen-Sans, Ubuntu, Cantarell,
       "Helvetica Neue", sans-serif;
`;

export const Legend = styled.legend`
  font-weight: 800;
  font-size: 20px;
  color: ${OurColors.white};
  text-shadow: 0px 0px 2px ${OurColors.textcolor};
  font-family: StabilGrotesk, -apple-system, BlinkMacSystemFont, "Segoe UI",
    Roboto, Oxygen-Sans, Ubuntu, Cantarell, "Helvetica Neue", sans-serif;
`;
