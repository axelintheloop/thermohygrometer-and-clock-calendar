#include <genieArduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <RTCDue.h>

//Sensor definitions
#define DHTPIN 2  
#define DHTTYPE DHT11   // DHT 11

RTCDue rtc(XTAL);


Genie genie;
DHT dht(DHTPIN, DHTTYPE);


char *_weekday[7] = {"Saturday","Sunday","Monday","Tuesday", "Wednesday","Thursday","Friday"};

char *Month_table[] = {"January", "February", "March",
                          "April", "May", "June", "July", "August",
                          "September", "October", "November", "December",
                          "Undecimber", "Duodecimber"};

char *zellersAlgorithm(int _day, int _month, int _year){
   int mon;
   if(_month > 2)
      mon = _month; //for march to december month code is same as month
   else{
      mon = (12+_month); //for Jan and Feb, month code will be 13 and 14
      _year--; //decrease year for month Jan and Feb
   }
   int y = _year % 100; //last two digit
   int c = _year / 100; //first two digit
   int w = (_day + floor((13*(mon+1))/5) + y + floor(y/4) + floor(c/4) + (5*c));
   w = w % 7;
   return _weekday[w];
}

int flag_day = 0;
int flag_hour = 0;
int RTC_flag = 0;

#define RESETLINE 4  // Change this if you are not using an Arduino Adaptor Shield Version 2 (see code below)

int seconds = 0;
int minutes = 0;
int hours = 0;
int days = 0;
int months = 0;
int years = 0;
    
void setup()
{
    // Use a Serial Begin and serial port of your choice in your code and use the 
    // genie.Begin function to send it to the Genie library (see this example below)
    // 200K Baud is good for most Arduinos. Galileo should use 115200.  
    // Some Arduino variants use Serial2 for the TX/RX pins, as Serial0 is for USB.
    Serial.begin(9600);  // Serial0 @ 200000 (200K) Baud
    //Serial.begin(9600);
    Serial2.begin(9600);
    genie.Begin(Serial2);   // Use Serial0 for talking to the Genie Library, and to the 4D Systems display

    genie.AttachEventHandler(myGenieEventHandler); // Attach the user function Event Handler for processing events
  
    // Reset the Display (change D4 to D2 if you have original 4D Arduino Adaptor)
    // THIS IS IMPORTANT AND CAN PREVENT OUT OF SYNC ISSUES, SLOW SPEED RESPONSE ETC
    // If NOT using a 4D Arduino Adaptor, digitalWrites must be reversed as Display Reset is Active Low, and
    // the 4D Arduino Adaptors invert this signal so must be Active High.  
    pinMode(4, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
    digitalWrite(4, 1);  // Reset the Display via D4
    delay(100);
    digitalWrite(4, 0);  // unReset the Display via D4
  
    // Let the display start up after the reset (This is important)
    // Increase to 4500 or 5000 if you have sync problems as your project gets larger. Can depent on microSD init speed.
    delay (4500); 
  
    // Set the brightness/Contrast of the Display - (Not needed but illustrates how)
    // Most Displays use 0-15 for Brightness Control, where 0 = Display OFF, though to 15 = Max Brightness ON.
    // Some displays are more basic, 1 (or higher) = Display ON, 0 = Display OFF.  
    genie.WriteContrast(12); // About 2/3 Max Brightness
    
    dht.begin();

    rtc.begin(); // initialize RTC

    tmElements_t tm;
    if( RTC.read(tm))
    {
        genie.WriteObject(GENIE_OBJ_LED, 0, 1);
        RTC_flag = 1;
        seconds = tm.Second;
        minutes = tm.Minute;
        hours = tm.Hour;
        days = tm.Day;
        months = tm.Month;
        years = tmYearToCalendar(tm.Year); 
        flag_day = tm.Day;
        flag_hour = tm.Hour;
    }
    else
    {
      genie.WriteObject(GENIE_OBJ_LED, 0, 0);
      rtc.setHours(19);
      rtc.setMinutes(0);
      rtc.setSeconds(0);

      flag_day = 30;
      flag_hour = 19;
      rtc.setDay(30);
      rtc.setMonth(12);
      rtc.setYear(2021);

      hours = rtc.getHours();
      minutes = rtc.getMinutes();
      seconds = rtc.getSeconds();
      days = rtc.getDay();
      months = rtc.getMonth();
      years = rtc.getYear();
      RTC_flag == 0;
    }

    //Write time in hours
      if(RTC_flag == 1)
      {
        if(hours < 12 && hours >= 5)
        {
          genie.WriteStr( 4, "Buenos dias \nAxelintheloop!");
        }else if(hours < 20 && hours >= 12)
        {
          genie.WriteStr( 4, "Buenas tardes \nAxelintheloop!");
        }else{
          genie.WriteStr( 4, "Buenas noches \nAxelintheloop!");
        }
      }
      else{
        if(hours < 12 && hours >= 5)
        {
          genie.WriteStr( 4, "Buenos dias \nAxelintheloop");
        }else if(hours < 20 && hours >= 12)
        {
          genie.WriteStr( 4, "Buenas tardes \nAxelintheloop");
        }else{
          genie.WriteStr( 4, "Buenas noches \nAxelintheloop");
        }
      }

      //Write weekday
      genie.WriteStr(0, zellersAlgorithm(days, months, years));
  
      //Write time in days
      genie.WriteStr( 1, days);
  
      //Write time in months
      genie.WriteStr( 2, Month_table[months - 1]);
  
      //Write time in years
      genie.WriteStr( 3, years);
  
    #define string_thermoCelcius1   0
    #define string_thermoFarenheit1 1
    #define string_thermoCelcius2   2
    #define string_thermoFarenheit2 3
    #define string_thermoCelcius13  4
    #define string_thermoFarenheit3 5
    #define string_thermoCelcius14  6
    #define string_thermoFarenheit4 7
  
    #define GENIE_OBJ_THERMOMETER           18
    #define GENIE_OBJ_ANGULAR_METER         7
    #define GENIE_OBJ_LED_DIGITS            15
    #define GENIE_OBJ_STRINGS               17
    #define GENIE_OBJ_STATIC_TEXT   21
    #define GENIE_OBJ_GAUGE         11
    #define GENIE_OBJ_USER_LED      19
  
    genie.WriteObject(GENIE_OBJ_GAUGE, 0, 60);

}

void loop()
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    float f = dht.readTemperature(true);
 
    float f_meter = 0;
    float t_meter = 0;
    float f_dig = 0;
    float t_dig = 0;

    t_meter = map(t, -20, 50, 0, 70);
    
    if(t < 0)
    {
      t_dig = abs(t);
      genie.WriteObject(GENIE_OBJ_USER_LED, 0, 1);
    }
    else{
        genie.WriteObject(GENIE_OBJ_USER_LED, 0, 0);
        t_dig = t;
    }

    f_meter = map(f, -10, 90, 0, 100);
    
    if(f < 0)
    {
      f_dig = abs(f);
      genie.WriteObject(GENIE_OBJ_USER_LED, 1, 1);
    }
    else{
        f_dig = f;
        genie.WriteObject(GENIE_OBJ_USER_LED, 1, 0);
    }

    tmElements_t tm;
    if( RTC.read(tm))
    {
        genie.WriteObject(GENIE_OBJ_LED, 0, 1);
        RTC_flag = 1;
        seconds = tm.Second;
        minutes = tm.Minute;
        hours = tm.Hour;
        days = tm.Day;
        months = tm.Month;
        years = tmYearToCalendar(tm.Year);
    }
    else
    {
      genie.WriteObject(GENIE_OBJ_LED, 0, 0);
      RTC_flag = 0;
      hours = rtc.getHours();
      minutes = rtc.getMinutes();
      seconds = rtc.getSeconds();
      days = rtc.getDay();
      months = rtc.getMonth();
      years = rtc.getYear();
    }
   
    //Write humidity
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, h);
    genie.WriteObject(GENIE_OBJ_ANGULAR_METER , 0, h);

    //Write temperature in °C
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 1, t_dig*10);
    genie.WriteObject(GENIE_OBJ_THERMOMETER, 0, t_meter);

    //Write temperature in °F
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 2, f_dig*10);
    genie.WriteObject(GENIE_OBJ_THERMOMETER, 1, f_meter);

    //Write time in minutes
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 4, minutes);

  
    if(flag_hour == hours) 
    {
      //Write time in hours
      genie.WriteObject(GENIE_OBJ_LED_DIGITS, 3, hours);

      if(RTC_flag == 1)
      {
        if(hours < 12 && hours >= 5)
        {
          genie.WriteStr( 4, "Buenos dias \nAxelintheloop!");
        }else if(hours < 20 && hours >= 12)
        {
          genie.WriteStr( 4, "Buenas tardes \nAxelintheloop!");
        }else{
          genie.WriteStr( 4, "Buenas noches \nAxelintheloop!");
        }
      }
      else{
        if(hours < 12 && hours >= 5)
        {
          genie.WriteStr( 4, "Buenos dias \nAxelintheloop");
        }else if(hours < 20 && hours >= 12)
        {
          genie.WriteStr( 4, "Buenas tardes \nAxelintheloop");
        }else{
          genie.WriteStr( 4, "Buenas noches \nAxelintheloop");
        }
      }
      flag_hour++;
    }

    if(flag_day == days)
    {
      //Write weekday
      genie.WriteStr(0, zellersAlgorithm(days, months, years));
  
      //Write time in days
      genie.WriteStr( 1, days);
  
      //Write time in months
      genie.WriteStr( 2, Month_table[months - 1]);
  
      //Write time in years
      genie.WriteStr( 3, years);

      flag_day++;
    }

    //Write time in seconds
    genie.WriteObject(GENIE_OBJ_GAUGE, 0, seconds);

    //Write time in seconds
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 5, seconds);

}

