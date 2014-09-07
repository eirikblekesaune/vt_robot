~kopper = [
	VTKoppRobot("/dev/tty.usbserial-FT5QT6UUA"), //9 //OK kobling
	VTKoppRobot("/dev/tty.usbserial-FT5QT6UUB"), //10 // svarer ikke
	VTKoppRobot("/dev/tty.usbserial-FT5QT6VAA"), //11 // ok svarer
	VTKoppRobot("/dev/tty.usbserial-FT5QT6VAB"), //12 // ok svarer
	VTKoppRobot("/dev/tty.usbserial-FT5QT6VKA"), //13
	VTKoppRobot("/dev/tty.usbserial-FT5QT6VKB") //14 OK koblet til kopp nr 3
];

[0, 1, 2, 3, 4, 5].do{arg i;
	OSCFunc({arg msg, time, addr, resp;
		var dir;
		dir = msg[1].isNegative.not.asInt;
		"kopper: % speed: %, dir: %".format(i + 1, msg, dir).postln;
		~kopper[i].sendMsg(\set, \direction, dir);
		~kopper[i].sendMsg(\set, \speed, msg[1].clip(-512, 512).asInteger.abs);
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/speed").asSymbol);
	OSCFunc({arg msg, time, addr, resp;
		~kopper[i].stop;
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/stop").asSymbol);
};
[0, 1, 2, 3, 4, 5].do{arg i;
	OSCFunc({arg msg, time, addr, resp;
		~kopper[i].sendMsg(\set, \goToParkingPosition, 0);
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/goToParkingPosition").asSymbol);
};
[0, 1, 2, 3, 4, 5].do{arg i;
	OSCFunc({arg msg, time, addr, resp;
		~kopper[i].sendMsg(\set, \targetPosition, msg[1]);
	}.inEnvir, ("/robot/flying." ++ (i + 1) ++ "/targetPosition").asSymbol);
};
OSCFunc({arg msg, time, addr, resp;
	~kopper.do(_.stop);
}.inEnvir, '/robot/flying/stop');
~koppWalkMaxSpeed = 150;

OSCdef('koppWalkMaxSpeed', {arg msg, time, addr, port;
	"koppWalkMaxSpeed: %".format(msg[1]).postln;
	~koppWalkMaxSpeed = msg[1];}, '/robot/flying/walkMaxSpeed');

~koppWalkStart = {
	[0,1,2,3,4,5].do({arg koppIndex;
	var koppNum = koppIndex + 1;
	var doNextTarget = {
		var taskKey = "koppWalk%".format(koppNum).asSymbol;
		Tdef(taskKey).clear;
		Tdef(taskKey, {
				var waitTime = rrand(5.0, 20.0);
				var newPos = rrand(250, 600);
				var speedRamp = Env([0, rrand(80, ~koppWalkMaxSpeed), 40], [5, rrand(1, 10)]).asPseg.asStream;
				"setting new target kopp %: %".format(koppNum, newPos).postln;
				"going in % seconds".format(waitTime).postln;
				waitTime.wait;
				~kopper[koppIndex].sendMsg(\set, \targetPosition, newPos);
				loop{
					var newSpeed = speedRamp.next;
					newSpeed !? { ~kopper[koppIndex].sendMsg(\set, \speed, newSpeed.asInteger); };
					0.1.wait;
				};
			}.inEnvir).play;
	};
	~kopper[koppIndex].stateChangeAction_({arg newState;
		if(newState == \stoppedAtTarget, {
			doNextTarget.value;
		});
	});
	doNextTarget.value;
});
};
~koppWalkStop = {
	~kopper.do({arg item, i;
		var taskKey = "koppWalk%".format(i + 1).asSymbol;
		Tdef(taskKey).clear;
		item.stateChangeAction_(nil);
		item.sendMsg(\set, \speed, 0);
	});
};
OSCdef(\koppWalkResponder, {arg msg, time, addr, port;
	if(msg[1] == 1, {
		"Started kopp walk".postln;
		~koppWalkStart.value;
	});
	if(msg[1] == 0, {
		"Stopped kopp walk".postln;
		~koppWalkStop.value;
	});
}, '/robot/flying/walk');
