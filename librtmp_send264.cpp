#include <stdio.h>
#include <stdlib.h>
#include "librtmp_send264.h"
#include "librtmp/rtmp.h"   
#include "librtmp/rtmp_sys.h"   
#include "librtmp/amf.h"  
#include "sps_decode.h"
#include <sys/time.h>

#ifdef WIN32     
#include <windows.h>  
#pragma comment(lib,"WS2_32.lib")   
#pragma comment(lib,"winmm.lib")  
#endif 

//�����ͷ���ȣ�RTMP_MAX_HEADER_SIZE=18
#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)
//�洢Nal��Ԫ���ݵ�buffer��С
#define BUFFER_SIZE 327680
//��ѰNal��Ԫʱ��һЩ��־
#define GOT_A_NAL_CROSS_BUFFER BUFFER_SIZE+1
#define GOT_A_NAL_INCLUDE_A_BUFFER BUFFER_SIZE+2
#define NO_MORE_BUFFER_TO_READ BUFFER_SIZE+3

/**
 * _NaluUnit
 * �ڲ��ṹ�塣�ýṹ����Ҫ���ڴ洢�ʹ���Nal��Ԫ�����͡���С������
 */ 
typedef struct _NaluUnit  
{  
	int type;  
    int size;  
	unsigned char *data;  
}NaluUnit;

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
int RTMP264_Connect(const char* url)  
{  
	nalhead_pos=0;
	m_nFileBufSize=BUFFER_SIZE;
	m_pFileBuf=(unsigned char*)malloc(BUFFER_SIZE);
	m_pFileBuf_tmp=(unsigned char*)malloc(BUFFER_SIZE);
	InitSockets();  

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
} 

/**
 * ����RTMP���ݰ�
 *
 * @param nPacketType ��������
 * @param data �洢��������
 * @param size ���ݴ�С
 * @param nTimestamp ��ǰ����ʱ���
 *
 * @�ɹ��򷵻� 1 , ʧ���򷵻�һ��С��0����
 */
int SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp)  
{  
	if(m_pRtmp == NULL)
	{
		return FALSE;
	}

	RTMPPacket packet;
	RTMPPacket_Reset(&packet);
	RTMPPacket_Alloc(&packet,size);

	packet.m_packetType = nPacketType;
	packet.m_nChannel = 0x04;  
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;  
	packet.m_nTimeStamp = nTimestamp;  
	packet.m_nInfoField2 = m_pRtmp->m_stream_id;
	packet.m_nBodySize = size;
	memcpy(packet.m_body,data,size);

	int nRet = RTMP_SendPacket(m_pRtmp,&packet,0);

	RTMPPacket_Free(&packet);

	return nRet;
}  
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
int SendVideoSpsPps(unsigned char *pps,int pps_len,unsigned char * sps,int sps_len,int nTimeStamp)
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
	packet->m_nTimeStamp = nTimeStamp;
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
 *
 */
int SendH264Packet(unsigned char *data,unsigned int size,int bIsKeyFrame,unsigned int nTimeStamp)  
{  
	if(data == NULL && size<11){  
		return false;  
	}  

	if (data[2] == 0x00) { 
		data += 4;
		size -= 4;
	} else if (data[2] == 0x01){
		data += 3;
		size -= 3;

	}
	unsigned char *body = (unsigned char*)malloc(size+9);  
	memset(body,0,size+9);

	int i = 0; 
	if(bIsKeyFrame){  
		body[i++] = 0x17;// 1:Iframe  7:AVC   
	}
	else {  
		body[i++] = 0x27;// 2:Pframe  7:AVC   
	}

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
	int bRet;	
	
	bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO,body,i+size,nTimeStamp);  

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
int ReadFirstNaluFromBuf(NaluUnit &nalu,int real_size,int (*read_buffer)(uint8_t *buf, int buf_size)) 
{
	int naltail_pos=nalhead_pos;
	memset(m_pFileBuf_tmp,0,BUFFER_SIZE);
	//while(nalhead_pos<m_nFileBufSize)  
	while(nalhead_pos<real_size)  
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
		//while (naltail_pos<m_nFileBufSize)  
		while (naltail_pos<real_size)  
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
int ReadOneNaluFromBuf(NaluUnit &nalu,int (*read_buffer)(uint8_t *buf, int buf_size))  
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
			if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
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
			
			if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
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

	 struct timeval tBegin, tEnd;
	 int is_read_meta = 0;
int RTMP264_Send(int rfd,int (*read_buffer)(unsigned char *buf, int *buf_size,int * type,unsigned * timestamp))  
{    

	int ret;
	uint32_t now,last_update;
	  
	memset(&metaData,0,sizeof(RTMPMetadata));
	memset(m_pFileBuf,0,BUFFER_SIZE);
	unsigned int tick = 0;  
	unsigned int tick_gap; 
	unsigned int timestamp;
	int width = 0,height = 0, fps=0;  
	NaluUnit naluUnit;  
	naluUnit.data = m_pFileBuf;

        int fd = rfd;

    struct timeval TimeoutVal;

    fd_set read_fds;

	metaData.Sps=NULL;
	metaData.Pps=NULL;
	metaData.Sps=(unsigned char*)malloc(5000);
	metaData.Pps=(unsigned char*)malloc(5000);
	while(1)  
	{    
		FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        TimeoutVal.tv_sec  = 3;
        TimeoutVal.tv_usec = 0;
        ret = select(fd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (ret < 0)
        {
            printf("select failed!\n");
            break;
        }
        else if (ret == 0)
        {
            printf("get venc stream time out,\n");
           	continue;
        }
        else
        {
			if (FD_ISSET(fd, &read_fds)) {
        
				ret=read_buffer(naluUnit.data,&naluUnit.size,&naluUnit.type,&timestamp);
				if(ret <= 0)
				  continue;
				printf("NALU size:%8d type:%d\n",naluUnit.size,naluUnit.type);
				if(!is_read_meta)
				{


					if(naluUnit.type == 0x07)
					{

						metaData.nSpsLen = naluUnit.size-4;  

						printf("1 ret %d\n",naluUnit.size-4);
						memcpy(metaData.Sps,naluUnit.data+4,naluUnit.size-4);

					}
					
					else if(naluUnit.type == 0x08)
					{

						metaData.nPpsLen = naluUnit.size-4; 
						memcpy(metaData.Pps,naluUnit.data+4,naluUnit.size-4);

						h264_decode_sps(metaData.Sps,metaData.nSpsLen,width,height,fps);  
						printf("width %d heigh %d fps %d\n",width,height,fps);

						if(fps)
							metaData.nFrameRate = fps; 
						else
							metaData.nFrameRate = 25;

						metaData.nWidth = 704;  
						metaData.nHeight = 576;  

						tick_gap = 1000/metaData.nFrameRate; 

						//SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen,nTimeStamp);
						SendMetadata(&metaData);
						is_read_meta = 1; //������metadata ������
					}
				}
				//if(naluUnit.type == 0x06)
				 // continue;

				int bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;

				tick +=tick_gap;

				//gettimeofday(&tBegin, NULL);
				//static int first_time = tBegin.tv_sec*1000+tBegin.tv_usec/1000;
				//int time = tBegin.tv_sec*1000+tBegin.tv_usec/1000- first_time;
				printf("time %d\n",tick);
				
				ret = SendH264Packet(naluUnit.data,naluUnit.size,bKeyframe,tick);
				printf("Send Packet ret %d \n",ret);
			}
		}
	}  
	end:
	free(metaData.Sps);
	free(metaData.Pps);
	printf("EEEEEEND\n");
	return TRUE;  
}  


