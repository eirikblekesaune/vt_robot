// SerialPort.devices
// ~tallerkenTo = SerialPort('/dev/tty.usbserial-FT3OMYLUA', 2400);
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
//     ~tallerkenTo.putAll(msg);
//     //"msg: %".format(msg).postln;
// };
