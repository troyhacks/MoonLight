/**
    @title     MoonLight
    @file      Modifiers.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONLIGHT

//alphabetically from here, Custom Nodes at the end

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

  void modifySize() override {
    originalSize = layerV->size;

    modifyPosition(layerV->size); //modify the virtual size as x, 0, 0

    // change the size to be one bigger in each dimension
    layerV->size.x++;
    layerV->size.y++;
    layerV->size.z++;
  }

  void modifyPosition(Coord3D &position) override {
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

  bool mirrorX = true;
  bool mirrorY = false;
  bool mirrorZ = false;
  
  void setup() override {
    hasModifier = true;
    addControl(mirrorX, "mirrorX", "checkbox");
    addControl(mirrorY, "mirrorY", "checkbox");
    addControl(mirrorZ, "mirrorZ", "checkbox");
  }

  Coord3D originalSize;
  
  void modifySize() override {
    if (mirrorX) layerV->size.x = (layerV->size.x + 1) / 2;
    if (mirrorY) layerV->size.y = (layerV->size.y + 1) / 2;
    if (mirrorZ) layerV->size.z = (layerV->size.z + 1) / 2;
    originalSize = layerV->size;
    MB_LOGV(ML_TAG, "mirror %d %d %d", layerV->size.x, layerV->size.y, layerV->size.z);
  }

  void modifyPosition(Coord3D &position) override {
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
    addControl(proMulti, "multipliers", "coord3D");
  }

  void modifySize() override {
    layerV->size = (layerV->size + proMulti - Coord3D({1,1,1})) / proMulti; // Round up
    originalSize = layerV->size;
    MB_LOGV(ML_TAG, "multiply %d %d %d", layerV->size.x, layerV->size.y, layerV->size.z);
  }

  void modifyPosition(Coord3D &position) override {
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

  uint8_t petals = 60;
  uint8_t swirlVal = 30;
  bool    reverse = false;
  uint8_t symmetry = 1;
  uint8_t zTwist = 0;
  float petalWidth = 6.0;

  Coord3D originalSize;

  void setup() override {
    hasModifier = true;
    addControl(petals, "petals", "range");
    addControl(swirlVal, "swirlVal", "range");
    addControl(reverse, "reverse", "checkbox");
    addControl(symmetry, "symmetry", "range");
    addControl(zTwist, "zTwist", "range");
  }
  
  void modifySize() override {
    if (layerV->layerDimension > _1D && layerV->effectDimension > _1D) {
      layerV->size.y = sqrt(sq(max<uint8_t>(layerV->size.x - layerV->middle.x, layerV->middle.x)) + 
                            sq(max<uint8_t>(layerV->size.y - layerV->middle.y, layerV->middle.y))) + 1; // Adjust y before x
      layerV->size.x = petals;
      layerV->size.z = 1;
    }
    else {
      layerV->size.x = petals;
      layerV->size.y = 1;
      layerV->size.z = 1;
    }
    if (petals < 1) petals = 1; // Ensure at least one petal
    const int FACTORS[23] = {360, 180, 120, 90, 72, 60, 45, 40, 36, 30, 24, 20, 18, 15, 12, 10, 9, 8, 6, 5, 4, 3, 2}; // Factors of 360
    int factor;
    if (symmetry > 23) factor = 2; // Default to 2 if symmetry is greater than 23
    else if (symmetry > 0) factor = FACTORS[symmetry - 1]; // Convert symmetry to a factor of 360
    else factor = 360; // Default to 360 if symmetry is <= 0
    petalWidth = factor / float(petals);

    originalSize = layerV->size;
    MB_LOGD(ML_TAG, "Pinwheel %d %d %d", layerV->size.x, layerV->size.y, layerV->size.z);
  }

  void modifyPosition(Coord3D &position) override {
    // Coord3D mapped;

    const int dx = position.x - layerV->middle.x;
    const int dy = position.y - layerV->middle.y;
    const int swirlFactor = swirlVal == 0 ? 0 : hypot(dy, dx) * abs(swirlVal); // Only calculate if swirlVal != 0
    int angle = degrees(atan2(dy, dx)) + 180;  // 0 - 360
    
    if (swirlVal < 0) angle = 360 - angle; // Reverse Swirl

    int value = angle + swirlFactor + (zTwist * position.z);
    value /= petalWidth;
    value %= petals;

    if (reverse) value = petals - value - 1; // Reverse Movement

    position.x = value;
    position.y = 0;
    if (layerV->effectDimension > _1D && layerV->layerDimension > _1D) {
      position.y = int(sqrt(sq(dx) + sq(dy))); // Round produced blank position
    }
    position.z = 0;

    // if (position.x == 0 && position.y == 0 && position.z == 0) MB_LOGD(ML_TAG, "Pinwheel  Center: (%d, %d) SwirlVal: %d Symmetry: %d Petals: %d zTwist: %d\n", layerV->middle.x, layerV->middle.y, swirlVal, symmetry, petals, zTwist);
    // MB_LOGD(ML_TAG, "position %2d,%2d,%2d -> %2d,%2d,%2d Angle: %3d Petal: %2d\n", position.x, position.y, position.z, mapped.x, mapped.y, mapped.z, angle, value);
  }
};

//Idea and first implementation (WLEDMM Art-Net) by @Troy
class RippleYZModifier: public Node {
  public:

  static const char * name() {return "RippleYZ 💎💡💫";}
  static const char * tags() {return "";}

  bool shrink = true;
  bool towardsY = true;
  bool towardsZ = false;

  void setup() override {

    hasModifier = true;

    addControl(shrink, "shrink", "checkbox");
    addControl(towardsY, "towardsY", "checkbox");
    addControl(towardsZ, "towardsZ", "checkbox");
  }

  void modifySize() override {
    // modifyPosition(layerV->size);
    // change the size to be one bigger in each dimension
    // layerV->size.x++;
    // layerV->size.y++;
    // layerV->size.z++;
    if (shrink) {
      if (towardsY)
        layerV->size.y = 1;
      if (towardsZ)
        layerV->size.z = 1;
    }
  }

  void modifyPosition(Coord3D &position) override {
    if (shrink) {
      if (towardsY)
        position.y = 0;
      if (towardsZ)
        position.z = 0;
    }
  }

  void loop() override {

    //1D->2D: each X is rippled through the y-axis
    if (towardsY) {
      if (layerV->effectDimension == _1D && layerV->layerDimension > _1D) {
        for (int y=layerV->size.y-1; y>=1; y--) {
          for (int x=0; x<layerV->size.x; x++) {
            layerV->setRGB(Coord3D(x, y, 0), layerV->getRGB(Coord3D(x,y-1,0)));
          }
        }
      }
    }

    //2D->3D: each XY plane is rippled through the z-axis
    if (towardsZ) { //not relevant for 2D fixtures
      if (layerV->effectDimension < _3D && layerV->layerDimension == _3D) {
        for (int z=layerV->size.z-1; z>=1; z--) {
          for (int y=0; y<layerV->size.y; y++) {
            for (int x=0; x<layerV->size.x; x++) {
              layerV->setRGB(Coord3D(x, y, z), layerV->getRGB(Coord3D(x,y,z-1)));
            }
          }
        }
      }
    }
  }
}; //RippleYZ

// RotateNodifier rotates the light position around the center of the layout.
// It can flip the x and y coordinates, reverse the rotation direction, and alternate the rotation
// direction every full rotation. It also supports shear transformations to create a more dynamic effect.
// by WildCats08 / @Brandon502
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

  uint8_t direction = 0;
  uint8_t rotateSpeed = 128;
  bool expand = false;

  void setup() override {

    hasModifier = true;

    JsonObject property = addControl(direction, "direction", "select");
    JsonArray values = property["values"].to<JsonArray>();
    values.add("Clockwise");
    values.add("Counter-Clockwise");
    values.add("Alternate");

    addControl(rotateSpeed, "rotateSpeed", "range"); 
    addControl(expand, "expand", "checkbox");

  }

  void modifySize() override {

    if (expand) {
      uint8_t size = MAX(layerV->size.x, MAX(layerV->size.y, layerV->size.z));
      size = sqrt(size * size * 2) + 1;
      Coord3D offset = Coord3D((size - layerV->size.x) / 2, (size - layerV->size.y) / 2, 0);

      layerV->size = Coord3D{size, size, 1};
    }

    data.midX = layerV->size.x / 2;
    data.midY = layerV->size.y / 2;
  }

  void modifyPosition(Coord3D &position) override {

    if (expand) {
      int size = MAX(layerV->size.x, MAX(layerV->size.y, layerV->size.z));
      size = sqrt(size * size * 2) + 1;
      Coord3D offset = Coord3D((size - layerV->size.x) / 2, (size - layerV->size.y) / 2, 0);

      position.x += offset.x;
      position.y += offset.y;
      position.z += offset.z;
    }
  }

  void modifyXYZ(Coord3D &position) override {

    constexpr int Fixed_Scale = 1 << 10;
    constexpr int Scale_Shift = 10;
    

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
    int x1 = dx + ((data.shearX * dy + ( 1<<(Scale_Shift-1))) >> Scale_Shift);
    int y1 = dy + ((data.shearY * x1 + ( 1<<(Scale_Shift-1))) >> Scale_Shift);
    int x2 = x1 + ((data.shearX * y1 + ( 1<<(Scale_Shift-1))) >> Scale_Shift);

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

class TransposeModifier: public Node {
  public:

  static const char * name() {return "Transpose 💎💡";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  bool transposeXY = true;
  bool transposeXZ = false;
  bool transposeYZ = false;
  
  void setup() override {
    hasModifier = true;
    addControl(transposeXY, "XY", "checkbox");
    addControl(transposeXZ, "XZ", "checkbox");
    addControl(transposeYZ, "YZ", "checkbox");
  }

  Coord3D originalSize;
  
  void modifySize() override {
    if (transposeXY) { int temp = layerV->size.x; layerV->size.x = layerV->size.y; layerV->size.y = temp; }
    if (transposeXZ) { int temp = layerV->size.x; layerV->size.x = layerV->size.z; layerV->size.z = temp; }
    if (transposeYZ) { int temp = layerV->size.y; layerV->size.y = layerV->size.z; layerV->size.z = temp; }
    originalSize = layerV->size;
    MB_LOGV(ML_TAG, "transpose %d %d %d", layerV->size.x, layerV->size.y, layerV->size.z);
  }

  void modifyPosition(Coord3D &position) override {
    if (transposeXY) { int temp = position.x; position.x = position.y; position.y = temp; }
    if (transposeXZ) { int temp = position.x; position.x = position.z; position.z = temp; }
    if (transposeYZ) { int temp = position.y; position.y = position.z; position.z = temp; }
  }
}; //Transpose

//by WildCats08 / @Brandon502
class CheckerboardModifier: public Node {
  public:
  static const char * name() {return "Checkerboard 💎💫";}
  static const char * tags() {return "";}

  Coord3D    size   = {3,3,3};
  bool invert = false;
  bool group  = false;

  void setup() override {
    hasModifier = true;
    addControl(size, "squareSize", "coord3D", 1, 100);
    addControl(invert, "invert", "checkbox");
    addControl(group, "group", "checkbox");
  }

  Coord3D originalSize;
  
  void modifySize() override {

    if (group) { layerV->middle /= size; layerV->size = (layerV->size + (size - Coord3D{1,1,1})) / size; }
    originalSize = layerV->size;
  }

  void modifyPosition(Coord3D &position) override {
    Coord3D check = position / size;

    if (group) position /= size;

    if ((check.x + check.y + check.z) % 2 == 0) {
      if (invert)  { position.x = UINT16_MAX; return; }//do not show this pixel
    } 
    else {
      if (!invert) { position.x = UINT16_MAX; return; } //do not show this pixel
    }

  }
}; //CheckerboardModifier

#endif