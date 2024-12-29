SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;C:\Program Files\Python36\Scripts\;C:\Program Files\Python36\;C:\Program Files (x86)\STMicroelectronics\st_toolset\asm;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\TortoiseSVN\bin;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files (x86)\Pico Technology\PicoScope6\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Users\yangkunlin\AppData\Local\Microsoft\WindowsApps;C:\Users\yangkunlin\AppData\Local\Programs\Microsoft VS Code\bin;C:\Program Files\TortoiseSVN\bin;
SET CPU_TYPE=STM32G070CBTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=FGr12B release
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via "..\..\output\fgr12b bootloader release\startup_stm32g070xx._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\flash.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\lnk timer.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\led.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\spi sx1280.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\system.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\system receiver.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\encryption.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr12b bootloader release\poweronch.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via "..\..\Output\FGr12B bootloader release\FGr12B bootloader.lnp"
"C:\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" "..\..\Output\FGr12B bootloader release\FGr12B bootloader.axf" --i32combined --output "..\..\Output\FGr12B bootloader release\FGr12B bootloader.hex"
