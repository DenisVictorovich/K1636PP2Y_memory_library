/// `K1636PP2Y.c`

#ifndef  _K1636PP2Y_C_
#define  _K1636PP2Y_C_

/// РУССКАЯ ФЛЕШЬ-ПАМЯТЬ «К1636РР2У»
/// Эпиграф: «Умом Россию не понять, аршином общим не измерить: у ней особенная стать – в Россию можно только верить» [Фёдор Иванович Тютчев (1803)]
/// Платформа: ARM Cortex M3 «STM32F105» 72 MГц

void fl_mem_init()
{
    io_out(fl_mem_STROBE);
    io_out(fl_mem_MTCK);
    io_out(fl_mem_MTDI);

    io_out(fl_mem_MRST);
    io_out(fl_mem_HV);
    io_out(fl_mem_nOE);
    io_out(fl_mem_nWE);

    io_clr(fl_mem_STROBE);
    io_clr(fl_mem_MTCK);
    io_clr(fl_mem_MTDI);

    io_set(fl_mem_MRST);
    io_clr(fl_mem_HV);
    io_set(fl_mem_nOE);
    io_set(fl_mem_nWE);

    io_clr(fl_mem_MRST); delay_timer_delay_ms(5/* мс */);
    io_set(fl_mem_MRST); delay_timer_delay_ms(5/* мс */);
}

/* NDV в соответствии с `stm32f10x.h` */

#define  APB2_GPIO_BASE  (0x40000000 | 0x00010000)

#define  memory_address_GPIOA  (APB2_GPIO_BASE | 0x0800)
#define  memory_address_GPIOB  (APB2_GPIO_BASE | 0x0C00)
#define  memory_address_GPIOC  (APB2_GPIO_BASE | 0x1000)
#define  memory_address_GPIOD  (APB2_GPIO_BASE | 0x1400)
#define  memory_address_GPIOE  (APB2_GPIO_BASE | 0x1800)
#define  memory_address_GPIOF  (APB2_GPIO_BASE | 0x1C00)
#define  memory_address_GPIOG  (APB2_GPIO_BASE | 0x2000)

#ifndef  stringify
    #define  pro_stringify(a)  #a
    #define  stringify(a)      pro_stringify(a)
#endif

#define  io_memory_address_GPIO(_p,_b)  (memory_address_GPIO##_p)

#define  io_GPIO_n(port_bit)           io_memory_address_GPIO(port_bit)
#define  io_bit_n(port_bit)            io_bit(port_bit)
#define  io_nibble_position(port_bit)  "((" stringify(io_bit(port_bit)&7) ") << 2)"
#define  io_GPIO_CR_offset(port_bit)   ((io_bit(port_bit)&8)>>1)/* offset `CRL`0x00 / `CRH`0x04 [F3] */

    #define  fl_mem_MTCK_management_registers_prepare()                                                 \
        asm volatile ( "ldr r1, =" stringify(io_GPIO_n(fl_mem_MTCK) | 0x10/* offset `BSRR`[F3] */) "\n" \
                       "ldr r2, =1 << " stringify(io_bit_n(fl_mem_MTCK)) " << 0x10\n" /* reset bit */   \
                       "ldr r3, =1 << " stringify(io_bit_n(fl_mem_MTCK)) " << 0x00\n" /* set bit   */ )
    #define  fl_mem_MTDI_management_registers_prepare()                                                 \
        asm volatile ( "ldr r5, =" stringify(io_GPIO_n(fl_mem_MTDI) | 0x10/* offset `BSRR`[F3] */) "\n" \
                       "ldr r6, =1 << " stringify(io_bit_n(fl_mem_MTDI)) " << 0x10\n" /* reset bit */   \
                       "ldr r7, =1 << " stringify(io_bit_n(fl_mem_MTDI)) " << 0x00\n" /* set bit   */   \
                       /* чтобы менять направление потока данных */                                     \
                       "ldr r8, =" stringify(io_GPIO_n(fl_mem_MTDI) | io_GPIO_CR_offset(fl_mem_MTDI)) "\n" )
    #define  fl_mem_bit_output_MTDI(b)                                                                  \
                       "ands r10, r0, #(" stringify(1 << b) ")\n"                                       \
                       "IT"/* `if then` */ "E"/* `else` */ " " "ne"/* `not equal` */ "\n"               \
                       "str" "ne"/* `not equal` 0 */ " r7, [r5]\n"/* `1` */                             \
                       "str" "eq"/* `equal`     0 */ " r6, [r5]\n"/* `0` */
    #define  fl_mem_bit_output(b)                                                                       \
                       fl_mem_bit_output_MTDI(b)                                                        \
                       "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"                                          \
                       "str" " r3, [r1]\n"/* `1` возрастающий фронт */                                  \
                       "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"                                          \
                       "str" " r2, [r1]\n"/* `0` спадающий фронт */
    #define  fl_mem_switch_to_input_MTDI() /* HI-Z input */                                             \
                       "ldr  r11, [r8]\n"                                                               \
                       "bic  r11, #(15 << " io_nibble_position(fl_mem_MTDI) ")\n"                       \
                       "orr  r11, #( 4 << " io_nibble_position(fl_mem_MTDI) ")\n"                       \
                       "str  r11, [r8]\n"
    #define  fl_mem_switch_to_output_MTDI() /* pp output */                                             \
                       "ldr  r11, [r8]\n"                                                               \
                       "bic  r11, #(15 << " io_nibble_position(fl_mem_MTDI) ")\n"                       \
                       "orr  r11, #( 3 << " io_nibble_position(fl_mem_MTDI) ")\n"                       \
                       "str  r11, [r8]\n"
    #define  fl_mem_core_registers_prepare()                                                                \
        WDT_FLAG = CONCODE;                                                                                 \
        fl_mem_MTCK_management_registers_prepare(); /* готовим средства для управления тактовым сигналом */ \
        fl_mem_MTDI_management_registers_prepare(); /* готовим средства для управления сигналом данных   */

