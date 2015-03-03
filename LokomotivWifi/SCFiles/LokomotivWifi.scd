p = SerialPort("/dev/tty.usbserial-13X2XBD8", 57600);

Tdef(\pRead, { loop{ "BYTE:%".format(p.read).postln; } }).play

NetAddr("10.0.0.21", 0x2616).sendMsg(7);
NetAddr("1.2.3.151", 0x2616).sendMsg('/bjarne', 7);

OSCFunc.trace
OSCdef(\trainListen, {arg msg, time, addr, port; ["JEAHH", msg, time, addr, port].postln;}, '/hei')
(
var msg, val = 993.rand2;
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
			300.reciprocal.wait;
		}
	}).play;
)
Tdef('pushin').stop;
Tdef('pushin').play;

9992.asBinaryString(32)
NetAddr.localAddr.sendMsg("/ting/t", 3);



a = VTLokomotiv2.new(NetAddr("1.2.3.151", 0x2616));
a.set(\speed, 200);
a.get(\speed);
