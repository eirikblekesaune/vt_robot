//#!/Applications/SuperCollider/SuperCollider.app/Contents/Resources/sclang
///CMD + a  - > select all
///CMD + ENTER -> run code

MIDIClient.init;
MIDIIn.connectAll;
//MIDIFunc.trace(false)
//~flyingCtrl = MIDIController.new("BCF2000B", "Port 1", "BCF2000B", "Port 1");
~togCtrlA = MIDIController.new("TogA", "BCF2000", "TogA", "BCF2000");
~togCtrlB = MIDIController.new("TogB", "Port 1", "TogB", "Port 1");
[~togCtrlA, ~togCtrlB].do({arg ctrl;
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
try{ "~/vt_robot/MekanikkPatches/Lokomotiver.scd".standardizePath.load; } {"Error in Lokotiver".postln;};

"~/vt_robot/MekanikkPatches/Lys.scd".standardizePath.load;

//////////////////////////////////////////////////
//Send OSC to penser
//////////////////////////////////////////////////

7.do{arg i;
	var number, pensNumber;
	number = i + 1;
	pensNumber = Dictionary[
		1 -> 1,
		2 -> 2,
		3 -> 3,
		4 -> 4,
		5 -> 5,
		6 -> 8,
		7 -> 7
	].at(number);
	[~togCtrlA, ~togCtrlB].do{arg ctrl;
		ctrl.components["button.%".format(number).asSymbol].action_({arg comp;
			var val = comp.value.booleanValue.asInteger;
			"pens: % val: %".format(number, val).postln;
			NetAddr("1.2.3.75", 57120).sendMsg("/robot/pens.%/position".format(pensNumber).asSymbol, val);
		});
	};
};


//Kranser remote
~roofMac = NetAddr("1.2.3.54", 57120);
~setKransSpeed = {arg kransNumber, speed;
	~roofMac.sendMsg("/robot/krans.%/speed".format(kransNumber).asSymbol, speed);
};
~kransFaders = Dictionary.new;
~kransStopButtons = Dictionary.new;

[1,2,3].do{arg number;
	var speedCtrlComp, stopCtrlComp, faderSpec;
	speedCtrlComp = Dictionary[1 -> 'fader.7', 2 -> 'fader.8', 3 -> 'rotaryA.8'].at(number);
	stopCtrlComp = Dictionary[1 -> 'button.15', 2 -> 'button.16', 3 -> 'rotaryButtonA.8'].at(number);
	faderSpec = Dictionary[1 -> [0, 1023].asSpec, 2 -> [0, 1023].asSpec, 3 -> [0, 127].asSpec].at(number);
	~kransFaders.put(number,
		~togCtrlA.components.at(speedCtrlComp);
	);
	~kransStopButtons.put(number,
		~togCtrlA.components.at(stopCtrlComp);
	);
	~kransFaders[number].action = {arg ctrlComp;
		var val;
		val = ctrlComp.value.linlin(faderSpec.minval, faderSpec.maxval, -1.0, 1.0);
		"Krans % - bipolarSpeed: %".format(number, val).postln;
		~setKransSpeed.value(number, val);
	};
	~kransStopButtons[number].action = {arg ctrlComp;
		"Krans % stopp".format(number).postln;
		~setKransSpeed.value(number, 0);
		~kransFaders[number].value_((faderSpec.maxval - faderSpec.minval / 2).asInteger).refresh;
	};
	OSCdef("kransResponer%".format(number).asSymbol, {arg msg, time, addr, port;
		~setKransSpeed.value(number, msg[1]);
	},"/robot/krans.%/bipolarSpeed".format(number).asSymbol);
};