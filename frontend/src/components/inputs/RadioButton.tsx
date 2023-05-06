import styled from "styled-components";
import { Label } from "./inputStyles";
import { InputElementProps } from "./InputInterface";

const Wrapper = styled.div`
  display: flex;
  gap: 0.5rem;
  align-items: center;
`;

const RadioButton = ({
  label,
  id,
  name,
  disabled = false,
  onChange,
  value,
}: InputElementProps) => {
  return (
    <Wrapper>
      <input
        value={value}
        type="radio"
        id={id}
        name={name}
        disabled={disabled}
        onChange={onChange}
      />
      <Label htmlFor={id} disabled={disabled}>
        {label}
      </Label>
    </Wrapper>
  );
};

export default RadioButton;
