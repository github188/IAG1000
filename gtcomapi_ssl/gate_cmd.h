/*
	����GT1000ϵͳ��Զ�����ؽ�����������
*/

#ifndef GATE_CMD_H
#define GATE_CMD_H
//#include "dev_svr.h"
#include <status.h>
#ifndef _WIN32  
#include <pthread.h>
#include <typedefine.h>
#include<sys/time.h>
#include<unistd.h>
#include<stdio.h>
//#include <gt_netsdk_ip.h>
#else
#include <windows.h>
#endif
//GT1000������ͨѶʱ�õ��Ĺ���ʱ��Ľṹ
struct gt_time_struct{
	WORD		year;		//��
	BYTE		month;		//��
	BYTE		day;		//��
	BYTE		hour;		//ʱ
	BYTE		minute;		//��
	BYTE		second;		//��
	BYTE		reserve;		 
};

//PC������������
#define USR_REQUIRE_RT_IMAGE	0x0100				 //���ط����趨�������ʵʱͼ��˿ڵĿͻ�ip
#define USR_STOP_RT_IMAGE       0x0102
#define USR_REQUIRE_RT_IMAGE_ANSWER 0x8100

#define USR_REQUIRE_SPEAK		0x0107				//���ط������������Ƶ���ж˿ڵĿͻ���ip
#define USR_STOP_SPEAK		0x0107				//���ط������������Ƶ���ж˿ڵĿͻ���ip

#define USR_REQUIRE_RECORD_PLAYBACK 0x0617        //���ط�������¼��㲥����
#define USR_REQUIRE_RECORD_PLAYBACK_ANSWER 0x8617        //���ط�������¼��㲥����
#define USR_STOP_RECORD_PLAYBACK 0x06118        //���ط�������¼��㲥����

//control&set
#define  USR_SET_AUTH_IP   				0x0101		 //���ط�������ַ�趨
#define  USR_CLOCK_SETING				0x0200 		 //ʱ���趨
#define  USR_SCREEN_SETING				0x0204 		 //����ָ����
#define  USR_NET_STREAM_SETING 		0x0205		 //�趨���紫��ͼ������
#define  USR_LOCAL_STREAM_SETING 		0x0206		//�趨������·������
#define  USR_SET_SENSER_ARRAY			0x0208		//�趨�˶����
#define  USR_SET_VIDEO_AD_PARAM		0x0209		//��Ƶת����������
#define  USR_SET_SWITCH_IN				0x0213		//������������������
#define  USR_SET_SWITCH_OUT			0x0214		//�����������������
#define  USR_SWITCH_OUT				0x0215		//�������������


#define  USR_LOCAL_RECORDER_CONTROL 	0x0216		//������·�����
#define  USR_LOCAL_RECORDER_SETING	0x0217		//������·����������
#define  USR_SEIRAL_PORT_CONTROL		0x0218		//�豸���пڿ���
#define  USR_CANCEL_ALARM				0x0219		//�������״̬

#define  USR_QUERY_INDEX				0x0105		//��ѯ�豸�����������ļ�
#define  USR_QUERY_FTP					0x0106		//��ѯftp�����ʺ�����
#define  USR_RW_DEV_PARA				0x0108		//�����豸�����ļ�
#define  USR_QUERY_TRIGSTATE			0x0109		//�豸����״̬��ѯ	add-20071113		

#define 	USR_QUERY_STATE				0x0103		//�豸״̬��ѯ
#define 	USR_CMD_ACK					0x0201		//����USER��Ӧ�豸����������
#define 	USER_UPDATE                			0x0220 		//�������
#define 	USR_TAKE_HQ_PIC				0x0221 		//ץͼ			
#define	START_ALARM_ACTIONS_YES		0x0222 		//ȷ�ϱ���
#define 	START_ALARM_ACTIONS			0x0223		//����ִ�б�������
#define 	USR_LOCK_FILE_TIME			0x0224		//��ָ��ʱ��εĸ������ļ����������
#define	USR_REBOOT_DEVICE				0x0225		//�豸��λ
#define	USR_SET_ALARM_SCHEDULE		0x0226		//���������趨������Чʱ���
#define	UPDATE_SOFTWARE_DIRECT		0x0227		///ֱ�Ӹ��豸����(������ftp��ʽ)
#define 	USR_QUERY_HD_STATUS			0x0228		//Ӳ��״̬������ѯ	//add-20071113
#define	USR_QUERY_REGIST				0x0229		//ע���ѯ				//add-20071113
#define	USR_QUERY_VIDEO_AD_PARAM	0x0230		//ͼ��A/D������ѯ		//add-20071112
#define   USR_RUN_HD_TEST 				0x0231     	//����Ӳ��״̬����
#define	USR_FORMAT_HARDDISK			0x0232		//Զ�̸�ʽ��Ӳ��

//// 2010-9-16 by lsk
#define	USR_TAKE_HQ_PIC_DIR			0x0233		//������¼�����(ֱ��ģʽ)
#define	USR_SEIRAL_PORT_INFO 			0x0234		//͸�����ڿ������� (ֱ��ģʽ)
#define   USR_QUERY_INDEX_DIR			0x0235		//��ѯ�豸�����������ļ�(ֱ��ģʽ)

#define 	USR_RW_DEV_PARA				0x0108		//�����豸�����ļ�

#define	USR_LOGIN_DEVICE				0x0110		///<��¼�豸
#define   USR_REQUIRE_SELF_IP			0x0111		///<��ѯ�Լ����豸�˿�����ip��ַ
#define	USR_QUERY_DEVICE_POSITION	0x0113		//�����豸�����Լ���λ����Ϣ			
//�豸���Ѻ�����2010 -02- 01	//add by lsk
#define	USR_WAKE_UP_DEVICE			0x0114		//����
//#define	USR_QUERY_HEART_BEAT			0x0115		//����

//// 2010-9-16 by lsk
#define	USR_QUERY_LOG					0x0116		//��ѯ�豸��־(ֱ��ģʽ)


//// 2013-12-24 lc ����¼��㲥���������ṹ
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
	unsigned short year;
	unsigned char  month;
	unsigned char  day;
	unsigned char  hour;
	unsigned char  minute;
	unsigned char  second;
	unsigned char  reserved;
}timepoint_struct;

typedef enum
{
	CQSPEED,
	CHSPEED,
	CNSPEED,
	CTSPEED,
	CFSPEED,
	CISPEED
}transpeed_t;

typedef enum
{
	PAUSE,
	RESUME,
	SPEED,
	SEEK
}recordctl_t;

#define  USR_QUERY_TIMESECTION			0x0120		//��ѯ�豸¼��ʱ���

typedef struct
{
	unsigned char  channel;            ////��Ӧ��Ƶͨ��
	unsigned char reserved[3];
	
	timepoint_struct starttime;
	timepoint_struct endtime;
}usr_query_timesection_struct;  

#define  DEV_TIMESECTION_RETURN		0x8220
typedef struct
{
	unsigned char  dev_id[8];           ///dev guid
	unsigned short result;              ////�����ɹ�0���鲻������ERR_DVC_NO_RECORD_INDEX,ERR_ENC_NOT_ALLOW�豸����������
	unsigned short format;              ////������ʽ 0��ԭ�� 1��gzipѹ�� Ŀǰֻ֧��ԭ�ķ�ʽ

	unsigned char  index_file[48];      ////�����ļ�������ʽΪ: ·�� + �ļ���
}dev_timesection_ret_struct;

