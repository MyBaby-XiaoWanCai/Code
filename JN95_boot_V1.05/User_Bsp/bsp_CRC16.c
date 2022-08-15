/**********************************************************************************************************
*
*	模块名称 : 通用GPIO配置
*	文件名称 : bsp_CRC16.c
*	版    本 : V1.0
*	说    明 : 用于CRC16计算
*	作	 者：  wang liang
*	日	 期:  2021.7.1

**********************************************************************************************************/

#include "bsp_CRC16.h"
#include "main.h"

/*
 * An array containing the pre-computed intermediate result for each
 * possible byte of input. This is used to speed up the computation.
 */
static width_t crcTable[256];

/*
 * Initialize the CRC lookup table.
 * This table is used by crcCompute() to make CRC computation faster.
 */
void crcInit(void)
{
    width_t remainder;
    width_t dividend;
    int bit;
    /* Perform binary long division, a bit at a time. */
    for(dividend = 0; dividend < 256; dividend++)
    {
        /* Initialize the remainder.  */
        remainder = dividend << (WIDTH - 8);
        /* Shift and XOR with the polynomial.   */
        for(bit = 0; bit < 8; bit++)
        {
            /* Try to divide the current data bit.  */
            if(remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = remainder << 1;
            }
        }
        /* Save the result in the table. */
        crcTable[dividend] = remainder;
    }
} /* crcInit() */

/*
 * Compute the CRC checksum of a binary message block.
 * @para message, 用来计算的数据
 * @para nBytes, 数据的长度
 * @note This function expects that crcInit() has been called
 *       first to initialize the CRC lookup table.
 */
width_t crcCompute(unsigned char* message, unsigned int nBytes)
{
    unsigned int offset;
    unsigned char byte;
    width_t remainder = INITIAL_REMAINDER;
    /* Divide the message by the polynomial, a byte at a time. */
    for(offset = 0; offset < nBytes; offset++)
    {
        byte = (remainder >> (WIDTH - 8)) ^ message[offset];
        remainder = crcTable[byte] ^ (remainder << 8);
    }
    /* The final remainder is the CRC result. */
    return (remainder ^ FINAL_XOR_VALUE);
} /* crcCompute() */


/***************************************************************************************************
**  函 数 名: CRC16_2
**  功能说明:
**  形    参:
**  返 回 值:
***************************************************************************************************/
uint16_t CRC16_2(uint8_t datain, uint16_t CRCin)
{
    uint16_t i;
    uint16_t tempcrc;
    tempcrc = CRCin ^ datain;
    for(i = 0; i < 8; i++)
    {
        if (tempcrc & 0x01 )
        {
            tempcrc =(tempcrc >> 1) ^ 0xA001;
        }
        else
        {
            tempcrc = tempcrc >> 1;
        }
    }
    return tempcrc;
}
/***************************************************************************************************
**  函 数 名: CRC16N
**  功能说明:
**  形    参:
**  返 回 值:
***************************************************************************************************/
uint16_t CRC16N(uint8_t *s, uint16_t len)
{
    uint16_t i;
    uint16_t crc_temp = 0xFFFF;
    for(i = 0; i < len; i++)
    {
       crc_temp = CRC16_2(*s++,crc_temp);
    }
    return crc_temp;
}
