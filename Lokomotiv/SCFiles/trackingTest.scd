NetAddr("1.2.3.44", 1234).sendMsg('/video/tog', 250.0);
NetAddr("1.2.3.44", 1234).sendMsg('/video/tog', -388.0);

//  250 right max position 3@-82790
//-388 left edge 3@-66767
~coordinator.networkAddress;
~coordinator.sendATCommand(\NetworkAddress);
(
~coordinator.sendATCommand(\SerialNumberLow, responseAction: {arg data;
	XBeeParser.prParseAddressBytes(data['commandData']).postln;
});
)
~coordinator.createSourceRoute();
//lastAdress not initialized

~lok["TOG08"].get(\lastDetectedAddress);
~lok["TOG08"].set(\lastDetectedAddress, 3);

~lok["TOG10"].get(\distanceFromLastAddress);
~lok["TOG10"].set(\distanceFromLastAddress, 0)
~lok["TOG08"].get(\distanceFromLastAddress);
~lok["TOG08"].set(\distanceFromLastAddress, 0)
~lok["TOG02"].get(\distanceFromLastAddress);
~lok["TOG02"].set(\distanceFromLastAddress, 0)
~lok["TOG07"].get(\distanceFromLastAddress);
~lok["TOG07"].set(\distanceFromLastAddress, 0)
~lok["TOG06"].get(\distanceFromLastAddress);
~lok["TOG06"].set(\distanceFromLastAddress, 0)


~lok["TOG08"].get(\motorMode);
~lok["TOG08"].set(\motorMode, 1);

~lok["TOG10"].set(\distancePollingInterval, 200);
~lok["TOG10"].set(\distancePollingInterval, 0);
~lok["TOG08"].set(\distancePollingInterval, 200);
~lok["TOG08"].set(\distancePollingInterval, 0);
~lok["TOG02"].set(\distancePollingInterval, 200);
~lok["TOG02"].set(\distancePollingInterval, 0);
~lok["TOG07"].set(\distancePollingInterval, 200);
~lok["TOG07"].set(\distancePollingInterval, 0);
~lok["TOG06"].set(\distancePollingInterval, 200);
~lok["TOG06"].set(\distancePollingInterval, 0);


~lok["TOG08"].speed

