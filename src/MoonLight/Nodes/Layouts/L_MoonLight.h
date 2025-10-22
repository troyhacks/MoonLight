/**
    @title     MoonLight
    @file      L_MoonLight.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONLIGHT

class HumanSizedCubeLayout: public Node {
  public:

  static const char * name() {return "Human Sized Cube 🚥🧊💫";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint8_t width = 10;
  uint8_t height = 10;
  uint8_t depth = 10;
  // bool snake;
  char pins[20] = "12,13,14,15,16"; //minimal 5, 1 for each side.

  void setup() override {
    addControl(width, "width", "range", 1, 20);
    addControl(height, "height", "range", 1, 20);
    addControl(depth, "depth", "range", 1, 20);
    // addControl(snake, "snake", "checkbox");
    addControl(pins, "pins", "text", 1, sizeof(pins));
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override {

    char *nextPin = pins;

    //front: z = 0
    for (int x = 0; x<width; x++) for (int y = 0; y<height; y++) addLight(Coord3D(x+1, y+1, 0));
    addNextPin(nextPin);

    //back: z = depth+1
    for (int x = 0; x<width; x++) for (int y = 0; y<height; y++) addLight(Coord3D(x+1, y+1, depth+1));
    addNextPin(nextPin);

    //above: y = 0
    for (int x = 0; x<width; x++) for (int z = 0; z<depth; z++) addLight(Coord3D(x+1, 0, z+1));
    addNextPin(nextPin);

    // //below: y = height+1
    // for (int x = 0; x<width; x++) for (int z = 0; z<depth; z++) addLight(Coord3D(x+1, height+1, z+1));
    // addNextPin(nextPin);

    //left: x = 0
    for (int z = 0; z<depth; z++) for (int y = 0; y<height; y++) addLight(Coord3D(0, y+1, z+1));
    addNextPin(nextPin);

    //right: x = width+1
    for (int z = 0; z<depth; z++) for (int y = 0; y<height; y++) addLight(Coord3D(width+1, y+1, z+1));
    addNextPin(nextPin);
  }
};

struct Wiring {
  uint16_t size[3] = {16, 16, 16}; //panels of 16x16
  uint8_t wiringOrder = 1; //first Y, then X, then Z
  bool inc[3] = {true, true}; //left, top, front
  bool snake[3] = {false, true, false}; //snake on Y
  uint8_t* axes;

  void iterate(int axis, int snaker, std::function<void(uint16_t)> fun) {
    bool iInc = inc[axes[axis]];
    if (snake[axis] && snaker%2==1) iInc = !iInc; //reverse order for snake
    for (int j = iInc? 0 : size[axes[axis]]-1 ; iInc? j < size[axes[axis]] : j>=0 ; j+= iInc? 1 : -1) {
      fun(j);
    } 
  }
};

//PanelLayout is a simplified version of PanelsLayout (only one panel)
class PanelLayout: public Node {
  public:

  static const char * name() {return "Panel 🚥";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  Wiring panel = {{16,16,1}, 1, {true,true,true}, {false,true,false}};; // 16x16 panel, increasing over the axis, snake on the Y-axis

  uint8_t  pin = 16;

  void setup() override {
    JsonObject property;
    JsonArray values;

    addControl(panel.size[0], "panelWidth", "number", 1, 65536);
    addControl(panel.size[1], "panelHeight", "number", 1, 65536);
    property = addControl(panel.wiringOrder, "wiringOrder", "select"); values = property["values"].to<JsonArray>(); values.add("XY"); values.add("YX");
    addControl(panel.inc[0], "X++", "checkbox"); addControl(panel.inc[1], "Y++", "checkbox"); 
    addControl(panel.snake[1], "snake", "checkbox");

    addControl(pin, "pin", "pin", 1, SOC_GPIO_PIN_COUNT);
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override {

    uint8_t axisOrders[2][2] = {
      {1, 0}, // Y(1) outer loop, X(0) inner loop
      {0, 1}, // X(0) outer loop, Y(1) inner loop
    };

    panel.axes = axisOrders[panel.wiringOrder]; //choose one of the orders
    panel.iterate(0, 0, [&](uint16_t i) {
      panel.iterate(1, i, [&](uint16_t j) {
        int coords[2];
        coords[panel.axes[0]] = i;
        coords[panel.axes[1]] = j;
        addLight(Coord3D(coords[0], coords[1]));
      });
    });

    addPin(pin);
  }

};

class PanelsLayout: public Node {
  public:

  static const char * name() {return "Panels 🚥";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  Wiring panels = {{2,2,1}, 1, {true,true,true}, {false,true,false}}; // 2x2 panels, increasing over the axis, snake on the Y-axis
  Wiring panel = {{16,16,1}, 1, {true,true,true}, {false,true,false}};; // 16x16 panel, increasing over the axis, snake on the Y-axis

  #if CONFIG_IDF_TARGET_ESP32
    // char pins[80] = "22,21,14,18,5,4,2,15,13,12"; //Cube202020 (D0)
    char pins[80] = "2,3,4,16,17,18,19,21,22,23,25,26,27,32,33"; //(D0), more pins possible. to do: complete list.
  #else
    char pins[80] = "47,48,21,38,14,39,1,13,40,12,41,11,42,10,2,3"; //SE16 pins (S3)
  #endif

  void setup() override {
    JsonObject property;
    JsonArray values;

    addControl(panels.size[0], "horizontalPanels", "number", 1, 32);
    addControl(panels.size[1], "verticalPanels", "number", 1, 32);
    property = addControl(panels.wiringOrder, "wiringOrderP", "select"); values = property["values"].to<JsonArray>(); values.add("XY"); values.add("YX");
    addControl(panels.inc[0], "X++P", "checkbox"); addControl(panels.inc[1], "Y++P", "checkbox"); 
    addControl(panels.snake[1], "snakeP", "checkbox");

    addControl(panel.size[0], "panelWidth", "number", 1, 65536);
    addControl(panel.size[1], "panelHeight", "number", 1, 65536);
    property = addControl(panel.wiringOrder, "wiringOrder", "select"); values = property["values"].to<JsonArray>(); values.add("XY"); values.add("YX");
    addControl(panel.inc[0], "X++", "checkbox"); addControl(panel.inc[1], "Y++", "checkbox"); 
    addControl(panel.snake[1], "snake", "checkbox");

    addControl(pins, "pins", "text", 1, sizeof(pins));
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override {

    char *nextPin = pins;

    uint8_t axisOrders[2][2] = {
      {1, 0}, // Y(1) outer loop, X(0) inner loop
      {0, 1}, // X(0) outer loop, Y(1) inner loop
    };

    panels.axes = axisOrders[panels.wiringOrder]; //choose one of the orders
    panels.iterate(0, 0, [&](uint16_t a) {
      panels.iterate(1, a, [&](uint16_t b) {
        int coordsP[2];
        coordsP[panels.axes[0]] = a;
        coordsP[panels.axes[1]] = b;

        panel.axes = axisOrders[panel.wiringOrder]; //choose one of the orders
        panel.iterate(0, 0, [&](uint16_t i) {
          panel.iterate(1, i, [&](uint16_t j) {
            int coords[2];
            coords[panel.axes[0]] = i;
            coords[panel.axes[1]] = j;
            addLight(Coord3D(coordsP[0] * panel.size[0] + coords[0], coordsP[1] * panel.size[1] + coords[1]));
          });
        });

        addNextPin(nextPin);

      });
    });

  }
};

class CubeLayout: public Node {
  public:

  static const char * name() {return "Cube 🚥🧊";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  Wiring panels = {{10,10,10}, 3, {true,true,true}, {false,true,false}};; // 16x16 panel, increasing over the axis, snake on the Y-axis

  char pins[20] = "12,13,14,15,16,17";

  void setup() override {
    addControl(panels.size[0], "width", "number", 1, 128);
    addControl(panels.size[1], "height", "number", 1, 128);
    addControl(panels.size[2], "depth", "number", 1, 128);
    JsonObject property;
    JsonArray values;
    property = addControl(panels.wiringOrder, "wiringOrder", "select"); 
    values = property["values"].to<JsonArray>();
    values.add("XYZ");
    values.add("YXZ");
    values.add("XZY");
    values.add("YZX");
    values.add("ZXY");
    values.add("ZYX");
    addControl(panels.inc[0], "X++", "checkbox"); 
    addControl(panels.inc[1], "Y++", "checkbox"); 
    addControl(panels.inc[2], "Z++", "checkbox"); 
    addControl(panels.snake[0], "snakeX", "checkbox");
    addControl(panels.snake[1], "snakeY", "checkbox");
    addControl(panels.snake[2], "snakeZ", "checkbox");
    addControl(pins, "pins", "text", 1, sizeof(pins));
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override {
    uint8_t axisOrders[6][3] = {
      {2, 1, 0}, // Z (outer), Y (middle), X (inner) -- X fastest
      {2, 0, 1}, // Z, X, Y
      {1, 2, 0}, // Y, Z, X
      {1, 0, 2}, // Y, X, Z
      {0, 2, 1}, // X, Z, Y
      {0, 1, 2}  // X, Y, Z
    };
    char *nextPin = pins;

    panels.axes = axisOrders[panels.wiringOrder]; //choose one of the orders
    panels.iterate(0, 0, [&](uint16_t i) {
      panels.iterate(1, i, [&](uint16_t j) {
        panels.iterate(2, j, [&](uint16_t k) {
          int coords[3];
          coords[panels.axes[0]] = i;
          coords[panels.axes[1]] = j;
          coords[panels.axes[2]] = k;
          addLight(Coord3D(coords[0], coords[1], coords[2]));
        });
      });

      addNextPin(nextPin);
    });

  }

};

class SingleLineLayout: public Node {
  public:

  static const char * name() {return "Single Line 🚥";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  uint8_t start_x = 0;
  uint8_t width = 30;
  uint16_t yposition = 0;
  uint8_t pin = 16;
  bool reversed_order = false;

  void setup() override {
    addControl(start_x, "starting X", "range", 0, 255);
    addControl(width, "width", "range", 1, 255);
    addControl(yposition, "Y position", "number", 0, 255); 
    addControl(reversed_order, "reversed order", "checkbox");
    addControl(pin, "pin", "pin", 1, SOC_GPIO_PIN_COUNT);
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override {
    if (reversed_order){
      for (int x = start_x+width-1; x>=start_x; x--) {
        addLight(Coord3D(x, yposition, 0));
      }
    }
    else {
      for (int x = start_x; x<start_x+width; x++) {
        addLight(Coord3D(x, yposition, 0));
      }
    }
    addPin(pin);
  }

};

class SingleRowLayout: public Node {
  public:

  static const char * name() {return "Single Row 🚥";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  uint8_t start_y = 0;
  uint8_t height = 30;
  uint16_t xposition = 0;
  uint8_t pin = 16;
  bool reversed_order = false;

  void setup() override {
    addControl(start_y, "starting Y", "range", 0, 255);
    addControl(height, "height", "range", 1, 255);
    addControl(xposition, "X position", "number", 0, 255); 
    addControl(reversed_order, "reversed order", "checkbox");
    addControl(pin, "pin", "pin", 1, SOC_GPIO_PIN_COUNT);
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override {
    if (reversed_order){
      for (int y = start_y+height-1; y>=start_y; y--) {
        addLight(Coord3D(xposition, y, 0));
      }
    }
    else {
      for (int y = start_y; y<start_y+height; y++) {
        addLight(Coord3D(xposition, y, 0));
      }
    }
    addPin(pin);
  }
};

class RingsLayout: public Node {
  public:

  static const char * name() {return "Rings 🚥";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  uint8_t pin = 16;

  uint8_t width = 16;
  uint8_t height = 16;
  
  void setup() override {
    addControl(pin, "pin", "pin", 1, SOC_GPIO_PIN_COUNT);
  }

  void add(int leds, int radius) {

    for (int i = 0; i<leds; i++) {
      uint8_t x = width / 2.0;
      uint8_t y = height / 2.0;
      if (leds != 1) {
        x = width / 2.0 + ((sin8(255 * i / (leds-1)) - 127) / 127.0) * radius / 10.0;
        y = height / 2.0 + ((cos8(255 * i / (leds-1)) - 127) / 127.0) * radius / 10.0;
      }
      addLight({x, y, 0});
    }
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override{
    
    add(1, 0);
    add(8, 13);
    add(12, 23);
    add(16, 33);
    add(24, 43);
    add(32, 53);
    add(40, 63);
    add(48, 73);
    add(60, 83);

    addPin(pin);
  }
};

class WheelLayout: public Node {
  public:

  static const char * name() {return "Wheel 🚥";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  uint8_t pin = 16;

  uint8_t nrOfSpokes = 12;
  uint8_t ledsPerSpoke = 16;
  
  void setup() override {
    addControl(nrOfSpokes, "nrOfSpokes", "range", 1, 48);
    addControl(ledsPerSpoke, "ledsPerSpoke", "range", 1, 255);
    addControl(pin, "pin", "pin", 1, SOC_GPIO_PIN_COUNT);
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override {
    // uint16_t size = ledsPerSpoke * 2;
    Coord3D middle;
    middle.x = ledsPerSpoke;
    middle.y = ledsPerSpoke;
    middle.z = 0; // size >> 1; this is for later 🔥
    for (int i=0; i<nrOfSpokes; i++) {
      float radians = i*360/nrOfSpokes * DEG_TO_RAD;
      for (int j=0; j<ledsPerSpoke; j++) {
        float radius = j + 1;
        float x = radius * sinf(radians);
        float y = radius * cosf(radians);
        addLight(Coord3D(x + middle.x, y + middle.y, middle.z));
      }
    }
    addPin(pin);
  }
};

#endif //FT_MOONLIGHT