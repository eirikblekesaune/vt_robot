MIDIClient.init;
MIDIIn.connectAll;

~bcfMappings = IdentityDictionary.new;
(81..88).do({arg item, i; ~bcfMappings.put(item.asSymbol, "fader.%".format(i+1).asSymbol); });
(1..8).do({arg item, i; ~bcfMappings.put(item.asSymbol, "rotary.%".format(i+1).asSymbol); });
(73..80).do({arg item, i; ~bcfMappings.put(item.asSymbol, "button.%/A".format(i+1).asSymbol); });
(65..72).do({arg item, i; ~bcfMappings.put(item.asSymbol, "button.%/B".format(i+1).asSymbol); });
(33..40).do({arg item, i; ~bcfMappings.put(item.asSymbol, "button.%/C".format(i+1).asSymbol); });
~sendActorData = {arg addr, val;
    NetAddr("1.2.3.44", 1234).sendMsg(addr, val);
    NetAddr("1.2.3.201", 50000).sendMsg(addr, val);
    NetAddr("1.2.3.111", 50000).sendMsg(addr, val);
    NetAddr("1.2.3.57", 57120).sendMsg(addr, val);
};
try{
    ["Elisabeth", "Martin"].do{arg name;
        try{
            var midiOut, portName;
            portName = "BCF2000%".format(name);
            midiOut = MIDIOut.newByName(portName, "Port 1");
            MIDIdef.cc(name.toLower.asSymbol, {arg val, num, chan, src;
                var addr, outVal;
                outVal = val.linlin(0, 127, 0.0, 1.0);
                addr = "/%/control/%".format(name.toLower, ~bcfMappings.at(num.asSymbol)).asSymbol;
                "%: % \n %: %".format(name, [num, val, chan, src], addr, outVal).postln;
                ~sendActorData.value(addr, outVal);
            }, srcID: MIDIIn.findPort(portName, "Port 1").uid);
            ~bcfMappings.keysValuesDo({arg key, val;
                OSCFunc({arg msg, time, addr, port;
                    midiOut.control(0, key.asInteger, msg[1].linlin(0.0, 1.0, 0,127).asInteger);
                }, "/%/control/%".format(name.toLower, val).asSymbol);
            });
        } {arg err; "% BCF2000 is not connected!".format(name).error.throw;}
    };

    try{
        MIDIdef.program(\MartinFotbrett, { arg val, chan;
            "Martin fotbrett: chan: % val: %".format(chan + 1, val + 1).postln;
            ~sendActorData.value("/martin/control/fotbrett.%".format(chan + 1).asSymbol, val + 1);
        }, srcID:  MIDIIn.findPort("MartinFotbrett", "USB Uno MIDI Interface").uid);
        MIDIdef.sysrt(\MartinFotbrettRT, { arg val;
            var msgDict = ('10': "start", '12': "stop");
            "Martin fotbrett sysrt: %".format(msgDict.at(val.asSymbol)).postln;
            ~sendActorData.value("/martin/control/fotbrett/%".format(msgDict.at(val.asSymbol)).asSymbol);
        });
    } {arg err;
        "Martin fotpedal is not connected!".error.throw;
    };
    try{
        "~/vt_robot/MekanikkPatches/TallerkenTo.scd".standardizePath.load;
    }{"Loading TallerkenTo failed".error.postln;};
    "Actor conrol ready".postln;
}{"Error starting actor control patch".error.postln;};

"~/vt_robot/MekanikkPatches/Tallerkener.scd".standardizePath.load;
"~/vt_robot/MekanikkPatches/Penser.scd".standardizePath.load;
"~/vt_robot/MekanikkPatches/AliControl.scd".standardizePath.load;
"~/vt_robot/MekanikkPatches/HvitFugl.scd".standardizePath.load;