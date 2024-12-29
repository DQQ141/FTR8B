SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;C:\Program Files\Python36\Scripts\;C:\Program Files\Python36\;C:\Program Files (x86)\STMicroelectronics\st_toolset\asm;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\TortoiseSVN\bin;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files (x86)\Pico Technology\PicoScope6\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Users\yangkunlin\AppData\Local\Microsoft\WindowsApps;C:\Users\yangkunlin\AppData\Local\Programs\Microsoft VS Code\bin;C:\Program Files\TortoiseSVN\bin;C:\Users\yangkunlin\AppData\Local\GitHubDesktop\bin
SET CPU_TYPE=STM32G071KBUx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=INr6-FC release
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via "..\..\output\inr6-fc release\startup_stm32g071xx._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\adc.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\bmp280.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\eeprom.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\encryption.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\spi sx1280.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\flash.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\fc.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\gyrofc.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\pid fc.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\imu.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\i2c.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\lnk timer.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\led.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\npt timer.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\pwm adapter.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\system.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\system receiver.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-fc release\usart new port.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via "..\..\Output\INr6-FC release\INr6-FC.lnp"
"C:\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" "..\..\Output\INr6-FC release\INr6-FC.axf" --i32combined --output "..\..\Output\INr6-FC release\INr6-FC.hex"
cmd /c "Post INr6-FC release.bat"
