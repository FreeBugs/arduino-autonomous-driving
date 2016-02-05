int pwm_a = 3;  //PWM control for motor outputs 1 and 2 
int pwm_b = 9;  //PWM control for motor outputs 3 and 4 
int dir_a = 2;  //direction control for motor outputs 1 and 2 
int dir_b = 8;  //direction control for motor outputs 3 and 4 
#define RECHTS LOW
byte leistung = 255;
int dauer = 2000;

void setup()
{
  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
  digitalWrite(dir_a, LOW); 
  digitalWrite(dir_b, LOW);

  // gerade
  analogWrite(pwm_a, leistung);
  analogWrite(pwm_b, 0);
  delay(dauer);

  // links
  analogWrite(pwm_a, leistung);
  digitalWrite(dir_b, LOW);
  analogWrite(pwm_b, leistung);
  delay(dauer);

  // rechts
  analogWrite(pwm_a, leistung);
  digitalWrite(dir_b, HIGH);
  analogWrite(pwm_b, leistung);
  delay(dauer);
  
  // drehen
  analogWrite(pwm_a, 0);
  digitalWrite(dir_b, HIGH);
  analogWrite(pwm_b, leistung);
  delay(dauer);

   // stop
  analogWrite(pwm_a, 0);
  digitalWrite(dir_b, HIGH);
  analogWrite(pwm_b, 0);
  delay(dauer);

}

void loop()
{
  delay(10000);
}

