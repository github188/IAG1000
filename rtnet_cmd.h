#ifndef RTNET_CMD_H
#define RTNET_CMD_H

#define	VIEWER_SUBSCRIBE_D1_VIDEO		0x0600	//����D1ͨ������Ƶ(������)
#define	VIEWER_SUBSCRIBE_VIDEO			0x0601	//������Ƶ
#define   VIEWER_SUBSCRIBE_VIDEO_ANSWER   0x0602	//�豸�Զ�����Ƶ�ķ���
#define   VIEWER_UNSUBSCRIBE_VIDEO			0x0603	//�˶���Ƶ
#define 	VIEWER_SUBSCRIBE_AUDIO			0x0604	//������Ƶ����ͨ��
#define	VIEWER_SUBSCRIBE_ANSWER_AUDIO	0x0605	//��Ƶ���ж��������
#define   VIEWER_SUBSCRIBE_AUDIO_START	0x0606	//��ʼ��Ƶ���д���


//�������ϴ�����Ƶ������ṹ
#define AV_TCP		1
#define AV_UDP    	2
#define AV_RTP_UDP	3
#define AV_QOS_UDP	4

struct viewer_subscribe_cmd_struct{//����Ƶ���Ͷ���
	DWORD	devip;			//�豸ip��ַ
	BYTE 	reserve[12];		//����for ipv6
	BYTE	dev_id[8];		//ǰ���豸id
	DWORD	gateip;			//����ip
	BYTE	reserve1[12];	//����
	BYTE	token[20];		//�������ƣ��ַ���
	WORD	user_level;		//�û����ʼ���
	BYTE	account[20];		//�û��ʺ�
	WORD	protocal;		//����Ƶ���Ĵ���Э��
	WORD	timeout;			//����ؼ����ӱ��ϣ�ת����������Ҫ�ȴ���ʱ�䣬����Ϊ��λ���������Ϊ0����ʹ��ת���������Զ���ĳ�ʱʱ�䣩
	WORD	bandwidth;		//�û����������KΪ��λ(���ֶ�Ԥ����������趨�ɽ�����Ϊ0)
	DWORD   device_port;		//�豸��Ƶ����˿�
	DWORD   audio_flag;		//�Ƿ�����Ƶ��־0x55aa��ʾ��Ҫ��Ƶ,1��ʾҪ��Ƶ	
	DWORD    channel;          //�û����ӱ��������
	
};
//GT1000��Ӧ����Ƶ��������Ľṹ
struct viewer_subscribe_cmd_answer_struct{
	BYTE	dev_id[8];			//ǰ���豸��id
	WORD	result;				//VIEWER_SUBSCRIBE_VIDEO�����ִ�н����0��ʾ�ɹ�������ֵ��ʾ����
	BYTE	video_trans_id[4];	//��ת����������̬���ɵĴ�������ID�ţ��粻�ɹ�Ϊ0��
	WORD	answer_data_len;	//������Ϣanswer_data�ĳ��ȣ��ɹ�ʱ���ص�ʱAVI�ļ�ͷ��ʧ�ܷ��ش�����Ϣ��
	BYTE	answer_data[4];		//������Ϣ
};


#define AUDIO_PLAY_TYPE_MP3			0
#define AUDIO_PLAY_TYPE_PCM			1
#define AUDIO_PLAY_TYPE_ADPCM			2
#define AUDIO_PLAY_TYPE_MP2			4
#define AUDIO_PLAY_TYPE_UPCM			5
#define AUDIO_PLAY_TYPE_AAC				6 //for ip device
#define AUDIO_PLAY_TYPE_UNKNOW			255
static  char *get_audio_fmt_str(int type)
{
	switch(type)
	{
		case AUDIO_PLAY_TYPE_UPCM:
			return "u-pcm";
		break;
		case AUDIO_PLAY_TYPE_PCM:
			return "raw-pcm";
		case AUDIO_PLAY_TYPE_AAC:
			return "AAC";

		break;
	}
	return "unknow";
}
//lc 2014-1-2
struct viewer_subscribe_audio_cmd_struct
{//��Ƶ���ж���
	DWORD		device_ip;			//	ǰ���豸ip��ַ
	BYTE		reserve2[12];		//	Ϊipv6�������ֶ�
	DWORD		device_port;			// �豸��Ƶ����˿�
	BYTE		dev_id[8];		       //	ǰ���豸GUID
	DWORD		gateway_ip;			//	����IP��ַ
	BYTE		reserve3[12];		//	Ϊipv6�������ֶ�
	BYTE		token[20];			//	�������ƣ��ַ�����
	WORD		user_level;			//	�û����ʼ���
	BYTE		account[20];	              //	�û��ʺ�
	WORD		trans_protocal;   	//	����Э�飨�μ�����"����Э�鶨��"��
	WORD		timeout;				//	����ؼ����ӱ��ϣ�ת����������Ҫ�ȴ���ʱ�䣬����Ϊ��λ���������Ϊ0����ʹ��ת���������Զ���ĳ�ʱʱ�䣩
	WORD		bandwidth;			//	�û����������KΪ��λ(���ֶ�Ԥ����������趨�ɽ�����Ϊ0)
	WORD		audiotype;			//	0-mp3; 1-pcm; 2-adpcm; 4-mp2 ,5-ulaw-pcm,255-δ֪
    WORD		audioheader_datalen;//��Ƶ�ļ�ͷ�ĳ��ȣ�
	BYTE		audioheader_data[4];//audioheader_datalen��	��Ƶ�ļ�ͷ��Ϣ��
	WORD        audio_channel;		//��Ƶ����ͨ���ţ���ָ��������
};



struct viewer_subscribe_answer_audio_struct
{//�����������������
	//Ӧ�ü�guid
	BYTE	dev_id[8];
	WORD	status;				//	���ز���״̬��0��ʾ�ɹ�����0��ʾ����ţ�	
	BYTE	audio_trans_id[4];	//	��ת����������̬���ɵĴ�������ID�ţ��粻�ɹ�Ϊ0��
	WORD	answer_data_len;	//	������Ϣanswer_data�ĳ��ȣ��ɹ�ʱ����"success"��ʧ�ܷ��ش�����Ϣ��
	BYTE	answer_data[2];			//(answer_data_len)	������Ϣ
};




/*

//ʵʱͼ��������Ĵ�����

#define RT_RESULT_SUCCESS		0		//�ɹ�
#define ERR_DVC_INTERNAL		0x1001	//�豸æ
#define ERR_DVC_INVALID_REQ	0x1002	//�ͻ��������ݸ�ʽ��
#define ERR_DVC_BUSY			0X1003	//�豸����

*/


#endif




