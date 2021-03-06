//LCD SECTION
/*
 Hitachi HD44780 driver with 16 columns by 2 lines LCD display.
 PINOUT CONNECTIONS:
 
 //      LCD BOARD PINS                                         SR OUPUTS        SR INPUTS              MICROCONTROLLER PINS
 * LCD RS pin to digital pin 8  ->  altered to pin 2       <--- PIN 2 (Q2)       PIN 14 (DATA)    <---  (DIO)PIN 6
 * LCD Enable pin to digital pin 9  -> altered to pin 3    <--- PIN 12(RCK)      PIN 12 (RCK/EN)  <---  (DIO)PIN 5
 * LCD D4 pin to digital pin 4                             <--- PIN 3 (Q3)       PIN 11 (CLOCK)   <---  (DIO)PIN 7
 * LCD D5 pin to digital pin 5                             <--- PIN 4 (Q4)
 * LCD D6 pin to digital pin 6                             <--- PIN 5 (Q5)
 * LCD D7 pin to digital pin 7                             <--- PIN 6 (Q6)
 * LCD R/W pin to ground
 * LCD Backlight pin to digital pin 10
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */
 
// initialize the library with the numbers of the interface pins
LiquidCrystal_SR lcd(6,5,7);

//define for backlight on main page

//---------------------------------
//THERMOSTAT DISPLAY PARAMTERS HERE
//---------------------------------


//STATIC DATA STORED IN FLASH IN THERMOSTAT_BUILD_(MAIN)


//*NOTE MOST GLOBAL VARIABLES ARE IN MAIN HEADER FILE

//local parameters for Keypad
DFR_Key keypad;
int localKey = 0;

//-------------------------
//FUNCTIONS FOR LCD DISPLAY
//-------------------------

void setup_lcd() {
  
  pinMode(backlight, OUTPUT);
  digitalWrite(backlight, LOW);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16,2);
  
  keypad.setRate(8);
}

void clear_line(int line_num)
{
  lcd.setCursor(0, line_num);
  for(byte i = 0; i < 16; ++i)
    lcd.print(" ");
  
}

//function for reading buttons and navigating through options
void read_button()
{
  localKey = keypad.getKey();

  switch(localKey)
  {
    case 1: //SYSTEM MODE KEY PRESSED 
    {
        if(system_status == 3) //system status BEFORE key pressed
          system_status = 0;
        else
          system_status++;
             
        //updates made to FAN based on system status change
        if(system_status == 0) //if system status is off then fan is also off
        {
          fan_status = 0;
          AC = false;
          HEAT = false;
        }
        
        if(system_status == 2 || system_status == 3) //if auto OR eff is set for system then auto is set for fan
          fan_status = 2;
       
        if(system_status != prev_system_status)
        { 
          request_temperature = 73; //set the request temperature to a default state again
          AC = false;
          HEAT = false;
        }        
    }
      break;
    case 2: //FAN KEY 
    {
        if(system_status == 0) //if system status is off then fan is also off
            fan_status = 0;
        if(system_status == 2 || system_status == 3) //if auto OR eff is set for system then fan is set to auto mode
            fan_status = 2;
        if(system_status == 1)//for manual modes 
            fan_status = !fan_status;
     }
       break;
     case 3: //UP KEY PRESSED (Raise Setpoint)  OR if MANU MODE set A/C On/Off
     {
        if(system_status == 1) //if system is in manual mode set A/C On/Off
        {
          AC = !AC;
          
          clear_line(1);
          lcd.setCursor(0,1);
          
          if(AC)  //DISPLAY AC ON IF IN MANU MODE - LINE 2
          {
             strcpy_P(buffer, (char*)pgm_read_word(&string_table[31]));
             lcd.print(buffer);
             lcd.setCursor(4,1);
             strcpy_P(buffer, (char*)pgm_read_word(&string_table[20]));
             lcd.print(buffer);
          }
          if(!AC)  //DISPLAY AC OFF IF IN MANU MODE - LINE 2
          {
             strcpy_P(buffer, (char*)pgm_read_word(&string_table[31]));
             lcd.print(buffer);
             lcd.setCursor(4,1);
             strcpy_P(buffer, (char*)pgm_read_word(&string_table[15]));
             lcd.print(buffer);
          }
          delay(1000);
        }
        else
        {
          if(request_temperature != 85)
            request_temperature++;
        }
     }
        break;
        
     case 4: //DOWN KEY PRESSED (Lower Setpoint) OR if MANU MODE set HEAT On/Off
     {
         if(system_status == 1) //if system is in manual mode set Heat On/Off
         {
           HEAT = !HEAT;
           
           clear_line(1);
           lcd.setCursor(0,1);
           
           if(HEAT) //DISPLAY HEAT ON IF IN MANU MODE - LINE 2
           {
             strcpy_P(buffer, (char*)pgm_read_word(&string_table[32]));
             lcd.print(buffer);
             lcd.setCursor(5,1);
             strcpy_P(buffer, (char*)pgm_read_word(&string_table[20]));
             lcd.print(buffer);
           }
           if(!HEAT) //DISPLAY HEAT OFF IF IN MANU MODE -LINE 2
           {
             strcpy_P(buffer, (char*)pgm_read_word(&string_table[32]));
             lcd.print(buffer);
             lcd.setCursor(5,1);
             strcpy_P(buffer, (char*)pgm_read_word(&string_table[15]));
             lcd.print(buffer);
           }
           delay(1000);
         }
         else  
         {
           if(request_temperature != 52)
             request_temperature--;
         } 
     }
        break;
        
     case 5: //RIGHT KEY PRESSED (MENU ADVANCE)
     {
        if(display_mode == 4)
          display_mode = 0;
        else
          display_mode++;
          
        clear_line(1);
     }
      break;
       
      
   }
  delay(200); 
}

