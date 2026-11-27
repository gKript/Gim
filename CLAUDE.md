# CLAUDE.md - Gim

Istruzioni per chi lavora su Gim (libreria C++ e tool collegati). Le regole qui valgono per ogni sessione e ogni PC.

**All'inizio di ogni sessione:** leggere `SYNC/HANDOFF.md` (stato e passi per riprendere). Lo script `scripts/inizio_sessione.ps1` è lanciato automaticamente dall'hook `SessionStart` in `.claude/settings.json`; se l'output non compare, lanciarlo a mano.

**Controllo della modifica (da fare su ogni PC):** verificare che `.claude/settings.json` contenga un hook `SessionStart` che esegue `scripts/inizio_sessione.ps1`. Se manca, aggiungerlo come nel file di Gim. Il file non è in git: va copiato insieme alla cartella.

## Metodo di lavoro: usare le classi di Gim

Quando un codice ha bisogno di file, memoria, thread, tempi, hash, log o opzioni, si usano prima le classi di Gim, non la libreria standard o le chiamate di sistema.

**Perché.** Usare le classi di Gim fa due cose: fa funzionare il codice, e verifica le classi stesse. Se una classe è incompleta o non è thread-safe, la scopriamo e la correggiamo in Gim. Se la aggiriamo, il difetto resta nascosto e Gim non migliora.

### Classi verificate

La colonna "Thread-safe" dice cosa è stato verificato leggendo il sorgente.

| Esigenza | Classe | Metodi usati | Thread-safe? |
|---|---|---|---|
| file: apertura, lettura, scrittura | `gim_file_obj` | costruttore `(nome, metodo, modo)`, `check()`, `size()`, `load()`, `flush()`, `close()` | sì per istanze separate; passa dal gestore sotto |
| gestore dei file (lista condivisa) | `gim_file_manager_obj` | `open()`, `close()`, `load()`, `flush()` | sì: lock su `gim_global_lock()` (aggiunto nel lavoro GKFP) |
| memoria per i buffer | `gim_memory_obj` | `Alloc(size, codice, __GIM_HIDE)`, `Free(ptr)`, `Check(ptr)` | sì: mutex globale ricorsivo |
| thread | `gim_thread_obj` | `start(body)`, `start_if_allowed(classe, body)`, `join()`, `request_stop()` | un oggetto = un thread; rispetta `[thread]` in `gim.conf` |
| tempi | `gim_time_obj` | `start()`, `stop()`, `get()`, `start_usec()` | per istanze separate; `get_usec()`, `process()`, `date()` restituiscono un buffer statico condiviso: **non thread-safe** |
| hash MD5 | `gim_md5_obj` | `MD5Init()`, `MD5Update()` | sì, se ogni thread usa il proprio `MD5_CTX` |
| hash MD5 (wrapper) | `gim_checksum_obj` | `md5()` | **no**: restituisce un buffer `static` (`hex[33]`) |
| log e messaggi | `gim_error_obj` | `set(tipo, funzione, messaggio, valore)`, `Set(tipo, funzione, formato, ...)`, `app(tipo, funzione, formato, ...)` | sì: lock su `gim_global_lock()` |
| cifratura del flusso | `gim_cryptography_obj` | `derive_key()` (const), `apply_stream()` e `apply_stream_parallel()` (statici) | sì per i metodi statici; `advanced_password_nonce()` scrive su `gim_interface`: **no** |
| opzioni da riga di comando | `gim_getopt_obj` | `scanopt()`, `search()` | non verificato |

**Classi non verificate:** tutte le altre (per esempio `gim_list_obj`, `gim_directory_obj`, `gim_kvalue`, `gim_interface_obj`, `gim_serial_obj`, `gim_db_obj`, `gim_html_obj`, `gim_identity_obj`). Prima di usarle in codice concorrente, leggerne il sorgente.

### Regole

1. **Prima verificare la classe.** Leggere il sorgente: stato condiviso, buffer statici, lock, chi libera cosa. Non fidarsi del nome.
2. **Se la classe non basta, correggere la classe.** Non scrivere una versione parallela. La correzione va in Gim, con un test.
3. **La libreria standard è un'eccezione.** Si usa solo se la classe è verificata inadatta, e l'eccezione va motivata in un commento nel codice.
4. **Non liberare due volte.** `gim_file_obj::load()` alloca il buffer in `gim_memory` e `close()` lo libera già. Chiamare `unload()` dopo `close()` è una doppia liberazione (il log mostra `Frame NOT found`).
5. **Verificare ogni correzione** con un test: il contenuto o gli hash devono restare identici, e il log non deve mostrare errori di memoria.
6. **Riportare i difetti non corretti**, con una riga su cosa manca.

