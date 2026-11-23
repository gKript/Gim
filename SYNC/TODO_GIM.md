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
