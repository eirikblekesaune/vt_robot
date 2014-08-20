~kransMonster = VTMonsterBox("/dev/tty.usbserial-FT3EET44A");
~kranser = Dictionary[
	1 -> ~kransMonster.channels[2],
	2 -> ~kransMonster.channels[0]
];

~setKransSpeed = {arg kransNumber, speed;
	~kranser[kransNumber].valueAction_(speed.clip(-512, 512).asInteger);
};
~kransFader1 = ~togCtrlA.components.at('fader.7');
~kransStopButton1 = ~togCtrlA.components.at('button.15');
~kransFader1.action = {arg ctrlComp;
	var val;
	val = ctrlComp.value.linlin(0, 1023, -512, 512);
	"Krans 1 - bipolarSpeed: %".format(val).postln;
	~setKransSpeed.value(1, val);
};
~kransStopButton1.action = {arg ctrlComp;
	"Krans stopp".postln;
	~setKransSpeed.value(1, 0);
	~kransFader1.value_(511).refresh;
};

OSCdef('kransResponer1', {arg msg, time, addr, port;
	~setKransSpeed.value(1, msg[1].linlin(-1.0, 1.0, -512, 512).asInteger);
},'/robot/krans.1/bipolarSpeed');

~kransFader2 = ~togCtrlA.components.at('fader.8');
~kransStopButton2 = ~togCtrlA.components.at('button.16');
~kransFader2.action = {arg ctrlComp;
	var val;
	val = ctrlComp.value.linlin(0, 1023, -512, 512);
	"Krans 2 - bipolarSpeed: %".format(val).postln;
	~setKransSpeed.value(2, val);
};
~kransStopButton2.action = {arg ctrlComp;
	"Krans stopp".postln;
	~setKransSpeed.value(2, 0);
	~kransFader2.value_(511).refresh;
};

OSCdef('kransResponer2', {arg msg, time, addr, port;
	~setKransSpeed.value(2, msg[1].linlin(-1.0, 1.0, -512, 512).asInteger);
},'/robot/krans.2/bipolarSpeed');
