ISR(TCA0_CMP1_vect){
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm; //clear interrupt flag
  loopTrigger = 1;
}

// This subroutine processes all of the analogue samples, creating the required values for the main loop

void sampling(){

  // Make the initial sampling operations for the circuit measurements
  
  sensorValue0 = analogRead(A0); //sample Vb
  sensorValue2 = analogRead(A2); //sample Vref
  sensorValue3 = analogRead(A3); //sample Vpd
  current_mA = ina219.getCurrent_mA(); // sample the inductor current (via the sensor chip)

  // Process the values so they are a bit more usable/readable
  // The analogRead process gives a value between 0 and 1023 
  // representing a voltage between 0 and the analogue reference which is 4.096V


  
  vb = sensorValue0 * (4.096 / 1023.0); // Convert the Vb sensor reading to volts
  vref = sensorValue2 * (4.096 / 1023.0) / speed_factor; // Convert the Vref sensor reading to volts 
  vpd = sensorValue3 * (4.096 / 1023.0); // Convert the Vpd sensor reading to volts

  // The inductor current is in mA from the sensor so we need to convert to amps.
  // We want to treat it as an input current in the Boost, so its also inverted
  // For open loop control the duty cycle reference is calculated from the sensor
  // differently from the Vref, this time scaled between zero and 1.
  // The boost duty cycle needs to be saturated with a 0.33 minimum to prevent high output voltages
  
  if (Boost_mode == 1){
    iL = -current_mA/1000.0;
    dutyref = saturation(sensorValue2 * (1.0 / 1023.0),0.99,0.33);
  }else{
    iL = current_mA/1000.0;
    dutyref = sensorValue2 * (1.0 / 1023.0);
  }
  
}

float saturation( float sat_input, float uplim, float lowlim){ // Saturatio function
  if (sat_input > uplim) sat_input=uplim;
  else if (sat_input < lowlim ) sat_input=lowlim;
  else;
  return sat_input;
}

void pwm_modulate(float pwm_input){ // PWM function
  analogWrite(6,(int)(255-pwm_input*255)); 
}

// This is a PID controller for the voltage

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

// This is a PID controller for the current

float pidi(float pid_input){
  float e_integration;
  e0i = pid_input;
  e_integration=e0i;
  
  //anti-windup
  if(u1i >= ui_max){
    e_integration = 0;
  } else if (u1i <= ui_min) {
    e_integration = 0;
  }
  
  delta_ui = kpi*(e0i-e1i) + kii*Ts*e_integration + kdi/Ts*(e0i-2*e1i+e2i); //incremental PID programming avoids integrations.
  u0i = u1i + delta_ui;  //this time's control output

  //output limitation
  saturation(u0i,ui_max,ui_min);
  
  u1i = u0i; //update last time's control output
  e2i = e1i; //update last last time's error
  e1i = e0i; // update last time's error
  return u0i;
}

// This is a PI controller for position control
float pi_p(float pi_p_input){
  float e_integration;
  e0p=pi_p_input;
  e_integration=e0p;
  delta_up=kpp*(e0p-e1p)+kip*Ts*e_integration;
  u0p=u1p+delta_up;
  u1p = u0p; //update last time's control output
  e2p = e1p; //update last last time's error
  e1p = e0p; // update last time's error
  return u0p;
}

// Speed control
void Speed_Control(float cp){
analogWrite(6,(int)(255-cp*255));
}

void move_forward(){
  digitalWrite(pwmr, HIGH);
  digitalWrite(pwml, HIGH);
  DIRRstate = HIGH;
  DIRLstate = LOW;
  digitalWrite(DIRR, DIRRstate);
  digitalWrite(DIRL, DIRLstate); 
}

void move_backward(){
  digitalWrite(pwmr, HIGH);
  digitalWrite(pwml, HIGH);
  DIRRstate = LOW;
  DIRLstate = HIGH;
  digitalWrite(DIRR, DIRRstate);
  digitalWrite(DIRL, DIRLstate); 
}
void rover_stop(){
  digitalWrite(pwmr, LOW);
  digitalWrite(pwml, LOW);
}

void left_correction(){
  digitalWrite(pwmr, HIGH);
  digitalWrite(pwml, HIGH);
  DIRRstate =LOW ;
  DIRLstate =LOW;
  digitalWrite(DIRR, DIRRstate);
  digitalWrite(DIRL, DIRLstate); 
}
void right_correction(){
  digitalWrite(pwmr, HIGH);
  digitalWrite(pwml, HIGH);
  DIRRstate = HIGH;
  DIRLstate = HIGH;
  digitalWrite(DIRR, DIRRstate);
  digitalWrite(DIRL, DIRLstate); 
}

