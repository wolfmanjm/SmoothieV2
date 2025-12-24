\ set the HSEM
$58024400 constant RCC_BASE
RCC_BASE $E0 + constant rcc_AHB4ENR
\ Bitfields for rcc_AHB4ENR
1 0 lshift constant b_rcc_AHB4ENR_GPIOAEN
1 1 lshift constant b_rcc_AHB4ENR_GPIOBEN
1 2 lshift constant b_rcc_AHB4ENR_GPIOCEN
1 3 lshift constant b_rcc_AHB4ENR_GPIODEN
1 4 lshift constant b_rcc_AHB4ENR_GPIOEEN
1 5 lshift constant b_rcc_AHB4ENR_GPIOFEN
1 6 lshift constant b_rcc_AHB4ENR_GPIOGEN
1 7 lshift constant b_rcc_AHB4ENR_GPIOHEN
1 8 lshift constant b_rcc_AHB4ENR_GPIOIEN
1 9 lshift constant b_rcc_AHB4ENR_GPIOJEN
1 10 lshift constant b_rcc_AHB4ENR_GPIOKEN
1 19 lshift constant b_rcc_AHB4ENR_CRCEN
1 21 lshift constant b_rcc_AHB4ENR_BDMAEN
1 24 lshift constant b_rcc_AHB4ENR_ADC3EN
1 25 lshift constant b_rcc_AHB4ENR_HSEMEN
1 28 lshift constant b_rcc_AHB4ENR_BKPRAMEN

$58026400 constant HSEM_BASE
HSEM_BASE $0 + constant hse_HSEM_R0
HSEM_BASE $80 + constant hse_HSEM_RLR0
\ HSEM_BASE $100 + constant hse_HSEM_IER1 \ for CM7
HSEM_BASE $110 + constant hse_HSEM_IER2   \ for CM4
HSEM_BASE $114 + constant hse_HSEM_ICR2   \ for CM4
HSEM_BASE $118 + constant hse_HSEM_ISR2   \ for CM4
HSEM_BASE $11C + constant hse_HSEM_MISR2  \ for CM4

1 31 lshift constant b_hse_HSEM_RLR0_LOCK
$000000FF 0 2constant m_hse_HSEM_RLR0_PROCID
$000000FF 8 2constant m_hse_HSEM_RLR0_MASTERID

\ CM4
1 8 lshift constant HSEM_CR_COREID_CURRENT

$E000ED00 constant SCB_BASE
SCB_BASE $0 + constant scb_CPUID

: get-cpuid
    \ if (((SCB->CPUID & 0x000000F0U) >> 4 )== 0x7U)
    scb_CPUID @ $00F0 and 4 rshift
;


: hsem0-quick-take
    \ SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_HSEMEN)
    b_rcc_AHB4ENR_HSEMEN rcc_AHB4ENR bis!
    \ take semaphore
    \ if (HSEM->RLR[SemID] == (HSEM_CR_COREID_CURRENT | HSEM_RLR_LOCK))
    hse_HSEM_RLR0 @ HSEM_CR_COREID_CURRENT b_hse_HSEM_RLR0_LOCK or <> if ." failed to take hsem" cr then

    \ release semaphore
    \ HSEM->R[SemID] = (ProcessID | HSEM_CR_COREID_CURRENT);
    HSEM_CR_COREID_CURRENT hse_HSEM_R0 !
;

: hsem2-irq-handler
    ." got hsem2 interrupt"
    \ /* Get the list of masked freed semaphores*/
    \ statusreg = HSEM->C2MISR;/*Use interrupt line 1 for CPU2 Master*/
    hse_HSEM_ISR2 @
    \ /*Disable Interrupts*/
    \ HSEM->C2IER &= ~((uint32_t)statusreg);
    dup hse_HSEM_IER2 bic!
    \ /*Clear Flags*/
    \ HSEM->C2ICR = ((uint32_t)statusreg);
    hse_HSEM_ICR2 bis!
;

: hsem-activate-notification ( n -- )
    \ HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
    \   HSEM_COMMON->IER |= SemMask;
    1 swap lshift hse_HSEM_IER2 bis!
;

$E000E100 constant NVIC
\ Enable IRQn
: NVIC_EnableIRQ ( n -- )
    32 /mod cells NVIC + swap 1 swap lshift swap bis!
;

\ Disable IRQn
: NVIC_DisableIRQ ( n -- )
    32 /mod cells NVIC + $80 + swap 1 swap lshift swap bis!
;

\ Set priority for IRQn, priority 0..255
: NVIC_SetPriority ( prio n -- )
    4 /mod cells $300 + NVIC +  \ register offset
    -rot 8 * lshift             \ byte offset, register value
    swap bis!
;

: init-hsem2-irq
    b_rcc_AHB4ENR_HSEMEN rcc_AHB4ENR bis!

    ['] hsem2-irq-handler irq-hsem2 !                         \ Hook for handler

    \ HAL_NVIC_SetPriority(HSEM2_IRQn, 10, 0);
    \ HAL_NVIC_EnableIRQ(HSEM2_IRQn);
    126 NVIC_EnableIRQ
    1 hsem-activate-notification
;

