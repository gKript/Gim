# Da finire (Gim)

Stato: aperto. Aggiornato 2026-11-22, dopo il commit f4494e9.

## Subito
- [ ] Push di f4494e9 su origin/windows-port (chiedere conferma prima).

## Log: livelli
- [ ] Revisione file per file dei livelli. Restano circa 100 MESSAGE con parole da errore (gim_error.cc, gim_memory.cc, resto di gim_db.cc) e circa 100 CRITICAL con testo informativo.
- [ ] Verificare se "SKIPPED" in gim_db.cc deve essere WARNING.
- [x] CRITICAL doppio: causa in gim_error_obj::get() (il blocco verbose_on_critical e il blocco always_verbose stampavano entrambi). Corretto 2026-11-22: il primo blocco ora non stampa se always_verbose e attivo. Verificato a runtime.
- [ ] Il messaggio "gim_home_obj::up" senza nome applicazione resta CRITICAL: decidere.

## Thread e campionamento
- [ ] gim_queue_obj (coda thread-safe), previsto per la lettura seriale.
- [ ] Controllo di sicurezza tra thread di gim_log e dei file (non ancora fatto).
- [ ] Ramo Linux di sample(): lettura /proc/stat, non ancora provato.

## Build e piattaforme
- [ ] Linux: compilare gim_serial e gim_rpi_gpio (non provato).
- [ ] gim_ai_train: decidere se logga con gim_error o resta su cout/cerr (oggi resta).

## Documentazione
- [ ] Controllo Doxygen di ogni classe (presenza di \brief e dei metodi).

