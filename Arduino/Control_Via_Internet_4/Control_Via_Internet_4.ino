#include <SoftwareSerial.h> // Bibliothèque utilisé pour communication Serial
#include <dht.h> // Bibliothèque utilisé pour le capteur d'humidité et température
#include "Grove_I2C_Motor_Driver.h" // Bibliothèque utilisé pour le "Motor Controller"
#define I2C_ADDRESS 0x0f // Variable de type I2C_ADDRESS pour définir l'adresse de "Motor Controller"
SoftwareSerial GPRS(10, 11); // Objet GPRS pour communiquer avec le module GSM, il faut utilisé les pins 10 et 11(RX et TX) sur arduino mega !!
dht DHT; // Objet de Type DHT pour utilisé le capteur d'humidité et température
#define DHT11_PIN 8 // Pin pour récupérer la température et l'humidité
enum StepObject { // Un enum pour définir l'etape de l'execution du code
  SetAPN,
  SetSAPBR,
  SetINIT,
  SetCID,
  SetURL,
  SetACTION,
  SetREAD,
  SetExec
};
StepObject Step = SetAPN; // Objet de notre type StepObject
long StartedAt = millis(); // Un variable qui contient le temps depuis l'exécution du programme
String HttpReturn = ""; // Un variable contient la valeur récupéré depuis le module GSM
bool ExecNext = true; // Un variable de type bool ==> true : on peut executer la commande la configuration suivante
bool CommandExecuting = false; // Un variable de type bool ==> true : le robot est entrain d'executer une commande
bool GoingBackWard = false; // Un variable de type bool ==> true : le robot il récule
int LastCommandeDone = 0; // Identifiant de la dernière commande
const int trigPin = 3; // Trigger Pin de DHT
const int echoPin = 2; // Echo Pin de DHT
const int MAX_DIS = 20; // Max distance de sécurité en CM
int ResetTimes = 0; // Combien de fois le robot a relancé le code
long duration; // le temps du mouvement
int distance; // distance entre le robot et l'obstacle
int RedPin = 7; // pin rouge de LED
int GreenPin = 6; // pin vert de LED
int BluePin = 5; // pin blue de LED

void setColor(int redValue, int greenValue, int blueValue) // Fonction pour changer la couleur de la LED
{
  analogWrite(RedPin, redValue); // Modifier le rouge
  analogWrite(GreenPin, greenValue); // Modifier le vert
  analogWrite(BluePin, blueValue); // Modifier le blue
}

void setup() // Fonction pricipale
{
  Motor.begin(I2C_ADDRESS); // Ouvrir la conexion Serial avec le contrôleur des moteurs
  GPRS.begin(19200); // Ouvrir la conexion Serial avec le module GSM
  Serial.begin(19200); // Ouvrir la conexion Serial avec le Serial Monitor
  pinMode(trigPin, OUTPUT); // trigPin comme sortie
  pinMode(echoPin, INPUT); // echoPin comme entré 
  pinMode(RedPin, OUTPUT); // RedPin comme sortie
  pinMode(GreenPin, OUTPUT); // GreenPin comme sortie
  pinMode(BluePin, OUTPUT); // BluePin comme sortie
  delay(2000); // un retard de 2 secondes
  Serial.println("In the loop !"); // fonction setup terminé
}

