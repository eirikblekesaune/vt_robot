~tallerkenProto = Environment[
    \port -> nil,//the serial port instance
    \number -> nil,//the tallerken number
    \name -> nil,
    \speed -> 0.0,
    \setSpeed -> {arg env, speed;
        env.use{
            var bytes;
            ~speed = speed.clip(-1.0, 1.0);
            bytes = ~formatMsgBytes.value(env, ~speed);
            ~port.putAll(bytes);
            "Tallerken % speed: %".format(~speed).postln;
            env.changed(\speed);
        };
    },
    \init -> {arg env; env.use{
        ~oscResponder = OSCFunc({arg msg, time, addr, port;
            ~setSpeed.value(env, msg[1]);
        }.inEnvir, "/robot/tallerken.%/speed".format(~number).asSymbol);
    };},
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
//~tallerken.do{arg item; item.proto_(~tallerkenProto)};

//Mini monster box 1 is where tallerken 1 and 2 are controlled
~miniMonsterBox1 = SerialPort('/dev/tty.usbserial-FT3OMYLUA', 2400, crtscts:true);
~miniMonsterBox2 = SerialPort('/dev/tty.usbserial-FT3OMYLUB', 2400, crtscts:true);
~tallerken = Dictionary[];
~tallerken.put(4,
    Environment.new(proto: ~tallerkenProto, know: true).putAll((
        number: 4, port: ~miniMonsterBox1, channel: 2
    ))
);

~tallerken.put(3,
    Environment.new(proto: ~tallerkenProto, know: true).putAll((
        number: 3, port: ~miniMonsterBox1, channel: 1
    ))
);
~tallerken.put(2,
    Environment.new(proto: ~tallerkenProto, know: true).putAll((
        number: 2, port: ~miniMonsterBox2, channel: 2
    ))
);

~tallerken.put(1,
    Environment.new(proto: ~tallerkenProto, know: true).putAll((
        number: 1, port: ~miniMonsterBox2, channel: 1
    ))
);
~tallerken.do{arg item; item[\init].value(item);};

//
//
//
// //Tallerkener
// ~tallerken = SerialPort('/dev/tty.usbserial-FT5QT6W2A', 2400, crtscts:true);
// ~tallerkenLookup = [(122..97), (65..90)].flat;
// ~tallerkenSpec = ControlSpec(
//     (~tallerkenLookup.size / 2).neg,
// (~tallerkenLookup.size / 2), step: 1, default: 0);
// ~tallerkenSlider = Slider();
// ~setTallerkenSpeed = {arg num, val;
//     var msg;
//     val = val.linlin(-1.0, 1.0, 0, ~tallerkenLookup.size).asInteger;
//     //"val: %[%]".format(val, val.class).postln;
//     msg = "%%".format(
//         num,
//         ~tallerkenLookup[val.clip(0, ~tallerkenLookup.size)].asAscii
//     );
//     ~tallerken.putAll(msg);
//     //"msg: %".format(msg).postln;
// };
// OSCdef(\tallerken1, {arg msg, time, addr, port;
//     "Tallerken 1 speed: %".format(msg[1]).postln;
//     ~setTallerkenSpeed.value(1, msg[1].clip(-1.0, 1.0));
// }, '/robot/tallerken.1/speed');
// OSCdef(\tallerken2, {arg msg, time, addr, port;
//     "Tallerken 2 speed: %".format(msg[1]).postln;
//     ~setTallerkenSpeed.value(2, msg[1].clip(-1.0, 1.0));
// }, '/robot/tallerken.2/speed');
// OSCdef(\tallerken3, {arg msg, time, addr, port;
//     "Tallerken 3 speed: %".format(msg[1]).postln;
//     ~setTallerkenSpeed.value(3, msg[1].clip(-1.0, 1.0));
// }, '/robot/tallerken.3/speed');
// Tdef(\tallerkenListen, {
//     loop{
//         "tallerken input: %".format(~tallerken.read.asAscii).postln;
//     }
// }).play;
// //w = Window.new.front;
// //w.onClose_({0.exit});