/// «IO_ASM»
#define  io_P_prepare(port_bit)  "ldr r4, =" stringify(io_memory_address_GPIO(port_bit) | 0x10/* offset `BSRR`[F3] */) "\n"
#define  io_1_prepare(port_bit)  "mov r11, #(1 << " stringify(io_bit(port_bit)) " << 0x00)\n"
#define  io_0_prepare(port_bit)  "mov r12, #(1 << " stringify(io_bit(port_bit)) " << 0x10)\n"
//
#define  io_1()  "str r11, [r4]\n"
#define  io_0()  "str r12, [r4]\n"

#define  fl_mem_MTCK_1()  "str r3, [r1]\n"/* возрастающий фронт, не меняем состояние «флагов» суффиксом «s» */
#define  fl_mem_MTCK_0()  "str r2, [r1]\n"/* спадающий    фронт, не меняем состояние «флагов» суффиксом «s» */

void fl_mem_STROBE_op_code(volatile register u32 r0/* [29.01.2018] | op_code */)
{
    /* ~5 MHz */
    fl_mem_core_registers_prepare()
    asm volatile
    (
            fl_mem_switch_to_output_MTDI()
            /// СТРОБ
            io_P_prepare(fl_mem_STROBE)
            io_1_prepare(fl_mem_STROBE)
            io_0_prepare(fl_mem_STROBE)
            fl_mem_MTCK_1() "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            fl_mem_MTCK_0() io_1()  "nop\n" "nop\n" "nop\n" "nop\n"
            fl_mem_MTCK_1() "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            fl_mem_MTCK_0() io_0()  "nop\n" "nop\n" "nop\n" "nop\n"
            /// СТАРТОВЫЙ СИМВОЛ
            fl_mem_bit_output(1)
            fl_mem_bit_output(0)
    ) ;
}

void fl_mem_command(volatile register u32 r0/* [29.01.2018] */)
{
    /* ~5 MHz */
    fl_mem_core_registers_prepare()
    asm volatile
    (
            fl_mem_bit_output(0)
            fl_mem_bit_output(1)
            fl_mem_bit_output(2)
            fl_mem_bit_output(3)
            fl_mem_bit_output(4)
            fl_mem_bit_output(5)
            fl_mem_bit_output(6)
            fl_mem_bit_output(7)
            fl_mem_bit_output(8)
            fl_mem_bit_output(9)
            fl_mem_bit_output(10)
    ) ;
}

void fl_mem_Addr_12(volatile register u32 r0/* [29.01.2018] */)
{
    /* ~5 MHz */
    fl_mem_core_registers_prepare()
    asm volatile
    (
            fl_mem_bit_output(0)
            fl_mem_bit_output(1)
            fl_mem_bit_output(2)
            fl_mem_bit_output(3)
            fl_mem_bit_output(4)
            fl_mem_bit_output(5)
            fl_mem_bit_output(6)
            fl_mem_bit_output(7)
            fl_mem_bit_output(8)
            fl_mem_bit_output(9)
            fl_mem_bit_output(10)
            fl_mem_bit_output(11)
    ) ;
}

