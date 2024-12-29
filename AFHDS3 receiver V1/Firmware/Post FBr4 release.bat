@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\FBr4 release\FBr4.bin" "..\..\Output\FBr4 release\FBr4.axf"
..\CalcBINCRC "..\..\Output\FBr4 release\FBr4.bin"
..\ConvertBIN "..\..\Output\FBr4 release\FBr4.bin" "..\..\Output\FBr4 release\FBr4.h" FBr4_Bin
