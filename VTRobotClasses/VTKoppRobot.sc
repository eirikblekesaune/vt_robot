VTKoppRobotMotor{
	var robot;
	var <speed = 0;
	var <direction = \down; //symbol up or down
	var <targetPosition = 0;
	var <currentPosition = 0;
	var <minPosition = 0;
	var <maxPosition = 4096;
	var <state = \goingDown;
	var <speedRampUp = 0;
	var <speedRampDown = 0;
	var <speedScaling = 0;
	var <stopTime = 0;
	var <specs;
	var <directionEnum;
	var needsRefresh;//parameters that need to be sent to the robot
	var outputCache;//parameter values to be sent to the robot
	var needsUpdate;//parameter values to be update from the robot

	*new{arg robot;
		^super.new.init(robot);
	}

	init{arg robot_;
		robot = robot_;
		directionEnum = TwoWayIdentityDictionary[\down -> 0, \up -> 1];
		specs = (
			speed: ControlSpec(0, 512,step:1,default:0),
			targetPosition: ControlSpec(0, 4096, step: 1),
			currentPosition: ControlSpec(0, 4096, step: 1),
			minPosition: ControlSpec(0, 4096, step: 1),
			maxPosition: ControlSpec(0, 4096, step: 1),
			speedRampUp: ControlSpec(0, 5000, step: 1, default: 0),
			speedRampDown: ControlSpec(0, 5000, step: 1, default: 0),
			speedScaling: ControlSpec(0.0, 1.0, step: 0.001, default: 0),
			stopTime: ControlSpec(0, 5000, step:1, default:0),
			goToTargetPosition: ControlSpec(0, 5000, \lin, 1, default: 1000)
		);
		needsRefresh = IdentitySet.new;
		needsUpdate = IdentitySet.newFrom(specs.keys);
		outputCache = IdentityDictionary.new;
	}

	prInvalidateParameter{arg key, val;
		//		"Invalidating parameter: % %".format(key, val).postln;
		outputCache.put(key, val);
		needsRefresh.add(key);
	}

	speed_{arg val, sync = true;
		speed = specs.at(\speed).constrain(val);
		if(sync, { this.prInvalidateParameter(\speed, speed)});
	}

	direction_{arg newDir, sync = true;
		if(directionEnum.at(newDir).isNil,
			{"Not a recognized direction".error.postln; ^this;}
		);
		direction = newDir;
		if(sync, { this.prInvalidateParameter(\direction, directionEnum.at(direction))});
	}

	targetPosition_{arg val, sync = true;
		targetPosition = specs.at(\targetPosition).constrain(val);
		if(sync, { this.prInvalidateParameter(\targetPosition, targetPosition)});
	}

	currentPosition_{arg val, sync = true;
		currentPosition = specs.at(\currentPosition).constrain(val);
		if(sync, { this.prInvalidateParameter(\currentPosition, currentPosition)});
	}

	minPosition_{arg val, sync = true;
		minPosition = specs.at(\minPosition).constrain(val);
		if(sync, { this.prInvalidateParameter(\minPosition, minPosition)});
	}

	maxPosition_{arg val, sync = true;
		maxPosition = specs.at(\maxPosition).constrain(val);
		if(sync, { this.prInvalidateParameter(\maxPosition, maxPosition)});
	}

	stop{arg val = 0, sync = true;
		stopTime = specs.at(\stopTime).constrain(val);
		if(sync, { this.prInvalidateParameter(\stop, stopTime.asInteger)});
	}

	goToParkingPosition{
		this.prInvalidateParameter(\goToParkingPosition, 0);
	}

	goToParkingPosition_{//temp hack
		this.prInvalidateParameter(\goToParkingPosition, 0);
	}

	goToTargetPosition{arg duration;
		duration = duration ? specs.at(\goToTargetPosition).constrain(duration);
		this.prInvalidateParameter(\goToTargetPosition, duration);
	}

	goToTargetPosition_{arg duration;
		duration = duration ? specs.at(\goToTargetPosition).constrain(duration);
		this.prInvalidateParameter(\goToTargetPosition, duration);
	}

	speedRampUp_{arg val;
		speedRampUp = specs.at(\speedRampUp).constrain(val);
		this.prInvalidateParameter(\speedRampUp, speedRampUp);
	}

	speedRampDown_{arg val;
		speedRampDown = specs.at(\speedRampDown).constrain(val);
		this.prInvalidateParameter(\speedRampDown, speedRampDown);
	}

	speedScaling_{arg val;
		speedScaling = specs.at(\speedScaling).constrain(val);
		this.prInvalidateParameter(\speedScaling, speedScaling);
	}

	state_{arg newState, sync = true;
		if(newState != state, {
			state = newState;
			this.changed(\state);
		});
	}

	refresh{arg forceRefresh = false;
		var cachedValue, cachedKey;
		cachedKey = needsRefresh.pop;
		while({cachedKey.notNil}, {
			robot.sendMsg(\set, cachedKey, outputCache.removeAt(cachedKey));
			cachedKey = needsRefresh.pop;
		});
	}

	requestValuesFromRobot{arg which;
		if(which.isNil, {
			//"reqeusting all".postln;
			specs.keys.do({arg item;
				//robot.sendMsg(\get, item);
			});
			}, {
				//"Requesting: %\n".postf(which);
				which.do({arg item;
					robot.sendMsg(\get, item, 0);
				})
		});
	}

	//this is a push style udpate function. The changed message must send the new value as an argument
	//
	update{arg theChanged, theChanger, what, value;
		"Motor:Update: $".format(theChanged, theChanger, what).postln;
		if(theChanger !== this, {
			this.setParameter(what, value, sync: false);
		});
	}
}

