var bcfMappings = IdentityDictionary.new;
MIDIClient.init;
MIDIIn.connectAll;

(81..88).do({arg item, i; bcfMappings.put(item.asSymbol, "fader.%".format(i+1).asSymbol); });
(1..8).do({arg item, i; bcfMappings.put(item.asSymbol, "rotary.%".format(i+1).asSymbol); });
(73..80).do({arg item, i; bcfMappings.put(item.asSymbol, "button.%/A".format(i+1).asSymbol); });
(65..72).do({arg item, i; bcfMappings.put(item.asSymbol, "button.%/B".format(i+1).asSymbol); });
(33..40).do({arg item, i; bcfMappings.put(item.asSymbol, "button.%/C".format(i+1).asSymbol); });
~sendActorData = {arg addr, val;
    NetAddr("1.2.3.111", 50000).sendMsg(addr, val);
};
try{
	["Elisabeth"].do{arg name;
        try{
            var midiOut, portName;
            portName = "BCF2000%".format(name);
            midiOut = MIDIOut.newByName(portName, "Port 1");
            MIDIdef.cc(name.toLower.asSymbol, {arg val, num, chan, src;
                var addr, outVal;
                outVal = val.linlin(0, 127, 0.0, 1.0);
                addr = "/%/control/%".format(name.toLower, bcfMappings.at(num.asSymbol)).asSymbol;
                "%: % \n %: %".format(name, [num, val, chan, src], addr, outVal).postln;
                ~sendActorData.value(addr, outVal);
            }, srcID: MIDIIn.findPort(portName, "Port 1").uid);
            bcfMappings.keysValuesDo({arg key, val;
                OSCFunc({arg msg, time, addr, port;
                    midiOut.control(0, key.asInteger, msg[1].linlin(0.0, 1.0, 0,127).asInteger);
                }, "/%/control/%".format(name.toLower, val).asSymbol);
            });
        } {arg err; "% BCF2000 is not connected!".format(name).error.throw;}
    };
    "Elisbeth control ready".postln;
}{"Error starting Elisbeth control patch".error.postln;};