void set_display()
{
  lcd.setCursor(0, 0);
  //lcd.print("SYS:");
  strcpy_P(buffer, (char*)pgm_read_word(&string_table[14]));
  lcd.print(buffer);
  
  lcd.setCursor(4, 0);
  switch(system_status)
  {
    case 0:  //DISPLAY "OFF"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[15]));
          lcd.print(buffer);
          break;
    case 1:  //DISPLAY "MAN"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[17]));
          lcd.print(buffer);
          break;
    case 2:  //DISPLAY "AUT" 
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[16]));
          lcd.print(buffer);
          break;
    case 3:  //DISPLAY "EFF"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[18]));
          lcd.print(buffer);
          break;
  }
  lcd.setCursor(9, 0);
  
  //lcd.print("FAN:");
  strcpy_P(buffer, (char*)pgm_read_word(&string_table[19]));
  lcd.print(buffer);
  
  lcd.setCursor(13, 0);
  
  switch(fan_status)
  {
    case 0: //DISPLAY "OFF"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[15]));
          lcd.print(buffer);   
          break;
    case 1: //DISPLAY "ON "
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[20]));
          lcd.print(buffer);
          break;
    case 2: //DISPLAY "AUT"
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[16]));
          lcd.print(buffer);
          break;
   }
  
   lcd.setCursor(0,1);
  
