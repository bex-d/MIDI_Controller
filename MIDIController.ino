#include <ResponsiveAnalogRead.h>
#include <Bounce.h> 

// def vars
const int channel = 1; // MIDI channel, needs to be set to same value or any input within DAW
const int num_analog = 16; // number of analog inputs
const int num_digital = 8; // number of digital inputs

// define the analog input pin numbers and corresponding midi cc numbers 
const int analog_inp[num_analog] = {A7,A6,A5,A4,A3,A2,A1,A0,A8,A9,A10,A11,A14,A18,A16,A20};
const int cc_analog[num_analog] = {7,11,1,46,47,48,49,50,7,10,19,74,80,71,95,91};

// define the digital inputs pin numbers and corresponding midi cc numbers 
const int digital_inp[num_digital] = {11,10,9,8,7,6,5,4};
const int cc_digital[num_digital] = {22,23,24,25,26,27,28,29};
const int bounce_time = 7; //in ms, to ignore noise
const boolean toggled = true;

// digital inputs 'on' threshold
int cc_off = 0;
int cc_on = 65;

byte current_reading[num_analog]; // to store original pin reading
byte new_reading[num_analog]; // to check new pin reading for cc changes


// eliminate noise for analog pins
ResponsiveAnalogRead analog[]{
  {analog_inp[0],true},
  {analog_inp[1],true},
  {analog_inp[2],true},
  {analog_inp[3],true},
  {analog_inp[4],true},
  {analog_inp[5],true},
  {analog_inp[6],true},
  {analog_inp[7],true},
  {analog_inp[8],true},
  {analog_inp[9],true},
  {analog_inp[10],true},
  {analog_inp[11],true},
  {analog_inp[12],true},
  {analog_inp[13],true},
  {analog_inp[14],true},
  {analog_inp[15],true},
}; 

// eliminate noise for digital pins
Bounce digital[] =   {
  Bounce(digital_inp[0], bounce_time), 
  Bounce(digital_inp[1], bounce_time),
  Bounce(digital_inp[2], bounce_time),
  Bounce(digital_inp[3], bounce_time),
  Bounce(digital_inp[4], bounce_time),
  Bounce(digital_inp[5], bounce_time),
  Bounce(digital_inp[6], bounce_time),
  Bounce(digital_inp[7], bounce_time),
}; 

// digital inputs - enable pull up resistor so that button off == HIGH
void setup() {
  for (int i=0;i<num_digital;i++){
    pinMode(digital_inp[i], INPUT_PULLUP);
  }
}

void loop() {
  readAnalogInps(); //call func for reading analog pins
  readDigitalInps(); //call func for reading digital pins
  //ignore incoming midi
  while (usbMIDI.read()) { 
  }
}

//read analog inputs
void readAnalogInps(){  
  for (int i=0;i<num_analog;i++){
    analog[i].update(); // update each pin 
	
    // check if cc value changed
    if(analog[i].hasChanged()) {
      current_reading[i] = analog[i].getValue()>>3;
      if (current_reading[i] != new_reading[i]){
        new_reading[i] = current_reading[i];
        usbMIDI.sendControlChange(cc_analog[i], current_reading[i], channel); //send new cc value over midi
      }
    }
  }
}

//read digital inputs
void readDigitalInps(){
  for (int i=0;i<num_digital;i++){
	digital[i].update();
  
	//  if buttonState == HIGH button is pressed. LOW->HIGH == risingEdge
    if (digital[i].fallingEdge()) {
      usbMIDI.sendControlChange(cc_digital[i], cc_on, channel);  
    }
	
	//  if buttonState == LOW button is not pressed. HIGH->LOW == fallingEdge. send cc_off for button release
    if (digital[i].risingEdge()) {
      usbMIDI.sendControlChange(cc_digital[i], cc_off, channel);  
    }
  }
}
