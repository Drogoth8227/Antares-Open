# Antares
DWM1000 based RTLS (replacing plan.txt)

pros:
	
	works indoors
	line of sight denied
	relatively low cost
	more trackers can be used at cost of decreasing performance
	
cons:

	not as precise as laser tracking
	requires calibration when setup
	more battery usage than other systems
	requires base stations
	

REMEMBER TO CALIBRATE!	

Ranging method: 
	
	TDOA
	6.8mbps
	16 prf
	36 byte payload (12bits for id, acc, gyro respectively)
	preamble 128
	(example b on dw1000 data sheet)
	expected tx: 69ma avg
	expected rx: 122ma avg
	
radio bandwidth:

	1754 (or smth) pings/second for system max
	split 54 for anchor clock sync,
	1700 for trackers, (17 tags/100hz or dynamic adjust possibly)

4 non planar anchors

	single module per anchor
	Main anchor has usb connection to pc
	work out distance from each other (work that out at some point)


3 dwm1000's per tag?

	2 tag types, 
	1 for high accuracy with 3 modules
	1 for low accuracy with 1 module

use some dwm1000 library make life ez

Components:

	mcu - STM32f411
	IMU - LSM6DSRTR
	Battery charge ic - BQ24072
	Batteries - 18650 or smth idfk

Processing:

	kalman filters go brrr (someone work this shit out)
	imu refresh at 1000hz
	ranging refresh at some speed (depending on how fast imu drift/kalman sample size)


data to pc: anchors receive imu data via dwm1000 transmission possibly

Future Board Plans:
	Break up DWM1000 into inderviual DW1000 components
