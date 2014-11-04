~lanbox = SerialPort.new("/dev/tty.usbmodem621", 38400, crtscts: true);
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

[16,20,13,14,15].do{arg ledChannel, i;
	var midiComp, dmx;
	var rotaryNumber = #[1,2,3,4,5][i];
	midiComp = ~togCtrlA.components.at(("rotaryA.%".format(rotaryNumber).asSymbol));
	midiComp.postln;
	midiComp.action_({arg comp;
		var val, commandString;
		val = comp.value.linlin(0, 127, 0, 255).asInteger.asHexString(2);
		commandString = ("*C901" ++ ledChannel.asHexString(2) ++ val ++ "#").ascii;
		~lanbox.putAll(commandString);
		"dmx % : %\n".postf(ledChannel, val);
	}.inEnvir);
};