#define  VIEWER_SUBSCRIBE_RECORD		0x0607

typedef struct
{
/*
	unsigned short len;
	unsigned short cmd;                 ////VIEWER_SUBSCRIBE_RECORD
	unsigned char  en_ack; 
	unsigned char  reserved[3];
*/
	unsigned char  dev_ip[32];           ////�豸ip
	unsigned char  reserved1[12];        //// for ipv6
	unsigned char  dev_id[8];           ////dev guid
	unsigned char  gateway_ip[32];          ////gate ip
	unsigned char  reserved2[12];        //// for ipv6
	unsigned char  token[20];            ////������������(�ַ���)

	unsigned short user_level;          ////�����û����ʼ���
	unsigned char  user_account;        ////    �û��ʺ�
	unsigned char  reserved3;
	
	unsigned short trans_protocal;      ////    ����Э��
	unsigned short time_out;             ////	����ؼ����ӱ��ϣ�ת����������Ҫ�ȴ���ʱ�䣬����Ϊ��λ���������Ϊ0����ʹ��ת���������Զ���ĳ�ʱʱ�䣩

	unsigned short band_width;          ////  �û����������KΪ��λ(���ֶ�Ԥ����������趨�ɽ�����Ϊ0)
	unsigned char  reserved4[2];

	unsigned int   dev_port;             ////  �豸¼�����˿�
	unsigned int   stream_idx;           ////  ¼��ط�����ţ��ɼ����˲���

	unsigned char  peer_ip[32];            ////�����ip��ַ
	unsigned short peer_port;            ////����˼���port�˿�
	unsigned short channel;              //// 0-31
	
	transpeed_t    speed;                ////  �㲥���� 
	timepoint_struct starttime;          ////  ��ʼ����ʱ�䣨��Чʱ����ڣ�
	timepoint_struct endtime;            ////  ��ֹ����ʱ�䣨��Чʱ����ڣ�
}viewer_subscribe_record_struct;

#define  VIEWER_SUBSCRIBE_ANSWER_RECORD	0x0608

typedef struct
{
	unsigned char  dev_id[8];           ////dev guid
	unsigned short status;              ////���ز���״̬��0��ʾ�ɹ�����0��ʾ�����)
	unsigned char  reseved[2];
	
	unsigned int   video_trans_id;     ////��ת����������̬���ɵĴ�������ID�ţ��粻�ɹ�Ϊ0��
	unsigned int   query_usr_id;       //// �����豸���ظ�sdk�˵Ķ����û���ʶ��
}viewer_subsrcibe_answer_record_struct;

#define  VIEWER_SUBSCRIBE_RECORD_CONTROL 	0x0609

typedef struct
{
	unsigned char  dev_id[8];           ////dev guid
	unsigned int   query_usr_id;       //// �����豸���ظ�sdk�˵Ķ����û���ʶ��

	recordctl_t    ctl_cmd;             //// �طſ�������
	transpeed_t    speed;               //// �ٶ�ֵ

	timepoint_struct starttime;        ////  �ı䶩��ʱ��
	timepoint_struct endtime;              ////  �ı䶩��ʱ��
}viewer_subscribe_record_ctl_struct;

#define  VIEWER_SUBSCRIBE_RECORD_CONTROL_RESULT	 0x061a

typedef struct
{
	unsigned char  dev_id[8];           ////dev guid
	unsigned short status;              ////���ز���״̬��0��ʾ�ɹ�����0��ʾ�����)
	unsigned char  reserved2[2];

	unsigned char  msg[16];              //// Ӣ����������ԭ��
}viewer_subscribe_record_ctl_ret_struct;

#define  VIEWER_UNSUBSCRIBE_RECORD	0x061b

typedef struct
{
	unsigned char  dev_id[8];           //// dev guid
	unsigned int   query_usr_id;       //// �����豸���ظ�sdk�˵Ķ����û���ʶ��
}viewer_unsubscribe_record_struct;



////////////////////////////////////////////////////////////////////////////////////////////
//��ʱ�����ã�ɱvsmain
#define USR_KILL                        			0x0250


//GT1000����������
#define DEV_REGISTER					0x8101		//�豸ע��
#define DEV_STATE_RETURN				0x8102		//�豸״̬����
#define DEV_ALARM_RETURN				0x8103		//��������
#define DEV_COM_PORT_RET				0x8104		//���ڶ�Ӧ��tcp�˿ڷ���
#define USR_QUERY_FTP_ANSWER			0x8105		//ftp�ʺŷ���
#define DEV_RECORD_RETURN				0x8106		//���ظ�����¼�������ļ���

#define UPDATE_ANSWER                  		0x8107		//������Ӧ    
#define HQ_PIC_ANSWER              			0x8108		//ץͼ��Ӧ
#define DEV_PARA_RETURN 				0x8109		//���������ļ�
#define DEV_REQ_SYNC_TIME				0x810a		//����ʱ��ͬ��
#define DEV_QUERY_ADPARAM_RETURN 	0x810b		//����ͼ��A/D����	//add-20071112
#define DEV_QUERY_HDSTATUS_RETURN	0x810c		//����Ӳ��״̬������ѯ	//add-20071113
#define DEV_QUERY_TRIG_RETURN			0x810d 		//�����豸����״̬	//add-20071113
#define DEV_LOGIN_RETURN				0x8110		///�豸�Ե�¼�������Ӧ		
#define DEV_IP_RETURN					0x8111		///<����Զ�̿ͻ��˼��豸�Լ���ip��ַ
#define FORMAT_DISK_ANSWER			0x8112		///<��ʽ��Ӳ����Ӧ
#define DEV_POSITION_RETURN			0x8113		//�豸����λ����Ϣ,�յ�USR_QUERY_DEVICE_POSITION����󷵻�
#define DEV_SLEEP_REPORT				0x8114		//�豸����ǰ����Ϣ֪ͨ������USER	// add 2010-2-1 lsk
#define DEV_HEART_BEAT					0x8115		//����

//// 2010-9-16	by lsk
#define DEV_COM_INFO_RET				0x8116		//͸�����ڿ��ƺ����ݷ���(ֱ��ģʽ)
#define HQ_PIC_ANSWER_DIR         		0x8117		//ץͼ��Ӧ(ֱ��ģʽ)
#define DEV_QUERY_LOG_RETURN         		0x8118		//��־�ļ�����(ֱ��ģʽ)
#define DEV_QUERY_NDEX_DIR_RETURN      	0x8119		//¼�������ļ�����(ֱ��ģʽ)

#define DEV_CMD_ACK					0x8200		//�豸������Ӧ����USER������

//#define DEV_UNSUBSCRIBE_RECORD_ANSWER  0x8201   ///�豸����ֹͣ¼������Ӧ

