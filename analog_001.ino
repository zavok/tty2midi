int knobs[6], oldknobs[6];
int pins[6] = {A0, A1, A2, A3, A4, A5};

void
setknobs(void) {
  int i;
  for (i = 0; i < 6; i++)
    knobs[i] = analogRead(pins[i])>>3;
}

void
setoldknobs(void) {
  int i;
  for (i = 0; i < 6; i++) oldknobs[i] = knobs[i];
}

void setup() {
  setknobs();
  Serial.begin(115200);
}

void loop() {
  int i;
  char msg[3];
  char buf[1024];
  setoldknobs();
  setknobs();
  for (i = 0; i < 6; i++) {
    if (oldknobs[i] != knobs[i]) {
      msg[0] = 0xB0;
      msg[1] = i;
      msg[2] = knobs[i];
      Serial.write(msg, 3);
    }
  }
}