VTKoppRobotParser{
	var state = \waitingForCommandByte;
	var dataBytesReceived = 0;
	var robot;
	var <serialPort, readTask, serialPortLock;
	var currentCommand;
	var currentSetGet;
	var infoBytes;
	var valueBytes;
	var valueBytesBuffer;

	classvar <commandMasks, <setGetMasks, <stateChangeMasks;

	*initClass{
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
			\goToTargetPosition -> 2r1011,//0x0B
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
		serialPortLock = Semaphore(1);
		this.connect(path_);
		this.addDependant(robot);
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
		valueBytes = nil;
		state = \waitingForCommandByte;
	}

	parseByte{arg byte;
		//		"Praser state: %".format([state, byte]).postln;
		if(byte.bitAnd(128) > 0 and: {state == \waitingForCommandByte}, {//if first bit is 1
			var command, nextParserState, setGet;
			//byte is a command byte
			dataBytesReceived = 0;
			valueBytesBuffer = nil;
			nextParserState = \waitingForDataByte;
			command = this.class.commandMasks.getID(byte.bitAnd(2r00001111));//keep the lower 4 bits, and get command symbol
			setGet = this.class.setGetMasks.getID(byte.bitAnd(2r01000000));//get set or get command
			switch(command,
				\stop, {"Received stop command".postln},
				\stateChange, {
					//"Receive state change command".postln;
					nextParserState = \waitingForStateByte;
				},
				\speed, {/*"Received speed command".postln*/},
				\direction, {"Received direction command".postln},
				\targetPosition, {"Received targetPosition command".postln},
				\currentPosition, {/*"Receive currentPosition command".postln;*/},
				\brake, {"Received brake command".postln;},
				\minPosition, {"Receive min position command".postln;},
				\maxPosition, {"Receive max position command".postln;},
				\info, {
					nextParserState = \waitingForInfoByte;
				}
			);
			state = nextParserState;
			currentCommand = command;
			currentSetGet = setGet;
			}, {
				if(state == \waitingForDataByte, {
					switch(dataBytesReceived,
						0, {
							valueBytesBuffer = valueBytesBuffer.add(byte);
							dataBytesReceived = 1;
						},
						1, {
							valueBytes = valueBytesBuffer.add(byte).copy;
							this.doCommand;
						}
					)
				});
				if(state == \waitingForStateByte, {
					//"State byte received".postln;
					this.doStateCommand(byte);
				});
				if(state == \waitingForInfoByte, {
					if(byte != 4, {//4 is end of transmission byte according to ASCII
						infoBytes = infoBytes.add(byte);
						}, {
							"INFO: kopp : ".postln;
							String.newFrom(infoBytes).collect(_.asAscii);
							infoBytes = Array.new;
							this.reset;
					});
				});
		});
	}

	doCommand{
		var value;
		value = this.class.parseDataBytes(valueBytes);
		robot.update(robot, this, currentCommand, value);
		this.reset;
	}

	doStateCommand{arg newState;
		//		"State change: %".format(this.class.stateChangeMasks.getID(newState)).postln;
		robot.update(robot, this, \state, this.class.stateChangeMasks.getID(newState));
		this.reset;
	}

	prBuildMessage{ arg setGet, command, value;
		var msg;
		msg = [
			128,//2r10000000 signifies a command byte
			this.class.setGetMasks[setGet],
			this.class.commandMasks[command],
		].reduce(\bitOr).asArray;
		if(setGet == \set, {
			msg = msg ++ this.class.makeDataBytes(value);
		});
		^msg;
	}

	sendMsg{arg setGet, command, value;
//		"Sending message: %".format([setGet, command, value]).postln;
		forkIfNeeded{
			serialPortLock.wait;
			serialPort.putAll(this.prBuildMessage(setGet, command, value));
			serialPortLock.signal;
		};
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
		value = value.asInteger;
		^[value.rightShift(7).bitAnd(127), value.bitAnd(127)];
	}

	*parseDataBytes{arg bytes;
		^bytes.at(0).leftShift(7).bitOr(bytes.at(1));
	}
}


VTKoppRobot {
	var parser, <online = false;
	var <motor;
	var refreshTask, <>refreshInterval = 0.01;//refresh is when cached values are sent to the robot
	var updateTask, <>updateInterval = 1.0;//update is when values are requested from the robot
	var semaphore;
	var <>stateChangeAction;

	*new{arg path;
		^super.new.init(path);
	}

	init{arg path_;
		motor = VTKoppRobotMotor.new(this);
		this.connect(path_);
		if(this.connected.not, {"KoppRobot is offline".warn});
		refreshTask = Task({
			loop{
				this.refresh;
				refreshInterval.wait;
			}
		}).play;
		updateTask = Task({
			loop{//this doesnt work
				this.requestValuesFromRobot([\currentPosition]);
				updateInterval.wait;
			}
		});
	}

	connect{arg path;
		parser = VTKoppRobotParser(this, path);
	}

	connected{
		^parser.connected;
	}

	disconnect{
		parser.disconnect;
	}

	startRefreshTask{
		refreshTask.play;
		updateTask.play;
	}

	stopRefreshTask{
		refreshTask.stop;
		updateTask.stop;
	}

	sendMsg{arg setGet, command, val;
		parser.sendMsg(setGet, command, val);
	}

	refresh{
		motor.refresh;
	}

	stop{
		motor.stop;
	}

	requestValuesFromRobot{arg which;//a list of keys to be updated
		motor.requestValuesFromRobot(which);
	}

	update{arg theChanged, theChanger, key, value;
		"Kopp Robot update: %".format([theChanged, theChanger, key, value]).postln;
		if(theChanger !== this, {
			this.motor.perform(key.asSetter, value, sync: false);
			if(key == \state, {
				this.stateChangeAction.value(value);
			});
		});
	}
}