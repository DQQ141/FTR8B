SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;C:\Keil_v5\ARM\ARMCC\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files (x86)\Pico Technology\PicoScope6\;C:\Program Files\TortoiseSVN\bin;C:\Program Files\Git\cmd;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility;C:\Users\dengqi\AppData\Local\Microsoft\WindowsApps;C:\Users\dengqi\AppData\Local\Programs\Microsoft VS Code\bin
SET ARMCC6_ASMOPT=--diag_suppress=9931
SET ARMCC6_CCOPT=--diag_suppress=9931
SET ARMCC6_LINKOPT=--diag_suppress=9931
SET CPU_TYPE=STM32G070CBTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=A_FS_TSST
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via "..\..\output\fgr12b bootloader release\startup_stm32g070xx._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\flash.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\lnk timer.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\adc.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\led.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\spi sx1280.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\system.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\system receiver.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\encryption.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\poweronch.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via "..\..\Output\FGr12B bootloader release\FGr12B bootloader.lnp"
"C:\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" "..\..\Output\FGr12B bootloader release\FGr12B bootloader.axf" --i32combined --output "..\..\Output\FGr12B bootloader release\FGr12B bootloader.hex"
