(
~servoCity = SerialPort("/dev/tty.usbmodem411", 9600);
~parseServoCityState = \waitHeader;
~parseServoCityData = nil;
~parseServoCityDebug = {arg byte;
	switch(~parseServoCityState,
		\waitHeader, {(byte == 4).if({
			~parseServoCityState = \waitString;
		})},
		\waitString, {
			if(byte != 13,
				{~parseServoCityData = ~parseServoCityData.add(byte);},
				{~parseServoCityState = \waitNL;}
			)
		},
		\waitNL, {(byte == 10).if(
			{
				"ServoDebug: %".format(String.newFrom(~parseServoCityData.collect(_.asAscii))).postln;
				~parseServoCityData = nil;
			},
			{
				~parseServoCityState = \waitHeader;
			});
			~parseServoCityState = \waitHeader;
		}
	);
}.inEnvir;
Tdef(\servoCityReader, {
	loop{ ~parseServoCityDebug.value(~servoCity.read); }
}.inEnvir).play;
)


~servoCity.putAll([0xAA, 0x00, 1, 0, 0, 13, 10]);

~servoCity.putAll([0xAA, 0x02, 1, 10, 0, 13, 10]);

~servoCity.putAll([0xAA, 0x01, 1, 30, 0, 13, 10]);
~servoCity.putAll([0xAA, 0x01, 1, 0, 0, 13, 10]);

(
var min, max;
min = 800;
max = 2500;
~servoCity.putAll([0xAA, 0x04, 1] ++ [(min >> 8).bitAnd(0xFF), min.bitAnd(0xFF)] ++ [ 13, 10] );
~servoCity.putAll([0xAA, 0x05, 1] ++ [(max >> 8).bitAnd(0xFF), max.bitAnd(0xFF)] ++ [ 13, 10] );
)