#include "SPI.h"
#include <Wire.h>
#include <INA219_WE.h>

INA219_WE ina219; // this is the instantiation of the library for the current sensor

//*******************************optical Sensor*****************//

#define PIN_SS        10
#define PIN_MISO      12
#define PIN_MOSI      11
#define PIN_SCK       13

#define PIN_MOUSECAM_RESET     8
#define PIN_MOUSECAM_CS        7

#define ADNS3080_PIXELS_X                 30
#define ADNS3080_PIXELS_Y                 30

#define ADNS3080_PRODUCT_ID            0x00
#define ADNS3080_REVISION_ID           0x01
#define ADNS3080_MOTION                0x02
#define ADNS3080_DELTA_X               0x03
#define ADNS3080_DELTA_Y               0x04
#define ADNS3080_SQUAL                 0x05
#define ADNS3080_PIXEL_SUM             0x06
#define ADNS3080_MAXIMUM_PIXEL         0x07
#define ADNS3080_CONFIGURATION_BITS    0x0a
#define ADNS3080_EXTENDED_CONFIG       0x0b
#define ADNS3080_DATA_OUT_LOWER        0x0c
#define ADNS3080_DATA_OUT_UPPER        0x0d
#define ADNS3080_SHUTTER_LOWER         0x0e
#define ADNS3080_SHUTTER_UPPER         0x0f
#define ADNS3080_FRAME_PERIOD_LOWER    0x10
#define ADNS3080_FRAME_PERIOD_UPPER    0x11
#define ADNS3080_MOTION_CLEAR          0x12
#define ADNS3080_FRAME_CAPTURE         0x13
#define ADNS3080_SROM_ENABLE           0x14
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_LOWER      0x19
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_UPPER      0x1a
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_LOWER      0x1b
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_UPPER      0x1c
#define ADNS3080_SHUTTER_MAX_BOUND_LOWER           0x1e
#define ADNS3080_SHUTTER_MAX_BOUND_UPPER           0x1e
#define ADNS3080_SROM_ID               0x1f
#define ADNS3080_OBSERVATION           0x3d
#define ADNS3080_INVERSE_PRODUCT_ID    0x3f
#define ADNS3080_PIXEL_BURST           0x40
#define ADNS3080_MOTION_BURST          0x50
#define ADNS3080_SROM_LOAD             0x60

#define ADNS3080_PRODUCT_ID_VAL        0x17


float total_x = 0;

float total_y = 0;


float total_x1 = 0;

float total_y1 = 0;


float x = 0;
float y = 0;

float a = 0;
float b = 0;

int max_x = 50;
int max_y = 50;

float distance_x = 0;
float distance_y = 0;

boolean lcorrection = false;
boolean rcorrection = false;

volatile byte movementflag = 0;
volatile int xydat[2];

//***********************************************SMPS***********************************//
float open_loop, closed_loop; // Duty Cycles
float vpd, vb, vref, iL, dutyref, current_mA; // Measurement Variables
unsigned int sensorValue0, sensorValue1, sensorValue2, sensorValue3; // ADC sample values declaration
float ev = 0, cv = 0, ei = 0, oc = 0; //internal signals
float Ts = 0.0008; //1.25 kHz control frequency. It's better to design the control period as integral multiple of switching period.
float kpv = 0.05024, kiv = 15.78, kdv = 0; // voltage pid.
float u0v, u1v, delta_uv, e0v, e1v, e2v; // u->output; e->error; 0->this time; 1->last time; 2->last last time
float kpi = 0.02512, kii = 39.4, kdi = 0; // current pid.
float u0i, u1i, delta_ui, e0i, e1i, e2i; // Internal values for the current controller
float uv_max = 4, uv_min = 0; //anti-windup limitation
float ui_max = 1, ui_min = 0; //anti-windup limitation
float current_limit = 1.0;
boolean Boost_mode = 0;
boolean CL_mode = 0;


unsigned int loopTrigger;
unsigned int com_count = 0; // a variables to count the interrupts. Used for program debugging.

