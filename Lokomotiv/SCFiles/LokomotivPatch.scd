SerialPort.devices
(
~coordinator = XBeeCoordinatorAPIMode(SerialPort('/dev/tty.usbserial-A600ezu2', 115200));
//~coordinator = XBeeCoordinatorAPIMode(SerialPort('/dev/tty.usbserial-13X2XBD8', 9600));
~coordinator.start;
~coordinator.rxAction_({arg byte; "Coordinator received: \t%\t%\t%".format(byte.asHexString(2), byte, byte.asAscii).postln;});
~coordinator.rxAction_(nil);
~coordinator.sendATCommand('NodeDiscover');
)
(
~myTog = VTLokomotiv.new(~coordinator, ~coordinator.getChildDeviceByName('TOG08').addressLo);
~controller = {arg ...args; "changed: %: %".format(args, args[0].perform(args[1].asSymbol)).postln};
~myTog.addDependant(~controller);
)
~myTog = ~lok["TOG08"];
~myTog = ~myTog2;
~myTog.set(\speed, 130)
~myTog.set(\speed, 0)
~myTog.set(\direction, 1);//right, position decrements (minus) backward
~myTog.set(\direction, 0);//left, position increments (pluss) forward

~coordinator.sendATComand('NodeDiscover', "TOG08".ascii);
~coordinator.childDevices
~myTog.set(\speed, 250);
~myTog.set(\speed, 0);
~myTog.set(\direction, 1);//right, position decrements (minus) backward
~myTog.set(\direction, 0);//left, position increments (pluss) forward
~myTog.set(\speed, 0);
~myTog.get(\measuredSpeed); //10441
~myTog.get(\distanceFromLastAddress);
~myTog.get(\lastDetectedAddress);
~myTog.set(\motorMode, 1);
~myTog.set(\pidTargetSpeed, 8);
~myTog.get(\pidTargetSpeed);
~myTog.set(\pidPValue, 1.3);
~myTog.set(\pidIValue, 0.3);
~myTog.set(\pidDValue, 0.01);
PID: 1.3, 10.3, 0.01 //

(
w= Window.new;
~speedSlider = Slider.new(w,Rect(0, 0, 300, 50));
~speedSlider.action_({arg slid;
	~myTog.set(\speed, slid.value().linlin(0.0, 1.0, 0, 511).asInteger);
});
~speedSlider.value_(0.0);

~stopButton = Button.new(w, Rect(0, 60, 100, 50));
~stopButton.states_([["STOP"]]);
~stopButton.action_({arg butt; ~myTog.set(\speed, 0); defer{~speedSlider.value_(0.0);}});

~directionButton = Button.new(w, Rect(100, 60, 100, 50));
~directionButton.states_([["LEFT"], ["RIGHT"]]);
~directionButton.action_({arg butt; ~myTog.set(\direction, butt.value);});

~modeButton = Button.new(w, Rect(200, 60, 100, 50));
~modeButton.states_([["MANUAL"], ["CRUISE"]]);
~modeButton.action_({arg butt; ~myTog.set(\motorMode, butt.value);});
w.front;
)


~myTog.set(\distancePollingInterval, 500);
~myTog.set(\distancePollingInterval, 0);
~myTog.set(\distanceFromLastAddress, 0);
//0 is the driection of the arduino chassis

~myTog.set(\speed, 2.rand);

~myTog.set(\stop, 4000);
~myTog.set(\glideToSpeed, 3000);
~myTog.set(\endSpeed, 0);

9216.asBinaryString(16)

2r10010000000000

~myTog.set(\targetPosition, 123);
~myTog.set(\distanceFromLastAddress, -2198798);
~myTog.set(\led, 0);
~myTog.set(\lastDetectedAddress, 14513457);
~myTog.set(\pidPValue, -1.1345);
~myTog.set(\pidIValue, 2.2);
~myTog.set(\pidDValue, 3.3);
~myTog.set(\stop, 0);


~myTog.get(\speed);
~myTog.get(\direction);
~myTog.get(\targetPosition);
~myTog.get(\distanceFromLastAddress);
~myTog.get(\led);
~myTog.get(\state);
~myTog.get(\measuredSpeed); //10441
~myTog.get(\lastDetectedAddress);
~myTog.get(\pid);
~myTog.get(\pidPValue);
~myTog.get(\pidIValue);
~myTog.get(\pidDValue);

~myTog.removeDependant(~controller);
~myTog.pid
~myTog.speed;
~myTog.direction;
~myTog.targetPosition;
~myTog.distanceFromLastAddress;
~myTog.led;
~myTog.state;
~myTog.measuredSpeed;
~myTog.lastDetectedAddress;

(
f = fork {512.do{arg i;
	0.5.wait;
	~myTog.set(\led, {i}.value);
	}
};
)
f.stop;
~myTog.set(\led, 120);
~myTog.set(\led, 0);


