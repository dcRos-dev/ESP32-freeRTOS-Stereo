# ESP32 WAV Audio Player (FreeRTOS & DAC DMA)

Un riproduttore audio basato su **ESP32** e framework **ESP-IDF** (C++). Il progetto legge un file WAV da una scheda MicroSD e lo riproduce tramite il DAC interno a 8-bit utilizzando l'accesso diretto alla memoria (DMA) e il clock di precisione APLL, garantendo un audio fluido e senza interruzioni.

L'architettura software è basata su **FreeRTOS**, con task separati per il controllo hardware (pulsante) e la riproduzione audio, sincronizzati tramite semafori binari.

## Funzionalità
* Lettura di file `.wav` direttamente da MicroSD tramite bus SPI.
* Riproduzione audio asincrona gestita da DMA (Direct Memory Access).
* Utilizzo dell'APLL clock per frequenze di campionamento precise (16000 Hz) senza crash del divisore di clock.
* Architettura Multi-Tasking con FreeRTOS (nessun blocco della CPU).
* Debounce hardware/software per la lettura sicura del pulsante tramite GPIO.
* Logica orientata agli oggetti (OOP) in C++.

## Requisiti Hardware

* **Scheda:** ESP32 Development Board (testato su Rev 3.1)
* **Storage:** Modulo MicroSD Card (SPI)
* **Audio:** 
  * Speaker/Cassa (preferibilmente amplificata o tramite modulo PAM8403 per un volume ottimale)
  * Condensatore elettrolitico (es. 10µF) per filtrare la corrente continua (DC blocking)
* **Input:** Un pulsante (Push Button) normalmente aperto

### Cablaggio (Pinout)

| Componente | Pin ESP32 | Note |
| :--- | :--- | :--- |
| **MicroSD MOSI** | `GPIO 23` | SPI Data In |
| **MicroSD MISO** | `GPIO 19` | SPI Data Out |
| **MicroSD CLK** | `GPIO 18` | SPI Clock |
| **MicroSD CS** | `GPIO 5` | SPI Chip Select |
| **Pulsante (Play)** | `GPIO 32` | Collegato tra GPIO 32 e GND (Usa la resistenza di Pull-Up interna) |
| **Audio Out (DAC)** | `GPIO 25` | Collegare al polo positivo del condensatore. Il polo negativo va alla cassa. |
| **Cassa (GND)** | `GND` | Chiusura del circuito audio |

> **Nota importante sul pulsante:** Evitare di usare i GPIO di "Strapping" (come il GPIO 2) per i pulsanti di input, altrimenti l'ESP32 potrebbe non avviarsi correttamente o ignorare l'input a causa delle resistenze fisiche saldate sulla scheda.

## Preparazione del file Audio (FONDAMENTALE)

Il DAC interno dell'ESP32 è un convertitore a 8-bit e non può leggere file audio standard o stereo. Se carichi un file WAV normale, sentirai solo un forte rumore distorto.

Usa **Audacity** (gratuito) per formattare il tuo file audio prima di metterlo sulla SD:
1. Importa il tuo brano su Audacity.
2. Vai su *Tracce > Mix > Mixa Stereo in Mono*.
3. Imposta la **Frequenza di Campionamento (Project Rate)** a `16000 Hz`.
4. Clicca su *File > Esporta audio...* e imposta:
   * **Formato / Intestazione:** WAV (Microsoft)
   * **Codifica:** **Unsigned 8-bit PCM** (Senza segno, fondamentale!)
5. Rinomina il file in `audio.wav` e copialo nella cartella radice della MicroSD.

## Installazione e Utilizzo

Questo progetto è sviluppato con l'estensione **Espressif IDF** (v5.0 o superiore).

1. Clona questo repository:
   ```bash
   git clone [https://github.com/TUO-NOME-UTENTE/ESP32-WAV-Player.git](https://github.com/TUO-NOME-UTENTE/ESP32-WAV-Player.git)
