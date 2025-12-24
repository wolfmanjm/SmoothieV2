\
\ 	register definitions for STM32H74x
\
\		based on work by Ralph Sahli, 2017
\		

400000000 constant hclk	\ system clock: 400MHz

: registers ( -- )
	0 ;				\ offset start

: reg 
    <builds 		( offset -- newoffset )
		dup , cell+	
    does>			( structure-base -- structure-member-address )  
		@ + ;

: regC
    <builds 		( offset -- newoffset )
		dup , cell+	
    does>			( structure-base stream -- structure-member-address )  
		@ swap $18 * + + ;

: end-registers ( -- )
	drop ;			\ last offset

\ bit masks
: bit ( n -- n )
	1 swap lshift 1-foldable ;

$E000E100 constant NVIC
$58000400 constant SYSCFG

$58024400 constant RCC ( Reset and clock control )
	registers
		drop $2C
		reg _rPLLCFGR 		\ RCC PLLs Configuration  Register
		drop $d0
		reg _rRSR (  )  	\ RCC Reset Status Register
		reg _rAHB3ENR (  )  \ RCC AHB3 Clock Register
		reg _rAHB1ENR (  )  \ RCC AHB1 Clock Register
		reg _rAHB2ENR (  )  \ RCC AHB2 Clock Register
		reg _rAHB4ENR (  )  \ RCC AHB4 Clock Register
		reg _rAPB3ENR (  )  \ RCC APB3 Clock Register
		reg _rAPB1LENR (  )  \ RCC APB1 Clock Register
		reg _rAPB1HENR (  )  \ RCC APB1 Clock Register
		reg _rAPB2ENR (  )  \ RCC APB2 Clock Register
		reg _rAPB4ENR (  )  \ RCC APB4 Clock Register
	end-registers

$58020000 constant PORTA
$58020400 constant PORTB
$58020800 constant PORTC
$58020C00 constant PORTD
$58021000 constant PORTE
$58021400 constant PORTF
$58021800 constant PORTG
$58021C00 constant PORTH
$58022000 constant PORTI
$58022400 constant PORTJ
$58022800 constant PORTK

	registers
		reg _pMODER   	\ Port Mode Register - 00=Input  01=Output  10=Alternate  11=Analog
		reg _pOTYPER  	\ Port Output type register - (0) Push/Pull vs. (1) Open Drain
		reg _pOSPEEDR 	\ Output Speed Register - 00=2 MHz  01=25 MHz  10=50 MHz  11=100 MHz
		reg _pPUPDR		\ Pullup / Pulldown - 00=none  01=Pullup  10=Pulldown
		reg _pIDR		\ Input Data Register
		reg _pODR     	\ Output Data Register
		reg _pBSRR		\ port bit set/reset register
		reg _pLCKR		\ port configuration lock register
		reg _pAFRL		\ Alternate function  low register
		reg _pAFRH		\ Alternate function high register
	end-registers

$40013000 constant SPI1 ( Serial peripheral interface )
$40003800 constant SPI2 ( Serial peripheral interface )
$40003C00 constant SPI3 ( Serial peripheral interface )
$40013400 constant SPI4 ( Serial peripheral interface )
$40015000 constant SPI5 ( Serial peripheral interface )
$58001400 constant SPI6 ( Serial peripheral interface )
	registers
		reg _sCR1 (  )  			\ control register 1
		reg _sCR2 (  )  			\ control register 2
		reg _sCFG1 ( read-write ) 	\ configuration register 1
		reg _sCFG2 ( read-write )  	\ configuration register 2
		reg _sIER (  )  			\ Interrupt Enable Register
		reg _sSR ( read-only )  	\ Status Register
		reg _sIFCR ( write-only )  	\ Interrupt/Status Flags Clear  Register
		drop $20
		reg _sTXDR ( write-only )  	\ Transmit Data Register
		drop $30
		reg _sRXDR ( read-only )  	\ Receive Data Register
		drop $40
		reg _sCRCPOLY ( read-write ) \ Polynomial Register
		reg _sTXCRC ( read-write )  \ Transmitter CRC Register
		reg _sRXCRC ( read-write )  \ Receiver CRC Register
		reg _sUDRDR ( read-write )  \ Underrun Data Register
		reg _sI2SCFGR ( read-write )  	\ configuration register
	end-registers