//���ط��������������ļ���������
struct usr_rwdevpara_struct{
  	WORD	type;	       // ���ͣ�2:ip1004.ini 3:alarm.ini����ֵ���������REMOTE_FILE_TYPE�Ķ��ԣ�
	WORD	mode;		//����ģʽ:0���� 1��д
	DWORD 	filelen;		//�ļ�����(��дģʽ����Ч,���Ȳ��ܳ���60k)
	BYTE	file[4];		//	�ļ�����(����дģʽ����Ч)
};
//���ط���������ʵʱͼ������
//����ṹ�ᱻԭ��ת����ʵʱͼ��ģ��
//ת��ʱ�ڽṹǰӦ����һ��fd�ֶα�ʾ������Ϣ��������
struct usr_req_rt_img_struct{
	WORD	save_flag;		//Ӧ��һֱΪ0
	WORD	mode;			//����ģʽ 1��ʾ�������ӣ�0��ʾǿ�жϿ��κ����е�ʵʱͼ�����ӣ�����ֵ����
	BYTE    dev_id[8];      //dev guid
	DWORD 	remoteip;		//Ҫ����ʵʱͼ��ļ����ip��ַ
	WORD	remoteport;     //�Զ˶˿�
	WORD    channel;        //ͨ����
	DWORD   stream_idx;     //�����
	WORD 	trans_protocal; //Э������ 
	WORD	audio_flag     //�Ƿ�����Ƶ
	BYTE  	reserved[12];   //ipv6
	
};


//�豸��Ӧ��������ʵʱͼ������
struct usr_req_rt_img_answer_struct{
	BYTE  	dev_id[8];           ////dev guid
	WORD  	status;              ////���ز���״̬��0��ʾ�ɹ�����0��ʾ�����)
	BYTE  	reseved[2];	
	DWORD	video_trans_id;     ////��ת����������̬���ɵĴ�������ID�ţ��粻�ɹ�Ϊ0��
	DWORD   query_usr_id;       //// �����豸���ظ�sdk�˵Ķ����û���ʶ��
};
struct usr_stop_rt_img_struct{
	unsigned char  dev_id[8];           //// dev guid
	unsigned int   query_usr_id;       //// �����豸���ظ�sdk�˵Ķ����û���ʶ��
};

//���ط������������ط���������
struct usr_set_auth_ip_struct{
	WORD save_flag;			//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��0-���棬1-����
	WORD reserve;			//����
							//������ʵķ�������Ϣ
	BYTE  server_sn;			//�������洢���(�������ȼ����)
	BYTE  reserve1[3];		//����
    DWORD ip;				//������ip��ַ��ip3,ip2,ip1,ip0��
	DWORD reserve2[12];		//Ϊipv6��ַ��չ����

};
//���ط�������ϵͳʱ�������
//#define SIZEOF_USR_SET_TIME_STRUCT	(9)
#define SIZEOF_USR_SET_TIME_STRUCT	(sizeof(struct usr_set_time_struct))
struct usr_set_time_struct{
	WORD	save_flag;		//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��0-���棬1-����
	WORD	year;			//��
	BYTE	month;			//��
	BYTE	day;			//��
	BYTE	hour; 			//ʱ
	BYTE	minute; 			//��
	BYTE	second;			//��

};




//����������������
struct trig_in_attrib_struct{
	WORD	save_flag;		//�Ƿ�洢��־
	WORD	ch_mode;		//���ֽڱ�ʾͨ���ţ����ֽڱ�ʾ�����򳣱�
	DWORD	action0;			//��Ӧ�˿ڰ󶨵Ķ���
	DWORD	action1;
	DWORD	action2;
	DWORD	action3;
};

//�豸���пڿ�����������ݽṹ USR_SEIRAL_PORT_CONTROL
#define SIZEOF_USR_SERIAL_CTL_STRUCT 28
struct usr_serial_ctl_struct{
	WORD	save_flag;		//�Ƿ�洢��־
	BYTE	ch;				//GT1000�ϵĴ��ں�
	BYTE	mode;			//�������ǶϿ�
	DWORD	remoteip;		//Ҫ���ʴ��ڵļ����ip��ַ
	BYTE	reserve[12];		//����for ipv6
//���ڲ�������
	DWORD	baud;			//������
	BYTE	databit;			//����λ��һ��Ϊ8
	BYTE	parity;			//�Ƿ�Ҫ��żУ��λ,һ��Ϊ����Ҫ'N'	
	BYTE	stopbit;			//ֹͣλ��һ��Ϊ1
	BYTE	flow;			//�������ƣ�һ�㲻��Ҫ
};
//�豸���ش��ڶ�Ӧ�˿ڵ����ݽṹ DEV_COM_PORT_RET
#define SIZEOF_DEV_COM_RET_STRUCT	6
struct dev_com_ret_struct{
	WORD	ch;				//GT1000�ϵĴ���ͨ����
	WORD	result;			//�������
	WORD	port;			//����ɹ���ʾ���ڶ�Ӧ��tcp�˿�
};



//zw-add-20100916 --------------------->ֱ��ģʽ�µĴ������ݽṹ
//PC�˷��͸��豸�Ľṹ
struct usr_serial_info_struct
{
	WORD	save_flag;		//�Ƿ�洢��־
	BYTE	ch;				//GT1000�ϵĴ��ں�
	BYTE	mode;			//���͵��Ǵ������û��Ǵ�������mode=0ȡ�����ӣ�mode=1���ã�mode=2 �������ݷ���
	DWORD	remoteip;		//Ҫ���ʴ��ڵļ����ip��ַ
	BYTE	reserve[12];		//����for ipv6
//���ڲ�������
	DWORD	baud;			//������
	BYTE	databit;			//����λ��һ��Ϊ8
	BYTE	parity;			//�Ƿ�Ҫ��żУ��λ,һ��Ϊ����Ҫ'N'	
	BYTE	stopbit;			//ֹͣλ��һ��Ϊ1
	BYTE	flow;			//�������ƣ�һ�㲻��Ҫ
	WORD 		data_len;				//�û����͵����ݳ���
//zw-del	BYTE		Data[data_len];			//������ݵĻ�����
	BYTE		data[2];
};
#define SIZEOF_USR_SERIAL_INFO_STRUCT (sizeof(struct usr_serial_info_struct))

//�豸���ظ�PC�Ľṹ
struct dev_com_info_ret_struct
{
	WORD		ch;				//GT1000�ϵĶ˿ں�
	WORD		result;			//�豸�������ڵĽ��
	WORD		data_len;		//�豸���յ��������ֽ���
};
#define	SIZEOF_DEV_COM_INFO_RET_STRUCT		(sizeof(struct dev_com_info_ret_struct))

//<-----------------------------------zw-add ֱ��ģʽ�´������ݽṹ


//----------------------------------------->zw-add ֱ��ģʽ��ץȡ����ͼ��
//PC�˷����豸�Ľṹ
struct usr_take_hq_pic_dir_struct
{
	WORD	save_flag;			//�����־
	WORD	rec_ch; 			//��λ��ʾ����Ҫ����ץͼ��ͨ��������2·ѹ����ǰ��Ƕ��ʽ��Ƶ����
								//��(DEVICE)�˲�����Ч������5·ѹ����ϵͳ�˲�����Ч��1��ʾ��Ҫ
								//¼��0��ʾ����Ҫ¼����0x0003��ʾͨ��0��1��Ҫץͼ������ͨ
								//������Ҫץͼ
	BYTE	time[8];				//ʱ���
	WORD	getpic;				//����ץͼ������
	WORD	interval;			//ץ�ĵ�ʱ����(��λ:����)(����ֵ)
};
#define 	SIZEOF_USR_TAKE_HQ_PIC_DIR_STRUCT		(sizeof(struct usr_take_hq_pic_dir_struct))

