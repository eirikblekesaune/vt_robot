SerialPort.devices.do(_.postln)
(
p = SerialPort("/dev/tty.usbserial-FT5QT6JNA", 9600, crtscts:true,xonxoff:true);
//p = SerialPort("/dev/tty.usbmodem411", 9600, crtscts:true,xonxoff:true);
Tdef(\listenToSerial, {
	loop{
		var byte;
		byte = p.read;
		//byte.post;
		~pinneParser.putByte(byte);
		//p.read.asBinaryString.postln;
	}
}).play;
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
	\maxPosition -> 2r1001
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
	\blockedByTopSensor -> 2r00000100,
	\blockedBySlackSensor -> 2r00000101,
	\blockedByMinPosition -> 2r00000110,
	\blockedByMaxPosition -> 2r00000111,
	\blockedByAbsMinPosition -> 2r00001000,
	\motorDriverFault -> 2r00001001

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
	parserStates: [
		\waitingForCommandByte,
		\waitingForDataByte,
		\waitingForStateByte,
		\waitingForInfoByte
	],
	dataBytesReceived: 0,
	state: \waitingForCommandByte,
	currentCommand: \none,
	currentAddress: \none,
	currentSetGet: \none,
	valueBytes: Array.newClear(2),
	infoBytes: Array.new,
	putByte: {arg ev, byte;
		if(byte.bitAnd(128) > 0 and: {ev[\state] == \waitingForCommandByte}, {//if first bit is 1
			var command, nextParserState, address, setGet;
			//byte is a command byte
			nextParserState = \waitingForDataByte;
			command = ~commandMasks.getID(byte.bitAnd(2r00001111));//keep the lower 4 bits, and get command symbol
			address = ~addressMasks.getID(byte.bitAnd(2r00110000));//get address symbol
			setGet = ~setGetMasks.getID(byte.bitAnd(2r01000000));//get set or get command
			switch(command,
				\stop, {
					//"Received stop command".postln;
					nextParserState = \waitingForCommandByte;
					ev[\doCommand].value
				},
				\stateChange, {
					//"Receive state change command".postln;
					nextParserState = \waitingForStateByte;
				},
				\speed, {"Received speed command".postln},
				\direction, {"Received direction command".postln},
				\targetPosition, {"Received targetPosition command".postln},
				\currentPosition, {"Receive currentPosition command".postln;},
				\brake, {"Received brake command".postln;},
				\minPosition, {"Receive min position command".postln;},
				\maxPosition, {"Receive max position command".postln;},
				\info, {
					nextParserState = \waitingForInfoByte;
				}
			);
			ev[\state] = nextParserState;
			ev[\currentCommand] = command;
			ev[\currentAddress] = address;
			ev[\currentSetGet] = setGet;
			}, {
				if(ev[\state] == \waitingForDataByte, {
					//"Data byte received".postln;
					switch(ev[\dataBytesReceived],
						0, {
							ev[\valueBytes][0] = byte;
							ev[\dataBytesReceived] = 1
						},
						1, {
							ev[\valueBytes][1] = byte;
							ev[\doCommand].value(ev);
						}
					)
				});
				if(ev[\state] == \waitingForStateByte, {
					//"State byte received".postln;
					ev[\valueBytes][0] = byte;
					ev[\doStateCommand].value(ev);
				});
				if(ev[\state] == \waitingForInfoByte, {
					if(byte != 4, {//4 is end of transimssion byte according to ASCII
						ev[\infoBytes] = ev[\infoBytes].add(byte);
					}, {
							"INFO: [%]: ".postf(ev[\currentAddress]);
							String.newFrom(ev[\infoBytes].collect(_.asAscii)).postln;
							ev[\infoBytes] = Array.new;
							ev[\reset].value(ev);
					});
				});
		});
	},
	doCommand: {|ev|
		"doing command: %, address: %, setGet: %, value: %\n".format(
			ev[\currentCommand], ev[\currentAddress], ev[\currentSetGet], ~parseDataBytes.value(ev[\valueBytes])
		).postln;
		ev[\reset].value(ev);
	},
	doStateCommand: {|ev|
		var state;
		state = ~stateChanges.getID(ev[\valueBytes][0]);
		"doing state command - address: % state: %\n".postf(ev[\currentAddress], state);
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


p.putAll(~buildMessage.value(\left, \set, \speed, 200));
p.putAll(~buildMessage.value(\left, \get, \speed));
p.putAll(~buildMessage.value(\left, \set, \stop));
p.putAll(~buildMessage.value(\left, \set, \minPosition, 100));
p.putAll(~buildMessage.value(\left, \set, \maxPosition, 400));
p.putAll(~buildMessage.value(\left, \get, \currentPosition));
p.putAll(~buildMessage.value(\left, \set, \targetPosition, 100));
p.putAll(~buildMessage.value(\left, \set, \targetPosition, 0));
p.putAll(~buildMessage.value(\left, \get, \targetPosition));
p.putAll(~buildMessage.value(\left, \set, \direction, 1));

p.putAll(~buildMessage.value(\rotation, \set, \speed, 250));
p.putAll(~buildMessage.value(\rotation, \set, \speed, 0));
p.putAll(~buildMessage.value(\rotation, \get, \speed));
p.putAll(~buildMessage.value(\rotation, \set, \direction, 1));
p.putAll(~buildMessage.value(\rotation, \set, \direction, 0));
p.putAll(~buildMessage.value(\rotation, \get, \direction));


p.putAll(~buildMessage.value(\right, \set, \speed, 220));
p.putAll(~buildMessage.value(\right, \get, \speed));
p.putAll(~buildMessage.value(\right, \set, \stop));
p.putAll(~buildMessage.value(\right, \set, \minPosition, 100));
p.putAll(~buildMessage.value(\right, \set, \maxPosition, 300));
p.putAll(~buildMessage.value(\right, \get, \currentPosition));


p.putAll(~buildMessage.value(\left, \set, \currentPosition, hh));
p.putAll(~buildMessage.value(\left, \get, \currentPosition));
p.putAll(~buildMessage.value(\right, \set, \currentPosition, jdjd));
p.putAll(~buildMessage.value(\right, \get, \currentPosition));

p.putAll(~buildMessage.value(\left, \set, \direction, 0));
p.putAll(~buildMessage.value(\left, \set, \direction, 1));
p.putAll(~buildMessage.value(\left, \get, \direction));
p.putAll(~buildMessage.value(\right, \set, \direction, 1));
p.putAll(~buildMessage.value(\right, \set, \direction, 0));
p.putAll(~buildMessage.value(\right, \get, \direction));

p.putAll(~buildMessage.value(\left, \set, \brake, 0));
p.putAll(~buildMessage.value(\left, \get, \brake));
p.putAll(~buildMessage.value(\right, \set, \brake, 0));
p.putAll(~buildMessage.value(\right, \get, \brake));

p.putAll(~buildMessage.value(\left, \set, \targetPosition, 11));
p.putAll(~buildMessage.value(\left, \get, \targetPosition));
p.putAll(~buildMessage.value(\right, \set, \targetPosition, 24));
p.putAll(~buildMessage.value(\right, \get, \targetPosition));

p.putAll(~buildMessage.value(\left, \set, \minPosition, 100));
p.putAll(~buildMessage.value(\left, \get, \minPosition));
p.putAll(~buildMessage.value(\right, \set, \minPosition, 322));
p.putAll(~buildMessage.value(\right, \get, \minPosition));

p.putAll(~buildMessage.value(\left, \set, \maxPosition, 400));
p.putAll(~buildMessage.value(\left, \get, \maxPosition));
p.putAll(~buildMessage.value(\right, \set, \maxPosition, 51));
p.putAll(~buildMessage.value(\right, \get, \maxPosition));


(0x09).asBinaryString
fork{ (400..0).do {|i| p.putAll(~buildMessage.value(\right, \set, \speed, i)); 0.01.wait};}
fork{ (0..400).do {|i| p.putAll(~buildMessage.value(\right, \set, \speed, i)); 0.1.wait};}

~buildMessage.value(\left, \set, \stop)[0].asAscii

SerialPort.devices


Tdef(\listenToSerial).stop;p.close


Tdef(\listenToSerial).clear


"L0240".ascii

16000000 / 1 / 2 / 800

s.boot
{SinOsc.ar(440)}.play