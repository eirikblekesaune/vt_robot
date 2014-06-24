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
~lok = VTLokomotiv.new(~coordinator, ~coordinator.getChildDeviceByName('TOG08').addressLo);
~controller = {arg ...args; "changed: %: %".format(args, args[0].perform(args[1].asSymbol)).postln};
~lok.addDependant(~controller);
)
~lok = ~lok8;
~lok = ~lok2;
~lok.set(\speed, 130)
~lok.set(\speed, 0)
~lok.set(\direction, 1);//right, position decrements (minus) backward
~lok.set(\direction, 0);//left, position increments (pluss) forward

~coordinator.sendATComand('NodeDiscover', "TOG08".ascii);
~coordinator.childDevices
~lok.set(\speed, 250);
~lok.set(\speed, 0);
~lok.set(\direction, 1);//right, position decrements (minus) backward
~lok.set(\direction, 0);//left, position increments (pluss) forward
~lok.set(\speed, 0);
~lok.get(\measuredSpeed); //10441
~lok.get(\distanceFromLastAddress);
~lok.get(\lastDetectedAddress);
~lok.set(\motorMode, 1);
~lok.set(\pidTargetSpeed, 8);
~lok.get(\pidTargetSpeed);
~lok.set(\pidPValue, 1.3);
~lok.set(\pidIValue, 0.3);
~lok.set(\pidDValue, 0.01);
PID: 1.3, 10.3, 0.01 //

(
w= Window.new;
~speedSlider = Slider.new(w,Rect(0, 0, 300, 50));
~speedSlider.action_({arg slid;
	~lok.set(\speed, slid.value().linlin(0.0, 1.0, 0, 511).asInteger);
});
~speedSlider.value_(0.0);

~stopButton = Button.new(w, Rect(0, 60, 100, 50));
~stopButton.states_([["STOP"]]);
~stopButton.action_({arg butt; ~lok.set(\speed, 0); defer{~speedSlider.value_(0.0);}});

~directionButton = Button.new(w, Rect(100, 60, 100, 50));
~directionButton.states_([["LEFT"], ["RIGHT"]]);
~directionButton.action_({arg butt; ~lok.set(\direction, butt.value);});

~modeButton = Button.new(w, Rect(200, 60, 100, 50));
~modeButton.states_([["MANUAL"], ["CRUISE"]]);
~modeButton.action_({arg butt; ~lok.set(\motorMode, butt.value);});
w.front;
)


~lok.set(\distancePollingInterval, 500);
~lok.set(\distancePollingInterval, 0);
~lok.set(\distanceFromLastAddress, 0);
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


