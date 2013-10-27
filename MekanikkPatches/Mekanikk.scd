//#!/Applications/SuperCollider/SuperCollider.app/Contents/Resources/sclang
MIDIClient.init;
MIDIIn.connectAll;
~flyingCtrl = MIDIController.new("BCF2000", "BCF2000", "BCF2000", "BCF2000");
~togCtrl = MIDIController.new("BCF2000", "Port 1", "BCF2000", "Port 1");
[~flyingCtrl, ~togCtrl].do({arg ctrl;
	ctrl.setMappings(
		(
			fader: [number: (0..7), chan: 0, msgType: \control14].flop.collect(Event.newFrom(_)),
			rotaryA: [number: (80..87), chan: 0].flop.collect(Event.newFrom(_)),
			rotaryB: [number: (8..15), chan: 0].flop.collect(Event.newFrom(_)),
			rotaryC: [number: (16..23), chan: 0].flop.collect(Event.newFrom(_)),
			rotaryD: [number: (24..31), chan: 0].flop.collect(Event.newFrom(_)),
			rotaryButtonA: [number: (40..47), chan: 0].flop.collect(Event.newFrom(_)),
			rotaryButtonB: [number: (48..55), chan: 0].flop.collect(Event.newFrom(_)),
			rotaryButtonC: [number: (56..63), chan: 0].flop.collect(Event.newFrom(_)),
			rotaryButtonD: [number: (64..71), chan: 0].flop.collect(Event.newFrom(_)),
			button: [number: (72..79) ++ (88..95) ++ (96..99), chan: 0].flop.collect(Event.newFrom(_))
		)
	);
});
~pens = VTPenseBox("/dev/tty.usbserial-FT3EET3MA");
~tog = VTMonsterBox("/dev/tty.usbserial-FT3EET3MB");
~pinner = VTMonsterBox("/dev/tty.usbserial-FT3EET44A");
~lanbox = SerialPort.new("/dev/tty.usbmodem641", 38400, crtscts: true);

//map pens buttons
8.do{arg i;
	var midiComp, pens;
	midiComp = ~togCtrl.components.at(("button." ++ (i + 1)).asSymbol);
	pens = ~pens.penses[i];
	midiComp.addDependant(pens);
	pens.addDependant(midiComp);
};

//~togCtrl.components['rotaryA.1']
6.do{arg i;
	var midiComp, dmx;
	midiComp = ~togCtrl.components.at((("rotaryA.") ++ (i + 1)).asSymbol);
	midiComp.postln;
	midiComp.action_({arg comp;
		var val, commandString;
		val = comp.value.linlin(0, 127, 0, 255).asInteger.asHexString(2);
		commandString = ("*C901" ++ (i + 13).asHexString(2) ++ val ++ "#").ascii;
		~lanbox.putAll(commandString);
		"dmx % : %\n".postf((i + 13), val);
	}.inEnvir);
};
//map tog to faders
7.do{arg i;
	var fader, tog, button;
	fader = ~togCtrl.components.at(("fader." ++ (i + 1)).asSymbol);
	tog = ~tog.channels[i];
	fader.addDependant(tog);
	tog.addDependant(fader);

	button = ~togCtrl.components.at(("button." ++ (i + 9)).asSymbol);
	button.action = {arg butt;
		if(butt.value != 0, {
			tog.stop;
			tog.changed(butt, \value);
		});
	};
	tog.stop;
	fader.valueAction_(512);
	fader.refresh;
};
//map faders to pinner
8.do{arg i;
	var fader, tog, button;
	fader = ~flyingCtrl.components.at(("fader." ++ (i + 1)).asSymbol);
	tog = ~pinner.channels[i];
	fader.addDependant(tog);
	tog.addDependant(fader);

	button = ~flyingCtrl.components.at(("button." ++ (i + 9)).asSymbol);
	button.action = {arg butt;
		if(butt.value != 0, {
			tog.stop;
			tog.changed(butt, \value);
		});
	};
};
~flyingCtrl.components['button.20'].action_({arg comp;
	8.do{arg i;
		~pinner.channels[i].stop;
		~pinner.channels[i].changed(comp, \value);
	};
	"global koppstopp".postln;
});
//Wireless LED fader
~xbee = XBeeAPI(SerialPort('/dev/tty.usbserial-A600ezu2', 9600));
~xbee.rxAction = {|serialAddr, networkAddr, receiveOptions, data|
	[serialAddr, networkAddr, receiveOptions, data].postln;
};

~xbee.start;

