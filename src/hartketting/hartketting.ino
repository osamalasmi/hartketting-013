// Hartslagketting - H6 Niet-zo-single-feestje
// ESP32: 3 statusleds, 1 knop, hartslag van een Whoop (BLE) of de potmeter
// Namen en waarden volgen docs/ontwerp.md, hoofdstuk 5 en 6.

#include <NimBLEDevice.h>

// Pinnen
const int PIN_GROEN = 21;   // single
const int PIN_ROOD = 19;    // bezet
const int PIN_BLAUW = 18;   // ingewikkeld
const int PIN_KNOP = 23;
const int PIN_POT = 4;      // ADC2: werkt niet als de radio aan staat

// Waarden uit de acceptatiecriteria
const int BPM_MIN = 50;                      // AC-02.1
const int BPM_MAX = 180;                     // AC-02.1, nooit hoger: dan sneller dan 3 knippers per seconde
const unsigned long MEETTIJD_MS = 10000;     // AC-03.1: 10 s rustpols meten
const float GRENS_FACTOR = 1.25;             // AC-03.2: 25% boven de rustpols
const int HELDER_NORMAAL = 40;               // procent, AC-03.2 en AC-04.2
const int HELDER_MAX = 100;                  // procent, AC-03.2
const unsigned long LANG_DRUKKEN_MS = 2000;  // AC-01.1 en AC-04.1
const unsigned long FLITS_MS = 100;          // zo lang is de led aan per knipper
const unsigned long ONTDENDER_MS = 50;       // een druk korter dan dit telt niet

// Whoop koppelen (US-05): standaard BLE-hartslagprofiel
const char* WHOOP_NAAM = "WHOOP";                  // alleen apparaten met deze naam (begin van de naam)
const unsigned long WHOOP_TIMEOUT_MS = 5000;       // geen bericht meer? Dan terug naar de potmeter (AC-05.2)
const uint32_t SCAN_MS = 5000;                     // zo lang zoeken we per ronde
const NimBLEUUID HR_SERVICE((uint16_t)0x180D);     // Heart Rate Service
const NimBLEUUID HR_METING((uint16_t)0x2A37);      // Heart Rate Measurement

enum Status { SINGLE, BEZET, INGEWIKKELD };
const int LEDS[3] = {PIN_GROEN, PIN_ROOD, PIN_BLAUW};  // volgorde = Status
const char* NAMEN[3] = {"Single", "Bezet", "Ingewikkeld"};

Status status = SINGLE;  // AC-01.3
bool stilstand = false;
bool metenKlaar = false;
long bpmSom = 0;
int aantalMetingen = 0;
int rustpols = 0;
int grens = 0;
unsigned long startTijd = 0;
unsigned long laatsteKnipper = 0;
unsigned long knopIngedruktSinds = 0;
bool knopWasIngedrukt = false;

// De Whoop schrijft, loop() leest. Daarom volatile.
volatile int whoopBpm = 0;
volatile unsigned long whoopLaatsteMs = 0;

// Komt binnen bij elk bericht van de Whoop (ongeveer elke seconde).
// Bit 0 van de eerste byte: 0 = bpm in byte 1, 1 = bpm in byte 1 en 2 samen.
void bijHartslag(NimBLERemoteCharacteristic* kenmerk, uint8_t* data, size_t lengte, bool isNotify) {
  if (lengte < 2) {
    return;
  }
  int bpm = data[1];
  if ((data[0] & 0x01) && lengte >= 3) {
    bpm = data[1] | (data[2] << 8);
  }
  whoopBpm = bpm;
  whoopLaatsteMs = millis();
}

// Draait los van loop(), zodat zoeken en verbinden de led niet laat haperen.
// Zoekt de Whoop, verbindt, en begint opnieuw als de verbinding wegvalt.
void whoopTaak(void* onbenut) {
  NimBLEDevice::init("hartketting");
  NimBLEScan* scan = NimBLEDevice::getScan();
  scan->setActiveScan(true);  // de naam zit in het scan-antwoord

  for (;;) {
    NimBLEScanResults gevonden = scan->getResults(SCAN_MS, false);
    const NimBLEAdvertisedDevice* whoop = nullptr;
    for (int i = 0; i < gevonden.getCount(); i++) {
      const NimBLEAdvertisedDevice* apparaat = gevonden.getDevice(i);
      if (apparaat->isAdvertisingService(HR_SERVICE) && apparaat->getName().rfind(WHOOP_NAAM, 0) == 0) {
        whoop = apparaat;
        break;
      }
    }
    if (whoop == nullptr) {
      continue;
    }

    Serial.print("Whoop gevonden: ");
    Serial.println(whoop->getName().c_str());
    NimBLEClient* client = NimBLEDevice::createClient();
    if (client->connect(whoop)) {
      NimBLERemoteService* dienst = client->getService(HR_SERVICE);
      NimBLERemoteCharacteristic* meting = dienst ? dienst->getCharacteristic(HR_METING) : nullptr;
      if (meting && meting->subscribe(true, bijHartslag)) {
        Serial.println("Whoop gekoppeld");
        while (client->isConnected()) {
          vTaskDelay(pdMS_TO_TICKS(500));
        }
      }
    }
    NimBLEDevice::deleteClient(client);
    whoopBpm = 0;
    Serial.println("Whoop weg, terug naar de potmeter");
  }
}

