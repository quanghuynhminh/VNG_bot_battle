/*
 * ROBOCAR - VNG
 * VBLUno - Arduino
 * 2016
 */

//Userdefine datatype--------------------------------------------------------------
//#define DEBUG_SERIAL 
#define uint    unsigned int
#define uchar   unsigned char
#ifdef DEBUG_SERIAL
#define debug   Serial.println
#else
#define debug
#endif

//For Control 2 Motor---------------------------------------------------------------
#define MOTOR_LEFT_PWM          D11       //SHIELD L298/MOTOR B   P24
#define MOTOR_LEFT_DIRECTION    D13                            //P28
#define MOTOR_RIGHT_PWM         D10       //SHIELD L298/MOTOR A   P23
#define MOTOR_RIGHT_DIRECTION   D12                              //P25

#define PWM_RESOLUTION_BIT      8
#define PWM_VALUE_MAX           255
#define DIR_FORWARD             LOW       //0: TIEN
#define DIR_BACK                HIGH      //1: LUI
uchar motor_left_current_dir = DIR_FORWARD;
uchar motor_right_current_dir = DIR_FORWARD;
#define STOP          0
#define FORWARD       1
#define BACK          2
#define LEFT          3
#define RIGHT         4
uchar pr_state, nx_state=STOP;

//For Sensors ------------------------------------------------------------------
#define SENSOR_LINE_RIGHT       A1          //Cam bien do vach Phai - DIGITAL
#define SENSOR_LINE_LEFT        A2          //Cam bien do vach Trai - DIGITAL
#define SENSOR_LINE_BEHIND      A3          //Cam bien do vach Phia sau - DIGITAL
#define LINE_BLACK_VALUE       HIGH
#define LINE_WHITE_VALUE       LOW
#define LINE_BLACK             HIGH         //Co vach
#define OFF                    LOW          //Khong co vach

#define SENSOR_DISTANCE        A0           //ANALOG,  Do phan giai 10bit: 0-1023
#define ANALOG_VALUE_MAX       1023         //ADC 10 bit
#define VREF                   3300         //mV
#define CONSTANT_NOISE1        100          //So lan dem de chong nhieu cho CAm bien do line
#define CONSTANT_NOISE2        70           //Nguong chap nhan 
     
//Cac muc toc do: 0 - 65535 (16bit)------------------------------------------------------
//Chu y: Dong co quay kha nhanh
#define V0                     50        			//Muc toc do 0  
#define V1                     80
#define V2                     128
#define V3                     200
#define V4                     PWM_VALUE_MAX  		//255

//-----------------------------------------------------------------------------------------------
//-----------------------------------FUNCTIONS FOR MOTORS--------------------------------------//
//-----------------------------------------------------------------------------------------------
//Function: Dieu khien 2 motor------------------------------
void control_motor(uint left_pwm, uchar left_direction, uint right_pwm, uchar right_direction){
  //motor left
  digitalWrite(MOTOR_LEFT_DIRECTION, left_direction);
  analogWrite(MOTOR_LEFT_PWM, left_pwm);
  //motor right
  digitalWrite(MOTOR_RIGHT_DIRECTION, right_direction);
  analogWrite(MOTOR_RIGHT_PWM, right_pwm);

  motor_left_current_dir = left_direction;
  motor_right_current_dir = right_direction;
}

//Function: Dung xe-----------------------------------------
void stop_car(){
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  
  //digitalWrite(MOTOR_LEFT_DIRECTION, motor_left_current_dir^0x01);
  //digitalWrite(MOTOR_RIGHT_DIRECTION, motor_right_current_dir^0x01);
  pr_state = nx_state;
  nx_state = STOP;
}

//Function: Quay trai khong gioi han---------------------
void rotate_left(uint v){
  if(check_line_left()==OFF){
    control_motor(v, DIR_BACK, v, DIR_FORWARD);  
    pr_state = nx_state;
    nx_state = LEFT;
  }
}

//Function: Quay phai khong gioi han---------------------
void rotate_right(uint v){
  if(check_line_right()==OFF){
    control_motor(v, DIR_FORWARD, v, DIR_BACK);  
    pr_state = nx_state;
    nx_state = RIGHT;
  }
}

//Function: Chay thang----------------------------------
void go_forward(uint v){
  if(has_line_front()==OFF){
    control_motor(v, DIR_FORWARD, v-7, DIR_FORWARD);  
    pr_state = nx_state;
    nx_state = FORWARD;
  }
}

//Function: Chay lui-------------------------------------
void go_back(uint v){
  if(check_line_behind()==OFF){
    control_motor(v, DIR_BACK, v, DIR_BACK);  
    pr_state = nx_state;
    nx_state = BACK;
  }
}

