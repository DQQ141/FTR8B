@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\INr6-FC release\INr6-FC.bin" "..\..\Output\INr6-FC release\INr6-FC.axf"
..\CalcBINCRC "..\..\Output\INr6-FC release\INr6-FC.bin"
..\ConvertBIN "..\..\Output\INr6-FC release\INr6-FC.bin" "..\..\Output\INr6-FC release\INr6-FC.h" INr6_FC_Bin
