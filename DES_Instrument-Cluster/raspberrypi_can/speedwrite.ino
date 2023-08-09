// #include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <mcp2515.h>

// LiquidCrystal_I2C lcd (0x27, 16, 2);

// Make new object of MCP2515I which is a class in mcp2515.h and the object uses CS pin 10
// Can sheild of arduino's CS pin is 9
MCP2515 mcp2515(10);

// Check 2 pulses for each lap
const byte PulsesPerRevolution = 2;
// 100ms for timeout
const unsigned long ZeroTimeout = 100000;
// Revolutions Per Minute
const byte numReadings = 2;

volatile unsigned long LastTimeWeMeasured;
volatile unsigned long PeriodBetweenPulses = ZeroTimeout + 1000;
volatile unsigned long PeriodAverage = ZeroTimeout + 1000;
unsigned long FrequencyRaw;
unsigned long FrequencyReal;
unsigned long RPM;
unsigned int PulseCounter = 1;
unsigned long PeriodSum;

unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;
unsigned long CurrentMicros = micros();
unsigned int AmountOfReadings = 1;
unsigned int ZeroDebouncingExtra;
unsigned long readings[numReadings];
unsigned long readIndex;  
unsigned long total; 
unsigned long average;

// struct from mcp2515.h 
struct can_frame canMsg;
/*
    uint32_t can_id;  // CAN ID of the frame and RTR and IDE bits
    uint8_t can_dlc;  // Length of the payload (0-8)
    uint8_t data[8];  // Payload data
*/

// only acitivate once
void setup() {
  // For serial communicate
  Serial.begin(9600);
  // lcd.init();
  // lcd.backlight();

  // Connect pin 3 to speed sensor
  attachInterrupt(digitalPinToInterrupt(3), Pulse_Event, RISING);

  // For CAN communicate
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS);
  mcp2515.setNormalMode();

  delay(1000);
}

void loop() {
  // Stores the start time of the current loop and the end time of the previous loop.
  LastTimeCycleMeasure = LastTimeWeMeasured;
  CurrentMicros = micros();
  // Check overflow
  if (CurrentMicros < LastTimeCycleMeasure) {
    LastTimeCycleMeasure = CurrentMicros;
  }

  // Pulse is the "width" of a digital signal that changes over a given period of time
  // Calculated using the average period between pulses to calculate frequency.
  FrequencyRaw = 10000000000 / PeriodAverage;

  // Check if wheel stop or not
  if (PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra) {
    FrequencyRaw = 0;  // Set frequency as 0.
    ZeroDebouncingExtra = 2000;
  } else {
    ZeroDebouncingExtra = 0;
  }

  // Calculate actual real frequency
  FrequencyReal = FrequencyRaw / 10000;

  RPM = FrequencyRaw / PulsesPerRevolution * 60;
  RPM = RPM / 10000;
  total = total - readings[readIndex];
  readings[readIndex] = RPM;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;

  canMsg.can_id  = 0x0F6;
  canMsg.can_dlc = 2;
  // canMsg.data[0] = RPM;
  canMsg.data[0] = 0xFF;
  canMsg.data[1] = 0xFF;

  mcp2515.sendMessage(&canMsg);

  Serial.print("Period: ");
  Serial.print(PeriodBetweenPulses);
  Serial.print("\tReadings: ");
  Serial.print(AmountOfReadings);
  Serial.print("\tFrequency: ");
  Serial.print(FrequencyReal);
  Serial.print("\tRPM: ");
  Serial.print(RPM);
  Serial.print("\tTachometer: ");
  Serial.println(average);
  int rpm_integer = static_cast<int>(RPM);
  Serial.print("\tRPM(int): ");
  Serial.println(rpm_integer);

  // lcd.setCursor(0, 0);
  // lcd.print("RPM : ");
  // lcd.print(RPM);
  // lcd.print("   ");
}

void Pulse_Event() {
  PeriodBetweenPulses = micros() - LastTimeWeMeasured;
  LastTimeWeMeasured = micros();
  if (PulseCounter >= AmountOfReadings)  {
    PeriodAverage = PeriodSum / AmountOfReadings;
    PulseCounter = 1;
    PeriodSum = PeriodBetweenPulses;

    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);
    AmountOfReadings = RemapedAmountOfReadings;
  } else {
    PulseCounter++;
    PeriodSum = PeriodSum + PeriodBetweenPulses;
  }
}