//�豸���ظ�PC�Ľṹ
struct dev_hq_pic_answer_dir_struct
{
	BYTE		dev_id[8];		//�豸id
	WORD 		state;			//���������0�ɹ�
								//����ֵΪ���󣬴������<<����Ŷ���>>�е�[ǰ���豸����Ĵ���]
								//ERR_DVC_FAILURE	ERR_DVC_NOT_SUPPORT	ERR_NO_SPACE
	WORD		reserve2;		//����
	BYTE		timestamp[8];	//ʱ���(stateΪ0��Ч����ֵ����USR_TAKE_HQ_PIC���͵�)
	BYTE		data_send;		//���ݷ��ͱ�־��1��ʾ���ݷ���δ��ɣ�0�������ݷ������(��������ļ�)
	BYTE		file_send;		//�ļ����ͱ�־��1����δ��ɣ�0�������
	WORD		reserve3;		//����
	BYTE		filename[48];	//ͼƬ���ļ���
	DWORD		pic_seq;			// ͼƬ���1.....10
	DWORD		pack_seq;		//�����к�1......100
	DWORD		pack_len;		//������ �Ϊ0--4096BYTEs
	BYTE		pack[2];			//������ȫ�ֽڶ���Ļ�����
};
#define SIZEOF_DEV_HQ_PIC_ANSWER_DIR_STRUCT		(sizeof(struct dev_hq_pic_answer_dir_struct))

//����ipmain_paraʹ�õı���
typedef struct 
{
	int		take_pic_dr_f;					//ֱ��ģʽ�¸���ץͼ��־�����Ϊ1����ʾֱ��ģʽ��ץͼ
	int 		netfd;									//���������õ�������
	int		send_file;								//���ļ���Ҫ���ͱ�־
	int		send_data;								//������Ҫ���ͱ�־
	int		pic_fd;									//ͼƬ�ļ�����
	int		pic_seq;									//ͼƬ���к�
	int		pack_seq;								//���ݰ����к�
	int 		dev_no;
	int 		result;
	unsigned short enc;
	unsigned short env;
	FILE		*index_fp;								//����ץͼ�����ļ�������
	BYTE	filename[48];							//����ͼƬ������
	BYTE	timestamp[8];							//����ͼƬ��ʱ���
	BYTE	devid[8];								//DEVID
}TAKE_HQ_STRUCT_DIR_FOR_VSMAIN;
//<-----------------------------------------zw-addֱ��ģʽ��ץȡ����ͼ��


//------------------------------------------>zw-add�豸���ص�ֱ��ģʽ�µĲ�ѯ�豸�����������ļ�
struct dev_query_index_dir_return
{
	BYTE		dev_id[8];		//�豸ID
	WORD		state;			//�������
	WORD		reserve2;
	BYTE			data_send;
	BYTE			reserve3;	
	BYTE			filename[48];
	DWORD		pack_seq;
	DWORD		pack_len;
	BYTE			pack[2];
};
#define 	SIZEOF_DEV_QUERY_INDEX_DIR_STRUCT		(sizeof(struct dev_query_index_dir_return))

//<-----------------------------------------zw-add 





//���紫���ͼ����ʾ��ʽ���ƽṹ
#define SIZEOF_SCR_CTRL_STRUCT			6
struct scr_ctrl_struct{
	WORD	save_flag;		//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��0-���棬1-����
	BYTE	disp_type;		//��ʾ��ʽ 	0����������ʾ
							//		  		1��4����ָ���ʾ
	BYTE	ch;				//����ǵ����淽ʽ������ֶα�ʾ��ʾ��ͨ���ţ�4���淽ʽ���ֶ���Ч
	WORD	reserve; 
};

//������Ƶת�������������?
#define SIZEOF_USR_VIDEO_PARA_STRUCT 12
struct usr_video_para_struct{
	WORD	save_flag;		//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��0-���棬1-����
	WORD	channel;		//Ҫ���õ�ͼ��ͨ����0��1��2��3�����²��� -1000��ʾ���趨����Ч��
	short	bright;			//����	��0��255
	short	hue;			//ɫ��	����127��127
	short	contrast;		//�Աȶ�	����127��127
	short	saturation;		//���Ͷ�  ��0��255
		
};

//�������紫������Ƶ�����Ĳ���
#define SIZEOF_USR_NET_AVSTREAM_SET_STRUCT 14
struct usr_net_avstream_set{
		WORD save_flag;		//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��0-���棬1-����
		WORD picsize;		//ͼ��������������
							//0x0000:  ��Ч��������������Ҫ�ı�
							//0x0001:	 D1------720*576��PAL��
							//0x0002:  CIF-----352*288
							//0x0003:  QCIF----176*144
							//0x0004:  HD1-----720*288
		WORD vbitrate;		//ͼ����������λkbit/s��0��ʾ����������Ҫ�ı�
		WORD frame;		//ͼ��֡�� ���25
		BYTE   aud_mode;	//	��Ƶ���뷽ʽ	0:����Ҫ��Ƶ 1:adpcm  2:pcm 3:raw-pcm
		BYTE   aud_samrate;	//��Ƶ�����ʣ���λkbit/s
		BYTE   aud_sambit;	//��Ƶת������ 24������
		BYTE   reserve[3];
	
};
#define SIZEOF_USR_LOCAL_AVSTREAM_SET_STRUCT 16
struct usr_local_avstream_set{
	
		WORD	save_flag;	//		����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��0-���棬1-����
		WORD	channel;	//	 	ͼ��ͨ��0,1,2,3(��������鿨�ṹ������Ƶ������������¼����4������ʽ����1��2��3��Ч)
		WORD	picsize	;	// 			ͼ��������������
							//0x0000:  ��Ч��������������Ҫ�ı�
							//0x0001:	 D1------720*576��PAL��
							//0x0002:  CIF-----352*288
							//0x0003:  QCIF----176*144
							//0x0004: halfd1
		WORD	vbitrate;	//ͼ����������λkbit/s��0��ʾ���������ֲ���
		WORD	frame;		//	ͼ��֡�� 0��25 1��25/2  2�� 25/4  3��25/8  4��25/16 5��25/32 ����ֵ��Ч
		BYTE   	aud_mode;	//��Ƶ���뷽ʽ	0:����Ҫ��Ƶ 1:adpcm 2��Pcm
		BYTE	aud_samrate; // ��Ƶ�����ʣ���λkbit/s  //����
		BYTE	aud_sambit;//��Ƶת������ 24������  //����
		BYTE   reserve[3];

};

typedef struct {
		WORD	save_flag; 		//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��ӦΪ0-����
		WORD	diskno;	 		//Ҫ��ʽ��Ӳ�̱��
								//0��ʾ��ʽ������Ӳ�̣�1��2�����ȱ�ʾ��ʽ����1��2..��Ӳ�̣���Ӳ��ʱ����
		DWORD	reserved1;		//����	
		char	usr_info[256];	//���и�ʽ���������û���Ϣ
}usr_format_hd_struct;


