void setup() {
  for (int x = 0; x<16; x++)
    for (int y=0; y<16; y++)
      addPixel(x,(x%2)?y:15 - y,0);
}