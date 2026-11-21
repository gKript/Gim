# Contratto di comunicazione Gim <-> PicGIM

Copia identica in `PicGim/SYNC/` e `Gim/SYNC/`. Se cambi qualcosa di qui sotto
o del codice che lo implementa, aggiorna il contratto in ENTRAMBI i repo e
aggiungi una voce in `SYNC_LOG.md` (stessa voce in entrambi).
Verifica: `powershell -ExecutionPolicy Bypass -File SYNC/check_sync.ps1`.

Stato lato Gim: **CRC-8 SSP e CRC-32 GCP implementati** in `gim_picgim_obj` (`crc8_ssp`, `crc32_gcp`), testati sui vettori. SSP e GCP lato PC: ancora da scrivere.

## 1. SSP - Simple Serial Protocol  (PicGIM: `pgim_ssp.h/.c`)
Handshake byte per byte (ogni byte inviato e' confermato da una risposta):

| Passo | Mittente | Byte | Risposta attesa |
|---|---|---|---|
| Header | TX | `'K'` | `'k'` |
| Lunghezza | TX | HB, LB (uint16 big-endian) | nessuna |
| Dati | TX | N byte | nessuna |
| CRC (se `PG_SSP_CRC_ENABLE`) | TX | CRC-8 dei dati | `PG_SSP_OK_CRC` (=55) oppure `PG_SSP_ERROR_CRC` (=54) |
| Footer | TX | `'X'` | `'x'` |

- Lunghezza dati: 1..65535.
- CRC-8: init `0x00`, riflesso (LSB first), polinomio `0x8C` (Dallas/Maxim), nessun xorout.
- Codici errore (`enum pg_ssp_error`, base 48 = '0'): NO_ERROR 48, WRONG_HEADER 49,
  WRONG_FOOTER 50, WRONG_HEADER_REPLY 51, WRONG_FOOTER_REPLY 52, RX_TIMEOUT 53, ERROR_CRC 54, OK_CRC 55.

## 2. GCP - General Communication Protocol  (PicGIM: `pgim_gcp.h/.c`, `pgim_gcp_setup_public.h`, v0.1)
Letto da `pgim_gcp.c` (righe di riferimento tra parentesi). Verificato sul codice, non su un trasferimento reale.

### 2.1 Livello di comando (escape + control byte)
Ogni comando: TX `'>'` -> RX `'^'`; poi TX control byte -> RX control + 32 (minuscolo).
Ogni byte e' quindi confermato (`pg_gcp_tx_control_byte`, ~1407). Reply diverso dall'atteso: errore `TX_CONTROL_FAILED`.
Timeout: escape = `TIMEOUT_MS_TX` 5000 ms; reply del control byte = `TIMEOUT_MS_DIAL` 1000 ms;
attesa CRC = `TIMEOUT_MS_CRC` 10000 ms; attesa comando lato RX = `TIMEOUT_MS_RX` 5000 ms.
Con `PG_GCP_AUTORESET_TIMEOUT` un timeout provoca un reset (TX: remoto+locale, RX: solo locale).

### 2.2 Byte di controllo (tutti preceduti da `'>'`)
| Cmd | Reply | Significato | Payload dopo il reply |
|---|---|---|---|
| `E` | `e` | engage (apre sessione) | nessuno |
| `X` | `x` | engage_end (chiude; solo fuori da data mode) | nessuno |
| `V` | `v` | version | 8 byte, vedi 2.3 |
| `C` | `c` | config | 1 byte numero config, eco dal RX, poi `PG_OK` dal TX |
| `D` | `d` | data (entra in data mode) | dati grezzi |
| `Z` | `z` | data_end (esce da data mode) | nessuno |
| `K` | `k` | crc | 4 byte CRC, ciascuno confermato da `PG_GCP_WAITING_DATA` (101), poi 1 byte esito |
| `S` | `s` | status_sync | da verificare (`pgim_gcp.c:999`) |
| `M` | `m` | status_mod | da verificare (`:1085`) |
| `Q` | `q` | request | da verificare (`:1894`) |
| `G` | `g` | streaming ON/OFF | da verificare (`:1171`) |
| `R` | `r` | reset (remoto+locale) | da verificare (`:366`) |
| `A` | `a` | command | 1 byte comando, da verificare (`:1951`) |

Dispatcher RX (`pg_gcp_rx`, ~1570): fuori sessione accetta solo R, V, A, E. In sessione: C sempre;
in data mode D->Z (data_end) e `'>'` come dato; fuori data mode D, X, S, M, Q, K.

### 2.3 Sequenza di una trasmissione (`pg_gcp_send`, ~2032)
`engage` (invia prima VERSION, poi ENGAGE) -> `data` -> `config(n)` -> dati -> `data_end`
-> [`status_mod(FULL)` se stringa] -> [`crc` se richiesto] -> `engage_end` (a cura dell'utente).
- Version: TX `V`, poi 8 byte: nome app (4 caratteri), app major, app minor, PicGIM major, PicGIM minor.
  RX confronta con i propri (`PG_GCP_APPLICATION_NAME` default `"USER"`, app 0.1, `PG_MAJOR`/`PG_MINOR`)
  e risponde 1 byte `PG_OK` se tutto coincide; altrimenti non risponde (il TX va in timeout).
- Dati `PG_GCP_BUFFER`: `xbuffer_length` byte, lunghezza fissa per config.
- Dati `PG_GCP_STRING`: caratteri + terminatore `0x00`. Un dato `'>'` va raddoppiato
  (il RX vede `'>'`, risponde `'^'`; se il byte dopo e' ancora `'>'` lo salva come dato: `pg_gcp_rx_escape`).
- I byte di dato non hanno ack. Fuori dalla data mode i byte non-escape sono ignorati.
- Config 0..16: ognuna e' un buffer di lunghezza e modo fissi, definiti in `pgim_gcp_setup_public.h`
  (es. config 01 = 4 byte). Lato PC servira' la stessa tabella config -> lunghezza/modo.

### 2.4 CRC GCP
Campo di 4 byte (`_pg_Uint32_VAL.v[0..3]`), inviati in ordine di indice 0..3 (PIC18 little-endian: `v[0]` = byte meno significativo).
**ATTENZIONE: l'algoritmo attuale NON e' un CRC-32** (`pg_gcp_crc32_calc`, ~1848): parte da 0 e per ogni byte
fa `if (m % 1) crc -= b; else crc *= b;`. Poiche' `m % 1` e' sempre 0 esegue sempre `crc *= b`, quindi il
risultato e' sempre `0x00000000`. Il controllo passa sempre ed e' inutile.
**DECISO (2026-11-20): CRC-32 IEEE 802.3** (zlib/Ethernet): poly riflesso `0xEDB88320`, init `0xFFFFFFFF`, xorout `0xFFFFFFFF`, input e output riflessi.
Vettore di test: CRC32("123456789") = `0xCBF43926`; sul filo i 4 byte vanno `26 39 F4 CB` (meno significativo per primo, `v[0]` PIC18). Da implementare identico in PicGIM (`pg_gcp_crc32_calc`) e in Gim: **stato PicGIM: FATTO in `pgim_gcp.c` (algoritmo verificato sul vettore, NON compilato con C18 ne' provato su PIC); Gim: FATTO in `gim_picgim.cc` (`crc32_gcp`), testato sul vettore; non ancora collegato al protocollo**.
Lunghezza su cui si calcola: TX = `xbuffer_length`; RX = `xbuffer_index - 1`.
Esito: `PG_OK` se coincide, altrimenti `PG_GCP_ERROR_CRC_FAILED`.

### 2.5 Costanti
- Ritorni `PG_OK`/`PG_NOK`: valori in `pgim_defines.h`, DA VERIFICARE (il byte di esito del CRC e' `PG_OK`).
- `enum _pg_gcp_return_code_enum` base 100: DUMMY 100, WAITING_DATA 101, STATUS_WAITING 102, STREAMING_ON 103,
  STREAMING_OFF 104, poi gli errori nell'ordine di `pgim_gcp.h`.
- Stato buffer: `e` empty, `i` incomplete, `f` full, `c` crc, `r` read, `?` unknown.

## 3. Problemi noti lato PicGIM (da non replicare in Gim)
- **GCP CRC sempre 0** (vedi 2.4). RISOLTO in PicGIM il 2026-11-20.
- **`pg_gcp_tx_string`** (~1541): `sidx` si incrementa solo quando il carattere e' `'>'`, quindi con una
  stringa normale il ciclo `while(tstring[sidx])` non termina. **CONFERMATO (2026-11-20).**
- **SSP `pg_ssp_tx`/`pg_ssp_rx`**: indice `_pg_Uint8` confrontato con lunghezza uint16: oltre 255 byte il ciclo non termina.
- **SSP `pg_ssp_tx`**: con CRC chiama `pg_lcd_hd44780_put_char` incondizionatamente (debug lasciato nel codice).
- `pg_gcp_send_byte_serial` dichiara `_pg_Uint8` ma non ritorna nulla.
- `pg_gcp_engage`/`pg_gcp_rx_engage` definite due volte: **RISOLTO (2026-11-20)**, attiva quella a riga ~740; la copia a riga ~2480 e' in un commento `/* */` (2474-2538).
- `pg_gcp_flag_set/get` usano il tipo `_pg_Uin8` (refuso).

## 4. Normalizzazione degli ingressi (in implementazione: Gim genera la tabella dei canali; PicGIM non ancora compilato con C18)
Scopo: portare N sensori di tipi diversi in un unico vettore di ingresso comparabile per la rete.
Tre livelli separati: (1) driver di acquisizione per tipo di sensore, basati sui moduli PicGIM
(`pgim_adc`, `pgim_i2c`, `pgim_spi`, `pgim_3wire`, `pgim_sensor`, `pgim_amg88xx`, `pgim_encoder`);
(2) classe di normalizzazione generica; (3) vettore di ingresso consegnato alla rete.

### 4.1 Larghezza dell'uscita (decisa)
- La larghezza e il segno dell'uscita sono **costanti di compilazione per progetto**, non cambiano a runtime.
- Macro di configurazione in `pgim_ai_input_setup_public.h`: `PGIM_AI_OUT_BITS` = `8` oppure `16`.
  Il `typedef` del vettore di ingresso deriva da questa macro.
- Il calcolo intermedio e' sempre a 16 bit; la riduzione a 8 bit avviene solo in uscita.
- Ogni modello addestrato su Gim dichiara la propria larghezza. L'header generato da Gim contiene
  la stessa costante, e un `#error` in compilazione se non coincide con quella del PIC.
- Segno: da definire (con o senza segno) per il prototipo; default proposto: con segno.

### 4.2 Parametri per canale (da esportare da Gim)
Per ogni canale, in ordine fisso: nome, unita' fisica, `min`, `max` (intervallo atteso in addestramento),
scala e offset interi precalcolati, tipo di normalizzazione (min-max proposto; z-score da confermare).
Valore di fallback per guasto e flag di errore per canale. Tabella in ROM accanto ai pesi.

**Regola di calcolo dello scale (da rispettare su Gim, implementata in `pgim_ai_input.h`):**
- `OUT_MAX` = 127 (8 bit) oppure 32767 (16 bit), uscita con segno, intervallo -OUT_MAX..+OUT_MAX.
- `scale = ceil( 2*OUT_MAX * 2^shift / (in_max - in_min) )`, con `shift` il piu' grande in 0..16 tale che `scale <= 65535`.
- `ceil` e non `floor`: con `floor` il massimo darebbe OUT_MAX-1.
- Conversione PIC: `diff = raw - in_min` (raw gia' saturato), `out = ((diff * scale) >> shift) - OUT_MAX`.
- Esempio di controllo (8 bit, in 0..1023): scale 16272, shift 16 -> raw 0 => -127, raw 512 => 0, raw 1023 => 127.

### 4.4 Generazione del modello lato Gim (IMPLEMENTATA in Gim: `gim_ai_train.cc`, tool `gimaitrain`; verificata sul prototipo, mancano dataset reale e smoke test CI)
Obiettivo: da un dataset etichettato, Gim addestra la rete e produce un unico header `pgim_ai_model.h`
che il progetto PIC include. Il PIC non deve sapere nulla di addestramento: riceve solo costanti.

**Input dell'addestramento**
- Campioni grezzi per canale, nello stesso formato con cui il PIC legge i driver (interi, unita' grezze).
  Un campione = N valori `raw` + `status` per canale + etichetta di classe.
- Lo stesso ordine dei canali e le stesse larghezze del firmware. Se un canale cambia, il dataset va rifatto.

**Passi su Gim**
1. **Statistiche per canale** sul solo set di addestramento: `in_min`, `in_max` = minimo e massimo dei valori
   validi (`status` ok). Opzionale: percentili 0.5% / 99.5% per ignorare outlier, ma va dichiarato nel log.
2. **Scale e shift** per canale con la regola della sezione 4.2 (ceil, shift <= 16, scale <= 65535).
   Il dataset va poi normalizzato con la stessa formula intera del PIC, non con float.
3. **Normalizzazione intera del dataset**: ogni campione viene convertito come farebbe il PIC, cosi' la rete
   vede esattamente gli stessi numeri a runtime (niente differenza tra training e firmware).
4. **Addestramento** in float sul PC, sui valori gia' normalizzati e convertiti in intero.
5. **Quantizzazione dei pesi** in int8 (o int16 se `PGIM_AI_OUT_BITS == 16`), con una scala per layer.
   Le attivazioni intermedie usano una funzione intera (es. ReLU con saturazione), definita in questa sezione.
6. **Simulatore intero su PC**: riproduce bit per bit il calcolo del PIC (normalizzazione, layer, saturazione).
   Il modello e' accettato solo se l'accuratezza del simulatore intero e' vicina a quella float (soglia da fissare nel log).
7. **Vettori di test**: per alcuni campioni, Gim salva ingresso grezzo e uscita attesa del simulatore intero.
   Il firmware PIC deve dare la stessa uscita: e' il test di accettazione del porting.

**Output: `pgim_ai_model.h`** (generato, non modificare a mano)
- Il file e' specifico per un progetto firmware. Gim lo scrive **direttamente** nella cartella include del progetto
  PIC di destinazione, indicata nella configurazione del progetto Gim. Nessun passaggio manuale di copia.
- Header guard e commento con data, versione del modello, hash del dataset.
- `#if ( PGIM_AI_OUT_BITS != <valore> ) #error` con il valore dichiarato dal modello.
- Tabella canali: array `const pg_ai_channel_cfg PG_AI_CHANNELS[ N ]` con `in_min`, `in_max`, `scale`, `shift`,
  `fallback`, `enabled`, nell'ordine dei canali.
- Pesi e bias: array `const` per layer, con le dimensioni (`PG_AI_LAYER0_IN`, `PG_AI_LAYER0_OUT`, ...).
- Scale di quantizzazione per layer e `shift` di riscalatura, come costanti intere.
- Vettori di test: `PG_AI_TEST_IN[ ]` e `PG_AI_TEST_OUT[ ]`.

**Vincoli per il firmware (da rispettare nella rete su PIC)**
- Nessun float: solo interi, con accumulatori a 32 bit (`_pg_Uint32` / `_pg_int32`).
- La dimensione massima dell'input e' `PGIM_AI_MAX_CHANNELS` (8 di default, max 16).
- Dimensione dei pesi: il modello deve stare in ROM accanto al firmware; la RAM richiesta per le attivazioni
  va dichiarata nell'header (`PG_AI_RAM_BYTES`).

### 4.3 Stato canale
- Guasto del sensore (es. NTC scollegato): valore di fallback + bit in maschera canali in errore.
- Fuori intervallo atteso: saturazione all'intervallo di uscita + flag separato dal guasto.

### 4.5 Inferenza intera (implementata su PicGIM in `pgim_ai_net.c` / `.h`, non compilata con C18)
- Rete fully connected. Per ogni layer `l` e neurone `o`: `acc = b[o] + sum_i( w[o][i] * x[i] )` in `_pg_int32`.
- Arrotondamento e shift: `acc = ( acc + 2^(shift-1) ) >> shift` (nessun arrotondamento se `shift = 0`), shift aritmetico.
- ReLU solo se il layer lo dichiara (`relu = PG_YES`), poi saturazione a `-OUT_MAX .. +OUT_MAX`. L'ultimo layer usa la stessa regola, senza ReLU.
- Classe = indice dell'uscita piu' alta dell'ultimo layer; a parita' vince l'indice piu' basso.
- Pesi int8 (8 bit) o int16 (16 bit), simmetrici (`|w| <= OUT_MAX`). Bias gia' in scala dell'accumulatore.
- Gim deve verificare per ogni neurone `sum_i |w[o][i]| * OUT_MAX + |b[o]| < 2^31` e rifiutare il modello se non vale.
- Tabelle (canali, pesi, bias, layer, vettori di test) in ROM come `static const` (header generato da Gim, nessun `rom`: e' C18). Puntatori a ROM: `const __rom` (XC8). Il tipo `pg_ai_out_t` e' `signed char` esplicito (C18: `char` e' senza segno).
- Vettori di test: `PG_AI_TEST_RAW` (grezzi), `PG_AI_TEST_OK`, `PG_AI_TEST_X` (normalizzati), `PG_AI_TEST_SCORES`, `PG_AI_TEST_CLASS`. Il firmware deve riprodurli bit per bit.
- Da verificare su hardware: shift aritmetico di C18 su valori negativi.

### 4.7 Dataset di riferimento e test end-to-end (2026-11-21)
- Dataset: `SYNC/ai_dataset/pgim_ai_dataset_v1.csv`, 900 campioni, 8 canali, 3 classi, formato sez. 4.4 (`label,ch0,ch0_ok,...`). SHA-256 `e0b18c59daa896af22dba647ba9d520224b621593f20fd477085f89f2c1dc5c9`.
- Addestramento (Gim): `gimaitrain --dataset <csv> --out <h> --report <txt> --out-bits 8 --hidden 8 --seed 1` → ACCETTATO (test: float 95.6%, intero 94.7%).
- Verifica (PicGIM): l'header generato compila con XC8 V4.00 per PIC18F4620 senza modifiche a mano; simulazione gpsim 0.32.1 di 6 milioni di passi: 16/16 vettori, `pg_test_errors` = 0, `pg_test_done` = 1.
- Criterio di accettazione: `pg_test_errors` = 0 e `pg_test_done` = 1, con gli indirizzi dalla mappa del linker (`-Wl,-Map`).

### 4.6 Ripartizione dei compiti (2026-11-21)
- **PicGIM**: solo inferenza intera (`pgim_ai_input`, `pgim_ai_net`). Riceve `pgim_ai_model.h` e non sa nulla di addestramento.
- **Gim**: tutto il resto della sezione 4.4 (statistiche, scale, normalizzazione intera del dataset, addestramento in float, quantizzazione, simulatore intero, vettori di test, scrittura di `pgim_ai_model.h` nel progetto PicGim).
- Il `pgim_ai_model.h` attuale e' un **prototipo** (8 ingressi, 2 layer, 3 classi, 13 vettori di test) generato dal prototipo Python di Gim, fuori dai repo. Va sostituito dal primo modello reale senza modifiche manuali a PicGim.
- Criterio di accettazione: il firmware deve riprodurre bit per bit `PG_AI_TEST_SCORES` e `PG_AI_TEST_CLASS` del modello generato.

**Implementazione Gim (2026-11-21, `scripts/gim_ai_train.py`)**
- Split train/test per shuffle (non stratificato). `in_min`/`in_max` dal solo training, solo campioni validi.
- Shift per layer: il piu' grande q in 0..16 tale che `|w_int| <= OUT_MAX` e, per ogni neurone, `sum|w_int|*OUT_MAX + |b_int| + 2^q < 2^31`. Pesi `w_int = round(w*2^q)`, bias `b_int = round(b*2^q*OUT_MAX)`.
- Ultimo layer scalato di `1/max|logit|` sul training, cosi' i logit stanno in [-1, 1] e non saturano; argmax invariato. Hidden: ReLU saturata a 1, come nel firmware.
- Accettazione: accuratezza intera (test) >= accuratezza float (test) - 0.02. Se no, nessun header viene scritto.
- Fallback 0 e `enabled` 1 per tutti i canali (da rivedere per i sensori reali).
- Simulatore intero = copia operazione per operazione di `pg_ai_input_normalize` e `pg_ai_net_infer`. Verificato: riproduce i 13 vettori del prototipo PicGim.

## 5. Altro materiale condiviso da verificare
| Tema | PicGIM | Gim |
|---|---|---|
| Endianness | `pgim_endian.c` | da cercare |
| Tipi custom | Custom-Type | `gim_custom_type.h` |
| Checksum | `pg_crc_8`, crc32 GCP | `gim_checksum.h` |
