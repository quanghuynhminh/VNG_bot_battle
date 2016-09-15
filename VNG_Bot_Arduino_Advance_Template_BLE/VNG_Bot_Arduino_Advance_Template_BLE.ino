/*
 * VNG Bot Battle
 * Code mẫu cho Arduino IDE - Phiên bản nâng cao (Có sử dụng BLE)
 * Main board:  VBLUno kit
 * IDE:         Arduino IDE
 * (c)2016 - VNG IoT Lab
 * 
 * Cần phần chính:
 *    1. Các defines và biến toàn cục
 *    2. Các hàm phục vụ điều khiển động cơ 
 *    3. Các hàm phục vụ đo cảm biến
 *    4. Các hàm để test các mô đun cơ bản
 *    5. Các hàm để giao tiếp BLE với Smartphone, tablet
 *    6. Các hàm dành cho chiến đấu
 *    7. Các hàm cơ bản của Arduino IDE (setup, loop)
 *    
 *     * @note    Hiện tại đang dùng ticker1s để gửi giá trị các cảm biến lên smartphone
 *          Cần sửa lại theo ý tưởng: Khi nào có cảm biến vạch hoặc có đối thủ thì gửi lên
 *          xem hàm setup_ble và task_handle
 */

#include <BLE_API.h>

/***********************************************************************************
 ****************************DEFINES & BIẾN TOÀN CỤC********************************
 ***********************************************************************************/
 
#define DEBUG_SERIAL                          //Sử dụng khi cần debug qua Serial
#ifdef DEBUG_SERIAL
  #define debug   Serial.println
#else
  #define debug
#endif

#define uint    unsigned int
#define uchar   unsigned char

/*
 *  @brief  Các kết nối phần cứng và tham số cơ bản cho điều khiển 2 động cơ bánh
 */
 
#define MOTOR_LEFT_PWM          D11       //SHIELD L298/MOTOR B   P24
#define MOTOR_LEFT_DIRECTION    D13                             //P28
#define MOTOR_RIGHT_PWM         D10        //SHIELD L298/MOTOR A   P23
#define MOTOR_RIGHT_DIRECTION   D12                             //P25


#define PWM_RESOLUTION_BIT      8
#define PWM_VALUE_MAX           255
#define DIR_FORWARD             LOW       //0: TIEN
#define DIR_BACK                HIGH      //1: LUI

/*
 *  @brief  Các kết nối phần cứng và tham số cơ bản cho các sensors
 */
#define SENSOR_LINE_RIGHT       A2          //Cam bien do vach Phai - DIGITAL
#define SENSOR_LINE_LEFT        A3          //Cam bien do vach Trai - DIGITAL
#define SENSOR_LINE_BEHIND      A4          //Cam bien do vach Phia sau - DIGITAL
#define LINE_BLACK_VALUE       HIGH
#define LINE_WHITE_VALUE       LOW
#define LINE_BLACK             HIGH         //Co vach
#define OFF                    LOW          //Khong co vach
#define CONSTANT_NOISE1        5 //100          //So lan dem de chong nhieu cho Cam bien do line
#define CONSTANT_NOISE2        3 //70           //Nguong chap nhan 

#define SENSOR_DISTANCE        A5           //ANALOG,  Do phan giai 10bit: 0-1023
#define ANALOG_VALUE_MAX       1023         //ADC 10 bit
#define VREF                   3300         //mV

/*
 *  @brief  Các trạng thái di chuyển của Bot
 */
#define STOP                  0
#define FORWARD               1
#define BACK                  2
#define LEFT                  3
#define RIGHT                 4

/*
 *  @brief  Một số mức tốc độ được định nghĩa sẵn
 *  
 *  @note   V=0: Động cơ không quay, V=255: động cơ quay nhanh nhất
 */
#define V0                     50        			    
#define V1                     80
#define V2                     128
#define V3                     200
#define V4                     PWM_VALUE_MAX  		//255

