/**
 * Simplest Librtmp Send 264
 *
 * �����裬����
 * leixiaohua1020@126.com
 * zhanghuicuc@gmail.com
 * �й���ý��ѧ/���ֵ��Ӽ���
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * ���������ڽ��ڴ��е�H.264����������RTMP��ý���������
 *
 */
#pragma pack(4)
#include <stdio.h>
#include <stdlib.h>
#include "librtmp_send264.h"
#include "librtmp/rtmp.h"   
#include "librtmp/rtmp_sys.h"   
#include "librtmp/amf.h"  
#include "sps_decode.h"
#include <time.h>

#ifdef WIN32     
#include <windows.h>  
#pragma comment(lib,"WS2_32.lib")   
#pragma comment(lib,"winmm.lib")  
#endif 

//�����ͷ���ȣ�RTMP_MAX_HEADER_SIZE=18
#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)
//�洢Nal��Ԫ���ݵ�buffer��С
#define BUFFER_SIZE 200*1024
//��ѰNal��Ԫʱ��һЩ��־
#define GOT_A_NAL_CROSS_BUFFER BUFFER_SIZE+1
#define GOT_A_NAL_INCLUDE_A_BUFFER BUFFER_SIZE+2
#define NO_MORE_BUFFER_TO_READ BUFFER_SIZE+3

/**
 * _NaluUnit
 * �ڲ��ṹ�塣�ýṹ����Ҫ���ڴ洢�ʹ���Nal��Ԫ�����͡���С������
 */ 

/**
 * _RTMPMetadata
 * �ڲ��ṹ�塣�ýṹ����Ҫ���ڴ洢�ʹ���Ԫ������Ϣ
 */ 
typedef struct _RTMPMetadata  
{  
	// video, must be h264 type   
	unsigned int    nWidth;  
	unsigned int    nHeight;  
	unsigned int    nFrameRate;      
	unsigned int    nSpsLen;  
	unsigned char   *Sps;  
	unsigned int    nPpsLen;  
	unsigned char   *Pps;   
} RTMPMetadata,*LPRTMPMetadata;  

enum  
{  
	 VIDEO_CODECID_H264 = 7,  
};  

/**
 * ��ʼ��winsock
 *					
 * @�ɹ��򷵻�1 , ʧ���򷵻���Ӧ�������
 */ 
int InitSockets()    
{    
	#ifdef WIN32     
		WORD version;    
		WSADATA wsaData;    
		version = MAKEWORD(1, 1);    
		return (WSAStartup(version, &wsaData) == 0);    
	#else     
		return TRUE;    
	#endif     
}
unsigned long GetTickCount()  
{  
	    struct timespec ts;  
		  
		    clock_gettime(CLOCK_MONOTONIC, &ts);  
			  
			    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);  
}
/**
 * �ͷ�winsock
 *					
 * @�ɹ��򷵻�0 , ʧ���򷵻���Ӧ�������
 */ 
inline void CleanupSockets()    
{    
	#ifdef WIN32     
		WSACleanup();    
	#endif     
}    

//�����ֽ���ת��
char * put_byte( char *output, uint8_t nVal )    
{    
	output[0] = nVal;    
	return output+1;    
}   

char * put_be16(char *output, uint16_t nVal )    
{    
	output[1] = nVal & 0xff;    
	output[0] = nVal >> 8;    
	return output+2;    
}  

char * put_be24(char *output,uint32_t nVal )    
{    
	output[2] = nVal & 0xff;    
	output[1] = nVal >> 8;    
	output[0] = nVal >> 16;    
	return output+3;    
}    
char * put_be32(char *output, uint32_t nVal )    
{    
	output[3] = nVal & 0xff;    
	output[2] = nVal >> 8;    
	output[1] = nVal >> 16;    
	output[0] = nVal >> 24;    
	return output+4;    
}    
char *  put_be64( char *output, uint64_t nVal )    
{    
	output=put_be32( output, nVal >> 32 );    
	output=put_be32( output, nVal );    
	return output;    
}  

