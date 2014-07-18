f.stop;g.stop;p.close;q.close;a =nil; ~slider.close;
(
p = SerialPort('/dev/tty.usbserial-FTCVPEA5', 9600, crtscts: true);
q = SerialPort('/dev/tty.usbmodem411', 9600);
a = VTLokomotiv(q);
f = fork{
	loop{
		var byte = p.read;
		"Mini: byte: %, ascii: %".format(byte, byte.asAscii).postln;
	}
};
g = fork{
	loop{
		var byte = q.read;
		"LOK: %".format(byte).postln;
		a.prParseByte(byte);
	}
};
)
p.putAll([77, 0x00, 0x07, 0xFF, 13, 10]);
p.putAll([77, 0x00, 0, 0, 13, 10]);

SCVim.generateTagsFile;
//set led
p.putAll([77, 0x05, 0x03, 0xFF, 13, 10]);
p.putAll([77, 0x05, 0x00, 0x00, 13, 10]);

p.putAll([77, 0x01, 0x80 | 0x07, 0xFF, 13, 10]);
p.putAll([77, 0x01, 0x80 | 0x00, 0x00, 13, 10]);

//flash led
//led 1
p.putAll([77, 0x01, 0x08, 0x1F, 13, 10]);

//led 2
p.putAll([77, 0x06, 0x00, 0x05, 13, 10]);

//set TWI leo comm
q.putAll([77, 2, 0x02, 0x80 | 0x01, 0xFF, 13, 10]);

q.putAll([77, 2, 0x01, 0x80 | 0x05, 0x00, 13, 10]);
VTModule2.new()
(
~slider = Slider.new(bounds: Rect(0, 0, 800, 50)).action_({arg slid;
	var val;
	val = slid.value.linlin(0.0, 1.0, 0, 2047).asInteger;
	//val = ~lookup.at(slid.value.linlin(0.0, 1.0, 0, 2047).asInteger).postln;
	//q.putAll([77, 2, 0x01, 0x80 | (val.bitAnd(0x0700) >> 8), val.bitAnd(0x00FF), 13, 10]);
	a.set(\peripheral, (1 << 24) | (8 << 16) | val );
}).front
)

a.set(\peripheral, (1 << 24) | (6 << 16) | 0xFF);
a.set(\peripheral, (1 << 24) | (7 << 16) | 250);
a.set(\peripheral, (1 << 24) | (8 << 16) | 1900);
a.set(\peripheral, (1 << 24) | (8 << 16) | 0);

a.set(\peripheralRequest, (1 << 24) | (5 << 16));

(1720 - 1001) / (1000 / 50)
(2r11001011).bitNot.bitAnd(2r00001111).asBinaryString


(
//only serial
p = SerialPort('/dev/tty.usbserial-FTCVPEA5', 9600, crtscts: true);
f = fork{
	loop{
		var byte = p.read;
		"Mini: byte: %, ascii: %".format(byte, byte.asAscii).postln;
	}
};
OSCdef(\flashLed2, {"flash".postln;p.putAll([77, 0x06, 0x00, 0x01, 13, 10]);}, '/led.1/flash');
OSCdef(\setLed2, {arg msg, addr, time, port;
    var valHi, valLo;
    valHi = (msg[1] >> 8).bitAnd(0xFF);
    valLo = (msg[1]).bitAnd(0xFF);
    "led1 set: %".format(msg[1]).postln;
    p.putAll([77, 0x05, valHi, valLo, 13, 10]);
},'/led.1/value')
)
p.putAll([77, 0x06, 0x00, 0x0F, 13, 10]);
p.putAll([77, 0x05, 0x07, 0xFF, 13, 10]);
p.putAll([77, 0x05, 0x00, 0x00, 13, 10]);