//ip1004ϵͳ��Ϣ
#define SIZEOF_IP1004_INFO_STRUCT	sizeof(struct ip1004_info_struct)//
struct ip1004_info_struct
{							//ע��ʱ���͵��豸��Ϣ�ṹ
	DWORD vendor;			//�豸�����̱�ʶ(4) +�豸�ͺű�ʶ(4)
	DWORD device_type;		//�豸�ͺ�
	DWORD protocal_version;	// ���غ�ǰ���豸ͨѶ��Э���ڲ��汾��(�޷�������)
	BYTE site_name[40];		//��װ�ص�����
	DWORD dev_ip;			//�豸�Ķ�̬ip��ַ
	BYTE reserve[12];		//Ϊipv6��ַ��չ����
 	WORD boot_type;		//�豸��ע��ԭ��,16λ����λ��ʾ����
  	WORD video_num;		//�豸��Ƶ����ͨ������
  	WORD com_num;			// ������
	WORD storage_type;		//�豸�Ĵ洢�������� 0��û�� 1��cf�� 2��Ӳ��
	DWORD storage_room;	//�豸�Ĵ洢�������� ��λMbyte
	WORD compress_num;	//ѹ��ͨ����	ĿǰΪ1��2��5
	WORD compress_type;	//ѹ���������ͣ�(ѹ��оƬ��ѹ����ʽ)
	WORD audio_compress;	//��������ѹ������
	WORD audio_in;			//��������ͨ������ĿǰΪ1
       WORD audio_in_act;		//��������ͨ����Чλ����������1-8��0-ͨ����Ч��1-��Ч
	WORD switch_in_num;		//����������ͨ����
	WORD switch_in_act;		//����������ͨ����Чλ����������1-8��0-ͨ����Ч��1-��Ч
	WORD switch_out_num;	//���������ͨ����
	WORD switch_out;			//���������ͨ����Чλ����������1-8��0-ͨ����Ч��1-��Ч
	WORD max_pre_rec;		//�豸���Ԥ¼ʱ�䣬����Ϊ��λ
	WORD max_dly_rec;		//�豸�����ʱ¼��ʱ��
	//�豸����ʱ��
	WORD	year;			//��
	BYTE	month;			//��
	BYTE 	day; 			//��
	BYTE	hour;			//ʱ
	BYTE	minute;			//��
	BYTE	second;			//��
	BYTE	reserve1; 
	BYTE	reserve2;		
	BYTE	sleep;			// ֧�����߹��ܱ�־  0 ��֧�֣� 1֧�֡�
	DWORD    cmd_port;    		//�������˿�
	DWORD    image_port;		//ͼ�����˿�	
	DWORD    audio_port;		//��Ƶ����˿�
	BYTE	firmware[20];		//�̼��汾�ţ���ʱ����
	BYTE	dev_info[40];		//�豸��һЩ�����Ϣ

	BYTE	ex_info[160];		//���dvs(����еĻ�)�������Ϣ������Ʒ�ƣ��˿ڣ��û���������
	
};

#if 0
struct per_state_struct{			//GT1000��������״̬
	unsigned 		video_loss0		:1;	//bit0��Ƶ���붪ʧ
	unsigned 		video_loss1		:1;
	unsigned		video_loss2		:1;
	unsigned 		video_loss3		:1;
	unsigned 		reserve			:8;
	unsigned		video_blind0	:1;//bit12 ��1·��Ƶ�ڵ�
	unsigned		video_blind1	:1;
	unsigned 		video_blind2	:1;
	unsigned 		video_blind3	:1;
	unsigned		audio_loss0		:1;	//bit16 ��Ƶ���붪ʧ
	unsigned		audio_loss1		:1;
	unsigned		audio_loss2		:1;
	unsigned		audio_loss3		:1;
	unsigned		disk_full		:1;	//������
	unsigned 		keyboard_err	:1;	 //�������ӹ���
	unsigned		reserve1		:2;
	unsigned 		pwr_loss		:1;	//��Ӵ����õ�Դ����
	unsigned		audio_out_err	:1;	//��Ƶ�������
	unsigned		reserve2		:6;
};
struct dev_state_struct{		//GT1000ϵͳ�ڲ�״̬
	unsigned		link_err			:1;	//����
	unsigned		mem_err		:1;	//�ڴ����
	unsigned		flash_err		:1;	//flash����
	unsigned		hd_err			:1;	//Ӳ�̹���	//��λ������
	unsigned		cf_err			:1;	//�洢������
	unsigned		audio_dec_err	:1;	//��Ƶ�����豸����
	unsigned		reserve			:2;
	unsigned		video_enc0_err	:1;	//netenc
	unsigned		video_enc1_err	:1;	//hq0
	unsigned		video_enc2_err	:1;	//hq1
	unsigned		video_enc3_err	:1;	//hq2
	unsigned		video_enc4_err	:1;	//hq3
	//06.09.06	�¼ӹ���
	unsigned		quad_dev_err	:1;	//����ָ�������
	unsigned 		watch_51_err		:1;	//51ģ�����
	unsigned        vda_mod_err     :1;//vdaģ�����
	unsigned        reserve1        :17;
};

struct	trig_state_struct{	//GT1000ϵͳ����״̬
	unsigned		trig0			:1;//�ⴥ��
	unsigned		trig1			:1;
	unsigned		trig2			:1;
	unsigned		trig3			:1;
	unsigned		trig4			:1;
	unsigned		trig5			:1;
	unsigned		trig6			:1;//�𶯴���//trig_vib
	unsigned		trig7			:1;//�ⲿ������Դ//trig_pwr
	unsigned		reserve			:2;
	unsigned		motion0			:1;//�ƶ�����
	unsigned		motion1			:1;
	unsigned		motion2			:1;
	unsigned		motion3			:1;
	unsigned		motion4			:1;
	unsigned		motion5			:1;
	unsigned		motion6			:1;
	unsigned		motion7			:1;
	unsigned		motion8			:1;
	unsigned		reserve1		:13;

	
};
#endif
//��ѯ����ʱ�õ��Ľṹ
struct query_index_struct{
	struct gt_time_struct start;
	struct gt_time_struct stop;
};

//��ѯFTP�ʺ���������ݽṹ
struct query_ftp_struct{
	  WORD		type;		//���ͣ�0x1: ��������¼��0x2����־; 0x3: �����ļ���
	  WORD		reserve;	//����
  	  DWORD 	client_ip;	//�ͻ�����IP��ַ
	  BYTE		reserve2[12];	//Ϊipv6�������ֶ�
};




