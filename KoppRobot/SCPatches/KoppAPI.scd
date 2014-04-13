SerialPort.devices.do(_.postln)
(
p = SerialPort("/dev/tty.usbserial-FT5QT6JNA", 9600, crtscts:true,xonxoff:true);
//p = SerialPort("/dev/tty.usbserial-FTCVPEA5", 9600, crtscts:true,xonxoff:true);
Tdef(\listenToSerial, {
	loop{
		var byte;
		byte = p.read;
		//byte.post;
		~koppParser.putByte(byte);
		//p.read.asBinaryString.postln;
	}
}).play;
~makeDataBytes = {|val|
	[(val >> 7) & 0x7F, val & 0x7F];
};
~parseDataBytes = {|bytes|
	(bytes[0] << 7).bitOr(bytes[1]);
};
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
~buildMessage = {arg setGet, command, value;
	var msg;
	msg = [
		0x80,
		~setGetMasks[setGet],
		~commandMasks[command],
	].reduce(\bitOr).asArray;
	if(setGet == \set, {
		msg = msg ++ ~makeDataBytes.value(value);
	});
	msg;
};
~koppParser = (
	parserStates: [
		\waitingForCommandByte,
		\waitingForDataByte,
		\waitingForStateByte,
		\waitingForInfoByte
	],
	dataBytesReceived: 0,
	state: \waitingForCommandByte,
	currentCommand: \none,
	currentSetGet: \none,
	valueBytes: Array.newClear(2),
	infoBytes: Array.new,
	putByte: {arg ev, byte;
		if(byte.bitAnd(128) > 0 and: {ev[\state] == \waitingForCommandByte}, {//if first bit is 1
			var command, nextParserState, setGet;
			//byte is a command byte
			nextParserState = \waitingForDataByte;
			command = ~commandMasks.getID(byte.bitAnd(2r00001111));//keep the lower 4 bits, and get command symbol
			setGet = ~setGetMasks.getID(byte.bitAnd(2r01000000));//get set or get command
			switch(command,
				\stop, {"Received stop command".postln},
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
							"INFO: ".post;
							String.newFrom(ev[\infoBytes].collect(_.asAscii)).postln;
							ev[\infoBytes] = Array.new;
							ev[\reset].value(ev);
					});
				});
		});
	},
	doCommand: {|ev|
		"doing command: %, setGet: %, value: %\n".format(
			ev[\currentCommand], ev[\currentSetGet], ~parseDataBytes.value(ev[\valueBytes])
		).postln;
		ev[\reset].value(ev);
	},
	doStateCommand: {|ev|
		var state;
		state = ~stateChanges.getID(ev[\valueBytes][0]);
		"doing state command - state: %\n".postf(state);
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
p.putAll(~buildMessage.value(\get, \direction));
p.putAll(~buildMessage.value(\set, \direction, 0));
p.putAll(~buildMessage.value(\get, \speed));
p.putAll(~buildMessage.value(\set, \speed, 0));
p.putAll(~buildMessage.value(\set, \speed, 300));
p.putAll(~buildMessage.value(\get, \currentPosition));
p.putAll(~buildMessage.value(\set, \currentPosition, 1024));
p.putAll(~buildMessage.value(\set, \goToTargetPosition, 0));
p.putAll(~buildMessage.value(\set, \goToParkingPosition, 0));


(
Tdef(\pos, {
	loop{
		p.putAll(~buildMessage.value(\get, \currentPosition));
		1.0.wait;
	};
}).play;
)
Tdef(\pos).stop;