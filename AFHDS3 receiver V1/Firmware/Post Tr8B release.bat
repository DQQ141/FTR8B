@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\Tr8B release\Tr8B.bin" "..\..\Output\Tr8B release\Tr8B.axf"
..\CalcBINCRC "..\..\Output\Tr8B release\Tr8B.bin"
..\ConvertBIN "..\..\Output\Tr8B release\Tr8B.bin" "..\..\Output\Tr8B release\Tr8B.h" Tr8B_Bin
