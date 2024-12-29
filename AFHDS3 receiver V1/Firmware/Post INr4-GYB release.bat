@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\INr4-GYB release\INr4-GYB.bin" "..\..\Output\INr4-GYB release\INr4-GYB.axf"
..\CalcBINCRC "..\..\Output\INr4-GYB release\INr4-GYB.bin"
..\ConvertBIN "..\..\Output\INr4-GYB release\INr4-GYB.bin" "..\..\Output\INr4-GYB release\INr4-GYB.h" INr4_GYB_Bin
