#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include "./wiring.h"
#include <wiringPiI2C.h>
#include "./bme280.h"

int pins[] = {22, 23, 24, 25};

int main(int argc, char *argv[])
{
    setupWiringPi();
    
    int fd = wiringPiI2CSetup(BME280_ADDRESS);
    if(fd < 0) {
        printf("Device not found");
        return -1;
    }

    int coldPin = 22;
    int medPin = 23;
    int warmPin = 24;
    int hotPin = 25;

    setPinMode(coldPin, OUTPUT);
    setPinMode(medPin, OUTPUT);
    setPinMode(warmPin, OUTPUT);
    setPinMode(hotPin, OUTPUT);

    for(int i = 0; i < 20; i++)
    {
        bme280_calib_data cal;
        readCalibrationData(fd, &cal);
        wiringPiI2CWriteReg8(fd, 0xf4, 0x25);   // pressure and temperature oversampling x 1, mode normal

        bme280_raw_data raw;
        getRawData(fd, &raw);

        int32_t t_fine = getTemperatureCalibration(&cal, raw.temperature);
        float c = compensateTemperature(t_fine);
        float f = c * 9 / 5 + 32;

        printf("{\"sensor\":\"bme280\", \"C\":%.2f, \"F\":%.2f, \"timestamp\":%d}\n", c, f, (int)time(NULL));

        // turn all LEDs off            
        writeToPin(coldPin, LOW);
        writeToPin(medPin, LOW);
        writeToPin(warmPin, LOW);
        writeToPin(hotPin, LOW);
        
        if (c > 15)
        {
            writeToPin(coldPin, HIGH);
        }
        if (c > 27)
        {
            writeToPin(medPin, HIGH);
        }
        if (c > 28)
        {
            writeToPin(warmPin, HIGH);
        }
        if (c > 29)
        {
            writeToPin(hotPin, HIGH);
        }

        delay(500);
    }
    
    writeToPin(coldPin, LOW);
    writeToPin(medPin, LOW);
    writeToPin(warmPin, LOW);
    writeToPin(hotPin, LOW);

    return 0;
}

int32_t getTemperatureCalibration(bme280_calib_data *cal, int32_t adc_T) 
{
  int32_t var1  = ((((adc_T>>3) - ((int32_t)cal->dig_T1 <<1))) *
     ((int32_t)cal->dig_T2)) >> 11;

  int32_t var2  = (((((adc_T>>4) - ((int32_t)cal->dig_T1)) *
       ((adc_T>>4) - ((int32_t)cal->dig_T1))) >> 12) *
     ((int32_t)cal->dig_T3)) >> 14;

  return var1 + var2;
}

void readCalibrationData(int fd, bme280_calib_data *data) 
{
    data->dig_T1 = (uint16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_T1);
    data->dig_T2 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_T2);
    data->dig_T3 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_T3);

    data->dig_P1 = (uint16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P1);
    data->dig_P2 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P2);
    data->dig_P3 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P3);
    data->dig_P4 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P4);
    data->dig_P5 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P5);
    data->dig_P6 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P6);
    data->dig_P7 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P7);
    data->dig_P8 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P8);
    data->dig_P9 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P9);

    data->dig_H1 = (uint8_t)wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H1);
    data->dig_H2 = (int16_t)wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_H2);
    data->dig_H3 = (uint8_t)wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H3);
    data->dig_H4 = (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H4) << 4) | (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H4+1) & 0xF);
    data->dig_H5 = (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H5+1) << 4) | (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H5) >> 4);
    data->dig_H6 = (int8_t)wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H6);
}

float compensateTemperature(int32_t t_fine) 
{
    float t = (t_fine * 5 + 128) >> 8;
    return t/100;
}

void getRawData(int fd, bme280_raw_data *raw) {
    wiringPiI2CWrite(fd, 0xf7);

    raw->pmsb = wiringPiI2CRead(fd);
    raw->plsb = wiringPiI2CRead(fd);
    raw->pxsb = wiringPiI2CRead(fd);

    raw->tmsb = wiringPiI2CRead(fd);
    raw->tlsb = wiringPiI2CRead(fd);
    raw->txsb = wiringPiI2CRead(fd);

    raw->hmsb = wiringPiI2CRead(fd);
    raw->hlsb = wiringPiI2CRead(fd);

    raw->temperature = 0;
    raw->temperature = (raw->temperature | raw->tmsb) << 8;
    raw->temperature = (raw->temperature | raw->tlsb) << 8;
    raw->temperature = (raw->temperature | raw->txsb) >> 4;

    raw->pressure = 0;
    raw->pressure = (raw->pressure | raw->pmsb) << 8;
    raw->pressure = (raw->pressure | raw->plsb) << 8;
    raw->pressure = (raw->pressure | raw->pxsb) >> 4;

    raw->humidity = 0;
    raw->humidity = (raw->humidity | raw->hmsb) << 8;
    raw->humidity = (raw->humidity | raw->hlsb);
}