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

//map pens buttons
8.do{arg i;
	var midiComp, pens;
	midiComp = ~togCtrl.components.at(("button." ++ (i + 1)).asSymbol);
	pens = ~pens.penses[i];
	midiComp.addDependant(pens);
	pens.addDependant(midiComp);
};
//map tog to faders
6.do{arg i;
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
//Wireless LED fader
~xbee = XBeeAPI(SerialPort('/dev/tty.usbserial-A600ezu2', 9600));
~xbee.rxAction = {|serialAddr, networkAddr, receiveOptions, data|
	[serialAddr, networkAddr, receiveOptions, data].postln;
};

~xbee.start;

~xbeeRadio = (
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

~togCtrl.components.at('fader.8').action_({arg comp;
	var val;
	val = comp.value.linlin(0, 1023, 0, 250).asInteger;
	~xbeeRadio.setLEDValue(val);
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

w = Window.new.front;
w.onClose_({0.exit});