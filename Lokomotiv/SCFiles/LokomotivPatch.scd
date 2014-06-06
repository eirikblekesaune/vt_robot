SerialPort.devices
(
~coordinator = XBeeCoordinatorAPIMode(SerialPort('/dev/tty.usbserial-A600ezu2', 9600));
//~coordinator = XBeeCoordinatorAPIMode(SerialPort('/dev/tty.usbserial-13X2XBD8', 9600));
~coordinator.start;
// ~coordinator.rxAction_({arg byte; "Coordinator received: \t%\t%\t%".format(byte.asHexString(2), byte, byte.asAscii).postln;});
~coordinator.rxAction_(nil);
~coordinator.sendATCommand('NodeDiscover');
)
(
~lok2 = VTLokomotiv.new(~coordinator, ~coordinator.getChildDeviceByName('TOG02').addressLo);
~lok8 = VTLokomotiv.new(~coordinator, ~coordinator.getChildDeviceByName('TOG08').addressLo);
~controller = {arg ...args; "changed: %: %".format(args, args[0].perform(args[1].asSymbol)).postln};
~lok2.addDependant(~controller);
~lok8.addDependant(~controller);
)
~lok = ~lok8;
~lok = ~lok2;
~lok.set(\speed, 250)
~lok.set(\speed, 0)
~lok.set(\direction, 1);//right, position decrements (minus) backward
~lok.set(\direction, 0);//left, position increments (pluss) forward

~coordinator.sendATComand('NodeDiscover', "TOG08".ascii);
~coordinator.childDevices
~lok.set(\speed, 100);
~lok.set(\speed, 0);
~lok.set(\direction, 1);//right, position decrements (minus) backward
~lok.set(\direction, 0);//left, position increments (pluss) forward
~lok.set(\speed, 0);
~lok.get(\measuredSpeed); //10441
~lok.get(\distanceFromLastAddress);
~lok.get(\lastDetectedAddress);

//0 is the driection of the arduino chassis

~lok.set(\speed, 2.rand);

~lok.set(\stop, 4000);
~lok.set(\glideToSpeed, 3000);
~lok.set(\endSpeed, 0);

9216.asBinaryString(16)

2r10010000000000

~lok.set(\targetPosition, 123);
~lok.set(\distanceFromLastAddress, -2198798);
~lok.set(\led, 0);
~lok.set(\lastDetectedAddress, 14513457);
~lok.set(\pidPValue, -1.1345);
~lok.set(\pidIValue, 2.2);
~lok.set(\pidDValue, 3.3);
~lok.set(\stop, 0);


~lok.get(\speed);
~lok.get(\direction);
~lok.get(\targetPosition);
~lok.get(\distanceFromLastAddress);
~lok.get(\led);
~lok.get(\state);
~lok.get(\measuredSpeed); //10441
~lok.get(\lastDetectedAddress);
~lok.get(\pid);
~lok.get(\pidPValue);
~lok.get(\pidIValue);
~lok.get(\pidDValue);

~lok.removeDependant(~controller);
~lok.pid
~lok.speed;
~lok.direction;
~lok.targetPosition;
~lok.distanceFromLastAddress;
~lok.led;
~lok.state;
~lok.measuredSpeed;
~lok.lastDetectedAddress;

(
f = fork {512.do{arg i;
	0.5.wait;
	~lok.set(\led, {i}.value);
	}
};
)
f.stop;
~lok.set(\led, 120);
~lok.set(\led, 0);