void myGenieEventHandler(void)
{
  genieFrame Event;
  genie.DequeueEvent(&Event); // Remove the next queued event from the buffer, and process it below

  int slider_val = 0;

  //If the cmd received is from a Reported Event (Events triggered from the Events tab of Workshop4 objects)
  if (Event.reportObject.cmd == GENIE_REPORT_EVENT)
  {
    if (Event.reportObject.object == GENIE_OBJ_SLIDER)                // If the Reported Message was from a Slider
    {
      if (Event.reportObject.index == 0)                              // If Slider0 (Index = 0)
      {
        slider_val = genie.GetEventData(&Event);                      // Receive the event data from the Slider0
        genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, slider_val);       // Write Slider0 value to LED Digits 0
      }
    }
  }

  //If the cmd received is from a Reported Object, which occurs if a Read Object (genie.ReadOject) is requested in the main code, reply processed here.
  if (Event.reportObject.cmd == GENIE_REPORT_OBJ)
  {
    if (Event.reportObject.object == GENIE_OBJ_USER_LED)              // If the Reported Message was from a User LED
    {
      if (Event.reportObject.index == 0)                              // If UserLed0 (Index = 0)
      {
        bool UserLed0_val = genie.GetEventData(&Event);               // Receive the event data from the UserLed0
        UserLed0_val = !UserLed0_val;                                 // Toggle the state of the User LED Variable
        genie.WriteObject(GENIE_OBJ_USER_LED, 0, UserLed0_val);       // Write UserLed0_val value back to UserLed0
      }
    }
  }
}
