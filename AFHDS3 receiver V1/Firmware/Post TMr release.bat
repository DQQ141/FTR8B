@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\TMr release\TMr.bin" "..\..\Output\TMr release\TMr.axf"
..\CalcBINCRC "..\..\Output\TMr release\TMr.bin"
..\ConvertBIN "..\..\Output\TMr release\TMr.bin" "..\..\Output\TMr release\TMr.h" TMr_Bin