void fl_mem_Addr_17(volatile register u32 r0/* [29.01.2018] */)
{
    /* ~5 MHz */
    fl_mem_core_registers_prepare()
    asm volatile
    (
            fl_mem_bit_output(0)
            fl_mem_bit_output(1)
            fl_mem_bit_output(2)
            fl_mem_bit_output(3)
            fl_mem_bit_output(4)
            fl_mem_bit_output(5)
            fl_mem_bit_output(6)
            fl_mem_bit_output(7)
            fl_mem_bit_output(8)
            fl_mem_bit_output(9)
            fl_mem_bit_output(10)
            fl_mem_bit_output(11)
            fl_mem_bit_output(12)
            fl_mem_bit_output(13)
            fl_mem_bit_output(14)
            fl_mem_bit_output(15)
            fl_mem_bit_output(16)
    ) ;
}

u8 fl_mem_Addr_17_rd(volatile register u32 r0/* [29.01.2018] */)
{
    /* ~5 MHz */
    u8 ack = 0;
    fl_mem_core_registers_prepare()
    asm volatile
    (
            fl_mem_bit_output(0)
            fl_mem_bit_output(1)
            fl_mem_bit_output(2)
            fl_mem_bit_output(3)
            fl_mem_bit_output(4)
            fl_mem_bit_output(5)
            fl_mem_bit_output(6)
            fl_mem_bit_output(7)
            fl_mem_bit_output(8)
            fl_mem_bit_output(9)
            fl_mem_bit_output(10)
            fl_mem_bit_output(11)
            fl_mem_bit_output(12)
            fl_mem_bit_output(13)
            fl_mem_bit_output(14)
            fl_mem_bit_output(15)
            fl_mem_bit_output(16)
            fl_mem_switch_to_input_MTDI()
    ) ;
    io_set(fl_mem_MTCK); if(io_read(fl_mem_MTDI)) ack = 1; io_clr(fl_mem_MTCK);
    return ack;
}

#define  inp_P_prepare(port_bit)  "ldr r11, =" stringify(io_memory_address_GPIO(port_bit) | 0x08/* offset `IDR`[F3] */) "\n"
#define  inp_P_read(port_bit)     "ldr  r4, [r11]\n" /* далее выделяем нужный разряд: */ \
                                  "ands r4, #(1 << " stringify(io_bit(port_bit)) ")\n" /* далее можно прочесть состояние «флагов» ... */

#define  asm_enable_interrupt()   "CPSIE i \n" /* macro to enable  all interrupts */
#define  asm_disable_interrupt()  "CPSID i \n" /* macro to disable all interrupts */

/// ПОДОБРАНО ОСЦИЛЛОГРАФОМ [17.05.2017]
#define  fl_mem_5_MHz_dummy_cycle()     fl_mem_MTCK_1() "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" \
                                        fl_mem_MTCK_0() "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
#define  fl_mem_5_MHz_wait_ack_cycle()  fl_mem_MTCK_1()                                  \
                                        inp_P_read(fl_mem_MTDI)                          \
                                        fl_mem_MTCK_0() "nop\n"                          \
                                        "IT"/* `if then` */ " " "eq"/* `equal` 0 */ "\n" \
                                        "b" "eq" "  fl_mem_Data_5_MHz_ack_wait \n"

void fl_mem_Data_5_MHz(volatile register u32 r0/* [29.01.2018] */)
{
    fl_mem_core_registers_prepare()
    asm volatile
    (
            asm_disable_interrupt()
               fl_mem_bit_output(0)
               fl_mem_bit_output(1)
               fl_mem_bit_output(2)
               fl_mem_bit_output(3)
               fl_mem_bit_output(4)
               fl_mem_bit_output(5)
               fl_mem_bit_output(6)
               fl_mem_bit_output(7)
               fl_mem_switch_to_input_MTDI()
               /* КРИТИЧЕСКАЯ ЗОНА ! ТАКТЫ 5 МГц { период [0,14 .. 0,24] µс } */
               /* 1 */ fl_mem_5_MHz_dummy_cycle()
               /* 2 */ fl_mem_5_MHz_dummy_cycle()
               /* 3 */ fl_mem_5_MHz_dummy_cycle()
               /* 4 */ fl_mem_5_MHz_dummy_cycle()
            asm_enable_interrupt()
    ) ;
}

