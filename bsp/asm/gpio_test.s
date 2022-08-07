.syntax unified
.cpu    cortex-m7
.thumb

.equ RCC_BASE_ADDR      ,0x58024400
.equ RCC_CR_ADDR        ,0x58024400
.equ RCC_AHB4ENR_ADDR   ,0x580244E0
.equ RCC_PLLCKSELR      ,0x58024428
.equ RCC_PLLCFGR        ,0x5802442C

.equ GPIOC_BASE_ADDR    ,0x58020800
.equ GPIOC_MODER_ADDR   ,0x58020800
.equ GPIOC_BSRR_ADDR    ,0x58020818

.equ GPIOI_BASE_ADDR    ,0x58022000
.equ GPIOI_MODER_ADDR   ,0x58022000
.equ GPIOI_BSRR_ADDR    ,0x58022018

.global led_gpio_init
.global led_red_off
.global led_red_on
.global led_blue_off
.global led_blue_on
.global delay_period

led_gpio_init:
    PUSH    {R0-R2, LR}
    // Enable GPIOC and GPIOI clock
    LDR     R0, =RCC_AHB4ENR_ADDR
    LDR     R1, [R0]
    ORR     R1, R1, #0x104
    STR     R1, [R0]
    // Delay after peripheral clock enabel
    LDR     R0, =RCC_AHB4ENR_ADDR
    LDR     R1, [R0]
    AND     R2, R1, #0x4
    AND     R2, R1, #0x100

    // Config GPIO mode register
    LDR     R0, =GPIOC_MODER_ADDR 
    LDR     R1, [R0]
    AND     R1, R1, #0x3FFFFFFF
    ORR     R1, R1, #0x40000000
    STR     R1, [R0]

    LDR     R0, =GPIOI_MODER_ADDR 
    LDR     R1, [R0]
    AND     R1, R1, #0xFFFCFFFF
    ORR     R1, R1, #0x10000
    STR     R1, [R0]

    POP     {R0-R2, PC}

/* LED on - set GPIO pin low level*/
led_red_on:
    PUSH    {R0, R1, LR}
    LDR     R0, =GPIOC_BSRR_ADDR
    LDR     R1, =0x80000000
    STR     R1, [R0]
    POP     {R0, R1, PC}
    
/* LED off - set GPIO pin high level*/
led_red_off:
    PUSH    {R0, R1, LR}
    LDR     R0, =GPIOC_BSRR_ADDR
    LDR     R1, =0x8000
    STR     R1, [R0]
    POP     {R0, R1, PC}

/* LED on - set GPIO pin low level*/
led_blue_on:
    PUSH    {R0, R1, LR}
    LDR     R0, =GPIOI_BSRR_ADDR
    LDR     R1, =0x1000000
    STR     R1, [R0]
    POP     {R0, R1, PC}
    
/* LED off - set GPIO pin high level*/
led_blue_off:
    PUSH    {R0, R1, LR}
    LDR     R0, =GPIOI_BSRR_ADDR
    LDR     R1, =0x100
    STR     R1, [R0]
    POP     {R0, R1, PC}

delay_period:
    PUSH    {R0, R1, LR}

    MOVS    R0, #1000
dloop1:
    CBZ     R0, dstop
    SUBS    R0, #1
    MOVS    R1, #0xFF00
dloop2:
    CMP     R1, #0
    BEQ     dloop1
    SUBS    R1, #1
    B       dloop2
dstop:
    POP     {R0, R1, PC}
