VTPenseBox {
    var serialPort;
    var <penses;

    *new{arg port;
        ^super.new.init(port);
    }

    init{arg port_;
        serialPort = SerialPort(port_, 2400, crtscts: true);
        penses = 12.collect({arg i;
            VTPens(this, i + 1);
        });
    }

    setPens{arg number, value;
        var message;
        value = value.booleanValue.asInteger;
        message = "S" ++ number.asStringToBase(width: 2) ++ value;
        message = message.ascii ++ [13, 10];
        this.sendMsg(message);
    }

    sendMsg{arg msg;
        "to pens: %\n".postf(msg);
        serialPort.putAll(msg);
    }
}

VTPens {
    var box;
    var <value;
    var <number;

    *new{arg box, number;
        ^super.new.init(box, number);
    }

    init{arg box_, number_;
        box = box_;
        number = number_;
        value = 0;
    }

    refresh{
        box.setPens(number, value);
    }

    update{arg theChanged, theChanger, what;
        "VTPens update: %\n".postf([theChanged, theChanger, what]);
        if(theChanger !== this, {
            var newVal;
            if(what == \value, {
                newVal = theChanged.value.booleanValue.asInteger;
                value = newVal;
                this.refresh;
            });
        });
    }
}