void fl_mem_Data_5_MHz_ack(volatile register u32 r0/* [29.01.2018] */)
{
    fl_mem_core_registers_prepare()
    asm volatile
    (
            asm_disable_interrupt()
               fl_mem_bit_output(0)
               fl_mem_bit_output(1)
               fl_mem_bit_output(2)
               fl_mem_bit_output(3)
               fl_mem_bit_output(4)
               fl_mem_bit_output(5)
               fl_mem_bit_output(6)
               fl_mem_bit_output(7)
               fl_mem_switch_to_input_MTDI()
               inp_P_prepare(fl_mem_MTDI)
               /* КРИТИЧЕСКАЯ ЗОНА ! ТАКТЫ 5 МГц { период [0,14 .. 0,24] µс } */
               /* 1 */ fl_mem_5_MHz_dummy_cycle()
               /* 2 */ fl_mem_5_MHz_dummy_cycle()
               /* 3 */ fl_mem_5_MHz_dummy_cycle()
               /* 4 */ fl_mem_5_MHz_dummy_cycle()
               /* ждём подтверждение 4 такта ... */
            "fl_mem_Data_5_MHz_ack_wait: \n"
               /* 1 */ fl_mem_5_MHz_wait_ack_cycle()
               /* 2 */ fl_mem_5_MHz_wait_ack_cycle()
               /* 3 */ fl_mem_5_MHz_wait_ack_cycle()
               /* 4 */ fl_mem_5_MHz_wait_ack_cycle()
            asm_enable_interrupt()
    ) ;
}

/// ПОДОБРАНО ОСЦИЛЛОГРАФОМ [17.05.2017]
#define  fl_mem_2_MHz_dummy_cycle()     fl_mem_MTCK_1() "nop\n" "nop\n" "nop\n" "nop\n"  \
                                        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"  \
                                        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"          \
                                        fl_mem_MTCK_0() "nop\n" "nop\n" "nop\n" "nop\n"  \
                                        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"  \
                                        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
#define  fl_mem_2_MHz_wait_ack_cycle()  fl_mem_MTCK_1() "nop\n" "nop\n" "nop\n" "nop\n"  \
                                        "nop\n" "nop\n" "nop\n"                          \
                                        inp_P_read(fl_mem_MTDI)                          \
                                        fl_mem_MTCK_0() "nop\n" "nop\n" "nop\n" "nop\n"  \
                                        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"  \
                                        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"  \
                                        "IT"/* `if then` */ " " "eq"/* `equal` 0 */ "\n" \
                                        "b" "eq" "  fl_mem_Data_2_MHz_ack_wait \n"

void fl_mem_Data_2_MHz_ack(volatile register u32 r0/* [29.01.2018] */)
{
    fl_mem_core_registers_prepare()
    asm volatile
    (
            asm_disable_interrupt()
               fl_mem_bit_output(0)
               fl_mem_bit_output(1)
               fl_mem_bit_output(2)
               fl_mem_bit_output(3)
               fl_mem_bit_output(4)
               fl_mem_bit_output(5)
               fl_mem_bit_output(6)
               fl_mem_bit_output(7)
               fl_mem_switch_to_input_MTDI()
               inp_P_prepare(fl_mem_MTDI)
               /* КРИТИЧЕСКАЯ ЗОНА ! ТАКТЫ 2 МГц { период [0,38 .. 0,64] µс } */
               /* 1 */ fl_mem_2_MHz_dummy_cycle()
               /* 2 */ fl_mem_2_MHz_dummy_cycle()
               /* 3 */ fl_mem_2_MHz_dummy_cycle()
               /* 4 */ fl_mem_2_MHz_dummy_cycle()
               /* ждём подтверждение 4 такта ... */
            "fl_mem_Data_2_MHz_ack_wait: \n"
               /* 1 */ fl_mem_2_MHz_wait_ack_cycle()
               /* 2 */ fl_mem_2_MHz_wait_ack_cycle()
               /* 3 */ fl_mem_2_MHz_wait_ack_cycle()
               /* 4 */ fl_mem_2_MHz_wait_ack_cycle()
            asm_enable_interrupt()
    ) ;
}

/// ИСТОЧНИК: | http://forum.milandr.ru/viewtopic.php?f=5&t=75&start=60 | { [Ctrl] & «мышь» }
/// Предположим, мы хотим записать значение 0xA5 в блок 2 по адресу 1000 по последовательному интерфейсу.
/// Команда для памяти 16 Мбит (1636РР2) будет выглядеть так:
/// | Num3 | Num2 | Num1 | Num0 | CEB | NVRB | OEB | WEB | TMEN | BYTEB | VREAD |
/// |    1 |    0 |    0 |    0 |   0 |    1 |   1 |   0 |    0 |     0 |     0 |
/// 1й (командный, Addr 555, Data AA)
///    01 10000110000 010101010101 10101010
/// 2й (командный, Addr AAA, Data 55)
///    01 10000110000 101010101010 01010101
/// 3й (командный, Addr 555, Data A0)
///    01 10000110000 010101010101 10100000
/// 4й (полный, Addr 1000, Data OO)
///    00 10000110000 00010000000000000 OOOOOOOO

