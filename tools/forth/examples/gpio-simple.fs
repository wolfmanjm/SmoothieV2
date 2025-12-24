\ gpio-simple.fs

#require lib_registers.fs
#require cycles.fs

: setbit 1 swap lshift ;  \ Calculates the value for bit 0 (LSB) to bit 31 (MSB)

\ create a pin definition ( port pin -- addr ; -- pin port)
: pin <builds , , does> dup @ swap 1 cells + @ ;
\ eg PORTA 0 pin button1
\ eg PORTC 13 pin led1

\ set pin to output
: output ( pin port -- )
    MODE_OUTPUT -rot set-moder \ takes mode pin port
;

\ set output pin to opendrain
: opendrain ( pin port -- )
	>R %1 over lshift r@ _pOTYPER bic! 	\ clear ..
	%1 swap lshift R> _pOTYPER bis!		\ .. set opendrain
;

\ set pin to input
: input ( pin port -- )
    MODE_INPUT -rot set-moder \ takes mode pin port
;

\ set input pin to pull up
: pu ( pin port -- )
	>R 2* %11 over lshift r@ _pPUPDR bic! 	\ clear ..
	%01 swap lshift R> _pPUPDR bis!			\ .. set pullup
;

\ set input pin to pull down
: pd ( pinmsk port -- )
	>R 2* %11 over lshift r@ _pPUPDR bic! 	\ clear ..
	%10 swap lshift R> _pPUPDR bis!			\ .. set pulldown
;

\ set given pin to value
: set ( value pin port -- )
	_pODR
	swap bit swap
	rot         \ move value to tos
	0= if bic! else bis! then
;

\ is given pin set
: set? ( pin port -- ? )
	_pIDR swap bit swap bit@
;

\ define builtin pins
PORTB 0 pin led1
PORTE 1 pin led2
PORTB 14 pin led3
PORTC 13 pin switch1

: green-led led1 ;
: yellow-led led2 ;
: red-led led3 ;

: bsp-init
    PORTB enable-port
    PORTC enable-port
    PORTE enable-port

	led1 output
	led2 output
	led3 output
	switch1 2dup input pd
;

0 variable cnt
: gpio-test
	init-cycles
	bsp-init
	begin
		$01 cnt bit@ led1 set
		$02 cnt bit@ led2 set
		$04 cnt bit@ led3 set

		200 ms

		1 cnt +!

		switch1 set? if exit then

		key?
	until
;
