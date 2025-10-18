uint8_t  horizontalPanels = 1;
uint8_t  verticalPanels = 4;
uint8_t  panelWidth = 32;
uint8_t  panelHeight = 8;
bool snake = true;
uint8_t pin = 16;

void setup() {
  addControl(&horizontalPanels, "horizontalPanels", "range", 1, 32);
  addControl(&verticalPanels, "verticalPanels", "range", 1, 32);
  addControl(&panelWidth, "panelWidth", "range", 1, 64);
  addControl(&panelHeight, "panelHeight", "range", 1, 64);
  addControl(&snake, "snake", "checkbox");
  addControl(&pin, "pin", "pin", 1, 48);
}

void onLayout() {

  for (int panelY = verticalPanels - 1; panelY >=0; panelY--) {

    for (int panelX = horizontalPanels-1; panelX >=0; panelX--) {

      for (int x=panelWidth-1; x>=0; x--) {
        for (int y=panelHeight-1; y>=0; y--) {
          int y2 = y; if (snake && x%2 == 0) {y2 = panelHeight - 1 - y;}
          addLight(panelX * panelWidth + x, panelY * panelHeight + y2, 0);
        }
      }

    }

    addPin(pin);

  }
}
