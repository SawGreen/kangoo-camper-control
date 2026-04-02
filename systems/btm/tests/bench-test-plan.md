# BTM bench test plan — 4× ADS1115 + 16× NTC (4P4S)

Datum testu: 2026-04-03  
Cíl: Ověřit měřicí řetězec BTM na stole (I2C → ADS1115 → 16 kanálů → odpor → teplota → Serial log) a rychle odhalit chyby v zapojení / adresách / přepočtech.

Kontext:
- BTM baseline v repo: ESP32 + 4× ADS1115 + 16× B3950 10k NTC + 16× 10k 1% rezistor (viz ADR-001).  
- Tento bench test je „ošklivý, ale účelový“: teď chceme čísla, ne finální architekturu.

---

## Scope bench firmwaru (MUST)

Firmware pro bench test dělá pouze:
- inicializace I2C (Wire.begin + clock)
- detekce 4× ADS1115 na I2C (adresy a počet)
- čtení všech 16 kanálů (4 ADS × 4 single-ended)
- přepočet ADC raw → napětí
- přepočet napětí → odpor NTC (dle orientace děliče)
- přepočet odpor → teplota (Beta model, B=3950, R25=10k; případně později Steinhart–Hart)
- výpis tabulky do Serial
- jednoduché flagy:
  - sensor_missing
  - out_of_range
  - adc_read_fail

---

## Out of scope (MUST NOT)

Do bench firmwaru teď nepatří:
- fancy architektura / modulární framework
- MCS / message model / transport / heartbeat
- GUI / HMI / webserver / Wi‑Fi
- persistent config / filesystem
- finální „production“ logging/telemetry formát

---

## Bench hardware (minimum)

Povinné:
- ESP32 (dev board) + USB kabel
- 4× ADS1115 moduly
- 16× NTC B3950 10k (podle repo baseline)
- 16× 10k 1% rezistor (podle repo baseline)
- breadboard / svorky / jumpery
- stabilní 3.3 V napájení pro ADC + děliče (doporučeno pro kompatibilní I2C úrovně)
- (volitelné, ale praktické) multimetr

Doporučené:
- teploměr nebo IR teploměr pro sanity-check
- horký vzduch / prst pro lokální ohřev jednoho senzoru
- štítky / fix na značení kanálů

---

## Praktická bench topologie

Preferovaná:
- NTC + 10k rezistory fyzicky v packu nebo v jeho bezprostřední blízkosti
- ADS1115 co nejblíž děličům (krátké analogové vedení)
- ESP32 externě (snadné ladění, nepřekáží u pack build)

Poznámka: dlouhé analogové vedení od NTC k ADC není ideální. Pro zítřek hlavně ať to běží a je to měřitelné.

---

## Pass / fail kritéria

PASS, pokud:
1) I2C se inicializuje bez pádu a firmware vypíše konfiguraci (SDA/SCL, I2C freq).  
2) Detekují se přesně 4 ADS1115 (ne 3, ne 5).  
3) Je možné číst všech 16 kanálů opakovaně (bez „adc_read_fail“).  
4) Při pokojové teplotě dávají všechny kanály teploty přibližně v realistickém rozsahu (typicky ~18–30 °C podle prostředí).  
5) Při ohřátí jednoho konkrétního NTC (prst / teplý vzduch) se:
   - jeho kanál viditelně změní (řádově aspoň několik °C)
   - ostatní kanály zůstanou relativně stabilní (malé drift/jitter ok)

FAIL, pokud:
- ADS1115 nejsou detekované / detekuje se špatný počet
- některé kanály nejdou číst (adc_read_fail)
- převod dává nerealistické teploty (např. -200 °C, 500 °C) při běžném zapojení
- „sensor_missing“ nejde vyvolat odpojením senzoru
- data jsou zjevně přeházená (kanál 0 reaguje na jiný fyzický senzor než si myslíš) a nemáš to zdokumentované

---

## Procedura testu krok za krokem

### Příprava
1) Zkontroluj fyzické zapojení podle `systems/btm/hardware/wiring-notes.md`.
2) Ověř, že všechny GND jsou společné (ESP32 ↔ ADS ↔ děliče).
3) Ověř, že ADS1115 jsou napájené správným napětím (doporučeně 3.3 V).

