VTLokomotiv {
	var <pid;
	var <speed = 0;
	var <direction = 0;
	var <targetPosition;
	var <lastDetectedAddress;
	var <measuredSpeed;
	var <distanceFromLastAddress;
	var <led;
	var <state;
	var <xbeeDevice;
	var <mode;
	var bipolarSpeed;
	var <trackingPosition;
	var <trackingSpeed;
	var lastSpeedChange;

	var parserState, parseRoutine, currentCommand, currentValueBytes;
	var currentStringData;
	var dumpInput;
	var dumpAction;
	var autoStopRoutine;
	classvar <command;
	classvar <setGet;
	classvar <byteType;
	classvar <valueType;

	*new{arg xbeeDevice;
		^super.new.init(xbeeDevice);
	}

	init{arg xbeeDevice_;
		xbeeDevice = xbeeDevice_;
		xbeeDevice.rxAction_({arg data;
			//"Parsing incoming lokomotiv data".postln;
			data.do{arg item; this.prParseByte(item); };
		});
		pid = (P: 0.0, I: 0.0, D: 0.0);
		currentStringData = String.new(128);
		currentValueBytes = Array.new(4);
		autoStopRoutine = Routine({
			loop{
				if(speed != 0 and: {(Main.elapsedTime - lastSpeedChange) > 120.0}, {
					"AUTOSTOPPED: %".format(this.name).postln;
					this.speed_(0);
				});
				1.0.wait;
			};
		}).play;
		//Propagate online status from router
		SimpleController(xbeeDevice).put(\online, {
			this.changed(\online);
		});
		this.prResetParser;
		this.set(\motorMode, 1);
		this.get(\direction);
		this.get(\speed);
	}

	online{^xbeeDevice.online;}

	name{
		^xbeeDevice.nodeIdentifier;
	}

	speed_{arg val;
		if(val != speed, {
			speed = val.clip(0, 511);
			this.set(\speed, speed);
			lastSpeedChange = Main.elapsedTime;
			this.changed(\speed);
		});
	}

	direction_{arg val;
		direction = val.clip(0, 1);
		this.set(\direction, val);
		this.changed(\direction);
	}

	dumpInput_{arg bool;
		var oldBool;
		oldBool = dumpInput;
		dumpInput = bool.booleanValue;
		if(oldBool != dumpInput, {
			if(dumpInput, {
				dumpAction = {arg theChanger, theChanged, more;
					"VTLokomotiv: % %: %".format(theChanger.name, theChanged, theChanger.perform(theChanged)).postln;
				};
			});
			this.addDependant(dumpAction);
		}, {
				this.removeDependant(dumpAction);
		});
	}

	bipolarSpeed{
		if(direction == 0, {^speed;}, {^speed.neg});
	}

	bipolarSpeed_{arg val;
		if(val.isNegative and: {direction != 1}, {
			this.direction_(1);
		});
		if(val.isPositive and: {direction != 0}, {
			this.direction_(0);
		});
		this.speed_(val.abs);
		this.changed(\bipolarSpeed);
	}

	stop{arg val = 1000;
		//this.set(\stop, val);
		//speed = 0;
		//this.changed(\stop);
		this.bipolarSpeed_(0);
	}

	prResetParser{
		parserState = \waitingForCommandByte;
		currentCommand = nil;
	}

	prParseByte{arg byte;
		switch(parserState,
			\waitingForCommandByte, {
				if(byte.bitAnd(0xC0) == 0x80, //check if it is command byte of 'set' type
					{
						//"Got command byte: %".format(this.class.command.getID(byte.bitAnd(0x0F))).postln;
						currentCommand = this.class.command.getID(byte.bitAnd(0x1F));//only read 5 low bits
						if(currentCommand == \info,
							{ parserState = \waitingForStringByte; },
							{ parserState = \waitingForValueByte; });
					},
					{
						this.prResetParser;
					}
				);
			},
			\waitingForValueByte, {
				if(byte != 143, //equals endTransmission set command byte
					{
						//"Got value byte: %".format(byte).postln;
						//byte is value byte
						currentValueBytes = currentValueBytes.add(byte);
					},
					{
						//byte marks end of transmission
						//"Got endTransmission byte: %".format(byte).postln;
						this.prProcessIncomingCommand(currentValueBytes.copy);
						currentValueBytes = Array.new(4);//reset data values
					}
				);
			},
			\waitingForStringByte, {
				if(byte != 4, //equals end of transmission byte acc. to ASCII
					{
						//"Got string value byte: %".format(byte.asAscii).postln;
						currentStringData = currentStringData.add(byte.asAscii);
					},
					{
						//"Got ASCII End of transmission".postln;
						this.prProcessIncomingCommand(currentStringData.copy);
						currentStringData = String.new(128);
					}
				)
			},
			{this.prResetParser};
		);
	}

	*parseDataBytes{arg bytes;
		var result;
		//the actual type is "hidden" in bit 6 of the last recieved value byte. If that is 1 it is a decimal number
		result = bytes.reverse.collect({arg it, i; it << (i * 7)}).reduce(\bitOr);
		if(bytes.first.bitAnd(0x40).booleanValue, {
			result = (result / 10000.0).asFloat;
		});
		^result;
	}

	trackingData{
		^[trackingPosition, trackingSpeed];
	}

	//formats the received command and value bytes according to API
	prProcessIncomingCommand{arg dataBytes;
		var newData;
		switch(currentCommand,
			\trackingData, {
				//special function for parsing tracking data. High two words are speed, lo two words are pos
				//check if negative sign is set
				//0.002 is the reciprocal for fixed point packing factor on Arduino
				newData = this.class.parseDataBytes(dataBytes);
				// dataBytes.collect({arg it; it.asBinaryString}).postln;
				// newData.asBinaryString(32).postln;
				if(newData.bitAnd(0x80000000).booleanValue, {
					trackingSpeed = (newData >> 16).bitOr(0xFFFF0000) * 0.01;
				}, {
						trackingSpeed = (newData >> 16) * 0.01;
				});
				//check if negative sign is set
				if(newData.bitAnd(0x00008000).booleanValue, {
					trackingPosition = newData.bitOr(0xFFFF0000);
				}, {
						trackingPosition = newData.bitAnd(0x0000FFFF);
				});
				this.changed(\trackingData);
			},
			\speed, {speed = this.class.parseDataBytes(dataBytes); this.changed(\speed)},
			\stop, {this.changed(\stopped)},
			\direction, {direction = this.class.parseDataBytes(dataBytes); this.changed(\direction)},
			\targetPosition, {targetPosition = this.class.parseDataBytes(dataBytes); this.changed(\targetPosition)},
			\distanceFromLastAddress, {
				distanceFromLastAddress = this.class.parseDataBytes(dataBytes);
				this.changed(\distanceFromLastAddress)},
			\led, {led = this.class.parseDataBytes(dataBytes); this.changed(\led); },
			\state, {state = this.class.parseDataBytes(dataBytes); this.changed(\state);},
			\info, {"INFO[%]: %".format(this.name, dataBytes).postln;},
			\measuredSpeed, {measuredSpeed = this.class.parseDataBytes(dataBytes); this.changed(\measuredSpeed);},
			\lastDetectedAddress, {lastDetectedAddress = this.class.parseDataBytes(dataBytes); this.changed(\lastDetectedAddress);},
			\pidPValue, {pid.put('P', this.class.parseDataBytes(dataBytes)); this.changed(\pid);},
			\pidIValue, {pid.put('I', this.class.parseDataBytes(dataBytes)); this.changed(\pid);},
			\pidDValue, {pid.put('D', this.class.parseDataBytes(dataBytes)); this.changed(\pid);}
		);
		this.prResetParser;
	}

	*encodeValue{arg value;
		var result, valueToSend, typeCode;
		result = Array.new(5);
		//this method returns an array of bytes. The class of the value
		//determines the encoding type.
		if(value.isNumber, {
			if(value.isFloat, {
				valueToSend = (value.clip(-214748, 214748) * 10000).asInteger;
				typeCode = this.valueType[\decimal];
			});
			if(value.isInteger,{
				valueToSend = value;
				typeCode = this.valueType[\integer];
			});
			}, {
				^nil;
		});
		result = result.addAll(
			[
				((valueToSend & 0xF0000000) >> 28).bitAnd(0x0F).bitOr(typeCode),
				(valueToSend & 0x0FE00000) >> 21,
				(valueToSend & 0x001FC000) >> 14,
				(valueToSend & 0x00003F80) >> 7,
				(valueToSend & 0x0000007F)
			];
		);
		^result.as(Int8Array);
	}

	set{arg command, value;
		var msg = Array.new(7);
		msg = msg.add(
			this.class.command[command.asSymbol] ?? {"uknown command".error.postln; ^this;}|
			this.class.setGet[\set] |
			this.class.byteType[\command]
		);
		msg = msg.addAll(this.class.encodeValue(value));
		msg = msg.add(
			this.class.command[\endTransmission] |
			this.class.setGet[\set] |
			this.class.byteType[\command]
		);
		//msg = msg.as(Int8Array);
		this.prSend(msg);
		//"Sending mess: %".format(msg).postln;
		^msg;
	}

	get{arg command;
		//find command code and add command byte bit
		if(command == \pid, {
			fork{
				[\pidPValue, \pidIValue, \pidDValue].do({arg item;
					this.prSend(
						this.class.command[item] |
						this.class.setGet[\get] |
						this.class.byteType[\command]
					);
				});
				0.001.wait;
			};
			^this;
		});
		this.prSend(
			this.class.command[command.asSymbol] |
			this.class.setGet[\get] |
			this.class.byteType[\command]
		);
	}

	prSend{arg msg;
		//msg = msg.asString.ascii.as(Int8Array);
		xbeeDevice.sendTXData(msg);
	}

	*initClass{
		command = TwoWayIdentityDictionary[
			\stop -> 0x00,
			\speed -> 0x01,
			\direction -> 0x02,
			\targetPosition -> 0x03,
			\distanceFromLastAddress -> 0x04,
			\peripheral -> 0x05,
			\state -> 0x06,
			\info -> 0x07,
			\measuredSpeed -> 0x08,
			\lastDetectedAddress -> 0x09,
			\pidPValue -> 0x0A,
			\pidIValue -> 0x0B,
			\pidDValue -> 0x0C,
			\glideToSpeed -> 0x0D,
			\endSpeed -> 0x0E,
			\endTransmission -> 0x0F,
			\peripheralRequest -> 0x10,
			\motorMode -> 0x11,
			\trackingPollingInterval -> 0x12,//0 = off, n = polling interval in ms (min.val. 20ms)
			\pidTargetSpeed -> 0x13,
			\trackingData -> 0x14
		];
		setGet = TwoWayIdentityDictionary[
			\set -> 0x00,
			\get -> 0x40
		];
		byteType = TwoWayIdentityDictionary[
			\command -> 0x80,
			\value -> 0x00
		];
		valueType = TwoWayIdentityDictionary[
			\integer -> 0x00,
			\decimal -> 0x40
		];
	}
}
