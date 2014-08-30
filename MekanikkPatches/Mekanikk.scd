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
"~/vt_robot/MekanikkPatches/HvitFugl.scd".standardizePath.load;
"~/vt_robot/MekanikkPatches/Lys.scd".standardizePath.load;
"~/vt_robot/MekanikkPatches/Pinner.scd".standardizePath.load;
//"~/vt_robot/MekanikkPatches/Kopper.scd".standardizePath.load;
"~/vt_robot/MekanikkPatches/Fugl.scd".standardizePath.load;
"~/vt_robot/MekanikkPatches/Kranser.scd".standardizePath.load;

//////////////////////////////////////////////////
//Send OSC to penser
//////////////////////////////////////////////////

8.do{arg i;
	var number;
	number = i + 1;
	[~togCtrlA, ~togCtrlB].do{arg ctrl;
		ctrl.components["button.%".format(number).asSymbol].action_({arg comp;
			var val = comp.value.booleanValue.asInteger;
			NetAddr("1.2.3.75", 57120).sendMsg("/robot/pens.%/position".format(number).asSymbol, val);
		});
	};
};
