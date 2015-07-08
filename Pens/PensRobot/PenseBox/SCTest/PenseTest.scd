SerialPort.listDevices;


(
p = SerialPort('/dev/tty.usbmodem411', 57600);
Tdef(\reader, {
	var msg;
	loop{
		var val;
		val = p.read.postln;
		if(val == 13, {
			if(p.read == 10, {
				String.newFrom(msg.collect(_.asAscii)).postln;
			});
			msg = nil;
		}, {
			msg = msg.add(val);
		});
	}
}).play;
)

"P1_0".ascii ++ [13, 10]
p.putAll("P1_0".ascii ++ [13, 10]);

Tdef(\reader).clear