enum { fm_Num3 = 1024, fm_Num2 = 512, fm_Num1 = 256, fm_Num0 = 128, fm_CEB = 64,
       fm_NVRB = 32, fm_OEB = 16, fm_WEB = 8, fm_TMEN = 4, fm_BYTEB = 2, fm_VREAD = 1 } ;

void fl_mem_command_cycle(u32 c, u32 a, u8 d)
{
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_STROBE_op_code(0b01); asm volatile ("POP {r0-r12}\n");
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_command       (c);    asm volatile ("POP {r0-r12}\n");
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_Addr_12       (a);    asm volatile ("POP {r0-r12}\n");
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_Data_5_MHz    (d);    asm volatile ("POP {r0-r12}\n");
}

void fl_mem_full_cycle_5_MHz(u32 c, u32 a, u8 d)
{
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_STROBE_op_code(0b00); asm volatile ("POP {r0-r12}\n");
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_command       (c);    asm volatile ("POP {r0-r12}\n");
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_Addr_17       (a);    asm volatile ("POP {r0-r12}\n");
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_Data_5_MHz_ack(d);    asm volatile ("POP {r0-r12}\n");
}

void fl_mem_full_cycle_2_MHz(u32 c, u32 a, u8 d)
{
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_STROBE_op_code(0b00); asm volatile ("POP {r0-r12}\n");
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_command       (c);    asm volatile ("POP {r0-r12}\n");
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_Addr_17       (a);    asm volatile ("POP {r0-r12}\n");
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_Data_2_MHz_ack(d);    asm volatile ("POP {r0-r12}\n");
}

void fl_mem_write(u8 block/* 1 .. 14 */, u32 byte, u8 data)
{
    vu32 c = ( (vu32)block & 15 ) << 7 | fm_NVRB | fm_OEB ;
    /* цикл 1 */ fl_mem_command_cycle(c, 0x555, 0xAA);
    /* цикл 2 */ fl_mem_command_cycle(c, 0xAAA, 0x55);
    /* цикл 3 */ fl_mem_command_cycle(c, 0x555, 0xA0);
    /* цикл 4 */ fl_mem_full_cycle_5_MHz(c, byte, data);
}

u8 fl_mem_read(u8 block/* 1 .. 14 */, u32 byte)
{
    vu32 c = ( (vu32)block & 15 ) << 7 | fm_NVRB | fm_WEB ;
    asm volatile ("PUSH {r0-r12}\n"); fl_mem_STROBE_op_code(0b10); asm volatile ("POP {r0-r12}\n");
    fl_mem_command(c);
    if(fl_mem_Addr_17_rd(byte))
    {
        u32 i,t=0;
        for(i=0; i<8; i++)
        {
            io_set(fl_mem_MTCK); asm volatile ("nop\n" "nop\n");
                if(io_read(fl_mem_MTDI)) t |= (u32)1<<i;
            io_clr(fl_mem_MTCK); asm volatile ("nop\n" "nop\n");
        }
        return t;
    }
    return 255;
}

void fl_mem_ERASE_block(vu8 block)
{
    vu32 c = ( (vu32)block & 15 ) << 7 | fm_NVRB | fm_OEB ;
    /* цикл 1 */ fl_mem_command_cycle   (c, 0x555, 0xAA);
    /* цикл 2 */ fl_mem_command_cycle   (c, 0xAAA, 0x55);
    /* цикл 3 */ fl_mem_command_cycle   (c, 0x555, 0x80);
    /* цикл 4 */ fl_mem_command_cycle   (c, 0x555, 0xAA);
    /* цикл 5 */ fl_mem_command_cycle   (c, 0xAAA, 0x55);
    /* цикл 6 */ fl_mem_full_cycle_2_MHz(c, 0xAAA, 0x10);
}

void fl_mem_wr_data(u8 block/* 1 .. 14 */, u32 byte, void const* p, u32 length)
{
    u8* q = (u8*)p;
    while(length--) fl_mem_write(block, byte++, *q++);
}

void fl_mem_rd_data(u8 block/* 1 .. 14 */, u32 byte, void* p, u32 length)
{
    register u32 r0 asm("r0");
    u8* q = (u8*)p;
    while(length--)
    {
        asm volatile ("PUSH {r1-r12}\n");
            r0 = fl_mem_read(block, byte);
        asm volatile ("POP  {r1-r12}\n");
        *q++ = r0; byte++;
    }
}

#endif /* _K1636PP2Y_C_ */

