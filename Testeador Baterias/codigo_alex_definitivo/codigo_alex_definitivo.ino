/*  Lithium Battery Tester - works with hardware shown at vwlowen.co.uk/arduino/battery-tester/batery-tester.htm */


#include <TimerOne.h>                         //https://github.com/PaulStoffregen/TimerOne

#include <Wire.h>
#include <Adafruit_INA219.h>                  //https://github.com/adafruit/Adafruit_INA219    

#include <LiquidCrystal.h>

int end_sounder = A2;                        // Digital output for sounder
boolean sounded = false;

int manual_volts_pin = 11;                   // Push button to cycle through battery cutoff volts when in manual.
int manual_pin = 12;                         // Manual/Run switch. Grounded in Manual.
int manual_value = A3;                       // Set target discharge current with potentiometer

int lcdRS = 2;                               // Define output pins for LCD
int lcdE = 4;
int lcdD4 = 5;
int lcdD5 = 6;
int lcdD6 = 7;
int lcdD7 = 8;

LiquidCrystal lcd(lcdRS, lcdE, lcdD4, lcdD5, lcdD6, lcdD7); 

byte bell[8] = {0, 4, 14, 14, 14, 31, 4, 0};  // Data for 'bell' character for LCD.

const byte pwm_pin = 9;                       // PWM DAC, only pins 9 and 10 are allowed
const byte period = 128;                      // for 10 bit DAC 

unsigned long  startMillisec;                 // Variables for discharge timer.
unsigned long  sampleTime = 10000;            // Default samples to PC time (ms)
int pc_SampleCount = 0;
unsigned long pc_SampleTime = 5000;
unsigned long millis_PC_wait;                 // Timer for samples to PC
unsigned long millisCalc_mAh;                 // Timer for nexr mAh calc. and LCD write.

float last_hours = 0.0;                       // Working variables for time and mAh
float mAh_soFar = 0.0;

int days, hours;
int mins, secs;
int tMins;

Adafruit_INA219 ina219;                       // Create instance for INA219 sensor library

int current_mA = 0;                       // Variables for INA219 readings
float busvoltage = 0.0;
float shuntvoltage = 0.0;
float loadvoltage = 0.0;
float battery_volts = 0.0;
float vR;                                     // Volt drop in circuit 

int target_mA = 0;                            // Default 'set point' variables
float cutoff_voltage = 3.0;
int time_limit = 180;
float offset = 0.0;
float error;

char* batt[] = {"Li-ion 3.7v", "Li-ion 7.2v", "Ni-MH", "Ni-Cd", "Lead 6v", "Lead 12v", "SE18-12"};
float voltage[] = {3.0, 5.8, 0.9, 1.1, 5.8, 11.6, 10.8};
int battCount = 0;

int cancel = 0;                               // Variables and flags to terminate test
boolean timed_out = false;
boolean high_current = false;
boolean cutoff_voltage_reached = false;

boolean manual = false;

int pwm = 0;                                     // Starting PWM value for 10-bit DAC
float kP = 30;                                   // Simple 'Proportional' control term
int tolerance = 1;                               // Deadband to stop 'hunting' around target value
int beep = 1;                                    // Sounder on/off  (0 = off, 1 = on)


/* === Set up - normal setup parameters and initialises values for the contol loop ===== */