/*
 *  @brief  Một vài thông số cho giao tiếp BLE
 */
#define DEVICE_NAME       "VNG_Bot1"
#define TXRX_BUF_LEN      2
// Create ble instance
BLE                       ble;
// Create a timer task
Ticker                    ticker1s;

// The constant uuid of service and characteristics
//Motors: 1 services gồm 5 characteristics
static const uint8_t motor_service_uuid[]                 = {0x00, 0x00, 0xFF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};
static const uint8_t motor_service_stop_char_uuid[]       = {0x00, 0x00, 0xFF, 0xF1, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};  //WRITE-uchar
static const uint8_t motor_service_forward_char_uuid[]    = {0x00, 0x00, 0xFF, 0xF2, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};  //WRITE-uchar
static const uint8_t motor_service_back_char_uuid[]       = {0x00, 0x00, 0xFF, 0xF3, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};  //WRITE-uchar
static const uint8_t motor_service_left_char_uuid[]       = {0x00, 0x00, 0xFF, 0xF4, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};  //WRITE-uchar
static const uint8_t motor_service_right_char_uuid[]      = {0x00, 0x00, 0xFF, 0xF5, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};  //WRITE-uchar
//Sensors: 1 services gồm 4 characteristics
static const uint8_t sensor_service_uuid[]                = {0x00, 0x00, 0xFF, 0xF6, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};
static const uint8_t sensor_service_distance_char_uuid[]  = {0x00, 0x00, 0xFF, 0xF7, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};   //NOTIFY-uchar
static const uint8_t sensor_service_left_char_uuid[]      = {0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};   //NOTIFY-uchar
static const uint8_t sensor_service_right_char_uuid[]     = {0x00, 0x00, 0xFF, 0xF9, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};   //NOTIFY-uchar
static const uint8_t sensor_service_behind_char_uuid[]    = {0x00, 0x00, 0xFF, 0xFA, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};   //NOTIFY-uchar

// Used in advertisement
static const uint8_t  vng_bot_uuid[]            = {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xF0, 0xFF, 0x00, 0x00};

// Initialize value of chars
uint8_t motor_stop_char_value[TXRX_BUF_LEN]       = {0};
uint8_t motor_forward_char_value[TXRX_BUF_LEN]    = {0};
uint8_t motor_back_char_value[TXRX_BUF_LEN]       = {0};
uint8_t motor_left_char_value[TXRX_BUF_LEN]       = {0};
uint8_t motor_right_char_value[TXRX_BUF_LEN]      = {0};

uint8_t sensor_distance_char_value[TXRX_BUF_LEN]  = {0};
uint8_t sensor_left_char_value[TXRX_BUF_LEN]      = {0};
uint8_t sensor_right_char_value[TXRX_BUF_LEN]     = {0};
uint8_t sensor_behind_char_value[TXRX_BUF_LEN]    = {0};
                        
// Create characteristic
GattCharacteristic  motor_stop_characteristic(motor_service_stop_char_uuid, motor_stop_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );
GattCharacteristic  motor_forward_characteristic(motor_service_forward_char_uuid, motor_forward_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );
GattCharacteristic  motor_back_characteristic(motor_service_back_char_uuid, motor_back_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );
GattCharacteristic  motor_left_characteristic(motor_service_left_char_uuid, motor_left_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );
GattCharacteristic  motor_right_characteristic(motor_service_right_char_uuid, motor_right_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );
GattCharacteristic *motorChars[] = {&motor_stop_characteristic, &motor_forward_characteristic, &motor_back_characteristic, &motor_left_characteristic, &motor_right_characteristic};

