#include "ringbuffer.h"
#include "main.h"


static uint16_t validlen=0;  				//��ʹ�õ����ݳ���
static uint8_t* pHead=NULL; 				//���λ������׵�ַ
static uint8_t* pTail=NULL; 				//���λ�����β��ַ
static uint8_t* pValid=NULL; 				//��ʹ�û��������׵�ַ
static uint8_t* pValidTail=NULL; 		    //��ʹ�û�������β��ַ


/**********************************************************************************************************
**  �� �� �� : InitRingBuffer()
**  ����˵�� : ��ʼ��ָ���Ļ��λ�����
**  ��    �� : @buffer���徲̬������������׵�ַ 
               @size:�û������Ĵ�С 
**  �� �� ֵ : ��
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
**  �� �� �� : WriteRingBuffer()
**  ����˵�� : ����:�򻺳���д������
**  ��    �� : @bufferд�������ָ��
               @size:д������ݳ���	 
**  �� �� ֵ : return:-1 д������ݳ��ȹ��� -2:������δ��ʼ��  0 ����д��
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

	
	//��Ҫд�������copy��pVaildTaill��
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

	//���¼�����ʹ��������ʼλ��
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
**  �� �� �� : ReadRingBuffer()
**  ����˵�� : ����:�ӻ�������ȡָ�����ȵ�����	
**  ��    �� : @buffer ���ն�ȡ���ݵ�ָ��
               @length ��ȡ�����ݳ��� 
**  �� �� ֵ : return:-1 ������δ��ʼ�� >0:��ȡ����Ч���ݴ�С
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
**  �� �� �� : WriteRingBuffer()
**  ����˵�� : ��ȡ��ʹ�õĻ��������ݳ���
**  ��    �� : void 
**  �� �� ֵ : ��ʹ�û����������ݳ��� 
**********************************************************************************************************/
uint16_t GetRingBufferValidLen(void)
{
	return validlen;
}
