@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\FGR4B release\FGR4B.bin" "..\..\Output\FGR4B release\FGR4B.axf"
..\CalcBINCRC "..\..\Output\FGR4B release\FGR4B.bin"
..\ConvertBIN "..\..\Output\FGR4B release\FGR4B.bin" "..\..\Output\FGR4B release\FGR4B.h" FGR4B_Bin
