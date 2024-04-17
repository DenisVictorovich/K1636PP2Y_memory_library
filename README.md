РУССКАЯ ФЛЕШЬ-ПАМЯТЬ «К1636РР2У» |
Платформа: ARM Cortex M3 «STM32F105» 72 MГц |
Система сборки: gcc-arm-none-eabi-4_8-2014q2-20140609 |
В заголовочнике необходимо прописать все выводы для подключения, например: |
// 1) последовательный двунаправленный канал данных |
#define  fl_mem_STROBE   A,7 |
#define  fl_mem_MTCK     A,1 |
#define  fl_mem_MTDI     B,5 |
// 2) остальное ... |
#define  fl_mem_MRST     A,0 |
#define  fl_mem_HV       A,4 |
#define  fl_mem_nOE      A,5 |
#define  fl_mem_nWE      A,6 |
