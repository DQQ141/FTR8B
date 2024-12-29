@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\FTR8B release\FTR8B.bin" "..\..\Output\FTR8B release\FTR8B.axf"
..\CalcBINCRC "..\..\Output\FTR8B release\FTR8B.bin"
..\ConvertBIN "..\..\Output\FTR8B release\FTR8B.bin" "..\..\Output\FTR8B release\FTR8B.h" FTr8B_Bin
