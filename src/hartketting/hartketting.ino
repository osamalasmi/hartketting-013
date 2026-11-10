// Hartslagketting - H6 Niet-zo-single-feestje
// ESP32: 3 statusleds, 1 knop, potmeter als hartslag

const int PIN_GROEN = 21;   // single
const int PIN_ROOD = 19;    // bezet
const int PIN_ORANJE = 18;  // ingewikkeld
const int PIN_KNOP = 23;
const int PIN_POT = 4;      // ADC2: werkt niet als wifi aan staat

const int LEDS[3] = {PIN_GROEN, PIN_ROOD, PIN_ORANJE};
const char* NAMEN[3] = {"Single", "Bezet", "Ingewikkeld"};

const int BPM_MIN = 50;
const int BPM_MAX = 180;
const unsigned long RUST_MEETTIJD = 10000;  // 10 s rustpols meten
const unsigned long LANG_DRUKKEN = 2000;    // 2 s = stilstand
const unsigned long FLITS_DUUR = 150;       // ms dat de led per hartslag aan is
const int HELDER_NORMAAL = 100;             // ~40% van 255
const int HELDER_SPANNING = 255;            // 100%

int status = 0;  // 0 single, 1 bezet, 2 ingewikkeld
bool stil = false;
float rustpols = 0;
unsigned long rustSom = 0;
unsigned long rustAantal = 0;
unsigned long startTijd = 0;
unsigned long laatsteSlag = 0;
unsigned long knopIngedruktSinds = 0;
bool knopWasIngedrukt = false;

int leesBpm() {
  int ruw = analogRead(PIN_POT);  // 0-4095 op ESP32
  return map(ruw, 0, 4095, BPM_MIN, BPM_MAX);
}

void zetLeds(int helderheid) {
  for (int i = 0; i < 3; i++) {
    analogWrite(LEDS[i], i == status ? helderheid : 0);
  }
}

void verwerkKnop() {
  bool ingedrukt = digitalRead(PIN_KNOP) == LOW;
  unsigned long nu = millis();
  if (ingedrukt && !knopWasIngedrukt) {
    knopIngedruktSinds = nu;
  }
  if (!ingedrukt && knopWasIngedrukt) {
    unsigned long duur = nu - knopIngedruktSinds;
    if (duur >= LANG_DRUKKEN) {
      stil = !stil;
      Serial.println(stil ? "Stilstand aan" : "Stilstand uit");
    } else if (duur > 30) {  // debounce
      status = (status + 1) % 3;
      Serial.print("Status: ");
      Serial.println(NAMEN[status]);
    }
  }
  knopWasIngedrukt = ingedrukt;
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) pinMode(LEDS[i], OUTPUT);
  pinMode(PIN_KNOP, INPUT_PULLUP);
  startTijd = millis();
  Serial.println("Hartslagketting gestart, rustpols meten (10 s)...");
}

void loop() {
  verwerkKnop();
  unsigned long nu = millis();
  int bpm = leesBpm();

  // Rustpols meten in de eerste 10 seconden
  if (nu - startTijd < RUST_MEETTIJD) {
    rustSom += bpm;
    rustAantal++;
  } else if (rustpols == 0) {
    rustpols = (float)rustSom / rustAantal;
    Serial.print("Rustpols: ");
    Serial.print(rustpols, 0);
    Serial.print(" BPM, spanning vanaf ");
    Serial.println(rustpols * 1.25, 0);
  }

  // Stilstand: gedimd continu branden, geen hartslag tonen
  if (stil) {
    zetLeds(30);
    return;
  }

  bool spanning = rustpols > 0 && bpm >= rustpols * 1.25;
  int helderheid = spanning ? HELDER_SPANNING : HELDER_NORMAAL;
  unsigned long interval = 60000UL / bpm;  // ms tussen twee slagen

  if (nu - laatsteSlag >= interval) {
    laatsteSlag = nu;
  }
  zetLeds(nu - laatsteSlag < FLITS_DUUR ? helderheid : 0);
  delay(5);
}
