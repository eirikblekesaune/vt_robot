//#!/Applications/SuperCollider/SuperCollider.app/Contents/Resources/sclang
MIDIClient.init;
MIDIIn.connectAll;
//~flyingCtrl = MIDIController.new("BCF2000B", "Port 1", "BCF2000B", "Port 1");
~togCtrl = MIDIController.new("BCF2000A", "BCF2000", "BCF2000A", "BCF2000");
[~togCtrl].do({arg ctrl;
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
~koppMonster = VTMonsterBox("/dev/tty.usbserial-FT3EET44A");
~lanbox = SerialPort.new("/dev/tty.usbmodem471", 38400, crtscts: true);

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
/*//map faders to kopper
8.do{arg i;
var fader, tog, button;
fader = ~flyingCtrl.components.at(("fader." ++ (i + 1)).asSymbol);
tog = ~koppMonster.channels[i];
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
~koppMonster.channels[i].stop;
~koppMonster.channels[i].changed(comp, \value);
};
"global koppstopp".postln;
});*/

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


OSCdef(\flashTog1,{arg msg, time, addr, port;
	~xbee.sendTXFrame(~togLED1[\address], ~togLED1[\networkAddress], ["ms".ascii, 250, 13, 10].flatten, 1);
	~xbee.sendTXFrame(~togLED1[\address], ~togLED1[\networkAddress], ["ms".ascii, 0, 13, 10].flatten, 1);

}, '/robot/tog.1/flash');

OSCdef(\flashTog2,{
	~xbee.sendTXFrame(~togLED2[\address], ~togLED2[\networkAddress], ["ms".ascii, 250, 13, 10].flatten, 1);
	~xbee.sendTXFrame(~togLED2[\address], ~togLED2[\networkAddress], ["ms".ascii, 0, 13, 10].flatten, 1);
}, '/robot/tog.2/flash');

OSCdef(\flashTog2,{
	~xbee.sendTXFrame(~togLED3[\address], ~togLED3[\networkAddress], ["ms".ascii, 250, 13, 10].flatten, 1);
	~xbee.sendTXFrame(~togLED3[\address], ~togLED3[\networkAddress], ["ms".ascii, 0, 13, 10].flatten, 1);
}, '/robot/tog.3/flash');

OSCdef(\flashTog4,{
	~xbee.sendTXFrame(~togLED4[\address], ~togLED4[\networkAddress], ["ms".ascii, 250, 13, 10].flatten, 1);
	~xbee.sendTXFrame(~togLED4[\address], ~togLED4[\networkAddress], ["ms".ascii, 0, 13, 10].flatten, 1);
}, '/robot/tog.4/flash');


//Fuglen
~fuglAddr = NetAddr("1.2.3.112", 50004);
// ~flyingCtrl.components['rotaryA.8'].action_({arg comp;
// 	var myval, mydir;
// 	myval = comp.value.linlin(0, 127, -5, 5).asInteger;
// 	mydir = if(myval.isNegative, {"down"}, {"up"});
// 	["fugl", mydir, myval.abs.asInteger].postln;
// 	~fuglAddr.sendMsg("/robot/fugl/direction", mydir);
// 	~fuglAddr.sendMsg("/robot/fugl/speed", myval.abs);
// });
// ~flyingCtrl.components['rotaryButtonA.8'].action_({arg comp;
// 	~fuglAddr.sendMsg("/robot/fugl/speed", 0);
// 	~flyingCtrl.components['rotaryA.8'].valueAction_(64).refresh;
// });
OSCFunc({arg msg, time, addr, port;
	var myval, mydir;
	myval = msg[1].linlin(-512, 512, -5, 5).asInteger;
	mydir = if(myval.isNegative, {"down"}, {"up"});
	["fuglOSC", mydir, myval.abs.asInteger].postln;
	~fuglAddr.sendMsg("/robot/fugl/direction", mydir);
	~fuglAddr.sendMsg("/robot/fugl/speed", myval.abs);
}, '/robot/fugl.1/speed');


//OSC repsonders for flying (LD)
// 6.do{arg i;
// 	OSCFunc({arg msg, time, addr, resp;
// 		~koppMonster.channels[i].valueAction_(msg[1].clip(-512, 512).asInteger);
// 	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/speed").asSymbol);
// 	OSCFunc({arg msg, time, addr, resp;
// 		~koppMonster.channels[i].stop;
// 	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/stop").asSymbol);
// };
// OSCFunc({arg msg, time, addr, resp;
// 	~koppMonster.channels.do(_.stop);
// }.inEnvir, '/robot/flying/stop');



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

/*
~pinnerne = [
SerialPort("/dev/tty.usbserial-FT5QT6TQA", 9600, crtscts:true),// pinne en
SerialPort("/dev/tty.usbserial-FT5QT6TQB", 9600, crtscts:true),// pinne to
SerialPort("/dev/tty.usbserial-FT5QT6TZA", 9600, crtscts:true),// pinne tre
SerialPort("/dev/tty.usbserial-FT5QT6TZB", 9600, crtscts:true),// pinne fire
SerialPort("/dev/tty.usbserial-FT5QT6UDA", 9600, crtscts:true),// pinne fem
SerialPort("/dev/tty.usbserial-FT5QT6UDB", 9600, crtscts:true),// pinne seks
SerialPort("/dev/tty.usbserial-FT5QT6ULA", 9600, crtscts:true),// pinne syv
SerialPort("/dev/tty.usbserial-FT5QT6ULB", 9600, crtscts:true)// pinne aatte
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
item.putAll(~buildMessage.value(\right, \set, \goToParkingPosition, 0));
item.putAll(~buildMessage.value(\left, \set, \goToParkingPosition, 0));
});
}, '/robot/pinne/park');
OSCdef(\pinneStopper, {arg msg, time, addr, port;
~pinnerne.do({arg item;
item.putAll(~buildMessage.value(\global, \set, \stop, 0));
});
}, '/robot/pinne/stop');
fork{
[
(
rotation: (minPosition: 200, maxPosition: 800),
left: (minPosition: 0, maxPosition: 690),
right: (minPosition: 0, maxPosition: 690)//traad maa forlenges
),
(
rotation: (minPosition: 200, maxPosition: 800),
left: (minPosition: 0, maxPosition: 590),
right: (minPosition: 0, maxPosition: 590)
),
(
rotation: (minPosition: 367, maxPosition: 576),
left: (minPosition: 0, maxPosition: 580),
right: (minPosition: 0, maxPosition: 580)
),
(
rotation: (minPosition: 200, maxPosition: 800),
left: (minPosition: 0, maxPosition: 650),
right: (minPosition: 0, maxPosition: 650)
),
(
rotation: (minPosition: 485, maxPosition: 550),
left: (minPosition: 0, maxPosition: 600),
right: (minPosition: 0, maxPosition: 600)//noe rart med rot pot feste her
),
(
rotation: (minPosition: 406, maxPosition: 722),
left: (minPosition: 0, maxPosition: 600),
right: (minPosition: 0, maxPosition: 600)//noe rart med rot pot feste her
),
(
rotation: (minPosition: 300, maxPosition: 700),//rot pot funker ikke
left: (minPosition: 0, maxPosition: 600),
right: (minPosition: 0, maxPosition: 600)
),
(
rotation: (minPosition: 300, maxPosition: 700),
left: (minPosition: 0, maxPosition: 600),
right: (minPosition: 0, maxPosition: 600)
)
].do({arg item, i;
item.keysValuesDo({arg address, data;
data.keysValueDo({arg settingKey, settingData;
~pinnerne[i].putAll(~buildMessage.value(address, \set, settingKey, settingData));//\left, \set, \goToParkingPosition, 0
0.01.wait;
})
})
});
};
*/
(
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

~kopper = [
	VTKoppRobot("/dev/tty.usbserial-FT5QT6UUA"), //9 //OK kobling
	VTKoppRobot("/dev/tty.usbserial-FT5QT6UUB"), //10 // svarer ikke
	VTKoppRobot("/dev/tty.usbserial-FT5QT6VAA"), //11 // ok svarer
	VTKoppRobot("/dev/tty.usbserial-FT5QT6VAB"), //12 // ok svarer
	VTKoppRobot("/dev/tty.usbserial-FT5QT6VKA"), //13
	VTKoppRobot("/dev/tty.usbserial-FT5QT6VKB") //14 OK koblet til kopp nr 3
];

[0, 1, 2, 3, 5].do{arg i;
	OSCFunc({arg msg, time, addr, resp;
		var dir;
		dir = msg[1].isNegative.not.asInt;
		"kopper: % speed: %, dir: %".format(i + 1, msg, dir).postln;
		~kopper[i].sendMsg(\set, \direction, dir);
		~kopper[i].sendMsg(\set, \speed, msg[1].clip(-512, 512).asInteger.abs);
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/speed").asSymbol);
	OSCFunc({arg msg, time, addr, resp;
		~kopper[i].stop;
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/stop").asSymbol);
};
[0, 1, 2, 3, 5].do{arg i;
	OSCFunc({arg msg, time, addr, resp;
		~kopper[i].sendMsg(\set, \goToParkingPosition, 0);
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/goToParkingPosition").asSymbol);
};
[0, 1, 2, 3, 5].do{arg i;
	OSCFunc({arg msg, time, addr, resp;
		~kopper[i].sendMsg(\set, \targetPosition, msg[1]);
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/targetPosition").asSymbol);
};
OSCFunc({arg msg, time, addr, resp;
	~kopper.do(_.stop);
}.inEnvir, '/robot/flying/stop');

~koppWalkStart = {
	[0,1,2,3,5].do({arg koppIndex;
	var koppNum = koppIndex + 1;
	var doNextTarget = {
		var taskKey = "koppWalk%".format(koppNum).asSymbol;
		Tdef(taskKey).clear;
		Tdef(taskKey, {
				var waitTime = rrand(5.0, 20.0);
				var newPos = rrand(250, 600);
				var speedRamp = Env([0, rrand(80, 150), 40], [5, rrand(1, 10)]).asPseg.asStream;
				"setting new target kopp %: %".format(koppNum, newPos).postln;
				"going in % seconds".format(waitTime).postln;
				waitTime.wait;
				~kopper[koppIndex].sendMsg(\set, \targetPosition, newPos);
				loop{
					var newSpeed = speedRamp.next;
					newSpeed !? { ~kopper[koppIndex].sendMsg(\set, \speed, newSpeed.asInteger); };
					0.1.wait;
				};
			}.inEnvir).play;
	};
	~kopper[koppIndex].stateChangeAction_({arg newState;
		if(newState == \stoppedAtTarget, {
			doNextTarget.value;
		});
	});
	doNextTarget.value;
});
};
~koppWalkStop = {
	~kopper.do({arg item, i;
		var taskKey = "koppWalk%".format(i + 1).asSymbol;
		Tdef(taskKey).clear;
		item.stateChangeAction_(nil);
		item.sendMsg(\set, \speed, 0);
	});
};
OSCdef(\koppWalkResponder, {arg msg, time, addr, port;
	if(msg[1] == 1, {
		"Started kopp walk".postln;
		~koppWalkStart.value;
	});
	if(msg[1] == 0, {
		"Stopped kopp walk".postln;
		~koppWalkStop.value;
	});
}, '/robot/flying/walk');

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
);

//Tallerken 1
~tallerken1 = SerialPort('/dev/tty.usbserial-FT5QT6W2A', 2400, crtscts:true);
~tallerkenLookup = [(122..97), (65..90)].flat;
~tallerkenSpec = ControlSpec(
	(~tallerkenLookup.size / 2).neg,
	(~tallerkenLookup.size / 2), step: 1, default: 0);
~tallerkenSlider = Slider();
~setTallerken1Speed = {arg val;
	var msg;
	val = val.linlin(-1.0, 1.0, 0, ~tallerkenLookup.size).asInteger;
	//"val: %[%]".format(val, val.class).postln;
	msg = "1%".format(
		~tallerkenLookup[val.clip(0, ~tallerkenLookup.size)].asAscii
	);
	~tallerken1.putAll(msg);
	//"msg: %".format(msg).postln;
};
OSCdef(\tallerken1, {arg msg, time, addr, port;
	//"Tallerken 1 speed: %".format(msg[1]).postln;
	~setTallerken1Speed.value(msg[1].clip(-1.0, 1.0));
}, '/robot/tallerken.1/speed');
Tdef(\tallerkenListen, {
	loop{
		"tallerken input: %".format(~tallerken1.read.asAscii).postln;
	}
}).play;
//w = Window.new.front;
//w.onClose_({0.exit});