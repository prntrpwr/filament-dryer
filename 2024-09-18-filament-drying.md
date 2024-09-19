# RANDOM THOUGHTS
- Use old ATMega2560 with RAMPS controller.  Gives 5V and heater/fan control
	- But don't use Marlin firmware or GCode; custom firmware only
- Build an I2C hub for all sensors, use connectors so sensors can be moved/replaced/etc
- better to get a single, quality, US made differential pressure sensor, or multiple cheap chinese ones?
# EXPERIMENTS
* Dehydration (fan_power, heater_power, time)
	* Execution
		* Start
		* Fan 1 Off, Fan 2 Off, Heater 1 Off, Heater 2  Off, equilibrate
		* Fan 2 On (fan_power), equilibrate
		* Heater 2 On (heater_power), equilibrate
		* Wait (time)
		* Heater 2 Off, equilibrate
		* Fan 2 Off, equilibrate
		* End
	* Analysis
		* TODO
* Hydration (fan_power, heater_power, time)
	* Execution
		* Start
		* Fan 1 Off, Fan 2 Off, Heater 1 Off, Heater 2  Off, equilibrate
		* Fan 1 On (fan_power), equilibrate
		* Heater 1 On (heater_power), equilibrate
		* Wait (time)
		* Heater 2 Off, equilibrate
		* Fan 2 Off, equilibrate
		* End
	* Analysis
		* TODO
* Fan Calibration
	* Execution
		* Start
		* Fan 1 Off, Fan 2 Off, Heater 1 Off, Heater 2  Off, equilibrate
		* For each Fan 1,2
			* For each Fan power 255 to 1
				* Fan X on power Y, equilibrate
				* Record mass flows
			* Fan X Off, equilibrate
		* End
	* Analysis
		* Table and chart of recorded values
			* X axis = fan PWM duty cycle
			* Y axis = recorded mass flow
		* Equation of best fit curve and noted R
* Temperature Calibration (manual)
	* Execution
		* Manually remove water container from Waterbath Chamber
		* Start
		* Fan 1 Off, Fan 2 Off, Heater 1 Off, Heater 2  Off, equilibrate
		* Fan 1 On full, Fan 2 On full
		* Equilibrate all temperature sensors except Room
	* Analysis
		* Calculate average reading across all temperature sensors, except Room
		* Table of each sensor and the difference between its average reading and the global average
* Mass Calibration (manual)
	* Execution
		* Manually put calibration weight on sensor
		* Start
		* Equilibrate mass sensor
		* End
	* Analysis
		* Report average mass reading
# COMMUNICATION PROTOCOL
Case insensitive
* Fan X Duty Cycle Y
	* F X Y\n
		* e.g. f1 255\n
		* e.g. F2 57\n
	* Response: OK\n
* Heater X Duty Cycle Y
	* H X Y\n
		* e.g. H1 127\n
		* e.g. h2 255\n
	* Response: OK\n
* Read Sensors, Interval X milliseconds
	* R X\n
		* e.g. R 1000\n
		* e.g. r 10000\n
	* Response (depends on number of sensors configured):
		* Actual milliseconds in interval
		* Actual sensor samples in interval
		* For each sensor
			* For each physical property measured
				* min, max, avg, std dev
		* Newline
* Sensor Identification
	* S\n
	* Response (depends on number of sensors configured):
		* Type
		* Name of physical property measured
		* Unit used for measurement
* Error
	* Response:
		* E MESSAGE
		* e.g. E Timeout
* Display Information
	* D XXXX\n
	* Response: OK\n
# PARTS
* I2C Breakout Board
	* 2 X TCA9548A boards
	* 1 X proto board
	* 16 X JST-XH2.54 4 Pin sockets
	* Misc hookup wire est. 5 ft
* I2C cables
	* 16  X JST-XH2.54 4 Pin connectors and pins
	* 16 X 2ft X 4 core shielded wire
	* 16  X JST-XH2.54 4 Pin connectors and pins
* I2C Sensors
	* 16 X JST-XH2.54 4 Pin sockets
* 1-Wire Breakout Board
	* 16 x JST-XH2.54 3 Pin sockets
	* 1 X proto board
	*  Misc hookup wire est 2 ft.
* 1-Wire Sensors
	* 16 x JST-XH2.54 3 Pin connectors and pins
	* 16 X [DS18B20 Datasheet and Product Info | Analog Devices](https://www.analog.com/en/products/ds18b20.html#documentation)
*  Differential pressure sensor
	*  [ABP2DRRN002ND2B3XX Honeywell | Mouser](https://www.mouser.com/ProductDetail/Honeywell/ABP2DRRN002ND2B3XX?qs=pUKx8fyJudCsCuOP9rBvpA%3D%3D)
	* [MDM291选型 (cfsensor.com)](https://cfsensor.com/wp-content/uploads/2022/11/XGZP6897D-Pressure-Sensor-V2.7.pdf)