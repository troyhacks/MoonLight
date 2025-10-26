uint8_t height = 16;

void setup() {
  addControl(&height, "height", "slider", 1, 255);
}

void addStrip( uint8_t xposition, uint8_t start_y,  uint8_t stop_y, uint8_t pin) {
  if (start_y > stop_y){
    for (int y = start_y; y>=stop_y; y--) {
      addLight(xposition, y, 0);
    }
  }
  else {
    for (int y = start_y; y<=stop_y; y++) {
      addLight(xposition, y, 0);
    }
  }
  addPin(pin);
}

void onLayout() {
  addStrip(7, height, 2*height-1, 47); addStrip(7, height-1, 0, 48);
  addStrip(6, height, 2*height-1, 21); addStrip(6, height-1, 0, 38);
  addStrip(5, height, 2*height-1, 14); addStrip(5, height-1, 0, 39);
  addStrip(4, height, 2*height-1, 13); addStrip(4, height-1, 0, 40);
  addStrip(3, height, 2*height-1, 12); addStrip(3, height-1, 0, 41);
  addStrip(2, height, 2*height-1, 11); addStrip(2, height-1, 0, 42);
  addStrip(1, height, 2*height-1, 10); addStrip(1, height-1, 0, 2);
  addStrip(0, height, 2*height-1, 3);  addStrip(0, height-1, 0, 1);
}