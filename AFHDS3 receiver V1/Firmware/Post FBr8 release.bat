@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\FBr8 release\FBr8.bin" "..\..\Output\FBr8 release\FBr8.axf"
..\CalcBINCRC "..\..\Output\FBr8 release\FBr8.bin"
..\ConvertBIN "..\..\Output\FBr8 release\FBr8.bin" "..\..\Output\FBr8 release\FBr8.h" FBr8_Bin
