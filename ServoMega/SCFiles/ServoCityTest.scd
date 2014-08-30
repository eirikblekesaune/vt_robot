(
~servoCity = SerialPort("/dev/tty.usbmodem411", 9600);
~parseServoCityState = \waitHeader;
~parseServoCityData = nil;
~parseServoCityDebug = {arg byte;
	switch(byte,
		\waitHeader, {(byte == 0x04).if({~parseServoCityState = \waitString;})},
		\waitString, {
			var char = char.asAscii;
			if(char.isAlphaNum or: {char == Char.space},
				{~parseServoCityData = ~parseServoCityData.add(byte);},
				{~parseServoCityState = \waitCR;}
			)
		},
		\waitCR, {(byte == 13).if(
			{~parseServoCityState = \waitNL;},
			{~parseServoCityState = \waitHeader;})
		},
		\waitNL, {(byte == 10).if(
			{String.newFrom(~parseServoCityData.collect(_.asAscii)).postln;},
			{~parseServoCityState = \waitHeader;});
			~parseServoCityState = \waitHeader;
		}
	);
};
Tdef(\servoCityReader, {
	loop{ ~servoCity.read.postln; }
}.inEnvir).play;
)
