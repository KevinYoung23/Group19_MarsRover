#include<WiFi.h>
#include<vector>

using namespace std;

#define RXD 16 // define RX connected to Drive as RXD pin 8
#define TXD 17 // define TX connected to Drive as TXD pin 9

#define RXV 22 // define RX connected to Vision as RXV pin 17
#define TXV 23 // define TX connected to Vision as TXV pin 18

vector<String> Command;
vector<String> Temp;
int current_instruct = 0;
int temp_current_instruct = 0;
int prev_instruct = -1;

const char* ssid = "HUAWEI Mate 40 Pro+";        //WiFi Name
const char* password = "qwertyui";               //WiFi Password

char* host = "192.168.43.149";             //URL or IP address
long port = 65427;                         //Port to be defined

WiFiClient client;

int Mode = 0;                   //2 for manual mode, and 1 for auto mode
boolean Mode_selected  = false; //0 for not selected, 1 for selected 
int speed_factor = 1;

String Command_received = "10000000";

int Instruction = 0;

int max_x = 0;
int max_y = 0;

String coor_x = "";
String coor_y = "";

String Command_data = "";
String Drive_data = "";
String Vision_data[44];

long State = 0;

int obstacle_flag = 0;
int direction_flag = 1;

int threshold_x = 400;
int threshold_y = 50;

int red_min_x, red_min_y, red_max_x, red_max_y = 0;
int green_min_x, green_min_y, green_max_x, green_max_y = 0;
int blue_min_x, blue_min_y, blue_max_x, blue_max_y = 0;
int orange_min_x, orange_min_y, orange_max_x, orange_max_y = 0;
int grey_min_x, grey_min_y, grey_max_x, grey_max_y = 0;

vector<String> route_design(int max_x, int max_y, int speed_factor);
vector<String> obstacle_avoidance();

