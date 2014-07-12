
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
		'TOG01': 7,//nest hoy skjerm
		'TOG02': 3,//hoy figur, stanga
		'TOG03': 4,//liten figur
		'TOG04': 6,//hoy skjerm
		'TOG05': 12,//linsetog 3
		'TOG06': 2,//boksern
		'TOG07': 9,//hesten
		'TOG08': 8,//prins
		'TOG09': 11,//linsetog 2
		'TOG10': 1,//skjevt hode
		'TOG11': 5,//bred skjerm
		'TOG12': 0,//tog 1
		'TOG13': 13//hyletog 4
	);
	~lok = Dictionary.new;
	~figur = Dictionary.new;
	~togRegistrationService = {arg theChanger, theChanged, more;
		if(theChanged == \childDeviceRegistered, {
			var togFader, faderNumber, togStopButton, faderUpdater, togAddress, newChildDevice, newTog, togName;
			var figurFaderMappings, speedUpdateTask, runUpdateTask;
			figurFaderMappings = Dictionary[
				0 -> (device: ~togCtrlB, number: 1),
				1 -> (device: ~togCtrlB, number: 2),
				2 -> (device: ~togCtrlB, number: 3),
				3 -> (device: ~togCtrlB, number: 4),
				4 -> (device: ~togCtrlB, number: 5),
				5 -> (device: ~togCtrlB, number: 6),
				6 -> (device: ~togCtrlB, number: 7),
				7 -> (device: ~togCtrlB, number: 8),
				8 -> (device: ~togCtrlA, number: 1),
				9 -> (device: ~togCtrlA, number: 2),
				10 -> (device: ~togCtrlA, number: 3),
				11 -> (device: ~togCtrlA, number: 4),
				12 -> (device: ~togCtrlA, number: 5),
				13 -> (device: ~togCtrlA, number: 6)
			];
			newChildDevice = more;
			togName = newChildDevice.nodeIdentifier.asString;
			"GOT new device: %, %".format(newChildDevice, togName).postln;
			//if this radio module has not been registered before
			if(~lok.includesKey(togName).not && "^TOG.+".matchRegexp(togName), {
				var mapping;
				togAddress = newChildDevice.addressLo;
				newTog = VTLokomotiv.new(newChildDevice);
				"Made new lok: newTog".format(newTog).postln;
				mapping = figurFaderMappings.at(~togToFigurMappings[togName.asSymbol]);
				~figur.put(~togToFigurMappings[togName.asSymbol], newTog);
				faderNumber = mapping[\number];
				togFader = mapping[\device].components.at(("fader." ++ faderNumber).asSymbol);
				"Mapped to fader number: %".format(faderNumber).postln;
				speedUpdateTask = Task.new({});
				runUpdateTask = {
					if(speedUpdateTask.isPlaying.not, {
						speedUpdateTask = Task.new({
							loop{
								var newVal;
								newVal = togFader.value.linlin(0, 1023, -512, 512).asInteger;
								if(newVal != newTog.bipolarSpeed, {
									newTog.bipolarSpeed_(newVal);
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
				OSCdef("lokomotiv%".format(togName).asSymbol, {arg msg, time, addr,port;
					"Remote lok speed %: %".format(togName, msg[1]).postln;
					togFader.valueAction_(msg[1].linlin(-1.0, 1.0, 0, 1023).asInteger);
				}, "/robot/lokomotiv/%/bipolarSpeed".format(togName).asSymbol);
				~lok.put(togName.asString, newTog);
				"~lok now: %".format(~lok).postln;

				newTog.addDependant({arg theChanged, what, more;
					"Lok: % % %".format(theChanged.name, what, theChanged.perform(what)).postln;
				};);
			}, {"Will not register as lokomotiv: %, %".format(newChildDevice, togName).postln;});
			if(~lok.includesKey(togName).not && "^TO$".matchRegexp(togName), {
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
	// ["TOG02", "TOG03", "TOG04", "TOG05", "TOG06", "TOG08"].do{arg togName, i;
	//
	// };
	//Wireless LED fader

	10.wait;
	~video = NetAddr("1.2.3.44", 1234);
	~trackingProto = IdentityDictionary[
		\interpolationStepsPerUpdate -> 20, \updateRate -> 0.4, \lastPosition -> 0.0
	];
	~trackingData = IdentityDictionary.new;
	~togToFigurMappings.size.do{arg item;
		var key;
		key = "TOG%".format( asPaddedString(item + 1) ).asSymbol;
		~trackingData.put(key, IdentityDictionary.new(parent: ~trackingProto).put(\videoOSCPath,
			"/video/figur%".format( asPaddedString(~togToFigurMappings[key]) ).asSymbol
			)
		);
	};
	~speedOffsetRange = 0.0;
	~trackingData['TOG08'].putAll(IdentityDictionary[\rightEdge -> 0, \leftEdge -> 14647, \rightVideoEdge -> 271.0, \leftVideoEdge -> -295]);
	~trackingData['TOG02'].putAll(IdentityDictionary[\rightEdge -> 0, \leftEdge -> -7275, \rightVideoEdge -> 30.0, \leftVideoEdge -> -681]);
	~trackingData['TOG06'].putAll(IdentityDictionary[\rightEdge -> 0, \leftEdge -> 7061, \rightVideoEdge -> 0.0, \leftVideoEdge -> -845]);
	~trackingData['TOG07'].putAll(IdentityDictionary[\rightEdge -> 0, \leftEdge -> -12370, \rightVideoEdge -> 323, \leftVideoEdge -> -259]);
	~trackingData['TOG10'].putAll(IdentityDictionary[\rightEdge -> 0, \leftEdge -> 7101, \rightVideoEdge -> 0.0, \leftVideoEdge -> -821]);
	//make start/stop video tracking functions
	~togToFigurMappings.size.do{arg i;
		var number, key, trackingData, lokomotiv, sendPath;
		number = i + 1;
		key = "TOG%".format( asPaddedString(i + 1) ).asSymbol;
		trackingData = ~trackingData[key];
		lokomotiv = ~lok[key.asString];
		sendPath = trackingData[\videoOSCPath];
		trackingData.put(\trackingController, SimpleController.new(lokomotiv));
		trackingData.put(\trackingAction, {
			var ctrl, waitTime;
			ctrl = trackingData[\trackingController];
			waitTime = trackingData[\updateRate] / trackingData[\interpolationStepsPerUpdate];
			ctrl.put(\distanceFromLastAddress, {arg theChanged, theChanger, more;
				var val, videoPos, stream, duration, speedOffset;
				duration = trackingData[\updateRate];
				val = theChanged.distanceFromLastAddress;
				videoPos = val.linlin(
					trackingData[\leftEdge],
					trackingData[\rightEdge],
					trackingData[\leftVideoEdge],
					trackingData[\rightVideoEdge],
					\none
				);
				stream = Env([trackingData[\lastPosition].asFloat, videoPos.asFloat], [duration]).asPseg.asStream;
				speedOffset = lokomotiv.bipolarSpeed.linlin(-511, 511, ~speedOffsetRange.neg, ~speedOffsetRange);
				fork{
					trackingData[\interpolationStepsPerUpdate].do{arg j;
						~video.sendMsg(sendPath, stream.next + speedOffset);
						waitTime.wait;
					}
				};
				trackingData[\lastPosition] = videoPos;
			});
			lokomotiv.set(\distancePollingInterval, asInteger( trackingData[\updateRate]*1000 ) );
		}.inEnvir);
		//function to start tracking for video
		trackingData.put(\startVideoTracking, trackingData[\trackingAction]);
		//function to stop video tracking
		trackingData.put(\stopVideoTracking, {
			// var ctrl = trackingData[\trackingController];
			// ctrl.remove;
			lokomotiv.set(\distancePollingInterval, 0);
		});
	};
	//Make responders for start/stop video tracking, calibration left and right
	~togToFigurMappings.size.do{arg i;
		var key, figurNumber, trackingData;
		key = "TOG%".format( asPaddedString(i + 1)).asSymbol;
		figurNumber = ~togToFigurMappings[key];
		trackingData = ~trackingData[key];
		OSCdef("videoTrackingResponder_%".format(key).asSymbol, {arg msg, time, addr, port;
			var val;
			val = msg[1].booleanValue;
			if(val, {
				"Turning on tracking for % figur number: %".format(key, figurNumber).postln;
				trackingData[\startVideoTracking].value;
			}, {
					"Turning off tracking for % figur number: %".format(key, figurNumber).postln;
					trackingData[\stopVideoTracking].value;
			});
		}, "/robot/figur%/videoTracking".format(figurNumber.asPaddedString).asSymbol);
		OSCdef("videoTrackingCalibrationRight_%".format(key).asSymbol, {arg msg, time, addr, port;
			var val, lok;
			val = msg[1];
			lok = ~lok[key.asString];
			//set tog DistanceFromLastAddress to 0
			lok.set(\distanceFromLastAddress, 0);
			//store rightVideoEdge in tracking data
			trackingData.put(\rightVideoEdge, val);
			//store rightEdge in tracking data as 0
			trackingData.put(\rightEdge, 0);
		}, "/robot/figur%/rightVideoEdge".format(figurNumber.asPaddedString).asSymbol);
		OSCdef("videoTrackingCalibrationLeft_%".format(key).asSymbol, {arg msg, time, addr, port;
			var val, lok;
			val = msg[1];
			lok = ~lok[key.asString];
			fork{
				//read distanceFromLastAddress from tog
				lok.get(\distanceFromLastAddress);
				//short wait to let the lokomotiv respond
				0.5.wait;
				//store that in tracking data as leftEdge
				trackingData.put(\leftEdge, lok.distanceFromLastAddress);
				//store videoLeftEdge from argument in trackingdata
				trackingData.put(\leftVideoEdge, val);
			}
		}, "/robot/figur%/leftVideoEdge".format(figurNumber.asPaddedString).asSymbol);
	};
	"Lokomotiver ready".postln;
};