void setup() {
  
  lcd.createChar(1, bell);                       // Bell char for LCD. Displayed when sounder is enabled.
   
  pinMode(manual_pin, INPUT_PULLUP);             // MAN/RUN switch. Grounded in MAN position.
 
  pinMode(manual_volts_pin, INPUT_PULLUP);

  pinMode(end_sounder, OUTPUT);                  // Output to sounder.
  digitalWrite(end_sounder, LOW);

  pinMode(pwm_pin, OUTPUT);                      // Set up 10-bit DAC pin as output
  Timer1.initialize(period);                     // 
  Timer1.pwm(pwm_pin, pwm);                      // and set zero PWM out
  
  ina219.begin();                                // Initialise INA219 Current Sensor

  Serial.begin(9600);                           // Initialise Arduino to PC Com Port
  
  lcd.begin(20, 4);                              // Initialize the LCD.
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 1);
  
  Serial.println("Initialising...");             // Print something on the console to show
  lcd.print("Initialising...");                  // Print something on the display to show
  delay(1000);                                   // it's working.
  
  lcd.clear();                                   // Clear display
  delay(100);
  
  while (digitalRead(manual_pin) == LOW) {          // Loop here to adjust desired load mA manually
     lcd.setCursor(0, 1); 
     lcd.print("Set mA: ");
     manual = true;
     target_mA = map(analogRead(manual_value), 0, 1023, 0, 3000);
     lcd.setCursor(9, 1);
     lcd.print("      ");
     lcd.setCursor(9, 1);
     lcd.print(target_mA); 

     if (digitalRead(manual_volts_pin) == LOW) {
       battCount++;
       if (battCount > sizeof(voltage)/sizeof(float)-1) battCount = 0;
       delay(250);
     }

     lcd.setCursor(0, 2);
     lcd.print("Battery: ");
     lcd.setCursor(9, 2);
     lcd.print("           ");
     lcd.setCursor(9, 2);
     lcd.print(batt[battCount]);
     
     time_limit = 480;                               // 8 hours. Test relies on the..
     cutoff_voltage = voltage[battCount];            // Cutoff voltage is determined by battery type.
     kP = 30;                                        // Default values are used in Manual Mode.
     offset = 0;
     tolerance = 1;
     beep = 1;
     cancel = 0;
     delay(200);
  }
  
  if (!manual) {                                     // If not manual, must be under PC control.
    lcd.setCursor(0, 1);
    lcd.print("Waiting for settings");               // Prompt that we're waiting to receive
    lcd.setCursor(6, 2);                             // the load settings from the application
    lcd.print("from PC...");                         // that's running on the PC.
  
    while (Serial.available() == 0 ) ;              // Wait for settings params from PC
    if (Serial.available() > 0) {                   // Data available on serial port from PC
      target_mA = Serial.parseInt();                // so read each one in turn into variables.
      cutoff_voltage = Serial.parseFloat();         // Minimum battery voltage to end test
      time_limit = Serial.parseInt();               // maximum time allowed for the test
      sampleTime = Serial.parseInt() * 1000;        // Interval to send data to PC
      kP = Serial.parseInt();                       // kP - control loop Proportional value
      offset = Serial.parseFloat();                 // To reduce offset between target and actual mA
      tolerance = Serial.parseInt();
      beep = Serial.parseInt();                     // Sounder on/off   (0=off, 1=on)
      cancel = Serial.parseInt();                   // Will = 0. Clear Cancel flag
    }
    Serial.flush();                                 // Make sure the serial port is empty to avoid
                                                     // false 'Cancel' messages in the control loop.
  }   

     
  /* These lines echo the received values to the LCD and display for 5 seconds  */
  
  lcd.clear();
  delay(200);
  lcd.print("Target mA   "); lcd.print(target_mA);
  lcd.setCursor(0, 1);
  lcd.print("Cutoff v    "); lcd.print(cutoff_voltage); 
  if (beep == 1) {
    lcd.setCursor(19, 0);
    lcd.write(1);
  }
  lcd.setCursor(0, 2); 
  lcd.print("Time (min)  "); lcd.print(time_limit); 
  lcd.setCursor(0, 3); lcd.print("S="); lcd.print(sampleTime/1000);
  lcd.print(" t="); lcd.print(tolerance); lcd.print(" kP="); lcd.print((int)kP);
  lcd.print(" i="); lcd.print(offset, 1); 
 

  delay(5000);
  lcd.clear();
  startMillisec = millis();                        // get millisec time at start
  if (beep == 1) {
    digitalWrite(end_sounder, HIGH);               // Bleep once to indicate test starting.
    delay(50);
    digitalWrite(end_sounder, LOW);  
  }
}

/* =========== Get Current and Voltage from Adafruin INA219 breakout board ============ */