//display mode for second line: 0-Indoor Temperature & Humidity,
//                              1-Indoor Temperature & Outdoor Temperature
//                              2-PPM/CF(RAW) & Air Quality
//                              3-Barometric Pressure & Weather Forcast
  if(request_temperature == prev_request)  
  {
    //display_mode BEFORE key pressed
    switch(display_mode) //first 13 columns (0-12) available for use
    {
      case 0:
      {
        //DISPLAY DATE AND TIME (LOCAL?)
        //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
        //  | S Y S : A U T     F  A  N  :  A  U  T |
        //  |_______________________________________|
        //  | D A Y   X X / X X       X  X  :  X  X |   //Display Day of week, Month and day of month,  then hour and minute
        //  |_______________________________________|
        lcd.print(DOW_);     //hardcoded for now but update  
        lcd.setCursor(4,1);
        lcd.print(MONTH_);
        lcd.setCursor(8,1);
        if(day() < 10)
          lcd.print('0'); 
        lcd.print(day());
        lcd.setCursor(11,1);
        if(isPM())
          lcd.print(12 + hour());
        else
        {
          if(hour() < 10)
            lcd.print('0');
          lcd.print(hour());
        }
        lcd.print(":");
        if(minute() < 10)
          lcd.print('0');
        lcd.print(minute());
      }
      break;
      
      case 1:
      {
        //DISPLAY INSIDE TEMP AND HUMIDITY
        //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
        //  | S Y S : A U T     F  A  N  :  A  U  T |
        //  |________ ______________________________|
        //  | I N : x x x * F   H  U  M  :  x  x  % |
        //  |_______________________________________|
        lcd.print("IN:");
        lcd.setCursor(3,1);
        if(indoor_temp > 99)
          lcd.print(indoor_temp);
        else
        {
          lcd.print(indoor_temp);
          lcd.print(" ");        
        }
        lcd.setCursor(6,1);
        lcd.print((char)223);
        lcd.setCursor(7,1);
        lcd.print("F ");
        lcd.setCursor(9,1);
        lcd.print("HUM:");
        lcd.setCursor(13,1);
        lcd.print(indoor_humidity);
        lcd.setCursor(15,1);
        lcd.print("%");
      }
        break;
      case 2:
      {
        //DISPLAY INSIDE AND OUTSIDE TEMPS
        //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
        //  | S Y S : A U T     F  A  N  :  A  U  T |
        //  |________ ______________________________|
        //  | I : x x x * F     O  :  x  x  x  *  F |
        //  |_______________________________________|
        lcd.print("I:");
        lcd.setCursor(2,1);
        if(indoor_temp > 100)
          lcd.print(indoor_temp);
        else
        {
          lcd.print(indoor_temp);
          lcd.print(" ");        
        }
        lcd.setCursor(5,1);
        lcd.print((char)223);
        lcd.setCursor(6,1);
        lcd.print("F ");
        lcd.setCursor(9,1);
        lcd.print("O:");
        lcd.setCursor(11,1);
        if(metar_table[0].temperature > 99)
          lcd.print((int)metar_table[0].temperature);
        else
        {
          lcd.print((int)metar_table[0].temperature);
          lcd.print(" ");        
        }
        lcd.setCursor(14,1);
        lcd.print((char)223);
        lcd.setCursor(15,1);
        lcd.print("F");
        
       }
        break;
      case 3:
      {
        //DISPLAY AQI
        //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
        //  | S Y S : A U T     F  A  N  :  A  U  T|
        //  |________ _____________________________|
        //  | P P M : x x x   A Q  I  :  G  O  O  D|
        //  |______________________________________|
        if((second() % 15) == 0)
        {
          swap = !swap;
          clear_line(1);
        }
          
        lcd.setCursor(0,1);
        
        if(swap)
        {
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[25]));
          lcd.print(buffer);
          lcd.setCursor(5,1);
          lcd.print(ppm_val);
        }
        else
        {
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[26]));
          lcd.print(buffer);
          
          if(ppm_val > 100)
          {
            strcpy_P(buffer, (char*)pgm_read_word(&string_table[29]));
            lcd.print(buffer);
          }
          else if(ppm_val > 50)
          {
            strcpy_P(buffer, (char*)pgm_read_word(&string_table[28]));
            lcd.print(buffer);
          }
          else
          {
            strcpy_P(buffer, (char*)pgm_read_word(&string_table[27]));
            lcd.print(buffer);
          }
        }
      }
        break;
      case 4:
      {
        //DISPLAY BAROMETRIC PRESSURE AND WEATHER
        //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
        //  | S Y S : A U T     F  A  N  :  A  U  T|
        //  |________ _____________________________|
        //  | P : x x x x m b      C  L  E  A  R   |  
        //  |______________________________________|
        lcd.print("P:");
        lcd.setCursor(2,1);
        if(metar_table[0].temperature > 999)
          lcd.print(metar_table[0].pressure);
        else
        {
          lcd.print(metar_table[0].pressure);
          lcd.print(" ");        
        }
        lcd.setCursor(6,1);
        lcd.print("mb");
	lcd.setCursor(8,1);
		if(press_change == 0)
			lcd.print("- ");
		if(press_change == 1)
			lcd.print("+ ");
		if(press_change == 2)
			lcd.print("  ");
        lcd.setCursor(10,1);  
        //conditions 0-CLEAR, 1-CHANGE, 2-STORMY 
        if(cond == 0) //PRINTS "CLEAR"
        {
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[22]));
          lcd.print(buffer);
        }
        if(cond == 1) //PRINTS "CHANGE"
        {
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[23]));
          lcd.print(buffer);
        }
        if(cond == 2) //PRINTS "STORMY"
        {
          strcpy_P(buffer, (char*)pgm_read_word(&string_table[21]));
          lcd.print(buffer);
        }
      }
        break;
    }
  }
  else 
  {
    //DISPLAY SET TEMPERATURE
    //    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
    //  | S Y S : A U T     F  A  N  :  A  U  T|
    //  |______________________________________|
    //  | T E M P   S E T :    x  x  *  F      |  
    //  |______________________________________|
    strcpy_P(buffer, (char*)pgm_read_word(&string_table[24]));
    lcd.print(buffer);
    lcd.setCursor(9,1);
    lcd.print(request_temperature);
    lcd.setCursor(11,1);
    lcd.print((char)223);
    lcd.setCursor(12,1);
    lcd.print("F   ");
    delay(1000);
    prev_request = request_temperature;
    clear_line(1);
  }
  
}

void lcd_loop()
{
  set_display();
  read_button();
}

