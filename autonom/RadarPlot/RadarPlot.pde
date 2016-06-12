void setup()
{
  background (0);
  size(700, 700);

drawDistances (600,50,40);

}

void drawDistances(int left, int center, int right)
{
  plotRadar(100, 100, left, -10);

  plotRadar(200, 100, center, 0);

  plotRadar(300, 100, right, 10);
};

void plotRadar(int topx, int topy, float radarDistance_cm, float angle_deg)
{
  int widgetSize = 100;

  PImage img = loadImage("HC-SR04-radiation-plot.JPG");

  image(img, topx, topy, widgetSize, widgetSize);

  fill(#00FF00, 220);

  // precise measurement up to 8 feet = 243 cm
  // 1.05 * widgetSize is the 8 ' area
  float radarDistance_pixels = map(min(243, radarDistance_cm), 0, 243, 0, 1.05*widgetSize); 
  println(radarDistance_pixels);

  arc(topx + widgetSize/2, topy + widgetSize, 
    radarDistance_pixels, radarDistance_pixels, 
    1.5*PI+radians(angle_deg)-0.4, 1.5*PI+radians(angle_deg)+0.4);
}