//************************** Motor Constants **************************//
unsigned long previousMillis = 0; //initializing time counter
const long f_i = 10000;           //time to move in forward direction, please calculate the precision and conversion factor
const long r_i = 13000;           //time to rotate clockwise
const long b_i = 16000;           //time to move backwards
const long l_i = 19000;           //time to move anticlockwise
const long s_i = 50000;
int DIRRstate = LOW;              //initializing direction states
int DIRLstate = HIGH;

int DIRL = 20;                    //defining left direction pin
int DIRR = 21;                    //defining right direction pin

float speed_factor = 1;

int pwmr = 5;                     //pin to control right wheel speed using pwm
int pwml = 9;                     //pin to control left wheel speed using pwm

int opcode = 0;                 //0 for left turn, 3 for right turn, 1 for moving forward, and 2 for moving backward
int operand = 0;
boolean turning = false;              //true for it is turning, false for ready turn.
boolean finish = true;       //status of current command
//int command[] = {1, 20, 0, 90, 1, 10, 3, 90, 4}; //aray for the command
//int command[];
int current_command = 0;          //record the current command
float current_x = 0;
float current_y = 0;
int instructionCompleteTime = 0;
float e0p, e_integration, delta_up, u0p, u1p, e2p, e1p;
float kpp = 1, kip = 1;
boolean firstFinish = true;
String curr_x;
String curr_y;
boolean firstTime = true;
//*******************************************************************//

struct MD
{
  byte motion;
  char dx, dy;
  byte squal;
  word shutter;
  byte max_pix;
};

struct coordinate
{
  float x;
  float y;
};

coordinate current_cor = {0, 0};

void setup()
{
  pinMode(PIN_SS, OUTPUT);
  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_SCK, OUTPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);

  Serial.begin(115200);
  Serial1.begin(9600);
  if (mousecam_init() == -1)
  {
    Serial.println("Mouse cam failed to init");
    while (1);
  }

  //************************** Motor Pins Defining **************************//
  pinMode(DIRR, OUTPUT);
  pinMode(DIRL, OUTPUT);
  pinMode(pwmr, OUTPUT);
  pinMode(pwml, OUTPUT);
  digitalWrite(pwmr, LOW);       //setting right motor speed at 0
  digitalWrite(pwml, LOW);       //setting left motor speed at 0
  //*******************************************************************//

  //Basic pin setups

  noInterrupts(); //disable all interrupts
  pinMode(13, OUTPUT);  //Pin13 is used to time the loops of the controller
  pinMode(3, INPUT_PULLUP); //Pin3 is the input from the Buck/Boost switch
  pinMode(2, INPUT_PULLUP); // Pin 2 is the input from the CL/OL switch
  analogReference(EXTERNAL); // We are using an external analogue reference for the ADC

  // TimerA0 initialization for control-loop interrupt.

  TCA0.SINGLE.PER = 999; //
  TCA0.SINGLE.CMP1 = 999; //
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm; //16 prescaler, 1M.
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm;

  // TimerB0 initialization for PWM output

  pinMode(6, OUTPUT);
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm; //62.5kHz
  analogWrite(6, 120);

  interrupts();  //enable interrupts.
  Wire.begin(); // We need this for the i2c comms for the current sensor
  ina219.init(); // this initiates the current sensor
  Wire.setClock(700000); // set the comms speed for i2c
}


