~pinner = [
	VTPinneRobot("/dev/tty.usbserial-FT5QT6TQA"),// pinne en
	VTPinneRobot("/dev/tty.usbserial-FT5QT6TQB"),// pinne to
	VTPinneRobot("/dev/tty.usbserial-FT5QT6TZA"),// pinne tre
	VTPinneRobot("/dev/tty.usbserial-FT5QT6TZB"),// pinne fire
	VTPinneRobot("/dev/tty.usbserial-FT5QT6UDA"),// pinne fem
	VTPinneRobot("/dev/tty.usbserial-FT5QT6UDB"),// pinne seks
	VTPinneRobot("/dev/tty.usbserial-FT5QT6ULA"),// pinne syv
	VTPinneRobot("/dev/tty.usbserial-FT5QT6ULB")// pinne aatte
];

OSCdef(\pinneClassResponder, {arg msg, time, addr, port;
	var index, motor, val, key, pinne, pinneMotor;
	index = msg[1].asInteger - 1;
	motor = msg[2].asSymbol;
	key = msg[3].asSymbol;
	val = if(key != \direction, {msg[4].asInteger}, {msg[4]});
	pinne = ~pinner[index];
	"pinne class: num: % motor: % key: % val: %\n".postf(index, motor, key, val);

	pinneMotor = switch(motor,
		\left, pinne.leftMotor,
		\right, pinne.rightMotor,
		\rotation, pinne.rotationMotor
	);
	"MOTOR: %".format([pinneMotor, key, val]).postln;
	pinneMotor.perform(key.asSetter, val);
}.inEnvir, '/robot/pinne');

OSCdef(\pinneClassStopper, {arg msg, time, addr, port;
	~pinner.do({arg item;
		item.stop;
	});
}, '/robot/pinne/stop');

OSCdef(\getPinnePositions, {arg msg, time, addr, port;
	var packet = ();
	~pinner.do({arg item, i;
		var result = ();
		result.put(\left, item.leftMotor.currentPosition);
		result.put(\right, item.rightMotor.currentPosition);
		result.put(\rotation, item.rotationMotor.currentPosition);
		packet.put((i + 1).asSymbol, result);
	});
	NetAddr("1.2.3.111", 50010).sendMsg('/pinneData', JSON.stringify(packet));
}, '/robot/getPinneData');