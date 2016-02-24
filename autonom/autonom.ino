#define BT_STATE A1
#define PWM_A 3  // PWM control for motor outputs 1 and 2 
#define PWM_B 9  // PWM control for motor outputs 3 and 4 
#define DIR_A 2  // direction control for motor outputs 1 and 2 
#define DIR_B 8  // direction control for motor outputs 3 and 4 

#define LEFT 128   // rotate steering motor left
#define RIGHT -128 // rotate steering motor right
#define MEASURE_LOOP_COUNT 8
// left sensor
#define HC04_LE_TRIGGER A10
#define HC04_LE_ECHO A11

// center sensor
#define HC04_CE_TRIGGER A8
#define HC04_CE_ECHO A9

// right sensor
#define HC04_RI_TRIGGER A2
#define HC04_RI_ECHO A3

#define LIM_DISTANCE_HI 100
#define LIM_DISTANCE_LO 40

int measureDistance(uint8_t triggerPort, uint8_t echoPort )
{
  //delayMicroseconds(2);
  digitalWrite(triggerPort, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPort, LOW);
  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  int duration = pulseIn(echoPort, HIGH, 3000);
  
  // convert the time into a distance
  //inches = microsecondsToInches(duration);
  int cm = microsecondsToCentimeters(duration);
  //Serial1.println(cm, DEC);
  if (0 == cm)
  {
    cm = LIM_DISTANCE_HI;
  }
  return cm;
}

int microsecondsToInches(int microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second). This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.

  return microseconds / 74 / 2;

}

int microsecondsToCentimeters(int microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled. 
  
  return microseconds / 29 / 2;

}
 
void setup()
{
  // configure motor control pins  
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);

  // bring motor control in a safe (stop) state
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
  digitalWrite(DIR_A, LOW); 
  digitalWrite(DIR_B, LOW);  

  // setup left ultrasonic sensor
  pinMode(HC04_LE_TRIGGER, OUTPUT);
  pinMode(HC04_LE_ECHO, INPUT);
  
  // setup center ultrasonic sensor
  pinMode(HC04_CE_TRIGGER, OUTPUT);
  pinMode(HC04_CE_ECHO, INPUT);

  // setup right ultrasonic sensor
  pinMode(HC04_RI_TRIGGER, OUTPUT);
  pinMode(HC04_RI_ECHO, INPUT);

  // default baud rate BT interface
  Serial1.begin(9600);
  while (!Serial1) {
    ; // wait for Serial1 port to connect. Needed for native USB port only
  }
}

// enum for driving states
typedef enum state
{
  stopped,
  forward_driving,
  reverse_driving,
  rotate_right,
  rotate_left
} state_t;


// global variables
int speed = 0;
int direction = 0;
bool autonomous = false; 
String str;
state_t driving_state = stopped;
int counter = 0;


void loop()
{   
   char zeichen;
   
   static int distanceLeft = 0;
   static int distanceRight = 0;
   static int distanceCenter = 0;

   distanceLeft   += measureDistance(HC04_LE_TRIGGER, HC04_LE_ECHO);
   distanceRight  += measureDistance(HC04_RI_TRIGGER, HC04_RI_ECHO);
   distanceCenter += measureDistance(HC04_CE_TRIGGER, HC04_CE_ECHO);

   if(0 == (++counter % MEASURE_LOOP_COUNT))
   {
     counter = 0;
     
     distanceLeft = distanceLeft / MEASURE_LOOP_COUNT;
     distanceRight = distanceRight / MEASURE_LOOP_COUNT;
     distanceCenter = distanceCenter / MEASURE_LOOP_COUNT;

     Serial1.print("L / C / R / spd / dir: \t");
     Serial1.print(distanceLeft);
     Serial1.print("\t");
     Serial1.print(distanceCenter);
     Serial1.print("\t");
     Serial1.print(distanceRight);
     Serial1.print("\t");
     Serial1.print(speed);
     Serial1.print("\t");
     Serial1.print(direction);
     Serial1.print("\r\n");

     if(autonomous)
     {
       // find the minimum distance for all three sensors
       int dmin = min(distanceLeft,min(distanceRight,distanceCenter));
  
       // the closer the minimum distance is, the slower we drive
       int vFwd = 255 - ((LIM_DISTANCE_HI - dmin) * 2);
  
       // the close we are to an obstacle, the faster we rotate
       int vRot = 255 - vFwd;
       if (vRot)
       {
         vRot = max(vRot, 128);
       }
  
       // find the rotation direction
       int dir  = distanceLeft - distanceRight;
       if (dir<0) dir = -1; else dir = 1;
       speed = vFwd;
       direction = vRot * dir;
     }
     else
     {
       driving_state = stopped;
     }

     // reset value for next measurement cycle
     distanceLeft = 0;
     distanceRight = 0;
     distanceCenter = 0;
   }
   
   // serial character available
   if (Serial1.available() > 0) {
    zeichen = Serial1.read();
    
    switch (zeichen)
    {
      case 'w':
      speed += 8;
      break;

      case 's':
      direction = 0;
      break;

      case 'x':
      speed -= 8;
      break;

      case 'a':
      direction += 8;
      break;

      case 'd':
      direction -= 8;
      break;
      
      case ' ':
      direction = 0;
      speed = 0;
      autonomous = false;
      break;

      case 'm':
      autonomous = !autonomous;
      if(autonomous)
      {
        Serial1.print("auto ON\r\n");
        direction = 0;
        speed = 160;
      }        
      else
      {
        Serial1.print("auto OFF\r\n");
        direction = 0;
        speed = 0;
      }
      break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      str.concat(zeichen);
      break;
      
      case '\n':
      case '\r':
       speed = str.toInt();
       str = "";
       break;
      
      default:
      break;
    }

    // limit speed to +/- 255
    if (speed > 255)
    {
      speed = 255;
    }
    
    if (speed < -255)
    {
      speed = -255;
    }

    // limit direction to +/- 255
    if (direction > 255)
    {
      direction = 255;
    }
    
    if (direction < -255)
    {
      direction = -255;
    }
    
    Serial1.print("Speed: ");
    Serial1.print(speed);
    Serial1.print("\r\n");

    Serial1.print("direction: ");
    Serial1.print(direction);
    Serial1.print("\r\n");
  }

  // set the new speed and direction parameters
  // this is done on every loop run
  if (speed >= 0)
  {
    digitalWrite(DIR_A, LOW);
  } 
  else
  {
    digitalWrite(DIR_A, HIGH);
  }
  analogWrite(PWM_A, abs(speed));

  if (direction >= 0)
  {
    digitalWrite(DIR_B, LOW);
  } 
  else
  {
    digitalWrite(DIR_B, HIGH);
  }
  analogWrite(PWM_B, abs(direction));

  // limit the loop rate to not "overload" the sensors
  delay(60);     
}

