
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

// //map tog to faders
// 7.do{arg i;
// 	var fader, tog, button;
// 	fader = ~togCtrlA.components.at(("fader." ++ (i + 1)).asSymbol);
// 	tog = ~tog.channels[i];
// 	fader.addDependant(tog);
// 	tog.addDependant(fader);
//
// 	button = ~togCtrlA.components.at(("button." ++ (i + 9)).asSymbol);
// 	button.action = {arg butt;
// 		if(butt.value != 0, {
// 			tog.stop;
// 			tog.changed(butt, \value);
// 		});
// 	};
// 	tog.stop;
// 	fader.valueAction_(512);
// 	fader.refresh;
// };

fork{
	~coordinator = XBeeCoordinatorAPIMode(SerialPort('/dev/tty.usbserial-A600ezu2', 115200));
	~coordinator.rxAction_(nil);
	~coordinator.sendATCommand(\AggregateRoutingNotification, [1]);
	~togToFigurMappings = (
		'TOG01': 7, //nest hoy skjerm
		'TOG02': 3, //hoy figur, stanga
		'TOG03': 4, //liten figur
		'TOG04': 6, //hoy skjerm
		'TOG05': 12,//linsetog 3
		'TOG06': 2, //boksern
		'TOG07': 9, //hesten
		'TOG08': 8, //prins
		'TOG09': 11,//linsetog 2
		'TOG10': 1, //skjevt hode
		'TOG11': 5, //bred skjerm
		'TOG12': 0, //tog 1
		'TOG13': 13 //hyletog 4
	);
	~lok = IdentityDictionary.new;
	~figur = Dictionary.new;
	~figurFaderMappings = Dictionary[
		0 -> (device: ~togCtrlB, number: 1),
		1 -> (device: ~togCtrlB, number: 2),
		2 -> (device: ~togCtrlB, number: 3),
		3 -> (device: ~togCtrlB, number: 4),
		4 -> (device: ~togCtrlB, number: 5),
		11 -> (device: ~togCtrlB, number: 6),
		12 -> (device: ~togCtrlB, number: 7),
		13 -> (device: ~togCtrlB, number: 8),
		8 -> (device: ~togCtrlA, number: 1),
		9 -> (device: ~togCtrlA, number: 2),
		5 -> (device: ~togCtrlA, number: 3),
		6 -> (device: ~togCtrlA, number: 4),
		7 -> (device: ~togCtrlA, number: 5)
	];

	~trackingDataStore = Dictionary[
		1 -> IdentityDictionary[\rightEdge -> -5582],
		2 -> IdentityDictionary[\rightEdge -> 6221],
		3 -> IdentityDictionary[\rightEdge -> -7101],
		5 -> IdentityDictionary[\rightEdge -> -7190],
		6 -> IdentityDictionary[\rightEdge -> -10802],
		7 -> IdentityDictionary[\rightEdge -> -10862],
		8 -> IdentityDictionary[\rightEdge -> -13883],
		9 -> IdentityDictionary[\rightEdge -> 11759]
	];

	~video = NetAddr("1.2.3.44", 1234);
	~trackingParent = IdentityDictionary[
		\interpolationStepsPerUpdate -> 20, \updateRate -> 0.4, \lastPosition -> 0.0
	];
	~speedOffsetRange = 0.0;
	//build tracking data dictionary
	~trackingData = Dictionary.new;
	~togRegistrationService = {arg theChanger, theChanged, more;
		if(theChanged == \childDeviceRegistered, {
			var togFader, faderNumber, togStopButton, faderUpdater, togAddress, newChildDevice;
			var speedUpdateTask, runUpdateTask, nodeID;
			newChildDevice = more;
			nodeID = newChildDevice.nodeIdentifier.asSymbol;
			"GOT new child device: %, %".format(newChildDevice, nodeID).postln;
			//if this radio module has not been registered before
			if(~lok.includesKey(nodeID).not && "^TOG.+".matchRegexp(nodeID.asString), {
				var mapping, newLokomotiv, figurNumber, trackingData, sendPath;
				~lok.put(nodeID, newChildDevice);
				togAddress = newChildDevice.addressLo;
				newLokomotiv = VTLokomotiv.new(newChildDevice);
				"Made new lokomotiv: %".format(newLokomotiv.name).postln;
				figurNumber = ~togToFigurMappings[newLokomotiv.name];
				mapping = ~figurFaderMappings.at(figurNumber);
				~figur.put(figurNumber, newLokomotiv);
				faderNumber = mapping[\number];
				togFader = mapping[\device].components.at(("fader." ++ faderNumber).asSymbol);
				"Figur% Mapped to fader number: %".format(figurNumber.asPaddedString, faderNumber).postln;
				speedUpdateTask = Task.new({});
				runUpdateTask = {
					if(speedUpdateTask.isPlaying.not, {
						speedUpdateTask = Task.new({
							loop{
								var newVal;
								newVal = togFader.value.linlin(0, 1023, -512, 512).asInteger;
								if(newVal != newLokomotiv.bipolarSpeed, {
									newLokomotiv.bipolarSpeed_(newVal);
								});
								0.1.wait;
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
				togStopButton = mapping[\device].components.at(("button." ++ (faderNumber + 8)).asSymbol);
				togStopButton.action = {arg val; newLokomotiv.stop(1000);};
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
				newLokomotiv.addDependant(faderUpdater);
				OSCdef("figur%_BipolarSpeedResponder".format(nodeID.asString).asSymbol, {arg msg, time, addr,port;
					"Remote figur speed %: %".format(nodeID, msg[1]).postln;
					togFader.valueAction_(msg[1].linlin(-1.0, 1.0, 0, 1023).asInteger);
				}, "/robot/figur/%/bipolarSpeed".format(nodeID).asSymbol);
				~figur.put(figurNumber, newLokomotiv);
				"~figur now: %".format(~figur).postln;
				newLokomotiv.addDependant({arg theChanged, what, more;
					"Figur: % % % %".format(figurNumber, theChanged.name, what, theChanged.perform(what)).postln;
				};);

				///////////////////////////////////////
				//Make new tracking data for figur
				///////////////////////////////////////
				trackingData = IdentityDictionary.new(parent: ~trackingParent).put(\videoOSCPath,
					"/video/figur%".format( asPaddedString(figurNumber) ).asSymbol
				);
				~trackingData.put(figurNumber, trackingData);

				///////////////////////////////////////
				//initialize tracking data for figur
				///////////////////////////////////////
				~trackingDataStore[figurNumber] !? {trackingData.putAll(~trackingDataStore[figurNumber]);};

				sendPath = trackingData[\videoOSCPath];
				//running state of tracking, false by default
				trackingData.put(\trackingIsRunning, false);
				trackingData.put(\trackingController, SimpleController.new(newLokomotiv));
				trackingData.put(\trackingAction, {
					var ctrl, waitTime;
					"TRacking action, test if running: %".format(trackingData[\trackingIsRunning]).postln;
					if(trackingData[\trackingIsRunning].not, {
						ctrl = trackingData[\trackingController];
						waitTime = trackingData[\updateRate] / trackingData[\interpolationStepsPerUpdate];
						ctrl.put(\distanceFromLastAddress, {arg theChanged, theChanger, more;
							var val, videoPos, stream, duration, speedOffset;
							"Calculateing: %".format(trackingData).postln;
							duration = trackingData[\updateRate];
							val = theChanged.distanceFromLastAddress;
							videoPos = val.linlin(
								0.0,//, trackingData[\leftEdge],
								trackingData[\rightEdge],
								0.0,//trackingData[\leftVideoEdge],
								1.0,//trackingData[\rightVideoEdge],
								\none
							);
							stream = Env([trackingData[\lastPosition].asFloat, videoPos.asFloat], [duration]).asPseg.asStream;

							fork{
								speedOffset = newLokomotiv.bipolarSpeed.linlin(-511, 511, ~speedOffsetRange.neg, ~speedOffsetRange);
								trackingData[\interpolationStepsPerUpdate].do{arg j;
									~video.sendMsg(sendPath, stream.next + speedOffset);
									waitTime.wait;
								}
							};
							trackingData[\lastPosition] = videoPos;
						});
						"Setting polling: %".format(trackingData[\updateRate]).postln;
						newLokomotiv.set(\distancePollingInterval, asInteger( trackingData[\updateRate]*1000 ) );
						trackingData[\trackingIsRunning] = true;
						},
						{
							"Tracking for figur% already running".format(figurNumber.asPaddedString).postln;
						}
					);
				}.inEnvir);
				//function to start tracking for video
				trackingData.put(\startVideoTracking, trackingData[\trackingAction]);

				//function to stop video tracking
				trackingData.put(\stopVideoTracking, {
					// var ctrl = trackingData[\trackingController];
					// ctrl.remove;
					newLokomotiv.set(\distancePollingInterval, 0);
					trackingData[\trackingIsRunning] = false;
				});

				///////////////////////////////////////
				//Make OSC responders for tracking
				///////////////////////////////////////

				OSCdef("videoTrackingResponder_%".format(figurNumber).asSymbol, {arg msg, time, addr, port;
					var val;
					val = msg[1].booleanValue;
					if(val, {
						"Turning on tracking for figur number: %".format(figurNumber).postln;
						trackingData[\startVideoTracking].value;
						}, {
							"Turning off tracking for figur number: %".format(figurNumber).postln;
							trackingData[\stopVideoTracking].value;
					});
				}, "/robot/figur%/videoTracking".format(figurNumber.asPaddedString).asSymbol);
				OSCdef("videoTrackingCalibrationRight_%".format(figurNumber).asSymbol, {arg msg, time, addr, port;
					var val, dataToStore;
					val = msg[1].asString.asFloat;
					"Calibrating right video edge: figur% %".format(figurNumber.asPaddedString, val).postln;
					fork{
						//read distanceFromLastAddress from tog
						newLokomotiv.get(\distanceFromLastAddress);
						//short wait to let the lokomotiv respond
						0.5.wait;
						//store that in tracking data as rightEdge
						"Calibrating right edge done".postln;
						dataToStore = IdentityDictionary.new;
						[\rightEdge].do{arg item;
							dataToStore.put(item, trackingData[item]);
						};
						dataToStore.writeArchive(
							"~/vt_robot/MekanikkPatches/data/trackingDataFigur%.vttrack".format(figurNumber).standardizePath
						);
					};
				}.inEnvir, "/robot/figur%/calibrateRight".format(figurNumber.asPaddedString).asSymbol);
				OSCdef("videoTrackingCalibrationLeft_%".format(figurNumber).asSymbol, {arg msg, time, addr, port;
					var val, dataToStore;
					val = msg[1].asString.asFloat;
					"Setting figur% counter to 0".format(figurNumber.asPaddedString).postln;
					newLokomotiv.set(\distanceFromLastAddress, 0);
				}.inEnvir, "/robot/figur%/calibrateLeft".format(figurNumber.asPaddedString).asSymbol);
			}, {"Will not register as figur: %, %".format(newChildDevice, nodeID).postln;});
			if(~lok.includesKey(nodeID).not && "^TO$".matchRegexp(nodeID.asString), {
				"Registering led tog 1 (TO)".postln;
				~lok.put(nodeID, newChildDevice);
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
											"sending: % [%]\n".postf(~ledValue, ~tog);
											~tog.sendTXData(["ms".ascii, ~ledValue.asInteger, 13, 10].flatten);
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
	"Lokomotiver ready".postln;
};
