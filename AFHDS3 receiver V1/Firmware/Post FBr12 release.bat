@echo off
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bincombined --output "..\..\Output\FBr12 release\FBr12.bin" "..\..\Output\FBr12 release\FBr12.axf"
..\CalcBINCRC "..\..\Output\FBr12 release\FBr12.bin"
..\ConvertBIN "..\..\Output\FBr12 release\FBr12.bin" "..\..\Output\FBr12 release\FBr12.h" FBr12_Bin
