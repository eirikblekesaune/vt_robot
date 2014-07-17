(
~lookup = Array.new(2048);
~lookup = ~lookup.add(0);
~lookup.addAll((850..1200).resamp0(99));//1 - 100
~lookup.addAll((1201..1375).resamp0(100));//101 - 200
~lookup.addAll((1376..1415).resamp0(100));//201 - 300
~lookup.addAll((1416..1435).resamp0(100));//301 - 400
~lookup.addAll((1436..1460).resamp0(100));//401 - 500
~lookup.addAll((1461..1480).resamp0(100));//501 - 600
~lookup.addAll((1481..1490).resamp0(100));//601 - 700
~lookup.addAll((1491..1520).resamp0(100));//701 - 800
~lookup.addAll((1521..1540).resamp0(100));//801 - 900
~lookup.addAll((1541..1555).resamp0(100));//901 - 1000
~lookup.addAll((1556..1575).resamp0(100));//1001 - 1100
~lookup.addAll((1576..1580).resamp0(100));//1101 - 1200
~lookup.addAll((1581..1600).resamp0(100));//1201 - 1300
~lookup.addAll((1601..1625).resamp0(100));//1301 - 1400
~lookup.addAll((1626..1660).resamp0(100));//1401 - 1500
~lookup.addAll((1661..1690).resamp0(100));//1501 - 1600
~lookup.addAll((1691..1740).resamp0(100));//1601 - 1700
~lookup.addAll((1741..1770).resamp0(100));//1701 - 1800
~lookup.addAll((1771..1800).resamp0(100));//1801 - 1900
~lookup.addAll((1801..1900).resamp0(100));//1901 - 2000
~lookup.addAll((1901..2047).resamp0(48));//2001 - 2048
)
~lookup.do{arg item; item.postln;}
~lookup.size
~lookup

(
f = File.new("~/vt_robot/LEDDimmerDuoMini/LEDLookup".standardizePath, "w");
f.put("#ifndef LED_LOOKUP_H\n");
f.put("#define LED_LOOKUP_H\n");
f.put("uint16_t lookup[2048] = {\n");
~lookup.do{arg item, i;
	f.put("\t%".format(item));
	if(i != (~lookup.size - 1), {f.put(",")});
	if((i % 10) == 9, {f.put("\n")});
};
f.put("};\n");
f.put("#endif");
f.close;
)

