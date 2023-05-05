/************************************************************************************
  IMPORTANT: Program Description                                                    *
  Teensy_Program reads analog input from signal conditioning circuit, then reads    *
  the desired audio routing mode selection from the WSoC buttons.                   *
*************************************************************************************/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            I2S_In;         //xy=71.49998474121094,212.99997997283936
AudioAmplifier           amp_L_M2;           //xy=283.2708053588867,173.2939395904541
AudioAmplifier           amp_L_M1;          //xy=284.50003814697266,75.99998950958252
AudioAmplifier           amp_R_M1;          //xy=284.50003814697266,118.99998092651367
AudioAmplifier           amp_R_M2; //xy=287.2708511352539,237.2939691543579
AudioAmplifier           amp_L_M3; //xy=289.2708511352539,336.29396629333496
AudioAmplifier           amp_R_M3; //xy=293.27083587646484,400.29393577575684
AudioMixer4              mixer_L_M2;         //xy=474.27075576782227,193.29395484924316
AudioMixer4              mixer_L_M3;  //xy=475.2708206176758,352.2939682006836
AudioMixer4              mixer_R_M2; //xy=476.2708168029785,257.2939682006836
AudioMixer4              mixer_R_M3; //xy=477.2708435058594,418.2939758300781
AudioMixer4              mixer_Stereo_M1;        //xy=492.49997329711914,95.99997520446777
AudioOutputI2S           I2S_Out_HP;     //xy=700.0454444885254,96.54545593261719
AudioRecordQueue         SD_Queue_R_M3; //xy=706.2708435058594,419.29393577575684
AudioRecordQueue         SD_Queue_L_M3;         //xy=707.2707214355469,351.29393768310547
AudioRecordQueue         SD_Queue_Mono_M2;         //xy=725.2707176208496,258.2938938140869
AudioConnection          patchCord1(I2S_In, 0, amp_L_M1, 0);
AudioConnection          patchCord2(I2S_In, 0, amp_L_M2, 0);
AudioConnection          patchCord3(I2S_In, 0, amp_L_M3, 0);
AudioConnection          patchCord4(I2S_In, 1, amp_R_M1, 0);
AudioConnection          patchCord5(I2S_In, 1, amp_R_M2, 0);
AudioConnection          patchCord6(I2S_In, 1, amp_R_M3, 0);
AudioConnection          patchCord7(amp_L_M2, 0, mixer_L_M2, 0);
AudioConnection          patchCord8(amp_L_M1, 0, mixer_Stereo_M1, 0);
AudioConnection          patchCord9(amp_R_M1, 0, mixer_Stereo_M1, 1);
AudioConnection          patchCord10(amp_R_M2, 0, mixer_R_M2, 0);
AudioConnection          patchCord11(amp_L_M3, 0, mixer_L_M3, 0);
AudioConnection          patchCord12(amp_R_M3, 0, mixer_R_M3, 0);
AudioConnection          patchCord13(mixer_L_M2, 0, mixer_Stereo_M1, 3);
AudioConnection          patchCord14(mixer_L_M3, SD_Queue_L_M3);
AudioConnection          patchCord15(mixer_R_M2, SD_Queue_Mono_M2);
AudioConnection          patchCord16(mixer_R_M3, SD_Queue_R_M3);
AudioConnection          patchCord17(mixer_Stereo_M1, 0, I2S_Out_HP, 0);
AudioConnection          patchCord18(mixer_Stereo_M1, 0, I2S_Out_HP, 1);
AudioControlSGTL5000     StereoCodecCtrl; //xy=81.5,20
// GUItool: end automatically generated code

// Teensy 4.1 SD pin configuration
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

// Custom variables initialization
File wavFile;               // Create .wav file
File secondWavFile;         // For mode 3
int wsocMode = 0;           // Initialization of wsocMode selection variable
int teensyMode = 0;         // Initialization of teensyMode selection variable
float gainVal = 20.0;       // Gain level when amps activated later, manually tuned to confortable level
AudioRecordQueue SD_Queue;  // Initialization og SD_Queue variable, used in start and stop recording functions
bool recordFlag = false;

void setup() {
  Serial.begin(9600);
  AudioMemory(60);

  // Initialize all SD card queues & buffers
  SD_Queue_Mono_M2.freeBuffer();
  SD_Queue_L_M3.freeBuffer();
  SD_Queue_R_M3.freeBuffer();
  SD_Queue_Mono_M2.clear();
  SD_Queue_L_M3.clear();
  SD_Queue_R_M3.clear();

  // Initilaize all amp gains to zero (to be changed within inf loop)
  amp_L_M1.gain(0);
  amp_R_M1.gain(0);
  amp_L_M2.gain(0);
  amp_R_M2.gain(0);
  amp_L_M3.gain(0);
  amp_R_M3.gain(0);

  // Stereo Codec Control Initialization
  StereoCodecCtrl.enable();
  StereoCodecCtrl.inputSelect(AUDIO_INPUT_LINEIN);
  StereoCodecCtrl.muteHeadphone();
  
  // SD card Initialization (generated code)
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {  // stop here if no SD card
    while (!SD.begin(SDCARD_CS_PIN)) { 
      Serial.println("Unable to access the SD card");
      delay(1000); // 1.0 sec delay
     }
  }  
}

