p = SerialPort("/dev/tty.usbserial-13X2XBD8", 57600);

Tdef(\pRead, { loop{ "BYTE:%".format(p.read).postln; } }).play

NetAddr("10.0.0.21", 0x2616).sendMsg(7);
NetAddr("192.168.1.101", 0x2616).sendMsg('/bjarne', 7);

OSCFunc.trace
OSCdef(\trainListen, {arg msg, time, addr, port; ["JEAHH", msg, time, addr, port].postln;}, '/hei')
(
var msg, val = 993;
msg = "/hei".ascii ++ [0,0,0,0];
msg = msg ++ ",i".ascii ++ [0, 0];
msg = msg ++ [(val >> 24).bitAnd(0xFF), (val >> 16).bitAnd(0xFF), (val >> 8).bitAnd(0xFF), (val >> 0).bitAnd(0xFF)];
p.putAll(msg.postln);
)
"hei".ascii
57120.asHexString

0x0F.asBinaryString
(
	Tdef('pushin', {
		loop{
			var msg, val = 993.rand2;
			msg = "/hei".ascii ++ [0,0,0,0];
			msg = msg ++ ",i".ascii ++ [0, 0];
			msg = msg ++ [(val >> 24).bitAnd(0xFF), (val >> 16).bitAnd(0xFF), (val >> 8).bitAnd(0xFF), (val >> 0).bitAnd(0xFF)];
			p.putAll(msg.postln);
			50.reciprocal.wait;
		}
	}).play;
)
Tdef('pushin').stop;
Tdef('pushin').play;

1/0.03
33.333.reciprocalk
