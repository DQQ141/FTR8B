@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\FGR12B release\FGR12B.bin" "..\..\Output\FGR12B release\FGR12B.axf"
..\CalcBINCRC "..\..\Output\FGR12B release\FGR12B.bin"
..\ConvertBIN "..\..\Output\FGR12B release\FGR12B.bin" "..\..\Output\FGR12B release\FGR12B.h" FGr12B_Bin