~togLED1 = (
	xbee: ~xbee,
	address: [0x00, 0x13, 0xA2, 0x00, 0x40, 0x90, 0x29, 0xEA],//TO
	networkAddress: [0xFF, 0xFE],
	theTask: Task.new({}),
	runUpdateTask: {arg ev;
		ev.use{
			if(~theTask.isPlaying.not, {
				~theTask = Task.new({
					loop {
						//~setLEDValue.value(~address, ~networkAddress, ~ledValue);
						if(~ledValue != ~lastTransmittedValue, {
							~xbee.sendTXFrame(~address, ~networkAddress, ["ms".ascii, ~ledValue, 13, 10].flatten, 1);
							"sending: %\n".postf([~address, ~networkAddress, ~ledValue]);
							~lastTransmittedValue = ~ledValue;
						});
						0.05.wait;
					};
				}.inEnvir).play;
				SystemClock.sched(0.5, {
					if(~theTask.isPlaying, {
						~stopUpdateTask.value(currentEnvironment);
					});
				}.inEnvir);
			});
		}
	},
	stopUpdateTask: {arg ev;
		ev.use{
			~theTask.stop;
		}
	},
	ledValue: 0,
	lastTransmittedValue: 0,
	setLEDValue: {arg ev, val;
		ev[\ledValue] = val.clip(0, 250);
		ev[\runUpdateTask].value(ev);
	}
);


~togLED2 = (
	xbee: ~xbee,
	address: [0x00, 0x13, 0xA2, 0x00, 0x40, 0x8C, 0xCB, 0x3D],//TRE
	networkAddress: [0x0A, 0x28],
	theTask: Task.new({}),
	runUpdateTask: {arg ev;
		ev.use{
			if(~theTask.isPlaying.not, {
				~theTask = Task.new({
					loop {
						//~setLEDValue.value(~address, ~networkAddress, ~ledValue);
						if(~ledValue != ~lastTransmittedValue, {
							~xbee.sendTXFrame(~address, ~networkAddress, ["ms".ascii, ~ledValue, 13, 10].flatten, 1);
							"sending: %\n".postf([~address, ~networkAddress, ~ledValue]);
							~lastTransmittedValue = ~ledValue;
						});
						0.05.wait;
					};
				}.inEnvir).play;
				SystemClock.sched(0.5, {
					if(~theTask.isPlaying, {
						~stopUpdateTask.value(currentEnvironment);
					});
				}.inEnvir);
			});
		}
	},
	stopUpdateTask: {arg ev;
		ev.use{
			~theTask.stop;
		}
	},
	ledValue: 0,
	lastTransmittedValue: 0,
	setLEDValue: {arg ev, val;
		ev[\ledValue] = val.clip(0, 250);
		ev[\runUpdateTask].value(ev);
	}
);

~togLED3 = (
	xbee: ~xbee,
	//	address: [0x00, 0x13, 0xA2, 0x00, 0x40, 0x90, 0x29, 0xEA],//TO
	address: [0x00, 0x13, 0xA2, 0x00, 0x40, 0x90, 0x29, 0x1B],//FEM
	networkAddress: [0xFF, 0xFE],
	theTask: Task.new({}),
	runUpdateTask: {arg ev;
		ev.use{
			if(~theTask.isPlaying.not, {
				~theTask = Task.new({
					loop {
						//~setLEDValue.value(~address, ~networkAddress, ~ledValue);
						if(~ledValue != ~lastTransmittedValue, {
							~xbee.sendTXFrame(~address, ~networkAddress, ["ms".ascii, ~ledValue, 13, 10].flatten, 1);
							"sending: %\n".postf([~address, ~networkAddress, ~ledValue]);
							~lastTransmittedValue = ~ledValue;
						});
						0.05.wait;
					};
				}.inEnvir).play;
				SystemClock.sched(0.5, {
					if(~theTask.isPlaying, {
						~stopUpdateTask.value(currentEnvironment);
					});
				}.inEnvir);
			});
		}
	},
	stopUpdateTask: {arg ev;
		ev.use{
			~theTask.stop;
		}
	},
	ledValue: 0,
	lastTransmittedValue: 0,
	setLEDValue: {arg ev, val;
		ev[\ledValue] = val.clip(0, 250);
		ev[\runUpdateTask].value(ev);
	}
);


~togCtrl.components.at('fader.8').action_({arg comp;
	var val;
	val = comp.value.linlin(0, 1023, 0, 250).asInteger;
	~togLED1.setLEDValue(val);
});
~togCtrl.components.at('rotaryA.7').action_({arg comp;
	var val;
	val = comp.value.linlin(0, 1023, 0, 250).asInteger;
	~togLED2.setLEDValue(val);
});
~togCtrl.components.at('rotaryA.8').action_({arg comp;
	var val;
	val = comp.value.linlin(0, 127, 0, 250).asInteger;
	~togLED3.setLEDValue(val);
});

//Fuglen
~fuglAddr = NetAddr("1.2.3.112", 50004);
~flyingCtrl.components['rotaryA.8'].action_({arg comp;
	var myval, mydir;
	myval = comp.value.linlin(0, 127, -5, 5).asInteger;
	mydir = if(myval.isNegative, {"down"}, {"up"});
	["fugl", mydir, myval.abs.asInteger].postln;
	~fuglAddr.sendMsg("/robot/fugl/direction", mydir);
	~fuglAddr.sendMsg("/robot/fugl/speed", myval.abs);
});
~flyingCtrl.components['rotaryButtonA.8'].action_({arg comp;
	~fuglAddr.sendMsg("/robot/fugl/speed", 0);
	~flyingCtrl.components['rotaryA.8'].valueAction_(64).refresh;
});
OSCFunc({arg msg, time, addr, port;
	var myval, mydir;
	myval = msg[1].linlin(-512, 512, -5, 5).asInteger;
	mydir = if(myval.isNegative, {"down"}, {"up"});
	["fuglOSC", mydir, myval.abs.asInteger].postln;
	~fuglAddr.sendMsg("/robot/fugl/direction", mydir);
	~fuglAddr.sendMsg("/robot/fugl/speed", myval.abs);
}, '/robot/fugl.1/speed');


