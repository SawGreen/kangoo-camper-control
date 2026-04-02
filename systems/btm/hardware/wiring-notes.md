
# BTM wiring notes — ESP32 + 4× ADS1115 + 16× NTC (bench)

Tento dokument je praktická „wiring realita“ pro bench test.
Cíl: aby šlo rychle zkontrolovat pinout, adresy, napájení a správný vzorec pro dělič.

---

## I2C sběrnice

Společné signály pro všechny 4× ADS1115:
- SDA (data)
- SCL (clock)
- GND (společná zem)
- VDD (napájení ADC)

Poznámka k pull-upům:
- Většina ADS1115 breakoutů už pull-upy má.
- Se 4 moduly se pull-upy paralelně zmenší (větší proud). Pokud je sběrnice nestabilní, zvaž:
  - odpojit některé pull-upy na modulech, nebo
  - řídit se jedním párem (např. 4.7k–10k) pro celou sběrnici.

---

## ESP32 SDA/SCL piny

Repo aktuálně nespecifikuje konkrétní ESP32 variantu ani pin mapping (ADR-001 to explicitně nechává otevřené).  
Bench firmware skeleton používá výchozí mapping pro běžný ESP32 DevKit:

- SDA: GPIO21
- SCL: GPIO22

Pokud používáš jiný board (ESP32-S3 apod.), uprav to v `systems/btm/firmware/benchtest_ads1115_ntc/include/btm_config.h`.

---

## ADS1115 I2C adresy (ADDR pin)

ADS1115 má 4 možné 7-bit adresy dle toho, kam připojíš pin ADDR: citeturn6search2

- ADDR → GND  => 0x48 (default)
- ADDR → VDD  => 0x49
- ADDR → SDA  => 0x4A
- ADDR → SCL  => 0x4B

Praktická konfigurace pro 4 moduly:
- 1. modul: ADDR na GND (0x48)
- 2. modul: ADDR na VDD (0x49)
- 3. modul: ADDR na SDA (0x4A)
- 4. modul: ADDR na SCL (0x4B)

Pozor: „ADDR → SDA/SCL“ znamená fyzicky připojit ADDR na tu linku (ne na pin ESP32), tj. přímo na I2C vodič SDA/SCL.

---

## Napájení

Doporučené pro bench:
- VDD ADS1115: 3.3 V
- Dělič NTC: 3.3 V
- ESP32 logika: 3.3 V

Důvod: I2C úrovně zůstanou bezpečně kompatibilní bez level-shifteru.

Pokud bys ADS1115 napájel 5 V:
- musíš řešit I2C level shifting (ESP32 není 5V tolerant), a
- analog vstupy stále nesmí lézt mimo GND..VDD.

---

## Mapování kanálů (16 kanálů)

Každý ADS1115 má 4 single-ended vstupy AIN0..AIN3 => 4×4 = 16 kanálů.

Doporučené globální mapování (firmware to takto tiskne):
- ADS 0 (0x48): AIN0..3 => global 0..3
- ADS 1 (0x49): AIN0..3 => global 4..7
- ADS 2 (0x4A): AIN0..3 => global 8..11
- ADS 3 (0x4B): AIN0..3 => global 12..15

Do bench logu si doplň „který global ID = který fyzický senzor“.

---

## NTC dělič — 2 možné orientace (a vzorce)

RT = odpor NTC  
R = pevný rezistor (v repo baseline 10k)  
Vcc = napájení děliče  
Vout = napětí na uzlu (měřené ADS1115 vůči GND)

### Varianta A: R nahoře na Vcc, NTC dole na GND

Schéma:
Vcc --- R ---+--- NTC --- GND
             |
           Vout → ADS AINx

Platí:
- Vout = Vcc * (RT / (R + RT))
- RT = R * Vout / (Vcc - Vout)

### Varianta B: NTC nahoře na Vcc, R dole na GND

Schéma:
Vcc --- NTC ---+--- R --- GND
               |
             Vout → ADS AINx

Platí:
- Vout = Vcc * (R / (R + RT))
- RT = R * (Vcc - Vout) / Vout

Poznámka:
- Obě varianty jsou OK. Jen musí sedět vzorec v kódu.
- U 25 °C (R=10k, RT≈10k) vyjde Vout přibližně 1/2 Vcc u obou orientací.

---

## Poznámka k dlouhému analogovému vedení

Dlouhé analogové vedení mezi NTC a ADS1115 je náchylné na:
- šum, kapacitní vazby
- zemní smyčky a „mystery offsety“

Pro zítřek: hlavně to zprovoznit a změřit.
Dlouhodobě: ADC co nejblíž senzorům, digitální I2C dál k ESP32.

Pro stabilitu může pomoct:
- vedení analog signálů v párech se zemí
- společný „analog ground point“ u ADS
- (až budeš ladit šum) malý C na vstup (řádově 1–10 nF) podle potřeby
