~penser = SerialPort("/dev/tty.usbserial-FT3OMYMJB", 2400, crtscts:true);


Tdef(\pensReader, {
    loop{ ~penser.read; }
}.inEnvir).play;

12.do{arg i;
    var number;
    number = i + 1;
    OSCdef("pensResponder%".format(number).asSymbol, {arg msg, time, addr, port;
        var value;
        value = msg[1];
        "Pens %: %".format(number, value).postln;
        ~penser.putAll("S%%".format(number.asPaddedString, value).ascii ++ [13, 10]);
    }, "/robot/pens.%/position".format(number).asSymbol);
};
