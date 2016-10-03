
 // Graphing sketch


// This program takes ASCII-encoded strings
// from the serial port at 9600 baud and graphs them. It expects values in the
// range 0 to 1023, followed by a newline, or newline and carriage return

// Created 20 Apr 2005
// Updated 24 Nov 2015
// by Tom Igoe
// This example code is in the public domain.

import processing.serial.*;

Serial myPort;        // The serial port
float distLeft = 0.0;
color colLeft = #FF0000;
float distRight = 0.0;
color colRight = #00FF00;
float distCenter = 0.0;
color colCenter = #0000FF;
float speed = 0.0;
float direction = 0.0;

void setup () {
  // set the window size:
  size(800, 400);

  // List all the available serial ports
  // if using Processing 2.1 or later, use Serial.printArray()
  println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  Arduino, so I open Serial.list()[0].
  // Open whatever port is the one you're using.
  myPort = new Serial(this, Serial.list()[4], 9600);

  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');

  // set inital background:
  background(0);
}


void draw () {
drawDistances(distLeft, distCenter, distRight);
}



void drawDistances(float l, float c, float r )
{

  pushStyle();
  pushMatrix();
   
  translate(200,300);
  rotate(PI);
  
  fill(0);
  rect (0, 10, 200, 300);
  
  strokeWeight(2);
  
  fill(colRight);
  rect (0, 10, 50, r);
  
  fill(colCenter);
  rect (60, 10, 50, c);
  fill(colLeft);
  rect (120, 10, 50, l);
  popMatrix();
  popStyle();
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
    
    println("left   ", distLeft);
    println("right  ", distRight);
    println("center ", distCenter);
    println("speed  ", speed);
    println("direction ", direction);

    //inByte = map(inByte, 0, 100, 0, height);
  }
}