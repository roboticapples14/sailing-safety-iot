import { InputHTMLAttributes } from "react";

export interface InputElementProps extends InputHTMLAttributes<HTMLInputElement> {
  label: string;
  id: string; 
  key?: string;
  disabled?: boolean;
}

export interface IOption {
    label: string;
    value: string;
    name?: string;
    disabled?: boolean;
 }
 
 export interface IOptionGroup {
    label: string;
    options: IOption[];
    onChange: (e: React.ChangeEvent<HTMLInputElement>) => void
 }