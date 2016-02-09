#define BT_STATE A1
#define PWM_A 3  // PWM control for motor outputs 1 and 2 
#define PWM_B 9  // PWM control for motor outputs 3 and 4 
#define DIR_A 2  // direction control for motor outputs 1 and 2 
#define DIR_B 8  // direction control for motor outputs 3 and 4 

#define LEFT 255   // rotate steering motor left
#define RIGHT -255 // rotate steering motor right

// left sensor
#define HC04_LE_TRIGGER A10
#define HC04_LE_ECHO A11

// center sensor
#define HC04_CE_TRIGGER A8
#define HC04_CE_ECHO A9

// right sensor
#define HC04_RI_TRIGGER A12
#define HC04_RI_ECHO A13

#define STOP_DISTANCE 40

int measureDistance(uint8_t triggerPort, uint8_t echoPort )
{
  //delayMicroseconds(2);
  digitalWrite(triggerPort, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPort, LOW);
  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  int duration = pulseIn(echoPort, HIGH, 10000);
  
  // convert the time into a distance
  //inches = microsecondsToInches(duration);
  int cm = microsecondsToCentimeters(duration);
  //Serial1.println(cm, DEC);
  if (0 == cm)
  {
    cm = 1000;
  }
  return cm;
}

long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second). This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.

  return microseconds / 74 / 2;

}

long microsecondsToCentimeters(long microseconds)
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

void loop()
{   
   char zeichen;
   
   int distanceLeft   = measureDistance(HC04_LE_TRIGGER, HC04_LE_ECHO);
   int distanceRight  = measureDistance(HC04_RI_TRIGGER, HC04_RI_ECHO);
   int distanceCenter = measureDistance(HC04_CE_TRIGGER, HC04_CE_ECHO);

   //int mediumDistance = (distanceLeft + distanceRight + distanceCenter) / 3;
   
   Serial1.print("L / R / C / M: \t");
   Serial1.print(distanceLeft);
   Serial1.print("\t");
   Serial1.print(distanceRight);
   Serial1.print("\t");
   Serial1.print(distanceCenter);
   //Serial1.print("\t");
   //Serial1.print(mediumDistance);
   Serial1.print("\r\n");

   if(autonomous)
   {
     switch (driving_state)
     {
       case stopped:
         speed = 0;
         direction = 0;
         driving_state = forward_driving;
       break;

       case forward_driving:
        // full speed forward
        speed = 255;
        direction = 0;

        // check for obstacles
        if ((distanceLeft < STOP_DISTANCE) || (distanceRight < STOP_DISTANCE) || (distanceCenter < STOP_DISTANCE))
        {
          if (distanceLeft <= distanceRight)
          {
            // obstacle in the left --> turn right
            driving_state = rotate_right;
          }
          else
          {
            // obstacle in the right --> turn left
            driving_state = rotate_left;
          }
        }
       break;

       case rotate_right:
         speed = 0;
         direction = RIGHT;
         if (distanceLeft > STOP_DISTANCE)
         {
           // turned away from the obstacle --> forward
           driving_state = forward_driving;
         }
       
       break;

       case rotate_left:
         speed = 0;
         direction = LEFT;
         if (distanceRight > STOP_DISTANCE)
         {
           // turned away from the obstacle --> forward
           driving_state = forward_driving;
         }
       break;

       // no reverse driving at the moment
       case reverse_driving:
       // no break
       default:
       // this should never happen! stop the machine! leave auto mode!
       speed = 0;
       direction = 0;
       driving_state = stopped;
       autonomous = false;
       break;
     }     
   }
   else
   {
     driving_state = stopped;
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

