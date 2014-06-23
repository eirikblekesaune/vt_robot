
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

	~controller = {arg ...args; "changed: %: %".format(args, args[0].perform(args[1].asSymbol)).postln};

	~lok = Dictionary.new;
	~togRegistrationService = {arg theChanger, theChanged, more;
		if(theChanged == \childDeviceRegistered, {
			var togFader, faderNumber, togStopButton, faderUpdater, togAddress, newChildDevice, newTog, togName;
			var togFaderMappings, speedUpdateTask, runUpdateTask;
			togFaderMappings = (
				TOG02: 1,
				TOG03: 2,
				TOG04: 3,
				TOG06: 4,
				TOG05: 5,
				TOG08: 6
			);
			newChildDevice = more;
			togName = newChildDevice.nodeIdentifier.asString;
			"GOT new device: %, %".format(newChildDevice, togName).postln;
			//if this radio module has not been registered before
			if(~lok.includesKey(togName).not && "^TOG.+".matchRegexp(togName), {
				"Will register".postln;
				togAddress = newChildDevice.addressLo;
				newTog = VTLokomotiv.new(~coordinator, togAddress);
				"Made new lok: newTog".format(newTog).postln;
				faderNumber = togFaderMappings.at(togName.asSymbol);
				togFader = ~togCtrlB.components.at(("fader." ++ faderNumber).asSymbol);
				"Mapped to fader number: %".format(faderNumber).postln;
				speedUpdateTask = Task.new({});
				runUpdateTask = {
					if(speedUpdateTask.isPlaying.not, {
						speedUpdateTask = Task.new({
							loop{
								newTog.bipolarSpeed_(togFader.value.linlin(0, 1023, -512, 512).asInteger);
								0.05.wait;
							}
						}).play;
						SystemClock.sched(0.5, {
							if(speedUpdateTask.isPlaying, {
								speedUpdateTask.stop;
							});
						});
					});
				};
				togFader.action = {runUpdateTask.value;};
				togStopButton = ~togCtrlB.components.at(("button." ++ (faderNumber + 8)).asSymbol);
				togStopButton.action = {arg val; newTog.stop(1000);};
				faderUpdater = {arg theChanger, theChanged;
					switch(theChanged,
						\speed, {
							if(theChanger.bipolarSpeed == 0, {
								togFader.value_(512).refresh;
							});
						},
						\stop, {togFader.value_(512).refresh;}
					);
				};
				newTog.addDependant(faderUpdater);
				newTog.addDependant(~controller);
				OSCdef("lokomotiv%".format(togName).asSymbol, {arg msg, time, addr,port;
					"Remote lok speed %: %".format(togName, msg[1]).postln;
					togFader.valueAction_(msg[1].linlin(-1.0, 1.0, 0, 1023).asInteger);
				}, "/robot/lokomotiv/%/bipolarSpeed".format(togName).asSymbol);
				~lok.put(togName.asString, newTog);
				"~lok now: %".format(~lok).postln;
			}, {"Will not register as lokomotiv: %, %".format(newChildDevice, togName).postln;});
			if(~lok.includesKey(togName).not && "^TO$".matchRegexp("TO"), {
				"Registering led tog 1 (TO)".postln;
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

			});
		});
	};
	~coordinator.addDependant(~togRegistrationService);
	~coordinator.sendATCommand('NodeDiscover');
	// ["TOG02", "TOG03", "TOG04", "TOG05", "TOG06", "TOG08"].do{arg togName, i;
	//
	// };
	//Wireless LED fader

	10.wait;

	"Lokomotiver ready".postln;
};
