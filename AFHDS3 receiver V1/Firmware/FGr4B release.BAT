SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;C:\Program Files\Python36\Scripts\;C:\Program Files\Python36\;C:\Program Files (x86)\STMicroelectronics\st_toolset\asm;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\TortoiseSVN\bin;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files (x86)\Pico Technology\PicoScope6\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Users\yangkunlin\AppData\Local\Microsoft\WindowsApps;C:\Users\yangkunlin\AppData\Local\Programs\Microsoft VS Code\bin;C:\Program Files\TortoiseSVN\bin;
SET CPU_TYPE=STM32G070CBTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=FGr4B release
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via "..\..\output\fgr4b release\startup_stm32g070xx._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr4b release\adc.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @"..\..\output\fgr4b release\eeprom.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via "..\..\Output\FGr4B release\FGr4B.lnp"
