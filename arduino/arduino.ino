#include <Arduino.h>
#include <SPIFFS.h>
//https://github.com/earlephilhower/ESP8266Audio
#include <AudioFileSourceSPIFFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

// config
const int timeSleep = 60 * 10  * uS_TO_S_FACTOR;
const int timeLongSleep = 60 * 15 * uS_TO_S_FACTOR;
const int timeRunning = 10 * 1000;
const float volume = 1.5;//0.026;//3.99;//
const float minVoltage = 0.5; //go long sleep
int _speed = 255; // speed motor
 
// pin audio

const int pinBCK = 25;
const int pinLRCK = 33;
const int pinDATA = 26;

// pin motor
const int pinPWMA = 13;
const int pinAIN2 = 12;
const int pinAIN1 = 14;
const int pinSTBY = 27;

// solar reading pin voltage 7.2v
const int pinINPUT = 32;

// variables
enum direction {
   clockwise,
   counterClockwise
};

RTC_DATA_ATTR int indexList = 0;
RTC_DATA_ATTR unsigned int countRun = 0;
bool isPlaying= false;
int countList = 0;
char playList[20][40];

AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;

const int pinMotorA[3] = { pinPWMA, pinAIN2, pinAIN1 };
direction dir;

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void enableMotors(bool value)
{ 
  if(value) digitalWrite(pinSTBY, HIGH);
  else digitalWrite(pinSTBY, LOW);
}

void setupMotor()
{
   pinMode(pinPWMA, OUTPUT);
   pinMode(pinAIN2, OUTPUT);
   pinMode(pinAIN1, OUTPUT);
   pinMode(pinSTBY, OUTPUT);
   
   enableMotors(true);
}

void moveMotor(int dir,const int pinMotor[3], int speed)
{
  if(dir == clockwise){
    digitalWrite(pinMotor[1], LOW);
    digitalWrite(pinMotor[2], HIGH);
    analogWrite(pinMotor[0], speed);    
  }else{
    digitalWrite(pinMotor[1], HIGH);
    digitalWrite(pinMotor[2], LOW);
    analogWrite(pinMotor[0], speed);    
  }
}

void scanSound()
{
    File root = SPIFFS.open("/");
    if(!root || !root.isDirectory()) return;

    File file = root.openNextFile();
    while(file){
        if(!file.isDirectory()){
            //Serial.println(file.name());

            strcpy(playList[countList], "/");
            strcat(playList[countList], file.name());
            countList++;
        }
        file = root.openNextFile();
    }
}

void setupAudio()
{
  SPIFFS.begin();

  scanSound();
  
  out = new AudioOutputI2S();
  out->SetPinout(pinBCK, pinLRCK, pinDATA);
  out->SetGain(volume);
  
  mp3 = new AudioGeneratorMP3();
}

void play(const char* nameFile)
{
  Serial.println(String ("play :")+ nameFile);
  file = new AudioFileSourceSPIFFS(nameFile);
  mp3->begin(file, out);
}

void setup() 
{
  Serial.begin( 115200 ); 

  delay(1000);

  // scale 3.3v 7.2v max 
  float voltage = fmap(analogRead(pinINPUT), 0.0f, 4095.0f, 0.0f, 7.2f);
  Serial.println(voltage);
  
  if(voltage < minVoltage)
  {
    Serial.println("sleep long");
    //sleep long
    esp_sleep_enable_timer_wakeup(timeLongSleep);
    esp_deep_sleep_start();
  }
  //init
  setupMotor();
  moveMotor(counterClockwise,pinMotorA, _speed);

  Serial.println("motor");
  // 
  if(countRun % 2 == 0)
  {
    isPlaying = true;
    Serial.println("audio");
    setupAudio();
    play(playList[indexList]); 
  }

  countRun++;
}

void loop ()
{  
  //float voltage = fmap(analogRead(pinINPUT), 0.0f, 4095.0f, 0.0f, 7.2f);
  //Serial.println(voltage);

  //sleep
  if (millis() > timeRunning) 
  {
    // stop all
    if(isPlaying && mp3->isRunning()) mp3->stop();
    enableMotors(false);
    
    if (isPlaying){
      // next sound index
      indexList++;
      if(indexList>=countList) indexList = 0; 
    }
    Serial.println("sleep");
    esp_sleep_enable_timer_wakeup(timeSleep);
    esp_deep_sleep_start();
  }
  // mp3->loop();
  if (isPlaying && mp3->isRunning() && !mp3->loop()) mp3->stop();
  //repeat sound (loop)
  else if (isPlaying && !mp3->isRunning()) play(playList[indexList]);
}
