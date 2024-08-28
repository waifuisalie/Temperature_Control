/*
 * my_tx_rx.inc
 *
 *  Created: 20/04/2024 21:56:37
 *   Author: stefa
 */ 

 .macro		le_tabela_R16
	push	R31
	push	R30
	push	R16
		add		R30, R16                // adding the index I want to select to read (I assume these gave been added)
		ldi		R16, 0					// for carry operation
		adc		R31, R16
		lpm		@1, Z					// finally, the result is assigned to R17
	pop		R16
	pop		R30
	pop		R31
.endmacro	

.macro		 store
	push	R16
		ldi		R16, @1
		sts		@0, R16
	pop		R16
.endm

.macro		store_io
	push	R16
		ldi		R16, @1
		out		@0, R16
	pop		R16
.endm

.macro		tx_char
	push	R17
		ldi		R17, @0
		call	tx_R17
	pop		R17
.endm

.macro		tx_register
	push	R17
		mov		R17, @0
		call	tx_R17
	pop		R17
.endm

.macro txs
	push	R30
	push	R31
		ldi		R31, HIGH(@0*2)
		ldi		R30, LOW(@0*2)
		call	tx_string
	pop		R31
	pop		R30
.endmacro

.macro	clear_zero_flag
	cp		R17, R16						// because R16 != R17, cp will result in Z == 0
.endm

.macro	load_n_compare
	lds		R17, UDR0
	cpi		R17, '-'
.endm

 config:
	; configura serial
	store	UBRR0L,0x33	; Baudrate 38400bps
	store	UBRR0H,0x00
	store	UCSR0A,0x22
	store	UCSR0B,0x18
	store	UCSR0C,0x0E

	; configura ADC
	store	ADMUX, 0xE3
	store	ADCSRA, 0x87

	;configura DDRD
	store_io	DDRD, 0xFF
	ret

tx_R17:
	push	R16
tx_R17_volta:
	lds		R16, UCSR0A		; with in wouldnt work bc its only for IO register (we are loading into R18 the flag to see if buffer is empty)
	sbrs	R16, 5			; if set, indicates that transmit buffer is ready to receive data
	rjmp	tx_R17_volta
	sts		UDR0, R17		; with out wouldnt work bc its only for IO register
	pop		R16
	ret

rx_R17:
	push	R16
rx_R17_loop:
	lds		R16, UCSR0A
	sbrs	R16, 7
	jmp		rx_R17_loop
	lds		R17, UDR0
	pop		R16
	ret

tx_string:
	push	R16
	push	R17
	ldi		R16, 0
	tx_string_loop:
		le_tabela_R16	string1, R17		// read the memory provided by index, and now is stored in R17
		cpi		R17, 0
		breq	tx_string_fim				// if its equal to 0, will jump to end
		call	tx_R17
		inc		R16							// this will run through the chars 
		jmp		tx_string_loop
	tx_string_fim:
		pop		R17
		pop		R16
		ret



verifica_user_input:
	// sets zero flag to 0 in status register
	clear_zero_flag									// for some reason, if I use 'clz'. it never gets to exaustor_on_init_msg. My code gets stuck on the ligado state forever.

	// checks if user has sent anything
	lds		R16, UCSR0A
	sbrc	R16, 7
	load_n_compare								// if user input = '-', then Z will be set to 1
	ret

