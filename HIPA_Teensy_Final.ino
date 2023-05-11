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
char activeFileName[16];
char nextFileName[16];
int teensyMode = 0;
int gainVal = 20;

void setup() 
{
  Serial.begin(9600);
  AudioMemory(1024);

  // Initialize all SD card queues & buffers
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
  StereoCodecCtrl.unmuteHeadphone();
  
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

  modeSelect();

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
      break;
    case 2: // Mode 2: CH1 routed to HP, CH2 routed to SD card
      StereoCodecCtrl.unmuteHeadphone();
      amp_L_M1.gain(0);
      amp_R_M1.gain(0);
      amp_L_M2.gain(gainVal);
      amp_R_M2.gain(gainVal);
      amp_L_M3.gain(0);
      amp_R_M3.gain(0);
      startRecording(2);
      continueRecording(2);
      delay(40000);
      stopRecording(2);
      delay(1000);
      break;
    case 3: // Mode 3: CH1 & CH2 routed to SD card (seperate files)
      StereoCodecCtrl.muteHeadphone();
      amp_L_M1.gain(0);
      amp_R_M1.gain(0);
      amp_L_M2.gain(0);
      amp_R_M2.gain(0);
      amp_L_M3.gain(gainVal);
      amp_R_M3.gain(gainVal);
      startRecording(3);
      continueRecording(3);
      delay(40000);
      stopRecording(3);
      delay(1000);
      break;
    default: // teensyMode=0 upon startup
      break; // do nothing if mode not yet selected from WSoC
  }
}

void modeSelect()
{
  int wsocMode = analogRead(A17);
  if((wsocMode > 200) && (wsocMode < 400))
  { // Recieved button 1 press signal
    Serial.println("Mode 1");
    teensyMode=1; // Set teensy to "Standby" mode, routing both CH1 & CH2 to HP output
  }
  else if((wsocMode > 500) && (wsocMode < 700))
  { // Recieved button 2 press signal
    Serial.println("Mode 2");
    teensyMode=2; // Set teensy to "Record/Listen" mode, routing CH1 to HP output and CH2 to SD save
  }
  else if((wsocMode > 800) && (wsocMode < 1000))
  { // Recieved button 3 press signal
    Serial.println("Mode 3");
    teensyMode=3; // Set teensy to "Standby" mode, routing both CH1 & CH2 to SD save
  }
  delay(10);
}

void startRecording(int mode) 
{
  char wavFileName[] = "RECORDING_000000"; // create a new file
  int offset = 1;                            //offset by 1 to get the next file for mode 3 
  for (int count = 0; count < 1000000; count++) 
  { // Iterate file name
    wavFileName[10] = count/100000 + '0';
    wavFileName[11] = ((count/10000) % 10) + '0';
    wavFileName[12] = ((count/1000) % 10) + '0';
    wavFileName[13] = ((count/100) % 10) + '0';
    wavFileName[14] = ((count/10) % 10) + '0';
    wavFileName[15] = count % 10 + '0';
    if (!SD.exists(wavFileName)) 
    { // only open a new file if it doesn't exist
      offset += count;
      wavFile = SD.open(wavFileName, FILE_WRITE);
      break;
    }
  }
  if(mode == 3 && offset < 999999)
  {
    char secondFileName[] = "RECORDING_00000";
    wavFileName[10] = offset/100000 + '0';
    wavFileName[11] = ((offset/10000) % 10) + '0';
    wavFileName[12] = ((offset/1000) % 10) + '0';
    wavFileName[13] = ((offset/100) % 10) + '0';
    wavFileName[14] = ((offset/10) % 10) + '0';
    wavFileName[15] = offset % 10 + '0';
    secondWavFile =  SD.open(secondFileName, FILE_WRITE);
  }
  if (wavFile) 
  {
    SD_Queue_Mono_M2.begin();
  }
  if(secondWavFile)
  {
    SD_Queue_L_M3.begin();
    SD_Queue_R_M3.begin();
  }
}

void continueRecording(int mode)
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