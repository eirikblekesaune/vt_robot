
~tog = VTMonsterBox("/dev/tty.usbserial-FT3EET3MB");
~pens = VTPenseBox("/dev/tty.usbserial-FT3EET3MA");

//map pens buttons
8.do{arg i;
	var midiComp, pens;
	midiComp = ~togCtrlA.components.at(("button." ++ (i + 1)).asSymbol);
	pens = ~pens.penses[i];
	midiComp.addDependant(pens);
	pens.addDependant(midiComp);
};

//map tog to faders
7.do{arg i;
	var fader, tog, button;
	fader = ~togCtrlA.components.at(("fader." ++ (i + 1)).asSymbol);
	tog = ~tog.channels[i];
	fader.addDependant(tog);
	tog.addDependant(fader);

	button = ~togCtrlA.components.at(("button." ++ (i + 9)).asSymbol);
	button.action = {arg butt;
		if(butt.value != 0, {
			tog.stop;
			tog.changed(butt, \value);
		});
	};
	tog.stop;
	fader.valueAction_(512);
	fader.refresh;
};

fork{
	~coordinator = XBeeCoordinatorAPIMode(SerialPort('/dev/tty.usbserial-A600ezu2', 115200));
	~coordinator.start;
	~coordinator.rxAction_(nil);
	~coordinator.sendATCommand('NodeDiscover');
	~controller = {arg ...args; "changed: %: %".format(args, args[0].perform(args[1].asSymbol)).postln};
	10.wait;
	~lok = Dictionary.new;
	// ["TOG08"].do{arg togName, i;
	// 	var togFader, togStopButton, faderUpdater;
	// 	var newLok = VTLokomotiv.new(~coordinator, ~coordinator.getChildDeviceByName(togName.asSymbol).addressLo);
	// 	togFader = ~togCtrlB.components.at(("fader." ++ (i + 1)).asSymbol);
	// 	togFader.action = {arg val; newLok.bipolarSpeed_(val.value.linlin(0, 1023, -512, 512).asInteger)};
	// 	togStopButton = ~togCtrlB.components.at(("button." ++ (i + 9)).asSymbol);
	// 	togStopButton.action = {arg val; newLok.stop(1000);};
	// 	faderUpdater = {arg theChanger, theChanged;
	// 		switch(theChanged,
	// 			\speed, {togFader.value_(theChanger.bipolarSpeed.asInteger.linlin(-512,512, 0, 1023).asInteger).refresh;},
	// 			\stop, {togFader.value_(512).refresh;}
	// 		);
	// 	};
	// 	newLok.addDependant(faderUpdater);
	// 	newLok.addDependant(~controller);
	//
	// 	~lok.put(togName, newLok);
	// };
	//Wireless LED fader


	~ledTog = ~coordinator.getChildDeviceByName('TO');
	~togLED1 = (
		tog: ~ledTog,
		theTask: Task.new({}),
		runUpdateTask: {arg ev;
			ev.use{
				if(~theTask.isPlaying.not, {
					~theTask = Task.new({
						loop {
							//~setLEDValue.value(~address, ~networkAddress, ~ledValue);
							if(~ledValue != ~lastTransmittedValue, {
								~tog.sendTXData(["ms".ascii, ~ledValue.asInteger, 13, 10].flatten);
								"sending: % [%]\n".postf(~ledValue, ~tog);
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
	~togCtrlA.components.at('fader.8').action = {arg comp; ~togLED1.setLEDValue(comp.value.linlin(0, 1023, 0, 250).asInteger)};

	"Lokomotiver ready".postln;
};