$40005400 constant I2C1 ( I2C )
$40005800 constant I2C2 ( I2C )
$40005C00 constant I2C3 ( I2C )
$58001C00 constant I2C4 ( I2C )
	registers
		reg _iCR1 ( read-write )
		reg _iCR2 ( read-write )
		reg _iOAR1 ( read-write )
		reg _iOAR2 ( read-write )
		reg _iTIMINGR ( read-write )
		reg _iTIMEOUTR ( read-write )
		reg _iISR (  )
		reg _iICR ( write-only )
		reg _iPECR ( read-only )
		reg _iRXDR ( read-only )
		reg _iTXDR ( read-write )
	end-registers

	
$40011000 constant USART1
$40004400 constant USART2
$40004800 constant USART3
$40011400 constant USART6
$40004C00 constant UART4
$40005000 constant UART5
$40007800 constant UART7
$40007C00 constant UART8
	registers
		reg _uCR1 ( read-write )  \ Control register 1
		reg _uCR2 ( read-write )  \ Control register 2
		reg _uCR3 ( read-write )  \ Control register 3
		reg _uBRR ( read-write )  \ Baud rate register
		reg _uGTPR ( read-write )  \ Guard time and prescaler  register
		reg _uRTOR ( read-write )  \ Receiver timeout register
		reg _uRQR ( write-only )  \ Request register
		reg _uISR ( read-only )  \ Interrupt & status  register
		reg _uICR ( write-only )  \ Interrupt flag clear register
		reg _uRDR ( read-only )  \ Receive data register
		reg _uTDR ( read-write )  \ Transmit data register
		reg _uPRESC ( read-write )  \ USART prescaler register
	end-registers


$40000000 constant TIM2 ( General purpose timers )
$40000400 constant TIM3 ( General purpose timers )
$40000800 constant TIM4 ( General purpose timers )
$40000C00 constant TIM5 ( General purpose timers )
$40001800 constant TIM12 ( General purpose timers )
$40001C00 constant TIM13 ( General purpose timers )
$40002000 constant TIM14 ( General purpose timers )

	registers
		reg _tCR1 ( read-write )  \ control register 1
		reg _tCR2 ( read-write )  \ control register 2
		reg _tSMCR ( read-write )  \ slave mode control register
		reg _tDIER ( read-write )  \ DMA/Interrupt enable register
		reg _tSR ( read-write )  \ status register
		reg _tEGR ( write-only )  \ event generation register
		reg _tCCMR1 ( read-write )  \ capture/compare mode register 1 output  mode
		reg _tCCMR2 ( read-write )  \ capture/compare mode register 2 output  mode
		reg _tCCER ( read-write )  \ capture/compare enable  register
		reg _tCNT ( read-write )  \ counter
		reg _tPSC ( read-write )  \ prescaler
		reg _tARR ( read-write )  \ auto-reload register
		drop $34
		reg _tCCR1 ( read-write )  \ capture/compare register 1
		reg _tCCR2 ( read-write )  \ capture/compare register 2
		reg _tCCR3 ( read-write )  \ capture/compare register 3
		reg _tCCR4 ( read-write )  \ capture/compare register 4
		reg _tDCR ( read-write )  \ DMA control register
		reg _tDMAR ( read-write )  \ DMA address for full transfer
		drop $60
		reg _tAF1 ( read-write )  \ TIM alternate function option register  1
		drop $68
		reg _tTISEL ( read-write )  \ TIM timer input selection
	end-registers

