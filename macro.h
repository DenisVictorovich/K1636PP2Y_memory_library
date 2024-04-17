
/* «macro.h» для STM ARM cortex M3 */
/* редакция от [02.11.2017] */

#ifndef  CM3_MACRO_H
#define  CM3_MACRO_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* чтобы «протащить» через несколько макросов несколько аргументов как один аргумент */
#define  SGL_ARG(...)  __VA_ARGS__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "digtyp.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* упрощённая работа с портами ввода/вывода */
#define  io_RCC_EN(_p,_b)  (RCC->APB2ENR |= RCC_APB2ENR_IOP##_p##EN)
#define  io_port(_p,_b)    (GPIO##_p)
#define  io_bit(_p,_b)     (_b)

/* «Cortex_M3_reference_manual.pdf» . страница 166
   c := CNF
      in input mode (MODE = 00)
         00: analog mode
         01: floating input (reset state)
         10: input with pull-up / pull-down
         11: reserved
      in output mode (MODE > 00)
         00: general purpose output push-pull
         01: general purpose output open-drain
         10: alternate function output push-pull
         11: alternate function output open-drain
   m := MODE
         00: input mode (reset state)
         01: output mode, max speed 10 MHz
         10: output mode, max speed 2 MHz
         11: output mode, max speed 50 MHz */

#define  io_config(port_bit, c, m)              \
   {                                            \
      vu8  b = io_bit(port_bit);                \
      vu32 f = c << 2 | m ;                     \
      io_RCC_EN(port_bit);                      \
      if(b < 8)                                 \
      {                                         \
          b *= 4;                               \
          io_port(port_bit)->CRL &= ~(15 << b); \
          io_port(port_bit)->CRL |=  ( f << b); \
      }                                         \
      else if(b < 16)                           \
      {                                         \
          b -= 8; b *= 4;                       \
          io_port(port_bit)->CRH &= ~(15 << b); \
          io_port(port_bit)->CRH |=  ( f << b); \
      }                                         \
   }

#define  io_out(port_bit)       io_config(SGL_ARG(port_bit), 0b00, 0b11)
#define  io_alt(port_bit)       io_config(SGL_ARG(port_bit), 0b10, 0b11)
#define  io_alt_od(port_bit)    io_config(SGL_ARG(port_bit), 0b11, 0b11)
#define  io_inp(port_bit)       io_config(SGL_ARG(port_bit), 0b01, 0b00)
#define  io_inp_pull(port_bit)  io_config(SGL_ARG(port_bit), 0b10, 0b00)

#define  io_set(port_bit)     io_port(port_bit)->BSRR = 1 << io_bit(port_bit) /* io_port(port_bit)->ODR |=  (1 << io_bit(port_bit)) */
#define  io_clr(port_bit)     io_port(port_bit)->BRR  = 1 << io_bit(port_bit) /* io_port(port_bit)->ODR &= ~(1 << io_bit(port_bit)) */
#define  io_check(port_bit)  (io_port(port_bit)->ODR  & 1 << io_bit(port_bit))
#define  io_read(port_bit)   (io_port(port_bit)->IDR  & 1 << io_bit(port_bit))
#define  io_byte_ODR(port_bit,byte)                                \
   {                                                               \
      io_port(port_bit)->ODR &= ~((vu32)255  << io_bit(port_bit)); \
      io_port(port_bit)->ODR |=  ((vu32)byte << io_bit(port_bit)); \
   }
#define  io_byte_read(port_bit)   (io_port(port_bit)->IDR & 255 << io_bit(port_bit))

#define  io_out_8_lo(port_bit)    io_port(port_bit)->CRL = 0x33333333/* `3==0011b` - push-pull output 50 MHz */;
#define  io_out_8_hi(port_bit)    io_port(port_bit)->CRH = 0x33333333/* `3==0011b` - push-pull output 50 MHz */;
#define  io_inp_8_lo(port_bit)    io_port(port_bit)->CRL = 0x44444444/* `4==0100b` - floating input */;
#define  io_inp_8_hi(port_bit)    io_port(port_bit)->CRH = 0x44444444/* `4==0100b` - floating input */;

// очень полезные макроопределения:
#define  cat(_x,_y)   _x ## _y
#define  xcat(_x,_y)  cat(_x,_y)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ТАЙМЕРЫ ОБЩЕГО НАЗНАЧЕНИЯ, СРЕДСТВА НАСТРОЙКИ :
#define  TIM(_tim_)                       xcat(TIM,_tim_)
#define  TIM_IRQn(_tim_)                  xcat(TIM,xcat(_tim_,_IRQn))
#define  TIM_IRQHandler(_tim_)            xcat(TIM,xcat(_tim_,_IRQHandler))
#define  RCC_APB_ENR_TIM_EN(_apb_,_tim_)  xcat(xcat(RCC->APB,_apb_),ENR) |= xcat(xcat(xcat(xcat(RCC_APB,_apb_),ENR_TIM),_tim_),EN)
#define  TIM_CCR(_channel_)               xcat(CCR,_channel_)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  offsetof
   #define  offsetof(st, field)  ( (size_t) ( & ( (st*) 0 ) -> field ) )
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// АСИНХРОННЫЕ / СИНХРОНИЗИРОВАННЫЕ ПОСЛЕДОВАТЕЛЬНЫЕ ПОРТЫ ВВОДА-ВЫВОДА :
/* define  baud-rate setting (BRR) for UART/USART */
   #define  USART_DIV(PCLK, BR)        ( ( PCLK * 25 ) / ( 4 * BR ) )
   #define  USART_DIV_MANT(PCLK, BR)   ( USART_DIV(PCLK, BR) / 100 )
   #define  USART_DIV_FRAQ(PCLK, BR)   ( ( ( USART_DIV(PCLK, BR) - ( USART_DIV_MANT(PCLK, BR) * 100 ) ) * 16 + 50 ) / 100 )
   #define  USART_BRR(PCLK, BR)        ( ( USART_DIV_MANT(PCLK, BR) << 4 ) | ( USART_DIV_FRAQ(PCLK, BR) & 15 ) )

/* define  bus clock & interrupt setting for UART/USART */
   #define  RCC_APB_ENR_USART_EN(_apb_,_or_,_n_)  xcat(xcat(RCC->APB,_apb_),ENR) |= \
                                                  xcat(xcat(xcat(xcat(xcat(RCC_APB,_apb_),ENR_),_or_),_n_),EN)
   #define  USART_n(_or_,_n_)                     xcat(_or_,_n_)
   #define  USART_n_IRQn(_or_,_n_)                xcat(_or_,xcat(_n_,_IRQn))
   #define  USART_n_IRQHandler(_or_,_n_)          xcat(_or_,xcat(_n_,_IRQHandler))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* CM3_MACRO_H */

