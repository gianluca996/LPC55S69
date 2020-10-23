

 * Gianluca Pasquali
 * USB Log Inizio Luglio 2020
 * 64K Flex_NVM salvataggio di 546 Log
 * Su ogni locazione uint32_t riesco a salvare 4 valori uint8_t
 * Indirizzo di partenza 0x10000000
 *
 * Utilizzo 60 K di Flash
 * Gli ultimi 2k (0x1000F800 -> 0x10010000) li utilizzo per salvare l'indirizzo del log da memorizzare
 * I 2K da 0x1000F000 -> 0x1000F800 li devo sacrificare per salvare il codice a 6 cifre
 *
 * US00 #000000# 			Ogni 10 secondi
 * #35#NEWCODE:,000000, 	Per memorizzare un nuovo codice a 6 cifre
 * #35# RESET 				Per resettare la memoria
 
   L'indirizzo di memoria del log (0x1000F800 -> 0x10010000) lo salvo nella stessa locazione ogni 512 Log. Considerando una media di 20 Log al giorno
   salvo l'indirizzo del Log nella stessa locazione ogni 25 giorni. Dato che ogni locazione può essere scritta 50.000 volte ho 50K*25 = 1.250.000 giorni / 365 = 3424 anni
   
   Per quanto riguarda i log ho gli stessi numeri. Ho 60K di flash (61440)/4 = 15360 locazioni uint32_t. Considerando che un log sono 30 locazioni ho esattamente 15360/30 = 512 log
   Considerando una media di 20 log al giorno scrivo nella stessa locazione ogni 25 gioni. Quindi dato che ogni locazione può essere scritta 50.000 volte ho 50K*25 = 1.250.000 giorni / 365 = 3424 anni
   
   - 02 Ottobre 2020
   Fine Sviluppo USB Log con salvataggio su eeprom e scrittura file su chiavetta.
   In test da venerdi 2 ottobre ore 17 fino a lunedi 5 ottobre ore 8 inviando un log ogni secondo.
   
   - 05 ottobre 2020
   Inizio sviluppo USB Log con scrittura log direttamente su chiavetta (No eeprom) che sarà sempre inserita.
   
   
   
   