~video.sendMsg('/video/tog08', -295.0);
~video.sendMsg('/video/tog06', -247.0);
~video.sendMsg('/video/tog06', 0.0);
~video.sendMsg('/video/tog02', 30.0);
(
~video = NetAddr("1.2.3.44", 1234);
///TOG08
~rightEdge8 = 0;
~leftEdge8 = 14951;
~rightVideoEdge8 = 271;//
~leftVideoEdge8 = -295;//
~lastPos8 = 0.0;
~steps8 = 10;
~togToVideo8 = {arg theChanger, theChanged, args;
	if(theChanged == \distanceFromLastAddress, {
		var val, videoPos, stream, duration;
		duration = 0.2;
		val = theChanger.distanceFromLastAddress;
		videoPos = val.linlin(~leftEdge8, ~rightEdge8, ~leftVideoEdge8, ~rightVideoEdge8, \none);
		stream = Env([~lastPos8.asFloat, videoPos.asFloat], [duration]).asPseg.asStream;
		//"pos: % videoPos: %".format(val, videoPos).postln;
		//~video.sendMsg('/video/tog', videoPos);
		fork{
			10.do{arg i;
				~video.sendMsg('/video/tog08', stream.next);
				//stream.next.postln;
				0.02.wait;
			}
		};
		~lastPos8 = videoPos;
	});
};
~lok["TOG08"].addDependant(~togToVideo8);
//TOG02
~rightEdge2 = 0;
~leftEdge2 = -7455;
~rightVideoEdge2 = 30;//
~leftVideoEdge2 = -681;//
~lastPos2 = 0.0;
~steps2 = 10;
~togToVideo2 = {arg theChanger, theChanged, args;
	if(theChanged == \distanceFromLastAddress, {
		var val, videoPos, stream, duration;
		duration = 0.2;
		val = theChanger.distanceFromLastAddress;
		videoPos = val.linlin(~leftEdge2, ~rightEdge2, ~leftVideoEdge2, ~rightVideoEdge2, \none);
		stream = Env([~lastPos2.asFloat, videoPos.asFloat], [duration]).asPseg.asStream;
		//"pos: % videoPos: %".format(val, videoPos).postln;
		//~video.sendMsg('/video/tog', videoPos);
		fork{
			10.do{arg i;
				~video.sendMsg('/video/tog02', stream.next);
				//stream.next.postln;
				0.02.wait;
			}
		};
		~lastPos2 = videoPos;
	});
};
~lok["TOG02"].addDependant(~togToVideo2);

//tog 6
~rightEdge6 = 0;
~leftEdge6 = 2552;
~rightVideoEdge6 = -247.0;//
~leftVideoEdge6 = -549.0;//
~lastPos6 = 0.0;
~steps6 = 10;
~togToVideo6 = {arg theChanger, theChanged, args;
	if(theChanged == \distanceFromLastAddress, {
		var val, videoPos, stream, duration;
		duration = 0.2;
		val = theChanger.distanceFromLastAddress;
		videoPos = val.linlin(~leftEdge6, ~rightEdge6, ~leftVideoEdge6, ~rightVideoEdge6, \none);
		stream = Env([~lastPos6.asFloat, videoPos.asFloat], [duration]).asPseg.asStream;
		//"pos: % videoPos: %".format(val, videoPos).postln;
		//~video.sendMsg('/video/tog', videoPos);
		fork{
			10.do{arg i;
				~video.sendMsg('/video/tog06', stream.next);
				//stream.next.postln;
				0.02.wait;
			}
		};
		~lastPos6 = videoPos;
	});
};
~lok["TOG06"].addDependant(~togToVideo6);

//tog 7
~rightEdge7 = 0;
~leftEdge7 = -12389;
~rightVideoEdge7 = 323.0;//
~leftVideoEdge7 = -259.0;//
~lastPos7 = 0.0;
~steps7 = 10;
~togToVideo7 = {arg theChanger, theChanged, args;
	if(theChanged == \distanceFromLastAddress, {
		var val, videoPos, stream, duration;
		duration = 0.2;
		val = theChanger.distanceFromLastAddress;
		videoPos = val.linlin(~leftEdge7, ~rightEdge7, ~leftVideoEdge7, ~rightVideoEdge7, \none);
		stream = Env([~lastPos7.asFloat, videoPos.asFloat], [duration]).asPseg.asStream;
		//"pos: % videoPos: %".format(val, videoPos).postln;
		//~video.sendMsg('/video/tog', videoPos);
		fork{
			10.do{arg i;
				~video.sendMsg('/video/tog07', stream.next);
				//stream.next.postln;
				0.02.wait;
			}
		};
		~lastPos7 = videoPos;
	});
};
~lok["TOG07"].addDependant(~togToVideo7);

//tog 10
~rightEdge10 = 0;
~leftEdge10 = 7101;
~rightVideoEdge10 = 0.0;//
~leftVideoEdge10 = -821.0;//
~lastPos10 = 0.0;
~steps10 = 10;
~togToVideo10 = {arg theChanger, theChanged, args;
	if(theChanged == \distanceFromLastAddress, {
		var val, videoPos, stream, duration;
		duration = 0.2;
		val = theChanger.distanceFromLastAddress;
		videoPos = val.linlin(~leftEdge10, ~rightEdge10, ~leftVideoEdge10, ~rightVideoEdge10, \none);
		stream = Env([~lastPos10.asFloat, videoPos.asFloat], [duration]).asPseg.asStream;
		//"pos: % videoPos: %".format(val, videoPos).postln;
		//~video.sendMsg('/video/tog', videoPos);
		fork{
			10.do{arg i;
				~video.sendMsg('/video/tog10', stream.next);
				//stream.next.postln;
				0.02.wait;
			}
		};
		~lastPos10 = videoPos;
	});
};
~lok["TOG10"].addDependant(~togToVideo10);
)

~lok["TOG02"].removeDependant(~togToVideo2);
~lok["TOG06"].removeDependant(~togToVideo6);
~lok["TOG07"].removeDependant(~togToVideo7);
~lok["TOG08"].removeDependant(~togToVideo8);
~lok["TOG10"].removeDependant(~togToVideo10);


Tdef(\tracking).stop;
~togName = "TOG08";
~lok[~togName].get(\lastDetectedAddress);
~lok[~togName].set(\lastDetectedAddress, 3);

~lok[~togName].get(\distanceFromLastAddress);
~lok[~togName].set(\distanceFromLastAddress, 0);

~lok[~togName].set(\distancePollingInterval, 500);
~lok[~togName].set(\distancePollingInterval, 0);
~lok[~togName].set(\speed, 0)
~lok[~togName].set(\speed, 200)
~lok[~togName].set(\direction, 0);
~lok[~togName].set(\direction, 1);
~lok[~togName].get(\distanceFromLastAddress);
~lok[~togName].get(\speed);
~lok[~togName].set(\stop, 1000)


(
~stations = Dictionary[
	1 -> (position: 0, nextUp: 2, nextDown: 3, videoPosition: -325),
	2 -> (position: 1564, nextUp: 3, nextDown: 1, videoPosition: 15),
	3 -> (position: 3521, nextUp: 1, nextDown: 2, videoPosition: 271),
];

)
(
~getVideoPosition = {arg station, distanceFromLastAddress;
	var currentStation,result;
	currentStation = ~stations[station];
	result = currentStation[\position] - ~stations[currentStation[\nextDown]][\position] + distanceFromLastAddress;//select by direction here
	result.postln;
};
)
~getVideoPosition.value(1, -155);