void loop() 
{
  int knob = analogRead(A1);        // read the volume potentiometer position (analog input A1)
  float vol = (float)knob / 1280.0; // calc A2 float
  StereoCodecCtrl.volume(vol);      // set 'vol' to A2 pot value
  
  // Watch for WSoC button presses
  int wsocMode = analogRead(A17);
  if((wsocMode > 200) && (wsocMode < 400))
  { // Recieved button 1 press signal
    teensyMode=1; // Set teensy to "Standby" mode, routing both CH1 & CH2 to HP output
  }
  else if((wsocMode > 500) && (wsocMode < 700))
  { // Recieved button 2 press signal
    teensyMode=2; // Set teensy to "Record/Listen" mode, routing CH1 to HP output and CH2 to SD save
  }
  else if((wsocMode > 800) && (wsocMode < 1000))
  { // Recieved button 3 press signal
    teensyMode=3; // Set teensy to "Standby" mode, routing both CH1 & CH2 to SD save
  }

  // Watch for teensyMode state change, then set audio routing mode
  switch (teensyMode) 
  {
    case 1: // Mode 1: CH1 & CH2 routed to HP
      StereoCodecCtrl.unmuteHeadphone();
      amp_L_M1.gain(gainVal);
      amp_R_M1.gain(gainVal);
      amp_L_M2.gain(0);
      amp_R_M2.gain(0);
      amp_L_M3.gain(0);
      amp_R_M3.gain(0);
      recordFlag = false;
      break;
    case 2: // Mode 2: CH1 routed to HP, CH2 routed to SD card
      StereoCodecCtrl.umuteHeadphone();
      amp_L_M1.gain(0);
      amp_R_M1.gain(0);
      amp_L_M2.gain(gainVal);
      amp_R_M2.gain(gainVal);
      amp_L_M3.gain(0);
      amp_R_M3.gain(0);
      setFile(2);
      recordFlag = true;
      recordingState(2);
      break;
    case 3: // Mode 3: CH1 & CH2 routed to SD card (seperate files)
      StereoCodecCtrl.muteHeadphone();
      amp_L_M1.gain(0);
      amp_R_M1.gain(0);
      amp_L_M2.gain(0);
      amp_R_M2.gain(0);
      amp_L_M3.gain(gainVal);
      amp_R_M3.gain(gainVal);
      setFile(3);
      recordFlag = true;
      recordingState(3);
      break;
    default: // teensyMode=0 upon startup
      break; // do nothing if mode not yet selected from WSoC
  }
}

//10second rec


void setFile(int mode) 
{
  char wavFileName[] = "RECORDING_00000"; // create a new file
  int j = 1;                            //offset by 1 to get the next file for mode 3 
  for (int i = 0; i < 1000; i++) 
  { // Iterate file name
    wavFileName[10] = i/100 + '0';
    wavFileName[11] = ((i/10) % 10) + '0';
    wavFileName[12] = i% + '0';
    wavFileName[13] =
    wavFileName[14] = 
    wavFileName[15] =
    if (!SD.exists(wavFileName)) 
    { // only open a new file if it doesn't exist
      j += i;
      wavFile = SD.open(wavFileName, FILE_WRITE);
    }
  }
  if(mode == 3 && j < 999)
  {
    char secondFileName[] = "RECORDING_000";
    secondFileName[10] = j/100 + '0';
    secondFileName[11] = ((j/10) % 10) + '0';
    secondFileName[12] = j%10 + '0';
    secondWavFile =  SD.open(secondFileName, FILE_WRITE);
  }
}

void setFileWrite(int mode)
{
  //byte memLimit[1024^2];
  //while(memLimit [1024^2] != NULL)
  //{
  if (SD_Queue_Mono_M2.available() > 1 && mode == 2) 
  {
    byte buffer[512];
    memcpy(buffer, SD_Queue_Mono_M2.readBuffer(), 256);
    SD_Queue_Mono_M2.freeBuffer();
    memcpy(buffer+256, SD_Queue_Mono_M2.readBuffer(), 256);
    SD_Queue_Mono_M2.freeBuffer();
    wavFile.write(buffer, 512); // write all 512 bytes to the SD card
    //memcpy(memLimit, buffer, 512);
  }
  else if(SD_Queue_L_M3.available() > 1 && SD_Queue_R_M3.available() > 1 && mode == 3)
  {
    //
    byte firstBuffer[256], secondBuffer[256];
    memcpy(firstBuffer, SD_Queue_L_M3.readBuffer(), 256);
    SD_Queue_L_M3.freeBuffer();
    wavFile.write(firstBuffer, 256); // write all 512 bytes to the SD card
    //memcpy(memLimit, firstBuffer, 256);
    //
    memcpy(secondBuffer, SD_Queue_R_M3.readBuffer(), 256);
    SD_Queue_R_M3.freeBuffer();
    wavFile.write(secondBuffer, 256); // write all 512 bytes to the SD card
    //memcpy(memLimit, secondBuffer, 256);
  }
  //}
}

void stopRecording(int mode) 
{
  if(mode == 2)
  {
    SD_Queue_Mono_M2.end();
    if (SD_Queue_Mono_M2.available() > 0) 
    {
      while (SD_Queue_Mono_M2.available() > 0) 
      {
        wavFile.write((byte*)SD_Queue_Mono_M2.readBuffer(), 256);
        SD_Queue_Mono_M2.freeBuffer();
      }
      wavFile.close();
    }
  }
  if(mode == 3)
  {
    SD_Queue_L_M3.end();
    if (SD_Queue_L_M3.available() > 0) 
    {
      while (SD_Queue_L_M3.available() > 0) 
      {
        wavFile.write((byte*)SD_Queue_L_M3.readBuffer(), 256);
        SD_Queue_L_M3.freeBuffer();
      }
    }

    SD_Queue_R_M3.end();
    if (SD_Queue_R_M3.available() > 0) 
    {
      while (SD_Queue_R_M3.available() > 0) 
      {
        secondWavFile.write((byte*)SD_Queue_R_M3.readBuffer(), 256);
        SD_Queue_R_M3.freeBuffer();
      }
    }
      secondWavFile.close();
    }
}


