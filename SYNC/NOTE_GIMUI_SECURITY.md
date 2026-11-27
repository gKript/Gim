# Nota: configurazione di Gim e dati sensibili

Scritta da: sessione Claude che lavora su GimUI (`i:\GitHub Asy\GimUI`), su richiesta dell'utente, 2026-11-22.
Non è una modifica al contratto Gim <-> PicGIM: per questo non è in `COMM_CONTRACT.md` né in `SYNC_LOG.md`.

## Cosa si è visto
Con il log attivo (`gim_main.log`), la cartella di configurazione di Gim risulta così:

- `<utente>\.gim\GimUI\gim.conf`, `alias.conf`, `gim_main.log`: la configurazione è separata per applicazione (il nome è quello passato a `gim_set_application_name` / `strcpy(gim_application_name, ...)`).
- `<utente>\.gim\environment.conf`: è **unico per tutte le app Gim** dello stesso utente. Elenca, per ogni applicazione, le sezioni e i percorsi (`home`, `programs`, `privatekeypath`, `otherkeyspath`, `configuration`, `debug_fname`).
- Senza nome di applicazione, Gim usa direttamente la radice `<utente>\.gim\`, condivisa da tutte le app senza nome. Gim stesso lo segnala con «Other applications that uses Gim will be damaged».

## Perché conta
- Qualunque app Gim (o qualunque programma che sappia leggere il formato) può leggere `environment.conf`, trovare la cartella di un'altra app e aprirne o modificarne il `gim.conf`. Non c'è isolamento tra app dello stesso utente.
- `privatekeypath` indica dove sono le chiavi private: il contenuto non è esposto, ma il percorso sì.
- Il rischio è basso finché tutte le app girano con lo stesso utente e i dati sono già dell'utente; cresce se in futuro Gim custodisce dati sensibili o se le app girano con utenti o permessi diversi.

## Suggerimenti per lo sviluppo
- Tenere le chiavi private fuori dalla cartella di configurazione, in una cartella dedicata con permessi ristretti (su Windows, ACL solo per l'utente).
- Valutare se `environment.conf` debba contenere i percorsi delle chiavi, o solo l'elenco delle applicazioni.
- Non usare la radice `.gim\` per le app senza nome: richiedere sempre un nome, o rifiutare l'avvio senza.
- Decidere se un'app dovrebbe poter leggere la configurazione di un'altra (oggi può farlo, per costruzione).

## Valutazione (2026-11-22, sessione Gim, su richiesta dell'utente)
Stato: **valutata**. Decisioni:

- **Giudizio generale**: la nota è più severa del rischio attuale. Gli utenti sono già chiusi nel proprio account, e il percorso della cartella delle chiavi non rivela il contenuto. Non c'è urgenza.
- **Da fare (piccolo)**: in `src/gim_environment.cc` la cartella delle chiavi viene creata con `mkdir(..., 0777)`. Va ridotta a `0700` sui sistemi POSIX. Su Windows i permessi POSIX non contano, quindi lì non cambia nulla.
- **Da fare solo se Gim custodirà dati sensibili**: tenere le chiavi fuori dalla cartella di configurazione, e usare ACL ristrette su Windows.
- **Da rivedere**: i punti su isolamento tra app, rifiuto degli avvii senza nome e lettura della configurazione di un'altra app. Per ora non li applico.
- **Da verificare**: la frase «Other applications that uses Gim will be damaged» non compare nei sorgenti di Gim (ricerca su `src` e `include`). Va confermata o tolta.
- **Posizione del file**: questa nota riguarda Gim e GimUI, non il contratto con PicGIM. Andrebbe spostata in una cartella di documentazione, non in `SYNC`. Non l'ho spostata: decide l'utente.
- **Questione aperta**: cosa rappresenta la "chiave privata". Oggi la cifratura di gkfp è a password, e `gim_private_key_obj` genera il file ma non viene usato dalla cifratura. Se serve come sale o segreto locale, la gestione cambia; se serve per scambi tra persone, serve una cifratura a chiave pubblica che Gim non ha. Decisione rimandata all'utente.