void loop()
{
#if 0

  /*
      if(movementflag){

      tdistance = tdistance + convTwosComp(xydat[0]);
      Serial.println("Distance = " + String(tdistance));
      movementflag=0;
      delay(3);
      }

  */
  // if enabled this section grabs frames and outputs them as ascii art
  if (mousecam_frame_capture(frame) == 0)
  {
    int i, j, k;
    for (i = 0, k = 0; i < ADNS3080_PIXELS_Y; i++)
    {
      for (j = 0; j < ADNS3080_PIXELS_X; j++, k++)
      {
        Serial.print(asciiart(frame[k]));
        Serial.print(' ');
      }
      Serial.println();
    }
  }
  Serial.println();
  delay(250);

#else

  // if enabled this section produces a bar graph of the surface quality that can be used to focus the camera
  // also drawn is the average pixel value 0-63 and the shutter speed and the motion dx,dy.

  int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
  MD md;
  mousecam_read_motion(&md);
  for (int i = 0; i < md.squal / 4; i++)
    Serial.print('*');
  Serial.print(' ');
  Serial.print((val * 100) / 351);
  Serial.print(' ');
  Serial.print(md.shutter); Serial.print(" (");
  Serial.print((long)md.dx); Serial.print(',');
  Serial.print((long)md.dy); Serial.println(')');

  // Serial.println(md.max_pix);
  delay(100);


  distance_x = md.dx; //convTwosComp(md.dx);
  distance_y = md.dy; //convTwosComp(md.dy);

  total_x1 = (total_x1 + distance_x);
  total_y1 = (total_y1 + distance_y);

  total_x = total_x1 / 157; //Conversion from counts per inch to cm (400 counts per inch)
  total_y = total_y1 / 157; //Conversion from counts per inch to cm (400 counts per inch)


  Serial.print('\n');


  Serial.println("Distance_x = " + String(total_x));

  Serial.println("Distance_y = " + String(total_y));
  Serial.print('\n');

  delay(100);
#endif
  //*****************************************Motor****************************************************
  unsigned long currentMillis = millis();
  if (loopTrigger) { // This loop is triggered, it wont run unless there is an interrupt

    digitalWrite(13, HIGH);   // set pin 13. Pin13 shows the time consumed by each control cycle. It's used for debugging.

    // Sample all of the measurements and check which control mode we are in
    sampling();
    CL_mode = digitalRead(3); // input from the OL_CL switch
    Boost_mode = digitalRead(2); // input from the Buck_Boost switch

    if (Boost_mode) {
      if (CL_mode) { //Closed Loop Boost
        pwm_modulate(1); // This disables the Boost as we are not using this mode
      } else { // Open Loop Boost
        pwm_modulate(1); // This disables the Boost as we are not using this mode
      }
    } else {
      if (CL_mode) { // Closed Loop Buck
        // The closed loop path has a voltage controller cascaded with a current controller. The voltage controller
        // creates a current demand based upon the voltage error. This demand is saturated to give current limiting.
        // The current loop then gives a duty cycle demand based upon the error between demanded current and measured
        // current
        current_limit = 3; // Buck has a higher current limit
        ev = vref - vb;  //voltage error at this time
        cv = pidv(ev); //voltage pid
        cv = saturation(cv, current_limit, 0); //current demand saturation
        ei = cv - iL; //current error
        closed_loop = pidi(ei); //current pid
        closed_loop = saturation(closed_loop, 0.99, 0.01); //duty_cycle saturation
        pwm_modulate(closed_loop); //pwm modulation
      } else { // Open Loop Buck
        current_limit = 3; // Buck has a higher current limit
        oc = iL - current_limit; // Calculate the difference between current measurement and current limit
        if ( oc > 0) {
          open_loop = open_loop - 0.001; // We are above the current limit so less duty cycle
        } else {
          open_loop = open_loop + 0.001; // We are below the current limit so more duty cycle
        }
        open_loop = saturation(open_loop, dutyref, 0.02); // saturate the duty cycle at the reference or a min of 0.01
        pwm_modulate(open_loop); // and send it out
      }
    }
    // closed loop control path

    digitalWrite(13, LOW);   // reset pin13.
    loopTrigger = 0;
  }
  //*********************************Movement testing************************************************
  current_cor = cor(total_x, total_y, distance_x, distance_y, current_cor);
  if (finish == true) {
    Serial1.print("11000000"); // 111111 for ready
    Serial1.print('\n');
    //Serial.println("11");
    String data_received = String(Serial1.readStringUntil('\n'));
    //Serial.println(String(data_received));
    firstTime = true;
    finish = false;
    speed_factor = String(data_received[0]).toInt();
    opcode =  String(data_received[1]).toInt();
    operand = String(String(data_received[2]) + String(data_received[3]) + String(data_received[4])).toInt();
  } else {
    switch (opcode) {
      case (5): {
          if (turning == false) {
            current_x = 0;
            current_y = 0;
            turning = true;
          } else {
            current_x = current_x + distance_x / 157;
            current_y = current_y + distance_y / 157;
            finish = turning_left(operand, current_x, speed_factor);
            if (finish == true) {
              curr_x = String(int(abs(current_cor.x)));
              curr_y = String(int(abs(current_cor.y)));    
              if (curr_x.length() == 1) {
                curr_x = String("00" + curr_x);
              } else if (curr_x.length() == 2) {
                curr_x = String("0" + curr_x);
              } else if (curr_x.length() == 0)  {
                curr_x = String("000"); 
              }
              if (curr_y.length() == 1) {
                curr_y = String("00" + curr_y);
              } else if (curr_y.length() == 2) {
                curr_y = String("0" + curr_y);
              } else if (curr_y.length() == 0) {
                curr_y = String("000");
              }
                Serial1.print("33" + String(curr_x) + String(curr_y));
                Serial1.print('\n');
                //Serial.println("33");
            }
            turning = not finish;
          }
          break;
        }

      case (3): {
          if (turning == false) {
            current_x = 0;
            current_y = 0;
            turning = true;
          } else {
            current_x = current_x + distance_x / 157;
            current_y = current_y + distance_y / 157;
            finish = turning_right(operand, current_x, speed_factor);
            if (finish == true) {
              curr_x = String(int(abs(current_cor.x)));
              curr_y = String(int(abs(current_cor.y)));    
              if (curr_x.length() == 1) {
                curr_x = String("00" + curr_x);
              } else if (curr_x.length() == 2) {
                curr_x = String("0" + curr_x);
              } else if (curr_x.length() == 0)  {
                curr_x = String("000"); 
              }
              if (curr_y.length() == 1) {
                curr_y = String("00" + curr_y);
              } else if (curr_y.length() == 2) {
                curr_y = String("0" + curr_y);
              } else if (curr_y.length() == 0) {
                curr_y = String("000");
              }
                Serial1.print("33" + String(curr_x) + String(curr_y));
                Serial1.print('\n');
                //Serial.println("33");
            }
            turning = not finish;
          }
          break;
        }

      case (1): {
          if (turning == false) {
            current_x = 0;
            current_y = 0;
            turning = true;
          } else {
            current_x = current_x + distance_x / 157;
            current_y = current_y + distance_y / 157;
            finish = moving_forward(operand, current_y, speed_factor);
            Serial.println("Finish:" + String(finish));
            if (finish == true) {
              curr_x = String(int(abs(current_cor.x)));
              curr_y = String(int(abs(current_cor.y)));    
              if (curr_x.length() == 1) {
                curr_x = String("00" + curr_x);
              } else if (curr_x.length() == 2) {
                curr_x = String("0" + curr_x);
              } else if (curr_x.length() == 0)  {
                curr_x = String("000"); 
              }
              if (curr_y.length() == 1) {
                curr_y = String("00" + curr_y);
              } else if (curr_y.length() == 2) {
                curr_y = String("0" + curr_y);
              } else if (curr_y.length() == 0) {
                curr_y = String("000");
              }
                Serial1.print("33" + String(curr_x) + String(curr_y));
                Serial1.print('\n');
                //Serial.println("33");
            }
            turning = not finish;
          }
          break;
        }

        case (2): {
          if (turning == false) {
            current_x = 0;
            current_y = 0;
            turning = true;
          } else {
            current_x = current_x + distance_x / 157;
            current_y = current_y + distance_y / 157;
            finish = moving_backward(operand, current_y, speed_factor);
            Serial.println("Finish:" + String(finish));
            if (finish == true) {
              curr_x = String(int(abs(current_cor.x)));
              curr_y = String(int(abs(current_cor.y)));    
              if (curr_x.length() == 1) {
                curr_x = String("00" + curr_x);
              } else if (curr_x.length() == 2) {
                curr_x = String("0" + curr_x);
              } else if (curr_x.length() == 0)  {
                curr_x = String("000"); 
              }
              if (curr_y.length() == 1) {
                curr_y = String("00" + curr_y);
              } else if (curr_y.length() == 2) {
                curr_y = String("0" + curr_y);
              } else if (curr_y.length() == 0) {
                curr_y = String("000");
              }
                Serial1.print("33" + String(curr_x) + String(curr_y));
                Serial1.print('\n');
                //Serial.println("33");
            }
            turning = not finish;
          }
          break;
        }

      case(4): {
          rover_stop();
          Serial1.print("22000000");
          Serial1.print('\n');
          //Serial.println("22");
          finish = true;
          break;
        }

     case(0):{
          rover_stop();
          finish = true;
          break;
     }
    }
  }
}
