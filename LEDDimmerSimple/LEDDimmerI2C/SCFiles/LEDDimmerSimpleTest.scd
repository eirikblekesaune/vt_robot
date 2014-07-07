p = SerialPort('/dev/tty.usbmodem411', 9600);
p.putAll([0x03, 0x00, 0x01, 0x02])