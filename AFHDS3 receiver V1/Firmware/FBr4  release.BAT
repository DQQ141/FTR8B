SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;C:\Program Files\Python36\Scripts\;C:\Program Files\Python36\;C:\Program Files (x86)\STMicroelectronics\st_toolset\asm;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\TortoiseSVN\bin;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files (x86)\Pico Technology\PicoScope6\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Users\yangkunlin\AppData\Local\Microsoft\WindowsApps;C:\Users\yangkunlin\AppData\Local\Programs\Microsoft VS Code\bin;C:\Program Files\TortoiseSVN\bin;
SET CPU_TYPE=STM32G070CBTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=FBr4  release
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via "..\..\output\fbr4 release\startup_stm32g070xx._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\adc.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\eeprom.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\encryption.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\spi sx1280.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\flash.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\i2c.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\lnk timer.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\led.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\npt timer.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\pwm adapter.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\system.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\system receiver.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fbr4 release\usart new port.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via "..\..\Output\FBr4 release\FBr4.lnp"
"C:\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" "..\..\Output\FBr4 release\FBr4.axf" --i32combined --output "..\..\Output\FBr4 release\FBr4.hex"
cmd /c "Post FBr4 release.bat"
