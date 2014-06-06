~lanbox = SerialPort.new("/dev/tty.usbmodem471", 38400, crtscts: true);
~lysResponders = 256.collect({arg i;
	var num = i + 1;
	OSCFunc({arg msg, time, addr, port;
		var val, commandString;

		val = msg[1].clip(0, 255).asInteger.asHexString(2);
		commandString = ("*C901" ++ num.asHexString(2) ++ val ++ "#").ascii;
		~lanbox.putAll(commandString);
		"dmx % : %\n".postf(num, val);
	}, ("/light/dmx." ++ (i + 1)).asSymbol);
});

6.do{arg i;
	var midiComp, dmx;
	midiComp = ~togCtrlA.components.at((("rotaryA.") ++ (i + 1)).asSymbol);
	midiComp.postln;
	midiComp.action_({arg comp;
		var val, commandString;
		val = comp.value.linlin(0, 127, 0, 255).asInteger.asHexString(2);
		commandString = ("*C901" ++ (i + 13).asHexString(2) ++ val ++ "#").ascii;
		~lanbox.putAll(commandString);
		"dmx % : %\n".postf((i + 13), val);
	}.inEnvir);
};