// De enige plek waar de hartslag vandaan komt.
// Is er een recent bericht van de Whoop, dan gebruiken we dat (AC-05.1).
// Anders de potmeter (AC-05.2).
int leesHartslag() {
  int bpm = whoopBpm;
  if (bpm > 0 && millis() - whoopLaatsteMs < WHOOP_TIMEOUT_MS) {
    return constrain(bpm, BPM_MIN, BPM_MAX);  // nooit boven 180: epilepsie
  }
  int ruw = analogRead(PIN_POT);  // 0-4095 op de ESP32
  return map(ruw, 0, 4095, BPM_MIN, BPM_MAX);
}

// Zet de led van de huidige status aan, de andere twee uit.
// procent: 0 (uit) tot 100 (maximaal fel).
void zetLeds(int procent) {
  int pwm = procent * 255 / 100;
  for (int i = 0; i < 3; i++) {
    analogWrite(LEDS[i], i == status ? pwm : 0);
  }
}

// Kort of lang drukken bepalen we bij het loslaten.
void verwerkKnop() {
  bool ingedrukt = digitalRead(PIN_KNOP) == LOW;  // INPUT_PULLUP: ingedrukt = LOW
  unsigned long nu = millis();

  if (ingedrukt && !knopWasIngedrukt) {
    knopIngedruktSinds = nu;
  }

  if (!ingedrukt && knopWasIngedrukt) {
    unsigned long duur = nu - knopIngedruktSinds;
    if (duur >= LANG_DRUKKEN_MS) {
      stilstand = !stilstand;  // AC-04.1
      Serial.println(stilstand ? "Stilstand aan" : "Stilstand uit");
    } else if (duur >= ONTDENDER_MS) {
      status = (Status)((status + 1) % 3);  // AC-01.1
      Serial.print("Status: ");
      Serial.println(NAMEN[status]);
    }
  }

  knopWasIngedrukt = ingedrukt;
}

// Rustpols meten in de eerste 10 seconden (AC-03.1).
void meetRustpols(int bpm, unsigned long nu) {
  if (metenKlaar) {
    return;
  }
  if (nu - startTijd < MEETTIJD_MS) {
    bpmSom += bpm;
    aantalMetingen++;
    return;
  }
  rustpols = bpmSom / aantalMetingen;
  grens = ceil(rustpols * GRENS_FACTOR);  // naar boven afronden: "minstens 25%"
  metenKlaar = true;
  Serial.print("Rustpols: ");
  Serial.print(rustpols);
  Serial.print(" BPM, spanning vanaf ");
  Serial.println(grens);
}

// Elke seconde een regel "tijd_ms,bpm,bron". Zo leggen we echte Whoop-data vast
// als dummydata voor de tests (issue #21).
void logHartslag(int bpm, unsigned long nu) {
  static unsigned long laatsteLog = 0;
  if (nu - laatsteLog < 1000) {
    return;
  }
  laatsteLog = nu;
  bool whoop = whoopBpm > 0 && nu - whoopLaatsteMs < WHOOP_TIMEOUT_MS;
  Serial.print("HR,");
  Serial.print(nu);
  Serial.print(",");
  Serial.print(bpm);
  Serial.print(",");
  Serial.println(whoop ? "whoop" : "potmeter");
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) {
    pinMode(LEDS[i], OUTPUT);
  }
  pinMode(PIN_KNOP, INPUT_PULLUP);
  startTijd = millis();
  xTaskCreate(whoopTaak, "whoop", 8192, nullptr, 1, nullptr);
  Serial.println("Hartslagketting gestart, rustpols meten (10 s)...");
}

void loop() {
  verwerkKnop();
  unsigned long nu = millis();
  int bpm = leesHartslag();
  meetRustpols(bpm, nu);  // loopt ook door als de stilstand aan staat
  logHartslag(bpm, nu);

  if (stilstand) {
    // AC-04.2: continu en gedimd, zonder te knipperen
    zetLeds(HELDER_NORMAAL);
  } else {
    // AC-03.2: boven de grens maximaal fel, anders 40%
    bool spanning = metenKlaar && bpm >= grens;
    int helderheid = spanning ? HELDER_MAX : HELDER_NORMAAL;

    // AC-02.2: een knipper per hartslag
    unsigned long interval = 60000UL / bpm;  // ms tussen twee slagen
    if (nu - laatsteKnipper >= interval) {
      laatsteKnipper = nu;
    }
    zetLeds(nu - laatsteKnipper < FLITS_MS ? helderheid : 0);
  }

  delay(5);  // korte pauze, maakt de simulatie in Wokwi sneller
}
