import styled from "styled-components";
import { Legend } from "./inputStyles";
import { IOptionGroup, IOption } from "./InputInterface";
import RadioButton from "./RadioButton";

const Fieldset = styled.fieldset`
  border: none;
`;

const Wrapper = styled.div`
   display: grid;
   gap: 1rem;
   padding: 0.5rem;
`;

const RadioButtonGroup = ({ label, options, onChange }: IOptionGroup) => {
   function renderOptions(){
      return options.map(({label, value, name, disabled }: IOption, index) => {
         const shortenedOptionLabel = label.replace(/\s+/g, "");
         const optionId = `radio-option-${shortenedOptionLabel}`;
         
         return (         
            <RadioButton
               key={value}
               value={value}
               label={label} 
               id={optionId}
               name={name}
               disabled={disabled}
               defaultChecked={index === 0}
               onChange={onChange}
            />
         );
      });
   }

   return (
      <Fieldset>
         <Legend>{label}</Legend>
         <Wrapper>
            {renderOptions()}
         </Wrapper>         
      </Fieldset>
   );
};

export default RadioButtonGroup;