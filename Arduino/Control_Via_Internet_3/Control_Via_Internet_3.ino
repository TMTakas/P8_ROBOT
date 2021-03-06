#include <SoftwareSerial.h>
#include <dht.h>
#include "Grove_I2C_Motor_Driver.h"
#define I2C_ADDRESS 0x0f
SoftwareSerial GPRS(10, 11);
dht DHT;
#define DHT11_PIN 8
enum StepObject {
  SetAPN,
  SetSAPBR,
  SetINIT,
  SetCID,
  SetURL,
  SetACTION,
  SetREAD,
  SetExec
};
StepObject Step = SetAPN;
long StartedAt = millis();
String HttpReturn = "";
bool ExecNext = true;
int LastCommandeDone = 0;
void setup()
{
  Motor.begin(I2C_ADDRESS);
  delay(1000);
  GPRS.begin(19200);
  Serial.begin(19200);
  delay(2000);
  Serial.println("loooop");
}

void loop()
{
  if(millis() - StartedAt > 30000)
  {
    Serial.println("Reset !!");
    StartedAt = millis();
    Step = SetAPN;
    ExecNext = true;
  }
  if(Step == SetAPN)
  {
    Motor.stop(MOTOR1);
    Motor.stop(MOTOR2);
    if(ExecNext)
    {
       GPRS.println("AT+SAPBR=3,1,\"APN\",\"orange\"");
       delay(500);
       ExecNext = false;
    }
    if(GPRS.available()) 
    {
      HttpReturn = GPRS.readString();
      Serial.print(HttpReturn);
      if(HttpReturn.indexOf("OK") > 0)
      {
        Step = SetSAPBR;
        ExecNext = true;
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetSAPBR)
  {
    if(ExecNext)
    {
      GPRS.println("AT+SAPBR=1,1");
      delay(500);
      ExecNext = false;
    }
    if(GPRS.available()) 
    {
      HttpReturn = GPRS.readString();
      Serial.println(HttpReturn);
      if(HttpReturn.indexOf("ERROR") > 0 || HttpReturn.indexOf("OK") > 0)
      {
        Step = SetINIT;
        ExecNext = true;
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetINIT)
  {
    if(ExecNext)
    {
      GPRS.println("AT+HTTPINIT");
      delay(500);
      ExecNext = false;
    }
    if(GPRS.available()) 
    {
      HttpReturn = GPRS.readString();
      Serial.println(HttpReturn);
      if(HttpReturn.indexOf("ERROR") > 0 || HttpReturn.indexOf("OK") > 0)
      {
        Step = SetCID;
        ExecNext = true;
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetCID)
  {
    if(ExecNext)
    {
      GPRS.println("AT+HTTPPARA=\"CID\",1");
      delay(500);
      ExecNext = false;
    }
    if(GPRS.available()) 
    {
      HttpReturn = GPRS.readString();
      Serial.println(HttpReturn);
      if(HttpReturn.indexOf("ERROR") > 0 || HttpReturn.indexOf("OK") > 0)
      {
        Step = SetURL;
        ExecNext = true;
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetURL)
  {
    if(ExecNext)
    {
      int chk = DHT.read11(DHT11_PIN);
      String URL = "";
      URL.concat("AT+HTTPPARA=\"URL\",\"http://xxx.xxx/robot/get_order.php?lastcommande=");
      URL.concat(String(LastCommandeDone));
      URL.concat("&temperature=" + String(DHT.temperature));
      URL.concat("&humidity=" + String(DHT.humidity));
      URL.concat("\"");
      Serial.println(URL);
      GPRS.println(URL);
      LastCommandeDone = 0;
      //delay(500);
      ExecNext = false;
    }
    if(GPRS.available()) 
    {
      HttpReturn = GPRS.readString();
      Serial.println(HttpReturn);
      if(HttpReturn.indexOf("ERROR") > 0 || HttpReturn.indexOf("OK") > 0)
      {
        Step = SetACTION;
        ExecNext = true;
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetACTION)
  {
    if(ExecNext)
    {
      GPRS.println("AT+HTTPACTION=0");
      delay(1000);
      ExecNext = false;
    }
    bool avariable = false;
    if(GPRS.available()) 
    {
      avariable = true;
      HttpReturn = GPRS.readString();
      Serial.println(HttpReturn);
      //if(HttpReturn.indexOf("+HTT") > 0 && HttpReturn.indexOf(',') > 0)
      //{
        Serial.println("Going to read");
        Step = SetREAD;
        ExecNext = true;
        delay(5000);
      //}
      /*else if(HttpReturn.indexOf(' ⸮') >= 0)
      {
        Serial.println("Erroooor ⸮");
         Step = SetURL;
         ExecNext = true;
      }*/
    }
    //Serial.println(HttpReturn);
    //delay(1000);
    HttpReturn = "";
  }
  else if(Step == SetREAD)
  {
    if(ExecNext)
    {
      Serial.println("Innnnn read");
      GPRS.println("AT+HTTPREAD=0,200");
      delay(500);
      ExecNext = false;
    }
    if(GPRS.available()) 
    {
      HttpReturn = GPRS.readString();
      Serial.println(HttpReturn);
      if(HttpReturn.indexOf("NoCmd") > 0 || HttpReturn.indexOf("|") > 0)
      {
        HttpReturn.remove(0,HttpReturn.indexOf('\n') + 1);
        HttpReturn.remove(0,HttpReturn.indexOf('\n') + 1);
        HttpReturn.remove(0,HttpReturn.indexOf('\n') + 1);
        HttpReturn.remove(HttpReturn.indexOf('\n'), HttpReturn.indexOf("OK") + 2);
        Serial.println("...");
        Serial.println(HttpReturn);
        Serial.println("...");
        Step = SetExec;
        ExecNext = true;
      }
    }
    //HttpReturn = "";
  }
  else if(Step == SetExec)
  {
    if(HttpReturn == "NoCmd")
    {
      Serial.println("NoCmd");
      Step = SetURL;
      StartedAt = millis();
      HttpReturn = "";
      delay(3000);
    }
    else
    {
      LastCommandeDone = HttpReturn.substring(0,HttpReturn.indexOf('|')).toInt();
      HttpReturn.remove(0, HttpReturn.indexOf('|') + 1);
      String Cmd = HttpReturn.substring(0,HttpReturn.indexOf('|'));
      HttpReturn.remove(0, HttpReturn.indexOf('|') + 1);
      if(Cmd == "M")
      {
        String Dirrec = HttpReturn.substring(0,HttpReturn.indexOf('|'));
        HttpReturn.remove(0, HttpReturn.indexOf('|') + 1);
        int MotorSpeed = HttpReturn.substring(0,HttpReturn.indexOf('|')).toInt();
        HttpReturn.remove(0, HttpReturn.indexOf('|') + 1);
        char tmp[sizeof(HttpReturn)];
        HttpReturn.toCharArray(tmp, sizeof(tmp));
        //char * tmp = String.toCharArray();
        long Time = atol(tmp); 
        Serial.println("Motor Will Move : " + Dirrec);
        Serial.println(MotorSpeed);
        Serial.println(Time);
        StartedAt = millis();
        if(Dirrec == "F")
        {
          Motor.speed(MOTOR1, MotorSpeed);
          Motor.speed(MOTOR2, MotorSpeed);
          delay(Time);
        }
        else if(Dirrec == "B")
        {
          Motor.speed(MOTOR1, -MotorSpeed);
          Motor.speed(MOTOR2, -MotorSpeed);
          delay(Time);
        }
        else if(Dirrec == "RF")
        {
          Motor.speed(MOTOR2, 100);
          delay(500);
          Motor.stop(MOTOR2);
          delay(500);
          Motor.speed(MOTOR1, MotorSpeed);
          Motor.speed(MOTOR2, MotorSpeed);
          delay(Time);
        }
        else if(Dirrec == "LF")
        {
          Motor.speed(MOTOR1, 100);
          delay(500);
          Motor.stop(MOTOR1);
          delay(500);
          Motor.speed(MOTOR1, MotorSpeed);
          Motor.speed(MOTOR2, MotorSpeed);
          delay(Time);
        }
        else if(Dirrec == "R")
        {
          Motor.speed(MOTOR2, 100);
          delay(1000);
          Motor.stop(MOTOR2);
          delay(500);
          Motor.speed(MOTOR1, MotorSpeed);
          Motor.speed(MOTOR2, MotorSpeed);
          delay(Time);
        }
        else if(Dirrec == "L")
        {
          Motor.speed(MOTOR1, 100);
          delay(1000);
          Motor.stop(MOTOR1);
          delay(500);
          Motor.speed(MOTOR1, MotorSpeed);
          Motor.speed(MOTOR2, MotorSpeed);
          delay(Time);
        }
        else if(Dirrec == "RB")
        {
          Motor.speed(MOTOR1, 100);
          delay(500);
          Motor.stop(MOTOR1);
          delay(500);
          Motor.speed(MOTOR1, -MotorSpeed);
          Motor.speed(MOTOR2, -MotorSpeed);
          delay(Time);
        }
        else if(Dirrec == "LB")
        {
          Motor.speed(MOTOR2, 100);
          delay(500);
          Motor.stop(MOTOR2);
          delay(500);
          Motor.speed(MOTOR1, -MotorSpeed);
          Motor.speed(MOTOR2, -MotorSpeed);
          delay(Time);
        }
      }
    }
    Motor.stop(MOTOR1);
    Motor.stop(MOTOR2);
    Step = SetURL;
    StartedAt = millis();
    HttpReturn = "";
    delay(3000);
  }
}
