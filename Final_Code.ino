#include <Wire.h>
#include <INA219_WE.h>
#include <SPI.h>
#include <SD.h>

INA219_WE ina219; // this is the instantiation of the library for the current sensor

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10;
unsigned int rest_timer;
unsigned int loop_trigger;
unsigned int int_count = 0; // a variables to count the interrupts. Used for program debugging.
float u0i, u1i, delta_ui, e0i, e1i, e2i; // Internal values for the current controller
float u0v,u1v,delta_uv,e0v,e1v,e2v; // u->output; e->error; 0->this time; 1->last time; 2->last last time
float uv_max=4, uv_min=0; //anti-windup limitation
float ui_max = 1, ui_min = 0; //anti-windup limitation
float kpi = 0.02512, kii = 39.4, kdi = 0; // current pid.
float kpv=0.05024,kiv=15.78,kdv=0; // voltage pid.
float Ts = 0.001; //1 kHz control frequency.
float current_measure, current_ref = 0, error_amps,ei,current_limit = 0; // Current Control
float voltage_ref = 0, ev; //Constant Voltage
float pwm_out,cv;
float V_Bat, vb;
float V_meas1,V_meas2,V_meas3,V_sum,V_avg;
float V_total1,V_total2,V_total3;
float V_mean1,V_mean2,V_mean3;
float p_prev,p_now;
bool finish;
int cc;
float soc = 0; //State of Charge
boolean input_switch;
int state_num=0,next_state;
String dataString;
String controlString;

