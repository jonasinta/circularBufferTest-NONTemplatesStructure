
#include "esp_task_wdt.h"
#include "esp_mac.h" // esp_mac.h is needed for esp_reset_reason()
#include "esp_sleep.h" // esp_sleep.h is needed for esp_deep_sleep_start()
//circularBuffer include

#include "ESP32Time.h"
#include "udpTelem.h"

#include "mainInclude.h"
#define WDT_TIMEOUT 3          // define a 3 seconds WDT (Watch Dog Timer)
#include "Arduino.h"
#include "WiFi.h"
#include "CircularBuffer.h"
#include <FastLED.h> // for the serial rgb led control on GPIO 38
#define LED_PIN     48      // Pin connected to the LED strip
#define NUM_LEDS    1      // Number of LEDs in the strip
#define COLOR_ORDER GRB     // Color order for SK6812

#define switchGPIO 0 // GPIO pin for the switch

CRGB leds[NUM_LEDS];


//buffer globals

DATASEND sendstruct {
    0, // epoch
    0.0f, // Float1
    0.0f, // Float2
    0.0f, // Float3
   
};
char data[6];  // 5 bytes + null terminator for read comand from serial port


// Function declarations



const char* ssid = "fatherJack";
const char* password = "batmanN9";


//SET YOUR PC IP ADDRESS (YOU GOT FROM MODEM)
IPAddress udpip(192,168,1,22);

//SET PORTS TO CONNECT
int printport = 47269;

int chartport = 8081;
int udplocalport = 67777;


ESP32Time rtc;




// set pin numbers
const int touchPin = 13; 
const int ledPin = 5;

// change with your threshold value
const int threshold = 20;

int resetreason;
uint32_t delaysecs;
RTC_DATA_ATTR uint16_t persistant=0;
int looptime_ms;
uint32_t now_ms;
uint32_t last_ms;


bool touchToggle = false;



void myBufferPrintout(DATASEND datas) {
   Serial.print("Epoch: ");
    Serial.print(datas.epoch);
    Serial.print(", Float1: ");
    Serial.print(datas.Float1);
    Serial.print(", Float2: ");
    Serial.print(datas.Float2);
    Serial.print(", Float3: ");
    Serial.print(datas.Float3);
    
  
    Serial.println(" ");
}

RTC_DATA_ATTR bool buttonPressed = false; // Flag for button press

void IRAM_ATTR handleButtonPress() {
    buttonPressed = true; // Set the flag when button is pressed
  
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
			//esp_task_wdt_add(NULL); //add current thread to WDT watch
   esp_sleep_enable_timer_wakeup(sleepTime * 1000000);  // Set up timer as the wake up source and set sleep duration to 5 seconds
   Serial.println("buffer items number; before initialise");

   //FAST LED setup
   FastLED.addLeds<SK6812, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(100); // Set brightness (0-255)
    leds[0] = CRGB::Red;   // Set to red
    FastLED.show();        // Update the LED strip

    pinMode(switchGPIO, INPUT); // Set the button pin as input
     // Attach interrupt to the button pin
     attachInterrupt(digitalPinToInterrupt(switchGPIO), handleButtonPress, ONLOW);
   
  // Initialize the circular buffer
  initCircularBuffer();  // from circularBuffer.h
  
 // Print buffer status
  Serial.print("Buffer initialized. Size: ");
  Serial.println(BUFFER_SIZE);
  
  // Print the number of items in the buffer
  Serial.print("Items in buffer before any operations: ");
  Serial.println(itemCount);

 
  
  // Initialize RTC
  rtc.setTime(0, 0, 0, 1, 1, 2023); // Set to Jan 1, 2023, at midnight
  Serial.print("RTC initialized to: ");
  Serial.println(rtc.getTime());

  
   // Print buffer status after initialization
 Serial.print("Items in buffer: ");
 Serial.println(itemCount);


  
   
      Serial.print("RTC Test get Epoch; ");
       Serial.println(rtc.getEpoch());
  
 

    //circular buffer test routine----------------------------------------------


   resetreason = esp_reset_reason();
   ++persistant;
   Serial.print("persistant =");
   Serial.println(persistant);
    pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, false);
 
    Serial.print("Reset Reason was: "); Serial.println(resetreason);
    
    esp_reset_reason_t reason = esp_reset_reason();
 