### Esempio: GKFP parallelo

La prima versione usava `std::thread` e `fopen`. Verificando le classi sono emersi: la lista dei file senza lock in `gim_file_manager_obj`, e `apply_stream_parallel` con `std::thread` che ignorava `[thread]`. Sono stati corretti usando `gim_file_manager_obj` con il lock, `gim_thread_obj` e `gim_file_obj`.

### Da verificare ancora

- il percorso senza thread (`[thread] gim_gkfp = NO`);
- i buffer statici di `gim_time` e `gim_checksum`;
- il comportamento di `gim_file_obj::unload()`, che lascia il puntatore pendente.

## Priorità: usare Gim e i suoi tool

Per tutto ciò che riguarda Gim (scrivere codice, compilare, provare, eseguire, impacchettare), si usano prima le classi e i tool di Gim, prima di qualsiasi altra scelta.

- **Build:** prima `gkmake`. Non usare `cmake --build` o altri strumenti al suo posto senza motivo scritto.

### Uso di gkmake (verificato 2026-11-26)

- Lanciare **`gkmake`** dal PATH, dalla cartella del progetto. Nel PATH c'è `build-mingw\gkmake.exe`. **Non** lanciare `build_e\gkmake.exe`: gkmake non può ricompilare sé stesso mentre è in esecuzione, e il link fallisce con "Permission denied".
- Opzioni: `-c` pulizia, `-d` documentazione, `-cd` pulizia poi documentazione, `-o` stampa la configurazione (sola lettura), `-h` aiuto.
- La cartella di build è registrata in `C:\Users\asynt\.gim\gKmake\build.conf` (sezione GIM, chiave `gim::dir` = `E:/GitHub Asy/Gim/build_e`). Non modificarla a mano.
- **Questo file sta nella cartella utente di questo PC, non sulla chiavetta.** Su un altro PC va registrato di nuovo: dalla cartella del progetto, `gkmake -K "<cartella di build>"` (vedi `gkmake -h`). Se gkmake dice "no build directory for project", è questo il passo che manca.
- Il file contiene anche la cartella di build di GimUI (`gimui::dir` = `E:/GitHub Asy/GimUI/build`), registrata da GimUI.
- **gkmake è multi utente e multi piattaforma.** `.gkmake_conf` ha un profilo per ogni sviluppatore e PC (ASYNTOTE su `osboxes`, SKYMATRIX su `meredith`, ASYNT su `Ancarlo`), con architettura, versione e contatori di build. Le cartelle di build sono per utente, in `~\.gim\gKmake\build.conf`. Non serve adattare il progetto a ogni PC: si lancia gkmake e lui usa il profilo e i percorsi del PC in cui gira.
- **La lettera di unità cambia per PC.** Ancarlo: `E:\GitHub Asy\`. Proxima: `I:\GitHub Asy\`. Per questo non si scrivono percorsi assoluti nei documenti condivisi: gkmake registra la cartella di build per ogni PC (`build.conf`) e risolve il percorso da lì. La cache CMake di Proxima punta a `I:\`, quindi su Ancarlo va rigenerata: è un motivo in più per usare gkmake invece di CMake a mano.
- I documenti di progetto (es. `GimUI\CLAUDE.md`, con `i:\GitHub Asy\Gim`) riportano il percorso di un solo PC: vanno letti come "la cartella Gim sull'unità del PC in uso".
- **Documentazione su Windows (2026-11-26):** `gkmake -d` esegue `documentation\make_doc.bat` (gkmake lo preferisce al comando in `.gkmake_conf`, vedi `gkmake/main.cc` ~riga 862), e quel file delega a `make_doc_modern.bat`: Doxygen con `gim_modern.css` e poi le card con `postprocess_cards.ps1`. Per avere l'aspetto moderno basta `gkmake -d`; non serve lanciare altro. Richiede **doxygen** e **Graphviz**: su questo PC `pacman -S mingw-w64-ucrt-x86_64-doxygen mingw-w64-ucrt-x86_64-graphviz`. Senza `dot` Doxygen non produce i grafici e stampa centinaia di errori `Failed to rename`. Su Linux `make_doc.sh` resta la versione vecchia (le card sono uno script PowerShell).
- La documentazione richiede **doxygen**. Su questo PC è installato con `pacman -S mingw-w64-ucrt-x86_64-doxygen` (MSYS2 UCRT64). Se manca, gkmake stampa "Doxygen failed" e prosegue.
- **Bug aperto:** gkmake esce con 0 anche quando il link o la documentazione falliscono. Controllare l'output, non il codice di uscita.
- gkmake aggiorna da solo `.gkmake_conf` e `include\gim_gkmake.h` (contatori di build e data). Sono modifiche normali; non ripristinarle senza motivo.
- **Build incrementale (2026-11-26):** i contatori di `gim_gkmake.h` li vede solo `src/gim_version.cc` (definisce `GIM_VERSION_INTERNAL`). Gli altri file usano `GIM_SUBMINOR` / `GIM_BUILD_LAST`, che ora chiamano `gim_version_subminor()` / `gim_version_build_last()`. Una build senza modifiche ricompila solo `gim_version.cc` e rifà i link (9 passi invece di 62). Non includere `gim_gkmake.h` in altri file, altrimenti torna a ricompilare tutto. La libreria porta il numero della build precedente, perché gkmake riscrive l'header dopo la build (comportamento di sempre).
- **Build parallela:** con CMake gkmake passa `--parallel N`, con make `-jN`. `-t N` vale solo per l'esecuzione; `-T N` lo salva nella sezione dell'utente in `.gkmake_conf`. Senza opzioni Ninja usa comunque tutti i core.
- Il titolo `BUILDING (#N)` mostra il **totale di progetto** (somma di tutte le sezioni di `.gkmake_conf`, uguale a `GIM_GKMAKE_TOTAL_BUILD`), non il contatore del singolo utente. Modificato il 2026-11-26 in `gkmake\main.cc`, riga ~804; verificato su Ancarlo (#1600 con header a 1599).
- **Altri tool di Gim:** `gimshell`, `gimstat`, `gkfp`, `gkdp`.
- Se un tool o una classe non basta, si correggono in Gim, con un test. Non si scrive una versione parallela.
- Ricorrere a strumenti esterni solo se quelli di Gim sono verificati inadatti, e dirlo nella risposta.
- I file di configurazione (`gim.conf`, `.gkmake_conf`) si scrivono con le API di Gim, non a mano.

## Altre regole di progetto

- **File di configurazione di Gim** (`gim.conf` e simili): non modificarli a mano. Si scrivono da codice, con le API di Gim.
- **Commit e push** solo su richiesta esplicita di chi lavora sul repo.
- **Nuovi file** `.cc`/`.h`: aggiungerli anche alle liste di `Makefile.am`, così la build Linux e `dist` continuano a funzionare.
- **Log**: tutto ciò che accade va registrato attraverso il log system di Gim (`gim_error`), non con `printf`, `cout` o file propri. Il livello minimo (`debug::min_type`) è una scelta consapevole di chi usa il log, non una violazione: con un livello alto alcuni messaggi non vengono registrati di proposito. Il `-q` riguarda solo l'output a schermo.

## Stato

Le modifiche del lavoro GKFP sono nel working tree del branch `windows-port`, non committate.

## Uso dell'advisor (skill /gestione-advisor)
- Per tutti i progetti: usare la skill `/gestione-advisor` per decidere quando chiamare il modello più capace (advisor).
- Chiamare l'advisor solo per: pianificazione di una feature che tocca più file o l'architettura core; stallo dopo 2 tentativi falliti con esito verificato; validazione prima di consolidare modifiche alla logica principale.
- Massimo 1 chiamata per fase di pianificazione e 1 per ogni stallo distinto. Per lavoro ripetitivo, build, test e check: non chiamarlo.
- Annotare in una riga la decisione presa, per non richiederla due volte.


## Riferimenti (aggiornati 2026-11-26)
- **Test:** `test\db_records\run_db_records.ps1` (database: record, drop, GDBS, CSV; si rimuove da solo), `test\gim_kvalue_delsection_test.cc` (`DelSection`), `test\gim_list_test.cc` (indici di `gim_list`). I due `.cc` si compilano a mano con `g++` e le librerie di gkmake (vedi il commento in fondo a ciascun file).
- **Liste:** `gim_list` tiene i puntatori generici; la proprietà dei membri è spiegata in `include\gim_list.h` (`destroy_list` libera solo i membri allocati con `gim_memory`, a un livello).
- **GitHub:** repository pubblico `gKript/Gim`. Il PC non ha `gh`: gli issue si leggono con `curl` dall'API pubblica (`https://api.github.com/repos/gKript/Gim/issues?state=open`). Nessuna credenziale.
- **Stato del lavoro:** `SYNC\HANDOFF.md` (stato e passi) e `SYNC\TODO_GIM.md` (voci aperte).
