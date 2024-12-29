SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files (x86)\Pico Technology\PicoScope6\;C:\Program Files\TortoiseSVN\bin;C:\Program Files\Git\cmd;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Users\dengqi\AppData\Local\Microsoft\WindowsApps;C:\Users\dengqi\AppData\Local\Programs\Microsoft VS Code\bin
SET CPU_TYPE=STM32F051K8Ux
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=GMr release
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via "..\..\output\gmr release\startup_stm32f051x8._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\adc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\eeprom.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\encryption.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\spi sx1280.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\gyrogmr.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\i2c.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\lnk timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\led.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\npt timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\system.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\..\output\gmr release\system receiver.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via "..\..\Output\GMr release\GMr.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" "..\..\Output\GMr release\GMr.axf" --i32combined --output "..\..\Output\GMr release\GMr.hex"
cmd /c "Post GMr release.bat"
