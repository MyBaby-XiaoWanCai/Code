#include "ringbuffer.h"
#include "main.h"


static uint16_t validlen=0;  				//已使用的数据长度
static uint8_t* pHead=NULL; 				//环形缓冲区首地址
static uint8_t* pTail=NULL; 				//环形缓冲区尾地址
static uint8_t* pValid=NULL; 				//已使用缓冲区的首地址
static uint8_t* pValidTail=NULL; 		    //已使用缓冲区的尾地址


/**********************************************************************************************************
**  函 数 名 : InitRingBuffer()
**  功能说明 : 初始化指定的环形缓冲区
**  形    参 : @buffer定义静态缓冲区数组的首地址 
               @size:该缓冲区的大小 
**  返 回 值 : 无
**********************************************************************************************************/

void InitRingBuffer(uint8_t* buffer,uint16_t size)
{
	if(pHead==NULL)
	{
		pHead= buffer;
	}
	pValid = pValidTail = pHead;
	pTail = pHead + size;
	validlen = 0;
}

/**********************************************************************************************************
**  函 数 名 : WriteRingBuffer()
**  功能说明 : 功能:向缓冲区写入数据
**  形    参 : @buffer写入的数据指针
               @size:写入的数据长度	 
**  返 回 值 : return:-1 写入的数据长度过大 -2:缓冲区未初始化  0 正常写入
**********************************************************************************************************/

int WriteRingBuffer(uint8_t* buffer,uint16_t length)
{
	uint16_t len1,len2,movelen;
	if(length>RINGBUFSIZE)
	{
		return -2;
	}
	if(pHead==NULL)
	{
		return -1;
	}

	
	//将要写入的数据copy到pVaildTaill处
	if(pValidTail+length > pTail)
	{
		len1 = pTail - pValidTail;
		len2 = length - len1;
		memcpy(pValidTail,buffer,len1);
		memcpy(pHead,buffer+len1,len2);
		pValidTail = pHead + len2;
	}else
	{
		memcpy(pValidTail,buffer,length);
		pValidTail += length;
	}

	//重新计算已使用区的起始位置
	if(validlen+length >RINGBUFSIZE)
	{
		movelen=validlen+length-RINGBUFSIZE;
		if(pValid+movelen > pTail)
		{
			len1=pTail-pValid;
			len2=movelen-len1;
			pValid=pHead+len2;
		}else
		{
			pValid +=movelen;
		}
		validlen=RINGBUFSIZE;
	}else
	{
		validlen +=length;
	}
	return 0;
}


/**********************************************************************************************************
**  函 数 名 : ReadRingBuffer()
**  功能说明 : 功能:从缓冲区读取指定长度的数据	
**  形    参 : @buffer 接收读取数据的指针
               @length 读取的数据长度 
**  返 回 值 : return:-1 缓冲区未初始化 >0:读取的有效数据大小
**********************************************************************************************************/

int ReadRingBuffer(uint8_t* buffer,uint16_t length)
{
	uint16_t len1,len2;
	
	if(pHead==NULL)
	{
		return -1;
	}

	if(length > validlen)
	{
		length = validlen;
	}
	if(validlen==0)
	{
		return 0;
	}
	
	if(pValid+length>pTail)
	{
		len1=pTail-pValid;
		len2=length-len1;
		memcpy(buffer,pValid,len1);
		memcpy(buffer+len1,pHead,len2);
		pValid = pHead+len2;
	}else
	{
		memcpy(buffer,pValid,length);
		pValid +=length;
	}
	validlen -=length;
	return length;
}


/**********************************************************************************************************
**  函 数 名 : WriteRingBuffer()
**  功能说明 : 获取已使用的缓冲区数据长度
**  形    参 : void 
**  返 回 值 : 已使用缓冲区的数据长度 
**********************************************************************************************************/
uint16_t GetRingBufferValidLen(void)
{
	return validlen;
}
