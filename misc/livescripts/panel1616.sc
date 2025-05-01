void addLayout() {
  addPin(2); //not working yet
  for (int x = 0; x<16; x++)
    for (int y=0; y<16; y++)
      addLight(x,(x%2)?y:15 - y,0);
}