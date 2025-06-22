uint8_t bpm;
uint8_t pan;
uint8_t tilt;
uint8_t range;
bool invert;

void setup() {
    addControl(&bpm, "bpm", "range", 30, 1, 255);
    addControl(&pan, "pan", "range", 175, 0, 255);
    addControl(&tilt, "tilt", "range", 90, 0, 255);
    addControl(&range, "range", "range", 20, 0, 255);
    addControl(&invert, "invert", "checkbox", false, false, true);
}
void loop() {
  for (uint8_t x = 0; x < width; x++) {
    setPan(x, beatsin8(bpm, pan-range, pan + range, 0,  (invert && x%2==0)?128:0));
    setTilt(x, beatsin8(bpm, tilt - range, tilt + range, 0,  (invert && x%2==0)?128:0));
  }
}