p = SerialPort("/dev/tty.usbserial-13X2XBD8", 57600);
Tdef(\pRead).clear;
Tdef(\pRead, { loop{var it = p.read; "BYTE:\t%\t%\t%".format(it, it.asHexString(2),it.asAscii).postln; } }).play
OSCFunc.trace
OSCFunc.trace(false);
OSCdef(\figTrack, {arg msg, time, addr, port; ["position", msg, time].postln;}, '/t/A>')
OSCdef(\figStation, {arg msg, time, addr, port; ["station", msg, time].postln;}, '/t/l>')

NetAddr("10.0.0.21", 0x2616).sendMsg("/t/s>", 400);
NetAddr("10.0.0.21", 0x2616).sendMsg("/t/s>", 0);
NetAddr("10.0.0.21", 0x2616).sendMsg("/t/d>", 1);//right
NetAddr("10.0.0.21", 0x2616).sendMsg("/t/d>", 0);//left
NetAddr("10.0.0.21", 0x2616).sendMsg("/t/S>", 5000);
NetAddr("10.0.0.21", 0x2616).sendMsg("/t/S>", 0);

30.reciprocal * 1000
NetAddr("10.0.0.21", 0x2616).sendMsg("/t/T>", 20);
105.asAscii



NetAddr("1.2.3.44", 1234).sendMsg('/figur.3/tracking',1)
(
OSCdef(\figTrack, {arg msg, time, addr, port;
	var leftSide = 0,rightSide = 15765;
	NetAddr("1.2.3.44", 1234).sendMsg('/figur.3/tracking', msg[1].linlin(leftSide, rightSide, 0.0, 1.0));
	~trackingData = msg[1].linlin(leftSide, rightSide, 0.0, 1.0);
	["position", msg, time].postln;}.inEnvir,
	'/t/A>')
)

(
	Tdef(\sendTrack, {loop{
		NetAddr("1.2.3.44", 1234).sendMsg('/figur.3/tracking', ~trackingData);
		30.reciprocal.wait;
	}}.inEnvir).play(SystemClock);
)
Tdef(\sendTrack).stop.clear
("1.2.3.150".split($.).collect(_.asInt) << [24, 16, 8,0]).sum
"1.2.3.150".split($.).collect(_.asInt).collect(_.asHexString(2))
0xC0A80182.asIPString
57120.asHexString
j
(0xDF00).bitOr(0x20)

(0xDF << 8) + 0x20
(223 << 8) + 32
0x20

(
~sendHei = {
	var msg, start, checksum, length;
	start = 0x7E; //start delimiter

	msg = [
		0x20, //api frame ID
		0x00, //frame ID
		10, 0, 0, 25,//the IP address
		0xDF, 0x20, //the port number 57120
		0x26, 0x16, //the port number for the XBee module
		0x00, //UDP
		0x00, //reserverd in UDP
		"/hei".ascii, [0,0,0,0], ",".ascii, [0,0,0], //the data to send
		//0x00 //the checksum
	].flat;

	length = [(msg.size >> 8).bitAnd(0xFF), msg.size.bitAnd(0xFF)];
	checksum = 0xFF - (msg.sum.bitAnd(0xFF));
	p.putAll([start, length, msg, checksum].flat.postln);
};
)

//
OSCdef(\heiListen, {arg msg, time, addr, port; [msg, time].postln;}, '/hei')
~sendHei.value;//test connection
//Start stress test
//Test show that sending data from the Arduino to the network is very fast.
Tdef(\heiStress).stop;
Tdef(\heiStress, { loop{ ~sendHei.value; 200.reciprocal.wait;} }.inEnvir).play;

/*
Now testing sending data to the XBee from Supercollider
*/
NetAddr("10.0.0.21", 0x2616).sendMsg("/t/s", 150);

(//Sending data through the network to the XBee WIFI
var msg, length, start, checksum;
start = 0x7E; //start delimiter

msg = [
	0x20, //api frame ID
	0x00, //frame ID
	1, 2, 3, 150,//the IP address
	0xDF, 0x20, //the port number 57120
	0x26, 0x16, //the port number for the XBee module
	0x00, //UDP
	0x00, //reserverd in UDP
	"/hei".ascii, [0,0,0,0], ",".ascii, [0,0,0], //the data to send
	//0x00 //the checksum
].flat;

length = [(msg.size >> 8).bitAnd(0xFF), msg.size.bitAnd(0xFF)];
checksum = 0xFF - (msg.sum.bitAnd(0xFF));

NetAddr("1.2.3.151", 0x2616).sendRaw([start, length, msg, checksum].flat);
)


NetAddr("10.0.0.21", 0x2616).sendMsg('/sdfsdf');
NetAddr("1.2.3.150", 57120).sendMsg('/t/s>', 7);
NetAddr("1.2.3.151", 0x2616).sendMsg('/t/s>', 7);
NetAddr("1.2.3.151", 0x2616).sendMsg('/ttest', 7);
NetAddr("1.2.3.151", 0x2616).sendMsg('/ttest', 7);

OSCFunc.trace
OSCFunc.trace(false);
OSCdef(\trainListen, {arg msg, time, addr, port; ["JEAHH", msg, time, addr, port].postln;}, '/hei')
OSCdef(\trainBeat, {arg msg, time, addr, port; ["alive", msg, time].postln;}, '/t/s>')
OSCdef(\trainInfo, {arg msg, time, addr, port; ["INFO", msg, time].postln;}, '/t/i>')
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


"killall sclang".unixCmd
a = VTLokomotiv2.new(NetAddr("1.2.3.151", 0x2616));
a.set(\speed, 200);
a.get(\speed);
0x03E8kkkkkkkkkk
