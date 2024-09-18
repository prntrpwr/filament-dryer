# RANDOM THOUGHTS
- Use old ATMega2560 with RAMPS controller.  Gives 5V and heater/fan control
	- But don't use Marlin firmware or GCode; custom firmware only
- Build an I2C hub for all sensors, use connectors so sensors can be moved/replaced/etc
- better to get a single, quality, US made differential pressure sensor, or multiple cheap chinese ones?
- Keep things simple by not using active (e.g. PID) control of fans/heaters.
# EXPERIMENTS
* Dehydration
* Hydration
* Fan calibration (manual)
* Mass flow calibration
* Temperature calibration
* Mass calibration (manual)
# PARTS
* 2C Breakout Board
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