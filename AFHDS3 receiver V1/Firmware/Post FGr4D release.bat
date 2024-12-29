@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\FGR4D release\FGR4D.bin" "..\..\Output\FGR4D release\FGR4D.axf"
..\CalcBINCRC "..\..\Output\FGR4D release\FGR4D.bin"
..\ConvertBIN "..\..\Output\FGR4D release\FGR4D.bin" "..\..\Output\FGR4D release\FGR4D.h" FGR4D_Bin
