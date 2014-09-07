~kransProto = Environment[
	\port -> nil,//the serial port instance
	\number -> nil,//the krans number
	\name -> nil,
	\speed -> 0.0,
	\setSpeed -> {arg env, speed;
		env.use{
			var bytes;
			~speed = speed.clip(-1.0, 1.0).linlin(-1.0, 1.0, 1.0, -1.0);
			bytes = ~formatMsgBytes.value(env, ~speed);
			~port.putAll(bytes);
			"Krans % speed: %, bytes: %".format(~number, ~speed, bytes).postln;
			env.changed(\speed);
			~lastSpeedChange = Main.elapsedTime;
		};
	},
	\init -> {arg env;
		env.use{
			~oscResponder = OSCFunc({arg msg, time, addr, port;
				~setSpeed.value(currentEnvironment, msg[1]);
			}.inEnvir, "/robot/krans.%/speed".format(~number).asSymbol);
			~lastSpeedChange = Main.elapsedTime;
		};
	},
	\clear -> {arg env; env.use{
		~oscResponder.clear;
	};},
	\lookup -> [(122..97), (65..90)].flat,
	\getSpec -> {arg env;
		ControlSpec(0, env[\lookup].size -1, step: 1, default: 0);
	},
	\formatMsgBytes -> {arg env, speed;
		var result, speedByte, spec;
		env.use{
			spec = ~getSpec.value(env);
			speedByte = ~lookup[spec.map(\bipolar.asSpec.unmap(speed)).asInteger].asInteger;
			result = "%%".format(~channel, speedByte.asAscii);
		};
		result;
	}
];

~kransBox = SerialPort('/dev/tty.usbserial-FT5QT6W2A', 2400, crtscts:true);
Tdef(\kransRead, {loop{~kransBox.read.postln;}}).play;
~setKransSpeed = {arg kransNumber, speed;
	~kranser[kransNumber][\setSpeed].value(~kranser[kransNumber], speed);
};
~kranser = Dictionary.new();
~kranser.put(1, Environment.new(
	proto: ~kransProto, know: true).putAll((
		number: 1, port: ~kransBox, channel: 1
	));
);
~kranser.put(2, Environment.new(
	proto: ~kransProto, know: true).putAll((
		number: 2, port: ~kransBox, channel: 2
	));
);
~kranser.put(3, Environment.new(
	proto: ~kransProto, know: true).putAll((
		number: 3, port: ~kransBox, channel: 3
	));
);
~kranser.do{arg item; item[\init].value(item);};


~kransFaders = Dictionary.new;
~kransStopButtons = Dictionary.new;

[1,2,3].do{arg number;
	var speedCtrlComp, stopCtrlComp, faderSpec;
	speedCtrlComp = Dictionary[1 -> 'fader.7', 2 -> 'fader.8', 3 -> 'rotaryA.8'].at(number);
	stopCtrlComp = Dictionary[1 -> 'button.15', 2 -> 'button.16', 3 -> 'rotaryButtonA.8'].at(number);
	faderSpec = Dictionary[1 -> [0, 1023].asSpec, 2 -> [0, 1023].asSpec, 3 -> [0, 127].asSpec].at(number);
	~kransFaders.put(number,
		~togCtrlA.components.at(speedCtrlComp);
	);
	~kransStopButtons.put(number,
		~togCtrlA.components.at(stopCtrlComp);
	);
	~kransFaders[number].action = {arg ctrlComp;
		var val;
		val = ctrlComp.value.linlin(faderSpec.minval, faderSpec.maxval, -1.0, 1.0);
		"Krans % - bipolarSpeed: %".format(number, val).postln;
		~setKransSpeed.value(number, val);
	};
	~kransStopButtons[number].action = {arg ctrlComp;
		"Krans % stopp".format(number).postln;
		~setKransSpeed.value(number, 0);
		~kransFaders[number].value_((faderSpec.maxval - faderSpec.minval / 2).asInteger).refresh;
	};
	OSCdef("kransResponer%".format(number).asSymbol, {arg msg, time, addr, port;
		~setKransSpeed.value(number, msg[1]);
	},"/robot/krans.%/bipolarSpeed".format(number).asSymbol);
};

//Autostopper
Tdef(\kransAutostopper, {
	loop{
		~kranser.keysValuesDo({arg number, krans;
			if((Main.elapsedTime - krans[\lastSpeedChange]) > 30, {
				if(krans[\speed] < 0.0 or: {krans[\speed] > 0.0}, {
					var faderSpec;
					faderSpec = Dictionary[1 -> [0, 1023].asSpec, 2 -> [0, 1023].asSpec, 3 -> [0, 127].asSpec].at(number);
					"Autostopping krans %".format(krans[\number]).postln;
					krans[\setSpeed].value(krans, 0);
					~kransFaders[number].value_((faderSpec.maxval - faderSpec.minval / 2).asInteger).refresh;
				});
			});
		});
		1.0.wait;
	}
}).play;

