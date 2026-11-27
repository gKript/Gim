# Cifratura di Gim: piano di lavoro (2026-11-27)

Stato: PIANO, nessun codice scritto per questa parte. Tutto da compilare e provare su Proxima o Ancarlo: sul PC di lavoro non ci sono compilatori.

## Obiettivo

Cifrare file e messaggi di passaggio, per impedire la lettura occasionale. NON per dati sensibili (codici, credenziali, dati personali di terzi): per quelli servono strumenti dedicati e una revisione esterna.

## Decisioni prese

1. **Cifratura dei dati: simmetrica.** Un solo algoritmo autenticato, ChaCha20-Poly1305 (RFC 8439). Implementato in Gim da zero, senza librerie esterne. Vettori di test dell'RFC.
2. **Una chiave casuale a 256 bit per ogni file.** Mai riusata. Generata con la sorgente di casualità del sistema operativo.
3. **Nonce casuale per ogni cifratura**, salvato in chiaro nel file. Mai riusato con la stessa chiave.
4. **Due modi per ottenere la chiave del file. Il cifrario dei dati è lo stesso.**
   - **Modo A (password):** PBKDF2-HMAC-SHA256 con sale casuale. Il risultato protegge la chiave del file.
   - **Modo B (chiavi pubbliche/private):** X25519 tra la propria privata e la pubblica dell'altro. Il segreto condiviso, con una funzione di derivazione, diventa la chiave del file. Le chiavi pubbliche servono solo a ottenere la chiave, non a cifrare il file.
5. **Firma opzionale: Ed25519.** Il file dichiara solo se è firmato (campo sì/no) e contiene la firma (64 byte). NON contiene la chiave pubblica del firmatario.
6. **Identificazione del mittente:** il ricevente prova la firma con ciascuna chiave della propria rubrica.
   - Una verifica riesce: mittente noto.
   - Nessuna riesce, ma il file è firmato: firmatario sconosciuto, identità non verificabile.
   - Non firmato: file anonimo.
7. **Rubrica:** i contatti si aggiungono solo dopo aver confermato l'impronta della chiave pubblica con un canale diverso (di persona, telefono). Una firma valida non vale fiducia: la fiducia sta nella rubrica.

## Difetti noti dell'implementazione attuale (da NON riusare così com'è)

Vedi `include/gim_gkp_cryptography.h` e `src/gim_gkp_cryptography.cc`:
- `advanced_password()` usa nonce vuoto: stesso flusso per tutti i file con la stessa password. XOR di due cifrati rivela XOR dei due in chiaro.
- La chiave è derivata direttamente dalla password, con 4096 round di SHA-512: veloce da attaccare su GPU. Non è una KDF adatta alle password.
- `simple_password()`: XOR con la password ripetuta. Solo offuscazione.
- La password resta in chiaro in memoria per tutta la vita dell'oggetto (`gim_password_obj`).
- `gim_mersennetwister`: il seme ha 32 bit, e su Windows `MTRand::seed()` ripiega su tempo e clock (prevedibili). Non usare per chiavi.
- `gim_cnc`: codifica con alfabeto derivato dalla password; decodifica non implementata; vedi `SYNC/TODO_GIM.md`.

## Formato del file (da definire in dettaglio, passo 0)

Campi previsti, nell'ordine:
- magic e versione
- modo (A password, B chiavi)
- parametri KDF (iterazioni, lunghezza del sale) e sale (modo A)
- chiave pubblica effimera del mittente (modo B)
- nonce
- chiave del file cifrata (con la chiave derivata)
- flag firmato (sì/no) e firma Ed25519 (se firmato)
- testo cifrato
- tag di autenticazione Poly1305

Da fissare: ordine dei byte, lunghezza massima, gestione del file troncato.

## Passi

0. Specifica del formato in questo file (o in `SYNC/FORMATO_FILE.md`).
1. Sorgente di casualità: wrapper per `BCryptGenRandom` (Windows) e `getrandom()` (Linux). Eccezione alla regola "classi Gim prima delle chiamate di sistema": va motivata nel codice. Test: lunghezza, valori non tutti zero, due chiamate diverse.
2. Verifica di `gim_sha256` e `gim_sha512` con i vettori NIST.
3. HMAC-SHA256 e PBKDF2-HMAC-SHA256, con i vettori pubblicati.
4. ChaCha20 e Poly1305, poi ChaCha20-Poly1305, con i vettori RFC 8439.
5. File con password (modo A): chiave casuale per file, nonce casuale, chiave del file protetta con PBKDF2. Test: andata e ritorno; password sbagliata rifiutata; un byte alterato rifiutato.
6. Scambio A→B (modo B): X25519 (RFC 7748) e derivazione della chiave.
7. Firma Ed25519 (RFC 8032) e identificazione con la rubrica.

Ogni passo: test in `test/`, voce in `SYNC/TODO_GIM.md`, nessuna chiusura senza test verdi su Proxima o Ancarlo.

## Da fare appena c'è un PC con compilatore

- Compilare e far girare i test di `gim_sha256` / `gim_sha512` (passo 2) prima di usarli.
- Verificare che `gim_gkp_cryptography` esistente non sia usato per dati nuovi.
- Scegliere il nome dei file del formato e il modo di indicare la versione.

## Domande ancora aperte

- Il formato deve supportare file molto grandi (streaming) o solo file in memoria? Influisce sull'implementazione di Poly1305.
- Si vuole anche la cifratura dei messaggi brevi (testo), o solo dei file?
- Il programma di rubrica: dove si salvano le chiavi pubbliche, e in che formato (`gim_kvalue`?).
