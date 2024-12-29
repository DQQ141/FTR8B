@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\INr6-HS release\INr6-HS.bin" "..\..\Output\INr6-HS release\INr6-HS.axf"
..\CalcBINCRC "..\..\Output\INr6-HS release\INr6-HS.bin"
..\ConvertBIN "..\..\Output\INr6-HS release\INr6-HS.bin" "..\..\Output\INr6-HS release\INr6-HS.h" INr6_HS_Bin
