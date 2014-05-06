//Pinner
(
~pinne1 = VTPinneRobot("/dev/tty.usbserial-FT5QT6TQA");// pinne en
~pinne2 = VTPinneRobot("/dev/tty.usbserial-FT5QT6TQB");// pinne to
~pinne3 = VTPinneRobot("/dev/tty.usbserial-FT5QT6TZA");// pinne tre
~pinne4 = VTPinneRobot("/dev/tty.usbserial-FT5QT6TZB");// pinne fire
~pinne5 = VTPinneRobot("/dev/tty.usbserial-FT5QT6UDA");// pinne fem
~pinne6 = VTPinneRobot("/dev/tty.usbserial-FT5QT6UDB");// pinne seks
~pinne7 = VTPinneRobot("/dev/tty.usbserial-FT5QT6ULA");// pinne syv
~pinne8 = VTPinneRobot("/dev/tty.usbserial-FT5QT6ULB");// pinne aatte
);


~pinne = ~pinner[1];
~pinne.stop;
~pinne.leftMotor.speed_(120);
~pinne.leftMotor.speed_(0);
~pinne.leftMotor.direction_(\down);
~pinne.leftMotor.direction_(\up);
~pinne.rightMotor.speed_(100);
~pinne.rightMotor.stop(1000);
~pinne.rightMotor.speed_(0);
~pinne.rightMotor.direction_(\down);
~pinne.rightMotor.direction_(\up);
~pinne.rotationMotor.speed_(500);
~pinne.rotationMotor.speed_(0);
~pinne.rotationMotor.direction_(\left);
~pinne.rotationMotor.direction_(\right);
~pinne.leftMotor.currentPosition;
~pinne.rightMotor.currentPosition;
~pinne.rotationMotor.currentPosition;

~pinne.leftMotor.speed_(120);
~pinne.rightMotor.speed_(120);
~pinne.leftMotor.targetPosition_(200);
~pinne.rightMotor.targetPosition_(200);
~pinne.leftMotor.goToTargetPosition(10);
~pinne.rightMotor.goToTargetPosition(10);
~pinne.leftMotor.speedScaling_(200);
~pinne.rightMotor.speedScaling_(200);



~pinne.leftMotor.goToParkingPosition;
~pinne.rightMotor.goToParkingPosition;
~pinne.rotationMotor.goToParkingPosition;

f = fork{ loop { ~pinne.rotationMotor.currentPosition.postln; 0.5.wait; } };
f.stop;
~pinneRanges = (
	\1: (
		rotation: (minPosition: 200, maxPosition: 800),
		left: (minPosition: 0, maxPosition: 690),
		right: (minPosition: 0, maxPosition: 690)//traad maa forlenges
	),
	\2: (
		rotation: (minPosition: 200, maxPosition: 800),
		left: (minPosition: 0, maxPosition: 590),
		right: (minPosition: 0, maxPosition: 590)
	),
	\3: (
		rotation: (minPosition: 367, maxPosition: 576),
		left: (minPosition: 0, maxPosition: 580),
		right: (minPosition: 0, maxPosition: 580)
	),
	\4: (
		rotation: (minPosition: 200, maxPosition: 800),
		left: (minPosition: 0, maxPosition: 650),
		right: (minPosition: 0, maxPosition: 650)
	),
	\5: (
		rotation: (minPosition: 485, maxPosition: 550),
		left: (minPosition: 0, maxPosition: 600),
		right: (minPosition: 0, maxPosition: 600)//noe rart med rot pot feste her
	),
	\6: (
		rotation: (minPosition: 406, maxPosition: 722),
		left: (minPosition: 0, maxPosition: 600),
		right: (minPosition: 0, maxPosition: 600)//noe rart med rot pot feste her
	),
	\7: (
		rotation: (minPosition: 300, maxPosition: 700),//rot pot funker ikke
		left: (minPosition: 0, maxPosition: 600),
		right: (minPosition: 0, maxPosition: 600)
	),
	\8: (
		rotation: (minPosition: 300, maxPosition: 700),//rot pot funker ikke
		left: (minPosition: 0, maxPosition: 600),
		right: (minPosition: 0, maxPosition: 600)
	)
)



