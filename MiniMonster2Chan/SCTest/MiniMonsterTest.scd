(
//p = SerialPort('/dev/tty.usbserial-A40014xj', 57600);
p = SerialPort('/dev/tty.usbserial-FT5QT6TQA', 57600);
Tdef(\readPort, {
	loop{
		var byte;
		byte = p.read;
		[byte, byte.asAscii, byte.asBinaryString].postln;
	}
}).play;
~setMotorSpeed = {arg num, val;
	var result, msb, lsb;
	var value = val.clip(-500, 500);
	msb = (num << 4).bitOr(val.isNegative.asInt << 3).bitOr(val.abs >> 7);
	lsb = val.abs % 0x7F;
	result = [$M.ascii, msb, lsb, 13, 10];
	//["out",msb,lsb,val.abs.bitAnd(0x3F)].collect(_.asBinaryString).postln;
	//	["out",msb,lsb,val.abs.bitAnd(0x3F)].postln;
	p.putAll(result.as(Int8Array));
};
~setRelay = {arg val;
	var result;
	result = [$R.ascii, val.asBoolean.asInteger, 13, 10];
	p.putAll(result.as(Int8Array));
};
)
p.put($?.ascii);
(
	w = Window.new;
	w.view.layout_(
		VLayout(
			Slider().action_({|slid|
				~setMotorSpeed.value(1, slid.value.linlin(0.0, 1.0, -511, 511).asInt;);
			}).value_(0.5),
			Slider().action_({|slid|
				~setMotorSpeed.value(2, slid.value.linlin(0.0, 1.0, -511, 511).asInt;);
			}).value_(0.5),
			Slider().action_({|slid|
				~setMotorSpeed.value(3, slid.value.linlin(0.0, 1.0, -511, 511).asInt;);
			}).value_(0.5)
		)
	);
	w.front;
)


~setMotorSpeed.value(0, 0);
~setMotorSpeed.value(1, 0);
~setMotorSpeed.value(2, 0);
~setMotorSpeed.value(3, -300);
~setMotorSpeed.value(3, 100);
~setMotorSpeed.value(3, 0);
~setMotorSpeed.value(2, 0);
~setMotorSpeed.value(1, -100);
~setMotorSpeed.value(1, -127)

~setRelay.value(0);
~setRelay.value(1);



[77, 123, 13, 10].collect(_.asAscii)
$M.ascii

(2r11000011).bitXor(2r10000000).asBinaryString
(0x80).asBinaryString
