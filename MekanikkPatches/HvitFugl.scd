~hvitFuglPort = SerialPort('/dev/tty.usbserial-FT3OMYM8B', 9600, crtscts:true);

~setHvitFuglLiftSpeed = {arg val;
	~hvitFuglPort.putAll([1, val.linlin(0.0, 1.0, 0, 254).asInteger, 13, 10]);
};

~setHvitFuglDirection = {arg val;
	"val: %[%]".format(val, val.class).postln;
	if(val == 'up', {
		val = 1;
	}, {val = 0;});
	"Real direction: %".format(val).postln;
	~hvitFuglPort.putAll([2, val.clip(0, 1), 13, 10]);
};

~setHvitFuglHead= {arg val;
	~hvitFuglPort.putAll([3, val.linlin(-1.0, 1.0, 10, 140).asInteger, 13, 10]);
};

OSCdef('hvitFuglSpeed', {arg msg,time,addr,port;
	"Hvit fugle lift speed: %".format(msg[1]).postln;
	~setHvitFuglLiftSpeed.value(msg[1]);
}, '/robot/hvitFugl/lift/speed');
OSCdef('hvitFuglDirection', {arg msg,time,addr,port;
	"Hvit fugle lift direction %".format(msg[1]).postln;
	~setHvitFuglDirection.value(msg[1]);
}, '/robot/hvitFugl/lift/direction');
OSCdef('hvitFuglHead', {arg msg,time,addr,port;
	"Hvit fugle head: %".format(msg[1]).postln;
	~setHvitFuglHead.value(msg[1]);
}, '/robot/hvitFugl/head/position');
OSCFunc.trace(false)