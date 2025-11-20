uint8_t height = 16;

void setup() {
  addControl(&height, "height", "slider", 1, 255);
}

void addStrip( uint8_t xposition, uint8_t start_y,  uint8_t stop_y) {
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
  nextPin(); //each strip it's own pin
}

void onLayout() {
  addStrip(7, height, 2*height-1); addStrip(7, height-1, 0);
  addStrip(6, height, 2*height-1); addStrip(6, height-1, 0);
  addStrip(5, height, 2*height-1); addStrip(5, height-1, 0);
  addStrip(4, height, 2*height-1); addStrip(4, height-1, 0);
  addStrip(3, height, 2*height-1); addStrip(3, height-1, 0);
  addStrip(2, height, 2*height-1); addStrip(2, height-1, 0);
  addStrip(1, height, 2*height-1); addStrip(1, height-1, 0);
  addStrip(0, height, 2*height-1);  addStrip(0, height-1, 0);
}