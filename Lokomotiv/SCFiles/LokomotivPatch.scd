(
~port = SerialPort('/dev/tty.usbmodem411');
~lok = VTLokomotiv.new(~port, "eirik");
~controller = {arg ...args; "changed: %: %".format(args, args[0].perform(args[1].asSymbol)).postln};
~lok.addDependant(~controller);
)
~lok.set(\speed, 0);
~lok.set(\direction, 2.rand);
~lok.set(\speed, 2.rand);

~lok.set(\stop, 3000);
~lok.set(\glideToSpeed, 3000);
~lok.set(\endSpeed, 0);

9216.asBinaryString(16)

2r10010000000000

~lok.set(\targetPosition, 123);
~lok.set(\distanceFromLastAddress, -2198798);
~lok.set(\led, 0);
~lok.set(\measuredSpeed, -9423);
~lok.set(\lastDetectedAddress, 14513457);
~lok.set(\pidPValue, -1.1345);
~lok.set(\pidIValue, 2.2);
~lok.set(\pidDValue, 3.3);

~lok.get(\speed);
~lok.get(\direction);
~lok.get(\targetPosition);
~lok.get(\distanceFromLastAddress);
~lok.get(\led);
~lok.get(\state);
~lok.get(\measuredSpeed);
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