char * put_amf_string( char *c, const char *str )    
{    
	uint16_t len = strlen( str );    
	c=put_be16( c, len );    
	memcpy(c,str,len);    
	return c+len;    
}    
char * put_amf_double( char *c, double d )    
{    
	*c++ = AMF_NUMBER;  /* type: Number */    
	{    
		unsigned char *ci, *co;    
		ci = (unsigned char *)&d;    
		co = (unsigned char *)c;    
		co[0] = ci[7];    
		co[1] = ci[6];    
		co[2] = ci[5];    
		co[3] = ci[4];    
		co[4] = ci[3];    
		co[5] = ci[2];    
		co[6] = ci[1];    
		co[7] = ci[0];    
	}    
	return c+8;    
}  


unsigned int  m_nFileBufSize; 
unsigned int  nalhead_pos;
RTMP* m_pRtmp;  
RTMPMetadata metaData;
unsigned char *m_pFileBuf;  
unsigned char *m_pFileBuf_tmp;
unsigned char* m_pFileBuf_tmp_old;	//used for realloc

/**
 * ��ʼ�������ӵ�������
 *
 * @param url �������϶�Ӧwebapp�ĵ�ַ
 *					
 * @�ɹ��򷵻�1 , ʧ���򷵻�0
 */ 

FILE * pFile = NULL;
int RTMP264_Connect(const char* url)  
{  

	/*
	 if((pFile = fopen ("/mnt/custom/myfile", "wb"))==NULL)
	{
                     printf("cant open the file");
                          exit(0);
                }
				*/

	nalhead_pos=0;
	m_nFileBufSize=BUFFER_SIZE;
	m_pFileBuf=(unsigned char*)malloc(BUFFER_SIZE);
	m_pFileBuf_tmp=(unsigned char*)malloc(BUFFER_SIZE);
	InitSockets();  
	memset(&metaData,0,sizeof(RTMPMetadata));
	memset(m_pFileBuf,0,BUFFER_SIZE);

	metaData.Sps=(unsigned char*)malloc(2000);
	metaData.Pps=(unsigned char*)malloc(2000);
	

	m_pRtmp = RTMP_Alloc();
	RTMP_Init(m_pRtmp);
	/*����URL*/
	if (RTMP_SetupURL(m_pRtmp,(char*)url) == FALSE)
	{
		RTMP_Free(m_pRtmp);
		return false;
	}
	/*���ÿ�д,��������,�����������������ǰʹ��,������Ч*/
	RTMP_EnableWrite(m_pRtmp);
	/*���ӷ�����*/
	// 重新连接服务器并不需要做之前那么多事情，重连就好了
	if (RTMP_Connect(m_pRtmp, NULL) == FALSE) 
	{
		RTMP_Free(m_pRtmp);
		return false;
	} 

	/*������*/
	if (RTMP_ConnectStream(m_pRtmp,0) == FALSE)
	{
		RTMP_Close(m_pRtmp);
		RTMP_Free(m_pRtmp);
		return false;
	}
	return true;  
}  


/**
 * �Ͽ����ӣ��ͷ���ص���Դ��
 *
 */    
void RTMP264_Close()  
{  
	if(m_pRtmp)  
	{  
		RTMP_Close(m_pRtmp);  
		RTMP_Free(m_pRtmp);  
		m_pRtmp = NULL;  
	}  
	CleanupSockets();   
	if (m_pFileBuf != NULL)
	{  
		free(m_pFileBuf);
	}  
	if (m_pFileBuf_tmp != NULL)
	{  
		free(m_pFileBuf_tmp);
	}
	free(metaData.Sps);
	free(metaData.Pps);
	
} 

/**
 * 发送RTMP数据包
 *
 * @param nPacketType 数据类型
 * @param data 存储数据内容
 * @param size 数据大小
 * @param nTimestamp 当前包的时间戳
 *
 * @成功则返回 1 , 失败则返回一个小于0的数
 */
int SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp)  
{  
	RTMPPacket* packet;
	/*分配包内存和初始化,len为包体长度*/
	packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+size);
	memset(packet,0,RTMP_HEAD_SIZE);
	/*包体内存*/
	packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
	packet->m_nBodySize = size;
	memcpy(packet->m_body,data,size);
	packet->m_hasAbsTimestamp = 0;
	packet->m_packetType = nPacketType; /*此处为类型有两种一种是音频,一种是视频*/
	packet->m_nInfoField2 = m_pRtmp->m_stream_id;
	packet->m_nChannel = 0x04;

	packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
	if (RTMP_PACKET_TYPE_AUDIO ==nPacketType && size !=4)
	{
		packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	}
	packet->m_nTimeStamp = nTimestamp;
	/*发送*/
	int nRet =0;
	if (RTMP_IsConnected(m_pRtmp))
	{
		nRet = RTMP_SendPacket(m_pRtmp,packet,FALSE); /*TRUE为放进发送队列,FALSE是不放进发送队列,直接发送*/
	}
	/*释放内存*/
	free(packet);
	return nRet;  
}  

/**
 * ������Ƶ��sps��pps��Ϣ
 *
 * @param pps �洢��Ƶ��pps��Ϣ
 * @param pps_len ��Ƶ��pps��Ϣ����
 * @param sps �洢��Ƶ��pps��Ϣ
 * @param sps_len ��Ƶ��sps��Ϣ����
 *
 * @�ɹ��򷵻� 1 , ʧ���򷵻�0
 */
int SendVideoSpsPps(unsigned char *pps,int pps_len,unsigned char * sps,int sps_len)
{
	RTMPPacket * packet=NULL;//rtmp���ṹ
	unsigned char * body=NULL;
	int i;
	packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+1024);
	//RTMPPacket_Reset(packet);//����packet״̬
	memset(packet,0,RTMP_HEAD_SIZE+1024);
	packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
	body = (unsigned char *)packet->m_body;
	i = 0;
	body[i++] = 0x17;
	body[i++] = 0x00;

	body[i++] = 0x00;
	body[i++] = 0x00;
	body[i++] = 0x00;

	/*AVCDecoderConfigurationRecord*/
	body[i++] = 0x01;
	body[i++] = sps[1];
	body[i++] = sps[2];
	body[i++] = sps[3];
	body[i++] = 0xff;

	/*sps*/
	body[i++]   = 0xe1;
	body[i++] = (sps_len >> 8) & 0xff;
	body[i++] = sps_len & 0xff;
	memcpy(&body[i],sps,sps_len);
	i +=  sps_len;

	/*pps*/
	body[i++]   = 0x01;
	body[i++] = (pps_len >> 8) & 0xff;
	body[i++] = (pps_len) & 0xff;
	memcpy(&body[i],pps,pps_len);
	i +=  pps_len;

	packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	packet->m_nBodySize = i;
	packet->m_nChannel = 0x04;
	packet->m_nTimeStamp = 0;
	packet->m_hasAbsTimestamp = 0;
	packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet->m_nInfoField2 = m_pRtmp->m_stream_id;

	/*���÷��ͽӿ�*/
	int nRet = RTMP_SendPacket(m_pRtmp,packet,TRUE);
	free(packet);    //�ͷ��ڴ�
	return nRet;
}

/**
 * ����H264����֡
 *
 * @param data �洢����֡����
 * @param size ����֡�Ĵ�С
 * @param bIsKeyFrame ��¼��֡�Ƿ�Ϊ�ؼ�֡
 * @param nTimeStamp ��ǰ֡��ʱ���
 *
 * @�ɹ��򷵻� 1 , ʧ���򷵻�0
 */
