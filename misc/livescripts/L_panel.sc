uint8_t width;
uint8_t height;
uint8_t pin;

void addControls() {
  addControl(&width, "width", "range", 16, 1, 32);
  addControl(&height, "height", "range", 16, 1, 32);
  addControl(&pin, "pin", "number", 16, 1, 48);
}

void addLayout() {
  for (int x = 0; x<width; x++)
    for (int y=0; y<height; y++)
      addLight(x,(x%2)?y:height - 1 - y,0);

  addPin(pin);
}