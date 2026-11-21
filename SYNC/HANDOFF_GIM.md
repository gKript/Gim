# Handoff per la sessione Gim

Sei la sessione che lavora su **Gim** (`i:\GitHub Asy\Gim`, C++, lato PC).
Il progetto gemello **PicGIM** (`i:\GitHub Asy\PicGim`, C18, lato PIC18F) ha il firmware seriale in gran parte pronto e la classe di inferenza AI; Gim deve fornire il lato PC e l'addestramento.
Obiettivi: (a) far comunicare Gim con PicGIM via seriale; (b) generare il modello AI per il PIC. Tenere i due progetti allineati.

## Cosa leggere per primo
1. `SYNC\COMM_CONTRACT.md` = specifica di SSP, GCP e del modello AI. **E' la fonte di verita'.** Copia identica in `PicGim\SYNC\`.
2. `SYNC\SYNC_LOG.md` = registro dei cambiamenti. Aggiungi una voce (in entrambi i repo) ogni volta che cambi qualcosa di condiviso.
3. Sorgenti PicGIM di riferimento (sotto `PicGim\PicGIM_Sources\PicGIM\`):
   - SSP: `src\pgim_ssp.c`, `include\pgim_ssp.h`
   - GCP: `src\pgim_gcp.c`, `include\pgim_gcp.h`, `include\pgim_gcp_setup_public.h`
   - AI: `src\pgim_ai_input.c`, `include\pgim_ai_input.h`, `src\pgim_ai_net.c`, `include\pgim_ai_net.h`, `include\pgim_ai_model.h` (prototipo)

## Stato (aggiornato 2026-11-21)
- **Gim**: esistono gia' `crc8_ssp` e `crc32_gcp` in `gim_picgim_obj` (`src\gim_picgim.cc`), testati sui vettori. Non sono ancora collegati a un protocollo. SSP e GCP lato PC: da scrivere.
- **PicGIM seriale**: CRC GCP corretto (CRC-32 IEEE 802.3), non compilato con C18 ne' provato su PIC. Bug aperti in contratto sez. 3.
- **PicGIM AI**: classe di inferenza intera (`pgim_ai_input`, `pgim_ai_net`), non compilata con C18. `pgim_ai_model.h` e' un **prototipo** (8 ingressi, 2 layer, 3 classi, 13 vettori), generato dal prototipo Python di Gim fuori dai repo.
- **Ripartizione AI** (contratto sez. 4.6): PicGIM fa l'inferenza; **Gim fa l'addestramento** e genera `pgim_ai_model.h`.

## Cosa fare
### A. Comunicazione seriale
1. **SSP lato PC** dentro `gim_picgim_obj` (o una classe collegata): handshake `K`/`k`, lunghezza uint16 big-endian, dati, CRC-8 con esito `55`/`54`, footer `X`/`x`. Timeout e codici errore come da contratto. Usa `crc8_ssp` gia' presente.
2. **GCP lato PC**: escape `>`/`^`, control byte con reply +32, sequenza engage -> data -> config -> dati -> data_end -> crc -> engage_end, version a 8 byte, tabella config 0..16 (vedi `pgim_gcp_setup_public.h`). Usa `crc32_gcp` gia' presente.
3. I comandi S, M, Q, G, R, A sono marcati "da verificare" nel contratto: leggi il codice PicGIM indicato (numeri di riga nel contratto) e **completa il contratto** prima di implementarli.
4. Test con vettori noti (CRC-8, CRC-32, un frame SSP completo con byte attesi). Se possibile prova su loopback o su un PIC reale.

### B. Addestramento e modello AI
> Stato 2026-11-21: il generatore e' in C++ (`src\gim_ai_train.cc`, tool `gimaitrain`), compilato con MinGW GCC 16 + CMake/Ninja. Verificato: gli header generati, riletti dal simulatore Python (verificato sul prototipo PicGim, 13/13), danno 0 discrepanze. Lo script `scripts\gim_ai_train.py` resta come riferimento. Mancano: un dataset reale, e lo smoke test della CI (richiede bash).
5. Implementa i passi 1-7 della sezione 4.4 del contratto: statistiche per canale, scale e shift (regola sez. 4.2), normalizzazione intera del dataset, addestramento in float, quantizzazione int8/int16 per layer, simulatore intero bit per bit, vettori di test.
6. Verifica per ogni neurone `sum_i |w[o][i]| * OUT_MAX + |b[o]| < 2^31` e rifiuta il modello se non vale (sez. 4.5).
7. Genera `pgim_ai_model.h` direttamente nella cartella include di PicGIM (sez. 4.4), sostituendo il prototipo. Non copiarlo a mano.
8. Criterio di accettazione: il firmware PicGIM riproduce bit per bit `PG_AI_TEST_SCORES` e `PG_AI_TEST_CLASS`.

## Regole
- **Non replicare i bug di PicGIM** elencati in contratto sez. 3 (indici a 8 bit su lunghezze a 16, `tx_string` che non termina, ecc.).
- Se scopri che il contratto e' sbagliato o ambiguo, **correggilo** (nelle due copie), aggiungi la voce nel log e dillo all'utente. Non adattare Gim in silenzio.
- Se serve un cambiamento lato PicGIM (es. un bug), non modificare il codice dell'altro repo: scrivilo in `SYNC_LOG.md` come "PicGIM: da allineare" e avvisa l'utente. Aggiornare `SYNC\` (contratto, log) in entrambi i repo e' previsto.
- Prima di dichiarare finito: `powershell -ExecutionPolicy Bypass -File SYNC/check_sync.ps1` deve dare tutto `ok`.
- Stile Gim: header GPL, `gim_error->set(...)` per gli errori, build CMake gia' presente (porta Windows). Non committare senza che l'utente lo chieda.
- La cartella `Gim\SYNC\` e' creata ma **non ancora committata**.