//-----------------------------------------------------------------------------------------------
//----------------------------------FUNCTIONS FOR SENSORS--------------------------------------//
//-----------------------------------------------------------------------------------------------
//Function: Doc tin hieu line ben trai-----------------
uchar check_line_left(){
  uint val=0;
  for(int i=0; i<CONSTANT_NOISE1; i++){
    val = val + digitalRead(SENSOR_LINE_LEFT);
  }
  if(val > CONSTANT_NOISE2) return LINE_BLACK;
  else return OFF;
}

//Function: Doc tin hieu line ben phai----------------
uchar check_line_right(){
  uint val=0;
  for(int i=0; i<CONSTANT_NOISE1; i++){
    val = val + digitalRead(SENSOR_LINE_RIGHT);
  }
  if(val > CONSTANT_NOISE2) return LINE_BLACK;
  else return OFF;
}

//Function: Doc tin hieu led phia sau xe-----------------
uchar check_line_behind(){
  uint val=0;
  for(int i=0; i<CONSTANT_NOISE1; i++){
    val = val + digitalRead(SENSOR_LINE_BEHIND);
  }
  if(val > CONSTANT_NOISE2) return LINE_BLACK;
  else return OFF;
}

//Function: Kiem tra 1 trong 3 line-----------------------
uchar has_line(){
  if((check_line_left()==LINE_BLACK)||(check_line_right()==LINE_BLACK)||(check_line_behind()==LINE_BLACK)){
    return LINE_BLACK;
  }
  return OFF;
}

//Function: Kiem tra 2 line phia truoc-------------------
uchar has_line_front(){
  if((check_line_left()==LINE_BLACK)||(check_line_right()==LINE_BLACK)){
    return LINE_BLACK;
  }
  return OFF;
}

//Function: Ham chuyen doi tu gia tri adc sang cm-------------------
int value_to_cm(int adc){
  int cm;
  if(adc>800){
    cm=1;             //Qua gan
  }
  else if(adc>700){
    cm=4;             //Rat gan
  }
  else{
    adc=700-adc;
    if (adc > 600) cm = 0;
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
  }
  return cm;
}

//Function: Ham do khoang cach vat can phia truoc: cm--------------
int measure_distance(){
  int val[10], average=0;
  for(int i=0; i<5; i++){
    val[i] = analogRead(SENSOR_DISTANCE);
    average = average + val[i];
    delay(1);
  }
  average = average/5;

  #ifdef DEBUG_SERIAL
    debug("-------------------");
    debug(average, DEC);      //value_adc
    debug((int)((float)average*VREF/ANALOG_VALUE_MAX), DEC);         //mV
  #endif
  return value_to_cm(average);
}


//-----------------------------------------------------------------------------------------------
//-------------------------------------FUNCTIONS FOR TEST--------------------------------------//
//-----------------------------------------------------------------------------------------------
#ifdef DEBUG_SERIAL
void test_motor(){
  debug("LEFT: Forward - RIGHT: Back");
  control_motor(PWM_VALUE_MAX, DIR_FORWARD, PWM_VALUE_MAX, DIR_BACK);
  delay(3000);
  
  debug("LEFT: Stop - RIGHT: Back");
  control_motor(0, DIR_FORWARD, PWM_VALUE_MAX, DIR_BACK);
  delay(3000);
  
  debug("LEFT: Forward - RIGHT: Stop");
  control_motor(PWM_VALUE_MAX, DIR_FORWARD, 0, DIR_BACK);
  delay(3000);

  
  debug("LEFT: Back - RIGHT: Forward");
  control_motor(PWM_VALUE_MAX, DIR_BACK, PWM_VALUE_MAX, DIR_FORWARD);
  delay(3000);

  
  debug("LEFT: Forward - RIGHT: Forward");
  control_motor(PWM_VALUE_MAX, DIR_FORWARD, PWM_VALUE_MAX, DIR_FORWARD);
  delay(3000);

  
  debug("LEFT: Back - RIGHT: Back");
  control_motor(PWM_VALUE_MAX, DIR_BACK, PWM_VALUE_MAX, DIR_BACK);
  delay(3000);
  
}

void test_motor_pwm(){
  unsigned int i=0;
  debug("LEFT: Forward - RIGHT: Forward. Test pwm_value");
  for(i=0; i<256; i=i+5){
    debug(i, DEC);
    control_motor(i, DIR_FORWARD, i, DIR_FORWARD);
    delay(500);  
  }
  
}

void test_sensor_line_polling(){
  if(check_line_left()==LINE_BLACK){
    debug("Line_left is BLACK");
  }

  if(check_line_right()==LINE_BLACK){
    debug("Line_right is BLACK");
  }

  if(check_line_behind()==LINE_BLACK){
    debug("Line_behind is BLACK");
  }
}

