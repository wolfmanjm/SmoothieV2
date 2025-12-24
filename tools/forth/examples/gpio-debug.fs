#require lib_registers.fs
#require bin-utils.fs

porta variable gpio-sel
: gpio gpio-sel @ ;

: WRITEONLY ( -- ) ." write-only" cr ;

: gpio? gpio porta - 1024 / $41 + ;

: GPIO_MODER. cr ." GPIO" gpio? emit ." _MODER.  RW   $" GPIO _pMODER @ dup hex.  bin1. ;
: GPIO_OTYPER. cr ." GPIO" gpio? emit ." _OTYPER.  RW   $" GPIO _pOTYPER @ dup hex.  bin1. ;
: GPIO_OSPEEDR. cr ." GPIO" gpio? emit ." _OSPEEDR.  RW   $" GPIO _pOSPEEDR @ dup hex.  bin1. ;
: GPIO_PUPDR. cr ." GPIO" gpio? emit ." _PUPDR.  RW   $" GPIO _pPUPDR @ dup hex.  bin1. ;
: GPIO_IDR. cr ." GPIO" gpio? emit ." _IDR.  RO   $" GPIO _pIDR @ dup hex.  bin1. ;
: GPIO_ODR. cr ." GPIO" gpio? emit ." _ODR.  RW   $" GPIO _pODR @ dup hex.  bin1. ;
: GPIO_BSRR. cr ." GPIO" gpio? emit ." _BSRR " WRITEONLY ;
: GPIO_LCKR. cr ." GPIO" gpio? emit ." _LCKR.  RW   $" GPIO _pLCKR @ dup hex.  bin1. ;
: GPIO_AFRL. cr ." GPIO" gpio? emit ." _AFRL.  RW   $" GPIO _pAFRL @ dup hex.  bin1. ;
: GPIO_AFRH. cr ." GPIO" gpio? emit ." _AFRH.  RW   $" GPIO _pAFRH @ dup hex.  bin1. ;

: set-GPIO-port ( port -- )
	gpio-sel !
;

: GPIO. ( -- )
	GPIO_MODER.
	GPIO_OTYPER.
	GPIO_OSPEEDR.
	GPIO_PUPDR.
	GPIO_IDR.
	GPIO_ODR.
	GPIO_BSRR.
	GPIO_LCKR.
	GPIO_AFRL.
	GPIO_AFRH.
;
