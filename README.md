# Hartslagketting 013

Prototype voor het Valentijnsfeest "Smerrig" van Poppodium 013 (Hackathon 6, Niet-zo-single-feestje).

Een hartketting met een led die knippert op je hartslag. Met een korte druk op de knop kies je je status: single (groen), bezet (rood) of ingewikkeld (oranje). Bij het aanzetten meet de ketting je rustpols. Ligt je hartslag 25% daarboven, dan knippert de led feller. Met een lange druk verberg je je hartslag.

## Team

- Osama
- Ahmad

## Techniek

- ESP32 DevKit
- RGB-led en knop
- Prototype in Wokwi. Een potmeter doet de hartslag na.
- Echte product: hartslag uit een sensor in de ketting, of uit een Whoop of smartwatch via Bluetooth (BLE).

## Mappen

| Map | Inhoud |
|-----|--------|
| `docs/` | Het ontwerp: [docs/ontwerp.md](docs/ontwerp.md) |
| `src/hartketting/` | De code voor de ESP32 (komt in fase 2) |
