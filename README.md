# Hartslagketting 013

Prototype voor het Valentijnsfeest "Smerrig" van Poppodium 013 (Hackathon 6, Niet-zo-single-feestje).

Een hartketting met een led die knippert op je hartslag. Met een korte druk op de knop kies je je status: single (groen), bezet (rood) of ingewikkeld (blauw). Bij het aanzetten meet de ketting je rustpols. Ligt je hartslag 25% daarboven, dan knippert de led feller. Met een lange druk verberg je je hartslag.

## Team

- Osama
- Ahmad

## Techniek

- ESP32 DevKit
- Drie statusleds (groen, rood, blauw), knop en potmeter op een breadboard
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
| `tools/presentatiescherm/` | Presentatiescherm voor de beamer |

## Presentatiescherm

De ketting zelf is te klein om aan een zaal te laten zien. Het presentatiescherm is een webpagina die live meeleest met de ESP32 via USB en de ketting groot op de beamer toont: het hart klopt op de hartslag in de statuskleur, en je ziet de BPM, de bron, de rustpols en de grens.

Gebruik:

1. Sluit de Serial Monitor (die houdt de USB-poort bezet).
2. Open `tools/presentatiescherm/index.html` in Chrome of Edge (andere browsers ondersteunen Web Serial niet).
3. Klik op "Verbind met ESP32" en kies de usbserial-poort.
4. Druk op EN op de ESP32, zodat hij opnieuw start en de rustpols meet.
5. Fullscreen met ⌃⌘F.

Reserve: werkt de verbinding niet, klik dan op "Demo-modus". Met **S** wissel je de status, met **H** zet je de stilstand aan of uit en met **↑/↓** gaat de hartslag omhoog of omlaag.
