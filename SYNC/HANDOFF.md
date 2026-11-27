# HANDOFF: come riprendere il lavoro su Gim

Scritto per una nuova sessione che non ha memoria del lavoro precedente. Aggiornare alla fine di ogni sessione.

Stato al 2026-11-27.

## 1. Partenza (sempre)

1. Dalla cartella `E:\GitHub Asy\Gim`, lanciare `scripts\inizio_sessione.ps1`: dà branch, modifiche, voci aperte e strumenti.
2. Leggere `CLAUDE.md`: regole di progetto. Vale sopra a tutto questo file.
3. Leggere `SYNC\TODO_GIM.md`: le voci aperte, con lo stato di ogni voce.

## 2. Regole da non dimenticare

- Usare le classi e i tool di Gim prima di altre scelte. Build con **`gkmake`** dal PATH, mai da `build_e\gkmake.exe` (si blocca da solo).
- Documentazione: `gkmake -d` (usa `make_doc.bat` che delega a `make_doc_modern.bat`: card e grafici). Richiede doxygen e Graphviz.
- Non committare né fare push senza richiesta esplicita.
- I file di configurazione (`gim.conf`, `.gkmake_conf`) non si modificano a mano.
- Nuovi `.cc`/`.h`: aggiungerli anche alle liste di `Makefile.am`.
- Ogni correzione con un test; il log non deve mostrare errori di memoria.
- Skill `/gestione-advisor`: l'advisor solo per pianificazione, stalli (2 tentativi con esito verificato) e validazione; max 1 per fase.

## 3a. Lavoro del 2026-11-27 (Ancarlo)

**Script di avvio** (`scripts\inizio_sessione.ps1`, non in git)
- Primo passo: riconosce il PC da `$env:COMPUTERNAME` (confronto senza maiuscole). Tabella `$pcProfiles`: `ANCARLO` e `PROXIMA` = `full`. Un PC non in elenco (es. portatile di lavoro) va in `readonly`: niente git né controllo degli strumenti, solo memorie e data di HANDOFF.
- In `full`: `git fetch` (limite 30 s), commit avanti/indietro rispetto a origin, working tree, voci del TODO, strumenti.
- In entrambi i modi legge tutte le memorie di Claude: tutti i progetti in `~\.claude\projects\*\memory` e le `.claude\memory` del disco condiviso. Opzioni: `-Mode`, `-NoFetch`, `-NoMemory`.
- Provato su Ancarlo e con nomi simulati. NON ancora lanciato su Proxima. Mai provati: il timeout del fetch e la lettura di memorie dal disco condiviso (oggi non ce ne sono).

