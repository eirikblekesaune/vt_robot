MIDIClient.init;
MIDIIn.connectAll;

// ~bcfMappings = IdentityDictionary.new;
// (81..88).do({arg item, i; ~bcfMappings.put(item.asSymbol, "fader.%".format(i+1).asSymbol); });
// (1..8).do({arg item, i; ~bcfMappings.put(item.asSymbol, "rotary.%".format(i+1).asSymbol); });
// (73..80).do({arg item, i; ~bcfMappings.put(item.asSymbol, "button.%/A".format(i+1).asSymbol); });
// (65..72).do({arg item, i; ~bcfMappings.put(item.asSymbol, "button.%/B".format(i+1).asSymbol); });
// (33..40).do({arg item, i; ~bcfMappings.put(item.asSymbol, "button.%/C".format(i+1).asSymbol); });

~nanoControlMappings = IdentityDictionary.new;
((2..6) ++ [8,9,12,13]).do({arg item, i;
	~nanoControlMappings.put(item.asSymbol, "fader.%".format(i+1).postln;)});
(14..22).do({arg item, i;
	~nanoControlMappings.put(item.asSymbol, "rotary.%".format(i+1).postln;)});


~sendActorData = {arg addr, val;
	NetAddr("1.2.3.44", 1234).sendMsg(addr, val);
	NetAddr("1.2.3.201", 50000).sendMsg(addr, val);
	NetAddr("1.2.3.111", 50000).sendMsg(addr, val);
	NetAddr("1.2.3.57", 57120).sendMsg(addr, val);
};
try{
	["Ali"].do{arg name;
		try{
			MIDIdef.cc(name.toLower.asSymbol, {arg val, num, chan, src;
				var addr, outVal;
				outVal = val.linlin(0, 127, 0.0, 1.0);
				addr = "/%/control/%".format(name.toLower, ~nanoControlMappings.at(num.asSymbol)).asSymbol;
				"%: % \n %: %".format(name, [num, val, chan, src], addr, outVal).postln;
				~sendActorData.value(addr, outVal);
			}, srcID: MIDIIn.findPort("nanoKONTROL", "SLIDER/KNOB").uid);
		} {arg err; "% BCF2000 is not connected!".format(name).error.throw;}
	};
	"Ali control ready".postln;
}{"Error starting actor control patch".error.postln;};

/////////////////////////////////////////////////
///Ali to fugl.1
/////////////////////////////////////////////////
~aliCtrlFuglSpeed = 0;
MIDIdef.cc(\aliCtrlFugl1, {arg val, num, chan, src;
	var bipolarSpeed, direction;
	bipolarSpeed = val.linlin(0, 127, -5, 5).asInteger;
	if(bipolarSpeed != ~aliCtrlFuglSpeed, {
		~aliCtrlFuglSpeed = bipolarSpeed;
		if(bipolarSpeed.isNegative, {
			direction = "down";
			}, {
				direction = "up";
		});
		NetAddr("1.2.3.112", 50004).sendMsg('/robot/fugl/direction', direction);
		NetAddr("1.2.3.112", 50004).sendMsg('/robot/fugl/speed', bipolarSpeed.abs);
	});
}, srcID: MIDIIn.findPort("nanoKONTROL", "SLIDER/KNOB").uid, ccNum: 14/*rotary.1*/);



/////////////////////////////////////////////////
///Ali to hvit fugl
/////////////////////////////////////////////////
~aliCtrlData = (
	hvitFugl: (liftSpeed: 0, headPosition: 0);
);
MIDIdef.cc(\aliCtrlFuglLift, {arg val, num, chan, src;
	var bipolarSpeed, direction;
	var currentSpeed;
    currentSpeed = ~aliCtrlData[\hvitFugl][\liftSpeed];
	bipolarSpeed = val.linlin(0, 127, -1,1);
    "Hvit fugl lift: %".format(bipolarSpeed).postln;
	if(bipolarSpeed != currentSpeed, {
		var flaate = NetAddr("1.2.3.75", 57120);
		~aliCtrlData[\hvitFugl][\liftSpeed] = bipolarSpeed;
		if(bipolarSpeed.isNegative, {
			direction = "down";
			}, {
				direction = "up";
		});
		flaate.sendMsg('/robot/hvitFugl/lift/direction', direction);
		flaate.sendMsg('/robot/hvitFugl/lift/speed', bipolarSpeed.abs);
	});
}, srcID: MIDIIn.findPort("nanoKONTROL", "SLIDER/KNOB").uid, ccNum: 15/*rotary.2*/);

MIDIdef.cc(\aliCtrlFuglHead, {arg val, num, chan, src;
	var headPosition, direction;
	var currentPosition;
 	currentPosition = ~aliCtrlData[\hvitFugl][\headPosition];
	headPosition = val.linlin(0, 127, -1,1);
    "Hvit fugl head: %".format(headPosition).postln;
	if(headPosition != currentPosition, {
		var flaate = NetAddr("1.2.3.75", 57120);
		~aliCtrlData[\hvitFugl][\headPosition] = headPosition;
		flaate.sendMsg('/robot/hvitFugl/head/position', headPosition);
	});
}, srcID: MIDIIn.findPort("nanoKONTROL", "SLIDER/KNOB").uid, ccNum: 16/*rotary.3*/);

////////////////////////////////////////////
//Ali to kreket
////////////////////////////////////////////
~aliCtrlData.put(\kreket, (bipolarSpeed: 0));
MIDIdef.cc(\aliCtrlKreket, {arg val, num, chan, src;
	var bipolarSpeed, direction;
	var currentSpeed;
	currentSpeed = ~aliCtrlData[\kreket][\bipolarSpeed];
	bipolarSpeed = val.linlin(0, 127, -1,1);
	if(bipolarSpeed != currentSpeed, {
		var flaate = NetAddr("1.2.3.51", 57120);
		~aliCtrlData[\kreket][\bipolarSpeed] = bipolarSpeed;
		//"Kreket - Sending: %".format(bipolarSpeed).postln;
		flaate.sendMsg('/robot/tallerken.4/speed', bipolarSpeed);
	});
}, srcID: MIDIIn.findPort("nanoKONTROL", "SLIDER/KNOB").uid, ccNum: 17/*rotary.4*/);

