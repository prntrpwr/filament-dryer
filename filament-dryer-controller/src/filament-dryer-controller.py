import serial
import time

def serial_start():
    global s
    s = serial.Serial('COM6', 115200, dsrdtr=True)
    response = ""
    while response != b"OK":
        time.sleep(0.1)
        s.write(b"\n")
        response = s.readline().strip()
        #print(response)

def serial_end():
    s.close()

def serial_cmd(cmd):
    s.write( (cmd + "\n").encode())
    response = s.readline().strip()
    response = response.decode()
    if not response.startswith("OK"):
        raise RuntimeError("Command failed: " + response.decode("utf-8"))
    #print(response)
    return response

def equilibrate():
    # TODO
    time.sleep(1)

def record_sensors():
    interval_duration = 10000
    interval_start = round(time.time())
    sensor_readings = serial_cmd(f"R {interval_duration}")
    interval_end = round(time.time())
    print(f"{interval_start},{interval_end},{sensor_readings}")

def everything_off():
    serial_cmd("F 1 0")
    serial_cmd("F 2 0")
    serial_cmd("H 2 0")
    equilibrate()

def test_fan_step():
    serial_start()
    serial_cmd("D fan_step")
    everything_off()

    for fan_pwm in range(255,-1, -1):
        serial_cmd(f"F 1 {fan_pwm}")
        serial_cmd(f"F 2 {fan_pwm}")
        equilibrate()
        record_sensors()

    serial_cmd("D Complete")
    everything_off()

    serial_end()

def test_hot_step():
    serial_start()
    serial_cmd("D hot_step")

    everything_off()

    # fan 1 on 111, fan 2 on 132, equilibrate
    serial_cmd("F 1 111")
    serial_cmd("F 2 132")
    equilibrate()

    # for each heater power from 255 to 0
    for heater_pwm in range(255, -1, -5):
        # heater 2 on power, equilibrate
        serial_cmd(f"H 2 {heater_pwm}")
        equilibrate()

        # record
        record_sensors()

    serial_cmd("D Complete")
    everything_off()

    serial_end()

test_fan_step()