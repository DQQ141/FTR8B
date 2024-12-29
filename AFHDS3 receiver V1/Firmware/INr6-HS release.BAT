SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;C:\Program Files\Python36\Scripts\;C:\Program Files\Python36\;C:\Program Files (x86)\STMicroelectronics\st_toolset\asm;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\TortoiseSVN\bin;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files (x86)\Pico Technology\PicoScope6\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Users\yangkunlin\AppData\Local\Microsoft\WindowsApps;C:\Users\yangkunlin\AppData\Local\Programs\Microsoft VS Code\bin;C:\Program Files\TortoiseSVN\bin;C:\Users\yangkunlin\AppData\Local\GitHubDesktop\bin
SET CPU_TYPE=STM32G071KBUx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=INr6-HS release
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via "..\..\output\inr6-hs release\startup_stm32g071xx._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\adc.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\bmp280.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\eeprom.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\encryption.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\spi sx1280.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\flash.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\i2c.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\lnk timer.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\led.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\npt timer.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\pwm adapter.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\system.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\system receiver.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\inr6-hs release\usart new port.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via "..\..\Output\INr6-HS release\INr6-HS.lnp"
"C:\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" "..\..\Output\INr6-HS release\INr6-HS.axf" --i32combined --output "..\..\Output\INr6-HS release\INr6-HS.hex"
cmd /c "Post INr6-HS release.bat"
