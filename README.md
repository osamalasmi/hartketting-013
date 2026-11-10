# Hartslagketting 013

Prototype voor het Valentijnsfeest "Smerrig" van Poppodium 013 (Hackathon 6, Niet-zo-single-feestje).

Een hartketting met een led die knippert op je echte hartslag. De ketting haalt de hartslag via Bluetooth uit je Whoop. Met een knop kies je je status: single (groen), bezet (blauw) of ingewikkeld (geel). Boven de 100 bpm wordt de led rood en knippert hij snel.

## Team

- Osama
- Ahmad

## Techniek

- ESP32 DevKit
- Koppeling met de Whoop via Bluetooth (BLE). De Whoop zendt de hartslag uit met "Broadcast Heart Rate". De ESP32 leest die via de standaard Heart Rate Service.
- RGB-led, knop en buzzer
- Reserve: in Wokwi werkt Bluetooth niet. Daar doet een potmeter de hartslag na.

## Mappen

| Map | Inhoud |
|-----|--------|
| `docs/` | Het ontwerp: [docs/ontwerp.md](docs/ontwerp.md) |
| `src/hartketting/` | De code voor de ESP32 (komt in fase 2) |