//OSC repsonders for flying
6.do{arg i;
	OSCFunc({arg msg, time, addr, resp;
		~pinner.channels[i].valueAction_(msg[1].clip(-512, 512).asInteger);
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/speed").asSymbol);
	OSCFunc({arg msg, time, addr, resp;
		~pinner.channels[i].stop;
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/stop").asSymbol);
};
OSCFunc({arg msg, time, addr, resp;
	~pinner.channels.do(_.stop);
}.inEnvir, '/robot/flying/stop');

~lysResponders = 256.collect({arg i;
	var num = i + 1;
	OSCFunc({arg msg, time, addr, port;
		var val, commandString;

		val = msg[1].clip(0, 255).asInteger.asHexString(2);
		commandString = ("*C901" ++ num.asHexString(2) ++ val ++ "#").ascii;
		~lanbox.putAll(commandString);
		"dmx % : %\n".postf(num, val);
	}, ("/light/dmx." ++ (i + 1)).asSymbol);
});
~pinnerne = [
	SerialPort("/dev/tty.usbserial-FT3EET44B", 9600, crtscts:true,xonxoff:true),// pinne en
	SerialPort("/dev/tty.usbserial-FT3EET4IA", 9600, crtscts:true,xonxoff:true),// pinne to
	SerialPort("/dev/tty.usbserial-FT3EET4IB", 9600, crtscts:true,xonxoff:true),// pinne tre
	SerialPort("/dev/tty.usbserial-FT3EET52A", 9600, crtscts:true,xonxoff:true)// pinne fire
];

~makeDataBytes = {|val|
	[(val >> 7) & 0x7F, val & 0x7F];
};
~parseDataBytes = {|bytes|
	(bytes[0] << 7).bitOr(bytes[1]);
};
~addressMasks = TwoWayIdentityDictionary[
	\left -> 2r00000000,
	\right -> 2r00010000,
	\rotation -> 2r00100000,
	\global -> 2r00110000
];
~commandMasks = TwoWayIdentityDictionary[
	\stop -> 2r0000,
	\speed -> 2r0001,
	\direction -> 2r0010,
	\targetPosition -> 2r0011,
	\currentPosition -> 2r0100,
	\brake -> 2r0101,
	\stateChange -> 2r0110,
	\info -> 2r0111,
	\minPosition -> 2r1000,
	\maxPosition -> 2r1001,
	\goToParkingPosition -> 2r1010,
	\goToTargetPosition -> 2r1011,
	\goToSpeedRampUp -> 2r1100,//argument is ramp up percent of halfway point
	\goToSpeedRampDown -> 2r1101,//ramp down time will take effect after halfway point
	\goToSpeedScaling -> 2r1110
];
~setGetMasks = TwoWayIdentityDictionary[
	\set -> 2r00000000,
	\get -> 2r01000000
];
~stateChanges = TwoWayIdentityDictionary[
	\stopped ->   2r00000000,
	\goingDown ->   2r00000001,
	\goingUp -> 2r00000010,
	\stoppedAtTarget -> 2r00000011,
	\goingToTarget -> 2r00000100,
	\blockedByTopSensor -> 2r00000101,
	\blockedBySlackSensor -> 2r00000110,
	\blockedByMinPosition -> 2r00000111,
	\blockedByMaxPosition -> 2r00001000,
	\blockedByAbsMinPosition -> 2r00001001,
	\motorDriverFault -> 2r00001010
];
~buildMessage = {arg address, setGet, command, value;
	var msg;
	msg = [
		0x80,
		~addressMasks[address],
		~setGetMasks[setGet],
		~commandMasks[command],
	].reduce(\bitOr).asArray;
	if(setGet == \set, {
		msg = msg ++ ~makeDataBytes.value(value);
	});
	msg;
};
OSCdef(\pinneResponder, {arg msg, time, addr, port;
	var index, motor, val, key;
	index = msg[1].asInteger - 1;
	motor = msg[2].asSymbol;
	key = msg[3].asSymbol;
	val = msg[4].asInteger;
	"pinne: num: % motor: % key: % val: %\n".postf(index, motor, key, val);
	~pinnerne[index].putAll(~buildMessage.value(motor, \set, key, val));
}, '/robot/pinne');

OSCdef(\pinneParking, {arg msg, time, addr, port;
	~pinnerne.do({arg item;
		item.putAll(~buildMessage.value(\global, \set, \goToParkingPosition, 0));
	});
}, '/robot/pinne/park');

//w = Window.new.front;
//w.onClose_({0.exit});