var trackingMonitorWindow = Window.new;
//~tog = VTMonsterBox("/dev/tty.usbserial-FT3EET3MB");
//~pens = VTPenseBox("/dev/tty.usbserial-FT3EET3MA");

//map pens buttons
// 8.do{arg i;
// 	var midiComp, pens;
// 	midiComp = ~togCtrlA.components.at(("button." ++ (i + 1)).asSymbol);
// 	pens = ~pens.penses[i];
// 	midiComp.addDependant(pens);
// 	pens.addDependant(midiComp);
// };

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
		'TOG13': 13, //hyletog 4
		'TOG15': 14//reserve
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
		13 -> (device: ~togCtrlA, number: 6),
		14 -> (device: ~togCtrlA, number: 8),
		8 -> (device: ~togCtrlA, number: 1),
		9 -> (device: ~togCtrlA, number: 2),
		5 -> (device: ~togCtrlA, number: 3),
		6 -> (device: ~togCtrlA, number: 4),
		7 -> (device: ~togCtrlA, number: 5)
	];
	~beaconAddresses = Dictionary[
		8 -> 3,//figur 8 - 3
		5 -> 10, //figur 5 - 10
		6 -> 12,
		7 -> 13,
		3 -> 21,
	];
	~announceStation = false;
	~trackingDataStore = Dictionary.new;
	{
		var path, data;
		data = IdentityDictionary.new;
		path = PathName("~/vt_robot/MekanikkPatches/data/".standardizePath);
		path.filesDo{arg file;
			if(matchRegexp("trackingDataFigur\\d+\\.vttrack", file.fileName), {
				var figurData, figurNumber;
				figurData = IdentityDictionary.readArchive(file.fullPath);
				figurNumber = findRegexp(file.fileName, "(\\d+)")[1][1].asInteger;
				data.put(figurNumber, figurData);
			});
		};
		~trackingDataStore = data;
	}.value;
	~figurSoundData = Dictionary.new;

	//////////////////////////////////////////////////////
	//Lokomotiv GUI
	//////////////////////////////////////////////////////
	{
		var makeFigurView = {arg figurNumber, parent;
			var view, headView, trackingView, height, width, result;
			var pKnob, iKnob, dKnob, offsetKnob, rightEdgeView, rightEdgeText;
			var speedCtrlView, speedSlider, trackingStateButton;
			var makeMonitorWindow, monitorController;
			var margin = 2@2, gap = 2@2;
			makeMonitorWindow = {
				var width = 600, height = 400;
				var monitorFigur, speedPlotter, positionPlotter;
				var speedLabel, positionLabel, setPlotterValue;
				monitorFigur = ~figur[figurNumber];
				if(trackingMonitorWindow.notNil and: {trackingMonitorWindow.visible.notNil},{
					"Already exisiting monit window".postln;
					trackingMonitorWindow.close;
				});
				trackingMonitorWindow = Window("Peep show: (Figur %)".format(figurNumber), Rect(100, 100, width, height));
				trackingMonitorWindow.onClose_({
					"On close".postln;
					monitorController.remove(\trackingData);
					monitorController = nil;
				});
				trackingMonitorWindow.addFlowLayout(margin, gap);
				StaticText(trackingMonitorWindow, 150@30).string_("Speed:");
				speedLabel = StaticText(trackingMonitorWindow, 150@30).string_("none");
				trackingMonitorWindow.view.decorator.nextLine;
				speedPlotter = MultiSliderView.new(trackingMonitorWindow, Point(width, 150) - (margin * 2));
				StaticText(trackingMonitorWindow, 150@30).string_("Position:");
				positionLabel = StaticText(trackingMonitorWindow, 150@30).string_("none");
				trackingMonitorWindow.view.decorator.nextLine;
				positionPlotter = MultiSliderView.new(trackingMonitorWindow, Point(width, 150) - (margin * 2));
				[speedPlotter, positionPlotter].do{arg item;
					var dataSize = width;
					item.size = dataSize;
					item.thumbSize_(1.0);
					item.value_(0.5 ! dataSize);
					item.reference_(0.5 ! dataSize);
					item.isFilled_(true);
					item.editable_(false);
				};
				monitorController = SimpleController.new(monitorFigur);
				setPlotterValue = {arg val, plotter, label, spec;
					var newVal;
					// "Setting plotter: %".format([val, plotter, plotter.value]).postln;
					newVal = plotter.value;
					newVal.pop;
					newVal = newVal.addFirst(spec.unmap(val));
					plotter.value = newVal;
					label.string = val.asString;
				};
				monitorController.put(\trackingData, {arg theChanger, theChanged, more;
					var speed, position;
					#position, speed = theChanger.trackingData;
					{
						setPlotterValue.value(speed, speedPlotter, speedLabel, [-100.0, 100.0].asSpec);
						setPlotterValue.value(position, positionPlotter, positionLabel, [-20000.0, 20000.0].asSpec);
					}.defer;
				});
				trackingMonitorWindow.front;
			}.inEnvir;

			height = 100; width = 850;
			//"Making with this: % % %".format(figurNumber, parent, ~trackingData).postln;
			view = FlowView.new(parent, Point(width, height));
			headView = FlowView.new(view, Point(100, height), 0@0, 0@0);
			StaticText(headView, Point(50, (height / 2) - 4)).string_("Figur\n%".format(figurNumber)).font_(Font("Menlo", 16, true));
			Button(headView, Point(50, (height / 2) - 4)).states_([["monitor"]]).action_({
				makeMonitorWindow.value;
			}).font_(Font("Menlo", 10));
			headView.decorator.nextLine;
			trackingStateButton = Button(headView, Point(100, height / 2))
			.states_([["TrackingOFF", Color.black, Color.white], ["TrackingON", Color.white, Color.black]])
			.action_({arg butt;
				if(butt.value.booleanValue,
					{ ~trackingData[figurNumber][\startVideoTracking].value },
					{ ~trackingData[figurNumber][\stopVideoTracking].value }
				);
			}.inEnvir);
			rightEdgeView = FlowView(view, Point(100, height)).background_(Color.grey.alpha_(0.5));
			Button(rightEdgeView, Point(80, rightEdgeView.bounds.height * 0.45))
			.states_([["Calibrate\nright"]])
			.action_({NetAddr.localAddr.sendMsg("/robot/figur%/calibrateRight".format(figurNumber.asPaddedString).asSymbol);});
			rightEdgeText = StaticText(rightEdgeView, Point(80, rightEdgeView.bounds.height * 0.45)).string_("0").align_(\center);
			pKnob = EZKnob(view, Point(50, height), "P", [0.01, 10.0, \exp].asSpec, {arg it; ~trackingData[figurNumber].put(\pVal, it.value)}.inEnvir, 1.0);
			iKnob = EZKnob(view, Point(50, height), "I", [0.01, 10.0, \exp].asSpec, {arg it; ~trackingData[figurNumber].put(\iVal, it.value)}.inEnvir, 1.0);
			dKnob = EZKnob(view, Point(50, height), "D", [0.01, 10.0, \exp].asSpec, {arg it; ~trackingData[figurNumber].put(\dVal, it.value)}.inEnvir, 1.0);
			offsetKnob = EZKnob(
				view, Point(50, height), "offset", [-15.0, 15.0].asSpec,
				{arg it; ~trackingData[figurNumber].put(\speedOffsetDirectionMultiplier, it.value)}.inEnvir, 0.0);
			Button(view, Point(50, height))
			.states_([["Store\noffset\ndata"]])
			.action_({NetAddr.localAddr.sendMsg("/robot/figur%/storeTrackingOffsetData".format(figurNumber.asPaddedString).asSymbol);});
			speedCtrlView = FlowView.new(view, Point(200, height));
			speedSlider = EZSlider(speedCtrlView, Point(195, speedCtrlView.bounds.height * 0.75), "bipolarSpeed", [-511, 511].asSpec, {arg it;
				~figur[figurNumber].bipolarSpeed_(it.value.asInteger);
				"figur: % speed to set: %".format(~figur[figurNumber], it.value.asInteger).postln;
			}.inEnvir, 0, layout: \line2);
			Button(speedCtrlView, Point(195, speedCtrlView.bounds.height * 0.20)).states_([["stop"]]).action_({~figur[figurNumber].stop}.inEnvir);
			~trackingData.addDependant({arg theChanger, what, more;
				{
					if(what == figurNumber, {
						pKnob.value_(theChanger[figurNumber][\pVal]);
						iKnob.value_(theChanger[figurNumber][\iVal]);
						dKnob.value_(theChanger[figurNumber][\dVal]);
						offsetKnob.value_(theChanger[figurNumber][\speedOffsetDirectionMultiplier]);
						rightEdgeText.string_(theChanger[figurNumber][\rightEdge]);
						trackingStateButton.value_(theChanger[figurNumber][\trackingIsRunning].asInteger);
						view.refresh;
					});
				}.defer;
			});
			view.enabled_(~figur[figurNumber].notNil);
			if(view.enabled, {
				view.background_(Color.green.alpha_(1.0));
				}, {
					view.background_(Color.red.alpha_(1.0));
			});
			result = IdentityDictionary[\view -> view, \speedSlider -> speedSlider];
			result;
		}.inEnvir;
		~lokomotivWin = Window.new("Figurkalibrering", Rect(400, 0, 700, 800), scroll: true);
		~lokomotivWinViews = Dictionary.new;
		~lokomotivWin.addFlowLayout(2@5, 2@5);
		~togToFigurMappings.values.sort.do({arg figurNumber;
			var myButt;
			figurNumber = figurNumber.asInteger;
			~lokomotivWinViews.put(figurNumber, makeFigurView.value(figurNumber, ~lokomotivWin));
		});
		~lokomotivWin.view.decorator.nextLine;
		Button(~lokomotivWin, 350@50)
		.states_([["Station annoucing OFF"],["Station annoucing ON"]])
		.action_({arg butt;
			~announceStation = butt.value.booleanValue;
		});
		~lokomotivWin.alwaysOnTop_(true);
		~lokomotivWin.onClose_({0.exit;});
		~lokomotivWin.front;
	}.defer;



	~video = NetAddr("1.2.3.44", 1234);
	~trackingParent = IdentityDictionary[
		\interpolationStepsPerUpdate -> 20, \updateRate -> 0.4, \lastPosition -> 0.0,
		\pVal -> 1.0, \iVal -> 1.0, \dVal -> 1.0, \speedOffsetDirectionMultiplier -> 0.0
	];
	//build tracking data dictionary
	~trackingData = Dictionary.new;
	~storeFigurTrackingData = {arg figurNumber, keysToStore;
		var allKeysToStore, dataToStore, storePath;
		storePath = "~/vt_robot/MekanikkPatches/data/trackingDataFigur%.vttrack".format(figurNumber).standardizePath;
		allKeysToStore = #[\rightEdge, \pVal, \iVal, \dVal, \speedOffsetDirectionMultiplier];
		if(keysToStore.isNil, {
			keysToStore = allKeysToStore;
		});

		if(storePath.pathMatch.isEmpty,
			{
				dataToStore = IdentityDictionary.new;
			},
			{
				dataToStore = IdentityDictionary.readArchive(storePath);
			}
		);

		keysToStore.do({arg key;
			dataToStore.put(key, ~trackingData[figurNumber].at(key));
		});
		//store that in tracking data as rightEdge
		dataToStore.writeArchive(storePath);
		"Tracking data stored, keys: %".format(keysToStore).postln;
	}.inEnvir;
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
				}, "/robot/figur.%/bipolarSpeed".format(figurNumber).asSymbol);

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
				trackingData.put(\lastTrackingSpeed, 0.0);

				//function to start tracking for video
				trackingData.put(\startVideoTracking, {
					var ctrl, waitTime;
					//"TRacking action, test if running: %".format(trackingData[\trackingIsRunning]).postln;
					if(trackingData[\trackingIsRunning].not, {
						if(trackingData[\rightEdge].notNil, {
							"Turning on tracking for figur number: %".format(figurNumber).postln;
							trackingData.put(\trackingController, SimpleController.new(newLokomotiv));
							ctrl = trackingData[\trackingController];
							waitTime = trackingData[\updateRate] / trackingData[\interpolationStepsPerUpdate];

							//Danger code
							ctrl.put(\speed, {arg theChanged, theChanger, more;
								if(theChanged.speed == 0, {
									"Stopping routine: %".format(trackingData[\interpolationRoutine]).postln;
									trackingData[\interpolationRoutine].stop;
								});
							});
							ctrl.put(\trackingData, {arg theChanged, theChanger, more;
								var val, duration, speedOffset;
								var pSpeed, iSpeed, dSpeed;
								// "Calculating: %".format(trackingData).postln;
								duration = trackingData[\updateRate];
								val = theChanged.trackingPosition;
								pSpeed = theChanged.trackingSpeed;
								iSpeed = ((pSpeed + trackingData[\lastTrackingSpeed]) * 0.5);
								dSpeed = pSpeed - trackingData[\lastTrackingSpeed];
								trackingData.put(\lastTrackingSpeed, theChanged.trackingSpeed);
								speedOffset = (pSpeed * trackingData[\pVal]) + (iSpeed * trackingData[\iVal]) + (dSpeed * trackingData[\dVal]);
								speedOffset = speedOffset * (trackingData[\speedOffsetDirectionMultiplier] * (theChanged.speed / 512).squared);
								//"speedOffset: %, [%] [%] [%]".format(speedOffset, pSpeed, iSpeed, dSpeed).postln;
								{
									var stream;
									if(theChanged.speed != 0, {
										val = val + speedOffset;
									});
									stream = Env(
										[trackingData[\lastPosition].asFloat, val.asFloat],
										[duration]
									).asPseg.asStream;
									trackingData.put(\interpolationRoutine, fork{
										var streamVal, videoPos, offsetCorrection;
										trackingData[\interpolationStepsPerUpdate].do{arg j;
											streamVal = stream.next;
											videoPos = streamVal.linlin(0.0,trackingData[\rightEdge],0.0,1.0,\none);
											~video.sendMsg(sendPath, videoPos);
											trackingData[\lastPosition] = streamVal;
											waitTime.wait;
										}
									});
								}.value;
							}.inEnvir);
							//"Setting polling: %".format(trackingData[\updateRate]).postln;
							newLokomotiv.set(\trackingPollingInterval, asInteger( trackingData[\updateRate]*1000 ) );
							trackingData[\trackingIsRunning] = true;
							~trackingData.changed(figurNumber);
							},
							{
								"Can't start tracking! Figur % not calibrated!!".warn;
							}
						);
						},
						{
							"Tracking for figur% already running".format(figurNumber.asPaddedString).postln;
						}
					);
				}.inEnvir);

				//function to stop video tracking
				trackingData.put(\stopVideoTracking, {
					// var ctrl = trackingData[\trackingController];
					// ctrl.remove;
					if(trackingData[\trackingIsRunning], {
						"Turning off tracking for figur number: %".format(figurNumber).postln;
						newLokomotiv.set(\trackingPollingInterval, 0);
						trackingData[\trackingController].remove(\speed);
						trackingData[\trackingController].remove(\trackingData);
						trackingData.removeAt(\trackingController);
						trackingData[\trackingIsRunning] = false;
						~trackingData.changed(figurNumber);
					}, {"Tracking for figur % not running".format(figurNumber).warn;});
				});
				~trackingData.changed(figurNumber);

				//TODO:
				//listen to address updates from IRBeacons along the tracks. Update the tracking data accordingly.
				//When new address is detected
				////then: find the position for that address in addressData
				////store that number (trackingPosition) in the figur variable trackingPosition
				//Use the tracking position to calculate the values that we send to video

				///////////////////////////////////////
				//Make OSC responders for tracking
				///////////////////////////////////////

				OSCdef("videoTrackingResponder_%".format(figurNumber).asSymbol, {arg msg, time, addr, port;
					var val;
					val = msg[1].booleanValue;
					if(val, {

						trackingData[\startVideoTracking].value;
						}, {
							trackingData[\stopVideoTracking].value;
					});
				}, "/robot/figur%/videoTracking".format(figurNumber.asPaddedString).asSymbol);
				OSCdef("videoTrackingCalibrationRight_%".format(figurNumber).asSymbol, {arg msg, time, addr, port;
					var val;
					val = msg[1].asString.asFloat;
					"Calibrating right edge: figur% %".format(figurNumber.asPaddedString, val).postln;
					fork{
						var dataToStore;
						//read distanceFromLastAddress from tog
						newLokomotiv.get(\distanceFromLastAddress);
						newLokomotiv.get(\trackingData);
						//short wait to let the lokomotiv respond
						0.5.wait;
						trackingData.put(\rightEdge, newLokomotiv.trackingPosition);
						~storeFigurTrackingData.value(figurNumber, [\rightEdge]);
						~trackingData.changed(figurNumber);
						"Calibrating right edge done".postln;
					};
				}.inEnvir, "/robot/figur%/calibrateRight".format(figurNumber.asPaddedString).asSymbol);
				OSCdef("storeVideoOffsetTracking_%".format(figurNumber).asSymbol, {arg msg, time,addr,port;
					~storeFigurTrackingData.value(figurNumber, [\pVal, \iVal, \dVal, \speedOffsetDirectionMultiplier]);
					~trackingData.changed(figurNumber);
				}.inEnvir, "/robot/figur%/storeTrackingOffsetData".format(figurNumber.asPaddedString).asSymbol);

				/////////////////////////////////////////////////////////
				//Sound attachment responders
				/////////////////////////////////////////////////////////
				~figurSoundData.put(figurNumber, ());
				OSCdef("attachSoundFigur%".format(figurNumber).asSymbol, {arg msg, time, addr, port;
					var soundCue, soundCueKey, soundModuleKey, lydserver;
					var soundModulePath;
					"Attaching sound to figur %\n\t:[%]%".format(figurNumber,msg[1].class,msg[1]).postln;
					lydserver = NetAddr("1.2.3.57", 57120);
					soundCue = Dictionary.newFrom(msg[1].asString.parseYAML);
					soundCue.put(\name, \bufferplayer);
					soundCue = ().put(
						"figur%".format(figurNumber.asPaddedString),
						().put(
							\source,
							('figurSound': soundCue)
						)
					);
					soundModulePath = "/figur%/source/figurSound".format(figurNumber.asPaddedString);
					lydserver.sendMsg('/cue/json', JSON.stringify(soundCue));
					if(~figurSoundData[figurNumber][\soundController].isNil, {
						~figurSoundData.at(figurNumber).put(
							'soundController',
							SimpleController.new(~figur[figurNumber]).put(\bipolarSpeed, {arg theChanged, what;
								lydserver.sendMsg(
									"%/controlValue".format(soundModulePath).asSymbol,
									theChanged.bipolarSpeed.abs.linlin(0.0, 512, 0.0, 1.0);
								);
							});
						);
					});

				}.inEnvir, "/robot/figur%/attachSound".format(figurNumber.asPaddedString).asSymbol);
				OSCdef("detachSoundFigur%".format(figurNumber).asSymbol, {arg msg, time, addr, port;
					var soundCuePath;
					"Detaching sound from figur %".format(figurNumber).postln;
					soundCuePath = "/figur%/source/figurSound".format(figurNumber.asPaddedString).asSymbol;
					NetAddr("1.2.3.57", 57120).sendMsg('/cue/module/free', soundCuePath);
					~figurSoundData[figurNumber].removeAt('soundController').remove(\bipolarSpeed);
				}.inEnvir, "/robot/figur%/detachSound".format(figurNumber.asPaddedString).asSymbol);

				////////////////////////////////////////////////////////////
				///Announcement
				////////////////////////////////////////////////////////////

				{
					var beaconCtrl = SimpleController.new(newLokomotiv);
					beaconCtrl.put(\lastDetectedAddress, {arg theChanger, theChanged, more;
						if(~announceStation, {
							"Announcing".postln;
							Speech.setSpeechVoice(0, 2);
							Speech.setSpeechVolume(0, 2);
							"Figur % at station".format(figurNumber).speak;
						});
						// "\nSetting figur % to 0!!!\n".format(figurNumber).postln;
						// theChanger.set(\distanceFromLastAddress, 0);

					});
				}.value;

				//Connect figur to GUI
				SimpleController.new(newLokomotiv).put(\bipolarSpeed, {arg theChanger, what;
					//"Updating slider: %".format(theChanger.bipolarSpeed.asInteger).postln;
					{
						~lokomotivWinViews[figurNumber][\speedSlider].value_(theChanger.bipolarSpeed.asInteger);
					}.defer;
				}.inEnvir);
				SimpleController.new(newLokomotiv).put(\online, {arg theChanger, what;
					{
						var backColor;
						~lokomotivWinViews[figurNumber][\view].enabled_(theChanger.online);
						if(theChanger.online, {
							backColor = Color.green.alpha_(1.0);
							}, {
								backColor = Color.red.alpha_(1.0);
						});
						~lokomotivWinViews[figurNumber][\view].background_(backColor);
						~lokomotivWinViews[figurNumber][\view].refresh;
					}.defer;
				}.inEnvir);

				//Enable the gui view
				{~lokomotivWinViews[figurNumber][\view].background_(Color.green.alpha_(1.0)).enabled_(true).refresh;}.defer;

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

			//Temprary hack for creating LED control for tog.1
			if(~lok.includesKey(nodeID) && ("TOG12" == nodeID.asString), {
				"Registering led tog 1 (TOG12)".postln;
				//~lok.put(nodeID, newChildDevice);
				~ledTog = ~figur[0];
				~togLED1A = (
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
											//~tog.sendTXData(["ms".ascii, ~ledValue.asInteger, 13, 10].flatten);
											~tog.set(\peripheral, (0x00 << 24) | (0x08 << 16) | (~ledValue.clip(0, 0x7FF).asInteger));//(0x05 << 16)
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
					lookup: ([0] ++ (350..0x07FF)).resamp0(1024),
					ledValue: 0,
					lastTransmittedValue: 0,
					setLEDValue: {arg ev, val;
						var lookupIndex;
						lookupIndex = val.clip(0, 0x07FF).linlin(0, 0x07FF, 0, ev[\lookup].size);
						ev[\ledValue] = ev[\lookup][lookupIndex].asInteger;
						ev[\runUpdateTask].value(ev);
					}
				);
				~togCtrlB.components.at('fader.8').action = {arg comp; ~togLED1A.setLEDValue(comp.value.linlin(0, 1023, 0, 0x07FF).asInteger)};
				//tog.1 led 2
				~togLED1B = (
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
											//~tog.sendTXData(["ms".ascii, ~ledValue.asInteger, 13, 10].flatten);
											~tog.set(\peripheral, (0x00 << 24) | (0x04 << 16) | (~ledValue.clip(0, 0x7FF).asInteger));//(0x05 << 16)
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
					lookup: ([0] ++ (350..0x07FF)).resamp0(1024),
					ledValue: 0,
					lastTransmittedValue: 0,
					setLEDValue: {arg ev, val;
						var lookupIndex;
						lookupIndex = val.clip(0, 0x07FF).linlin(0, 0x07FF, 0, ev[\lookup].size);
						ev[\ledValue] = ev[\lookup][lookupIndex].asInteger;
						ev[\runUpdateTask].value(ev);
					}
				);
				~togCtrlB.components.at('rotaryA.1').action = {arg comp; ~togLED1B.setLEDValue(comp.value.linlin(0, 127, 0, 0x07FF).asInteger)};
				//tog 1 led micro 3
				~togLED1C = (
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
											//~tog.sendTXData(["ms".ascii, ~ledValue.asInteger, 13, 10].flatten);
											~tog.set(\peripheral, (0x00 << 24) | (0x09 << 16) | (~ledValue.clip(0, 255).asInteger));//(0x05 << 16)
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
						ev[\ledValue] = val.clip(0, 255);
						ev[\runUpdateTask].value(ev);
					}
				);
				//~togCtrlB.components.at('rotaryA.2').action = {arg comp; ~togLED1C.setLEDValue(comp.value.linlin(0, 127, 0, 250).asInteger)};
				~togCtrlA.components.at('rotaryA.7').action = {arg comp; ~togLED1C.setLEDValue(comp.value.linlin(0, 127, 0, 250).asInteger)};
			});
			////////////////////////////////////////////////////
			//Temprary hack for creating LED control for tog.2
			////////////////////////////////////////////////////
			if(~lok.includesKey(nodeID) && ("TOG09" == nodeID.asString), {
				"Registering led tog 2 (TOG09)".postln;
				//~lok.put(nodeID, newChildDevice);
				~ledTog = ~figur[11];
				~togLED2 = (
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
											//~tog.sendTXData(["ms".ascii, ~ledValue.asInteger, 13, 10].flatten);
											~tog.set(\peripheral, (0x00 << 24) | (0x08 << 16) | (~ledValue.clip(0, 0x7FF).asInteger));//(0x05 << 16)
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
					lookup: ([0] ++ (350..0x07FF)).resamp0(1024),
					ledValue: 0,
					lastTransmittedValue: 0,
					setLEDValue: {arg ev, val;
						var lookupIndex;
						lookupIndex = val.clip(0, 0x07FF).linlin(0, 0x07FF, 0, ev[\lookup].size);
						ev[\ledValue] = ev[\lookup][lookupIndex].asInteger;
						ev[\runUpdateTask].value(ev);
					}
				);
				~togLED2B = (
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
											//~tog.sendTXData(["ms".ascii, ~ledValue.asInteger, 13, 10].flatten);
											~tog.set(\peripheral, (0x00 << 24) | (0x04 << 16) | (~ledValue.clip(0, 0x7FF).asInteger));//(0x05 << 16)

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
					lookup: ([0] ++ (350..0x07FF)).resamp0(1024),
					ledValue: 0,
					lastTransmittedValue: 0,
					setLEDValue: {arg ev, val;
						var lookupIndex;
						lookupIndex = val.clip(0, 0x07FF).linlin(0, 0x07FF, 0, ev[\lookup].size);
						ev[\ledValue] = ev[\lookup][lookupIndex].asInteger;
						ev[\runUpdateTask].value(ev);
					}
				);
				~togCtrlB.components.at('rotaryA.6').action = {arg comp; ~togLED2.setLEDValue(comp.value.linlin(0, 127, 0, 0x07FF).asInteger)};
				~togCtrlB.components.at('rotaryA.5').action = {arg comp; ~togLED2B.setLEDValue(comp.value.linlin(0, 127, 0, 0x07FF).asInteger)};
			});
			//Temprary hack for creating LED control for tog.3
			if(~lok.includesKey(nodeID) && ("TOG05" == nodeID.asString), {
				"Registering led tog 3 (TOG05)".postln;
				//~lok.put(nodeID, newChildDevice);
				~ledTog = ~figur[12];
				~togLED3 = (
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
											//~tog.sendTXData(["ms".ascii, ~ledValue.asInteger, 13, 10].flatten);
											~tog.set(\peripheral, (0x00 << 24) | (0x04 << 16) | (~ledValue.clip(0, 0x7FF).asInteger));//(0x05 << 16)
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
					lookup: ([0] ++ (350..0x07FF)).resamp0(1024),
					ledValue: 0,
					lastTransmittedValue: 0,
					setLEDValue: {arg ev, val;
						var lookupIndex;
						lookupIndex = val.clip(0, 0x07FF).linlin(0, 0x07FF, 0, ev[\lookup].size);
						ev[\ledValue] = ev[\lookup][lookupIndex].asInteger;
						ev[\runUpdateTask].value(ev);
					}
				);
				~togCtrlB.components.at('rotaryA.7').action = {arg comp; ~togLED3.setLEDValue(comp.value.linlin(0, 127, 0, 0x07FF).asInteger)};
			});
			//Temprary hack for creating LED control for tog.4
			if(~lok.includesKey(nodeID) && ("TOG13" == nodeID.asString), {
				"Registering led tog 4 (TOG13)".postln;
				//~lok.put(nodeID, newChildDevice);
				~ledTog = ~figur[13];
				~togLED4 = (
					tog: ~ledTog,
					theTask: Task.new({}),
					runUpdateTask: {arg ev;
						ev.use{
							if(~theTask.isPlaying.not, {
								~theTask = Task.new({
									loop {
										//~setLEDValue.value(~address, ~networkAddress, ~ledValue);
										if(~ledValue != ~lastTransmittedValue, {
											"sending yog4 led: % [%]\n".postf(~ledValue, ~tog);
											//~tog.sendTXData(["ms".ascii, ~ledValue.asInteger, 13, 10].flatten);
											~tog.set(\peripheral, (0x00 << 24) | (0x08 << 16) | (~ledValue.clip(0, 0x7FF).asInteger));//(0x05 << 16)
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
					lookup: ([0] ++ (350..0x07FF)).resamp0(1024),
					ledValue: 0,
					lastTransmittedValue: 0,
					setLEDValue: {arg ev, val;
						var lookupIndex;
						lookupIndex = val.clip(0, 0x07FF).linlin(0, 0x07FF, 0, ev[\lookup].size);
						ev[\ledValue] = ev[\lookup][lookupIndex].asInteger;
						ev[\runUpdateTask].value(ev);
					}
				);
				~togCtrlA.components.at('rotaryA.6').action = {arg comp; ~togLED4.setLEDValue(comp.value.linlin(0, 127, 0, 0x07FF).asInteger)};
			});
		});
	};
	~coordinator.addDependant(~togRegistrationService);
	~togCtrlA.components['button.20'].action_({"NodeDiscovery".postln;~coordinator.sendATCommand('NodeDiscover');});
	~togCtrlB.components['button.20'].action_({"NodeDiscovery".postln;~coordinator.sendATCommand('NodeDiscover');});
	~togCtrlB.components['button.19'].action_({"Global figur stop".postln;
		//~figur.keysValuesDo({arg key, val; val.speed_(0)});
		[0,1,2,3,4,11,12].do{arg item;
			~figur[item].bipolarSpeed_(0);
		}

	});
	~togCtrlA.components['button.19'].action_({"Global figur stop".postln;
		[13,8,9,5,6,7].do{arg item;
			~figur[item].bipolarSpeed_(0);
		};
	});

	~coordinator.sendATCommand('NodeDiscover');


	"Lokomotiver ready".postln;
};
