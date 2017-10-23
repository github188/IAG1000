#ifndef RTIMAGE2_H_20070301
#define RTIMAGE2_H_20070301

static const char version[]="0.26";			///<����汾��
static const int def_rtstream_port 	 = 8096;    	   ///<����Ƶ���з���Ĭ�϶˿ں�
static const int def_rtsnd_port 	        = 8097;		   ///<��Ƶ���з���Ĭ�϶˿ں�

static const int def_mic_gain 		 = 8;		   ///<Ĭ��mic����
static const int def_audio_gain 	        = 8;		   ///<Ħ����Ƶ�������

static const int def_svr_timeout         = 10;               ///<Ĭ�ϵĳ�ʱʱ��

static const int def_audio_pkt_size    = 1024;           ///<Ĭ����Ƶ�������ݰ���С

static const int def_th_drop_p            = 30;              ///< ��ʼ����p֡��Ƶ��Ĭ����ֵ

#define DEF_AUDIO_SAMRATE             (8000)                  ///<Ĭ����Ƶ������
#define DEF_AUDIO_FRAGSIZE            (10)                     ///<Ĭ����Ƶ�������С2^DEF_FRAGSIZE
#define DEF_AUDIO_FRAGNB               (16)                    ///<Ĭ����Ƶ�ɼ�����������

#define	TCPRT_MAX_VIRAVUSR_NO	10		///<3022�������豸һ��֧�ֵ��������Ƶ�����û���

#define	TCPRTIMG_MAX_AVUSR_NO       16          ///<tcprtimage2֧�ֵ��������Ƶ�����û������� 
#define TCPRTIMG_MAX_APLAY_NO       1
#define	MAX_MAP_BUF_FRAMES            600        ///<���Ļ���֡��(���ڷ��仺����)



#ifndef IN
    #define IN          //���������־
    #define OUT       //���������־
    #define IO          //�������������־
#endif



//ϵͳͷ�ļ�
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

//���п�����ͷ�ļ�
#include "typedefine.h"
#include "file_def.h"
#include "devinfo.h"


#include "devinfo_virdev.h"			///<devinfo.h����չ

#include "rtimg_para2.h"



/** 
 *   @brief     �ж�һ����ַ�Ƿ�Ϊͬһ���������е�
 *   @param  ��Ҫ�жϵ�ip��ַ
 *   @return   1��ʾ��ͬһ����������,0��ʾ����
 */ 
int is_lan_user(in_addr_t addr);


/**
  *   @brief	�ж���������Ƶ��guid�Ƿ�Ϸ�
  *   @param	cmd_dev_id��������Ƶ�������guid
  *	@param	cmd_addr��������Ƶ���豸ip�ĵ�ַ
  *	@return	�ɹ����������豸�ţ�ʧ�ܷ��ظ�ֵ
*/
int is_guid_valid(unsigned char * cmd_dev_id, struct sockaddr_in * cmd_addr);

#endif