void setup() {
  Serial.begin(115200); //communicate with PC via usb
  Serial1.begin(115200, SERIAL_8N1, RXV, TXV);  //communicate with Vision
  Serial2.begin(9600, SERIAL_8N1, RXD, TXD);  //communicate with Drive via RX0, and TX1
  
  
  WiFi.begin(ssid, password);                 //connect to WiFi
  while (WiFi.status() != WL_CONNECTED){
     delay(500);
     Serial.print(".");  
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  delay(500);
  if(client.connect(host, port)){       //connect(ip/url, port)
    Serial.println("Handshake successful");
  }else{
    Serial.println("Handshake Failed"); 
  }
}

void loop() {
  if(!Mode_selected){
    if(client.connected()){
      Command_data = String(client.readStringUntil('\n'));
      
      Serial.println("Command Data: " + Command_data);
      if(Command_data.length() == 9){
        Serial.println("Command is valid");
        client.print(Command_received);
        Mode = String(String(Command_data[0]) + String(Command_data[1])).toInt();
        if(Mode == 11){
          max_x = String(String(Command_data[2]) + String(Command_data[3]) + String(Command_data[4])).toInt();
          max_y = String(String(Command_data[5]) + String(Command_data[6]) + String(Command_data[7])).toInt();
          speed_factor = String(Command_data[8]).toInt();
          Mode_selected = true;
          Command = route_design(max_x, max_y, speed_factor);
          //client.print(Command_received);
          Serial.println("Command Received!");
        }else if(Mode == 10){
          Instruction = String(Command_data[2]).toInt();
          Mode_selected = true;
          //client.print(Command_received);
          Serial.println("Command Received!");
          speed_factor = String(Command_data[8]).toInt();
          switch(Instruction){
          case(5):{
            Command.push_back(String(speed_factor) + "5090");
            Command.push_back(String(speed_factor) + "4000");
            break;
          }
          case(1):{
            Command.push_back(String(speed_factor) + "1021");
            Command.push_back(String(speed_factor) + "4000");
            break;  
          }
          case(2):{
            Command.push_back(String(speed_factor) + "2021");
            Command.push_back(String(speed_factor) + "4000");  
            break;
          }
          case(3):{
            Command.push_back(String(speed_factor) + "3090");
            Command.push_back(String(speed_factor) + "4000");
            break;  
          }
          case(4):{
            Command.push_back(String(speed_factor) + "4000"); 
            break; 
          }
        }
        }else{
          Serial.println("Invalid Mode");
        }
      }else{
        Serial.println("Invalid Command!");  
      } 
    } 
  }else{
    if(Serial2.available()){
      String Drive_data = String(Serial2.readStringUntil('\n'));
      Serial.println("Drive_data before:" + String(Drive_data)); 
      if(Drive_data.length() == 8){
        Serial.println("Drive_data:" + String(Drive_data)); 
        State = String(String(Drive_data[0]) + String(Drive_data[1])).toInt();
        switch(State){
          case(11):{
              Serial.println("Prev Instruction" + String(prev_instruct));
              Serial.println("Current Instruction" + String(current_instruct));
              if(prev_instruct != current_instruct){
                prev_instruct = current_instruct;
                if(obstacle_flag == 0){
                  Serial2.print(String(Command[current_instruct]));
                  Serial2.print('\n');
                  Serial.println("Current Instruction:" + String(Command[current_instruct]));
                }else{
                  Serial2.print(String(Temp[temp_current_instruct]));
                  Serial2.print('\n');
                }
                if (Command[current_instruct][1] == '1' or Command[current_instruct][1] == '4' or Command[current_instruct][1] == '2'){
                  Serial.println("Direction Flag does not change");
                  direction_flag = direction_flag;  
                }else if(Command[current_instruct][1] == '5'){
                  Serial.println("Direction Flag does small");
                  direction_flag = direction_flag == 0 ? 3 : direction_flag - 1;
                }else if(Command[current_instruct][1] == '3'){
                  Serial.println("Direction Flag does big");
                  direction_flag = direction_flag == 3 ? 0 : direction_flag + 1;
                }else{
                  Serial.println("Command is not valid");  
                }
              }
              break;}
          
          case(33):{
              current_instruct = current_instruct < Command.size() - 1 ? current_instruct + 1 : current_instruct;
              temp_current_instruct = obstacle_flag != 0 ? temp_current_instruct + 1 : temp_current_instruct;
              coor_x = String(String(Drive_data[2])+String(Drive_data[3])+String(Drive_data[4]));
              coor_y = String(String(Drive_data[5])+String(Drive_data[6])+String(Drive_data[7]));
              client.print(String(Mode) + String(coor_x) + String(coor_y) + String(direction_flag) + String(obstacle_flag));
              Serial.println("Feedback Message:" + String(Mode) + String(coor_x) + String(coor_y) + String(direction_flag) + String(obstacle_flag));
              obstacle_flag = 0;
          break;}

          case(22):{
              Command.clear();
              Command.resize(0); 
              Mode_selected = false;
              current_instruct = 0;
              prev_instruct = -1;
              obstacle_flag = 0;
          break;}
        }
      }else{
        Serial.println("Drive feedback message invalid");  
      }
    }else{
    }
  }
  
  //vision obstacle avoidance
  if(Serial1.available()){
    for(int i = 0; i < 44; i++){
        Vision_data[i] = String(Serial1.read());
        red_max_x = Vision_data[7].toInt() * 256 + Vision_data[6].toInt();
        red_max_y = Vision_data[5].toInt() * 256 + Vision_data[4].toInt();
        red_min_x = Vision_data[11].toInt() * 256 + Vision_data[10].toInt();
        red_min_y = Vision_data[9].toInt() * 256 + Vision_data[8].toInt();
        green_max_x = Vision_data[15].toInt() * 256 + Vision_data[14].toInt();
        green_max_y = Vision_data[13].toInt() * 256 + Vision_data[12].toInt();
        green_min_x = Vision_data[19].toInt() * 256 + Vision_data[18].toInt();
        green_min_y = Vision_data[17].toInt() * 256 + Vision_data[16].toInt();
        blue_max_x = Vision_data[23].toInt() * 256 + Vision_data[22].toInt();
        blue_max_y = Vision_data[21].toInt() * 256 + Vision_data[20].toInt();
        blue_min_x = Vision_data[27].toInt() * 256 + Vision_data[26].toInt();
        blue_min_y = Vision_data[25].toInt() * 256 + Vision_data[24].toInt();
        orange_max_x = Vision_data[31].toInt() * 256 + Vision_data[30].toInt();
        orange_max_y = Vision_data[29].toInt() * 256 + Vision_data[28].toInt();
        orange_min_x = Vision_data[35].toInt() * 256 + Vision_data[34].toInt();
        orange_min_y = Vision_data[33].toInt() * 256 + Vision_data[32].toInt();
        grey_max_x = Vision_data[39].toInt() * 256 + Vision_data[38].toInt();
        grey_max_y = Vision_data[37].toInt() * 256 + Vision_data[36].toInt();
        grey_min_x = Vision_data[43].toInt() * 256 + Vision_data[42].toInt();
        grey_min_y = Vision_data[41].toInt() * 256 + Vision_data[40].toInt();
        if(red_max_x < threshold_x and red_max_y < threshold_y){
          obstacle_flag = 1;
          Temp = obstacle_avoidance();
        }else if(green_max_x < threshold_x and green_max_y < threshold_y){
          obstacle_flag = 2;
          Temp = obstacle_avoidance();
        }else if(blue_max_x < threshold_x and blue_max_y < threshold_y){
          obstacle_flag = 4;
          Temp = obstacle_avoidance();
        }else if(orange_max_x < threshold_x and orange_max_y < threshold_y){
          obstacle_flag = 3;
          Temp = obstacle_avoidance();
        }else if(grey_max_x < threshold_x and grey_max_y < threshold_y){
          obstacle_flag = 5;
          Temp = obstacle_avoidance();
        }
    }
  }
}
