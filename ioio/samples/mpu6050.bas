' MPU6050 - Accelerometer, gyroscope and temperature sensor
' =========================================================
'
' This examample demonstrates how to use
' the MPU-6050 I2C sensor.
' 
' Connect the sensor to the IOIO-OTG board:
'
' ------         ------
'  IOIO |       |MPU6050
'  PIN 4|-------|SDA 
'  PIN 5|-------|SCL
'  GND  |-------|GND
'  3.3V |-------|VIN
'       |       |XCL
'       |       |XDA
'       |       |ADD
'       |       |INT
'-------         ------

' If ADD is open or connected to GND, 0x68 as I2C address 
' will be used. Otherwise 0x69. 
' Don't connect XCL, XDA, ADD and INT
' 
' "https://github.com/tockn/MPU6050_tockn" was very helpful
' to get the sensor working

import ioio

const ADDRESS = 0x68

Print "Connect to MPU-6050"
sensor = ioio.openTwiMaster(0, 0)
ioio.waitForConnect(10)
Print "Connection established"

delay(500)

WhoamI = sensor.readwrite(ADDRESS, 1, 0x75)
print "WHO_AM_I: ", hex(WhoamI)     ' Check for connection: sensor returns 0x68

' SMPLRT_DIV
sensor.write(ADDRESS, 0x19, 0x00)
' MPU config
sensor.write(ADDRESS, 0x1A, 0x00)
' Gyro config
sensor.write(ADDRESS, 0x1B, 0x08)
' Accel config
sensor.write(ADDRESS, 0x1C, 0x00)
' Turn on
sensor.write(ADDRESS, 0x6B, 0x01)

for ii = 1 to 1000
    
    A = GetAcceleration()
    G = GetGyroscope()
    T = GetTemperature()

    locate 5,0
    print "Acc: [";
    print USING "##.00 "; A.AccX, A.AccY, A.AccZ;
    print "]  Gryo: [";
    print USING "####.00 "; G.GyrX, G.GyrY, G.GyrZ;
    print "] Temp : ";
    print USING "##.00 "; T

    delay(100)
    showpage
next
    
    
func GetAcceleration()
    local A
    dim A
    
    A.AccX = short((sensor.readwrite(ADDRESS, 1, 0x3B) lshift 8) BOR sensor.readwrite(ADDRESS, 1, 0x3C)) / 16384    
    A.AccY = short((sensor.readwrite(ADDRESS, 1, 0x3D) lshift 8) BOR sensor.readwrite(ADDRESS, 1, 0x3E)) / 16384
    A.AccZ = short((sensor.readwrite(ADDRESS, 1, 0x3F) lshift 8) BOR sensor.readwrite(ADDRESS, 1, 0x40)) / 16384
    
    return A
end

func GetGyroscope()
    local d
    dim d
    
    d.GyrX = short((sensor.readwrite(ADDRESS, 1, 0x43) lshift 8) BOR sensor.readwrite(ADDRESS, 1, 0x44)) / 65.5
    d.GyrY = short((sensor.readwrite(ADDRESS, 1, 0x45) lshift 8) BOR sensor.readwrite(ADDRESS, 1, 0x46)) / 65.5
    d.GyrZ = short((sensor.readwrite(ADDRESS, 1, 0x47) lshift 8) BOR sensor.readwrite(ADDRESS, 1, 0x48)) / 65.5
    
    return d
end

func GetTemperature()
    return short((sensor.readwrite(ADDRESS, 1, 0x41) lshift 8) BOR sensor.readwrite(ADDRESS, 1, 0x42)) / 340 + 36.53
end

func CalculateAccelerationAngle(AccX, AccY, AccZ)
    local d
    dim d
    
    d.AngleAccX = atan2(AccY, sqr(AccZ^2 + AccX^2)) * 360 / 2.0 / PI
    d.AngleAccY = atan2(AccX, sqr(AccZ^2 * AccY^2)) * 360 / -2.0 / PI
    
    return d
end

func short(dat)
    if dat > 32767 then
        return dat - 65536
    else
        return dat
    endif
end
