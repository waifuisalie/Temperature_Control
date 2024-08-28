;;
; teste_exaustor.asm
;
; Created: 26/04/2024 20:19:41
; Author : stefa
;
	rjmp		start
	.include	"my_tx_rx.inc"

; Replace with your application code
start:
	call	config
	rjmp	ligado


desligado_init_msg:
	txs		string_turning_off
desligado:
	cbi		PORTD, 6				// desligo LED1
	cbi		PORTD, 7				// desligo LED2

	// wait/check for user input
	call	rx_R17
	cpi		R17, '+'				// if its '+'
	brne	desligado


ligado_init_msg:
	txs		string_its_on
ligado:
	sbi		PORTD, 6				// ligando LED1
	cbi		PORTD, 7				// ligando LED2

	// starts conversion
	store	ADCSRA, 0xC7			// 0b1100 0111
	
	// checks user input
	call	verifica_user_input
	breq	desligado_init_msg

	// checks if conversion is complete
	lds		R16, ADCSRA 
	sbrc	R16, 6					// 0b1000 0111
	rjmp	ligado

	// loads converted value
	lds		R18, ADCH				
	cpi		R18, 0x74				// checks if T=50 

	brsh	exaustor_on_init_msg	// (unsigned)
	rjmp	ligado


exaustor_on_init_msg:
	txs		string_exhaust_is_on
exaustor_on:
	sbi		PORTD, 6				// liga LED1
	sbi		PORTD, 7				// liga LED2

	// starts conversion
	store	ADCSRA, 0xC7			// 0b1100 0111

	call	verifica_user_input
	breq	desligado_init_msg

	// checks if conversion is done
	lds		R16, ADCSRA				// 0b1000 0111
	sbrc	R16, 6
	rjmp	exaustor_on

	// loads converted value
	lds		R18, ADCH
	cpi		R18, 0x5D				// checks if T=40

	breq	to_ligado	
	brlo	to_ligado				// (unsigned)

	rjmp	exaustor_on				// else, keep the exhaust going

to_ligado:
	txs		string_exhaust_is_off
	jmp		ligado


string_its_on:
	.db '\n', "Controle de temperatura ligado!", '\n', 0

string_exhaust_is_on:
	.db '\n', "Exaustor ligado", '\n', 0

string_exhaust_is_off:
	.db '\n', "Exaustor desligado", '\n', 0, 0

string_turning_off:
	.db '\n',"Desligando...", '\n', 0