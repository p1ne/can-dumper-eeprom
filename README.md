# can-dumper-eeprom

This project is a very simple sketch that dumps information from selected CAN bus messages to internal Arduino EEPROM memory

Here it listens for Audi cylinder retards values stored in messages

7E8 05 62 20 cyl ret1 ret2

where

cyl 
0x0A - cylinder 1
0x0B - cylinder 2
0x0C - cylinder 3
0x0D - cylinder 4

ret1 ret2 - negative hex number. To get angle in degrees, divide value by 100

Memory map
0-1000 - last 500 retards different from 0
1023-1024 - pointer to current saved value

Inspect EEPROM to get last dumps