int SendH264Packet(unsigned char *data,unsigned int size,int bIsKeyFrame,unsigned int nTimeStamp)  
{  
	if(data == NULL && size<11){  
		return false;  
	}  

	unsigned char *body = (unsigned char*)malloc(size+9);  
	memset(body,0,size+9);

	int i = 0; 
	if(bIsKeyFrame){  
		body[i++] = 0x17;// 1:Iframe  7:AVC   
		body[i++] = 0x01;// AVC NALU   
		body[i++] = 0x00;  
		body[i++] = 0x00;  
		body[i++] = 0x00;  


		// NALU size   
		body[i++] = size>>24 &0xff;  
		body[i++] = size>>16 &0xff;  
		body[i++] = size>>8 &0xff;  
		body[i++] = size&0xff;
		// NALU data   
		memcpy(&body[i],data,size);  
		//SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
	}else{  
		body[i++] = 0x27;// 2:Pframe  7:AVC   
		body[i++] = 0x01;// AVC NALU   
		body[i++] = 0x00;  
		body[i++] = 0x00;  
		body[i++] = 0x00;  


		// NALU size   
		body[i++] = size>>24 &0xff;  
		body[i++] = size>>16 &0xff;  
		body[i++] = size>>8 &0xff;  
		body[i++] = size&0xff;
		// NALU data   
		memcpy(&body[i],data,size);  
	}  
	

	int bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO,body,i+size,nTimeStamp);  

	free(body);  

	return bRet;  
} 

/**
 * ���ڴ��ж�ȡ����һ��Nal��Ԫ
 *
 * @param nalu �洢nalu����
 * @param read_buffer �ص������������ݲ����ʱ��ϵͳ���Զ����øú�����ȡ�������ݡ�
 *					2���������ܣ�
 *					uint8_t *buf���ⲿ���������õ�ַ
 *					int buf_size���ⲿ���ݴ�С
 *					����ֵ���ɹ���ȡ���ڴ��С
 * @�ɹ��򷵻� 1 , ʧ���򷵻�0
 */
int ReadFirstNaluFromBuf(NaluUnit &nalu,int (*read_buffer)(int fd,uint8_t *buf, int buf_size)) 
{
	int naltail_pos=nalhead_pos;
	memset(m_pFileBuf_tmp,0,BUFFER_SIZE);
	while(nalhead_pos<m_nFileBufSize)  
	{  
		//search for nal header
		if(m_pFileBuf[nalhead_pos++] == 0x00 && 
			m_pFileBuf[nalhead_pos++] == 0x00) 
		{
			if(m_pFileBuf[nalhead_pos++] == 0x01)
				goto gotnal_head;
			else 
			{
				//cuz we have done an i++ before,so we need to roll back now
				nalhead_pos--;		
				if(m_pFileBuf[nalhead_pos++] == 0x00 && 
					m_pFileBuf[nalhead_pos++] == 0x01)
					goto gotnal_head;
				else
					continue;
			}
		}
		else 
			continue;

		//search for nal tail which is also the head of next nal
gotnal_head:
		//normal case:the whole nal is in this m_pFileBuf
		naltail_pos = nalhead_pos;  
		while (naltail_pos<m_nFileBufSize)  
		{  
			if(m_pFileBuf[naltail_pos++] == 0x00 && 
				m_pFileBuf[naltail_pos++] == 0x00 )
			{  
				if(m_pFileBuf[naltail_pos++] == 0x01)
				{
					nalu.size = (naltail_pos-3)-nalhead_pos;
					break;
				}
				else
				{
					naltail_pos--;
					if(m_pFileBuf[naltail_pos++] == 0x00 &&
						m_pFileBuf[naltail_pos++] == 0x01)
					{	
						nalu.size = (naltail_pos-4)-nalhead_pos;
						break;
					}
				}
			}  
		}

		nalu.type = m_pFileBuf[nalhead_pos]&0x1f; 
		memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu.size);
		nalu.data=m_pFileBuf_tmp;
		nalhead_pos=naltail_pos;
		return TRUE;   		
	}
}

/**
 * ���ڴ��ж�ȡ��һ��Nal��Ԫ
 *
 * @param nalu �洢nalu����
 * @param read_buffer �ص������������ݲ����ʱ��ϵͳ���Զ����øú�����ȡ�������ݡ�
 *					2���������ܣ�
 *					uint8_t *buf���ⲿ���������õ�ַ
 *					int buf_size���ⲿ���ݴ�С
 *					����ֵ���ɹ���ȡ���ڴ��С
 * @�ɹ��򷵻� 1 , ʧ���򷵻�0
 */
