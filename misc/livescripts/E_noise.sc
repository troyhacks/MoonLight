void loop() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      uint8_t lightHue8 = inoise8(x * intensity, y * intensity, now() / (16 - speed/16));
      // leds.setLightColor(leds.XY(x, y), ColorFromPalette(leds.palette, lightHue8));
      sCFP(y*width+x, lightHue8, 255);
    }
  }
}