void setup() {
  //Some General Setup Stuff

  Wire.begin(); // We need this for the i2c comms for the current sensor
  Wire.setClock(700000); // set the comms speed for i2c
  ina219.init(); // this initiates the current sensor
  Serial.begin(9600); // USB Communications


  //Check for the SD Card
  Serial.println("\nInitializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("* is a card inserted?");
    while (true) {} //It will stick here FOREVER if no SD is in on boot
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  if (SD.exists("BatCycle.csv")) { // Wipe the datalog when starting
    SD.remove("BatCycle.csv");
  }

  
  noInterrupts(); //disable all interrupts
  analogReference(EXTERNAL); // We are using an external analogue reference for the ADC

  //SMPS Pins
  pinMode(13, OUTPUT); // Using the LED on Pin D13 to indicate status
  pinMode(2, INPUT_PULLUP); // Pin 2 is the input from the CL/OL switch
  pinMode(6, OUTPUT); // This is the PWM Pin

  //Cell 1 Arrangement
  pinMode(5, OUTPUT); //Relay
  pinMode(9, OUTPUT); //Discharge
  pinMode(A1, INPUT); //Measurement

  //Cell 2 Arrangement
  pinMode(4, OUTPUT); //Relay
  pinMode(3, OUTPUT); //Discharge
  pinMode(A2, INPUT); //Measurement

  //Cell 2 Arrangement
  pinMode(4, OUTPUT); //Relay
  pinMode(3, OUTPUT); //Discharge
  pinMode(A2, INPUT); //Measurement

  //Cell 3 Arrangement
  pinMode(8, OUTPUT); //Relay
  pinMode(7, OUTPUT); //Discharge
  pinMode(A7, INPUT); //Measurement
  
  //LEDs on pin 7 and 8
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  //Analogue input, the battery voltage (also port B voltage)
  pinMode(A0, INPUT);

  // TimerA0 initialization for 1kHz control-loop interrupt.
  TCA0.SINGLE.PER = 999; //
  TCA0.SINGLE.CMP1 = 999; //
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm; //16 prescaler, 1M.
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm;

  // TimerB0 initialization for PWM output
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm; //62.5kHz

  interrupts();  //enable interrupts.
  analogWrite(6, 120); //just a default state to start with

}


void loop() {
  if (loop_trigger == 1){ // FAST LOOP (1kHZ)
      state_num = next_state; //state transition
      V_Bat = analogRead(A0)*4.096/1.03; //check the battery voltage (1.03 is a correction for measurement error, you need to check this works for you)
      if ((V_Bat > 3700 || V_Bat < 2400)) { //Checking for Error states (just battery voltage for now)
          state_num = 13; //go directly to jail
          next_state = 13; // stay in jail
          cc = 1;
          current_ref = 0; // no current
          voltage_ref = 0; // no voltage
      }
      if(cc == 1) { //Doing Constant Current 
      current_measure = (ina219.getCurrent_mA()); // sample the inductor current (via the sensor chip)
      if(current_measure >= 200){
      error_amps = (current_ref - current_measure) / 1000; //PID error calculation
      pwm_out = pidi(error_amps); //Perform the PID controller calculation
      pwm_out = saturation(pwm_out, 0.99, 0.01); //duty_cycle saturation
      analogWrite(6, (int)(255 - pwm_out * 255)); // write it out (inverting for the Buck here)
      }
      else{ //MPPT
        p_prev = 0;
        finish = false;
        while (!finish){
          p_now = V_Bat * current_measure;
          if (p_now - p_prev == 0){
            finish = true;
          }
          else if(p_now >p_prev){
            pwm_out = pwm_out - 0.005;
            p_prev = p_now;
          }
          else{
            pwm_out = pwm_out + 0.005;
            p_prev = p_now;
          }
        }
      pwm_out = saturation(pwm_out, 0.99, 0.01); //duty_cycle saturation
      analogWrite(6, (int)(255 - pwm_out * 255)); // write it out (inverting for the Buck here)
        }
      }
      
      else{
          vb = analogRead(A0)*4.096/1.03; 
          current_measure = (ina219.getCurrent_mA()); // sample the inductor current (via the sensor chip)
          current_limit = 0.5; // Buck has a higher current limit
          ev = (voltage_ref - vb) / 1000;  //voltage error at this time
          cv=pidv(ev);  //voltage pid
          cv=saturation(cv, current_limit, 0); //current demand saturation
          ei=cv - current_measure / 1000; //current error
          pwm_out = pidi(ei);  //current pid
          pwm_out = saturation(pwm_out,0.99,0.01);  //duty_cycle saturation
          analogWrite(6, (int)(255 - pwm_out * 255));
      }
      int_count++; //count how many interrupts since this was last reset to zero
      loop_trigger = 0; //reset the trigger and move on with life
  }
  
  if (int_count == 1000) { // SLOW LOOP (1Hz)
    input_switch = digitalRead(2); //get the OL/CL switch status
    switch (state_num) { // STATE MACHINE (see diagram)
      case 0:{ // Start state (no current, every measurements are zero)
        cc = 1;
        current_ref = 0;
        V_avg = 0;
        if (V_Bat >= 3530 ){ // Voltage LUT to determine SoC
          soc = 100;
        }
        else if (V_Bat < 3530 && V_Bat > 3300){
          soc = 95;
        }
        else if (V_Bat <= 3300 && V_Bat >= 3237){
          soc = 90;
        }
        else if (V_Bat <= 3237 && V_Bat > 3218){
          soc = 80;
        }
        else if (V_Bat <= 3218 && V_Bat > 3211){
          soc = 70;
        }
        else if (V_Bat <= 3211 && V_Bat > 3202){
          soc = 60;
        }
        else if (V_Bat <= 3202 && V_Bat > 3191){
          soc = 50;
        }
        else if (V_Bat <= 3191 && V_Bat > 3176){
          soc = 40;
        }
        else if (V_Bat <= 3176 && V_Bat > 3151){
          soc = 30;
        }
        else if (V_Bat <= 3151 && V_Bat > 3109){
          soc = 20;
        }
        else if (V_Bat <= 3109 && V_Bat > 3065){
          soc = 10;
        }
        else if (V_Bat <= 3065 && V_Bat > 3000){
          soc = 5;
        }
        else{
          soc = 0;
        }
        V_meas1 = 0, V_total1 = 0, V_meas2 = 0,V_total2 = 0, V_mean1 = 0, V_mean2 = 0, V_meas3 = 0, V_total3 = 0, V_mean3 = 0;
        // Turn off the relays and Discharges
        digitalWrite(7,false), digitalWrite(8,false), digitalWrite(5,false), digitalWrite(4,false), digitalWrite(9,false), digitalWrite(3,false);
        if (Serial1.read() == 'c') { // if receive 'c' from control, move to charge
          next_state = 1;
        } 
        else if (Serial1.read () == 'd'){// if receive 'd' from control, move to discharge
          next_state = 14;
        }
        else { // otherwise stay put
          next_state = 0;
        }
        break;
      }
      
      case 1:{ // CC Charge state until 3.5V (91.7%) with all cells measurement
        cc = 1;
        current_ref = 500;
        digitalWrite(9,false), digitalWrite(3,false), digitalWrite(7,false);//turn off all discharge
        V_sum = V_mean1 + V_mean2 + V_mean3;
        V_avg = V_sum/3;
        if (V_avg < 3500) { // Continue CC
          soc = soc + current_measure/6269868 * 100; 
          if (rest_timer < 60){
            rest_timer++;  
            next_state = 1;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(5,true);
            V_meas1 = analogRead(A1)*4.096/1.05;
            V_total1 += V_meas1;
            rest_timer++;
            next_state = 1; 
          }
          else if(rest_timer >= 70 && rest_timer < 80){
            digitalWrite(5,false);
            digitalWrite(4,true);
            V_meas2 = analogRead(A2)*4.096/1.06;
            V_total2 += V_meas2;
            rest_timer++;
            next_state = 1;
          }
          else if(rest_timer >= 80 && rest_timer < 90){
            digitalWrite(4,false);
            digitalWrite(8,true);
            V_meas3 = analogRead(A7)*4.096/1.06;
            V_total3 += V_meas3;
            rest_timer++;
            next_state = 1;
          }
          else if (rest_timer < 91){ // Calculate the average voltage of both cells and back to state 1 to decide whether to go to CV stage
            digitalWrite(8,false),digitalWrite(5,false),digitalWrite(4,false);
            V_mean1 = V_total1/10;
            V_mean2 = V_total2/10;
            V_mean3 = V_total3/10;
            next_state = 1;
            rest_timer++;
            V_meas1 = 0, V_total1 = 0, V_meas2 = 0, V_total2 = 0, V_meas3 = 0, V_total3 = 0;
          }
          else{ //Head to comparison state
            rest_timer = 0;
            next_state = 2;
          }
        } 
      else{ // otherwise go to charge with constant voltage
          rest_timer = 0;
          V_mean1 = 0, V_mean2 = 0, V_mean3 = 0;
          next_state = 3;
        }
        if(Serial1.read() == 'r'){ // UNLESS receive 'r' from control, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;
      }

      case 2:{ // CC Comparison State
        current_ref = 0;
        cc = 1;
        if (abs(V_mean1-V_mean2) > 200 || abs(V_mean1-V_mean3) > 200 || abs(V_mean3-V_mean2) > 200 ) { //Go to balance state
          next_state = 6;
        }
        else { // otherwise go to CC and measure OCV of both cells
          next_state = 1;
          rest_timer = 0;
        }
        if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }
      
      case 3:{ // CV with both cells measurement
        cc = 0;
        voltage_ref = 3600;
        digitalWrite(9,false), digitalWrite(3,false), digitalWrite(7,false);//turn off all discharge
        V_sum = V_mean1 + V_mean2 + V_mean3;
        V_avg = V_sum/3;
        if (V_avg < 3530) { // Continue CC
          soc = soc + current_measure/6269868 * 100; 
          if (rest_timer < 60){
            rest_timer++;  
            next_state = 3;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(5,true);
            V_meas1 = analogRead(A1)*4.096/1.05;
            V_total1 += V_meas1;
            rest_timer++;
            next_state = 3; 
          }
          else if(rest_timer >= 70 && rest_timer < 80){
            digitalWrite(5,false);
            digitalWrite(4,true);
            V_meas2 = analogRead(A2)*4.096/1.06;
            V_total2 += V_meas2;
            rest_timer++;
            next_state = 3;
          }
          else if(rest_timer >= 80 && rest_timer < 90){
            digitalWrite(4,false);
            digitalWrite(8,true);
            V_meas3 = analogRead(A7)*4.096/1.06;
            V_total3 += V_meas3;
            rest_timer++;
            next_state = 3;
          }
          else if (rest_timer < 91){ // Calculate the average voltage of both cells and back to state 3 to decide whether to go to REST stage
            digitalWrite(8,false),digitalWrite(5,false),digitalWrite(4,false);
            V_mean1 = V_total1/10;
            V_mean2 = V_total2/10;
            V_mean3 = V_total3/10;
            next_state = 3;
            rest_timer++;
            V_meas1 = 0, V_total1 = 0, V_meas2 = 0, V_total2 = 0, V_meas3 = 0, V_total3 = 0;
          }
          else{ //Head to comparison state
            rest_timer = 0;
            next_state = 4;
          }
        } 
      else{ // otherwise go to REST State
          rest_timer = 0;
          V_mean1 = 0, V_mean2 = 0, V_mean3 = 0;
          next_state = 5;
        }
        if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;
      }

      case 4:{ // CV Comparison
        cc = 1;
        current_ref = 0;
        if (abs(V_mean1-V_mean2) > 200 || abs(V_mean1-V_mean3) > 200 || abs(V_mean3-V_mean2) > 200 ) { //Go to balance state
          next_state = 6;
        }
        else { // otherwise go to CV and measure OCV of both cells
          next_state = 3;
          rest_timer = 0;        }
        if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }
      
      case 5:{ // Rest state, no current
        cc = 1;
        current_ref = 0;
        voltage_ref = 0;
        soc = soc + current_measure/6269868 * 100;
        next_state = 5;
        if(Serial.read() == 'r'){ //UNLESS the switch = 0, then go back to start
          next_state = 0;
        }
        break;
      }
      case 6:{ // Cell Balancing Stage
        cc = 1;
        current_ref = 0;
        if ((V_mean1 > V_mean2) > 200) { //Discharge Cell 1
          next_state = 7;
          rest_timer = 0;
        }
        else if ((V_mean1 > V_mean3) > 200) { // otherwise Discharge Cell 1
          next_state = 8;
          rest_timer = 0;
        }
        else if ((V_mean2 > V_mean1) > 200) { // otherwise Discharge Cell 2
          next_state = 9;
          rest_timer = 0;
        }
        else if ((V_mean2 > V_mean3) > 200) { // otherwise Discharge Cell 2
          next_state = 10;
          rest_timer = 0;
        }
        else if ((V_mean3 > V_mean1) > 200) { // otherwise Discharge Cell 3
          next_state = 11;
          rest_timer = 0;
        }
        else if ((V_mean3 > V_mean2) > 200) { // otherwise Discharge Cell 3
          next_state = 12;
          rest_timer = 0;
        }
        else{
          next_state = 1;
          rest_timer = 0;
        }
        if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
          digitalWrite(8,false);
        }
        break;   
      }

      case 7:{ // Discharge Cell 1;
        cc = 1;
        current_ref = 0;
       if ((V_mean1 - V_mean2) < 200){
        next_state = 6;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 7;       
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(9,true);
            digitalWrite(5,true);
            V_meas1 = analogRead(A1)*4.096/1.05;
            V_total1 += V_meas1;
            rest_timer++;
            next_state = 7; 
          }
          
          else{ 
            digitalWrite(9,false);
            digitalWrite(5,false);
            V_mean1 = V_total1 / 10;
            V_meas1 = 0, V_total1 = 0;
            next_state = 7;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 8:{ // Discharge Cell 1;
        cc = 1;
        current_ref = 0;
       if ((V_mean1 - V_mean3) < 200){
        next_state = 6;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 8;       
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(9,true);
            digitalWrite(5,true);
            V_meas1 = analogRead(A1)*4.096/1.05;
            V_total1 += V_meas1;
            rest_timer++;
            next_state = 8; 
          }
          
          else{ 
            digitalWrite(9,false);
            digitalWrite(5,false);
            V_mean1 = V_total1 / 10;
            V_meas1 = 0, V_total1 = 0;
            next_state = 8;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 9:{ // Discharge Cell 2;
        cc = 1;
        current_ref = 0;
       if ((V_mean2 - V_mean1) < 200){
        next_state = 6;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 9;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(3,true);
            digitalWrite(4,true);
            V_meas2 = analogRead(A2)*4.096/1.06;
            V_total2 += V_meas2;
            rest_timer++;
            next_state = 9; 
          }
          else{ 
            digitalWrite(3,false);
            digitalWrite(4,false);
            V_mean2 = V_total2 / 10;
            V_meas2 = 0, V_total2 = 0;
            next_state = 9;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 10:{ // Discharge Cell 2;
        cc = 1;
        current_ref = 0;
       if ((V_mean2 - V_mean3) < 200){
        next_state = 6;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 10;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(3,true);
            digitalWrite(4,true);
            V_meas2 = analogRead(A2)*4.096/1.06;
            V_total2 += V_meas2;
            rest_timer++;
            next_state = 10; 
          }
          else{ 
            digitalWrite(3,false);
            digitalWrite(4,false);
            V_mean2 = V_total2 / 10;
            V_meas2 = 0, V_total2 = 0;
            next_state = 10;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 11:{ // Discharge Cell 3;
        cc = 1;
        current_ref = 0;
       if ((V_mean3 - V_mean1) < 200){
        next_state = 6;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 11;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(8,true);
            digitalWrite(7,true);
            V_meas3 = analogRead(A7)*4.096/1.06;
            V_total3 += V_meas3;
            rest_timer++;
            next_state = 11; 
          }
          else{ 
            digitalWrite(7,false);
            digitalWrite(8,false);
            V_mean3 = V_total3 / 10;
            V_meas3 = 0, V_total3 = 0;
            next_state = 11;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 12:{ // Discharge Cell 3;
        cc = 1;
        current_ref = 0;
       if ((V_mean3 - V_mean2) < 200){
        next_state = 6;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 12;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(8,true);
            digitalWrite(7,true);
            V_meas3 = analogRead(A7)*4.096/1.06;
            V_total3 += V_meas3;
            rest_timer++;
            next_state = 12; 
          }
          else{ 
            digitalWrite(7,false);
            digitalWrite(8,false);
            V_mean3 = V_total3 / 10;
            V_meas3 = 0, V_total3 = 0;
            next_state = 12;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }
      
      case 13: { // ERROR state RED led and no current
        cc = 1;
        current_ref = 0;
        next_state = 6; // Always stay here
        soc = 0;
        digitalWrite(7,false), digitalWrite(8,false), digitalWrite(5,false), digitalWrite(4,false), digitalWrite(9,false), digitalWrite(3,false);
        if(Serial.read() == 'r'){ //UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(7,false);
        }
        break;
      }

      case 14:{ // CC Discharge state until 2.5V with all cells measurement
        cc = 1;
        current_ref = -500;
        digitalWrite(9,false), digitalWrite(3,false), digitalWrite(7,false);//turn off all discharge
        V_sum = V_mean1 + V_mean2 + V_mean3;
        V_avg = V_sum/3;
        if (V_avg > 2500) { // Continue CC
          soc = soc + current_measure/6269868 * 100; 
          if (rest_timer < 60){
            rest_timer++;  
            next_state = 14;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(5,true);
            V_meas1 = analogRead(A1)*4.096/1.05;
            V_total1 += V_meas1;
            rest_timer++;
            next_state = 14; 
          }
          else if(rest_timer >= 70 && rest_timer < 80){
            digitalWrite(5,false);
            digitalWrite(4,true);
            V_meas2 = analogRead(A2)*4.096/1.06;
            V_total2 += V_meas2;
            rest_timer++;
            next_state = 14;
          }
          else if(rest_timer >= 80 && rest_timer < 90){
            digitalWrite(4,false);
            digitalWrite(8,true);
            V_meas3 = analogRead(A7)*4.096/1.06;
            V_total3 += V_meas3;
            rest_timer++;
            next_state = 14;
          }
          else if (rest_timer < 91){ // Calculate the average voltage of both cells and back to state 1 to decide whether to go to CV stage
            digitalWrite(8,false),digitalWrite(5,false),digitalWrite(4,false);
            V_mean1 = V_total1/10;
            V_mean2 = V_total2/10;
            V_mean3 = V_total3/10;
            next_state = 14;
            rest_timer++;
            V_meas1 = 0, V_total1 = 0, V_meas2 = 0, V_total2 = 0, V_meas3 = 0, V_total3 = 0;
          }
          else{ //Head to comparison state
            rest_timer = 0;
            next_state = 15;
          }
        } 
      else{ // otherwise go to Rest State
          rest_timer = 0;
          V_mean1 = 0, V_mean2 = 0, V_mean3 = 0;
          next_state = 5;
        }
        if(Serial1.read() == 'r'){ // UNLESS receive 'r' from control, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;
      }

      case 15:{ // CC Comparison State
        current_ref = 0;
        cc = 1;
        if (abs(V_mean1-V_mean2) > 200 || abs(V_mean1-V_mean3) > 200 || abs(V_mean3-V_mean2) > 200 ) { //Go to balance state
          next_state = 16;
        }
        else { // otherwise go to CC and measure OCV of both cells
          next_state = 14;
          rest_timer = 0;
        }
        if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 16:{ // Cell Discharging Balancing Stage
        cc = 1;
        current_ref = 0;
        if ((V_mean1 > V_mean2) > 200) { //Discharge Cell 1
          next_state = 17;
          rest_timer = 0;
        }
        else if ((V_mean1 > V_mean3) > 200) { // otherwise Discharge Cell 1
          next_state = 18;
          rest_timer = 0;
        }
        else if ((V_mean2 > V_mean1) > 200) { // otherwise Discharge Cell 2
          next_state = 19;
          rest_timer = 0;
        }
        else if ((V_mean2 > V_mean3) > 200) { // otherwise Discharge Cell 2
          next_state = 20;
          rest_timer = 0;
        }
        else if ((V_mean3 > V_mean1) > 200) { // otherwise Discharge Cell 3
          next_state = 21;
          rest_timer = 0;
        }
        else if ((V_mean3 > V_mean2) > 200) { // otherwise Discharge Cell 3
          next_state = 22;
          rest_timer = 0;
        }
        else{
          next_state = 14;
          rest_timer = 0;
        }
        if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
          digitalWrite(8,false);
        }
        break;   
      }

      case 17:{ // Discharge Cell 1;
        cc = 1;
        current_ref = 0;
       if ((V_mean1 - V_mean2) < 200){
        next_state = 16;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 17;       
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(9,true);
            digitalWrite(5,true);
            V_meas1 = analogRead(A1)*4.096/1.05;
            V_total1 += V_meas1;
            rest_timer++;
            next_state = 17; 
          }
          
          else{ 
            digitalWrite(9,false);
            digitalWrite(5,false);
            V_mean1 = V_total1 / 10;
            V_meas1 = 0, V_total1 = 0;
            next_state = 17;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 18:{ // Discharge Cell 1;
        cc = 1;
        current_ref = 0;
       if ((V_mean1 - V_mean3) < 200){
        next_state = 16;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 18;       
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(9,true);
            digitalWrite(5,true);
            V_meas1 = analogRead(A1)*4.096/1.05;
            V_total1 += V_meas1;
            rest_timer++;
            next_state = 18; 
          }
          
          else{ 
            digitalWrite(9,false);
            digitalWrite(5,false);
            V_mean1 = V_total1 / 10;
            V_meas1 = 0, V_total1 = 0;
            next_state = 18;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 19:{ // Discharge Cell 2;
        cc = 1;
        current_ref = 0;
       if ((V_mean2 - V_mean1) < 200){
        next_state = 16;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 19;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(3,true);
            digitalWrite(4,true);
            V_meas2 = analogRead(A2)*4.096/1.06;
            V_total2 += V_meas2;
            rest_timer++;
            next_state = 19; 
          }
          else{ 
            digitalWrite(3,false);
            digitalWrite(4,false);
            V_mean2 = V_total2 / 10;
            V_meas2 = 0, V_total2 = 0;
            next_state = 19;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 20:{ // Discharge Cell 2;
        cc = 1;
        current_ref = 0;
       if ((V_mean2 - V_mean3) < 200){
        next_state = 16;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 20;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(3,true);
            digitalWrite(4,true);
            V_meas2 = analogRead(A2)*4.096/1.06;
            V_total2 += V_meas2;
            rest_timer++;
            next_state = 20; 
          }
          else{ 
            digitalWrite(3,false);
            digitalWrite(4,false);
            V_mean2 = V_total2 / 20;
            V_meas2 = 0, V_total2 = 0;
            next_state = 20;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 21:{ // Discharge Cell 3;
        cc = 1;
        current_ref = 0;
       if ((V_mean3 - V_mean1) < 200){
        next_state = 6;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 21;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(8,true);
            digitalWrite(7,true);
            V_meas3 = analogRead(A7)*4.096/1.06;
            V_total3 += V_meas3;
            rest_timer++;
            next_state = 21; 
          }
          else{ 
            digitalWrite(7,false);
            digitalWrite(8,false);
            V_mean3 = V_total3 / 10;
            V_meas3 = 0, V_total3 = 0;
            next_state = 21;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      case 22:{ // Discharge Cell 3;
        cc = 1;
        current_ref = 0;
       if ((V_mean3 - V_mean2) < 200){
        next_state = 16;
       }
       else{
        if (rest_timer < 60){
            rest_timer++;  
            next_state = 22;        
          }
          else if (rest_timer < 70 && rest_timer >= 60){
            digitalWrite(8,true);
            digitalWrite(7,true);
            V_meas3 = analogRead(A7)*4.096/1.06;
            V_total3 += V_meas3;
            rest_timer++;
            next_state = 22; 
          }
          else{ 
            digitalWrite(7,false);
            digitalWrite(8,false);
            V_mean3 = V_total3 / 10;
            V_meas3 = 0, V_total3 = 0;
            next_state = 22;
            rest_timer = 0;
          }
       }
       if(Serial.read() == 'r'){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          rest_timer = 0;
        }
        break;   
      }

      default :{ // Should not end up here ....
        Serial.println("Boop");
        cc = 1;
        current_ref = 0;
        next_state = 13; // So if we are here, we go to error
      }
      
    }
    
    dataString = String(state_num) + "," + String(cc) + "," + String(rest_timer) + "," + String(V_Bat) + "," + String(V_meas1) + "," + String(V_meas2) + "," + String(V_meas3) + "," + String(V_mean1) + "," + String(V_mean2) + "," + String(V_mean3) + "," + String(V_avg) + "," + String(current_measure) + "," + String(soc); //build a datastring for the CSV file
    
    controlString = String(soc) + "," + String(current_measure);
    Serial1.println(controlString);
    
    Serial.println(dataString); // send it to serial as well in case a computer is connected
    File dataFile = SD.open("BatCycle.csv", FILE_WRITE); // open our CSV file
    if (dataFile){ //If we succeeded (usually this fails if the SD card is out)
      dataFile.println(dataString); // print the data
    } else {
      Serial.println("File not open"); //otherwise print an error
    }
    dataFile.close(); // close the file
    int_count = 0; // reset the interrupt count so we dont come back here for 1000ms
  }
}

// Timer A CMP1 interrupt. Every 1000us the program enters this interrupt. This is the fast 1kHz loop
ISR(TCA0_CMP1_vect) {
  loop_trigger = 1; //trigger the loop when we are back in normal flow
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm; //clear interrupt flag
}

float saturation( float sat_input, float uplim, float lowlim) { // Saturation function
  if (sat_input > uplim) sat_input = uplim;
  else if (sat_input < lowlim ) sat_input = lowlim;
  else;
  return sat_input;
}

float pidi(float pid_input) { // discrete PID function
  float e_integration;
  e0i = pid_input;
  e_integration = e0i;
  //anti-windup
  if (u1i >= ui_max) {
    e_integration = 0;
  } else if (u1i <= ui_min) {
    e_integration = 0;
  }

  delta_ui = kpi * (e0i - e1i) + kii * Ts * e_integration + kdi / Ts * (e0i - 2 * e1i + e2i); //incremental PID programming avoids integrations.
  u0i = u1i + delta_ui;  //this time's control output

  //output limitation
  saturation(u0i, ui_max, ui_min);

  u1i = u0i; //update last time's control output
  e2i = e1i; //update last last time's error
  e1i = e0i; // update last time's error
  return u0i;
}


float pidv( float pid_input){
  float e_integration;
  e0v = pid_input;
  e_integration = e0v;
 
  //anti-windup, if last-time pid output reaches the limitation, this time there won't be any intergrations.
  if(u1v >= uv_max) {
    e_integration = 0;
  } else if (u1v <= uv_min) {
    e_integration = 0;
  }

  delta_uv = kpv*(e0v-e1v) + kiv*Ts*e_integration + kdv/Ts*(e0v-2*e1v+e2v); //incremental PID programming avoids integrations.there is another PID program called positional PID.
  u0v = u1v + delta_uv;  //this time's control output

  //output limitation
  saturation(u0v,uv_max,uv_min);
  
  u1v = u0v; //update last time's control output
  e2v = e1v; //update last last time's error
  e1v = e0v; // update last time's error
  return u0v;
}
