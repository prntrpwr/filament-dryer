import serial
import time

interval_duration = 10

def serial_start():
    global s
    s = serial.Serial('COM6', 115200, dsrdtr=True)
    response = b""
    while not response.startswith(b"OK"):
        time.sleep(0.1)
        s.write(b"\n")
        response = s.readline().strip()
        print(response)

def serial_end():
    s.close()

def serial_cmd(cmd):
    #print(cmd)
    s.write( (cmd + "\n").encode())
    response = s.readline().strip()
    response = response.decode()
    if not response.startswith("OK"):
        raise RuntimeError("Command failed: " + response.decode("utf-8"))
    #print(response)
    return response

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False
    
def values_within(readingsa, readingsb, allowed_diff):
    if readingsa is None or readingsb is None: return False
    if len(readingsa) != len(readingsb): return False

    for i in range(9,len(readingsa)):
        if not is_number(readingsa[i]): continue
        a = float(readingsa[i])
        b = float(readingsb[i])
        diff = min(a, b) / max(a, b)
        if (diff < allowed_diff) :
            #print(a,b,diff)
            return False

    return True

def equilibrate(min_s=10, max_s=60, interval_s=10, min_drift=0.999):
    sensor_readings = None
    last_sensor_readings = None
    start = time.time()
    while ((not values_within(sensor_readings, last_sensor_readings, min_drift)) and ((time.time() - start) < max_s)) or ((time.time() - start) < min_s):
        last_sensor_readings = sensor_readings
        sensor_readings = serial_cmd(f"R {interval_s}").split(",")
        #print(sensor_readings)

def record_sensors():
    interval_start = round(time.time())
    sensor_readings = serial_cmd(f"R {interval_duration}")
    interval_end = round(time.time())
    print(f"{interval_start},{interval_end},{sensor_readings}")

def everything_off():
    serial_cmd("F 1 0")
    serial_cmd("F 2 0")
    serial_cmd("H 2 0")
    equilibrate()

def test_fan_step_pwm_pressure(step_size):
    serial_start()
    serial_cmd("D fansteppwm")
    everything_off()

    for fan_pwm in list(range(25,255, 1*step_size)) + list(range(255,25, -1*step_size)):
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
    serial_cmd("F 1 255")
    serial_cmd("F 2 255")
    equilibrate(min_s=600,max_s=900)
    record_sensors()

    # for each heater power from 255 to 0
    # TODO up and down
    for heater_pwm in range(15, -1, -1):
        # heater 2 on power, equilibrate
        serial_cmd(f"H 2 {heater_pwm}")
        equilibrate(min_s=600,max_s=900)
        record_sensors()

    serial_cmd("D Complete")
    everything_off()

    serial_end()

def test_hot_fan_step(step_size=25, heater_pwm=15, start_fan_pwm=125):
    serial_start()
    serial_cmd("D hotfanstep")
    everything_off()

    fan_pwm_list = list(range(start_fan_pwm,255, 1*step_size))
    midpoint_between_last_two = round((fan_pwm_list[-1] + fan_pwm_list[-2])/2)
    fan_pwm_list += list(range(midpoint_between_last_two,start_fan_pwm, -1*step_size))
    print(fan_pwm_list)

    serial_cmd("F 1 255")
    serial_cmd("F 2 255")
    equilibrate(min_s=600,max_s=900)
    record_sensors()

    for fan_pwm in fan_pwm_list:
        serial_cmd(f"F 1 {fan_pwm}")
        serial_cmd("F 2 255")
        serial_cmd(f"H 2 {heater_pwm}")
        equilibrate(min_s=600,max_s=900)
        record_sensors()

    for fan_pwm in fan_pwm_list:
        serial_cmd("F 1 255")
        serial_cmd(f"F 2 {fan_pwm}")
        serial_cmd(f"H 2 {heater_pwm}")
        equilibrate(min_s=600,max_s=900)
        record_sensors()

    serial_cmd("H 2 0")
    serial_cmd("F 1 255")
    serial_cmd("F 2 255")
    equilibrate(min_s=600,max_s=900)
    record_sensors()

    serial_cmd("D Complete")
    everything_off()

    serial_end()

test_hot_fan_step(step_size=50, heater_pwm=15, start_fan_pwm=100) # 2024-09-20