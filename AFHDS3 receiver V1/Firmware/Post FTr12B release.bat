@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\FTr12B release\FTr12B.bin" "..\..\Output\FTr12B release\FTr12B.axf"
..\CalcBINCRC "..\..\Output\FTr12B release\FTr12B.bin"
..\ConvertBIN "..\..\Output\FTr12B release\FTr12B.bin" "..\..\Output\FTr12B release\FTr12B.h" FTr12B_Bin