### Flash a první běh
1) Nahraj firmware `systems/btm/firmware/benchtest_ads1115_ntc/` do ESP32.
2) Otevři Serial monitor (115200).
3) Zkontroluj startovní výpis:
   - I2C init OK
   - detekce ADS a jejich adresy
   - start periodického výpisu tabulky

### Validace I2C a 4× ADS1115
1) Ověř, že firmware hlásí nalezené adresy 0x48–0x4B (nebo ty, které jsi fyzicky nastavil).
2) Pokud chybí zařízení:
   - ověř ADDR pin toho modulu
   - ověř SDA/SCL prohození
   - ověř pull-upy / napájení

### Čtení 16 kanálů
1) Sleduj tabulku kanálů 0–15:
   - raw ADC
   - V
   - R_ohm
   - T_C
   - flagy
2) Hledej:
   - „zaseklé“ kanály (raw se nikdy nemění)
   - kanály mimo smysl (V blízko 0 nebo Vcc)
   - výpadky (adc_read_fail)

### Sanity-check teploty
1) Nech systém ustálit 30–60 s.
2) Vyber *jeden* kanál a ohřej NTC (prst / teplý vzduch).
3) Očekávání:
   - T_C toho kanálu jde nahoru
   - ostatní kanály se hýbou minimálně

### Fault injection (flagy)
1) Odpoj jeden NTC vodič (simulace „sensor missing“):
   - očekávej `sensor_missing=1`
2) Zkrať vstup děliče na GND nebo VCC (simulace short/open):
   - očekávej `sensor_missing=1`
3) Pokud máš nastavené temp limity:
   - ohřej / ochlaď tak, aby se dostalo mimo rozsah → `out_of_range=1`

---

## Mermaid flowchart (procedura)

```mermaid
flowchart TD
  A[Zapoj HW] --> B[Power + společná GND]
  B --> C[Flash bench firmware]
  C --> D[Serial monitor: start log]
  D --> E{I2C init OK?}
  E -- ne --> E1[Opravit SDA/SCL/power/pull-upy] --> C
  E -- ano --> F{Nalezeny 4× ADS1115?}
  F -- ne --> F1[Opravit ADDR/adresy/I2C link] --> C
  F -- ano --> G[Čti 16 kanálů]
  G --> H{adc_read_fail?}
  H -- ano --> H1[I2C stabilita/power/rušení] --> C
  H -- ne --> I[Přepočet V→R→T]
  I --> J{Teploty realistické?}
  J -- ne --> J1[Ověřit orientaci děliče + Vcc + Rfixed + Beta] --> C
  J -- ano --> K[Ohřát 1 senzor]
  K --> L{Reakce jen na správném kanálu?}
  L -- ne --> L1[Opravit mapování kanálů / kabeláž] --> C
  L -- ano --> M[Zdokumentovat bench log + závěry]

  ```

## Referenční sanity tabulka (B3950, R25=10k, Rfixed=10k, Vcc=3.3 V, ADS FS=±4.096 V)
Pozn.: Výsledek napětí a raw závisí na orientaci děliče.

Teplota	R_NTC (ohm)	Vout (Rfixed top, NTC bottom)	Raw @FS=4.096V	Vout (NTC top, Rfixed bottom)	Raw @FS=4.096V
25 °C	~10000	~1.650 V	~13200	~1.650 V	~13200
40 °C	~5300	~1.143 V	~9147	~2.157 V	~17253

## Co z bench testu uložit (aby to nebylo jen „vypadalo to dobře“)

**Pack build log (šablona)**

- Datum:
- Konfigurace: 4P4S
- Typ článků:
- Způsob uchycení NTC (popis + foto):
- Rozmístění senzorů / ID:
- Poznámky (mechanika, izolace, vedení kabelů):

**- **BMS comparison log (šablona)**
- Cell / group ID	BMS napětí (V)	Externí měření (V)	Rozdíl (mV)	Poznámka

**BTM bench log (šablona)**
- ADS addr	Channel (AIN0-3)	Global ID (0-15)	Raw	V	R (ohm)	T (°C)	sensor_missing	out_of_range  