## Altro
- [x] gim_list: `del_item` non rinumerava gli indici, quindi dopo un delete e un add due elementi avevano lo stesso indice. Corretto 2026-11-26 (gli elementi dopo quello cancellato scalano di uno, come dice l'header). Test: `test/gim_list_test.cc`. Restano da sistemare: `seek` dichiarato `_gim_flag` ma ritorna `NULL`; `destroy_list` non azzera lo stato (oggi non raggiungibile: è privato e chiamato solo dal distruttore).
- [ ] SYNC/NOTE_GIMUI_SECURITY.md: spostare fuori da SYNC (decide l'utente). Non committato.
- [ ] Questione chiave privata (sale o chiave pubblica): decisione dell'utente.

## Thread: configurazione (decisione 2026-11-22)
- [x] Sezione `THREAD` in `documentation/gim.conf`: `thread::<classe> = YES|NO`, default `YES`. Il valore e' un permesso: il thread parte solo con `start()` esplicito.
- [x] Codice: `gim_thread_allowed()` in `gim_thread.cc` aggiunge la chiave mancante con default YES e salva il file (`gim_conf->Write()`).
- [x] Codice: `gim_thread_obj::start_if_allowed()` rifiuta l'avvio con avviso nel log se la classe e' disabilitata.
- [x] Codice: `gim_thread_report_config()` scrive nel log lo stato delle classi previste (`gim_interface`, `gim_serial`).
- [ ] Chiamare `gim_thread_report_config()` all'avvio del programma (oggi nessuno la chiama; da decidere dove, es. in `gim_home_obj::up`).
- [ ] Collegare `gim_interface` e `gim_serial` a `start_if_allowed()` quando avranno un thread (oggi non ne creano).
- [ ] Verificare il livello dei messaggi di log per questi eventi (oggi WARNING per l'avvio negato, MESSAGE per il resto).
- [ ] NON COMPILATO: il PC di sviluppo non ha il toolchain (MinGW/CMake/Ninja). Compilare e provare prima di committare.

## Casi d'uso generici della classe AI (idee, da scegliere; non nel contratto)
Non riguardano PicGIM: restano qui e non nel contratto condiviso. Scegliere 2-3 casi prima di definire il nucleo.
- [ ] Dati: classificazione di record del database di Gim (fatture, contatti, ticket).
- [ ] Dati: riconoscimento di duplicati con nomi scritti in modo diverso.
- [ ] Dati: stima di campi mancanti, con indicazione di affidabilita'.
- [ ] Documenti: smistamento per contenuto (non per nome) in cartelle o progetti.
- [ ] Documenti: estrazione di campi (data, importo, fornitore) da ricevute o moduli.
- [ ] Documenti: ricerca per somiglianza senza parole chiave esatte.
- [ ] Serie temporali: rilevamento di anomalie su flussi di numeri.
- [ ] Serie temporali: previsione a breve termine.
- [ ] Log: raggruppamento di messaggi simili e segnalazione di errori nuovi.
- [ ] Sviluppo: triage di report di crash per causa probabile.
- [ ] Sviluppo: stima di complessita' di una richiesta di modifica.
- [ ] Didattica: laboratorio di AI con addestramento e inferenza su dati reali, in italiano.
- [ ] Ricerca locale: indice dei file personali con punteggio di pertinenza, senza servizi esterni.
- [ ] Raccomandazioni: voci correlate a quella che l'utente sta guardando.
- [ ] Sicurezza file: validazione HEX prima della programmazione del PIC (formato e CRC-32).
- [ ] Sicurezza file: controllo di header e parametri dei contenitori GKP.
## Regole dei livelli di log (proposta, 2026-11-22; da confermare)
Conteggio grezzo in src/ e gim_shell/: CRITICAL 274, WARNING 142, MESSAGE 70, DEBUG 31, FATAL 49.
Verifica: nessun CRITICAL su un ramo che restituisce __GIM_OK; 90 WARNING restituiscono __GIM_ERROR.
- MESSAGE: progresso normale e stato ("env created", "SET TABLE to PERMANENT", "File closed", "Syntax ok!").
- DEBUG: dettagli che servono solo al debug, rumorosi.
- WARNING: il lavoro va avanti con un valore di default, o un input ignorato ("Mersenne::iterations key not found ... per default").
- CRITICAL: l'operazione fallisce e restituisce __GIM_ERROR o __GIM_NOT_OK ("GDBS line allocation failed", "Table not found", "Syntax error").
- FATAL: solo per condizioni che rendono impossibile continuare.
Un messaggio d'errore con ritorno __GIM_ERROR e' CRITICAL, non WARNING. Un messaggio di esito normale non e' mai CRITICAL.
Decisioni dell'utente:
- [x] Confermare le regole sopra. (confermate 2026-11-22)
- [x] gim_home_obj::up: i tre avvisi su nome applicazione non impostato sono WARNING (il codice continua con i default). Fatto 2026-11-22.
- [x] Per i WARNING con __GIM_ERROR: quelli di errore di operazione sono passati a CRITICAL (42), quelli di default o skip restano WARNING, gli esiti normali sono MESSAGE (14). Applicato 2026-11-22 via mappa messaggio -> livello; compilazione sintattica dei file modificati OK. Build Ninja non rieseguita: cache CMake punta a I:\ (percorso di un'altra macchina).
- [x] Revisione livelli su gim_file.cc e gim_db.cc (2026-11-22): nessun MESSAGE con testo d'errore e nessun errore con livello implicito. Unico caso: 'env not created : Volatile db' (gim_db.cc:178) e' esito normale, resta MESSAGE. SKIPPED (gim_db.cc:696) resta DEBUG, in attesa di conferma.
- [x] Revisione livelli su gim_environment, gim_memory, gim_error, gim_kvalue, gim_directory (2026-11-22): nessun cambio. Le righe segnalate sono avanzamenti o riepiloghi (es. gim_memory 'Not empty element' e' gia' WARNING per elemento). Conteggi precedenti gonfiati dalla parola 'error' nel nome gim_error.
- [x] Controllo sulle chiamate gim_error multilinea: nessuna in src/ e gim_shell/ (2026-11-22).
- [ ] Ricompilare con CMake dopo aver rigenerato la cache sul PC attuale.

## Log negli header (include/*.h), controllo 2026-11-22
- [x] Nessun errore con livello sbagliato: 42 chiamate senza livello sono inizializzazioni o avanzamenti (MESSAGE corretto); nessuna chiamata multilinea.
- [x] gim_db.h:354: FATAL -> CRITICAL per 'use of db is not allowed without the application name' (confermato 2026-11-22).

## Selezione del livello minimo (2026-11-22)
- [x] Chiave [debug] debug::min_type in gim.conf: DEBUG, MESSAGE (default), WARNING, CRITICAL, FATAL. Letta all'avvio in gim_home_obj::up e applicata con gim_error->set_min_type(). Valore non valido: MESSAGE e avviso.
- [x] Verificato a runtime con un programma di prova: DEBUG nascosto di default; DEBUG visibile con DEBUG; solo CRITICAL con critical.
- [ ] Il ramo 'valore non valido' non e' stato visto in output: l'avviso viene scritto durante la costruzione di gim_obj, prima che la prova attivi la stampa. Verificare con f_debug o con verbose attivo prima della costruzione.
- [ ] Documentare la chiave nei commenti di gim.conf (add_default_comment) e nella documentazione.- [x] File gia' esistente senza debug::min_type: la chiave viene aggiunta con MESSAGE e il file riscritto (stesso schema delle altre chiavi mancanti). Verificato 2026-11-22; al secondo avvio nessuna modifica.

## Sezione thread in gim.conf (2026-11-22)
- [x] Le chiavi [thread] mancanti vengono aggiunte all'avvio da gim_thread_add_missing_config(), chiamata in gim_home_obj::up con lo stesso flag 'changed' delle altre chiavi: il file viene riscritto una volta sola.
- [x] gim_thread_allowed() ora solo legge: chiave mancante = YES. Non scrive piu' il file.
- [x] Verificato: su gim.conf senza sezione la sezione viene aggiunta con gim_interface e gim_serial a YES; con gim_serial = NO la classe risulta disabilitata; al secondo avvio il contenuto non cambia.

## gimstat: thread e barra di avanzamento (2026-11-22)
- [x] I test della batteria girano in un thread ciascuno per file >= 1 MiB; sotto, sequenziale come prima. Output in ordine fisso, identico al riferimento (verificato su 300 KB, 2 MB e 12 MB).
- [x] Barra di avanzamento su stderr, solo se stderr e' un terminale e non in modalita' quiet.
- [x] thread::gim_stat = NO riporta a esecuzione sequenziale (verificato, output identico).
- [x] Misura: 12 MB 4,8 s sequenziale, 3,3 s con thread (12 core).
- [ ] Barra: da verificare a occhio in un terminale vero (in questo ambiente stderr non e' un terminale).
- [x] I 100 blocchi girano su un pool di thread (uno per core), con barra 'blocks' su stderr. Test interni sequenziali per blocco (niente thread annidati). Output identico al riferimento; 12 MB: 4,8 s -> 1,8 s.
- [ ] gim_stat_big (file oltre i limiti di memoria) ha la barra 'samples' ma non e' stata provata: serve un file di quella dimensione.
- [x] gimstat: i file oltre 1 GiB non partivano (crash silenzioso): il ramo gim_stat_big girava prima di inizializzare Gim (gim_error e conf nulli). Spostato dopo la creazione di gim_obj. Verificato su 1.1 GB (exit 0, 104 s).
- [ ] gim_stat_big e' sequenziale sui campioni (ogni campione ha i test in parallelo). Su file da 7 GB i 100 campioni richiedono circa 12 minuti: parallelizzare i campioni limitando la memoria.
- [x] gimstat: numero di thread = min(processori logici, job o blocchi, RAM libera x memory::limit / memoria per job). Stima per job: 11 volte la dimensione del campione (test seriali). Messaggio 'threads: ...' sul log (MESSAGE). Verificato: 75% -> 8/12 thread, 0% -> 1 thread, output identico.
- [x] gimstat: tutti i buffer passano da gim_memory (codice __GIM_MEM_GIMSTAT_BUF, nascosti nel log). Verificato: output identico, file da 1.1 GB ok (112 s).
- [ ] Bug trovato in gim_memory_obj::Memory_update: il 'if' sul limite non ha le graffe, quindi il messaggio 'setted' e' sempre stampato e con limite fuori 0..100 max_allocation resta indefinito.
- [x] Log: colonna thread = spazio per il thread principale, T1 T2 ... per gli altri, numerati dal primo messaggio scritto (verificato con 2 thread).
- [ ] Numerazione per ordine di creazione invece che di primo messaggio: se serve, contatore in gim_thread_obj::start.

## Database (gim_db): miglioramenti guidati dal Builder PicGIM e dal pannello AI (2026-11-25)
Origine: l'uso previsto in GimUI (catalogo dei modelli, campioni, esiti delle simulazioni) e nel configuratore PicGIM. Vedi `GimUI\TODO_GIMUI.md` e `PicGim\SPEC_PGIM_CONFIG.md`. Ordine per utilita'.
Regole: classi di Gim, non versioni parallele; ogni correzione con un test (contenuto o hash identici, nessun errore di memoria nel log); i `.conf` dei database gia' esistenti (`db\*.conf`, versione 2.8.5) devono caricarsi come prima (se il formato cambia: salto di versione e percorso di lettura per il vecchio); ogni comando nuovo sta nell'API, nella shell e nel linguaggio GDBS.
- [~] `update` e `delete` su record: FATTO 2026-11-26 in API (`update_record`, `delete_record`), shell (`update`, `delete`) e test (`test/db_records/run_db_records.ps1`, 11 controlli). Verificato anche con riapertura da disco e tabella svuotata. Drop di tabella FATTO (`drop_table` in API, `table drop` in shell, test incluso: rinumerazione in `.conf`, file cancellati con `gim_file_manager`). GDBS FATTO: `UPDATE <t> SET f v ... WHERE f v [AND ...]`, `DELETE <t> WHERE f v [AND ...]`, `DROP TABLE <t>` (passano per l'API). Limiti di GDBS: solo uguaglianza (`=` è separatore del linguaggio, `>`/`<`/`!=` sono rifiutati), niente valori con spazi (non ci sono virgolette), massimo 20 token per riga. E' la richiesta piu' urgente: la cattura con etichetta a tasto sbaglia qualche etichetta e va corretta.
- [~] Importazione ed esportazione CSV, nel formato del dataset del trainer (`label,ch0,ch0_ok,...`); l'impronta SHA-256 resta calcolata sul CSV. FATTO 2026-11-26 in API (`export_csv`, `import_csv`), shell (`table export`, `table import`) e test (`test/db_records`, 7 controlli): round trip con stessa impronta, il trainer accetta il file esportato, import rifiutati per intero. Regole: la KEY è opzionale; se c'è, i suoi valori devono essere nuovi (nel file e nella tabella). APERTI: comandi GDBS per CSV; il database non conosce il range del trainer (0..65535, il trainer rifiuta 70000 ma il DB lo accetta); un valore con a capo non si importa; con database PERMANENT ogni riga riscrive il file (serve l'inserimento a blocchi, voce sotto).
- [ ] Indice sulla chiave e sui campi di tempo; `where ... between ...`; `offset` accanto a `limit` e `last`. Oggi la `select` e' una scansione lineare che rilegge ogni chiave come testo: va bene per centinaia di righe, non per decine di migliaia.
- [ ] Aggregati `count`, `min`, `max`, `avg`, `sum` con `group by` (matrice di confusione, statistiche per canale).
- [ ] Inserimento a blocchi e salvataggio differito (la cattura non deve scrivere su disco a ogni campione). Revisione di thread-safety: stato condiviso, buffer statici e lock, perche' un thread scrive mentre la finestra legge.
- [ ] Punti gia' annotati in `gim_db.h`: `or` nella `where`, confronto tra due campi, `like` e `in`, tipo predefinito delle tabelle nei database permanenti, decimali dei FLOAT (5), buffer condivisi di `GetKeySTR()` e `GetSectionName()`.
- [ ] Divisione dei dati proposta: nel DB catalogo dei modelli, campioni e riassunti degli esiti (tabelle caricate in memoria, `VOLATILE` e modo `PERFORMANCE`, durante la simulazione); fuori dal DB il flusso grezzo (CSV del trainer) e la traccia passo-passo.
- [ ] NON COMPILATO e NON PROVATO: tutto da scrivere e provare dove c'e' il toolchain.

## Simulazione dell'inferenza PicGIM in Gim (idee, 2026-11-25)
Il simulatore intero del trainer riproduce gia' i vettori di test (copia operazione per operazione di `pg_ai_input_normalize` e `pg_ai_net_infer`). Da aggiungere:
- [ ] Traccia opzionale nel simulatore: un callback per ogni operazione (strato, neurone, ingresso, valori). Spenta di norma, il percorso veloce non cambia. Serve alla vista animata di GimUI (`GimUI\TODO_GIMUI.md`).
- [ ] Riproduzione di una sessione registrata (sequenza di ingressi nel tempo) con classe, punteggi, `fault_mask` e `sat_mask`; guasto simulato di un canale (`ok = PG_NOK` con `fallback`).
- [ ] Ciclo chiuso con un modello dell'impianto e il PID con la stessa aritmetica di `pgim_ctrl`: portare in C++ `PicGim\tests\ctrl\pgim_ctrl_ref.ps1` con gli stessi vettori.
- [ ] Confronto con il PIC vero: vettori su gpsim o picsim (Proxima) e prova con l'hardware via seriale (Gim manda un vettore e confronta la risposta con il proprio calcolo).
- [ ] Non si simulano: tempi, RAM reale, comportamento del compilatore (un `int*int` troncato a 16 bit, shift su tipi senza segno). Solo il PIC li prova.
- [ ] Regressione nel trainer (funzione di costo e scala d'uscita diverse): la proposta "4.7 Regressione" del contratto Gim-PicGIM non e' ancora scritta.
## Compressione: manca una classe (2026-11-25)
Controllato: in `include\` e `src\` non c'e' nessun riferimento a zlib, deflate, LZ4, zstd, lzma o bzip2. Serve una classe di compressione (per esempio `gim_compress_obj`), da usare nei contenitori GKP, nell'esportazione del `.project`, nella cattura dei dati e nel database. Da decidere con l'utente: i formati. Candidati (a memoria, licenze e prestazioni da verificare prima di scegliere):
- [ ] DEFLATE (zlib / gzip): e' il formato dentro lo zip e il piu' diffuso; velocita' e rapporto medi; licenza zlib, compatibile con GPL. Buono per l'interoperabilita'.
- [ ] Zstandard (zstd): ottimo compromesso velocita'/rapporto, livelli, dizionari (utili per tanti record piccoli e simili, come righe del database e log); BSD o GPLv2. Candidato come predefinito interno.
- [ ] LZ4: velocissimo, rapporto piu' basso; formato semplice (si potrebbe scrivere in proprio, in poche centinaia di righe). Per flussi in diretta (cattura, traccia).
- [ ] LZMA / xz: miglior rapporto, compressione lenta; liblzma e LZMA SDK sono di pubblico dominio. Per archivi e rilasci.
- [ ] bzip2 e Brotli: possibili ma meno utili qui (bzip2 lento, Brotli pensato per il web). LZO e' GPL: da evitare se non serve.
- [ ] Trasformazioni prima del codificatore per serie di campioni: differenze tra campioni successivi, codifica a lunghezza variabile, ADPCM (gia' previsto per l'audio di PicGIM).
- Lato PIC, se mai servisse (immagini HEX sul collegamento seriale con il bootloader, audio): decompressori minuscoli come LZ4 o heatshrink (licenza ISC). Da verificare.
Punti di progetto:
- [ ] Interfaccia a flusso (a blocchi), con callback di avanzamento per oggetto (come previsto per `gim_gkfp_progress`) e compatibile con `gim_thread_obj`.
- [ ] Nei GKP: comprimere prima di cifrare (dopo la cifratura i dati non si comprimono). Campi nell'intestazione: codice del formato, livello, dimensione originale, controllo per blocco (CRC-32 o SHA gia' in Gim). I GKP esistenti devono aprirsi come prima: salto di versione.
- [ ] Scelta tra libreria esterna (zlib, zstd, liblzma) e codice proprio (Gim ha gia' MD5, SHA e il suo Mersenne Twister): proposta di partenza = LZ4 e inflate scritti in proprio, e zlib o zstd come supporto opzionale in compilazione. Verificare le licenze con quella di Gim (GPL v2 o successiva).
- [ ] Un uso con `gimstat`: il rapporto di compressione come misura in piu' accanto al punteggio GKaos (da valutare).
- [ ] NON COMPILATO e NON PROVATO: da scrivere con i suoi test (contenuto identico dopo compressione e decompressione, nessun errore di memoria nel log).

## gim_kvalue e gim_lexical: difetti trovati scrivendo la documentazione (2026-11-25)
Letti nel sorgente, NON corretti e NON riprodotti (nessun compilatore). Ogni correzione va con il suo test.

Perdita di dati:
- [ ] `scan_line`: una riga con `#` e' un commento intero, controllato prima della chiave. `chiave = valore # nota` viene ignorata in Lex A, B e C; cosi' anche cio' che `Write()` scrive con `PRSR_INLINE`. Esempi reali: `thread::gim_interface` in `documentation/gim.conf`, `type` in `db/sensori_tab_Lex_A.conf`.
- [ ] `scan_line`: `< > [ ]` (A), `< >` (B), `< > { }` (C) in un valore fanno leggere la riga come titolo o sezione.
- [ ] `Read()` non carica i commenti: dopo `Read()` + `Write()` sono persi.
- [ ] `Write()` con Lex non scelto: il file viene troncato e scritto senza sezioni, e torna `__GIM_OK`.
- [ ] `tokenizer`: `l` e' `_gim_Uint8`, oltre 256 caratteri la riga va a capo nel contatore. Ultima riga senza newline ignorata. CRLF: `\r` resta nel nome di sezione (A) e nel titolo.

Controlli che non scattano:
- [ ] `syntax_a/b/c`: `error` parte da `__GIM_OK` (8) e il ciclo gira con `error == 0`: il controllo di sintassi non viene mai eseguito. Attenzione: riattivandolo, la riga con il solo nome di sezione in Lex B risulta `__SYNTAX_ERROR`.
- [ ] `AddSection`: confronta `ExistSection()` con `__GIM_YES` (1), ma torna `__GIM_EXIST` (6): le sezioni doppie passano.
- [ ] `AddKey`: confronta `ExistKey()` con 1; il secondo controllo salta l'ultima chiave: una copia dell'ultima chiave passa.
- [ ] `is_in_string( char * , const char * )` cerca i caratteri in ordine, non la sottostringa: `::` e `( ` possono essere riconosciuti anche separati.

Memoria:
- [ ] `key` e' di 32 byte, ma `AddKey` accetta fino a 63 caratteri (`ChangeKey` fino a 31): scrittura oltre il buffer.
- [ ] `RenameKey`, `RenameSection`, `RenameTitle`, `Up`: nessun controllo di lunghezza.
- [ ] Commenti: il controllo e' `> 10240` con buffer di 10240: a 10240 esatti manca il posto per il terminatore.
- [ ] `WriteOnBuffer`: 10240 byte fissi senza controllo; `sprintf( dest , "%s..." , dest )` ha sorgente e destinazione sovrapposte (comportamento indefinito).
- [ ] `GetKeyINT/FLT/FLAG`: `strdup( GetKeySTR() )` con chiave assente e' `strdup( NULL )`; il controllo su NULL viene dopo.
- [ ] Quasi tutti i metodi leggono `prsr_obj->first_section` prima di controllare `prsr_obj == NULL`.
- [ ] `strdup` mai liberati in quasi ogni metodo. `DelSection` di una sezione vuota non la libera. `Read()` ripetuto senza `Down()` perde il contenuto precedente.

Valori e ritorni:
- [ ] `AddKeyFlag` / `ChangeKeyFlag`: `__GIM_LOG_TEXT` scrive "HTML" e `__GIM_LOG_HTML` scrive "TEXT" (invertiti rispetto a `GetKeyFLAG`).
- [ ] `ChangeKeyFlag`: mancano `__GIM_AUTO` e `__LEX_C`.
- [ ] `AddKeyComment` e `AddSectionComment` tornano `__GIM_NOT_EXIST` anche quando riescono.
- [ ] `RenameSection` non porta in maiuscolo il nome nuovo: poi la sezione non si trova piu'.
- [ ] `AddSection` e `AddKey` tornano -1 in alcuni casi e `__GIM_NOT_OK` in altri. `GetHowManyKey` torna `_gim_int8`: oltre 127 chiavi diventa negativo.
- [ ] `List()` in Lex C stampa le chiavi senza le sezioni.
- [ ] Non thread-safe: buffer statici (`gim_section_key`, `get_line`, `scan_a/c`) e globali `Tok`, `Syntax`, `Lexical`.

## gim_db: difetti trovati scrivendo la documentazione (2026-11-25)
Letti nel sorgente (`src/gim_db.cc`, `include/gim_db*.h`), NON corretti e NON riprodotti (nessun compilatore). Ogni correzione va con il suo test.

Metodi dichiarati e mai scritti:
- [ ] `create_table`, `add_field_to_table` (2 forme), `check`, e i privati `create_db`, `add_table`, `del_table`, `add_field` (2), `get_table_handle`, `command_parser`: nel `.h` ma senza corpo in `src/gim_db.cc`. Una copia vecchia di alcuni e' in `pool/new_totest/gim_db.cc`. Chiamarli non fa linkare. Decidere se scriverli o toglierli (le tabelle si creano oggi solo da script).

Crash e perdita di dati:
- [ ] `init_from_gdbs` con un file che non esiste: `gdbs_open()` torna errore ma viene ignorato, `gdbs_getline()` torna NULL e `gdbs_tokenizer( NULL )` lo legge.
- [ ] `make_env`: con tipo VOLATILE e cartella esistente esegue `system( "rm -rf <cartella>" )`. Il nome viene dal nome del database (cambiati solo gli spazi): cancella dati, e con nomi strani e' una iniezione di comando. Su Windows non esiste `rm`.
- [ ] `read_tables`: se il file di struttura di una tabella non esiste (tabella permanente senza campi, mai scritto), `Tstruct` non e' mai stato `Up` e `GetKeyINT` legge un puntatore nullo.
- [ ] Un record con un valore di 64 caratteri o piu', o vuoto: `AddKey` rifiuta, il risultato di `insert_record` non lo controlla, `items` sale e torna `__GIM_OK`. Il record resta senza quel campo.
- [ ] INSERT di script (`gdbs_execute`): i valori sono assegnati ai campi per POSIZIONE, le etichette sono ignorate; con meno coppie dei campi `get_item( i )` torna NULL e viene usato; nessun controllo del tipo ne' della chiave.
- [ ] INSERT di script, tipo FLOAT: il caso e' vuoto, il valore non viene scritto. BOOL: scrive TRUE/FALSE, mentre `insert_record` scrive YES/NO.
- [ ] `read_tables`: i campi sono letti con id da 1 a N; gli script vecchi (`db/valtemp_db.gkdbs`) usano `ADD FIELD 0`: dopo la rilettura il campo 0 manca e quello N non si trova.
- [ ] `make_add_field`: il risultato di `AddKey` e' ignorato: con un id ripetuto o saltato i campi in memoria e nel file non coincidono, e `fields_number` sale lo stesso.

Valori e ritorni sbagliati:
- [ ] `init`: `res` non e' inizializzata. Per un database PERMANENT nuovo e' assegnata solo nel ramo VOLATILE, quindi il ritorno e' indefinito; vale anche per un `Read` che torna altro. Nei rami `__LEX_UNKNOWN` e `__SYNTAX_ERROR` usa `Up( file_name )` invece di `file_name_long`: il file finisce nella cartella corrente.
- [ ] `read`: ignora il risultato di `init()` e torna `__GIM_OK` anche quando `init` fallisce.
- [ ] `set_property`: cambia la chiave nella sezione "property" (non esiste, la sezione e' "DB"): il cambio non viene mai scritto. `GIM_DB_VOLATILE` con valore diverso da YES imposta `db->mode` invece di `db->type`; lo stesso `GIM_DB_PERMANENT`. `GIM_DB_BALANCED` con valore diverso da YES torna errore.
- [ ] `table_exist`: torna `__GIM_ERROR` quando non ci sono tabelle e scrive un CRITICAL per una domanda normale; guarda le chiavi del `.conf`, non la lista in memoria.
- [ ] `make_set_field`: KEY, AUTOINCREMENTAL e IS_UNIQUE scrivono la chiave nel file ma non aggiornano `key_field_number` & co. in memoria; IS_INDEX, NOT_KEY e IS_NOT_INDEX sono ignorati e tornano OK. Solo KEY e' applicata (da `insert_record`, che rilegge la chiave dal file di struttura).
- [ ] `gdbs_line_syntax_check`: CREATE TABLE torna `__GDBS_CREATE_DB`. `init_from_gdbs`: un errore di sintassi torna `__MISSING_STOP_CHAR`; il contatore di riga `l` e' un `_gim_int8` e va a capo dopo 127 righe.
- [ ] `gim_db_version`: confronta `atoi( "%d%d" )` con 23: la versione 1.99 passa.

Memoria e stato:
- [ ] `set_name`, `read`, `exist_environment`: controllano `> 64` ma il buffer `tmp_name` e' di 64: un nome di 64 caratteri scrive oltre. `make_create_table`: `strcpy( Ttmp->name , table_name )` senza limite (il campo e' di 64), e `char_subst` modifica la stringa del chiamante.
- [ ] `init_from_gdbs`: `gdbs_run` non torna mai a `__GIM_NOT_RUNNING` e `syntax->Feof` resta ON: un secondo script sullo stesso oggetto non parte. Sulle uscite anticipate il file non e' chiuso e le righe non sono liberate.
- [ ] Costruttore: senza nome dell'applicazione `db` non e' allocato e il distruttore lo usa. `gdbs_file`, `home` e `d_name` non sono inizializzati.
- [ ] Distruttore: libera `Tstruct`/`Tdata` con `Down()` ma non li cancella (perdita); `delete Ttmp->fields` senza liberare i singoli campi.
- [ ] `insert_record`: `norm[ 255 ][ 512 ]` statico (130 KB, non thread-safe); un INT oltre i 32 bit viene troncato (`atoi`); il controllo della chiave scorre tutti i record a ogni inserimento.
- [ ] `tables_count` ritorna un `_gim_Uint8`, `tables_number` pure: massimo 255 tabelle.
- [ ] DATE, TIME, TIMESTAMP e PERCENTAGE non sono validati da `insert_record`; il modo del database (SAVE_MEMORY, ...) viene salvato ma non usato.
- [ ] Esempio `db/gdbs_command_structure` e `valtemp_db.gkdbs` descrivono `CREATE DB nome file` e `ADD FIELD ... index=1`, `index=AUTO`: il codice li rifiuta o li ignora.

## gim_memory e gim_error: difetti trovati scrivendo la documentazione (2026-11-27)
Letti nel sorgente (`src/gim_memory.cc`, `src/gim_error.cc`), NON corretti e NON riprodotti (nessun compilatore). Ogni correzione va con il suo test.

gim_memory:
- [ ] `Duplicate()`: restituisce un blocco azzerato, non una copia. `src/gim_ascii_buffer.cc` (righe 359, 366) lo usa per copiare il file: il buffer caricato e' pieno di zeri. Test: contenuto identico dopo `load`.
- [ ] `Relloc()`: `else if ( size = currentlist->size )` e' un'assegnazione: con dimensione uguale o maggiore il blocco non cresce mai. Contatori (`allocated`, `currentlist->size`) non aggiornati per la differenza.
- [ ] `Alloc()`: se `malloc( size )` fallisce, il nodo `tmplist` resta allocato e non e' in lista (perdita). `caller` non viene riportato a `__GIM_MEM_OTHER` sul percorso di errore: la chiamata successiva senza codice viene loggata con il nome del chiamante precedente.
- [ ] `Alloc()` oltre il limite: `Set( GIM_ERROR_FATAL ...)` con `on_fatal_exit` attivo chiude il programma invece di tornare NULL come dice la documentazione.
- [ ] `allocated`, `allocated_peek`, `Limit`: `int` (`Allocated()` torna `_gim_Uint32`). Sopra 2 GB il conteggio va in overflow; il test `gimstat` da 2 GB e' a rischio.
- [ ] `Free()`: il rifiuto dei blocchi LOCKED vale solo se `caller == __GIM_MEM_OTHER`. Il comportamento dichiarato (un bloccato non si libera) non corrisponde.
- [ ] Costruttore `gim_memory_obj()`: inizializza solo le liste. `lock`, `virt_mem`, `Alloc_limit`, `Limit`, `max_allocation`, `allocated`, `caller` restano indefiniti fino a `up()`/configurazione.
- [ ] `Memory_update()`: `if ( ( Limit >= 0 ) && ( Limit <= 100 ) )` non ha parentesi: il log "Allocation limit setted" esce sempre, anche con un limite fuori range. `mem_alloc_limit` accetta `_gim_Uint8` (fino a 255) senza rifiutare i valori sopra 100.
- [ ] `gim_memory_page_obj`: `Alloc()`, `Free()` e `release_memory_list()` sono dichiarati ma non definiti in `src/`: una chiamata non linka. `lstart`/`lcurrent` mai liberati. Decidere se implementarlo o toglierlo.
- [ ] `memory_id_cnc`: dichiarato `extern` in `gim_memory.h`, non definito in `src/`.
- [ ] Identificatori dei codici: `__GIM_MEM_FOBJ_LOAD_GCH` (0xff000020) nell'header e `__GIM_MEM_FOBJ_OPEN_GCH` (0xff000020) nella tabella; `0xff000038` (`__GIM_MEM_DB_RECORD`) manca nella tabella, quindi nel log esce come `__GIM_MEM_UNKNOWN`; `0xff000037` usato sia da `__GIM_MEM_GIMSTAT_BUF` sia nella tabella. `__GIM_LIST_ITEM` e' in tabella due volte (0xff000033 e 0xff000051).
- [ ] `release_memory_list()`: i nodi con `data == NULL` (diversi dalla sentinella) non vengono liberati.
- [ ] `Duplicate()` e `Relloc()` tornano NULL su puntatore non trovato senza stato di errore distinto.
- [ ] `hide()`: `__GIM_HIDE` vale 1 e `__GIM_YES` vale 1 (`__GIM_NOT_HIDE` = 2 = `__GIM_AUTO`): confronti fra costanti di nomi diversi che si sovrappongono. Non cambiare il valore senza rivedere tutti i confronti.
- [ ] Puntatori statici (`metric_converter`, `code_to_id`) restituiti fuori dal lock.

gim_error:
- [ ] `char_filter()`: restituisce `from` invece di `tmp` (non filtra nulla) e perde `tmp` (`strdup` senza `free`) a ogni messaggio. Lo usano `set( function , message )` e `Set()`: i newline entrano nel log.
- [ ] `Set()`: `vsprintf( message , format , ... )` con `message[1024]` senza controllo: usare `vsnprintf`.
- [ ] `set( type , function , message , value )`: `strcpy( mess , message )` con `mess[512]` per i tipi non CRITICAL, e `sprintf( mess , "ERROR : %s" , message )` per CRITICAL: messaggi da 512 caratteri in su scrivono oltre il buffer. Usare `snprintf` con la dimensione giusta.
- [ ] `set( type , function , message , value )` e `set( pid_t ...)` con log aperto e `first_in != NULL`: chiamano `lflush()` e NON scrivono il messaggio corrente, che va perso. (Verificare: nessun altro punto chiama `lflush()` quando il log si apre.)
- [ ] `app( GIM_ERROR_FATAL , ...)`: passa valore 0; `get()` stampa e termina solo con `value != 0` (oppure `always_verbose`), quindi il FATAL di `app()` non termina il programma ne' va su stderr.
- [ ] `set( function , index )`: il controllo FATAL legge `type` PRIMA di `strcpy( type , gim_error_list[index].type )`, quindi usa il tipo del messaggio precedente. L'indice 7 (FATAL, "Impossible to write on Fdebug file") non termina il programma nel caso normale.
- [ ] `Set()`: solo sei tipi hanno un valore; `GIM_ERROR_OK` diventa `GIM_ERROR_UNKNOWN` (" UU ").
- [ ] I tipi si confrontano come testo con `strcmp`: una stringa diversa dalla macro non e' mai FATAL. `GIM_ERROR_FATAL` contiene sequenze di colore ANSI (`\033[1;31m`): verificare se finiscono nel file di log.
- [ ] Prima dell'apertura del log i messaggi si accumulano in memoria senza limite (`ladd()` con `malloc`).
- [ ] `error_time` e' definito due volte (membro della classe e globale in `gim_error.cc`).
- [ ] Il commento del costruttore del log (`set( type ... )`) dice "string of 3 bytes": i tipi sono lunghi fino a 31 caratteri.
- [ ] Nessun test per il livello minimo, per il filtro dei newline e per la coda prima del log.

## gim_file: difetti trovati scrivendo la documentazione (2026-11-27)
Letti nel sorgente (`src/gim_file.cc`, `include/gim_file.h`), NON corretti e NON riprodotti (nessun compilatore). Ogni correzione va con il suo test.

Memoria (gravi):
- [~] `gim_file_manager_obj::close( const char * name )`: `del_item( item )` libera `item->hand` e `item`; poi `gim_memory->Free( tmp )` libera di nuovo lo stesso handle. Il blocco puo' essere riusato e la seconda Free ne libera un altro. Test: aprire per nome, chiudere per nome, controllare `gim_memory` nel log. Corretto 2026-11-27 in `src/gim_file.cc`, NON compilato: da provare su Proxima.
- [~] `gim_file_obj::unload()`: non azzera `handler->loadp`. Dopo `unload()` un `load()` restituisce il buffer liberato (use after free), e `close()` dopo `unload()` lo libera di nuovo. Anche `gim_ascii_buffer.cc` (righe 360, 367) chiama `Ftl.unload()`. Correzione: azzerare `loadp` in `unload()`. Corretto 2026-11-27 in `src/gim_file.cc`, NON compilato: da provare su Proxima.
- [~] `gim_file_obj::type()`: legge `this->fname`, che non viene mai assegnato (ne' nel costruttore ne' altrove): puntatore non inizializzato. Corretto 2026-11-27 in `src/gim_file.cc`, NON compilato: da provare su Proxima.
- [~] `gim_file_manager_obj::flush()`: `tmp = item( hand->name )` e poi `tmp->mode` senza controllo NULL: un handle non in lista fa crashare il programma. Corretto 2026-11-27 in `src/gim_file.cc`, NON compilato: da provare su Proxima.

Apertura (gravi):
- [~] `open_fd()`: `__GIM_WRITE` e `__GIM_APPEND` senza `O_CREAT`: un file nuovo non viene creato, e `open()` non segnala l'errore. Corretto 2026-11-27 in `src/gim_file.cc`, NON compilato: da provare su Proxima.
- [~] `open()`, ramo `__GIM_FILE_DESCRIPTOR`: il controllo `item->hand->fd == 0` dovrebbe essere `< 0`: un open fallito restituisce un handle con descrittore -1. Corretto 2026-11-27 in `src/gim_file.cc`, NON compilato: da provare su Proxima.
- [ ] `open()`: `if ( exist( filename ) == __GIM_NO )` non scatta mai, perche' `exist()` torna `__GIM_EXIST` o `__GIM_NOT_EXIST`. Il messaggio "File not found" arriva solo dai rami successivi.
- [ ] `open_fm()`: su errore imposta un messaggio CRITICAL ma non lo segnala al chiamante; `open()` controlla `fm` (che resta NULL) e per questo funziona, ma un file vuoto (mmap di lunghezza 0) viene segnalato come "File not found" dal costruttore di `gim_file_obj`.

Gestione della lista:
- [ ] `item( name )`: confronto per nome. Due `open` dello stesso file danno due voci; chiudere una chiude il riferimento dell'altra. Decidere: rifiutare la seconda apertura, oppure cercare per handle.
- [ ] `release_item_list()`: `currentlist->hand->size` e `hand->name` sono letti prima del controllo `hand != NULL`; `%d` con `_gim_Uint32` nel `sprintf`.

Oggetti (`gim_file_obj`):
- [ ] `name( void )`, `size()`, `load()`, `flush()`, `fp()`, `fd()`, `fm()`, `close()`: dereferenziano `handler` senza controllo: con apertura fallita (costruttore con errore) crashano. `close()` in particolare chiama `gim_file_manager->close( NULL )`.
- [ ] `name( const char * )`: dichiarato in `gim_file.h`, non definito in `src/`.
- [ ] `get_stat()`: restituisce sempre NULL (dichiarato "todo" nel header).
- [ ] `put()`: scrive `"%s\n"` con il metodo bufferizzato, senza newline con il metodo non bufferizzato. Il comportamento e' diverso tra i due metodi.
- [ ] `exist` e `method`/`mode` non inizializzati se `gim_file_manager` e' NULL (costruttore).

Scrittura e cancellazione:
- [ ] `flush()` e `put()`: il risultato di `write()`, `fwrite()` e `fprintf()` non viene controllato: possono tornare OK con uno spazio pieno.
- [ ] `DELETE()`: il risultato di `remove()` viene ignorato, torna `__GIM_OK` se il file esisteva.
- [ ] `length()`: apre in modo testo (`"r"`): su Windows la dimensione puo' non coincidere con i byte del file. Usare `"rb"`.
- [ ] `load_gch()`: membro privato, legge un carattere alla volta; `Alloc` senza controllo del risultato. Verificare se e' usato.

Documentazione (header):
- [ ] `gim_file_obj::name( const char * )`: documentare come non implementato, oppure implementarlo.
- [ ] Il commento di `put()` non dice che aggiunge un newline.
- [ ] `open()`: il commento non dice che il metodo bufferizzato in scrittura svuota il file, e che il metodo non bufferizzato non crea file.

## gim_list: difetti trovati scrivendo la documentazione (2026-11-27)
Letti nel sorgente (`src/gim_list.cc`, `include/gim_list.h`). Non corretti (nessun compilatore). Ogni correzione va con il suo test.
- [ ] `seek()`: su indice 0 e su elemento non trovato torna `NULL` (= `__GIM_NO`) invece di un codice d'errore; su non trovato lascia il puntatore corrente oltre l'ultimo elemento. Il commento del header dice "__GIM_OK when found": coerente solo per il caso trovato.
- [ ] `restore()`: `savelist` puo' puntare a un nodo rimosso da `del_item()` tra `backup()` e `restore()`: puntatore penzolante.
- [ ] `size( mem_size )`: imposta `item_size`, che nessun metodo usa. Il costruttore senza argomento non lo inizializza (la riga e' commentata): `size()` legge un valore indefinito.
- [ ] `get_item( void )` e `next_item()`: i messaggi di log riportano nomi di altre funzioni (`next_item`, `add_item`) e `get_item` torna `__GIM_ERROR` come valore di un messaggio di avviso.
- [ ] `del_item()`: il messaggio "Item found and deleted" riporta `gim_list_obj::add_item`.
- [ ] `destroy_list()`: libera i dati degli elementi allocati con `gim_memory` anche se appartengono a un'altra lista: una coppia di liste con lo stesso puntatore fa doppio free.
- [ ] Nessun lock: non thread-safe (il `CLAUDE.md` lo elenca come classe non verificata).
- [ ] `get_item( index )` e `del_item( index )`: scansione lineare dall'inizio, e `del_item` rinumera tutti i successivi: O(n) a ogni rimozione.

## gim_checksum: documentata e rivista (2026-11-27), NON compilata
- [~] Aggiunti `md5_buf()`, `sha1_buf()`, `sha256_buf()`, `sha512_buf()` e le versioni `*_file_buf()`: scrivono nel buffer del chiamante, e sono thread-safe. Le funzioni con risultato statico restano invariate, con la nota "NOT thread-safe".
- [~] `gim_gkfp_file_data.cc` migrato ai `md5_buf()` (con buffer per ogni chiamata, perché usa i thread): NON compilato, da provare con gkfp su un altro PC.
- [ ] Ancora sulle funzioni con buffer statico (`md5`, `sha1`, `sha256`, `sha512`): `gim.cc`, `gim_db.cc`, `gim_gkp_header.cc`, `gim_gkfp_header.cc`, `gim_gkp_file_data.cc` (gkp, non threaded), `gim_gkp_password.cc`, `gim_private_key.cc`, `gim_gkfp_file_list.cc`. Si migrano quando si tocca quel file.
- [x] `chsum()` e `chsum8()` rimosse: nessun chiamante nel repository (solo copie nei build stage, che si rigenerano).
- [x] `chsum()` (aritmetica con `int` e overflow): tolto insieme alla funzione.
- [ ] `GIM_SHA1_SIZE` vale 65, ma il digest ascii SHA-1 è di 40 caratteri (41 con lo zero): il valore è coerente solo con altri usi. Da confermare nei chiamanti.
- [ ] `md5(file)` / `sha*(file)`: il file viene caricato per intero in memoria (sha) o mappato (md5): le due strade non sono uguali. Le versioni `*_file_buf()` usano `gim_file_obj::load()`.
- [ ] Costruttore: `sprintf` con `%d` per i `sizeof` (ora con cast a `int`). Il controllo `HIGHFIRST` non è mai stato provato su un PC big-endian.
- [ ] Vettori di test: `test/gim_checksum_test.cc` usa i vettori standard per "abc" (MD5, SHA-1, SHA-256, SHA-512). Non eseguito: va fatto su un altro PC (Proxima o Ancarlo, spenti al momento). Il PC di lavoro non ha compilatore.

## gim_time: documentata e rivista (2026-11-27), NON compilata
- [~] Aggiunti `get_usec_buf()`, `process_buf()` e `date_buf()`: versioni thread-safe che scrivono in un buffer del chiamante. `get_usec()`, `process()` e `date()` restano per compatibilità, ma `get_usec()` e `process()` usano ancora il buffer statico.
- [~] `date_buf()` usa `localtime_r` (POSIX) o `localtime_s` (Windows): prima `localtime` restituiva un puntatore statico.
- [~] Corretti `process_sec()` e `process_msec()`: restituivano il valore vecchio di `tvstop`, non l'ora corrente. `process_msec()` restituiva microsecondi. Ora leggono il tempo di parete (wall clock) e danno millisecondi. Il nome suggerirebbe il tempo di CPU: verificare i chiamanti prima di usarli.
- [ ] `date()` restituisce una stringa `strdup` che il chiamante deve liberare. `src/gim_log.cc` (righe 107, 139, 152, 185) e `gim_gkp_header.cc` / `gim_gkfp_header.cc` non la liberano: perdita di memoria per ogni chiamata.
- [ ] Usare `gim_memory` invece di `strdup` in `date()`, con firma che restituisca la lunghezza o un buffer dal chiamante.
- [ ] `get_usec()` e `process()` restano statici: i chiamanti in `src/gim.cc`, `gim_error.cc`, `gim_gkfp_file_data.cc`, `gim_gkp_password.cc`, `gim_log.cc` vanno passati a `*_buf()`.
- [ ] `struct timeval` e `struct rusage` dipendono da `gim_platform.h` su Windows: non verificato.
- [ ] La classe non ha lock: un oggetto per thread.
- [ ] DA TESTARE TUTTA LA CLASSE su un altro PC (Proxima o Ancarlo, spenti al momento): `test/gim_time_test.cc` non è mai stato eseguito. Il PC di lavoro non ha compilatore.

## Cifratura: piano e difetti (2026-11-27)
Piano completo in `SYNC/CRITTOGRAFIA_PIANO.md`. Nulla di nuovo è scritto ancora.
- [ ] DA FARE (passo 1): sorgente di casualità (BCryptGenRandom / getrandom), con test. Eccezione alle classi Gim da motivare nel codice.
- [ ] DA FARE (passo 2): verificare `gim_sha256` e `gim_sha512` con i vettori NIST.
- [ ] DA FARE (passi 3-4): HMAC-SHA256, PBKDF2-HMAC-SHA256, ChaCha20, Poly1305, ChaCha20-Poly1305 (vettori RFC 8439).
- [ ] DA FARE (passi 5-7): file con password, scambio X25519, firma Ed25519 (vettori RFC 7748, RFC 8032).
- [ ] Difetto: `gim_gkp_cryptography`, `advanced_password()` con nonce vuoto: stesso flusso per tutti i file con la stessa password.
- [ ] Difetto: `gim_gkp_cryptography`, chiave derivata dalla password con 4096 round di SHA-512, non è una KDF per password.
- [ ] Difetto: `gim_mersennetwister`, seme di 32 bit; su Windows `MTRand::seed()` ripiega su tempo e clock.
- [ ] DA FARE su Proxima o Ancarlo (spenti): compilare e far girare i test appena scritti. Sul PC di lavoro non c'è compilatore.

## gim_getopt: difetti trovati scrivendo la documentazione (2026-11-27)
Letti nel sorgente (`src/gim_getopt.cc`, `include/gim_getopt.h`). Non corretti, non compilati. Ogni correzione va con il suo test.
- [ ] `setopt()`: `strcpy( options , useropt )` in un buffer di 64 caratteri, senza controllo: una lista piu' lunga scrive oltre il buffer. Correzione: `strncpy` con controllo, o buffer dimensionato, e errore se troppo lunga.
- [ ] `options` non inizializzato: `scanopt( argc , argv )` senza `setopt()` usa un buffer indefinito.
- [ ] `scanopt()` accoda: una seconda chiamata sullo stesso oggetto non svuota `optlist`/`errlist`. `optind` (globale) non viene riportato a 1 dalla classe: una seconda scansione di `argv` non trova nulla (o trova altro).
- [ ] `search()`: se l'opzione non c'e' lascia la posizione corrente dopo l'ultimo elemento (il `backup()` non viene ripristinato). Il `restore()` e' a slot singolo: un `search()` dentro un altro backup lo sovrascrive.
- [ ] `strdup( optarg )` non viene mai liberato (`destroy_list` libera solo l'item): perdita di memoria per ogni opzione con valore. Il `strdup` non e' controllato per `NULL`.
- [ ] `gim_memory->Alloc` per ogni item non viene controllato: con memoria esaurita `temp_opt` e' `NULL` e l'assegnazione successiva crasha.
- [ ] Il costruttore imposta la globale `opterr = 0`: cambia il comportamento di `getopt()` per tutto il programma (niente messaggi di getopt).
- [ ] `opterror = optopt` e' impostato anche per le opzioni valide: il campo non ha significato per loro (la doc lo dice).
- [ ] Opzioni ripetute: entrambe finiscono nella lista, la classe non le segnala.
- [ ] Solo opzioni corte: `--nome` non e' gestito (scelta della classe, da dichiarare; `getopt_long` non e' usato).
- [ ] Le macro `GIM_GETOPT_OK` e `GIM_GETOPT_ERROR` non fanno `rewind()`: un ciclo parte dalla posizione corrente.
- [ ] Non thread-safe: `optind`, `optarg`, `optopt`, `opterr` sono globali (su Windows `gim_optind` ecc. in `gim_platform_win32.cc`). Il `CLAUDE.md` la elenca come "non verificato".
- [ ] Su Windows `getopt` e' la versione di `gim_platform_win32.cc` (con permutazione degli argomenti): il comportamento sugli errori va confrontato con quello POSIX sul test.
- [ ] DA FARE: un test (`test/gim_getopt_test.cc`) con casi: opzione valida, valore mancante, opzione sconosciuta, senza `:` iniziale, `search()` su opzione assente, due chiamate a `scanopt()`. Da far girare su Proxima o Ancarlo (entrambi spenti al momento).

## gim_string: riscritta (2026-11-27), NON compilata
- [~] Riscritta `include/gim_string.h` e `src/gim_string.cc`. Corretti: copia profonda (costruttore di copia e assegnazione, prima condividevano il puntatore con double free), nessun limite fisso di 32 KB, `size()` sempre coerente, `clean()` svuota davvero, `cat()` funziona anche su testo vuoto, `left_find`/`right_find`/`equal` implementati (prima stub), `c_str()` non torna piu' NULL.
- [~] Memoria: il buffer viene da `gim_memory` e viene sbloccato subito (la configurazione predefinita blocca le allocazioni, e `Free` rifiuta i blocchi bloccati). `Relloc` non e' usato (ha il difetto noto).
- [~] Aggiunti: `compare`, `pos_of`/`rpos_of`, `substr`, `at`, `insert`, `erase`, `trim`/`trim_left`/`trim_right`, `upper`/`lower`, `replace`, `fields`/`field` (separatore), `starts_with`/`ends_with`, `set_format`/`cat_format`, `set_int`/`set_float`, `to_int`/`to_float`, `reserve`, operatori `=`, `==`, `!=`.
- [ ] Cambi di interfaccia da verificare nei chiamanti: `size()`/`length()`/`capacity()` tornano `_gim_Uint32`; `set`/`cat` accettano solo `const char *`; `equal` torna `__GIM_YES`/`__GIM_NO`; `set(char*)` e `cat(char*)` non esistono piu'; la copia del testo precedente (`str_bkup`) e' stata tolta.
- [ ] DA FARE: compilare e far girare `test/gim_string_test.cc` (deve dare 0 errori e nessun errore di memoria nel log), e compilare `gkdp`, `gkfp`, `gimstat` che usano `_gim_string`. Si fa su Proxima oppure su Ancarlo: entrambi sono spenti al momento (Proxima e a Genova, Ancarlo e qui ma spento).
- [ ] Non thread-safe (nessun lock). I puntatori restituiti da `c_str()`, `left_find()` e `right_find()` valgono fino alla prossima modifica del testo.
- [ ] Non coperti: codifica (hex, base64), iteratori, confronto senza distinzione maiuscole/minuscole.
