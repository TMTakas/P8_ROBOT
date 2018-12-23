#include <SoftwareSerial.h>
SoftwareSerial GPRS(10, 11); // RX, TX
enum StepObject {
  started,
  init0,
  init1,
  init2,
  post0,
  post1,
  exec,
  readr,
  readr2,
  execCmd
};
StepObject Step = started;
String HttpReturn = "";
//String ExecCmd = "";

bool CheckIfErrorHappend()
{
  
}

void setup()
{
  GPRS.begin(19200);
  Serial.begin(19200);
  
  delay(5000);
  
}

void loop()
{
  if(Step == started)
  {
    GPRS.println("AT+SAPBR=3,1,\"APN\",\"orange\"");
    delay(1000);
    Step = init0;
  }
  else if(Step == init0)
  {
    while(GPRS.available()) 
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
          HttpReturn.trim();
          Serial.println(HttpReturn);
          if(HttpReturn == "OK")
          {
            Serial.println("Step Started Finished");
            GPRS.println("AT+SAPBR=1,1");
            //delay(1000);
            Step = init1;
          }
          HttpReturn = "";
      }
    } 
  }
  else if(Step == init1)
  {  
   while(GPRS.available()) 
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
          HttpReturn.trim();
          Serial.println(HttpReturn);
          if(HttpReturn == "OK" || HttpReturn == "ERROR")
          {
            Serial.println("Step init0 Finished");
            GPRS.println("AT+HTTPINIT");
            Step = init2;
          }
          HttpReturn = "";
      }
    } 
  }
  else if(Step == init2)
  {
     while(GPRS.available()) 
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
          HttpReturn.trim();
          Serial.println(HttpReturn);
          if(HttpReturn == "OK" || HttpReturn == "ERROR")
          {
            Serial.println("Step init1 Finished");
             GPRS.println("AT+HTTPPARA=\"CID\",1");
            Step = post0;
          }
          HttpReturn = "";
      }
    } 
  }
  else if(Step == post0)
  {
     while(GPRS.available()) 
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
          HttpReturn.trim();
          Serial.println(HttpReturn);
          if(HttpReturn == "OK" || HttpReturn == "ERROR")
          {
            Serial.println("Step init2 Finished");
             GPRS.println("AT+HTTPPARA=\"URL\",\"http://xxx.xxx/robot/get_order.php?lastcommande=0&temperature=0&humidity=0\"");
            Step = post1;
          }
          HttpReturn = "";
      }
    } 
  }
  else if(Step == post1)
  {
     while(GPRS.available()) 
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
          HttpReturn.trim();
          Serial.println(HttpReturn);
          if(HttpReturn == "OK")
          {
            Serial.println("Step post0 Finished");
             GPRS.println("AT+HTTPACTION=0");
             delay(5000);
            Step = exec;
          }
          HttpReturn = "";
      }
    } 
  }
  else if(Step == exec)
  {
     while(GPRS.available()) 
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
          HttpReturn.trim();
          Serial.println(HttpReturn);
          if(HttpReturn == "OK")
          {
            Serial.println("Step exec Finished");
            // GPRS.println("AT+HTTPREAD=0,100");
            Step = readr;
          }
          HttpReturn = "";
      }
    } 
  }
  else if(Step == readr)
  {
     while(GPRS.available()) 
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
          HttpReturn.trim();
          Serial.println(HttpReturn);
          if(HttpReturn.indexOf("HTT") > 0)
          {
            Serial.println("Reading Content ...");
            delay(500);
            GPRS.println("AT+HTTPREAD=0,100");
            delay(5000);
            Step = readr2;
          }
          HttpReturn = "";
      }
    } 
  }
  else if(Step == readr2)
  {
     while(GPRS.available()) 
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      Serial.println(c);
      /*if(c == '\n')
      {
          HttpReturn.trim();
          Serial.println(HttpReturn);
          if(HttpReturn == "NoCmd" || HttpReturn.indexOf("|") > 0)
          {
            Serial.println("Your Content Is : ");
            //Serial.println(HttpReturn.indexOf("HTTPREAD"));
            Serial.println(HttpReturn);
            // GPRS.println("AT+HTTPREAD=0,100");
            Step = execCmd;
            delay(1000);
          }
          //HttpReturn = "";
      }*/
    } 
  } 
  else if(Step == execCmd)
  {
    Serial.println("Cmd Exeuting !!");
    /*if(HttpReturn[0] == 'M')
    {
      String tmp = HttpReturn.substring(HttpReturn.indexOf('|'),HttpReturn.lastIndexOf('|'));
      if(tmp == "F")
      {
        Serial.println("Moving Forward");
      }
    }*/
    //Step = started;
    delay(2000);
  }
    /*while (Serial.available()) {
    byte b = Serial.read();
    if ( b == '*')
      GPRS.write( 0x1a );         // replace * with ctrl+z
    else
      GPRS.write(b);
    }*/
}

/*
// If you want to replace asterisks with ctrl+Z you can
// replace the second part above with this instead

  while (Serial.available()) {
    byte b = Serial.read();
    if ( b == '*')
      GPRS.write( 0x1a );         // replace * with ctrl+z
    else
      GPRS.write(b);
  }

*/
