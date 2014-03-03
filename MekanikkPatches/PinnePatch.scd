SerialPort.devices;
p = SerialPort("/dev/tty.usbserial-FT5QT6JNA", 9600, crtscts:true);
p.put($r)
p.close;
a = VTPinneRobot.new("/dev/tty.usbserial-FT5QT6JNA");
a.connected;
a.refresh;


a.leftMotor.speed_(100);
a.rightMotor.directionEnum;
a.rotationMotor.directionEnum


b = IdentitySet[\thukk, \tjusg];
b.pop
b.do(_.postln)