# Formato del file cifrato di Gim (passo 0 del piano di cifratura)

Stato: SPECIFICA, nessun codice scritto. Riferimento: `SYNC/CRITTOGRAFIA_PIANO.md`.
Scritto per: chi implementerà il contenitore e chi lo leggerà per verificarlo.

Il formato lavora su **buffer**, non su file. Il contenitore (file, rete, `gim_gkdp_obj` o altro) è una scelta separata.

## Cosa protegge e cosa no

Cifra il contenuto e rileva le modifiche. Non protegge da chi conosce la password: per dati sensibili servono strumenti dedicati (vedi piano).

## Primitive

- Cifratura dei dati e del wrapping: ChaCha20-Poly1305 (RFC 8439).
- Derivazione della chiave dalla password: PBKDF2-HMAC-SHA256 (RFC 7914 per i vettori).
- Casualità: sorgente del sistema operativo (passo 1 del piano).
- Interi nell'intestazione: little-endian, senza segno, dimensione fissa.

## Modo

Questa versione implementa solo il **modo A (password)**. Il campo `mode` è già previsto per il modo B (chiavi X25519), che è rimandato: un file con `mode = 0x02` viene rifiutato con errore esplicito, senza provare a decifrarlo.

## Intestazione (109 byte, nell'ordine)

| Offset | Dim | Campo | Valore / regola |
|---|---|---|---|
| 0 | 4 | `magic` | `47 4B 52 54` (`GKRT`) |
| 4 | 1 | `version` | `0x01` |
| 5 | 1 | `mode` | `0x01` = password. `0x02` = chiavi (riservato, rifiutato) |
| 6 | 1 | `flags` | bit 0 = firmato (riservato, deve valere 0 in questa versione). Gli altri bit a 0 |
| 7 | 1 | `reserved` | `0x00` |
| 8 | 1 | `kdf` | `0x01` = PBKDF2-HMAC-SHA256 |
| 9 | 4 | `kdf_iterations` | numero di iterazioni PBKDF2, uint32 little-endian |
| 13 | 16 | `salt` | casuale, nuovo per ogni file |
| 29 | 12 | `wrap_nonce` | casuale, nuovo per ogni file |
| 41 | 48 | `wrapped_key` | 32 byte di chiave del file cifrati + 16 byte di tag |
| 89 | 12 | `data_nonce` | casuale, nuovo per ogni file |
| 101 | 8 | `payload_length` | lunghezza in chiaro, uint64 little-endian |

Il payload cifrato segue l'intestazione, poi il tag finale di 16 byte.

Dimensione totale del contenitore: `109 + payload_length + 16` byte. Il controllo deve essere esatto: meno o più byte di così, il contenitore è rifiutato.

## Chiavi

1. `K = PBKDF2-HMAC-SHA256(password, salt, kdf_iterations, 32)`
2. `FK` = 32 byte casuali, una chiave per file, mai riusata.
3. `wrapped_key = ChaCha20-Poly1305(K, wrap_nonce, FK, AAD = byte 0..40)`
   L'AAD copre l'intestazione fino a `wrap_nonce` compreso.
4. `ciphertext + tag = ChaCha20-Poly1305(FK, data_nonce, payload, AAD = byte 0..108)`
   L'AAD è l'intestazione intera, compresi `wrapped_key` e `payload_length`.

Così nessun campo dell'intestazione (modo, versione, iterazioni, sale, lunghezza) può essere alterato senza che la verifica fallisca.

## Apertura (ordine obbligatorio)

1. Controllo della dimensione: almeno `109 + 16` byte, e `109 + payload_length + 16` esatto.
2. Controlli dei campi: `magic`, `version`, `mode == 0x01`, `kdf == 0x01`, `flags` bit 0 a 0, `reserved` a 0, `kdf_iterations` nel minimo consentito (vedi decisioni aperte).
3. Derivazione di `K` dalla password.
4. Verifica del tag su `wrapped_key`. Se non torna: errore "password errata o file alterato". Nessun testo viene restituito.
5. Verifica del tag sul payload. Il payload in chiaro si restituisce **solo dopo** che il tag è verificato.

Tutti i confronti dei tag sono a tempo costante. Chiavi e password intermedie si azzerano con una funzione che il compilatore non può eliminare.

## Errori

Un solo codice per "password errata o file alterato" (non distinguere i due casi, per non dare un oracolo). Codici separati solo per problemi di formato: magic, versione, modo non supportato, dimensione sbagliata, iterazioni sotto il minimo.

## Decisioni aperte

1. **Dimensione massima del payload.** Serve un limite per il buffer. Proposta: il limite della memoria disponibile, da fissare nel codice con una costante.
2. **Minimo di `kdf_iterations`.** Si sceglie misurando il tempo su Ancarlo, non da una cifra presa a memoria. Finché non è misurato, il controllo resta da scrivere ma il valore è `da fissare`.
3. **Classe nuova o modifica di `gim_gkp_cryptography`.** Vedi la domanda al punto precedente: il formato non dipende dalla risposta.

## Vettori di test

Da prendere dai testi delle RFC, non dalla memoria:
- RFC 8439 per ChaCha20-Poly1305;
- RFC 7914 per PBKDF2-HMAC-SHA256.

Test di apertura: andata e ritorno; password sbagliata; ogni byte alterato uno alla volta, tutti rifiutati; troncamento a ogni lunghezza; due cifrature dello stesso testo con output diversi; modo `0x02` rifiutato.