GattCharacteristic  sensor_distance_characteristic(sensor_service_distance_char_uuid, sensor_distance_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic  sensor_left_characteristic(sensor_service_left_char_uuid, sensor_left_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic  sensor_right_characteristic(sensor_service_right_char_uuid, sensor_right_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic  sensor_behind_characteristic(sensor_service_behind_char_uuid, sensor_behind_char_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic *sensorChars[] = {&sensor_distance_characteristic, &sensor_left_characteristic, &sensor_right_characteristic, &sensor_behind_characteristic};

//Create service
GattService         motorService(motor_service_uuid, motorChars, sizeof(motorChars) / sizeof(GattCharacteristic *));
GattService         sensorService(sensor_service_uuid, sensorChars, sizeof(sensorChars) / sizeof(GattCharacteristic *));
  
/***********************************************************************************
 ****************************CÁC HÀM ĐIỀU KHIỂN ĐỘNG CƠ*****************************
 ***********************************************************************************/
 
/*
 *  @brief  Điều khiển 2 động cơ bánh
 *  
 *  @param[in]  left_pwm, right_pwm
 *              Tốc độ chạy của động cơ trái và phải
 *              0-255 (0 là dừng)
 *             
 *  @param[in]  left_direction, right_direction
 *              Chiều quay động cơ
 *              DIR_FORWARD hoặc DIR_BACK
 */
void control_motor(uchar left_pwm, uchar left_direction, uchar right_pwm, uchar right_direction){
  //motor left
  digitalWrite(MOTOR_LEFT_DIRECTION, left_direction);
  analogWrite(MOTOR_LEFT_PWM, left_pwm);
  //motor right
  digitalWrite(MOTOR_RIGHT_DIRECTION, right_direction);
  analogWrite(MOTOR_RIGHT_PWM, right_pwm);
}

/*
 *  @brief  Dừng xe
 */
void stop_car(){
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
}

/*
 *  @brief  Cho xe quay trái với tốc độ v
 *  
 *  @param[in]  v
 *              Tốc độ quay: 0-255
 */
void rotate_left(uchar v){
    control_motor(v, DIR_BACK, v, DIR_FORWARD);  
}

/*
 *  @brief  Cho xe quay phải với tốc độ v
 *  
 *  @param[in]  v
 *              Tốc độ quay: 0-255
 */
void rotate_right(uchar v){
    control_motor(v, DIR_FORWARD, v, DIR_BACK);  
}

/*
 *  @brief  Cho xe chạy thẳng với tốc độ v
 *  
 *  @param[in]  v
 *              Tốc độ quay: 0-255
 */
void go_forward(uchar v){
    control_motor(v, DIR_FORWARD, v, DIR_FORWARD);  
}

/*
 *  @brief  Cho xe chạy lùi với tốc độ v
 *  
 *  @param[in]  v
 *              Tốc độ quay: 0-255
 */
void go_back(uchar v){
    control_motor(v, DIR_BACK, v, DIR_BACK);  
}

/***********************************************************************************
 ***************************CÁC HÀM ĐỌC TÍN HIỆU SENSORS****************************
 ***********************************************************************************/
 
/*
 *  @brief  Đọc tín hiệu cảm biến vạch trái
 *  
 *  @return LINE_BLACK:   Có vạch
 *          OFF:          Không có vạch
 */
uchar check_line_left(){
  uint val=0;
  for(int i=0; i<CONSTANT_NOISE1; i++){
    val = val + digitalRead(SENSOR_LINE_LEFT);
  }
  if(val > CONSTANT_NOISE2) return LINE_BLACK;
  else return OFF;
}

/*
 *  @brief  Đọc tín hiệu cảm biến vạch phải
 *  
 *  @return LINE_BLACK:   Có vạch
 *          OFF:          Không có vạch
 */
uchar check_line_right(){
  uint val=0;
  for(int i=0; i<CONSTANT_NOISE1; i++){
    val = val + digitalRead(SENSOR_LINE_RIGHT);
  }
  if(val > CONSTANT_NOISE2) return LINE_BLACK;
  else return OFF;
}

/*
 *  @brief  Đọc tín hiệu cảm biến vạch phía sau
 *  
 *  @return LINE_BLACK:   Có vạch
 *          OFF:          Không có vạch
 */
uchar check_line_behind(){
  uint val=0;
  for(int i=0; i<CONSTANT_NOISE1; i++){
    val = val + digitalRead(SENSOR_LINE_BEHIND);
  }
  if(val > CONSTANT_NOISE2) return LINE_BLACK;
  else return OFF;
}

/*
 *  @brief  Kiểm tra trạng thái cả 3 cảm biến vạch
 *  
 *  @return LINE_BLACK:   Xe chạm vạch (ít nhất 1 trong 3 cảm biến có vạch)
 *          OFF:          Không có vạch
 */
uchar has_line(){
  if((check_line_left()==LINE_BLACK)||(check_line_right()==LINE_BLACK)||(check_line_behind()==LINE_BLACK)){
    return LINE_BLACK;
  }
  return OFF;
}

/*
 *  @brief  Kiểm tra trạng thái cả 2 cảm biến vạch phía trước
 *  
 *  @return LINE_BLACK:   Đầu xe chạm vạch (cảm biến bên trái hoặc bên phải có vạch)
 *          OFF:          Không có vạch
 */
uchar has_line_front(){
  if((check_line_left()==LINE_BLACK)||(check_line_right()==LINE_BLACK)){
    return LINE_BLACK;
  }
  return OFF;
}

/*
 *  @brief      Hàm chuyển đổi từ giá trị đọc về của cảm biến (adc) khoảng cách sang cm, 
 *              Phục vụ cho hàm measure_distance()
 *  
 *  @param[in]  adc
 *              Giá trị đọc về từ cảm biến khoảng cách
 *              
 *  @return     Khoảng cách tính theo cm
 *  
 *  @note       Giá trị chỉ là tương đối, phụ thuộc nhiều vào chất lượng cảm biến --> cần test 
 */
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

/*
 *  @brief      Hàm chuyển đổi từ giá trị đọc về của cảm biến (adc) khoảng cách sang cm
 *  
 *  @return     Khoảng cách vật cản phía trước tính theo cm (0-70cm)
 *  
 *  @note       Giá trị chỉ là tương đối, phụ thuộc nhiều vào chất lượng cảm biến --> cần test 
 *              Thường dùng khoảng cách 30cm, 40cm
 */
int measure_distance(){
  int val_tmp, average=0;
  for(int i=0; i<5; i++){
    val_tmp = analogRead(SENSOR_DISTANCE);
    average = average + val_tmp;
    delay(1);
  }
  average = average/5;

  /*#ifdef DEBUG_SERIAL
    debug("-------------------");
    debug(average, DEC);      //value_adc
    debug((int)((float)average*VREF/ANALOG_VALUE_MAX), DEC);         //mV
  #endif*/
  return value_to_cm(average);
}

/***********************************************************************************
 *******************************CÁC HÀM ĐỂ TEST*************************************
 ***********************************************************************************/
 
#ifdef DEBUG_SERIAL

/*
 *  @brief      Kiểm tra hoạt động của các động cơ
 */
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

/*
 *  @brief      Kiểm tra hoạt động của các động cơ theo các giá trị pwm khác nhau
 */
void test_motor_pwm(){
  unsigned int i=0;
  debug("LEFT: Forward - RIGHT: Forward. Test pwm_value");
  for(i=0; i<256; i=i+5){
    debug(i, DEC);
    control_motor(i, DIR_FORWARD, i, DIR_FORWARD);
    delay(500);  
  }
}

/*
 *  @brief      Kiểm tra hoạt động của các cảm biến vạch
 */
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

/*
 *  @brief      Kiểm tra hoạt động của cảm biến khoảng cách
 */
void test_sensor_distance(){
  int val=0;
  debug("---Distance:");
  val=measure_distance();
  debug(measure_distance(),DEC);
  delay(1000);
}

/*
 *  @brief      Hàm test chính
 *              Enable, disable các hàm test cục bộ vào đây
 */
void test(){
  test_motor();
  //test_motor_pwm();
  //test_sensor_distance();
  //test_sensor_line_polling();
}
#endif

/***********************************************************************************
 *******************CÁC HÀM ĐỂ GIAO TIẾP BLE VỚI SMARTPHONE, TABLET*****************
 ***********************************************************************************/

/** @brief  Hàm callback cho sự kiện disconnect
 *
 *  @param[in] *params   params->handle : connect handle
 */
void disconnection_callback(const Gap::DisconnectionCallbackParams_t *params) {
  #ifdef DEBUG_SERIAL
    debug("Disconnected");
    debug("Restart advertising");
  #endif
  ble.startAdvertising();
}

/** @brief  Hàm callback cho sự kiện connect
 *
 *  @param[in] *params   params->handle : The ID for this connection
 *                       params->role : PERIPHERAL  = 0x1, // Peripheral Role
 *                                      CENTRAL     = 0x2, // Central Role.
 */
void connection_callback( const Gap::ConnectionCallbackParams_t *params ) {
  #ifdef DEBUG_SERIAL
    debug("Connected");
    if(params->role == Gap::PERIPHERAL) {
      debug("+++Peripheral ");
    }
  #endif
}

/** @brief  Hàm callback cho sự kiện ghi dữ liệu từ gatt server
 *          Stop, Go Forward, Go Back, Rotate Left, Rotate Right
 *
 *  @param[in] *Handler   Handler->connHandle : The handle of the connection that triggered the event
 *                        Handler->handle : Attribute Handle to which the write operation applies
 *                        Handler->writeOp : OP_INVALID               = 0x00,  // Invalid operation.
 *                                           OP_WRITE_REQ             = 0x01,  // Write request.
 *                                           OP_WRITE_CMD             = 0x02,  // Write command.            ////
 *                                           OP_SIGN_WRITE_CMD        = 0x03,  // Signed write command.
 *                                           OP_PREP_WRITE_REQ        = 0x04,  // Prepare write request.
 *                                           OP_EXEC_WRITE_REQ_CANCEL = 0x05,  // Execute write request: cancel all prepared writes.
 *                                           OP_EXEC_WRITE_REQ_NOW    = 0x06,  // Execute write request: immediately execute all prepared writes.
 *                        Handler->offset : Offset for the write operation
 *                        Handler->len : Length (in bytes) of the data to write
 *                        Handler->data : Pointer to the data to write
 */
void gattserver_write_callback(const GattWriteCallbackParams *Handler) {
  static uint8_t buf_motor[TXRX_BUF_LEN];
  static uint16_t bytes_read=0;

  //Stop
  if(Handler->handle == motor_stop_characteristic.getValueAttribute().getHandle()){
    stop_car();
    #ifdef DEBUG_SERIAL
      debug("Stop");
    #endif
  }
  //Go forward
  else if(Handler->handle == motor_forward_characteristic.getValueAttribute().getHandle()){
    ble.readCharacteristicValue(motor_forward_characteristic.getValueAttribute().getHandle(), buf_motor, &bytes_read);       // Read the value of characteristic
    go_forward(buf_motor[0]);           //v = data[0]
    #ifdef DEBUG_SERIAL
      debug("Go forward");
      debug(buf_motor[0]);
    #endif
  }
  //Go back
  else if(Handler->handle == motor_back_characteristic.getValueAttribute().getHandle()){
    ble.readCharacteristicValue(motor_back_characteristic.getValueAttribute().getHandle(), buf_motor, &bytes_read);       // Read the value of characteristic
    go_back(buf_motor[0]);           //v = data[0]
    #ifdef DEBUG_SERIAL
      debug("Go back");
      debug(buf_motor[0]);
    #endif
  }
  //Rotate left
  else if(Handler->handle == motor_left_characteristic.getValueAttribute().getHandle()){
    ble.readCharacteristicValue(motor_left_characteristic.getValueAttribute().getHandle(), buf_motor, &bytes_read);       // Read the value of characteristic
    rotate_left(buf_motor[0]);           //v = data[0]
    #ifdef DEBUG_SERIAL
      debug("Rotate left");
      debug(buf_motor[0]);
    #endif
  }
  //Rotate right
  else if(Handler->handle == motor_right_characteristic.getValueAttribute().getHandle()){
    ble.readCharacteristicValue(motor_right_characteristic.getValueAttribute().getHandle(), buf_motor, &bytes_read);       // Read the value of characteristic
    rotate_right(buf_motor[0]);           //v = data[0]
    #ifdef DEBUG_SERIAL
      debug("Rotate right");
      debug(buf_motor[0]);
    #endif
  }
}

/*
 * @brief     Hàm đọc rồi gửi giá trị cảm biến vạch bên trái lên smartphone
 * 
 * @note      Giá trị smartphone nhận được:
 *                  1 (LINE_BLACK): Có vạch
 *                  0 (OFF):        Không có vạch
 */
void send_sensor_left_to_smartphone(){
  static uint8_t val_for= 0, val_cur = 0;
  val_cur= check_line_left();
  if(val_cur != val_for){
    ble.updateCharacteristicValue(sensor_left_characteristic.getValueAttribute().getHandle(), &val_cur, 1);
    val_for = val_cur;
  #ifdef DEBUG_SERIAL
    debug("Left line");
    debug(val_cur);
  #endif
  }
}

/*
 * @brief     Hàm đọc rồi gửi giá trị cảm biến vạch bên phải lên smartphone
 * 
 * @note      Giá trị smartphone nhận được:
 *                  1 (LINE_BLACK): Có vạch
 *                  0 (OFF):        Không có vạch
 */
void send_sensor_right_to_smartphone(){
  static uint8_t val_for= 0, val_cur = 0;
  val_cur= check_line_right();
  if(val_cur != val_for){
    ble.updateCharacteristicValue(sensor_right_characteristic.getValueAttribute().getHandle(), &val_cur, 1);
    val_for = val_cur;
  #ifdef DEBUG_SERIAL
    debug("Right line");
    debug(val_cur);
  #endif
  }
}

/*
 * @brief     Hàm đọc rồi gửi giá trị cảm biến vạch phía sau lên smartphone
 * 
 * @note      Giá trị smartphone nhận được:
 *                  1 (LINE_BLACK): Có vạch
 *                  0 (OFF):        Không có vạch
 */
void send_sensor_behind_to_smartphone(){
  static uint8_t val_for= 0, val_cur = 0;
  val_cur= check_line_behind();
  if(val_cur != val_for){
    ble.updateCharacteristicValue(sensor_behind_characteristic.getValueAttribute().getHandle(), &val_cur, 1);
    val_for = val_cur;
  #ifdef DEBUG_SERIAL
    debug("Behind line");
    debug(val_cur);
  #endif
  }
}

/*
 * @brief     Hàm đọc rồi gửi giá trị cảm biến khoảng cách lên smartphone
 * 
 * @note      Giá trị smartphone nhận được là cm
 * 
 */
void send_sensor_distance_to_smartphone(){
  static uint8_t val_for= 0, val_cur = 0;
  val_cur= measure_distance();
  if((val_cur > val_for+1)||(val_cur < val_for - 1)){
    ble.updateCharacteristicValue(sensor_distance_characteristic.getValueAttribute().getHandle(), &val_cur, 1);
    val_for = val_cur;
  #ifdef DEBUG_SERIAL
    debug("Distance");
    debug(val_cur);
  #endif
  }
}

/**
 * @brief  Timer task callback handle
 * 
 * @note    Đây chỉ là hàm demo việc gửi dữ liệu cảm biến lên smartphone
 */
void task_handle(void) {
    // if false or ignore, notification or indication is generated if permit.
    send_sensor_left_to_smartphone();
    send_sensor_right_to_smartphone();
    send_sensor_behind_to_smartphone();
    send_sensor_distance_to_smartphone();
    delay(1);

}

/**
 * @brief  Set advertisement
 */
void set_advertisement(void) {
  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  // Add short name to advertisement
  ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,(const uint8_t *)"Bot1", 4);
  // Add complete 128bit_uuid to advertisement
  ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,(const uint8_t *)vng_bot_uuid, sizeof(vng_bot_uuid));
    // Add complete device name to scan response data
  ble.accumulateScanResponse(GapAdvertisingData::COMPLETE_LOCAL_NAME,(const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
}

/*
 * @brief   Cài đặt cho BLE
 * 
 * @note    Hiện tại đang dùng ticker1s để gửi giá trị các cảm biến lên smartphone
 *          Cần sửa lại theo ý tưởng: Khi nào có cảm biến vạch hoặc có đối thủ thì gửi lên
 */
 void setup_ble(){
  // Init timer task
  //ticker1s.attach(task_handle, 1);        //test các cảm biến theo chu kỳ 1s
  // Init ble
  ble.init();
  ble.onConnection(connection_callback);
  ble.onDisconnection(disconnection_callback);
  ble.onDataWritten(gattserver_write_callback);
  // set advertisement
  set_advertisement();
  // set adv_type(enum from 0)
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);       //un
  // add service
  ble.addService(motorService);
  ble.addService(sensorService);
  // set device name
  ble.setDeviceName((const uint8_t *)DEVICE_NAME);
  // set tx power,valid values are -40, -20, -16, -12, -8, -4, 0, 4
  ble.setTxPower(4);
  // set adv_interval, 100ms in multiples of 0.625ms.
  ble.setAdvertisingInterval(300);      //160
  // set adv_timeout, in seconds
  ble.setAdvertisingTimeout(0);
  // start advertising
  ble.startAdvertising();
 }

 
/***********************************************************************************
 ********************************CÁC HÀM ĐỂ CHIẾN ĐẤU*******************************
 ***********************************************************************************/

/*
 *  @brief      Hàm kiểm tra xem có đối thủ phía trước không
 *  
 *  @return     0: Không có đối thủ
 *              1: Đối thủ rất gần khoảng <10cm
 *              2: Đối thủ khoảng 20cm
 *              3: Đối thủ khoảng 30cm
 *            
 *  @note       Có thể gia tăng khoảng cách phát hiện tùy cảm biến
 *              Sử dụng hàm test_sensor_distance để kiểm tra
 */
uchar check_you(){
  int val = measure_distance();
  if(val==0)    return 0;             
  if(val<10)    return 1;
  if(val<20)    return 2;
  if(val<30)    return 3;
  return 0;
}

/*
 *  @brief      Thuật toán 1 dùng để chiến đấu
 */
void alg1(){

}

/***********************************************************************************
 *****************************CÁC HÀM CƠ BẢN CỦA ARDUINO****************************
 ***********************************************************************************/

 /*
 *  @brief      Hàm cài đặt ban đầu (Chỉ chạy 1 lần khi vừa bật nguồn)
 */
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

  //init BLE
  setup_ble();
  
  ticker1s.attach(task_handle, 0.05);        //test các cảm biến theo chu kỳ 0.05s

  //init test
  #ifdef DEBUG_SERIAL
    Serial.begin(115200); 
    Serial.println("Start test application");
  #endif
}

 /*
 *  @brief      Hàm này chạy trong một vòng lặp vô hạn
 */
void loop() {
  //Test
  #ifdef DEBUG_SERIAL
   //   test();
  
  #else
  //Chiến đấu
//  alg1();
  
  //for BLE
  ble.waitForEvent();
  #endif
}