$40010000 constant TIM1 ( Advanced-timers )
$40010400 constant TIM8 ( Advanced-timers )
	registers
		reg _atCR1 ( read-write )  \ control register 1
		reg _atCR2 ( read-write )  \ control register 2
		reg _atSMCR ( read-write )  \ slave mode control register
		reg _atDIER ( read-write )  \ DMA/Interrupt enable register
		reg _atSR ( read-write )  \ status register
		reg _atEGR ( write-only )  \ event generation register
		reg _atCCMR1 ( read-write )  \ capture/compare mode register 1 output  mode
		reg _atCCMR2 ( read-write )  \ capture/compare mode register 2 output  mode
		reg _atCCER ( read-write )  \ capture/compare enable  register
		reg _atCNT (  )  \ counter
		reg _atPSC ( read-write )  \ prescaler
		reg _atARR ( read-write )  \ auto-reload register
		reg _atRCR ( read-write )  \ repetition counter register
		reg _atCCR1 ( read-write )  \ capture/compare register 1
		reg _atCCR2 ( read-write )  \ capture/compare register 2
		reg _atCCR3 ( read-write )  \ capture/compare register 3
		reg _atCCR4 ( read-write )  \ capture/compare register 4
		reg _atBDTR ( read-write )  \ break and dead-time register
		reg _atDCR ( read-write )  \ DMA control register
		reg _atDMAR ( read-write )  \ DMA address for full transfer
		drop $54
		reg _atCCMR3_Output ( read-write )  \ capture/compare mode register 3 output  mode
		reg _atCCR5 ( read-write )  \ capture/compare register 5
		reg _atCCR6 ( read-write )  \ capture/compare register 6
		reg _atAF1 ( read-write )  \ TIM1 alternate function option register  1
		reg _atAF2 ( read-write )  \ TIM1 Alternate function odfsdm1_breakster  2
		reg _atTISEL ( read-write )  \ TIM1 timer input selection
	end-registers

$40001000 constant TIM6 ( Basic timers )
$40001400 constant TIM7 ( Basic timers )
	registers
		reg _btCR1 ( read-write )  \ control register 1
		reg _btCR2 ( read-write )  \ control register 2
		drop $C
		reg _btDIER ( read-write )  \ DMA/Interrupt enable register
		reg _btSR ( read-write )  \ status register
		reg _btEGR ( write-only )  \ event generation register
		drop $24
		reg _btCNT ( read-write )  \ counter
		reg _btPSC ( read-write )  \ prescaler
		reg _btARR ( read-write )  \ auto-reload register
	end-registers

$40022000 constant ADC1 ( Analog to Digital Converter )
$40022100 constant ADC2 ( Analog to Digital Converter )
$58026000 constant ADC3 ( Analog to Digital Converter )
	registers
	    reg _aISR 	  \ ADC interrupt and status  register
	    reg _aIER 	  \ ADC interrupt enable register
	    reg _aCR 	  \ ADC control register
	    reg _aCFGR 	  \ ADC configuration register 1
	    reg _aCFGR2 	  \ ADC configuration register 2
	    reg _aSMPR1 	  \ ADC sampling time register 1
	    reg _aSMPR2 	  \ ADC sampling time register 2
	    drop $20
	    reg _aLTR1 	  \ ADC analog watchdog 1 threshold  register
	    reg _aLHTR1 	  \ ADC analog watchdog 2 threshold  register
	    drop $30
	    reg _aSQR1 	  \ ADC group regular sequencer ranks register  1
	    reg _aSQR2 	  \ ADC group regular sequencer ranks register  2
	    reg _aSQR3 	  \ ADC group regular sequencer ranks register  3
	    reg _aSQR4 	  \ ADC group regular sequencer ranks register  4
	    reg _aDR 	  \ ADC group regular conversion data  register
	    drop $4c
	    reg _aJSQR 	  \ ADC group injected sequencer  register
	    drop $60
	    reg _aOFR1 	  \ ADC offset number 1 register
	    reg _aOFR2 	  \ ADC offset number 2 register
	    reg _aOFR3 	  \ ADC offset number 3 register
	    reg _aOFR4 	  \ ADC offset number 4 register
	    drop $80
	    reg _aJDR1 	  \ ADC group injected sequencer rank 1  register
	    reg _aJDR2 	  \ ADC group injected sequencer rank 2  register
	    reg _aJDR3 	  \ ADC group injected sequencer rank 3  register
	    reg _aJDR4 	  \ ADC group injected sequencer rank 4  register
	    drop $a0
	    reg _aAWD2CR 	  \ ADC analog watchdog 2 configuration  register
	    reg _aAWD3CR 	  \ ADC analog watchdog 3 configuration  register
	    drop $c0
	    reg _aDIFSEL 	  \ ADC channel differential or single-ended  mode selection register
	    reg _aCALFACT 	  \ ADC calibration factors  register
	    drop $1c
	    reg _aPCSEL 	  \ ADC pre channel selection  register
	    drop $b0
	    reg _aLTR2 	  \ ADC watchdog lower threshold register  2
	    reg _aHTR2 	  \ ADC watchdog higher threshold register  2
	    reg _aLTR3 	  \ ADC watchdog lower threshold register  3
	    reg _aHTR3 	  \ ADC watchdog higher threshold register  3
	    drop $c8
	    reg _aCALFACT2 	  \ ADC Calibration Factor register  2
	end-registers