**Test rieseguiti: tutti verdi**
- `test\db_records\run_db_records.ps1`: ALL OK.
- `test\gim_list_test.cc` e `test\gim_kvalue_delsection_test.cc`: ALL OK (0 failures). Prima non compilavano: dei commenti `/* */` dentro i blocchi Doxygen `/*! \page` di `include\gim_list.h` (righe ~220, ~226) e `include\gim_string.h` (righe ~294-295) chiudevano il blocco in anticipo. Corretti con `//`. Solo commenti, nessun codice cambiato.
- Build `gkmake` (#1678) completata senza errori dopo la correzione.

**Cifratura: formato del file**
- Nuovo `SYNC\FORMATO_FILE.md` (passo 0 di `SYNC\CRITTOGRAFIA_PIANO.md`): lavora su buffer, solo modo A (password), modo B riservato e rifiutato. Intestazione di 109 byte, magic `GKRT`, byte `version`, tutta l'intestazione autenticata. Nessun codice scritto.
- Decisioni dell'utente: l'API lavora su buffer, non su file. `gim_gkdp_obj` non è un passaggio obbligatorio. PicGIM: nessuna cifratura sulla seriale K22-K50 (stessa scheda). Al massimo, in futuro, un sistema di messaggistica, con perimetro da definire.
- Discussa e non ancora scritta nella specifica: la sezione "Versioni". `reserved` e i bit di `flags` non usati valgono 0 e vengono rifiutati se diversi. Una versione nuova solo quando cambia il layout. Lo scrittore produce solo l'ultima versione, il lettore le legge tutte. Un file di test per ogni versione, conservato nel repo.
- Restano aperti: il minimo di iterazioni PBKDF2 (da misurare su Ancarlo), la dimensione massima del payload, e la scelta tra classe nuova o modifica di `gim_gkp_cryptography` (proposta: classe nuova).
- `crypt::iterations` (2 in `gim.conf`, 5 forzato in `gkdp.cc` riga ~144) NON va usato per PBKDF2.

**Scoperte nel codice**
- `compatibility.conf`: ci sono riferimenti (nome del file, oggetto `gim_compatibility`, sezione in `gim.conf` con `enable = NO`) dal primo commit, ma il file non è gestito e non c'è un meccanismo di compatibilità. L'antenata CNC 0.3-3 (`gKript.org\old\0.9-dev-x\documentations\cnc\0.3-3\cnc_kernel.h`) aveva `cnc_compatibility_list_struct`, cioè funzioni vecchie riattivabili per aprire file vecchi.
- Unico controllo di compatibilità attivo: `gim_db.cc` righe ~270-279 (nome dell'applicazione e versione major/minor del DB).
- `gim_private_key_obj::grant()` (esporta la chiave in formato GKP per darla ad altri) e `acquire()` (importa in `OtherKeys`): solo dichiarati con `\todo`, mai implementati. Una chiamata fallirebbe al link. Idea da mettere nel piano, modo B: `grant` esporta la chiave PUBBLICA X25519 con il blocco di alias, `acquire` la salva nella rubrica dopo la conferma dell'impronta.
- La "chiave privata" attuale è una stringa da Mersenne Twister e dati di alias, non una coppia di chiavi.
- `gim_gkp_header`: niente versione del formato, struct scritta grezza (`sizeof`), solo hash MD5 dell'intestazione.

**Cartelle in `~\.gim`** (discussione, nulla spostato)
- Le cartelle dei test (`cryptcheck`, `ListTest`, `DelSectionTest`, `speed`, `gkdp_api_test`…) andrebbero pulite dai test stessi.
- `PrivateKey` e `OtherKeys` NON vanno sul disco condiviso (la privata finirebbe su ogni PC). Il percorso è in `gim_environment.cc` righe ~101-102.

**Altro**
- `E:\GitHub Asy\CLAUDE.md`: aggiunta la sezione "Se sei Haiku: regole per le risposte".

## 3. Stato del 2026-11-26

- Branch `windows-port`, ultimo commit `986b582`. Tutto il lavoro di oggi è **non committato** (63 file circa).
- Build: `gkmake` riuscita (contatore 1443 e oltre). Toolchain MSYS2 UCRT64 con doxygen e Graphviz installati.

**Database (`gim_db`), voce #1 e #2 del TODO**
- `update_record`, `delete_record`, `drop_table`: API, shell (`update`, `delete`, `table drop`) e GDBS (`UPDATE`, `DELETE`, `DROP TABLE`). GDBS solo uguaglianza.
- `export_csv` / `import_csv`: shell `table export` e `table import`. Formato del trainer, impronta SHA-256 sul file. La KEY è opzionale; se c'è, i valori devono essere nuovi. APERTI: comandi GDBS per CSV.
- Test: `test\db_records\run_db_records.ps1` (28 controlli, tutti verdi). Rimuove da sola le proprie cartelle di test.

**Altre correzioni di oggi**
- `gim_list`: `del_item` non rinumerava gli indici (duplicati dopo add). Corretto, test in `test\gim_list_test.cc`. Documentata la proprietà dei membri in `destroy_list`.
- `gim_kvalue`: `Read` accetta un file con solo il titolo (tabella svuotata); `GetHowManySection` torna 0 senza sezioni; `ChangeKey(float)`. Test `test\gim_kvalue_delsection_test.cc` (32 controlli).
- Documentazione: `gkmake -d` genera la versione con card; Doxyfile con `DISABLE_INDEX = YES`, `FULL_SIDEBAR = NO` (testata a tutta larghezza); CSS sidebar in `gim_modern.css`.

## 4. Problemi aperti noti

- gkmake esce con 0 anche quando link o doxygen falliscono.
- `gim_list`: `seek` dichiarato `_gim_flag` ma ritorna `NULL`; `destroy_list` non azzera lo stato (oggi non raggiungibile).
- Il messaggio CRITICAL "gim_list_obj::rewind(): The list is empty" compare a ogni avvio di gkmake (ora WARNING, ma il log su file di gkmake è disattivato).
- `gim_memory_obj::Memory_update`: graffe mancanti nell'`if`, il messaggio "setted" viene sempre stampato.
- Il database accetta valori che il trainer scarta (range 0..65535 di `ch0`).
- Il database `prova` nella home di Gim è sparito durante i test, causa non spiegata.
- Import CSV con database PERMANENT lento: ogni riga riscrive il file (serve l'inserimento a blocchi).

## 5. Issue su GitHub (gKript/Gim, aperti, 2020)

- #7 gkmake: disinstallare un progetto.
- #6 gkmake: file come template (variabili sostituite).
- #4 `gim_ascii_buffer`: due metodi da completare. Da verificare sul codice locale: è la classe che il futuro parser JSON potrebbe usare.

## 6. Decisioni in attesa (dell'utente)

- Conferma del push di `f4494e9` e di tutto il lavoro di oggi (per ora nulla è committato).
- Spostare `SYNC\NOTE_GIMUI_SECURITY.md` fuori da `SYNC`.
- Questione della chiave privata (sale o chiave pubblica).
- Formati di compressione per la nuova classe.
- Classe JSON: tre decisioni (chiavi duplicate, numeri come testo o tipizzati, oggetti annidati nel database).
- Range del trainer: il database deve rifiutare i valori fuori range, o basta il trainer?

## 7. Prossimo passo

Cifratura: aggiungere la sezione "Versioni" a `SYNC\FORMATO_FILE.md`, poi il passo 1 del piano (casualità del sistema). Prima, una decisione dell'utente: classe nuova o modifica di `gim_gkp_cryptography`. Su Proxima: lanciare lo script di avvio e verificare il modo `full`.

Database (in attesa): voce successiva del TODO sul database: inserimento a blocchi e salvataggio differito (il CSV su database PERMANENT riscrive il file a ogni riga). Poi i comandi GDBS per CSV.
