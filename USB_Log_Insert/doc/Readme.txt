

Test del 22 Ottobre 2020

* Tolto il tx da interrupt Timer
* In Test dalle 14.40 del 22 ottobre 2020
* void DEMO_UART_IRQHandler(void) 	tolto 	Dati_Presenti = 0;
* Fino alle 9.00 del 23 ottobre non c'è stato nessun errore




Test del 23 Ottobre 2020

* Adesso provo a riabilitare il Tx da interruput del timer
* Abilitato DisableIRQ(DEMO_UART_IRQn); 	in if ((true == pitIsrFlag)
* Abilitato EnableIRQ(DEMO_UART_IRQn);		in if ((true == pitIsrFlag)
			
			

  

