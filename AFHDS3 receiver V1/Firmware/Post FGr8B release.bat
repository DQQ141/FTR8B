@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\FGR8B release\FGR8B.bin" "..\..\Output\FGR8B release\FGR8B.axf"
..\CalcBINCRC "..\..\Output\FGR8B release\FGR8B.bin"
..\ConvertBIN "..\..\Output\FGR8B release\FGR8B.bin" "..\..\Output\FGR8B release\FGR8B.h" FGr8B_Bin
