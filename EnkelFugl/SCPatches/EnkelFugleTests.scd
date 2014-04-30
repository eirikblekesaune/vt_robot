~fuglAddr = NetAddr("1.2.3.112", 50004);
~fuglAddr.sendMsg('/robot/fugl/direction', "up");
~fuglAddr.sendMsg('/robot/fugl/direction', "down");
~fuglAddr.sendMsg('/robot/fugl/speed', 3)
~fuglAddr.sendMsg('/robot/fugl/speed', 0)

~fuglAddr.sendMsg('/robot/fugl/stepinterval', 10000)

~fuglAddr.sendMsg('/robot/fugl/setMin', 0)


(2r0001010).bitAnd(1)