#define	SIZEOF_FTP_INFO_STRUCT	sizeof(struct ftp_info_struct)
struct ftp_info_struct{		//����ftp�ʺ��õĽṹ
	WORD		result;		//������� 0:�ɹ� 
							//����ֵ��ʾ���󣬴������������Ŷ��塷�е�[ǰ���豸����Ĵ���]
							//ERR_ENC_NOT_ALLOW���豸����������
	WORD		reserve;		//����
  	BYTE		user[12];		//FTP�û���
  	BYTE		password[12];//FTP�û�����
	DWORD		expired_time;	//  �û����������ʱ�䣨����Ϊ��λ��
	DWORD		ftp_port;		//ftp����˿ں�
};
//����������������Ľṹ
struct index_return_struct{
  	WORD		result;		//������� 0:�ɹ�  
							//�鲻��ʱ����ERR_DVC_NO_RECORD_INDEX
							//����ֵ��ʾ���󣬴������������Ŷ��塷�е�[ǰ���豸����Ĵ���]
							//ERR_ENC_NOT_ALLOW���豸����������
	 WORD		format;		//������ʽ 0��ԭ�� 1��gzipѹ�� Ŀǰֻ֧��ԭ�ķ�ʽ
	 WORD		reserve;	//����
	 char		indexname[2]; //	�����ļ���

};
struct local_record_set_struct{
		WORD save_flag;		//	�Ƿ����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��0-���棬1-����
		WORD pre_rec;		// 	Ԥ¼ʱ�䣬����Ϊ��λ,���ǰ���豸�Ŀռ�ﲻ���趨��ֵ����ǰ���Զ�������Ԥ¼ʱ�䡣
		WORD dly_rec;		// 	��ʱ¼��ʱ�䣬����Ϊ��λ�����ǰ���豸�Ŀռ�ﲻ���趨��ֵ����ǰ���Զ��趨�������ʱ¼��ʱ�䡣
		WORD file_len;		// 	�趨��������¼���ļ���󳤶ȣ�����Ϊ��λ�����������¼���ʱ���ļ������˴˳��ȣ����Զ������и
};
//Զ�̼�������Ƹ�����¼��Ľṹ
struct local_record_ctl_struct{
	WORD	save_flag;	//	��������������������
	WORD	mode;		//	0ֹͣ������¼��
						// 1����������¼��
	WORD	rec_ch;		//��λ��ʾ����Ҫ���и�����¼���ͨ��������2·ѹ����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� �˲�����Ч������5·ѹ����ϵͳ�˲�����Ч��1��ʾ��Ҫ¼��0��ʾ����Ҫ¼����0x0003��ʾͨ��0��1��Ҫ������¼������ͨ������Ҫ������¼��
	WORD	rec_time;	//������¼���ʱ�䣬����Ϊ��λ����mode=1ʱ���ֶ���Ч��rec_time=0��ʾ¼��65535�룬��ֱ���յ�ֹͣ�Ժ�ʵ����������Ƶ�������洢�ռ����ޣ������������Ƶ�¼������rec_time=0Ҫ���ã����ռ䲻��ʱ��ǰ���豸���Զ���������ļ�¼��
};

struct update_software_struct{
   							//user��������������ṹ
    WORD    	type; 			//����,0ΪӦ�ó���,1Ϊ�ں�ӳ��2Ϊramdisk	//�Ѿ�û����
    WORD    	reset_flag;		//(0Ϊ�������̣�1Ϊ�����豸)				//�Ѿ�û����
    BYTE    	userid[12];		//�û���
    BYTE   	 password[12];	//����
    DWORD   	FTPip;			//FTP��������ַ
    BYTE    	reserve[12];		//Ϊipv6�������ֶ�
    WORD    	ftpport;			//FTP�������˿ں�
    WORD	  	reserve1;
    DWORD   	filesize;			 //�����ļ���С
    BYTE    	filepath[50]; 		//�ļ�·��
};

struct update_direct_struct{		//UPDATE_SOFTWARE_DIRECT
					///user������ֱ������������ṹ
	DWORD	type;			///�����ļ������ͣ�������д0
	DWORD	filesize;		//�����ļ���С��������ŵ��ļ����ݴ�С��
};


struct usr_run_hd_test{	//����Ӳ�̲���
	WORD	save_flag; 		//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��ӦΪ0-����
	WORD	diskno; 		//Ҫ���Ե�Ӳ�̱�ţ�0��1��2�����ȣ���Ӳ��ʱ����
	WORD	testtype;		//�������ͣ�0-�̲��ԣ�1-�����ԣ�2-�̲��Խ��������ͨ�������г�����	
	WORD	reserved1;		//����	
};


//user������������Ӧ�ṹ
struct usr_cmd_ack_struct{
	WORD	result;			//ִ�н��
	WORD	rec_cmd;		//��Ӧ������
};

struct user_upd_ack_struct{
    BYTE    dev_id[8]; //�豸id
    WORD    state;    //������
    WORD    reserve;  
    BYTE    info[16];   //��Ϣ
};

typedef struct {
	BYTE	dev_id[8]; 		//�豸id
  	WORD	result;			//������� 0:�ɹ� ����ֵΪ����
  							//�������������Ŷ��塷�е�[ǰ���豸����Ĵ���],��ERR_DVC_NO_DISK
	WORD	approxtime;		//Ԥ����Ҫ��ʱ�䣬����Ϊ��λ
	DWORD  	reserve1;		//����
}format_disk_answer_struct;	  

struct usr_lock_file_time_struct 
{
	WORD	mode;// 1��ʾ���ļ� 0��ʾ�����ļ�
	WORD	lock_ch;			
	//��λ��ʾ����Ҫ���и�����¼���ͨ��������2·ѹ����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� �˲�����Ч������5·ѹ����ϵͳ�˲�����Ч��1��ʾ��Ҫ������0��ʾ����Ҫ�����������
	//bit0:��ʾ�������ͨ������λ��Ч������Ϊ��ͳһ��ű�����ռλ
	//bit1:������¼��ͨ��1(������¼��ͨ������1,2,3,4���б��)
	//bit2:������¼��ͨ��2
	//bit3:������¼��ͨ��3
	//bit4:������¼��ͨ��4
	//��0x0006��ʾͨ��1��2��Ҫ������¼������ͨ������ԭ��״̬���䣬0xfe��ʾ������ͨ�����ļ������������
	struct  gt_time_struct start;//��ʼʱ��
	struct  gt_time_struct stop; //����ʱ��
};


struct user_get_hq_pic_struct{
	WORD  	save_flag; 	//0
	WORD    rec_ch; 
	BYTE    time[8]; 		//ʱ���
	WORD    getpic; 		//����ץ����Ƭ��
	WORD    interval; 	//ʱ����������
};

struct hqpic_answer_struct{
	BYTE    dev_id[8]; 	//�豸id
	WORD    state; 		//���������0Ϊ�ɹ�
	WORD    reserve2;
	BYTE    timeprint[8]; 	//ʱ���
	BYTE    index_file[128];
};

struct takepic_struct{
	int  takepic;
	int  interval;
	int  channel; 			//ץͼͨ����	
//	int  fd;				//remed by shixin
	BYTE time[8];

};

struct return_para_struct{
	BYTE    dev_id[8]; 	//�豸id
  	WORD type;			//    ���ͣ�1����ʾ�д������������ֶ���Ч2:gt1000.ini 3:alarm.ini����ֵ������
	WORD reserve;		//	����
	DWORD  filelen;		//	�ļ�����
	BYTE  file[4];			//	�ļ�����
};
struct send_dev_trig_state_struct {//��������״̬��ʱ�䣬���ڷ��ͱ�����Ҫ
	DWORD       alarmstate; 	//�豸�ı���״̬
	WORD		year;		//����ʱ�䣬��
	BYTE		month;
	BYTE		day;
	BYTE	 	hour;
	BYTE		minute;
	BYTE		second;
	BYTE 		reserve;
};	 

#ifndef _WIN32	
struct thread_struct{
     pthread_t thread_tid;      /* thread ID */
     long    thread_count;      /* # connections handled */
 };
#endif //_WIN32

struct usr_start_alarm_actions_yes_struct //�û�ȷ�ϱ����ṹ
{
	WORD save_flag;
	WORD reserve1;
	DWORD trig;
};