: enable-port ( portx -- ) PORTA - $400 / bit RCC _rAHB4ENR bis! ;

\ Port Mode Register - 00=Input  01=Output  10=Alternate  11=Analog
%00 constant MODE_Input  
%01 constant MODE_Output  
%10 constant MODE_Alternate  
%11 constant MODE_Analog
: set-moder ( mode pin# baseAddr -- )
	>R 2* %11 over lshift r@ _pMODER bic! 	\ clear ..
	lshift R> _pMODER bis!					\ .. set
;

\ Port Output Speed Register - 00=2 MHz  01=25 MHz  10=50 MHz  11=100 MHz
%00 constant SPEED_LOW 
%01 constant SPEED_MEDIUM
%10 constant SPEED_HIGH
%11 constant SPEED_VERYHIGH
: set-opspeed ( speed pin# baseAddr -- )
	>R 2* %11 over lshift r@ _pOSPEEDR bic!	\ clear ..
	lshift R> _pOSPEEDR bis!				\ .. set
;

\ Port pull up/down register - 00=none  01=Pullup  10=Pulldown
: set-pupd ( mode pin port -- )
    >R 2* %11 over lshift r@ _pPUPDR bic! \ clear ..
    lshift R> _pPUPDR bis!                \ .. set
;

\ Port alternate function
: set-alternate ( af# pin# baseAddr -- )
	>R dup 8 < if 
		4 * lshift R> _pAFRL
	else 
		8 - 4 * lshift R> _pAFRH 
	then
	bis!
;

\ Enable IRQn
: NVIC_EnableIRQ ( n -- )
    32 /mod 4 * NVIC + swap 1 swap lshift swap bis!
;

\ Disable IRQn
: NVIC_DisableIRQ ( n -- )
    32 /mod 4 * NVIC + $80 + swap 1 swap lshift swap bis!
;

\ Set priority for IRQn 0..80, priority 0..255	
: NVIC_SetPriority ( prio n -- )
	4 /mod cells $300 + NVIC + 	\ register offset
	-rot 8 * lshift				\ byte offset, register value
	swap bis! ;
	
\ \ set external interrupt configuration, port 0..7 (A..H), pin 0..15
\ : SYSCFG_SetEXTI ( port pin -- )
\ 	0 bit RCC _rAPB2ENR bis!	\ enable syscfg clock
\ 	4 /mod cells SYSCFG + 8 +	\ register offset
\ 	-rot 4 * lshift				\ 4-bit offset, register value
\ 	swap bis! ;

\ calculate baudrate
: baud ( n -- reg-val )
	hclk @ over 16 * /mod swap
	rot 10 /  / 10 /mod swap				\ round
	5 >= if 1+ then							\ ...
	dup 16 < if
		swap 4 lshift or
	else
		drop 1+ 4 lshift
	then
;	


	
