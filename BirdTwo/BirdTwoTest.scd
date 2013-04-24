s.quit
OSCFunc.trace
NetAddr("1.2.3.117", 50004).sendMsg('/hei')
NetAddr("1.2.3.117", 50004).sendMsg('/stretch/direction', 'in')
NetAddr("1.2.3.117", 50004).sendMsg('/stretch/direction', "out")
NetAddr("1.2.3.117", 50004).sendMsg('/stretch/speed', 0)
NetAddr("1.2.3.117", 50004).sendMsg('/stretch/speed', 1)//101 > 5
NetAddr("1.2.3.117", 50004).sendMsg('/stretch/speed', 2)//100 > 1
NetAddr("1.2.3.117", 50004).sendMsg('/stretch/speed', 3)//011 > 6
NetAddr("1.2.3.117", 50004).sendMsg('/stretch/speed', 4)//010 > 2
NetAddr("1.2.3.117", 50004).sendMsg('/stretch/speed', 5)//001 > 4
NetAddr("1.2.3.117", 50004).sendMsg('/stretch/speed', 6)//000 > 0


NetAddr("1.2.3.117", 50004).sendMsg('/stretch:range/bounds', 300, 800)
NetAddr("1.2.3.117", 50004).sendMsg('/stretch:stepper/interval', 1600)