void test_sensor_distance(){
  int val=0;
  debug("---Distance:");
  val=measure_distance();
  debug(measure_distance(),DEC);
  delay(1000);
}

void test_alg2(){
  int i=0;
  while((has_line_front()==OFF)&&(i<2000)){
    go_forward(V1);
    i++;
  }
  stop_car();
  delay(2000);
  i=0;
  while((check_line_behind()==OFF)&&(i<2000)){
    go_back(V1);
    i++;
  }
  stop_car();
  delay(2000);

  i=0;
  while((has_line_front()==OFF)&&(i<2000)){
    rotate_left(V1);
    i++;
  }
  stop_car();
  delay(2000);

  i=0;
  while((has_line_front()==OFF)&&(i<2000)){
    rotate_right(V1);
    i++;
  }
  stop_car();
  delay(2000);
}


void test(){
  //test_sensor_distance();
  //test_sensor_line_polling();
  test_alg2();
  
}
#endif

//-----------------------------------------------------------------------------------------------
//------------------------------------FUNCTIONS FOR FIGHT---------------------------------------//
//-----------------------------------------------------------------------------------------------
//Function: Kiem tra xem co doi thu phia truoc hay khong----------------------
//@return: 0 la Khong co, nguoc lai la co
uchar check_you(){
  int val = measure_distance();
  if(val==0)    return 0;             
  if(val<10)    return 1;
  if(val<20)    return 2;
  if(val<30)    return 3;
  return 0;
}

void delay_per10ms(int timer){
      for(int i =0; i < timer;i++){
        if((check_line_left()==LINE_BLACK)||(check_line_right()==LINE_BLACK)||(check_line_behind()==LINE_BLACK)){
          break;
        }
        else{
          delay(10);
        }
    }
}
//Function: Hanh dong khi gap vach--------------------------------------
//Giup xe chay quay lai
void keep_in_round(){
  if((check_line_left()==LINE_BLACK)&&(check_line_right()==LINE_BLACK)){
    go_back(V2);
    while((check_line_left()==LINE_BLACK)||(check_line_right()==LINE_BLACK));
    delay(100);
    stop_car();
    delay(200);
  }
  else if((check_line_left()==LINE_BLACK)){
    go_back(V2);
    while((check_line_left()==LINE_BLACK));
    delay(100);
    stop_car();
    delay(100);
  }
  else if((check_line_right()==LINE_BLACK)){
    go_back(V2);
    while((check_line_right()==LINE_BLACK));
    delay(100);
    stop_car();
    delay(100);
  }
  
  if(check_line_behind()==LINE_BLACK){
    go_forward(V4);
    while((check_line_behind()==LINE_BLACK));
    delay(100);
    stop_car();
    delay(100);
  } 
}

//Function: Thuat toan 1-------------------------------------------------
//Can bo sung them:
//      - Neu hai xe huc nhau mot luc thi dem thoi gian va cong suat
//      - neu co doi thu truoc mat, ma co tin hieu line thi van cu go_forward (hien tuong xe bi boc dau)
void alg1(){
  int count=0;
  int V;
  
  _start:
  while(check_you()==0){
    rotate_left(V0);
    keep_in_round();
  }
  stop_car();
  V=V1;
  while(has_line_front()==OFF){
    go_forward(V); 
    keep_in_round(); 
    //dem thoi gian va tang suc manh
    count=count+1;
    if(count>1000){
      V=V3;
    }
   //if((count%50==0)&&(check_you()==0)) goto _start;    chu y cho nay
  }
  stop_car();
  while(1){};

}

//-----------------------------------------------------------------------------------------------
//-----------------------------------BASIC FUNCTIONS FOR ARDUINO-------------------------------//
//-----------------------------------------------------------------------------------------------
void setup() {
  //init motor & pwm
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIRECTION, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_DIRECTION, OUTPUT);
  analogWriteResolution(PWM_RESOLUTION_BIT);
  stop_car();

  //init sensor
  pinMode(SENSOR_LINE_LEFT, INPUT_PULLUP);
  pinMode(SENSOR_LINE_RIGHT, INPUT_PULLUP);
  pinMode(SENSOR_LINE_BEHIND, INPUT_PULLUP);
  pinMode(SENSOR_DISTANCE, INPUT);        //analog
  
  //init test
  #ifdef DEBUG_SERIAL
    Serial.begin(9600); 
    Serial.println("Start test application");
  #endif
}
void loop() {
  //test-----------------------------------------
  #ifdef DEBUG_SERIAL
    while(1){
      test();
    };
  #endif

  //MAIN Function-------------------------------
 // delay(100);
  alg1();
}

