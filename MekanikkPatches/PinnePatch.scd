a = VTPinneRobot.new("/dev/tty.usbserial-FT5QT6JNA");
a.connected;
a.refresh;
a.leftMotor.directionEnum;
a.rightMotor.directionEnum;
a.rotationMotor.directionEnum.at(\left);


[0.0, 20.0].asSpec.constrain(23)

b = (hei: 223, hallo: 112)
a = IdentitySet.newFrom(b.keys);
a.do(_.postln)

a.add(\homo)
a.pop