struct usr_start_alarm_actions_struct //�û����Ʊ��������ṹ
{
	WORD save_flag;
	WORD event;
};

struct usr_set_alarm_schedule_struct	//���������趨������Чʱ��νṹ
{
	WORD save_flag;
	BYTE alarm_type;	//��ʾ��Ҫ���õı������ͣ�0Ϊ���ӱ�����1Ϊ�ƶ���ⱨ��
	BYTE setalarm;		//��ʾ�����򳷷���1Ϊ������0Ϊ����
	WORD channel;		//��alarm_typeΪ0ʱ����ʾ�������0,1,2,3,4,5..��alarm_typeΪ1ʱ����ʾ��Ƶͨ��0,1,2,3..
	WORD reserved;
	WORD begin_hour;	//��ʼ��ʱ  ��18
	WORD begin_min;		//��ʼ�ķ�  ��30   ��ʾ18:30
	WORD end_hour;		//������ʱ
	WORD end_min;		//�����ķ�
};
struct usr_set_motion_para_struct   //���������˶��������Ľṹ
{ 
	WORD save_flag;
	WORD channel;		//ͨ��
	WORD sen;	  		//������
	WORD alarm;   		//�Ƿ񱨾���1Ϊ����
	WORD beginhour;
	WORD beginmin;
	WORD endhour;
	WORD endmin;  		//ʱ���
	WORD area[12];		//�˶��������
};

#define SIZEOF_DEV_QUERY_ADPARAM_RETURN		(sizeof(struct dev_query_adparam_ret))
//���ڷ����û�ͼ��A/D������ѯ�Ľṹ	//add-20071112
struct dev_query_adparam_ret
{

	BYTE	dev_id[8];	//�豸id			//zw-del-20071113	//zw-add-20071116
	WORD	result;		//������� 
						//0:		   �ɹ�
						//����ֵ��ʾ�����磺  
						//ERR_ENC_NOT_ALLOW: �豸����������
						//�����������������Ŷ��塷�е�[ǰ���豸����Ĵ���]

	WORD	channel;		//ͼ��ͨ����,0,1,2,3..	
	WORD	bright;			//����	����Χ0��100����ͬ�������˷�ΧΪ���� 
	WORD	hue;			//ɫ��	��
	WORD	contrast;		//�Աȶ�	�� 
	WORD	saturation;		//���Ͷ�  �� 
};


//user�����Ĳ�ѯA/D���������ݽṹ	//add-20071112
struct usr_query_adparam
{
	WORD	save_flag;		//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��ӦΪ0-����
	WORD	channel;		//Ҫ��ѯ��ͼ��ͨ����0��1��2��3��
};

struct usr_req_position_struct
{
	//�������豸gpsλ����Ϣ������ṹ
	WORD	enable;				//��ʼ���ͻ���ֹͣ����1:��ʼ���� 0:ֹͣ����
	WORD	target;				//���豸�Ѷ�λ��Ϣ���͵�����,0:�����ӵ����� ����������,enable=1ʱ��Ч
	WORD	interval;			//����λ����Ϣ�ļ������λ���룩��enable=1ʱ��Ч
	WORD	format;				//λ����Ϣ��ʽ,0,����ֵ����
        //2011-06-16 zw-m ��16λ�ĳ�32λ
       DWORD	send_seconds;		//����λ����Ϣ�ĳ���ʱ�䣬��λ���룩����ʱ�����û���յ�ֹͣ�������ٷ���λ����Ϣ��enable=1ʱ��Ч��
};
#define SIZEOF_DEV_QUERY_TRIG_RETURN		(sizeof(struct dev_query_trig_return))
struct dev_query_trig_return		//�����豸����״̬���仯add-20071113
{
	BYTE	dev_id[8];			//�豸id		//zw-del-20071115 //zw-add-20071116
	WORD	result;				//������� 
				   				//0���ɹ�
				   				//����ֵ��ʾ�����������������Ŷ��塷�е�[ǰ���豸����Ĵ���]
	WORD	reserve1;			//����
	DWORD	alarmin;			//��ǰ�Ķ�������״̬����λ��ʾ��Ŀǰ0~15λ��Ч��1��ʾ�д�����0��ʾû�д�����
	DWORD 	changed_info_len;	//�˿ڱ仯��Ϣ�ĳ���
	BYTE 	changed_info[4];    //�˿ڱ仯��Ϣ������Ϣ��XML��ʽ����, ��ʽΪ��
								/*
								<changed_trigs>
								<trig>
								<id> �˿ں� </id>
								<state>��ǰ״̬��1��0</state>
								<time>�����仯��ʱ�䣬��ʽΪyyyy-MM-dd hh:mm:ss</time>
								</trig>
								</changed_trigs>
								����changed_trigs���԰������trig
								*/
};

//-->zwadd-20090714
#define SIZEOF_DEV_QUERY_STA_RETURN		(sizeof(struct dev_query_sta_return))
struct dev_query_sta_return
{
        BYTE devid[8];				//�豸id
        DWORD p_sta;				//�豸��������״̬
        DWORD d_sta;				//�豸����״̬
};
//<--



struct 	usr_query_hd_status		//user�����Ĳ�ѯӲ��״̬	//add-20071112
{
	WORD	save_flag;		//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��ӦΪ0-����
	WORD	diskno;			//Ҫ��ѯ��Ӳ�̱�ţ�0��1��2�����ȣ���Ӳ��ʱ����	
};


#define	SIZEOF_DEV_QUERY_HDSTATUS_RETURN	(sizeof(struct dev_query_hdstatus_return ))
struct  dev_query_hdstatus_return		//����Ӳ��״̬������ѯ	add-20071113
{
	BYTE	dev_id[8];			//�豸id		//zw-del-20071115	//zw-add-20071116
	WORD	result;				//������� 
				   				//0:		  �ɹ�
				   				//����ֵ��ʾ�����磺  
				   				//ERR_EVC_NOT_SUPPORT: �豸��ָ��Ӳ��
				   				//�����������������Ŷ��塷�е�[ǰ���豸����Ĵ���]
	WORD	diskno;			// Ӳ�̱��,0��1,2�����ȣ���Ӳ��ʱ��
	BYTE	model[16];		//Ӳ���ͺţ��ַ���    
	BYTE	serialno[16];		//Ӳ�����кţ��ַ���
	BYTE	firmware[8];		//�̼��汾�ţ��ַ���
	WORD	volume;			//����(GΪ��λ����250G��320G)
	WORD	temprature;		//��ǰ�¶�(����)
	WORD	maxtemprature;	//��ʷ����¶�(����),ֵ��100������Ч
	WORD	reserve1;		//����
	DWORD	age;			//����Сʱ��
	DWORD	relocate;		//�ط���������
	DWORD	pending;		//��ǰ����������
	DWORD	error_no;		//������־��
	WORD	shorttest;		//��һ�ζ̲��Խ����0ͨ����1ʧ��,2��������δ���Թ�,3������
	WORD	shortstatus;	//�̲������ڽ����У���ɵİٷֱȣ�0-100������	
	WORD	longtest;		//��һ�γ����Խ����0ͨ����1ʧ��,2��������δ���Թ�,3������
	WORD	longstatus;		//���������ڽ����У���ɵİٷֱȣ�0-100������	
};