void loop()
{
  if(millis() - StartedAt > 4000 && !CommandExecuting) // Si la robot a pris plus que 4 seconde on relance la demande
  {
    ResetTimes++;
    if(ResetTimes >= 3) // Si on a ralancé 3 fois on va refaire la configuration du module GSM
     Step = SetAPN;
    Serial.println("Reset !!");
    StartedAt = millis();
    ExecNext = true;
    
  }
  if(Step == SetAPN) // Etape 1
  {
    setColor(255, 0, 0); // changer le couleur de la LED en rouge
    Motor.stop(MOTOR1); // Arréter les moteurs a droite
    Motor.stop(MOTOR2); // Arréter les moteurs a gauche
    if(ExecNext) // Si ExecNext est true en va executer la commande de configuration
    {
       GPRS.println("AT+SAPBR=3,1,\"APN\",\"orange\"");
       ExecNext = false; // Changer ExecNext en false
    }
    while(GPRS.available())  // Si le module GSM nous a envoyé quelque chose
    {
      char c = GPRS.read(); // Lire le char suivant
      HttpReturn.concat(c); // Concatiner le c avec le String HttpReturn
      if(c == '\n') // Si c est un retoure a la ligne
      {
        HttpReturn.trim(); // on supprime les espaces
        if(HttpReturn.indexOf("OK") >= 0) // Si le String contain OK --> la commande est bien executé
        {
          Serial.println("Step SetAPN Done !");
          StartedAt = millis();
          ResetTimes = 0;
          Step = SetSAPBR; // la prochaine etape
          ExecNext = true; // chnager la valeur de ExecNext en true
          HttpReturn = ""; // vider le variable HttpReturn
          return;
        }
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetSAPBR) // Etape 2
  {
    setColor(255, 255, 255); // Coleur blanc
    if(ExecNext)
    {
      GPRS.println("AT+SAPBR=1,1");
      ExecNext = false;
    }
    while(GPRS.available())
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
        HttpReturn.trim();
        if(HttpReturn.indexOf("OK") || HttpReturn.indexOf("ERROR"))
        {
          Serial.println("Step SetSAPBR Done !");
          StartedAt = millis();
          ResetTimes = 0;
          Step = SetINIT;
          ExecNext = true;
          HttpReturn = "";
          return;
        }
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetINIT) // Etape 3
  {
    if(ExecNext)
    {
      GPRS.println("AT+HTTPINIT");
      ExecNext = false;
    }
    while(GPRS.available())
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
        HttpReturn.trim();
        if(HttpReturn.indexOf("OK") || HttpReturn.indexOf("ERROR"))
        {
          Serial.println("Step SetINIT Done !");
          StartedAt = millis();
          ResetTimes = 0;
          Step = SetCID;
          ExecNext = true;
          HttpReturn = "";
          return;
        }
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetCID) // Etape 4
  {
    if(ExecNext)
    {
      GPRS.println("AT+HTTPPARA=\"CID\",1");
      ExecNext = false;
    }
    while(GPRS.available())
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
        HttpReturn.trim();
        if(HttpReturn.indexOf("ERROR") > 0 || HttpReturn.indexOf("OK") > 0)
        {
          Serial.println("Step SetCID Done !");
          StartedAt = millis();
          ResetTimes = 0;
          Step = SetURL;
          ExecNext = true;
          HttpReturn = "";
          return;
        }
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetURL) // Etape 5
  {
    setColor(170, 0, 255); // Coleur Violet
    if(ExecNext)
    {
      CommandExecuting = false;
      int chk = DHT.read11(DHT11_PIN);
      String URL = "";
      URL.concat("AT+HTTPPARA=\"URL\",\"http://xxxxxxxxxx.xxxx/robot/get_order.php?lastcommande="); // Le serveur web pour récupérer la commande
      URL.concat(String(LastCommandeDone)); // Concaténer le numéro de la dernière commande executé
      URL.concat("&temperature=" + String(DHT.temperature)); // Concaténer la temperature
      URL.concat("&humidity=" + String(DHT.humidity)); // Concaténer la humidity
      URL.concat("\"");
      GPRS.println(URL); // Envoyer le lien au module GSM
      LastCommandeDone = 0;
      ExecNext = false;
    }
    while(GPRS.available())
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(HttpReturn.indexOf("ERROR") > 0 || HttpReturn.indexOf("OK") > 0 || HttpReturn.indexOf("URL") || HttpReturn.indexOf("orientra"))
      {
        HttpReturn.trim();
        Serial.println("Step SetURL Done !");
        StartedAt = millis();
        ResetTimes = 0;
        Step = SetACTION;
        ExecNext = true;
        HttpReturn = "";
        return;
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetACTION) // Etape 6
  {
    setColor(0, 0, 255); // Chnager la coleur de la LED en blue
    if(ExecNext)
    {
      GPRS.println("AT+HTTPACTION=0"); // Executer la requette avec la methode GET ==> 0 = GET ===> 1 = POST
      ExecNext = false;
    }
    while(GPRS.available())
    {
      char c = GPRS.read();
      HttpReturn.concat(c);
      if(c == '\n')
      {
        HttpReturn.trim();
        if(HttpReturn.indexOf("OK") > 0 || HttpReturn.indexOf("+HTT") > 0 || HttpReturn.indexOf(',') > 0)
        {
          Serial.println("Step SetACTION Done !");
          StartedAt = millis();
          ResetTimes = 0;
          Step = SetREAD;
          ExecNext = true;
          HttpReturn = "";
          return;
        }
      }
    }
    HttpReturn = "";
  }
  else if(Step == SetREAD) // Etape 7
  {
    if(ExecNext)
    {
      GPRS.println("AT+HTTPREAD=0,200");
      ExecNext = false;
    }
    HttpReturn = GPRS.readString();
    if(HttpReturn.indexOf('|') > 0 || HttpReturn.indexOf("NoCmd") > 0)
    {
      HttpReturn = HttpReturn.substring(HttpReturn.indexOf('|') + 1,HttpReturn.lastIndexOf('|'));
      Step = SetExec;
      ExecNext = true;
    }
  }
  else if(Step == SetExec)
  {
    bool Executing = false;
    Serial.println(HttpReturn);
    setColor(0, 255, 0); // Coleur Vert
    if(HttpReturn.indexOf("NoCmd") > 0)
    {
      Serial.println("No Commande To Execute !");
      Step = SetURL;
      StartedAt = millis();
      ResetTimes = 0;
      HttpReturn = "";
      delay(3000);
    }
    else if(HttpReturn.indexOf('M') > 0)
    {
      // Format : |Numero de la commande|Action|Direction|la vitesse|le temps
      //Exemple ===> |124|M|F|100|6000|
      ResetTimes = 0;
      LastCommandeDone = HttpReturn.substring(0,HttpReturn.indexOf('|')).toInt(); // Numéro de la commande
      HttpReturn.remove(0, HttpReturn.indexOf('|') + 1); // Supprimer le numéro de la commande donc le résultat est ===> |M|F|100|6000|
      String Cmd = HttpReturn.substring(0,HttpReturn.indexOf('|')); // Réupirer l'action
      HttpReturn.remove(0, HttpReturn.indexOf('|') + 1); // résultat est ===> |F|100|6000|
      CommandExecuting = true;
      GoingBackWard = false;
      if(Cmd == "M")
      {
        String Dirrec = HttpReturn.substring(0,HttpReturn.indexOf('|')); // Direction
        HttpReturn.remove(0, HttpReturn.indexOf('|') + 1); // résultat est ===> |100|6000|
        int MotorSpeed = HttpReturn.substring(0,HttpReturn.indexOf('|')).toInt(); // la vitesse
        HttpReturn.remove(0, HttpReturn.indexOf('|') + 1); // résultat est ===> |6000|
        // Execution de commande !!
        char tmp[sizeof(HttpReturn)]; 
        HttpReturn.toCharArray(tmp, sizeof(tmp));
        long Time = atol(tmp); 
        Serial.println("Motor Will Move : " + Dirrec);
        Serial.println("Speed : " + String(MotorSpeed));
        Serial.println("Time : " + String(Time));
        StartedAt = millis();
        long StartedExecutingAt = millis();
        if(Dirrec == "F")
        {
          Motor.speed(MOTOR1, MotorSpeed);
          Motor.speed(MOTOR2, MotorSpeed);
        }
        else if(Dirrec == "B")
        {
          Motor.speed(MOTOR1, -MotorSpeed);
          Motor.speed(MOTOR2, -MotorSpeed);
          GoingBackWard = true;
        }
        else if(Dirrec == "RF")
        {
          Motor.speed(MOTOR1, 100);
          delay(1000);
          Motor.stop(MOTOR1);
          delay(500);
          StartedExecutingAt = millis();
          Motor.speed(MOTOR1, MotorSpeed);
          Motor.speed(MOTOR2, MotorSpeed);
        }
        else if(Dirrec == "LF")
        {
          Motor.speed(MOTOR2, 100);
          delay(1000);
          Motor.stop(MOTOR2);
          delay(500);
          StartedExecutingAt = millis();
          Motor.speed(MOTOR2, MotorSpeed);
          Motor.speed(MOTOR1, MotorSpeed);
        }
        else if(Dirrec == "R")
        {
          Motor.speed(MOTOR1, 100);
          delay(2000);
          Motor.stop(MOTOR1);
          delay(500);
          StartedExecutingAt = millis();
          Motor.speed(MOTOR1, MotorSpeed);
          Motor.speed(MOTOR2, MotorSpeed);
        }
        else if(Dirrec == "L")
        {
          Motor.speed(MOTOR2, 100);
          delay(2000);
          Motor.stop(MOTOR2);
          delay(500);
          StartedExecutingAt = millis();
          Motor.speed(MOTOR2, MotorSpeed);
          Motor.speed(MOTOR1, MotorSpeed);
        }
        else if(Dirrec == "RB")
        {
          Motor.speed(MOTOR2, 100);
          delay(1000);
          Motor.stop(MOTOR2);
          delay(500);
          StartedExecutingAt = millis();
          Motor.speed(MOTOR1, -MotorSpeed);
          Motor.speed(MOTOR2, -MotorSpeed);
          GoingBackWard = true;
        }
        else if(Dirrec == "LB")
        {
          Motor.speed(MOTOR1, 100);
          delay(1000);
          Motor.stop(MOTOR1);
          delay(500);
          StartedExecutingAt = millis();
          Motor.speed(MOTOR1, -MotorSpeed);
          Motor.speed(MOTOR2, -MotorSpeed);
          GoingBackWard = true;
        }
        Executing = true;
        do
        {
          digitalWrite(trigPin, LOW);
          delayMicroseconds(2);
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
          duration = pulseIn(echoPin, HIGH);
          distance = duration*0.034/2;
          if(GoingBackWard)
            distance = MAX_DIS + 10;
        }
        while(millis() - StartedExecutingAt < Time && distance > MAX_DIS); // Si le robot trouvé un obstacle ou l'execution de la commande a terminé
      }
    }
    Motor.stop(MOTOR1); // Arreter les moteurs
    Motor.stop(MOTOR2); // Arreter les moteurs
    ResetTimes = 0;
    Step = SetURL; // Revenir à l'etape de configuration du lien
    StartedAt = millis();
    HttpReturn = "";
    Serial.println("Finished !");
    delay(1000);
  }
}
