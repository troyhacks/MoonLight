#define PI 3.1415926535
int nb_branches;
uint8_t C_X;
uint8_t C_Y;
uint8_t mapp;
uint8_t rMapRadius[NUM_LEDS];
uint8_t rMapAngle[NUM_LEDS];
uint32_t t = 0; //=speed

uint8_t speed;
uint8_t branches;

void setup()
{
  addControl(&speed, "speed", "range", 128, 1, 255);
  addControl(&branches, "branches", "range", 4, 1, 8);

  C_X = width / 2;
  C_Y = height / 2;
  mapp = 255 / width;
  for (int x = -C_X; x < C_X + (width % 2); x++) {
    for (int y = -C_Y; y < C_Y + (height % 2); y++) {

      float h=128*(atan2(y, x)/PI);
      rMapAngle[(x + C_X) *height+y + C_Y]= (int)(h);
      h=hypot(x,y);
      rMapRadius[(x + C_X)*height +y + C_Y] = (int)(h); //thanks Sutaburosu
    }
  }
}


void loop() {

  for (uint8_t x = 0; x < width; x++) {
    for (uint8_t y = 0; y < height; y++) {
      uint8_t angle = rMapAngle[x*height+y];
      uint8_t radius = rMapRadius[x*height+y];
      uint16_t intensity = sin8(sin8((angle * 4 - radius*mapp) / 4 + t) + radius*mapp - 2*t + angle * branches);

      setRGB(y*width+x, hsv(2*t - radius*mapp, 255, intensity));
    }
  }
  t = now() * speed / 32 / 25; //speed 0..8, 40 changes per second
}