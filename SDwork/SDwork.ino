#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            InputI2S;           //xy=107.19999694824219,145.1999969482422
//AudioPlaySdWav           playSdWav1;     //xy=252.1999969482422,387.20001220703125
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
AudioConnection          patchCord3(mixer2, 0, wavQueue, 0);
//AudioConnection          patchCord4(playSdRaw1, rawQueue);
AudioConnection          patchCord5(amp2, 0, mixer1, 0);
AudioConnection          patchCord6(amp1, 0, mixer2, 0);
AudioConnection          patchCord7(mixer1, 0, OutputI2S, 0);
AudioConnection          patchCord8(mixer2, 0, OutputI2S, 1);
AudioControlSGTL5000     StereoCodecCtrl;     //xy=65.19999694824219,45.19999694824219
// GUItool: end automatically generated code

// Use these with the Teensy Audio Shield
//#define SDCARD_CS_PIN    10
//#define SDCARD_MOSI_PIN  7
//#define SDCARD_SCK_PIN   14

// Use these with the Teensy 3.5 & 3.6 & 4.1 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

File wavFile;
float gainVal = 11.0;
//byte buffer[1024];

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  AudioMemory(1024);
  
  //input I2S no initialization

  //SD 

  //amp 1 & amp 2
  //amp2.gain(gainVal);
  //amp1.gain(gainVal);

  //mixer 1 & 2
  mixer1.gain(0, gainVal);
  mixer2.gain(0, gainVal);

  //outputI2S no initialization

  //wav queue & raw queue
  //wavQueue.freeBuffer();
  //wavQueue.clear();
  
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
    while (!SD.begin(SDCARD_CS_PIN)) //will escape Loop when SD card is put in 
     { 
      Serial.println("Unable to access the SD card");
      delay(1000); // 1.0 sec delay
     }//End of Loop
  }//End of If 
	
  
}

void loop() 
{
  startRecording();
  continueRecording();
  delay(1000);
	stopRecording();
  delay(1000);
}

void startRecording() 
{
  Serial.println("startRecording");
  // while (SD.exists(fileStr)) 
  // {
    /*The SD library writes new data to the end of the
    file, so to start a new recording, the old file
    must be deleted before new data is written.*/
    // SD.remove(fileStr);
  // }
  wavFile = SD.open("RECORD3.WAV", FILE_WRITE);
  if (wavFile) 
  {
    wavQueue.begin();
    //mode = 1;
  }
}

void continueRecording() 
{
  //wavQueue.update();
  wavQueue.freeBuffer();
  Serial.print("The queue has ");
  Serial.print(wavQueue.available());
  Serial.println(" packets available");
  Serial.print("The memory has ");
  Serial.print(AudioMemoryUsage());
  Serial.print(" blocks in use out of ");
  Serial.print(AudioMemoryUsageMax());
  Serial.println(" total");    
  if (wavQueue.available() > 1) 
  {
    Serial.println("In QUEUE");
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, wavQueue.readBuffer(), 256);
    wavQueue.freeBuffer();
    memcpy(buffer+256, wavQueue.readBuffer(), 256);
    wavQueue.freeBuffer();
    // write all 512 bytes to the SD card
    //elapsedMicros usec = 0;
    wavFile.write(buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The wavQueue object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    //Serial.print("SD write, us=");
    //Serial.println(usec);
  }
  else
  {
    Serial.println("first buffer not read. No audio is being Read");
  }
}

void stopRecording() 
{
  Serial.println("stopRecording");
  wavQueue.end();
  if (wavQueue.available() > 0) {
    while (wavQueue.available() > 0) {
      wavFile.write((byte*)wavQueue.readBuffer(), 256);
      wavQueue.freeBuffer();
    }
    wavFile.close();
  }
  //mode = 0;
}