//Tallerkener
~tallerken = SerialPort('/dev/tty.usbserial-FT5QT6W2A', 2400, crtscts:true);
~tallerkenLookup = [(122..97), (65..90)].flat;
~tallerkenSpec = ControlSpec(
	(~tallerkenLookup.size / 2).neg,
	(~tallerkenLookup.size / 2), step: 1, default: 0);
~tallerkenSlider = Slider();
~setTallerkenSpeed = {arg num, val;
	var msg;
	val = val.linlin(-1.0, 1.0, 0, ~tallerkenLookup.size).asInteger;
	//"val: %[%]".format(val, val.class).postln;
	msg = "%%".format(
		num,
		~tallerkenLookup[val.clip(0, ~tallerkenLookup.size)].asAscii
	);
	~tallerken.putAll(msg);
	//"msg: %".format(msg).postln;
};
OSCdef(\tallerken1, {arg msg, time, addr, port;
	"Tallerken 1 speed: %".format(msg[1]).postln;
	~setTallerkenSpeed.value(1, msg[1].clip(-1.0, 1.0));
}, '/robot/tallerken.1/speed');
OSCdef(\tallerken2, {arg msg, time, addr, port;
	"Tallerken 2 speed: %".format(msg[1]).postln;
	~setTallerkenSpeed.value(2, msg[1].clip(-1.0, 1.0));
}, '/robot/tallerken.2/speed');
OSCdef(\tallerken3, {arg msg, time, addr, port;
	"Tallerken 3 speed: %".format(msg[1]).postln;
	~setTallerkenSpeed.value(3, msg[1].clip(-1.0, 1.0));
}, '/robot/tallerken.3/speed');
Tdef(\tallerkenListen, {
	loop{
		"tallerken input: %".format(~tallerken.read.asAscii).postln;
	}
}).play;
//w = Window.new.front;
//w.onClose_({0.exit});