void readINA219() {                                 // Function to read curent and voltage from INA219.
  float R = 0.12;                                   // "Tweaking" value to compensate for circuit resistance.
  float temp_mA = 0.0;
  float temp_V = 0.0;
  float current_factor = 1.18;                      // factor empirico para que muestre corriente correcta
  shuntvoltage = ina219.getShuntVoltage_mV();       // Get values for the INA219 sensor.
  
  for (int i = 0; i< 10; i++) {
    temp_V = temp_V + ina219.getBusVoltage_V();     // Take average of 10 readings for
    delayMicroseconds(600);                                      // Bus Voltage.......
  } 
  busvoltage = temp_V / 10; 

  
  for (int i = 0; i< 20; i++) {
    temp_mA = temp_mA + ina219.getCurrent_mA();     //...... and 20 reading for current.
    delayMicroseconds(600);
  }
 current_mA = temp_mA*current_factor / 20;

  vR = R * current_mA / 1000;                            // Total load voltage has to factor in the
  loadvoltage = busvoltage  + (shuntvoltage/1000) + vR;  // volt drop across the 0.1R shunt & circuit resistance.

}

/* ========== Main Loop ============================================================= */

void loop() {
  
  readINA219();                                              // Get current and voltage values.    

  /* Calculate error between target_mA and actual mA and apply to PWM value */
  
  
  error = abs(target_mA - current_mA);
  error = (error / target_mA) * 100;
  
  if ((error > tolerance)  ) {                              // If out of tolerance (deadband to stop 'hunting')
    error = error - offset;                                 // Bias (long term error compensation)
    error = (error * kP) / 100;                             // 'proportional' factor reduces impact of 'raw' error.
    error = constrain(error, 0.0, 50.0); // 25
  
    if (current_mA > target_mA) error = -error;             // Determine if it's a pos or neg error.
  
    pwm =  abs(pwm + round(error));
    pwm = constrain(pwm, 0, 1023); 
  }

 
  if (current_mA > (target_mA * 2.0) ) {                     // Check if measured current has 
     pwm = 0;                                                // overshot the target value by more than
     Timer1.pwm(pwm_pin, pwm);                               // 100% and abort if it has.
     high_current = true;
     target_mA = 0;
     lcd.setCursor(3, 1);
     lcd.println("ERROR - Hi mA");
     Serial.print("MSGSTError - High mAMSGEND");
  }  

  if (loadvoltage < cutoff_voltage) {                        // Monitor battery voltage and finish
    delay(3000);                                             // the load test when it reaches the
    readINA219();                                            // target value.  Allow 3 seconds for
    if (loadvoltage < cutoff_voltage) {                     // short dip in battery voltage when
      pwm = 0;                                                 // load comes on.
      Timer1.pwm(pwm_pin, pwm);                                
      cutoff_voltage_reached = true;
      target_mA = 0;
      lcd.setCursor(5, 1);
      lcd.println("FINISHED");
      Serial.print("MSGSTTest FinishedMSGEND");
    }
  }

  if ((!cutoff_voltage_reached) && (tMins > time_limit)) {    // A time limit can also be set to
    pwm = 0;                                                  // abort the test if it appears to
    Timer1.pwm(pwm_pin, pwm);                                 // be taking too long.
    timed_out = true;
    target_mA = 0;
    lcd.setCursor(2, 1);
    lcd.println("TIMED OUT");    
    Serial.print("MSGSTTime ExceededMSGEND");
  }  
  
 
  if (Serial.available() > 0) {                     // Data available on serial port from PC
      cancel = Serial.parseInt();                   // 999 will calcel the test. 0 will clear Cancel flag

      if (cancel == 999) {                          // 999 from the PC means 'Cancel' the test.
        pwm = 0;
        Timer1.pwm(pwm_pin, pwm);
        target_mA = 0;
        lcd.setCursor(3, 1);
        lcd.print("CANCELLED");
        Serial.print("MSGSTUser cancelledMSGEND");
      } 
      
    }
    Serial.flush();
       
  /* If all is ok, outout the PWM value to adjust the current.  */
     
  if   ((cancel == 0) && (!timed_out) && (!high_current) && (!cutoff_voltage_reached)) {
     Timer1.pwm(pwm_pin, pwm);                               // Adjust PWM to calculated value.
  }
  else {                                                     // otherwise sound the beep
     if ((!sounded) && (beep == 1)) {
      sounded = true;
      for (int i = 0; i< 10; i++) {
        digitalWrite(end_sounder, HIGH);
        delay(50);
        digitalWrite(end_sounder, LOW);
        delay(50);
      }
    }
  }
  
  /* Calculate the elapsed time and the mA / hr used each second round the loop. Send to LCD and PC */
  
  getTime();

    if ((millis() - 1000) >= millisCalc_mAh) {
    float this_hours = (millis() - startMillisec) / (1000.0 * 3600.0);    // Calculate mA used in this time sample
    mAh_soFar = mAh_soFar + ((this_hours - last_hours) * current_mA);     //
    last_hours = this_hours;  
    write_to_lcd();                                                       // Write data to LCD
    millisCalc_mAh = millis();
  }


 if (pc_SampleCount < 100) {
  pc_SampleTime = 5000;
   
 } else
  pc_SampleTime = sampleTime;
  
  if (millis() > millis_PC_wait + pc_SampleTime) {               // If the Sample-to-PC time has
      write_to_pc();                                          // elapsed, send data to the PC
      pc_SampleCount++;
      millis_PC_wait = millis();                              // and reset the elapsed time
  }                                                           // counter.
}

