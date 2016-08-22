/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
//#include "ble/BLE.h"
//#include "ble/services/iBeacon.h"
//#include "ble/services/UARTService.h"

DigitalOut  led1(p7);
DigitalOut  DIR_L(p25);
DigitalOut  PWM_L(p23);
DigitalOut  DIR_R(p28);
DigitalOut  PWM_R(p24);

//// Sensor
#define NOISE 5
//#define DEBUG

#ifdef DEBUG
Serial      pc(p10, p11);
#endif
Ticker      ticker;
AnalogIn    analog_ir(p1);
DigitalIn   SEN_UP_R(p2);
DigitalIn   SEN_UP_L(p3);
DigitalIn   SEN_DOWN(p4);
DigitalIn   SEN_IR(p6);

#define ON      0
#define OFF     1
#define VREF    3300.0  //mV

//// Motor
#define STOP    0
#define UP      1
#define DOWN    2
#define LEFT    3
#define RIGHT   4
uint8_t     g_time_pwm = 0,     g_pwm_left = 0,     g_pwm_right = 0;
uint16_t    g_time_led = 0;
uint8_t     g_sts_car = STOP;



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/*
* @param    dir: UP or DOWN; speed: 0-100.
* @return   none.
*/
void motor_left(uint8_t dir, uint8_t speed)
{
    DIR_L = dir;
    g_pwm_left = speed;
}
//////////////////////////////////////////////////
/*
* @param    dir: UP or DOWN; speed: 0-100.
* @return   none.
*/
void motor_right(uint8_t dir, uint8_t speed)
{
    DIR_R = dir;
    g_pwm_right = speed;
}
//////////////////////////////////////////////////
/*
* @param    speed: 0-100.
* @return   none.
*/
void rotator_left(uint8_t speed)
{
    DIR_L = 0;
    g_pwm_left = speed;
    DIR_R = 1;
    g_pwm_right = speed;
    g_sts_car = LEFT;
}
//////////////////////////////////////////////////
/*
* @param    speed: 0-100.
* @return   none.
*/
void rotator_right(uint8_t speed)
{
    DIR_L = 1;
    g_pwm_left = speed;
    DIR_R = 0;
    g_pwm_right = speed;
    g_sts_car = RIGHT;
}
//////////////////////////////////////////////////
/*
* @param    speed_left, speed_right: 0-100.
* @return   none.
*/
void move_up(uint8_t speed_left, uint8_t speed_right)
{
    DIR_L = 0;
    g_pwm_left = speed_left;
    DIR_R = 0;
    g_pwm_right = speed_right;
    g_sts_car = UP;
}
//////////////////////////////////////////////////
/*
* @param    speed_left, speed_right: 0-100.
* @return   none.
*/
void move_down(uint8_t speed_left, uint8_t speed_right)
{
    DIR_L = 1;
    g_pwm_left = speed_left;
    DIR_R = 1;
    g_pwm_right = speed_right;
    g_sts_car = DOWN;
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
/*
* @param    none.
* @return   Distance (cm), 1-70 cm.
*/
uint16_t sensor_ir(void)
{
    uint16_t    adc;
    uint16_t    cm;
    uint16_t    sensor = 0, i;

    for(i=0; i<NOISE; i++) {
        adc = analog_ir.read_u16();
        adc = 750-adc;
        if      (adc > 60000) cm = 1;
        else if (adc > 600) cm = 0;
        else if (adc > 550) cm = adc/8;
        else if (adc > 500) cm = adc/10;
        else if (adc > 450) cm = adc/12;
        else if (adc > 400) cm = adc/14;
        else if (adc > 350) cm = adc/16;
        else if (adc > 300) cm = adc/18;
        else if (adc > 200) cm = adc/16;
        else if (adc > 200) cm = adc/14;
        else if (adc > 150) cm = adc/12;
        else if (adc > 100) cm = adc/10;
        else if (adc >  60) cm = adc/9;
        else if (adc >  30) cm = adc/8;
        else if (adc >   0) cm = adc/7;

        wait(0.001);
        sensor = sensor + cm;
        if(cm == 0) break;
        cm = sensor/NOISE;
    }

#ifdef DEBUG
    pc.printf("\r\n %d adc, %d cm", adc, cm);
#endif
    return cm;
}

//////////////////////////////////////////////////
/*
* @param    none.
* @return   ON or OFF.
*/
uint8_t sensor_up_left(void)
{
    uint16_t i, sensor = 0;

    for(i=0; i<NOISE; i++) {
        wait(0.001);
        sensor = sensor + SEN_UP_L;
    }
    if(sensor > NOISE/2)   return OFF;
    else                   return ON;
}
//////////////////////////////////////////////////
/*
* @param    none.
* @return   ON or OFF.
*/
uint8_t sensor_up_right(void)
{
    uint16_t i, sensor = 0;

    for(i=0; i<NOISE; i++) {
        wait(0.001);
        sensor = sensor + SEN_UP_R;
    }
    if(sensor > NOISE/2)   return OFF;
    else                   return ON;
}
//////////////////////////////////////////////////
/*
* @param    none.
* @return   ON or OFF.
*/
uint8_t sensor_down(void)
{
    uint16_t i, sensor = 0;

    for(i=0; i<NOISE; i++) {
        wait(0.001);
        sensor = sensor + SEN_DOWN;
    }
    if(sensor > NOISE/2)   return OFF;
    else                   return ON;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void periodicCallback(void)
{
    if(g_time_pwm < g_pwm_left)    PWM_L = 1; else PWM_L = 0;
    if(g_time_pwm < g_pwm_right)   PWM_R = 1; else PWM_R = 0;
    g_time_pwm++; if(g_time_pwm >= 100) {g_time_pwm = 0;}

    g_time_led++; if(g_time_led >= 1000) {g_time_led = 0; led1 = !led1;}   //DIR_L = !DIR_L; DIR_R = !DIR_R;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    wait(0.1);
//Init Hardware
    SEN_UP_L.mode(PullUp);
    SEN_UP_R.mode(PullUp);
    SEN_DOWN.mode(PullUp);
    SEN_IR.mode(PullUp);
    led1 = 1; DIR_L = 0; DIR_R = 0; PWM_L = 0;  PWM_R = 0;
#ifdef DEBUG
    pc.baud(9600);
    pc.printf("\n\r# Sumo Car\n\r");
#endif
//Init interupt Timer
    ticker.attach(periodicCallback, 0.00015);
    wait(0.5);

#ifdef DEBUG    //sensor
    while (true) {
        rotator_right(20);
        wait(0.5);
        sensor_ir();
    }
#endif


    while (true) {
        rotator_right(25);
        uint8_t sen_in = sensor_ir();
        if( (sen_in > 0) && (sen_in < 30) )
        {
            move_up(50, 50);
            while( (sensor_up_left() == ON) && (sensor_up_right() == ON) );
            move_up(0, 0);
            while(1);
        }
    }
}