int ReadOneNaluFromBuf(NaluUnit &nalu,int fd,int (*read_buffer)(int fd,uint8_t *buf, int buf_size))  
{    
	
	int naltail_pos=nalhead_pos;
	int ret;
	int nalustart;//nal�Ŀ�ʼ��ʶ���Ǽ���00
	memset(m_pFileBuf_tmp,0,BUFFER_SIZE);
	nalu.size=0;
	while(1)
	{
		if(nalhead_pos==NO_MORE_BUFFER_TO_READ)
			return FALSE;
		while(naltail_pos<m_nFileBufSize)  
		{  
			//search for nal tail
			if(m_pFileBuf[naltail_pos++] == 0x00 && 
				m_pFileBuf[naltail_pos++] == 0x00) 
			{
				if(m_pFileBuf[naltail_pos++] == 0x01)
				{	
					nalustart=3;
					goto gotnal ;
				}
				else 
				{
					//cuz we have done an i++ before,so we need to roll back now
					naltail_pos--;		
					if(m_pFileBuf[naltail_pos++] == 0x00 && 
						m_pFileBuf[naltail_pos++] == 0x01)
					{
						nalustart=4;
						goto gotnal;
					}
					else
						continue;
				}
			}
			else 
				continue;

			gotnal:	
 				/**
				 *special case1:parts of the nal lies in a m_pFileBuf and we have to read from buffer 
				 *again to get the rest part of this nal
				 */
				if(nalhead_pos==GOT_A_NAL_CROSS_BUFFER || nalhead_pos==GOT_A_NAL_INCLUDE_A_BUFFER)
				{
					nalu.size = nalu.size+naltail_pos-nalustart;
					if(nalu.size>BUFFER_SIZE)
					{
						m_pFileBuf_tmp_old=m_pFileBuf_tmp;	//// save pointer in case realloc fails
						if((m_pFileBuf_tmp = (unsigned char*)realloc(m_pFileBuf_tmp,nalu.size)) ==  NULL )
						{
							free( m_pFileBuf_tmp_old );  // free original block
							return FALSE;
						}
					}
					memcpy(m_pFileBuf_tmp+nalu.size+nalustart-naltail_pos,m_pFileBuf,naltail_pos-nalustart);
					nalu.data=m_pFileBuf_tmp;
					nalhead_pos=naltail_pos;
					return TRUE;
				}
				//normal case:the whole nal is in this m_pFileBuf
				else 
				{  
					nalu.type = m_pFileBuf[nalhead_pos]&0x1f; 
					nalu.size=naltail_pos-nalhead_pos-nalustart;
					if(nalu.type==0x06)
					{
						nalhead_pos=naltail_pos;
						continue;
					}
					memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu.size);
					nalu.data=m_pFileBuf_tmp;
					nalhead_pos=naltail_pos;
					return TRUE;    
				} 					
		}

		if(naltail_pos>=m_nFileBufSize && nalhead_pos!=GOT_A_NAL_CROSS_BUFFER && nalhead_pos != GOT_A_NAL_INCLUDE_A_BUFFER)
		{
			nalu.size = BUFFER_SIZE-nalhead_pos;
			nalu.type = m_pFileBuf[nalhead_pos]&0x1f; 
			memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu.size);
			if((ret=read_buffer(fd,m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
			{
				memcpy(m_pFileBuf_tmp+nalu.size,m_pFileBuf,ret);
				nalu.size=nalu.size+ret;
				nalu.data=m_pFileBuf_tmp;
				nalhead_pos=NO_MORE_BUFFER_TO_READ;
				return FALSE;
			}
			naltail_pos=0;
			nalhead_pos=GOT_A_NAL_CROSS_BUFFER;
			continue;
		}
		if(nalhead_pos==GOT_A_NAL_CROSS_BUFFER || nalhead_pos == GOT_A_NAL_INCLUDE_A_BUFFER)
		{
			nalu.size = BUFFER_SIZE+nalu.size;
				
				m_pFileBuf_tmp_old=m_pFileBuf_tmp;	//// save pointer in case realloc fails
				if((m_pFileBuf_tmp = (unsigned char*)realloc(m_pFileBuf_tmp,nalu.size)) ==  NULL )
				{
					free( m_pFileBuf_tmp_old );  // free original block
					return FALSE;
				}

			memcpy(m_pFileBuf_tmp+nalu.size-BUFFER_SIZE,m_pFileBuf,BUFFER_SIZE);
			
			if((ret=read_buffer(fd,m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
			{
				memcpy(m_pFileBuf_tmp+nalu.size,m_pFileBuf,ret);
				nalu.size=nalu.size+ret;
				nalu.data=m_pFileBuf_tmp;
				nalhead_pos=NO_MORE_BUFFER_TO_READ;
				return FALSE;
			}
			naltail_pos=0;
			nalhead_pos=GOT_A_NAL_INCLUDE_A_BUFFER;
			continue;
		}
	}
	return FALSE;  
} 

/**
 * ���ڴ��е�һ��H.264�������Ƶ��������RTMPЭ�鷢�͵�������
 *
 * @param read_buffer �ص������������ݲ����ʱ��ϵͳ���Զ����øú�����ȡ�������ݡ�
 *					2���������ܣ�
 *					uint8_t *buf���ⲿ���������õ�ַ
 *					int buf_size���ⲿ���ݴ�С
 *					����ֵ���ɹ���ȡ���ڴ��С
 * @�ɹ��򷵻�1 , ʧ���򷵻�0
 */ 
/*int RTMP264_Send(int fd,int (*read_buffer)(int fd,unsigned char *buf, int buf_size))  
{    
	int ret;
	uint32_t now,last_update;
	  
	memset(&metaData,0,sizeof(RTMPMetadata));
	memset(m_pFileBuf,0,BUFFER_SIZE);
	if((ret=read_buffer(fd,m_pFileBuf,m_nFileBufSize))<0)
	{
		return FALSE;
	}

	NaluUnit naluUnit;  
	// ��ȡSPS֡   
	ReadFirstNaluFromBuf(naluUnit,read_buffer);  
	metaData.nSpsLen = naluUnit.size;  
	printf("spslen %d\n",metaData.nSpsLen);
	metaData.Sps=NULL;
	metaData.Sps=(unsigned char*)malloc(naluUnit.size);
	memcpy(metaData.Sps,naluUnit.data,naluUnit.size);

	// ��ȡPPS֡   
	ReadOneNaluFromBuf(naluUnit,fd,read_buffer);  
	metaData.nPpsLen = naluUnit.size; 

	printf("Ppslen %d\n",metaData.nPpsLen);
	metaData.Pps=NULL;
	metaData.Pps=(unsigned char*)malloc(naluUnit.size);
	memcpy(metaData.Pps,naluUnit.data,naluUnit.size);
	
	// ����SPS,��ȡ��Ƶͼ���������Ϣ   
	int width = 0,height = 0, fps=0;  
	h264_decode_sps(metaData.Sps,metaData.nSpsLen,width,height,fps);  
	printf("width %d height %d fps %d\n",metaData.nWidth,metaData.nHeight,metaData.nFrameRate);
	//metaData.nWidth = width;  
	//metaData.nHeight = height;  
	if(fps)
		metaData.nFrameRate = fps; 
	else
		metaData.nFrameRate = 25;

	//����PPS,SPS
	//ret=SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
	//if(ret!=1)
	//	return FALSE;

	unsigned int tick = 0;  
	unsigned int tick_gap = 1000/metaData.nFrameRate; 
	ReadOneNaluFromBuf(naluUnit,fd,read_buffer);
	int bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;
	printf("type is %d\n",naluUnit.type);
	while(SendH264Packet(naluUnit.data,naluUnit.size,bKeyframe,tick))  
	{    
got_sps_pps:
		//if(naluUnit.size==8581)
			printf("NALU size:%8d\n",naluUnit.size);
		last_update=RTMP_GetTime();
		if(!ReadOneNaluFromBuf(naluUnit,fd,read_buffer))
				goto end;
		if(naluUnit.type == 0x07 || naluUnit.type == 0x08)
			goto got_sps_pps;
		bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;
		tick +=tick_gap;
		now=RTMP_GetTime();
		//msleep(tick_gap-now+last_update);  
		msleep(40);
	}  
	end:
	free(metaData.Sps);
	free(metaData.Pps);
	return TRUE;  
}  
*/
bool SendMetadata(LPRTMPMetadata lpMetaData)
{
	if(lpMetaData == NULL)
	{
		return false;
	}
	char body[1024] = {0};;
    
    char * p = (char *)body;  
	p = put_byte(p, AMF_STRING );
	p = put_amf_string(p , "@setDataFrame" );

	p = put_byte( p, AMF_STRING );
	p = put_amf_string( p, "onMetaData" );

	p = put_byte(p, AMF_OBJECT );  
	p = put_amf_string( p, "copyright" );  
	p = put_byte(p, AMF_STRING );  
	p = put_amf_string( p, "firehood" );  

	p =put_amf_string( p, "width");
	p =put_amf_double( p, lpMetaData->nWidth);

	p =put_amf_string( p, "height");
	p =put_amf_double( p, lpMetaData->nHeight);

	p =put_amf_string( p, "framerate" );
	p =put_amf_double( p, lpMetaData->nFrameRate); 

	p =put_amf_string( p, "videocodecid" );
	//p =put_amf_double( p, FLV_CODECID_H264 );
	p =put_amf_double( p, 7 );

	p =put_amf_string( p, "" );
	p =put_byte( p, AMF_OBJECT_END  );

	int index = p-body;

	SendPacket(RTMP_PACKET_TYPE_INFO,(unsigned char*)body,p-body,0);

	int i = 0;
	body[i++] = 0x17; // 1:keyframe  7:AVC
	body[i++] = 0x00; // AVC sequence header

	body[i++] = 0x00;
	body[i++] = 0x00;
	body[i++] = 0x00; // fill in 0;

	// AVCDecoderConfigurationRecord.
	body[i++] = 0x01; // configurationVersion
	body[i++] = lpMetaData->Sps[1]; // AVCProfileIndication
	body[i++] = lpMetaData->Sps[2]; // profile_compatibility
	body[i++] = lpMetaData->Sps[3]; // AVCLevelIndication 
    body[i++] = 0xff; // lengthSizeMinusOne  

    // sps nums
	body[i++] = 0xE1; //&0x1f
	// sps data length
	body[i++] = lpMetaData->nSpsLen>>8;
	body[i++] = lpMetaData->nSpsLen&0xff;
	// sps data
	memcpy(&body[i],lpMetaData->Sps,lpMetaData->nSpsLen);
	i= i+lpMetaData->nSpsLen;

	// pps nums
	body[i++] = 0x01; //&0x1f
	// pps data length 
	body[i++] = lpMetaData->nPpsLen>>8;
	body[i++] = lpMetaData->nPpsLen&0xff;
	// sps data
	memcpy(&body[i],lpMetaData->Pps,lpMetaData->nPpsLen);
	i= i+lpMetaData->nPpsLen;

	return SendPacket(RTMP_PACKET_TYPE_VIDEO,(unsigned char*)body,i,0);
}
int is_read_meta = 0;
unsigned int tick = 0; 
unsigned int tick_gap = 0; 


int RTMP264_Send(NaluUnit  *naluUnit, int buf_size)
{    
	int ret;
	//memset(m_pFileBuf,0,BUFFER_SIZE);
	/*
	int ll ; 
	char *pp;
	ll = naluUnit->size;
	pp = naluUnit->data;
	while(ll--)
	{
		printf("%x\n",*pp++);
	
	}

	*/

	int width = 0,height = 0, fps=0;  

	tick = GetTickCount();
	//if(!is_read_meta)
	{

				  
			if(naluUnit->type == 0x07)
			{
				 
				metaData.nSpsLen = naluUnit->size;
				memcpy(metaData.Sps,naluUnit->data,naluUnit->size);

				return ;
			  
			}
			
			 else if(naluUnit->type == 0x08)
			 {

				metaData.nPpsLen = naluUnit->size;
				memcpy(metaData.Pps,naluUnit->data,naluUnit->size);
				 h264_decode_sps(metaData.Sps,metaData.nSpsLen,width,height,fps);

				 printf("width %d heigh %d fps %d\n",width,height,fps);
				
				 if(fps)
					 metaData.nFrameRate = fps;
				 else
					 metaData.nFrameRate = 25;
				 metaData.nWidth = width; 
				 metaData.nHeight = height;

				 tick_gap = 1000/metaData.nFrameRate;
				 //SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
				 
				 SendMetadata(&metaData);
				 is_read_meta = 1; //读到了metadata 不读了
				 return;
				
			 }

	}
	if(naluUnit->type == 0x06)
	  
		return;


	int bKeyframe  = (naluUnit->type == 0x05) ? TRUE : FALSE;
	//tick +=35;//=tick_gap;
	//tick = GetTickCount();



//	if(naluUnit->type == 0x07 || naluUnit->type == 0x08)
//		return;
	/*
		char head[4] = {0x00,0x00,0x00,0x01};
			fwrite(head,1,4,pFile);
			fwrite(naluUnit->data,1,naluUnit->size,pFile);
			*/


#ifdef DEBUG
	printf("tick =%d\n",tick);
#endif

	ret = SendH264Packet(naluUnit->data,naluUnit->size,bKeyframe,tick);

	
#ifdef DEBUG
	printf("Send Packet ret %d \n",ret);
#endif



	return ret;  
}  

// AAC sequence header == AACDecoderSpecificInfo
int SendAacSequenceHderPacket0(unsigned char *spec_buf,int spec_len)
{
    RTMPPacket * packet;
    unsigned char * body;
    //int audio_specific_config = 0;
    int len;
    len = spec_len;  /*spec data长度,一般是2*/

    packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+len+2);
    memset(packet,0,RTMP_HEAD_SIZE);

    packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
    body = (unsigned char *)packet->m_body;

    /*AF 00 => AAC sequence header*/
    body[0] = 0xAF;
    body[1] = 0x00;                //AACPackType == 0x00 (AAC sequence header)
    memcpy(&body[2],spec_buf,len); //spec_buf是AAC sequence header数据

    /*
    //按照AACDecoderSpecificInfo 产生
    audio_specific_config |= ((2<<11)&0xF800); // 2:AAC:LC
    audio_specific_config |= ((4<<7)&0x0780);  // 4:44.1khz
    audio_specific_config |= ((2<<3)&0x78);    // 2:stereo
    audio_specific_config |= 0&0x07; //padding:000

    body[3] = (audio_specific_config>>8)&0xFF;
    body[4] = audio_specific_config&0xFF;
    */

    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nBodySize = len+2; 
    packet->m_nChannel = 0x04;
    packet->m_nTimeStamp = 0;
    packet->m_hasAbsTimestamp = 0;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_nInfoField2 = m_pRtmp->m_stream_id;

    /*调用发送接口*/
    RTMP_SendPacket(m_pRtmp,packet,TRUE);
    return TRUE;
}
int SendAacRawPacket(unsigned char *buf, int len, unsigned int nTimeStamp)
{
    if (len > 0) {
        RTMPPacket * packet;
        unsigned char * body;

        packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+len+2);
        memset(packet,0,RTMP_HEAD_SIZE);

        packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
        body = (unsigned char *)packet->m_body;

        /*AF 01 => AAC RAW data*/
        body[0] = 0xAF;
        body[1] = 0x01; //AAC RAW data
        //aac raw data
        memcpy(&body[2],buf,len);

        packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
        packet->m_nBodySize = len+2;
        packet->m_nChannel = 0x04;
        packet->m_nTimeStamp = nTimeStamp;//timeoffset;
        packet->m_hasAbsTimestamp = 0;
        packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
        packet->m_nInfoField2 = m_pRtmp->m_stream_id;

        /*调用发送接口*/
        RTMP_SendPacket(m_pRtmp,packet,TRUE);
        free(packet);
    }
    return 0;
}



