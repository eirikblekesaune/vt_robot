~kreketProto = Environment[
    \port -> nil,//the serial port instance
    \name -> nil,
    \speed -> 0.0,
    \setSpeed -> {arg env, speed;
        env.use{
            var bytes;
            ~speed = speed.clip(-1.0, 1.0);
            bytes = ~formatMsgBytes.value(env, ~speed);
            ~port.putAll(bytes);
            "Kreket speed: %".format(~speed).postln;
            env.changed(\speed);
        };
    },
    \init -> {arg env; env.use{
        ~oscResponder = OSCFunc({arg msg, time, addr, port;
            ~setSpeed.value(env, msg[1]);
        }.inEnvir, "/robot/kreket/speed".asSymbol);
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

~miniMonsterBox3 = SerialPort('/dev/tty.usbserial-FT3OMYM8B', crtscts:true);

~kreket = Environment.new(proto: ~kreketProto, know: true).putAll((
    port: ~miniMonsterBox3, channel: 1
));

~kreket[\init].value(~kreket)