VTPinneRobot {
	var parser, <online = false;
	var <leftMotor, <rightMotor, <rotationMotor;

	*new{arg path;
		^super.new.init(path);
	}

	init{arg path_;
		this.connect(path_);
		if(this.connected.not, {"PinneRobot is offline".warn});
		leftMotor = VTPinneRobotMotor.new(this);
		rightMotor = VTPinneRobotMotor.new(this);
		rotationMotor = VTPinneRotationMotor.new(this);
	}

	connect{arg path;
		parser = VTPinneRobotParser(this, path);
	}

	connected{
		^parser.connected;
	}

	disconnect{
		parser.disconnect;
	}

	sendMsg{arg address, setGet, command, val;
		parser.sendMsg(address, setGet, command, val);
	}

	refresh{
		[leftMotor, rightMotor, rotationMotor].do(_.refresh);
	}

	update{arg theChanged, theChanger;

	}
}

VTPinneRobotMotor{
	var robot;
	var <speed;
	var <direction;
	var <targetPosition;
	var <currentPosition;
	var <state;
	var <specs;
	var <speedRampUp, <speedRampDown, <speedScaling, <stopTime;
	var <directionEnum;
	var <address;
	var needsRefresh;//parameters that need update

	*new{arg robot, address;
		^super.new.init(robot, address);
	}

	init{arg robot_, address_;
		directionEnum = TwoWayIdentityDictionary[\down -> 0, \up -> 1];
		robot = robot_;
		address = address_;
		specs = (
			speed: ControlSpec(0, 512,step:1,default:0),
			direction: ControlSpec(0, 1,step:1,default:0),
			targetPosition: ControlSpec(0, 1023, step: 1, units: 512),
			currentPositon: ControlSpec(0, 1023, step: 1,default:0),
			speedRampUp: ControlSpec(0, 5000, step: 1, default: 0),
			speedRampDown: ControlSpec(0, 5000, step: 1, default: 0),
			speedScaling: ControlSpec(0.0, 1.0, step: 0.001, default: 0),
			stop: ControlSpec(0, 5000, step:1, default:0)
		);
		needsRefresh = IdentitySet.newFrom(specs.keys);
	}

	speed_{arg val;
		speed = specs.at(\speed).constrain(val);
		robot.sendMsg(address, \set, \speed, speed);
		needsRefresh.add(\speed);
	}

	stop{arg val;
		stopTime = stopTime ? specs.at(\stop).constrain(val);
		robot.sendMsg(address, \set, \stop, speed);
		needsRefresh.add(\stop);
		needsRefresh.add(\speed);
	}

	goToParkingPosition{
		robot.sendMsg(address, \set, \goToParkingPostition, 0);
		needsRefresh.add(\speed);
		needsRefresh.add(\direction);
	}

	goToTargetPosition{arg duration;
		robot.sendMsg(
			address,
			\set,
			\goToTargetPostition,
			speedScaling.clip(0, 5000).asInteger
		);
		needsRefresh.add(\speed);
		needsRefresh.add(\direction);
	}

	speedRampUp_{arg val;
		speedRampUp = specs.at(\speedRampUp).constrain(val);
		robot.sendMsg(address, \set, \speedRampUp, speedRampUp);
		needsRefresh.add(\speedRampUp);
	}

	speedRampDown_{arg val;
		speedRampUp = specs.at(\speedRampDown).constrain(val);
		robot.sendMsg(address, \set, \speedRampDown, speedRampUp);
		needsRefresh.add(\speedRampDown);
	}

	speedScaling_{arg val;
		speedScaling = specs.at(\speedScaling).constrain(val);
		robot.sendMsg(address, \set, \speedScaling, speedScaling);
		needsRefresh.add(\goToTargetPosition);
	}

	refresh{arg forceRefresh;
		if(needsRefresh.notEmpty, {
			if(forceRefresh,
				{
					specs.keys.do({arg item;
						robot.sendMsg(address, \get, item.asSymbol);
					})
				},
				{
					needsRefresh.do({arg item;
						robot.sendMsg(address, \get, item.asSymbol);
					});
				}
			);
		});
	}

	update{arg theChanged, theChanger, what;

	}
}

VTPinneRotationMotor : VTPinneRobotMotor {
	*new{arg robot;
		^super.new(robot).init;
	}

	init{
		directionEnum = TwoWayIdentityDictionary[\right -> 0, \left -> 1];
	}
}


