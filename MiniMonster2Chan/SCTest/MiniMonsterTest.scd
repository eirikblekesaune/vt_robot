(
p = SerialPort('/dev/tty.usbserial-A40014xj', 57600);
Tdef(\readPort, {
	loop{
		var byte;
		byte = p.read;
		[byte, byte.asAscii, byte.asBinaryString].postln;
	}
}).play;
)


(
~setMotorSpeed = {arg num, val;
	var result, msb, lsb;
	msb = (num << 4).bitOr(val.isNegative.asInt << 3).bitOr(val.abs.bitAnd(0x3F) >> 8);
	lsb = val.abs % 0x7F;
	result = [$M.ascii, msb, lsb, 13, 10];
	[msb,lsb,val.abs.bitAnd(0x3F)].collect(_.asBinaryString).postln;
	p.putAll(result.as(Int8Array));
};
~setRelay = {arg val;
	var result;
	result = [$R.ascii, val.asBoolean.asInteger, 13, 10];
	p.putAll(result.as(Int8Array));
};
)



~setMotorSpeed.value(1, 0);
~setMotorSpeed.value(1, -10);
~setMotorSpeed.value(1, 0);

~setRelay.value(0);
~setRelay.value(1);



[77, 123, 13, 10].collect(_.asAscii)
$M.ascii

(2r11000011).bitXor(2r10000000).asBinaryString
(0x80).asBinaryString