
(
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
	\state -> 2r0110
];
~setGetMasks = TwoWayIdentityDictionary[
	\set -> 2r00000000,
	\get -> 2r01000000
];
~buildMessage = {arg address, setGet, command, value;
	var msg;
	msg = [
		0x80,
		~addressMasks[address],
		~setGetMasks[setGet],
		~commandMasks[command],
	].reduce(\bitOr).asArray;
	if(setGet == \set and: {command != \stop}, {
		msg = msg ++ ~makeDataBytes.value(value);
	});
	msg;
};
~pinneParser = (
	parserStates: [\waitingForCommandByte, \waitingForDataByte],
	dataBytesReceived: 0,
	state: \waitingForCommandByte,
	currentCommand: \none,
	currentAddress: \none,
	currentSetGet: \none,
	valueBytes: Array.newClear(2),
	putByte: {arg ev, byte;
		if(byte.bitAnd(128) > 0, {//if first bit is 1
			var command, nextParserState, address, setGet;
			//byte is a command byte
			nextParserState = \waitingForDataByte;
			command = ~commandMasks.getID(byte.bitAnd(2r00001111));//keep the lower 4 bits, and get command symbol
			address = ~addressMasks.getID(byte.bitAnd(2r00110000));//get address symbol
			setGet = ~setGetMasks.getID(byte.bitAnd(2r01000000));//get set or get command
			switch(command,
				\stop, {
					"Received stop command".postln;
					nextParserState = \waitingForCommandByte;
					ev[\doCommand].value
				},
				\speed, {"Received speed command".postln},
				\direction, {"Received direction command".postln},
				\targetPosition, {"Received targetPosition command".postln},
				\currentPosition, {"Receive currentPosition command".postln;},
				\brake, {"Received brake command".postln;},
				\state, {"Receive state command".postln;}
			);
			ev[\state] = nextParserState;
			ev[\currentCommand] = command;
			ev[\currentAddress] = address;
			ev[\currentSetGet] = setGet;
			}, {
				"Data byte received".postln;
				switch(ev[\dataBytesReceived],
					0, { ev[\valueBytes][0] = byte; ev[\dataBytesReceived] = 1},
					1, {
						ev[\valueBytes][1] = byte;
						ev[\doCommand].value(ev);
					}
				)
		});
	},
	doCommand: {|ev|
		"doing command: %, address: %, setGet: %, value: %\n".format(
			ev[\currentCommand], ev[\currentCommand], ev[\currentSetGet], ~parseDataBytes.value(ev[\valueBytes])
		).postln;
		ev[\reset].value(ev);
	},
	reset: {|ev|
		ev.use {
			~state = \waitingForCommandByte;
			~currentCommand = \none;
			~currentSetGet = \none;
			~currentAddres = \none;
			~dataBytesReceived = 0;
			~valueBytes.fill(nil);
		}
	},
	dump: {arg ev;
		ev.postln;
	}
);
)

~pinneParser.putByte(2r10000001)
~pinneParser.putByte(2r10000001)

~pinneParser.reset


p.putAll(~buildMessage.value(\left, \set, \speed, 10));
p.putAll(~buildMessage.value(\left, \get, \speed));
p.putAll(~buildMessage.value(\right, \set, \speed, 100));
p.putAll(~buildMessage.value(\right, \get, \speed));
p.putAll(~buildMessage.value(\left, \set, \direction, 0));
p.putAll(~buildMessage.value(\left, \get, \direction));

fork{ (400..0).do {|i| p.putAll(~buildMessage.value(\right, \set, \speed, i)); 0.01.wait};}
fork{ (0..400).do {|i| p.putAll(~buildMessage.value(\right, \set, \speed, i)); 0.1.wait};}

~buildMessage.value(\left, \set, \stop)[0].asAscii

SerialPort.devices


Tdef(\listenToSerial).stop;p.close

p = SerialPort("/dev/tty.usbmodem1d11", 115200, crtscts:true,xonxoff:true);
(
Tdef(\listenToSerial, {
	loop{
		var byte;
		byte = p.read;
		byte.asAscii.post;
		~pinneParser.putByte(byte);
		//p.read.asBinaryString.postln;
	}
}).play
)
Tdef(\listenToSerial).clear

~addressMasks.at(\left)
~addressMasks[\left]
~addressMasks.getID(16)


~addressMasks = TwoWayIdentityDictionary[
	\left -> 2r00000000,
	\right -> 2r00010000,
	\rotation -> 2r00100000,
	\global -> 2r00110000
];
~commandMasks = (
	stop: 2r0000,
	speed: 2r0001,
	direction: 2r0010,
	targetPosition: 2r0011,
	currentPosition: 2r0100,
	brake: 2r0101,
	state: 2r0110
);
~setGetMasks = (
	set: 2r00000000,
	get: 2r01000000
);