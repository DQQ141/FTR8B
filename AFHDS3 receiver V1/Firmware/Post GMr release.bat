@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\GMr release\GMr.bin" "..\..\Output\GMr release\GMr.axf"
..\CalcBINCRC "..\..\Output\GMr release\GMr.bin"
..\ConvertBIN "..\..\Output\GMr release\GMr.bin" "..\..\Output\GMr release\GMr.h" GMr_Bin