/* ==============  Write values to LCD ========================================== */

void write_to_lcd() {
  
    lcd.setCursor(0, 0);
    if (current_mA < 1000) lcd.print(" "); if (current_mA < 100) lcd.print(" ");
    if (current_mA < 10) lcd.print(" ");
    lcd.print(current_mA); lcd.print(" mA");
    lcd.setCursor(9, 0);                              //
    lcd.print(loadvoltage); lcd.print(" v  "); 
    
    if (beep == 1) {
      lcd.setCursor(19, 0);
      lcd.write(1);
    }    
    
    if ((cancel == 0) && (!timed_out) && (!high_current) && (!cutoff_voltage_reached)) {
      lcd.setCursor(0, 2);
      lcd.print("Time: "); 
      lcd.print(days); lcd.print(":"); if (hours < 10) lcd.print("0"); lcd.print(hours); lcd.print(":");
      if (mins < 10) lcd.print("0"); lcd.print(mins); lcd.print(":"); 
      if (secs < 10) lcd.print("0"); lcd.print(secs);
      lcd.setCursor(0, 3);
      lcd.print("Used: "); lcd.print(mAh_soFar); lcd.print(" mAh");
    }
  
}

/* =================== Send values to PC =================================== */

void write_to_pc() {
  
  /* Send time elapsed to PC. Formatting is done this end as it's easier than
     having to parse/unparse the string twice at the other end.  */


 String message = "GRAPHVS";
 message += days;
 message += ":";
 if (hours < 10) message += "0"; message += hours; message += ":";
 if (mins < 10) message += "0"; message += mins; message += ":";
 if (secs < 10) message += "0"; message += secs; message += "!";

 message += mAh_soFar; message += "!";
 message += (int) current_mA; message += "!";
 message += loadvoltage; message += "GRAPHVEND";

 Serial.print(message);
 
 Serial.flush();        // Flush serial port before it returns to the main loop.
}


/* === Generic routine for hours, minutes and seconds between two millis() values.===*/
void getTime() { 
     
 long day = 86400000; // 86400000 milliseconds in a day
 long hour = 3600000; // 3600000 milliseconds in an hour
 long minute = 60000; // 60000 milliseconds in a minute
 long second =  1000; // 1000 milliseconds in a second

      
 unsigned long timeNow =  millis() - startMillisec;
 tMins = timeNow / minute;
 
 days = timeNow / day ;                               
 hours = (timeNow % day) / hour;                      
 mins = ((timeNow % day) % hour) / minute ;        
 secs = (((timeNow % day) % hour) % minute) / second;

}
