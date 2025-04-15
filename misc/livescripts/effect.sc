void setup() {
  printf("Run Live Script good morning\n");
}
void loop() {
  leds[random16(255)] = CRGB(0, 0, 255);
}
void main() {
  setup();
  while (true) {
    loop();
    sync();
 }
}