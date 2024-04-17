
/* `digtyp.h` */
/* редакци€ от [22.02.2018] */

#ifndef  DIG_TYPES_H
#define  DIG_TYPES_H

typedef  signed char         i8;
typedef  signed short        i16;
typedef  signed long         i32;
typedef  signed long long    i64;

typedef  unsigned char       ui8;
typedef  unsigned short      ui16;
typedef  unsigned long       ui32;
typedef  unsigned long long  ui64;

typedef  i8   s8;
typedef  i16  s16;
typedef  i32  s32;
typedef  i64  s64;

typedef  ui8   u8;
typedef  ui16  u16;
typedef  ui32  u32;
typedef  ui64  u64;

typedef  volatile s8   vs8;
typedef  volatile s16  vs16;
typedef  volatile s32  vs32;
typedef  volatile s64  vs64;

typedef  volatile u8   vu8;
typedef  volatile u16  vu16;
typedef  volatile u32  vu32;
typedef  volatile u64  vu64;

typedef  const u8      uc8;
typedef  const u16     uc16;
typedef  const u32     uc32;
typedef  const u64     uc64;

// дл€ работы со временем
typedef  ui32  sec2000 ; /* секунды от начала 2000 года */
typedef  ui32  ulong ;   /* unsigned long */

/// #pragma pack( push, 1 ) // ѕ–ј¬»Ћ№Ќјя упаковка структур
///    struct DTime { ui8 sec,min,hour,date,mon,year; } ;
/// #pragma pack( pop )

#define  constant    const
#define  __flash     const
#define  __farflash  const

typedef  ui16  file_ptr;

typedef  ui32  base_type/* [20.04.2015] <<ARM-cortex>> architecture */;

#endif /* DIG_TYPES_H */