VTPinneRobotParser{
	var state = \waitingForCommandByte;
	var dataBytesReceived = 0;
	var robot;
	var <serialPort, readTask;
	var currentCommand;
	var currentAddress;
	var currentSetGet;
	var infoBytes;
	var valueBytes;

	classvar <addressMasks, <commandMasks, <setGetMasks, <stateChangeMasks;

	*initClass{
		addressMasks = TwoWayIdentityDictionary[
			\left -> 2r00000000,
			\right -> 2r00010000,
			\rotation -> 2r00100000,
			\global -> 2r00110000
		];
		commandMasks = TwoWayIdentityDictionary[
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
		setGetMasks = TwoWayIdentityDictionary[
			\set -> 2r00000000,
			\get -> 2r01000000
		];
		stateChangeMasks = TwoWayIdentityDictionary[
			\stopped -> 2r00000000,
			\goingDown -> 2r00000001,
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
	}

	*new{arg robot, path;
		^super.new.init(robot, path);
	}

	init{arg robot_, path_;
		robot = robot_;
		this.connect(path_);
		readTask = Task({
			loop{
				var byte;
				byte = serialPort.read;
				this.parseByte(byte);
			}
		}).play;
		serialPort.doneAction = {
			readTask.stop;
		};
	}

	reset{
		state = \waitingForCommandByte;
	}

	parseByte{arg byte;
		if(byte.bitAnd(128) > 0 and: {state == \waitingForCommandByte}, {//if first bit is 1
			var command, nextParserState, address, setGet;
			//byte is a command byte
			nextParserState = \waitingForDataByte;
			command = this.class.commandMasks.getID(byte.bitAnd(2r00001111));//keep the lower 4 bits, and get command symbol
			address = this.class.addressMasks.getID(byte.bitAnd(2r00110000));//get address symbol
			setGet = this.class.setGetMasks.getID(byte.bitAnd(2r01000000));//get set or get command
			switch(command,
				\stop, {
					//"Received stop command".postln;
					nextParserState = \waitingForCommandByte;
					this.doCommand;
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
			state = nextParserState;
			currentCommand = command;
			currentAddress = address;
			currentSetGet = setGet;
			}, {
				if(state == \waitingForDataByte, {
					switch(dataBytesReceived,
						0, {
							valueBytes.add(byte);
							dataBytesReceived = 1;
						},
						1, {
							valueBytes.add(byte);
							this.doCommand;
						}
					)
				});
				if(state == \waitingForStateByte, {
					//"State byte received".postln;
					valueBytes.put(0, byte);
					this.doStateCommand;
				});
				if(state == \waitingForInfoByte, {
					if(byte != 4, {//4 is end of transmission byte according to ASCII
						infoBytes = infoBytes.add(byte);
						}, {
							"INFO: [%]: ".postf(currentAddress);
							String.newFrom(infoBytes).collect(_.asAscii).postln;
							infoBytes = Array.new;
							this.reset;
					});
				});
		});
	}

	parseCommand{
		"doing command: %, address: %, setGet: %, value: %\n".format(
			currentCommand, currentAddress, currentSetGet, this.class.parseDataBytes.value(valueBytes)
		).postln;
		this.reset;
	}

	prBuildMessage{ arg address, setGet, command, value;
		var msg;
		msg = [
			128,//2r10000000 signifies a command byte
			this.class.addressMasks[address],
			this.class.setGetMasks[setGet],
			this.class.commandMasks[command],
		].reduce(\bitOr).asArray;
		if(setGet == \set and: {command != \stop}, {
			msg = msg ++ this.class.makeDataBytes.value(value);
		});
		^msg;
	}

	sendMsg{arg addr, setGet, command, value;
		serialPort.putAll(this.prBuildMessage(addr, setGet, command, value));
	}

	connect{arg path;
		this.disconnect;
		serialPort = SerialPort.new(path, 9600, crtscts:true);
	}

	disconnect{
		if(serialPort.notNil, { serialPort.close; });
	}

	connected{//should ping robot device instead
		^serialPort.isOpen;
	}

	*makeDataBytes{arg value;
		^[value.rightShift(7).bitAnd(127), value.bitAnd(127)];
	}

	*parseDataBytes{arg bytes;
		^bytes.at(0).leftShift(7).bitOr(bytes.at(1));
	}
}