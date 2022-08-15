#ifndef __CRC_CHECK_H__
#define __CRC_CHECK_H__

#include "main.h"

/*
 * The CRC parameters. Currently configured for CCITT.
 * Simply modify these to switch to another CRC Standard.
 */
#define POLYNOMIAL          0x8005
#define INITIAL_REMAINDER   0xFFFF
#define FINAL_XOR_VALUE     0x0000

/*
 * The width of the CRC calculation and result.
 * Modify the typedef for an 8 or 32-bit CRC standard.
 */
// typedef unsigned short width_t;
// #define WIDTH (8 * sizeof(width_t))
typedef uint16_t    width_t;
#define WIDTH   16
#define TOPBIT  (1 << (WIDTH - 1))

/*
 * Initialize the CRC lookup table.
 * This table is used by crcCompute() to make CRC computation faster.
 */
void crcInit(void);

/*
 * Compute the CRC checksum of a binary message block.
 * @para message, 用来计算的数据
 * @para nBytes, 数据的长度
 * @note This function expects that crcInit() has been called
 *       first to initialize the CRC lookup table.
 */
width_t crcCompute(unsigned char* message, unsigned int nBytes);


uint16_t CRC16_2(uint8_t datain, uint16_t CRCin);
uint16_t CRC16N(uint8_t *s, uint16_t len);

#endif
