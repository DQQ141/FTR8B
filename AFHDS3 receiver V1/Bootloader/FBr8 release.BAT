SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;D:\Program Files\Python38\Scripts\;D:\Program Files\Python38\;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\TortoiseSVN\bin;C:\Windows\system32\config\systemprofile\.dnx\bin;C:\Program Files\Microsoft DNX\Dnvm\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Program Files\Git\cmd;C:\Program Files (x86)\Pico Technology\PicoScope6\;C:\Users\heqiuyun\AppData\Local\Microsoft\WindowsApps;;C:\Users\heqiuyun\AppData\Local\Programs\Microsoft VS Code\bin
SET CPU_TYPE=STM32G070CBTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=FBr8 release
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via "..\..\output\fbr8 bootloader release\startup_stm32g070xx._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\flash.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\lnk timer.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\adc.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\led.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\spi sx1280.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\system.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\system receiver.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\encryption.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr8 bootloader release\poweronch.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via "..\..\Output\FBr8 bootloader release\FBr8 bootloader.lnp"
"C:\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" "..\..\Output\FBr8 bootloader release\FBr8 bootloader.axf" --i32combined --output "..\..\Output\FBr8 bootloader release\FBr8 bootloader.hex"