struct usr_query_regist		//usr  ������ע���ѯ		//add-20071113
{
	WORD	save_flag;		//����ǰ��Ƕ��ʽ��Ƶ��������DEVICE�� Ӳ����־��ӦΪ0-����	
	WORD	reserve1;		//����
};


struct usr_login_device
{//�û���¼�豸�Ľṹ
	BYTE	username[200];		///�û���,��¼�豸���û������û���Ϣ�ַ���
	BYTE	passwd[32];		///����,��¼�豸������(��ʱ����)
};



struct dev_login_return
{
	BYTE	dev_id[8];		//�豸id
	WORD	result;			//������� 
	WORD	reserved;		//����
	DWORD	login_magic;		//�豸������˴ε�¼�������
};

struct dev_ip_return
{
	BYTE	dev_id[8];		//�豸id
	WORD	result;			//������� 
	WORD	reserved1;		//����
	DWORD	client_ip;		//�ͻ���ip
	BYTE	reserved2[12];		//����
	DWORD	device_ip;		//ǰ���豸ip��ַ
	BYTE	reserved3[12];		//Ϊipv6�������ֶ�
	
};

struct dev_position_return_struct{
	//�����豸��gpsλ����Ϣ
	BYTE	dev_id[8];			//�豸id
	WORD	state;				//״̬��0������ 1����gps�ź� 2�����ڶ�λ 3:����
	BYTE	reserved[6];			//����
	double	lon;				//����
	double  lat;				//γ��
	double  direction;			//��λ��  
	double	altitude;			//���θ߶�
	double	speed;				//�ٶ�(km/h)   
};
/**********************************************************************************************
 * ������	:get_gtcmd_name()
 * ����	:��������ͨѶ��������ת��Ϊ�ַ�������
 * ����	:cmd:Ҫת����������
 * ����ֵ	:�����ֵ������ַ���
 **********************************************************************************************/
#ifdef _WIN32
static  char * get_gtcmd_name(WORD cmd)
#else
static __inline__ char * get_gtcmd_name(WORD cmd)
#endif
{
	switch(cmd)
	{
//PC������������
		case USR_REQUIRE_RT_IMAGE:
			return "USR_REQUIRE_RT_IMAGE";
		break;
		case USR_REQUIRE_SPEAK:
			return "USR_REQUIRE_SPEAK";
		break;
		case USR_SET_AUTH_IP:
			return "USR_SET_AUTH_IP";
		break;
		case USR_CLOCK_SETING:
			return "USR_CLOCK_SETING";
		break;
		case USR_SCREEN_SETING:
			return "USR_SCREEN_SETING";
		break;
		case USR_NET_STREAM_SETING:
			return "USR_NET_STREAM_SETING";
		break;
		case USR_LOCAL_STREAM_SETING:
			return "USR_LOCAL_STREAM_SETING";
		break;
		case USR_SET_SENSER_ARRAY:
			return "USR_SET_SENSER_ARRAY";
		break;
		case USR_SET_VIDEO_AD_PARAM:
			return "USR_SET_VIDEO_AD_PARAM";
		break;
		case USR_SET_SWITCH_IN:
			return "USR_SET_SWITCH_IN";
		break;
		case USR_SET_SWITCH_OUT:
			return "USR_SET_SWITCH_OUT";
		break;
		case USR_SWITCH_OUT:
			return "USR_SWITCH_OUT";
		break;
		case USR_LOCAL_RECORDER_CONTROL:
			return "USR_LOCAL_RECORDER_CONTROL";
		break;
		case USR_LOCAL_RECORDER_SETING:
			return "USR_LOCAL_RECORDER_SETING";
		break;
		case USR_SEIRAL_PORT_CONTROL:
			return "USR_SEIRAL_PORT_CONTROL";
		break;
		case USR_CANCEL_ALARM:
			return "USR_CANCEL_ALARM";
		break;
		case USR_QUERY_INDEX:
			return "USR_QUERY_INDEX";
		break;
		case USR_QUERY_FTP:
			return "USR_QUERY_FTP";
		break;

		case USR_QUERY_STATE:
			return "USR_QUERY_STATE";
		break;
		case USR_CMD_ACK:
			return "USR_CMD_ACK";
		break;
		case USER_UPDATE:
			return "USER_UPDATE";
		break;
		case UPDATE_SOFTWARE_DIRECT:
			return "UPDATE_SOFTWARE_DIRECT";
		break;
		case USR_TAKE_HQ_PIC:
			return "USR_TAKE_HQ_PIC";
		break;
		case USR_RW_DEV_PARA:
			return "USR_RW_DEV_PARA";
		break;
		case USR_LOCK_FILE_TIME:
			return "USR_LOCK_FILE_TIME";
		break;
		case USR_QUERY_VIDEO_AD_PARAM:			//add-20071112
			return "USR_QUERY_VIDEO_AD_PARAM";
		break;
		case USR_QUERY_TRIGSTATE:					//add-20071113
			return "USR_QUERY_TRIGSTATE";
		break;
		case USR_QUERY_HD_STATUS:					//add-20071113
			return "USR_QUERY_HD_STATUS";
		break;
		case USR_QUERY_REGIST:						//add-20071113
			return "USR_QUERY_REGIST";
		break;
		case START_ALARM_ACTIONS:
			return "START_ALARM_ACTIONS";
		break;
		case START_ALARM_ACTIONS_YES:
			return "START_ALARM_ACTIONS_YES";
		break;
		

//�豸���͵�����		
		case DEV_REGISTER:
			return "DEV_REGISTER";
		break;
		case DEV_STATE_RETURN:
			return "DEV_STATE_RETURN";
		break;
		case DEV_ALARM_RETURN:
			return "DEV_ALARM_RETURN";
		break;
		case DEV_COM_PORT_RET:
			return "DEV_COM_PORT_RET";
		break;
		case DEV_CMD_ACK:
			return "DEV_CMD_ACK";
		break;
		case USR_QUERY_FTP_ANSWER:
			return "USR_QUERY_FTP_ANSWER";
		break;
		case DEV_RECORD_RETURN:
			return "DEV_RECORD_RETURN";
		break;
		case UPDATE_ANSWER:
			return "UPDATE_ANSWER";
		break;
		case HQ_PIC_ANSWER:
			return "HQ_PIC_ANSWER";
		break;
		case DEV_PARA_RETURN:
			return "DEV_PARA_RETURN";
		break;
		case DEV_REQ_SYNC_TIME:
			return "DEV_REQ_SYNC_TIME";
		break;
		case DEV_QUERY_ADPARAM_RETURN:			//add-20071113
			return "DEV_QUERY_ADPARAM_RETURN";
		break;
		case DEV_QUERY_TRIG_RETURN:				//add-20071113
			return "DEV_QUERY_TRIG_RETURN";
		break;
		case DEV_QUERY_HDSTATUS_RETURN :			//add-20071113
			return "DEV_QUERY_HDSTATUS_RETURN";
		break;
		case DEV_POSITION_RETURN :			//add-20100128
			return "DEV_POSITION_RETURN";
		break;
		
		case USR_RUN_HD_TEST:
			return "USR_RUN_HD_TEST";
		break;
			
		
	}
	return "UNKNOW_CMD";
}
		




#endif




