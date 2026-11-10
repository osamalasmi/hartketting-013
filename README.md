# Hartslagketting 013

Prototype voor het Valentijnsfeest "Smerrig" van Poppodium 013 (Hackathon 6, Niet-zo-single-feestje).

Een hartketting met een led die knippert op je hartslag. Met een korte druk op de knop kies je je status: single (groen), bezet (rood) of ingewikkeld (oranje). Bij het aanzetten meet de ketting je rustpols. Ligt je hartslag 25% daarboven, dan knippert de led feller. Met een lange druk verberg je je hartslag.

## Team

- Osama
- Ahmad

## Techniek

- ESP32 DevKit
- Drie statusleds (groen, rood, oranje), knop en potmeter op een breadboard
- Prototype in Wokwi. Een potmeter doet de hartslag na.
- Echte product: hartslag uit een sensor in de ketting, of uit een Whoop of smartwatch via Bluetooth (BLE).

## Ontwikkelomgeving

| Onderdeel | Keuze |
|-----------|-------|
| Taal | C++ met het Arduino-framework |
| Editor | VS Code |
| Bouwen en uploaden | PlatformIO met pioarduino 55.03.312 (Arduino-framework 3.3.12) |
| Simulator | Wokwi for VS Code |

Zo zet je het op:

1. Open deze map in VS Code.
2. Installeer de aanbevolen extensies (PlatformIO IDE en Wokwi Simulator). VS Code vraagt dit vanzelf.
3. Bouw het project met de knop Build (vinkje) in de onderste balk. De eerste keer downloadt PlatformIO het ESP32-pakket.
4. Simuleren: open `src/hartketting/diagram.json` en kies "Wokwi: Start Simulator". De eerste keer vraagt Wokwi om een gratis licentie.
5. Uploaden naar de echte ESP32: sluit hem aan met USB en klik op Upload (pijl) in de onderste balk.

## Mappen

| Map | Inhoud |
|-----|--------|
| `docs/` | Het ontwerp: [docs/ontwerp.md](docs/ontwerp.md) |
| `src/hartketting/` | De code voor de ESP32 (`hartketting.ino`), het Wokwi-schema (`diagram.json`) en `wokwi.toml` |
| `platformio.ini` | Instellingen van PlatformIO: bord en vaste versies |
