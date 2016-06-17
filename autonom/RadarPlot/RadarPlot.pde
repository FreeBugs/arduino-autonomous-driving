import controlP5.*;
ControlP5 cp5;
Knob directionWidget;
import processing.serial.*;

Serial myPort;        // The serial port
float distLeft = 0.0;
float distRight = 0.0;
float distCenter = 0.0;
float speed =0.0;
float direction =0.0;


int widgetSize = 300;

void setup()
{
  background (0);
  size(700, 700);

  cp5 = new ControlP5(this);

  // List all the available serial ports
  // if using Processing 2.1 or later, use Serial.printArray()
  println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  Arduino, so I open Serial.list()[0].
  // Open whatever port is the one you're using.
  myPort = new Serial(this, Serial.list()[3], 9600);

  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');

  directionWidget = cp5.addKnob("Direction")
    .setPosition(450, 300)
    .setRadius(50)
    .setScrollSensitivity(0.001)
    .setMin(-255)
    .setMax(255)
    .setViewStyle(Controller.LINE )
    .showTickMarks()
    ;
};

void draw()
{


  if (direction > 0) directionWidget.setColorForeground(color(255, 255, 0)); 
  else directionWidget.setColorForeground(color(255, 0, 0));

  directionWidget.setValue(direction);

  drawDistances (random(300), random(300), random(300));
  //drawDistances (distLeft, distCenter, distRight);

  delay(50);
};

void drawDistances(float left, float center, float right)
{
  fill(0);

  rect(100, 100, 100+widgetSize, widgetSize);

  plotRadar(100, 100, left, -30, #0000FF);

  plotRadar(110, 100, center, 0, #00FF00);

  plotRadar(120, 100, right, 30, #FF0000);
};

void plotRadar(int topx, int topy, float radarDistance_cm, float angle_deg, int cl )
{


  PImage img = loadImage("HC-SR04-radiation-plot.JPG");

  // image(img, topx, topy, widgetSize, widgetSize);

  fill(cl, 150);

  // precise measurement up to 8 feet = 243 cm
  // 1.05 * widgetSize is the 8 ' area
  // float radarDistance_pixels = map(min(243, radarDistance_cm), 0, 243, 0, 1.05*widgetSize); 

  float radarDistance_pixels = map(min(100, radarDistance_cm), 0, 100, 0, 1.05*widgetSize); 
  //println(radarDistance_pixels);

  arc(topx + widgetSize/2, topy + widgetSize, 
    radarDistance_pixels, radarDistance_pixels, 
    1.5*PI+radians(angle_deg)-0.4, 1.5*PI+radians(angle_deg)+0.4);
}

void serialEvent (Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');

  if (inString != null) {
    // trim off any whitespace:
    inString = trim(inString);
    String[] list = split(inString, ' ');
    // parse the input
    switch (list[0])
    {
    case "L":
      distLeft = float(list[1]);
      break;

    case "R":
      distRight = float(list[1]);
      break;

    case "C":
      distCenter = float(list[1]);
      break;

    case "S":
      speed = float(list[1]);
      break;

    case "D":
      direction = float(list[1]);
      break;

    default:
      // just ignore it
      break;
    }

    print("L ", distLeft);
    print("  C ", distCenter);
    print("  R ", distRight);
    print("  speed ", speed);
    println("  direction ", direction);

    //inByte = map(inByte, 0, 100, 0, height);
  }
}