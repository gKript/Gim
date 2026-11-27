# Da testare (sessione del 2026-11-27)

Tutto quello che è stato scritto o cambiato in questa sessione e NON è mai stato compilato né eseguito.
Il PC di lavoro non ha compilatore, e la chiavetta `E:` blocca l'esecuzione dei programmi (doxygen, gkmake): quindi i test vanno fatti su Proxima o Ancarlo, spenti al momento.

Per ogni voce: compilare, eseguire, annotare il risultato qui. Non chiudere nulla senza test verdi.

## 1. Codice cambiato

### gim_string (riscritta)
- [ ] `include/gim_string.h`, `src/gim_string.cc`: compilano con gcc/g++ della toolchain di Gim
- [ ] `test/gim_string_test.cc`: esce con 0, nessun FAIL nel log
- [ ] Nessun errore di memoria nel log di gim (gim_error) dopo il test
- [ ] `gkdp`, `gkfp`, `gimstat` compilano con `_gim_string` (i tre usano la classe)

### gim_file (correzioni)
- [ ] `close(name)` non libera due volte
- [ ] `unload()` azzera `loadp` dopo la liberazione: una `load()` successiva non restituisce il puntatore libero
- [ ] `open_fd`: `O_WRONLY | O_CREAT | O_TRUNC | O_BINARY` in WRITE; `O_APPEND | O_CREAT | O_BINARY` in APPEND
- [ ] `flush()` con handler `NULL` restituisce `__GIM_NOT_OK` senza crash
- [ ] `type()` usa `handler->name`
- [ ] Nessun test dedicato ancora: usare `test/gim_file_load_test.cc` come base e aggiungere un caso per ciascuna voce

### gim_time (documentata e rivista)
- [ ] `include/gim_time.h`, `src/gim_time.cc` compilano
- [ ] `test/gim_time_test.cc`: esce con 0 (formato di `get_usec_buf`, `process_buf`, `date_buf`, `date()`, `process_sec`, `process_msec`)
- [ ] `date_buf()` su Windows (`localtime_s`) e su Linux (`localtime_r`): stessa data
- [ ] `process_sec()` vicino a `time(NULL)`; `process_msec()` tra 0 e 999
- [ ] Tutta la classe su almeno due sistemi (Windows e Linux)

### gim_checksum (documentata, *_buf nuovi, chsum tolte)
- [ ] `include/gim_checksum.h`, `src/gim_checksum.cc` compilano
- [ ] `test/gim_checksum_test.cc`: esce con 0 (vettori MD5, SHA-1, SHA-256, SHA-512 per "abc")
- [ ] Versioni `*_file_buf()` su un file vero (non solo il file mancante): stesso risultato della versione con buffer
- [ ] Nessun chiamante di `chsum` / `chsum8` nel repository (controllare con grep prima del build)

### gim_gkfp_file_data.cc (migrazione a md5_buf)
- [ ] Compila
- [ ] gkfp in modalità threaded (`threads > 1`, `[thread] gim_gkfp = YES`): i checksum scritti nel file sono identici a quelli del build precedente sullo stesso input
- [ ] gkfp in modalità non threaded: stessi checksum
- [ ] Lettura (`read_data`): nessun "checksum error" su un file integro; un file alterato dà l'errore

### gim_gkp_cryptography (solo commenti nuovi)
- [ ] Compila (sono commenti Doxygen sulle funzioni `static`)

### gim_getopt (solo documentazione)
- [ ] Compila (commenti)
- [ ] `test/gim_getopt_test.cc` NON ESISTE ANCORA: da scrivere, con i casi: opzione valida, valore mancante, opzione sconosciuta, senza `:` iniziale, `search()` su opzione assente, due chiamate a `scanopt()`

### Intestazioni e licenza (solo commenti)
- [ ] Tutti i file di `include/` e `src/` compilano dopo il cambio di banner (circa 130 file): nessun `*/` mancante, nessun commento non chiuso. Il warning di Doxygen su `gim_directory.h` (riga 158, commento annidato) è da verificare: potrebbe essere già presente prima
- [ ] `include/gim_mersennetwister.h`: il notice di Matsumoto/Nishimura è intatto (non è nostro)
- [ ] `gim_shell/*`, `gim_ai_train/main.cc`: compilano con il banner nuovo

### gkmake: template dei file generati
- [ ] `gkmake/gkm_header.cc` compila (cambio di testo nel template C++)
- [ ] Rigenerare un build di gkmake: il file `include/gim_gkmake.h` generato contiene il testo GPL v3 (`along with this program.  If not, see <https://www.gnu.org/licenses/>.`)
- [ ] `gkmake/gkm_main.h`: solo testo di esempio, controllare che nessun altro file lo usi come sorgente

## 2. Documentazione

- [ ] `documentation\make_doc_modern.bat` eseguito senza errori: 76 warning di Doxygen, come prima. Nessuno nuovo su `gim_time`, `gim_checksum`, `gim_getopt`, `gim_gkp_cryptography`
- [ ] Pagine `gimdoc_getopt`, `gimdoc_time`, `gimdoc_checksum`, `gimdoc_string` generate e collegate dall'indice
- [ ] Il link `\ref gimdoc_getopt` in `include/gim_doxygen.h` (sezione tool) risolve
- [ ] La riga della tabella classi per `gim_time_obj` in `include/gim_doxygen.h` (riga 223 circa) dice quello che deve dire

## 3. Cose ancora da scrivere (non ancora fatte)

- [ ] `test/gim_getopt_test.cc` (vedi sopra)
- [ ] Migrazione dei chiamanti statici di `md5()`/`sha1()`/`sha256()`/`sha512()` in `gim.cc`, `gim_db.cc`, `gim_gkp_header.cc`, `gim_gkfp_header.cc`, `gim_gkp_password.cc`, `gim_private_key.cc`, `gim_gkfp_file_list.cc`
- [ ] Piano cifratura (`SYNC/CRITTOGRAFIA_PIANO.md`): passo 1 (sorgente di casualità), poi 2-7. Nessun codice ancora

## 4. Bloccato dalla chiavetta

- [ ] `doxygen.exe` e `gkmake.exe` su `E:` non partono: "Accesso negato". Causa probabile: regola Defender "blocca processi non attendibili e non firmati eseguiti da USB". Decisione: amministratore o build su Proxima/Ancarlo.
