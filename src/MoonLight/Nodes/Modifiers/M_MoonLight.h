/**
    @title     MoonLight
    @file      M_MoonLight.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONLIGHT

//Takes the x dimension from the layout (1D effect) and turn it into a circle in 2D or a sphere in 3D.
class CircleModifier: public Node {
  public:

  static const char * name() {return "Circle 💎💡";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  Coord3D originalSize;

  bool hasModifier() const override { return true; }

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
    addControl(mirrorX, "mirrorX", "checkbox");
    addControl(mirrorY, "mirrorY", "checkbox");
    addControl(mirrorZ, "mirrorZ", "checkbox");
  }

  Coord3D originalSize;
  
  bool hasModifier() const override { return true; }

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
    addControl(proMulti, "multipliers", "coord3D");
  }

  bool hasModifier() const override { return true; }

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
    addControl(petals, "petals", "range");
    addControl(swirlVal, "swirlVal", "range");
    addControl(reverse, "reverse", "checkbox");
    addControl(symmetry, "symmetry", "range");
    addControl(zTwist, "zTwist", "range");
  }
  
  bool hasModifier() const override { return true; }

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
    addControl(shrink, "shrink", "checkbox");
    addControl(towardsY, "towardsY", "checkbox");
    addControl(towardsZ, "towardsZ", "checkbox");
  }

  bool hasModifier() const override { return true; }

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

// RotateModifier rotates the light position around the center of the layout.
// It can flip the x and y coordinates, reverse the rotation direction, and alternate the rotation
// direction every full rotation. It also supports shear transformations to create a more dynamic effect.
// by WildCats08 / @Brandon502
class RotateModifier: public Node {
  public:

  static const char * name() {return "Rotate 💎💫";}
  static const char * tags() {return "";}

  bool expand = false;
  bool flip, reverse, alternate;
  uint8_t direction = 0;
  uint8_t rotateBPM = 15;
  uint16_t staticAngle = 0;

  void setup() override {
    JsonObject property = addControl(direction, "direction", "select");
    JsonArray values = property["values"].to<JsonArray>();
    values.add("Clockwise");
    values.add("Counter-Clockwise");
    values.add("Alternate");

    addControl(rotateBPM, "rotateBPM", "range");
    addControl(staticAngle, "staticAngle", "number", 0, 359);
    addControl(expand, "expand", "checkbox");
  }

  Coord3D originalSize;
  int midX, midY;
  int maxX, maxY;

  bool hasModifier() const override { return true; }

  void modifySize() override {

    if (expand) {
      uint8_t size = MAX(layerV->size.x, MAX(layerV->size.y, layerV->size.z));
      size = sqrt(size * size * 2) + 1;
      Coord3D offset = Coord3D((size - layerV->size.x) / 2, (size - layerV->size.y) / 2, 0);

      layerV->size = Coord3D{size, size, 1};
    }

    originalSize = layerV->size;
    midX = layerV->size.x / 2;
    midY = layerV->size.y / 2;
    maxX = layerV->size.x;
    maxY = layerV->size.y;

  }

  void modifyPosition(Coord3D &position) override {

    if (expand) {
      int size = MAX(originalSize.x, MAX(originalSize.y, originalSize.z));
      size = sqrt(size * size * 2) + 1;
      Coord3D offset = Coord3D((size - originalSize.x) / 2, (size - originalSize.y) / 2, 0);
      position += offset;
    }
  }

  uint16_t angle     = UINT16_MAX;
  uint16_t prevAngle = UINT16_MAX;
  int16_t  shearX;
  int16_t  shearY;
  const uint8_t  Scale_Shift = 10;
  const int      Fixed_Scale = (1 << Scale_Shift);
  const int      RoundVal    = (1 << (Scale_Shift - 1));

  void loop() override {
    //place in loop by by softhack007
    if (rotateBPM == 0) angle = staticAngle;
    else angle = ::map(beat16(rotateBPM), 0, UINT16_MAX, 0, 360); //change to time independant

    if (angle != prevAngle) {
      if (direction == 2) alternate = true; 
      else              { alternate = false; reverse = (direction == 1); }

      if (alternate && (angle < prevAngle)) reverse = !reverse;

      int shearAngle = reverse ? 360 - angle : angle;

      flip = (shearAngle > 90 && shearAngle < 270);

      shearAngle = flip ? (shearAngle + 180) % 360 : shearAngle; // Flip shearAngle if needed

      // Calculate shearX and shearY
      float angleRadians = radians(shearAngle);
      shearX = -tanf(angleRadians / 2) * Fixed_Scale; //f by softhack007
      shearY =  sinf(angleRadians)     * Fixed_Scale; //f by softhack007

      prevAngle = angle;
    }
  }

  void modifyXYZ(Coord3D &position) override {
    if (angle == 0) return; // No rotation needed
    if (flip) {
      // Reverse x and y values
      position.x = maxX-1 - position.x;
      position.y = maxY-1 - position.y;
    }

    // Translate position to origin
    int dx = position.x - midX;
    int dy = position.y - midY;

    // Apply the 3 shear transformations
    int x1 = dx + ((shearX * dy + RoundVal) >> Scale_Shift);
    int y1 = dy + ((shearY * x1 + RoundVal) >> Scale_Shift);
    int x2 = x1 + ((shearX * y1 + RoundVal) >> Scale_Shift);

    // Avoid current wrapping issues by setting out-of-bounds positions to INT8_MAX
    // Delete and swap the commented lines below if drawing out of bounds is no longer possible
    x2 += midX;
    y1 += midY;
    if (x2 >= maxX || y1 >= maxY || x2 < 0 || y1 < 0) position = {INT16_MAX, INT8_MAX, 0};
    else position = {static_cast<uint16_t>(x2), static_cast<uint8_t>(y1), 0};

    // Translate position back and assign
    // position.x = x2 + midX;
    // position.y = y1 + midY;
    // position.z = 0;
  }
}; //RotateModifier

class TransposeModifier: public Node {
  public:

  static const char * name() {return "Transpose 💎💡";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  bool transposeXY = true;
  bool transposeXZ = false;
  bool transposeYZ = false;
  
  void setup() override {
    addControl(transposeXY, "XY", "checkbox");
    addControl(transposeXZ, "XZ", "checkbox");
    addControl(transposeYZ, "YZ", "checkbox");
  }

  Coord3D originalSize;
  
  bool hasModifier() const override { return true; }

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
    addControl(size, "squareSize", "coord3D", 1, 100);
    addControl(invert, "invert", "checkbox");
    addControl(group, "group", "checkbox");
  }

  Coord3D originalSize;
  
  bool hasModifier() const override { return true; }

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