uint8_t speed;
uint8_t scale;

void setup() {
  addControl(&speed, "speed", "range", 128, 1, 255);
  addControl(&scale, "scale", "range", 128, 1, 255);
}

void loop() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      uint8_t lightHue8 = inoise8(x * scale, y * scale, now() / (16 - speed/16));
      // leds.setLightColor(leds.XY(x, y), ColorFromPalette(leds.palette, lightHue8));
      setLightPal(y*width+x, lightHue8, 255);
    }
  }
}