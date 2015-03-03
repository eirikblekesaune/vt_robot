(
var num = 0;
~myTrackingData = ~trackingData[num];
~myTog = ~figur[num];
~myFigurPath = ~myTrackingData[\videoOSCPath];
)
//2. Kjor video til onsket posijon
~myTrackingData[\startVideoTracking].value
~myTrackingData[\stopVideoTracking].value
~video.sendMsg(~myFigurPath, ~myTrackingData[\rightVideoEdge])
~video.sendMsg(~myFigurPath, ~myTrackingData[\leftVideoEdge])
~video.sendMsg(~myFigurPath, 0.5)
~video.sendMsg(~myFigurPath, 0.0)
~myTog.get(\distanceFromLastAddress);//1330
~myTog.set(\distanceFromLastAddress, 0);//3. shift + enter på denne linja. Dette setter telleverk til null -> Gå til Lokomotiver.scd
~myTog.get(\distanceFromLastAddress);
~myTog.trackingPosition;
~myTog.get(\distanceFromLastAddress)
~myTog.direction
~myTog.get(\direction);
~myTog.set(\direction, 1);
~myTog.get(\trackingData)
~myTog.get(\measuredSpeed)
~myTog.trackingPosition
~myTog.trackingData


~myTog.bipolarSpeed_( -260 );
~myTog.bipolarSpeed_( 0 );
~myTrackingData[\rightEdge] = -15466;//NB store this!!!
~speedOffsetRange = 1
~pVal = 0.3; ~iVal = 0.9; ~dVal = 0.0;
~speedOffsetDirectionMultiplier = -3;
0.5.squared

(
f = fork{
	loop{
		~video.sendMsg(~myFigurPath, 0.1.postln);
		0.4.wait;
		~video.sendMsg(~myFigurPath, 0.0.postln);
		0.4.wait;
	}
}
)
f.stop;

~figur[0].xbeeDevice


~myTog.set(\trackingPollingInterval, 0);

NetAddr.localAddr.sendMsg('/robot/figur06/calibrateRight');

~myTog.lastDetectedAddress;
(
~beaconAddresses = Dictionary[
	8 -> 3,//figur 8 - 3
	5 -> 10, //figur 5 - 10
	6 -> 12,
	7 -> 13,
	3 -> 21,
];

~myTogCtrl = SimpleController.new(~myTog);
~myTogCtrl.put(\lastDetectedAddress, {arg theChanger, theChanged, more;
	if(theChanger.lastDetectedAddress == 3, {
		"\n\nSetting figur 6 to 0!!!\n\n".postln;
		theChanger.set(\distanceFromLastAddress, 0);
	})
});
)



~myTog.get(\speed);
~myTog.set(\trackingPollingInterval, 200);
~myTog.set(\trackingPollingInterval, 0);
~myTog.get(\trackingPollingInterval);
~myTog.get(\measuredSpeed);
~myTog.get(\motorMode);
~myTog.set(\motorMode, 0);
~myTog.dumpInput_(true)




//inn
(
[["TOG10", -160], ["TOG06", 160], ["TOG02", -200], ["TOG03", 150], ["TOG08", 220], ["TOG07", -230]].do{arg item;
	~lok[item[0]].bipolarSpeed_(item[1]);
}
)

(
[["TOG10", -160, 0], ["TOG06", 140, 0], ["TOG02", -200, 0], ["TOG03", 150, 0], /*["TOG08", 200, 0],*/ ["TOG07", -230, 0]].do{arg item;
	fork{
		item[2].wait;
		~lok[item[0]].stop(1000);
	}
}
)

//ut
(
[["TOG10", -160], ["TOG06", 160], ["TOG02", -200], ["TOG03", 150], ["TOG08", 200], ["TOG07", -230]].do{arg item;
	~lok[item[0]].bipolarSpeed_(item[1].neg);
}
)



~trackingData[1].rightEdge


(
~trackingData[1].put(\beaconSettings, Dictionary[
	3 -> (position: 0, mirrored: false),
	6 -> (position: -15183, mirrored: false)
]);
)

(
var data;
data = Dictionary[
	1 -> IdentityDictionary[\rightEdge -> -5582],
	2 -> IdentityDictionary[\rightEdge -> 6080],
	3 -> IdentityDictionary[\rightEdge -> -6778],
	5 -> IdentityDictionary[\rightEdge -> -6849],
	6 -> IdentityDictionary[\rightEdge -> -10820],
	7 -> IdentityDictionary[\rightEdge -> -9899],
	8 -> IdentityDictionary[\rightEdge -> -15183],
	9 -> IdentityDictionary[\rightEdge -> 9048]
];
data.keysValuesDo({arg key, val;
	var path;
	path = PathName("~/vt_robot/MekanikkPatches/data/trackingDataFigur%.vttrack".format(key));
	val.writeArchive(path.fullPath);
});
)


(
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
data[3].postln;
)

~trackingData[1][\rightEdge]
~figur[8].distanceFromLastAddress



r = Routine({2.wait; "done".postln})
r.play.stop
r = r{2.wait; "fone".postln}
r.play; r.stop; r = fork{2.wait; "stil fon".postln}


///Denne linja kalibrerer hoyre side
NetAddr.localAddr.sendMsg('/robot/figur00/calibrateRight');
NetAddr.localAddr.sendMsg('/robot/figur00/storeTrackingOffsetData');


~trackingData[0].put(\speedOffsetDirectionMultiplier, -7.0);
a = IdentityDictionary.readArchive("~/vt_robot/MekanikkPatches/data/trackingDataFigur0.vttrack".standardizePath)
a

~trackingData[0][\pVal]

~trackingData.changed(0);