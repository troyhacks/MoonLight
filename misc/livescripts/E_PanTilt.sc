uint8_t bpm = 30;
uint8_t pan = 175;
uint8_t tilt = 90;
uint8_t range = 20;
bool invert = false;

void setup() {
    addControl(&bpm, "bpm", "range", 1, 255);
    addControl(&pan, "pan", "range", 0, 255);
    addControl(&tilt, "tilt", "range", 0, 255);
    addControl(&range, "range", "range", 0, 255);
    addControl(&invert, "invert", "checkbox", false, true);
}
void loop() {
  for (uint8_t x = 0; x < width; x++) {
    setPan(x, beatsin8(bpm, pan-range, pan + range, 0,  (invert && x%2==0)?128:0));
    setTilt(x, beatsin8(bpm, tilt - range, tilt + range, 0,  (invert && x%2==0)?128:0));
  }
}