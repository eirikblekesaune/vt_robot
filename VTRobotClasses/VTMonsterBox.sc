VTMonsterBox {
	var serialPort;
	var <channels;

	*new{arg port;
		^super.new.init(port);
	}

	init{arg port_;
		serialPort = SerialPort(port_, 2400, crtscts: true);
		channels = 8.collect({arg i;
			VTMonsterBoxChannel(this, i + 1);
		});
	}

	sendMsg{arg msg;
		serialPort.putAll(msg);
	}
}


VTMonsterBoxChannel {
	var monsterBox;
	var <spec;
	var <value;
	var <number;
	var lastMotorValue;
	var refreshTask;
	classvar <refreshRate = 10;

	*new{arg monsterBox, number;
		^super.new.init(monsterBox, number);
	}

	init{arg monsterBox_, number_;
		monsterBox = monsterBox_;
		number = number_;
		spec = ControlSpec(-512, 512, step: 1, default: 0);
		refreshTask = Task.new({
			var waitTime = this.class.refreshRate.reciprocal;
			loop{
				var message, newMotorValue;
				//motor driver takes values from 0 to 250
				newMotorValue = value.linlin(-512, 512, 0, 250).asInteger;
				if(newMotorValue != lastMotorValue, {
					message = ("M".ascii ++
						number.asString.ascii ++
						newMotorValue.asStringToBase(width: 3).ascii ++
						$\r.ascii);
					message.postln;
					monsterBox.sendMsg(message);
				});
				lastMotorValue = newMotorValue;
				waitTime.wait;
			}
		});
	}

	valueAction_{arg val;
		this.value_(val);
		this.refresh;
	}

	value_{arg val;
		value = val;
	}

	startRefreshing{
		refreshTask.start;
		SystemClock.sched(0.5, {
			this.stopRefreshing;
		});
	}

	stopRefreshing{
		refreshTask.stop;
	}

	refresh{
		if(refreshTask.isPlaying.not, {
			this.startRefreshing;
		});
	}

	stop{
		this.value_(spec.default);
		this.refresh;
	}

	minSpeed{ ^spec.minval; }
	maxSpeed{ ^spec.maxval; }

	update{arg theChanged, theChanger, what;
		"VTMotorBoxChannel update: %\n".postf([theChanged, theChanger, what]);
		if(theChanger !== this, {
			var newVal;
			if(what == \value, {
				newVal = theChanged.spec.unmap(theChanged.value);
				newVal = spec.map(newVal).asInteger;
				value = newVal;
				this.refresh;
			});
		});
	}
}