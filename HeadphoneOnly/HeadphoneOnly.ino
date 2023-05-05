#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            InputI2S;           //xy=107.19999694824219,145.1999969482422
AudioPlaySdWav           playSdWav1;     //xy=252.1999969482422,387.20001220703125
//AudioPlaySdRaw           playSdRaw1;     //xy=255.20001220703125,436.20001220703125
AudioAmplifier           amp2;           //xy=284.20001220703125,103.19999694824219
AudioAmplifier           amp1;           //xy=302.20001220703125,250.1999969482422
AudioMixer4              mixer1;         //xy=448.20001220703125,110.19999694824219
AudioMixer4              mixer2;         //xy=468.1999969482422,257.1999969482422
AudioOutputI2S           OutputI2S;           //xy=712.1999969482422,114.19999694824219
AudioRecordQueue         wavQueue;         //xy=728.2000122070312,409.20001220703125
//AudioRecordQueue         rawQueue;         //xy=733.1999969482422,459.1999969482422
AudioConnection          patchCord1(InputI2S, 0, amp2, 0);
AudioConnection          patchCord2(InputI2S, 1, amp1, 0);
AudioConnection          patchCord3(playSdWav1, 0, wavQueue, 0);
//AudioConnection          patchCord4(playSdRaw1, rawQueue);
AudioConnection          patchCord5(amp2, 0, mixer1, 0);
AudioConnection          patchCord6(amp1, 0, mixer2, 0);
AudioConnection          patchCord7(mixer1, 0, OutputI2S, 0);
AudioConnection          patchCord8(mixer2, 0, OutputI2S, 1);
AudioControlSGTL5000     StereoCodecCtrl;     //xy=65.19999694824219,45.19999694824219
// GUItool: end automatically generated code

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

// Use these with the Teensy 3.5 & 3.6 & 4.1 SD card
//#define SDCARD_CS_PIN    BUILTIN_SDCARD
//#define SDCARD_MOSI_PIN  11  // not actually used
//#define SDCARD_SCK_PIN   13  // not actually used


File wavFile;
float gainVal = 11.0;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  AudioMemory(60);
  
  //input I2S no initialization

  //SD Wav
  playSdWav1.stop();
  //playSDRaw2.stop();

  //amp 1 & amp 2
  //amp2.gain(gainVal);
  //amp1.gain(gainVal);

  //mixer 1 & 2
  //mixer1.gain(0, gainVal);
  //mixer2.gain(0, gainVal);

  //outputI2S no initialization

  //wav queue & raw queue
  wavQueue.freeBuffer();
  wavQueue.clear();
  
  //rawQueue.freeBuffer();
  //rawQueue.clear();

  //Stereo Codec Control Initialization
  StereoCodecCtrl.enable();  
  StereoCodecCtrl.volume(0.6);
  StereoCodecCtrl.inputSelect(AUDIO_INPUT_LINEIN);
  StereoCodecCtrl.unmuteHeadphone();
  //StereoCodecCtrl.headphoneSelect(AUDIO_HEADPHONE_LINEIN);
  
  //SD card Initialization
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) //Check for valid SD card
  {  // stop here if no SD card, but print a message
    while (!SD.begin(SDCARD_CS_PIN)) //infinite Loop 
     { 
      Serial.println("Unable to access the SD card");
      delay(1000); // 1.0 sec delay
     }
  }  
}

void loop() 
{
  Serial.println("In the Loop");
  delay(1000);  
}