Serial.printf("Connecting to %s ", ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("connected");

    Serial.printf("Client IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), udplocalport);
  } else {
    Serial.println("Cannot Connect To WIfi");
  }
   
}

void loop() {
 /*
  // check if the touchValue is below the threshold
  // if it is, set ledPin to HIGH
  if(touchRead(touchPin) < threshold){
    // toggle touchToggle
    touchToggle = !touchToggle;
*/

if (Serial.available()) {
  Serial.println("serialavailable-----------------------------------------------");
  String command = Serial.readStringUntil('\n'); // Read the command
  command.trim(); // Remove any whitespace

  if (command != "") {
      digitalWrite(ledPin, HIGH); // Turn the LED on
      Serial.println("LED is ON");
  } else if (command == "") {
      digitalWrite(ledPin, LOW); // Turn the LED off
      Serial.println("LED is OFF");
  } else {
      Serial.println("Unknown command. Use ON or OFF.");
  }
}
//digitalWrite(ledPin, touchToggle);

if (buttonPressed) {
  Serial.println("Button pressed!");
  FastLED.setBrightness(255); // Set brightness (0-255)
    leds[0] = CRGB::Green;   // Set to red
    FastLED.show();        // Update the LED strip
  delay(500);                 // Optional delay for LED
  FastLED.setBrightness(100); // Set brightness (0-255)
  leds[0] = CRGB::Red;   // Set to red
  FastLED.show();        // Update the LED strip
  
}
   
  
  int numbersend =random(5,30);
   now_ms = millis();
  
looptime_ms = now_ms -last_ms;
  char Str4[] = "arduino";
 if (WiFi.status() == WL_CONNECTED) { 
  sendTelemetry(Str4, looptime_ms);
  sendTelemetry("fuckinFuckFuck", numbersend);
  // add data to the circular buffer
  sendstruct.epoch = rtc.getEpoch();
  sendstruct.Float1 = numbersend;
  sendstruct.Float2 = numbersend + 1.0f;
  sendstruct.Float3 = numbersend * 2.0f;
  if (pushToBuffer(&sendstruct)) {
    Serial.print("Pushed to buffer: ");
    Serial.println(numbersend);
} else {
    Serial.println("Buffer is full, cannot push.");
}
  } // close if (WiFi.status() == WL_CONNECTED) {
 last_ms = now_ms;

/* my orig attempt at iterating the buffer
 for (size_t i = circularBuffer.tail; i < circularBuffer.items; ++i) {
        Serial.println(circularBuffer.buffer[i]);
    }
    Serial.println("-------------------buffer end-------------------");

    // Print the number of items in the buffer
    Serial.print("Number of items in buffer: ");
    Serial.println(circularBuffer.items);
    */

   /* // Print buffer contents
    Serial.println("Buffer contents:");
    iterateBuffer(&myBufferPrintout); // from circularBuffer.h
    Serial.println();

    */




uint16_t tempdata = 0;



    



 // Simulate popping data if buffer has more than 4 items
 if (buttonPressed) {
  Serial.println("------------------popping start------------------");
  // Pop data from the buffer
  for (size_t i = 0;  itemCount != 0; ++i) {
      if (popFromBuffer(&sendstruct)) {
          Serial.print("Popped from buffer: ");
          Serial.println(sendstruct.Float1);
          Serial.print("Epoch: ");
          Serial.println(sendstruct.epoch);
          Serial.print("Float1: ");
          Serial.println(sendstruct.Float1);
          Serial.print("Float2: ");
          Serial.println(sendstruct.Float2);
          Serial.print("Float3: ");
          Serial.println(sendstruct.Float3);
          if (i % 2 == 0) {
            ;
          } else {
            leds[0] = CRGB::Yellow;   // Set to red
            FastLED.show();        // Update the LED strip
          }
          
      } else {
          Serial.println("Buffer is empty, cannot pop.");
          break; // Exit if buffer is empty
      }
  }
  Serial.println("------------------popping end------------------");
  buttonPressed = false;      // Reset flag
}
   
delay(1000);
//Go to sleep now
 esp_deep_sleep_start();
     // esp_task_wdt_reset();   
  
  
  
}


// circular buffer routines-------------------------------
//moved to library