#include <IRremote.h>
// microphone setting
// Connect the MEMS AUD output to the Arduino A0 pin
int mic = A0;
// Variables to find the peak-to-peak amplitude of AUD output
const int sampleTime = 50;
int micOut;
int volume;
int volume_sum;
int volume_avg;
int nonzero_count;
int top_limit;
int bot_limit;

int auto_flag = 1; // 1 means automode, 0 means manual mode
int RECV_PIN = 11; // pin receive the infared signal
IRrecv irrecv(RECV_PIN);
IRsend irsend;
decode_results results;
int brand_num;
char* tvBrand;
int setup_flag;
String command = "volUp";
String bluetooth_command = "";
// all the supported brands, but only SONY and SAMSUNG are used below
char* brand[]={"UNUSED", "RC5", "RC6", "NEC", "SONY", "PANASONIC", "JVC", "SAMSUNG", 
            "WHYNTER", "AIWA_RC_T501", "LG", "SANYO", "MITSUBISHI", "DISH", "SHARP", "DENON", "PRONTO", "LEGO_PF"};
          
void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  
  setup_flag = 0;
  auto_flag = 1; // default to be auto
  top_limit = 12; // default limits, can be revised due to differenet situations
  bot_limit = 2;
}

void loop() {
  // one time setup for the tv_brand
  // device will recognize the tv_brand based on 
  // the signal received from the remote controller
  if (irrecv.decode(&results) && setup_flag == 0) {
    brand_num = int(dump(&results));
    Serial.println(brand[brand_num]);
    tvBrand = brand[brand_num];
    irrecv.resume(); // Receive the next value
    setup_flag = 1;
    delay(100);
  }
  delay(100);

  // receive the bluetooth command by serial communication
  while(Serial1.available() > 0){
     bluetooth_command += char(Serial1.read());
     delay(2);
  }
  
  //manipulate with the bluetooth command
  if(bluetooth_command.length() > 0){
    Serial.println("bluetooth_command is:");
    Serial.println(bluetooth_command);
  }
  if(bluetooth_command == "auto"){
    Serial.println("Set mode: auto");
     auto_flag = 1;
  }else if(bluetooth_command == "manual"){
    Serial.println("Set mode: manual");
    auto_flag = 0;
  }else if(bluetooth_command == "toplimit_up"){
    top_limit = top_limit + 1;
    Serial.println("top_limit up to: ");
    Serial.println(top_limit);
  }else if(bluetooth_command == "toplimit_down"){
     top_limit = top_limit - 1;
     Serial.println("top_limit down to: ");
     Serial.println(top_limit);
  }else if(bluetooth_command == "botlimit_up"){
    bot_limit = bot_limit + 1;
    Serial.println("bot_limit up to: ");
    Serial.println(bot_limit);
  }else if (bluetooth_command == "botlimit_down"){
    bot_limit = bot_limit - 1;
    Serial.println("bot_limit down to: ");
    Serial.println(bot_limit);
  }
  // manual mode, send out the infared signal based on the command
  // received from the bluetooth
  if(auto_flag == 0){
    if(tvBrand == "SAMSUNG"){   
      //Samsung specific hex code
      //E0E0E01F -> vol up
      //E0E0D02F -> vol down
      if(bluetooth_command == "volUP"){
        Serial.println("Manual, Sending SAMSUNG, volume up");
        irsend.sendSAMSUNG(0xE0E0E01F, 32);
      }else if(bluetooth_command == "volDOWN"){
        Serial.println("Manual, Sending SAMSUNG, volume down");
        irsend.sendSAMSUNG(0xE0E0D02F, 32);
      }
    }else if(tvBrand == "SONY"){
      if(bluetooth_command == "volUP"){
        Serial.println("Sending SONY, volume up");
        irsend.sendSony(0x490, 12);
      }else if(bluetooth_command == "volDOWN"){
        Serial.println("Sending SONY, volume down");
        irsend.sendSony(0xC90, 12);
      }
    }
   }else{ 
      // automode, get the volume from the microphone
      // adjust the volume automatically based on the preset threshold
      int micOutput = findPTPAmp();
      volume_sum = 0;
      nonzero_count = 0;
      for(int i = 0; i < 3; i++){
        volume = VUMeter(micOutput);
        delay(100);
        if(volume != 0){
          nonzero_count++;
        }
        volume_sum =+ volume;
      }
      volume_avg = volume / nonzero_count; 
      if(volume_avg > top_limit){ //send the volume down signal
        if(tvBrand == "SAMSUNG"){
          Serial.println("Sending SAMSUNG, volume down");
          irsend.sendSAMSUNG(0xE0E0D02F, 32);
        }else if (tvBrand == "SONY"){
          Serial.println("Sending SONY, volume down");
          irsend.sendSony(0xC90, 12);
        }
      }else if(volume_avg < bot_limit){
        if(tvBrand == "SAMSUNG"){
          Serial.println("Sending SAMSUNG, volume up");
          irsend.sendSAMSUNG(0xE0E0D01F, 32);
        }else if (tvBrand == "SONY"){
          Serial.println("Sending SONY, volume up");
          irsend.sendSony(0x490, 12);
        }
      }
   }


  //clean the bluetooth command string
  bluetooth_command = "";
  delay(10);
  //send the signal
  
}


unsigned long dump(decode_results *results) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  int count = results->rawlen;
  //Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  return results->decode_type;
}

//functions for microphone
// Find the Peak-to-Peak Amplitude Function
int findPTPAmp(){
// Time variables to find the peak-to-peak amplitude
   unsigned long startTime= millis();  // Start of sample window
   unsigned int PTPAmp = 0; 

// Signal variables to find the peak-to-peak amplitude
   unsigned int maxAmp = 0;
   unsigned int minAmp = 1023;

// Find the max and min of the mic output within the 50 ms timeframe
   while(millis() - startTime < sampleTime) 
   {
      micOut = analogRead(mic);
      if( micOut < 1023) //prevent erroneous readings
      {
        if (micOut > maxAmp)
        {
          maxAmp = micOut; //save only the max reading
        }
        else if (micOut < minAmp)
        {
          minAmp = micOut; //save only the min reading
        }
      }
   }

  PTPAmp = maxAmp - minAmp; // (max amp) - (min amp) = peak-to-peak amplitude
  double micOut_Volts = (PTPAmp * 3.3) / 1023; // Convert ADC into voltage

  //Uncomment this line for help debugging (be sure to also comment out the VUMeter function)
  //Serial.println(PTPAmp); 

  //Return the PTP amplitude to use in the soundLevel function. 
  // You can also return the micOut_Volts if you prefer to use the voltage level.
  return PTPAmp;   
}

// Volume Unit Meter function: map the PTP amplitude to a volume unit between 0 and 10.
int VUMeter(int micAmp){
  int preValue = 0;

  // Map the mic peak-to-peak amplitude to a volume unit between 0 and 10.
   // Amplitude is used instead of voltage to give a larger (and more accurate) range for the map function.
   // This is just one way to do this -- test out different approaches!
  int fill = map(micAmp, 23, 750, 0, 100); 

  // Only print the volume unit value if it changes from previous value
  while(fill != preValue)
  {
    //Serial.println(fill);
    preValue = fill;
  }
  return fill;
}
