SerialPort.devices;
a = VTPinneRobot.new("/dev/tty.Bluetooth-Serial-1");
a.connected;
a.refresh;


a.leftMotor.speed_(100);
a.rightMotor.directionEnum;
a.rotationMotor.directionEnum


b = IdentitySet[\thukk, \tjusg];
b.pop
b.do(_.postln)