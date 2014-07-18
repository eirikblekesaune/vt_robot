~video.sendMsg('/video/tog08', -295.0);
~video.sendMsg('/video/tog06', -247.0);
~video.sendMsg('/video/tog06', 0.0);
~video.sendMsg('/video/figur01', 1.0);

~figur[1].name
(
var num = 7;
~myTrackingData = ~trackingData[num];
~myTog = ~figur[num];
~myFigurPath = ~myTrackingData[\videoOSCPath];
)
//2. Kjor video til onsket posijon
~myTrackingData[\startVideoTracking].value
~myTrackingData[\stopVideoTracking].value
~video.sendMsg(~myFigurPath, ~myTrackingData[\rightVideoEdge])
~video.sendMsg(~myFigurPath, ~myTrackingData[\leftVideoEdge])
~myTog.get(\distanceFromLastAddress);//1330
~myTog.set(\distanceFromLastAddress, 0);//3. shift + enter på denne linja. Dette setter telleverk til null -> Gå til Lokomotiver.scd
~myTog.bipolarSpeed_( -200 );
~myTog.bipolarSpeed_( 0 );
~myTrackingData[\rightEdge] = -7091;//NB store this!!!
~speedOffsetRange = 0.0;

~myTog.get(\speed);
~myTog.set(\distancePollingInterval, 200);
~myTog.set(\distancePollingInterval, 0);
~myTog.get(\distancePollingInterval);
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



///kalibrering av telleverk, shift + enter
~figur[1].set(\distanceFromLastAddress, 0);
~figur[2].set(\distanceFromLastAddress, 0);
~figur[3].set(\distanceFromLastAddress, 0);
~figur[4].set(\distanceFromLastAddress, 0);
~figur[5].set(\distanceFromLastAddress, 0);
~figur[6].set(\distanceFromLastAddress, 0);
~figur[7].set(\distanceFromLastAddress, 0);
~figur[8].set(\distanceFromLastAddress, 0);
~figur[9].set(\distanceFromLastAddress, 0);
~figur[10].set(\distanceFromLastAddress, 0);
~figur[11].set(\distanceFromLastAddress, 0);
~figur[12].set(\distanceFromLastAddress, 0);
~figur[13].set(\distanceFromLastAddress, 0);
