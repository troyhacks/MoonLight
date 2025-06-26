/**
    @title     MoonLight
    @file      Modifiers.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONLIGHT

//alphabetically from here

//Takes the x dimension from the layout (1D effect) and turn it into a circle in 2D or a sphere in 3D.
class CircleModifier: public Node {
  public:

  static const char * name() {return "Circle 💎💡";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  void setup() override {
    hasModifier = true;
  }

  Coord3D originalSize;

  void modifyLayout() override {
    originalSize = layerV->size;

    modifyLight(layerV->size); //modify the virtual size as x, 0, 0

    // change the size to be one bigger in each dimension
    layerV->size.x++;
    layerV->size.y++;
    layerV->size.z++;
  }

  void modifyLight(Coord3D &position) override {
    //calculate the distance from the center
    int dx = position.x - originalSize.x / 2;
    int dy = position.y - originalSize.y / 2;
    int dz = position.z - originalSize.z / 2; 

    // Calculate the distance from the center
    float distance = sqrt(dx * dx + dy * dy + dz * dz);

    position.x = distance;
    position.y = 0;
    position.z = 0;
  }
};

class MirrorModifier: public Node {
  public:

  static const char * name() {return "Mirror 💎💡";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  bool mirrorX;
  bool mirrorY;
  bool mirrorZ;
  
  void setup() override {
    hasModifier = true;
    addControl(&mirrorX, "mirrorX", "checkbox", true);
    addControl(&mirrorY, "mirrorY", "checkbox", false);
    addControl(&mirrorZ, "mirrorZ", "checkbox", false);
  }

  Coord3D originalSize;
  
  void modifyLayout() override {
    if (mirrorX) layerV->size.x = (layerV->size.x + 1) / 2;
    if (mirrorY) layerV->size.y = (layerV->size.y + 1) / 2;
    if (mirrorZ) layerV->size.z = (layerV->size.z + 1) / 2;
    originalSize = layerV->size;
    ESP_LOGD(TAG, "mirror %d %d %d", layerV->size.x, layerV->size.y, layerV->size.z);
  }

  void modifyLight(Coord3D &position) override {
    if (mirrorX && position.x >= originalSize.x) position.x = originalSize.x * 2 - 1 - position.x;
    if (mirrorY && position.y >= originalSize.y) position.y = originalSize.y * 2 - 1 - position.y;
    if (mirrorZ && position.z >= originalSize.z) position.z = originalSize.z * 2 - 1 - position.z;
  }
};

class MultiplyModifier: public Node {
  public:

  static const char * name() {return "Multiply 💎💡";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  Coord3D proMulti = {2,2,2};
  bool    mirror = false;
  Coord3D originalSize;

  void setup() override {
    hasModifier = true;
  }

  void modifyLayout() override {
    layerV->size = (layerV->size + proMulti - Coord3D({1,1,1})) / proMulti; // Round up
    originalSize = layerV->size;
    ESP_LOGD(TAG, "multiply %d %d %d", layerV->size.x, layerV->size.y, layerV->size.z);
  }

  void modifyLight(Coord3D &position) override {
    if (mirror) {
      Coord3D mirrors = position / originalSize; // Place the light in the right quadrant
      position = position % originalSize;
      if (mirrors.x %2 != 0) position.x = originalSize.x - 1 - position.x;
      if (mirrors.y %2 != 0) position.y = originalSize.y - 1 - position.y;
      if (mirrors.z %2 != 0) position.z = originalSize.z - 1 - position.z;
    }
    else position = position % originalSize;
  }
};

class PinwheelModifier: public Node {
  public:

  static const char * name() {return "Pinwheel 💎💡";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  uint8_t petals;
  uint8_t swirlVal;
  bool    reverse;
  uint8_t symmetry;
  uint8_t zTwist;

  void setup() override {
    hasModifier = true;
    addControl(&petals, "petals", "range", 60);
    addControl(&swirlVal, "swirlVal", "range", 30);
    addControl(&reverse, "reverse", "checkbox", false);
    addControl(&symmetry, "symmetry", "range", 1);
    addControl(&zTwist, "zTwist", "range", 0);
  }
  
  void modifyLayout() override {
    // if (leds.projectionDimension > _1D && leds.effectDimension > _1D) {
      layerV->size.y = sqrt(sq(max<uint8_t>(layerV->size.x - layerV->middle.x, layerV->middle.x)) + 
                            sq(max<uint8_t>(layerV->size.y - layerV->middle.y, layerV->middle.y))) + 1; // Adjust y before x
      layerV->size.x = petals;
      layerV->size.z = 1;
    // }
    // else {
    //   layerV->size.x = petals;
    //   layerV->size.y = 1;
    //   layerV->size.z = 1;
    // }
    ESP_LOGD(TAG, "Pinwheel %d %d %d", layerV->size.x, layerV->size.y, layerV->size.z);
  }

  void modifyLight(Coord3D &position) override {
    // Coord3D mapped;
    // factors of 360
    const int FACTORS[24] = {360, 180, 120, 90, 72, 60, 45, 40, 36, 30, 24, 20, 18, 15, 12, 10, 9, 8, 6, 5, 4, 3, 2};
    // UI Variables
         
    const int dx = position.x - layerV->middle.x;
    const int dy = position.y - layerV->middle.y;
    const int swirlFactor = swirlVal == 0 ? 0 : hypot(dy, dx) * abs(swirlVal); // Only calculate if swirlVal != 0
    int angle = degrees(atan2(dy, dx)) + 180;  // 0 - 360
    
    if (swirlVal < 0) angle = 360 - angle; // Reverse Swirl

    int value = angle + swirlFactor + (zTwist * position.z);
    float petalWidth = symmetry / float(petals);
    value /= petalWidth;
    value %= petals;

    if (reverse) value = petals - value - 1; // Reverse Movement

    position.x = value;
    position.y = 0;
    // if (leds.effectDimension > _1D && leds.projectionDimension > _1D) {
      position.y = int(sqrt(sq(dx) + sq(dy))); // Round produced blank position
    // }
    position.z = 0;

    // if (position.x == 0 && position.y == 0 && position.z == 0) ppf("Pinwheel  Center: (%d, %d) SwirlVal: %d Symmetry: %d Petals: %d zTwist: %d\n", layerV->middle.x, layerV->middle.y, swirlVal, symmetry, petals, zTwist);
    // ppf("position %2d,%2d,%2d -> %2d,%2d,%2d Angle: %3d Petal: %2d\n", position.x, position.y, position.z, mapped.x, mapped.y, mapped.z, angle, value);
  }
};

// RotateNodifier rotates the light position around the center of the layout.
// It can flip the x and y coordinates, reverse the rotation direction, and alternate the rotation
// direction every full rotation. It also supports shear transformations to create a more dynamic effect.
// by WildCats08
class RotateNodifier: public Node {
  public:

  static const char * name() {return "Rotate 💎💫";}
  static const char * tags() {return "";}

  struct RotateData { // 16 bytes
    union {
      struct {
        bool flip : 1;
        bool reverse : 1;
        bool alternate : 1;
      };
      uint8_t flags;
    };
    uint8_t  midX;
    uint8_t  midY;
    uint16_t angle;
    int16_t  shearX;
    int16_t  shearY;
    unsigned long lastUpdate; // last millis() update
  };

  public:

  RotateData data;

  uint8_t direction;
  uint8_t rotateSpeed;
  bool expand;

  void setup() override {

    hasModifier = true;

    JsonObject property = addControl(&direction, "direction", "select", 0);
    JsonArray values = property["values"].to<JsonArray>();
    values.add("Clockwise");
    values.add("Counter-Clockwise");
    values.add("Alternate");

    addControl(&rotateSpeed, "rotateSpeed", "range", 128); 
    addControl(&expand, "expand", "checkbox", false);

  }

  void modifyLayout() override {

    if (expand) {
      uint8_t size = max(layerV->size.x, max(layerV->size.y, layerV->size.z));
      size = sqrt(size * size * 2) + 1;
      Coord3D offset = {(size - layerV->size.x) / 2, (size - layerV->size.y) / 2, 0};

      layerV->size = Coord3D{size, size, 1};
    }

    data.midX = layerV->size.x / 2;
    data.midY = layerV->size.y / 2;
  }

  void modifyLight(Coord3D &position) override {

    if (expand) {
      int size = max(layerV->size.x, max(layerV->size.y, layerV->size.z));
      size = sqrt(size * size * 2) + 1;
      Coord3D offset = {(size - layerV->size.x) / 2, (size - layerV->size.y) / 2, 0};

      position.x += offset.x;
      position.y += offset.y;
      position.z += offset.z;
    }
  }

  void modifyXYZ(Coord3D &position) override {

    constexpr int Fixed_Scale = 1 << 10;

    if ((millis() - data.lastUpdate > 1000 / (rotateSpeed + 1)) && rotateSpeed) { // Only update if the angle has changed
      data.lastUpdate = millis();

      if (direction == 0) data.reverse = false;
      if (direction == 1) data.reverse = true;
      if (direction == 2) data.alternate = true; else data.alternate = false;

      // Increment the angle
      data.angle = data.reverse ? (data.angle <= 0 ? 359 : data.angle - 1) : (data.angle >= 359 ? 0 : data.angle + 1);
      
      if (data.alternate && (data.angle == 0)) data.reverse = !data.reverse;

      data.flip = (data.angle > 90 && data.angle < 270);

      int newAngle = data.angle; // Flip newAngle if needed. Don't change angle in data
      if (data.flip) {newAngle += 180; newAngle %= 360;}

      // Calculate shearX and shearY
      float angleRadians = radians(newAngle);
      data.shearX = -tan(angleRadians / 2) * Fixed_Scale;
      data.shearY =  sin(angleRadians)     * Fixed_Scale;
    }

    int maxX = layerV->size.x;
    int maxY = layerV->size.y;

    if (data.flip) {
      // Reverse x and y values
      position.x = maxX - position.x;
      position.y = maxY - position.y;
    }

    // Translate position to origin
    int dx = position.x - data.midX;
    int dy = position.y - data.midY;

    // Apply the 3 shear transformations
    int x1 = dx + data.shearX * dy / Fixed_Scale;
    int y1 = dy + data.shearY * x1 / Fixed_Scale;
    int x2 = x1 + data.shearX * y1 / Fixed_Scale;

    // Translate position back and assign
    position.x = x2 + data.midX;
    position.y = y1 + data.midY;
    position.z = 0;

    // Clamp the position to the bounds
    if      (position.x < 0)     position.x = 0;
    else if (position.x >= maxX) position.x = maxX - 1;
    if      (position.y < 0)     position.y = 0;
    else if (position.y >= maxY) position.y = maxY - 1;
  }
}; //RotateNodifier

#endif