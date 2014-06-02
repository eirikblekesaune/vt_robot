//Wireless LED fader

~xbee = XBeeAPI(SerialPort('/dev/tty.usbserial-A600ezu2', 9600));
~xbee.rxAction = {|serialAddr, networkAddr, receiveOptions, data|
	[serialAddr, networkAddr, receiveOptions, data].postln;
};

~xbee.start;

~togLED1 = (
	xbee: ~xbee,
	address: [0x00, 0x13, 0xA2, 0x00, 0x40, 0x90, 0x29, 0xEA],//TO
	networkAddress: [0xFF, 0xFE],
	theTask: Task.new({}),
	runUpdateTask: {arg ev;
		ev.use{
			if(~theTask.isPlaying.not, {
				~theTask = Task.new({
					loop {
						//~setLEDValue.value(~address, ~networkAddress, ~ledValue);
						if(~ledValue != ~lastTransmittedValue, {
							~xbee.sendTXFrame(~address, ~networkAddress, ["ms".ascii, ~ledValue, 13, 10].flatten, 1);
							"sending: %\n".postf([~address, ~networkAddress, ~ledValue]);
							~lastTransmittedValue = ~ledValue;
						});
						0.05.wait;
					};
				}.inEnvir).play;
				SystemClock.sched(0.5, {
					if(~theTask.isPlaying, {
						~stopUpdateTask.value(currentEnvironment);
					});
				}.inEnvir);
			});
		}
	},
	stopUpdateTask: {arg ev;
		ev.use{
			~theTask.stop;
		}
	},
	ledValue: 0,
	lastTransmittedValue: 0,
	setLEDValue: {arg ev, val;
		ev[\ledValue] = val.clip(0, 250);
		ev[\runUpdateTask].value(ev);
	}
);


~togLED2 = (
	xbee: ~xbee,
	address: [0x00, 0x13, 0xA2, 0x00, 0x40, 0x8C, 0xCB, 0x3D],//TRE
	networkAddress: [0x0A, 0x28],
	theTask: Task.new({}),
	runUpdateTask: {arg ev;
		ev.use{
			if(~theTask.isPlaying.not, {
				~theTask = Task.new({
					loop {
						//~setLEDValue.value(~address, ~networkAddress, ~ledValue);
						if(~ledValue != ~lastTransmittedValue, {
							~xbee.sendTXFrame(~address, ~networkAddress, ["ms".ascii, ~ledValue, 13, 10].flatten, 1);
							"sending: %\n".postf([~address, ~networkAddress, ~ledValue]);
							~lastTransmittedValue = ~ledValue;
						});
						0.05.wait;
					};
				}.inEnvir).play;
				SystemClock.sched(0.5, {
					if(~theTask.isPlaying, {
						~stopUpdateTask.value(currentEnvironment);
					});
				}.inEnvir);
			});
		}
	},
	stopUpdateTask: {arg ev;
		ev.use{
			~theTask.stop;
		}
	},
	ledValue: 0,
	lastTransmittedValue: 0,
	setLEDValue: {arg ev, val;
		ev[\ledValue] = val.clip(0, 250);
		ev[\runUpdateTask].value(ev);
	}
);

~togLED3 = (
	xbee: ~xbee,
	//	address: [0x00, 0x13, 0xA2, 0x00, 0x40, 0x90, 0x29, 0xEA],//TO
	address: [0x00, 0x13, 0xA2, 0x00, 0x40, 0x90, 0x29, 0x1B],//FEM
	networkAddress: [0xFF, 0xFE],
	theTask: Task.new({}),
	runUpdateTask: {arg ev;
		ev.use{
			if(~theTask.isPlaying.not, {
				~theTask = Task.new({
					loop {
						//~setLEDValue.value(~address, ~networkAddress, ~ledValue);
						if(~ledValue != ~lastTransmittedValue, {
							~xbee.sendTXFrame(~address, ~networkAddress, ["ms".ascii, ~ledValue, 13, 10].flatten, 1);
							"sending: %\n".postf([~address, ~networkAddress, ~ledValue]);
							~lastTransmittedValue = ~ledValue;
						});
						0.05.wait;
					};
				}.inEnvir).play;
				SystemClock.sched(0.5, {
					if(~theTask.isPlaying, {
						~stopUpdateTask.value(currentEnvironment);
					});
				}.inEnvir);
			});
		}
	},
	stopUpdateTask: {arg ev;
		ev.use{
			~theTask.stop;
		}
	},
	ledValue: 0,
	lastTransmittedValue: 0,
	setLEDValue: {arg ev, val;
		ev[\ledValue] = val.clip(0, 250);
		ev[\runUpdateTask].value(ev);
	}
);


~togCtrl.components.at('fader.8').action_({arg comp;
	var val;
	val = comp.value.linlin(0, 1023, 0, 250).asInteger;
	~togLED1.setLEDValue(val);
});
~togCtrl.components.at('rotaryA.7').action_({arg comp;
	var val;
	val = comp.value.linlin(0, 1023, 0, 250).asInteger;
	~togLED2.setLEDValue(val);
});
~togCtrl.components.at('rotaryA.8').action_({arg comp;
	var val;
	val = comp.value.linlin(0, 127, 0, 250).asInteger;
	~togLED3.setLEDValue(val);
});


OSCdef(\flashTog1,{arg msg, time, addr, port;
	~xbee.sendTXFrame(~togLED1[\address], ~togLED1[\networkAddress], ["ms".ascii, 250, 13, 10].flatten, 1);
	~xbee.sendTXFrame(~togLED1[\address], ~togLED1[\networkAddress], ["ms".ascii, 0, 13, 10].flatten, 1);

}, '/robot/tog.1/flash');

OSCdef(\flashTog2,{
	~xbee.sendTXFrame(~togLED2[\address], ~togLED2[\networkAddress], ["ms".ascii, 250, 13, 10].flatten, 1);
	~xbee.sendTXFrame(~togLED2[\address], ~togLED2[\networkAddress], ["ms".ascii, 0, 13, 10].flatten, 1);
}, '/robot/tog.2/flash');

OSCdef(\flashTog2,{
	~xbee.sendTXFrame(~togLED3[\address], ~togLED3[\networkAddress], ["ms".ascii, 250, 13, 10].flatten, 1);
	~xbee.sendTXFrame(~togLED3[\address], ~togLED3[\networkAddress], ["ms".ascii, 0, 13, 10].flatten, 1);
}, '/robot/tog.3/flash');

OSCdef(\flashTog4,{
	~xbee.sendTXFrame(~togLED4[\address], ~togLED4[\networkAddress], ["ms".ascii, 250, 13, 10].flatten, 1);
	~xbee.sendTXFrame(~togLED4[\address], ~togLED4[\networkAddress], ["ms".ascii, 0, 13, 10].flatten, 1);
}, '/robot/tog.4/flash');
