vector<String> route_design(int max_x, int max_y, int speed_factor){
    boolean right = true;
    int current_x = 0; 
    int current_y = 0;
    vector<String> InstructionVec;
    InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
    InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
    InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
    InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
    while (current_x < max_x){
      current_x = current_x + 30;
      if(right){
        InstructionVec.push_back(String(speed_factor) +"3090");
        InstructionVec.push_back(String(speed_factor) +"1030");
        InstructionVec.push_back(String(speed_factor) +"3090");
        InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
        InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
        InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
        InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
        right = false;
      }else{
        InstructionVec.push_back(String(speed_factor) +"5090");
        InstructionVec.push_back(String(speed_factor) +"1030");
        InstructionVec.push_back(String(speed_factor) +"5090"); 
        InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
        InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
        InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
        InstructionVec.push_back(String(String(speed_factor) + "1" + String(max_y/4)));
        right = true; 
      }
    }
    InstructionVec.push_back("4000");
    return InstructionVec;
}


//the following function needed to be refined
vector<String> obstacle_avoidance(){
    vector<String> InstructionVec;
    InstructionVec.push_back("11020");
    InstructionVec.push_back("13090");
    InstructionVec.push_back("11020");
    InstructionVec.push_back("15090");
    InstructionVec.push_back("11050");
    InstructionVec.push_back("15090");
    InstructionVec.push_back("11020");
    InstructionVec.push_back("13090");
    return InstructionVec;
}
