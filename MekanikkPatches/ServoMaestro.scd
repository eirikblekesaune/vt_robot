p = SerialPort("/dev/tty.usbserial-A6008hMF", 9600, crtscts:true);
Tdef(\read, {
	var packet, state;
	loop{
		var byte;
		byte = p.read;
		[byte, byte.asAscii].postln;
	}
}).play;

~com
p.putAll([0, 1] ++ {arg item; [item.bitAnd(0xFF), (item >> 8).bitAnd(0xFF)]}.value(0));
p.putAll([1, 1] ++ {arg item; [item.bitAnd(0xFF), (item >> 8).bitAnd(0xFF)]}.value(1000));
p.putAll([2, 1] ++ {arg item; [item.bitAnd(0xFF), (item >> 8).bitAnd(0xFF)]}.value(400));

97.asAscii