SerialPort.devices.do(_.postln)

~servoCity = SerialPort("/dev/tty.usbmodem411", 9600);

Tdef(\servoCityReader, {
	loop{ ~servoCity.read.postln; }
}.inEnvir)