void left_turn(){
  digitalWrite(pwmr, HIGH);
  digitalWrite(pwml, HIGH);
  DIRRstate =LOW ;
  DIRLstate =LOW;
  digitalWrite(DIRR, DIRRstate);
  digitalWrite(DIRL, DIRLstate); 
}

void right_turn(){
  digitalWrite(pwmr, HIGH);
  digitalWrite(pwml, HIGH);
  DIRRstate = HIGH;
  DIRLstate = HIGH;
  digitalWrite(DIRR, DIRRstate);
  digitalWrite(DIRL, DIRLstate); 
}

bool turning_left(float degree, long current_x, int speed_factor)
{
   float final_dis=2.6*acos(-1)*14.4*degree/360; //2
   float err_l=current_x-final_dis;
   float cp=pi_p(err_l);
   Serial.println("err_l="+String(err_l));
   Speed_Control(abs(cp)*0.0025 * speed_factor + 0.4);
  if(err_l>0.5){
      right_turn();
      return false;
    }
    else if(err_l<-0.5){
      left_turn();
      return false;
    }
    else{
      if(firstTime==true){
        instructionCompleteTime=millis();
        firstTime=false;
        return false;
      }
      if(firstTime==false && millis()- instructionCompleteTime>5000)
      {
       rover_stop();
       return true;
      }
    }
}
 
bool turning_right(float degree, long current_x, int speed_factor)
{
   float final_dis=2.8*acos(-1)*14.4*degree/360; //2
   float err_r=-current_x-final_dis;
   float cp=pi_p(err_r);
   Speed_Control(abs(cp)*0.0025 * speed_factor+0.4);
   Serial.println("err_r="+String(err_r));
  if(err_r>0.5){
      left_turn();
      return false;
    }
    else if(err_r<-0.5){
      right_turn();
      return false;
    }
    else{
      if(firstTime==true){
        instructionCompleteTime=millis();
        firstTime=false;
        return false;
      }
      if(firstTime==false && millis()- instructionCompleteTime>5000)
      {
       rover_stop();
       return true;
      }
    }
}

bool moving_forward(float distance, long current_y, int speed_factor){
   float err_f=current_y-distance;
   float cp=pi_p(err_f);
   Speed_Control(abs(cp)*0.0025 * speed_factor+0.4);
  if(err_f>0.5){
      move_backward();
      return false;
    }
    else if(err_f<-0.5){
      move_forward();
      return false;
    }
    else{
      if(firstTime == true){
        instructionCompleteTime=millis();
        firstTime=false;
        return false;
      }
      if(firstTime==false && millis()- instructionCompleteTime>5000)
      {
       rover_stop();
       return true;
      }
    }
}

bool moving_backward(float distance, long current_y, int speed_factor){
   float err_f=-current_y-distance;
   float cp=pi_p(err_f);
   Speed_Control(abs(cp)*0.0025 * speed_factor+0.4);
  if(err_f>0.5){
      move_forward();
      return false;
    }
    else if(err_f<-0.5){
      move_backward();
      return false;
    }
    else{
      if(firstTime == true){
        instructionCompleteTime=millis();
        firstTime=false;
        return false;
      }
      if(firstTime==false && millis()- instructionCompleteTime>5000)
      {
       rover_stop();
       return true;
      }
    }
}



void rover_cruising(boolean lcorrection, boolean rcorrection){
  if(total_y < max_y)
  {

    if(total_x < - 3)
    {
      left_correction();
      lcorrection = true;
      rcorrection = false;
    }
    else if(total_x > 3)
    {
     right_correction();
     rcorrection = true;
     lcorrection = false;
    }
    else
    {
        if(lcorrection == false and rcorrection == false)
        {
          //moving_forward();
        }
        else if(lcorrection == true)
        {
          left_correction();
          lcorrection = total_x > -1.5 ? false : true;   
        }
        else if(rcorrection ==  true)
        {
          right_correction();
          rcorrection =  total_x < 1.5 ? false : true; 
        }
    }
}else
{
  if (rcorrection ==  true and lcorrection == true){
    rover_stop();
  }else{
  right_turn();
  rcorrection = true;
  lcorrection = true;
  }
}
}  
