//GT1000ϵͳ�ڲ�ͨѶ��������
#ifndef MOD_CMD_H
#define MOD_CMD_H
#include <typedefine.h>
#include "time.h"
#include "gate_cmd.h"
#include <stdio.h>



#ifndef DWORD
#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned long
#endif
/******************************************
	������ID����
******************************************/

#define		ALL_PROCESS		0           ///<�㲥��ַ
#define		MAIN_PROCESS_ID		1
//#define 	UPDATE_PROCESS_ID		2				//�Ѿ�����������
#define		RTIMAGE_PROCESS_ID	3		
#define		PLAYBACK_PROCESS_ID	    4				//�ط�ģ��ID
#define		HQSAVE_PROCESS_ID		5	
#define		PPPOE_WATCH_ID		6				//����adsl���ŵĽ���
#define		DISKMAN_ID				7				//���̹������
#define		KEYBOARD_ID			8				//�ⲿ���̴���
#define		ALARM_MOD_ID			9				//��������ģ��
#define		VIDEOENC_MOD_ID		10				//��Ƶ���ݲɼ�ģ��
#define          HW_DIAG_MOD_ID            11                        //Ӳ���������ģ��
#define		UPNPD_MOD_ID				12				//upnp�˿�ӳ��ģ��

#define 		HDBUF_PROCESS_ID		13				//Ӳ�̻�����ģ�� zw-add 2011-11-23
#define		CGI_MAIN_ID					14             //cgimain����ģ��
//


/************************************************************************************
	�����̷��͸��������̵�����
************************************************************************************/
//�����̲�ѯ�������̵�״̬,�������ʲô����Ҳû��
#define	MAIN_QUERY_STATE			0x1001



/*�����̽���������״̬���͸�������¼��ģ�飬������¼��ģ��
 *Ӧ������Ԥ�����õĲ�������֮���д���(¼��)
 *��ʽ:len(2)+SEND_TRIG_STATE(2)+state(4)
 *state�ľ��庬���������̶���
*/
#define	SEND_TRIG_STATE			0x1002

/*
	����������������ˢ�²���������
	�������û�в���
*/
#define	MAIN_REFRESH_PARA		0x1003

//zw-add -2011-08-05
//�����ط�ʹ�ø�tcprtimage
#define  MAIN_PLAYBACK_IMG_CMD			0x1004

//zw-add 2011-08-05
//���͸�videoenc,׼��ֹͣ���¹�������е���Ƶ֡����
//�ڱ�����������ʱ30�룬Ȼ��ֹͣ���£�ֱ��¼��ط���Ϻ���Ÿ��»���ء�
#define MAIN_PLAYBACK_ENC_CMD				0x1005

typedef struct
{
	int offset;		//֡ƫ����
}hddbuf_offset_t;

//zw-add 2011-11-30
//��Ҫ��gtvs1000ʹ�ã���tcprtimg2���ͻ�ȡ��ǰ��ȡӲ�̻�����Ϊλ�÷��͸�vsmain,��ת����hdrecd
#define MAIN_PLAYBACK_GETPOC_CMD				(0x1006)
#define MAIN_PLAYBACK_GETPOC_ANSWER_CMD	(0x1007) //��Ҫ����һ��int����

#define MAIN_REQUEST_APLAY (0x1008)

//zw-add 2011-08-17
//���͸�hdmodule�õ�����
//ʹ��MAIN_PALYBACK_ENC_CMD
/**********************************************************************************
	�������̷��͸������̵�����
***********************************************************************************/
//�����tcprtimg����vsmain�������Ҫvsmain��ת����videoenc��������¼��طŽ�����
//���Լ������¹����ڴ���
#define RTIMG_PLAYBACK_STOP_CMD					0x2a01

//rtimage��ipmain���������֪����ͨ����
#define RTIMG_AUDIODOWN_CMD                       0x2a05




/*
	rtimage��videoenc����i֡
*/
#define REQUIRE_IFRAME                                               0x2a02
#define PROBE_DEVICE												 0x2a06
#define PROBE_ACK											 0x2b06

#define SET_DEVICE_IP                                                0x2a11
#define SET_DEVICE_IP_ACK                                            0x2b11


#define SET_DEVICE_MASK                                              0x2a12
#define SET_DEVICE_MASK_ACK                                          0x2b12

#define SET_RATE                                                     0x2a13
#define SET_RATE_ACK												 0x2b13

#define SET_PRICSION                                                 0x2a14
#define SET_PRICSION_ACK                                             0x2b14

#define SET_CHANNEL                                                  0x2a15
#define SET_CHANNEL_ACK                                              0x2b15

#define SET_ENV                                                      0x2a16
#define SET_ENV_ACK													 0x2b16

#define GET_DEVICE_IP                                                0x3a11
#define GET_DEVICE_IP_ACK                                            0x3b11

#define GET_DEVICE_MASK                                              0x3a12
#define GET_DEVICE_MASK_ACK                                          0x3b12

#define GET_RATE                                                     0x3a13
#define GET_RATE_ACK												 0x3b13

#define GET_PRICSION                                                 0x3a14
#define GET_PRICSION_ACK                                             0x3b14

#define GET_CHANNEL                                                  0x3a15
#define GET_CHANNEL_ACK                                              0x3b15

#define GET_ENV                                                      0x3a16
#define GET_ENV_ACK													 0x3b16

#define REQUIRE_UP_AUDIO											 0x3a17	
#define REQUIRE_UP_ADUIO_ACK										 0x3b17

#define STOP_UP_AUDIO												 0x3a18	
#define STOP_UP_ADUIO_ACK											 0x3b18

#define SEND_DOWN_AUDIO												 0x3a19	
#define SEND_DOWN_ADUIO_ACK											 0x3b19

#define STOP_DOWN_AUDIO												 0x3a20	
#define STOP_DOWN_ADUIO_ACK											 0x3b20

#define QUERY_STATUS												 0x3a21
#define QUERY_STATUS_ACK											 0x3b21	









/*
     ���̴���ģ�鷢�͸������̵�״̬��Ϣ
*/
#define KEYBOARD_STAT_RETURN		0x6001	//����״̬����
struct keyboard_state_struct{
	unsigned		mod_state		:1;	//ģ��״̬0��ʾ���� 1��ʾ�쳣,��λ����ģ������
	unsigned		connect_fail		:1;	//�����ⲿ���̴���0��ʾ���� 1��ʾ����
	unsigned		reserve			:30; //����
};
#define	KEYBOARD_ALARM_SET			0x6002	//����ģ�鷢�͵��ⴥ������������Ϣ
struct keyboard_alarm_set_struct{
	int mode;		//0 ��ʾ�ⲿ����  1��ʾ�ƶ�����
	int channel;		// �ⴥ�����ƶ�����ͨ����0,1,2...  -1��ʾ�����ⴥ��ͨ�����ƶ�����ͨ��
	int flag;			// 1��ʾ���� 0��ʾ����
};










/*������������ģ���������̷����Լ���״̬����־λ������������ģ��ȷ��,0��ʾ������1��ʾ�쳣
 *����������ģ���Լ�����״̬�仯ʱ���ʹ�������ߵ����յ������̷�����MAIN_QUERY_STATE����ʱ���ʹ����� 
 *��ʽ:len(2)+AUDIO_STATE_RETURN(2)+pid(pid_t)+state(4)
 *pid:������Ƶ����ģ��Ľ���id����pid_t���͵����ݣ�����Ϊsizeof(pid_t)
 *state:������Ƶ����ģ���״̬�����ȫ0��ʾһ������������λ������������ģ��ȷ��
 */
#define AUDIO_STATE_RETURN	0x0201

/*ʵʱ����Ƶ����ģ���������̷����Լ���״̬,״̬��־λ��������ʵʱͼ��ģ��ȷ��,0��ʾ������1��ʾ�쳣
 *��ʵʱͼ��ģ���Լ�����״̬�仯ʱ���ʹ�������ߵ����յ������̷�����MAIN_QUERY_STATE����ʱ���ʹ����� 
 *��ʽ:len(2)+RTSTREAM_STATE_RETURN(2)+pid(pid_t)+state(4)
 *pid:ʵʱͼ��ģ��Ľ���id����pid_t���͵����ݣ�����Ϊsizeof(pid_t)
 *state:ʵʱͼ��ģ���״̬�����ȫ0��ʾһ������������λ��������ʵʱͼ��ģ��ȷ��
 */
#define RTSTREAM_STATE_RETURN		0x0301
struct rtimage_state_struct{
	unsigned		mod_state		:1;	//ģ��״̬0��ʾ���� 1��ʾ�쳣,��λ����ģ������
	unsigned		net_enc_err		:1;	//�����ѹ��оƬ����
	unsigned 		net_enc_busy	:1;  //�������оƬæ��־
	unsigned 		test_d1_flag		:1; //����d1ͨ����־
	unsigned		reserve			:28;
};


/*
	������֪ͨʵʱͼ������л���Ƶͨ��������
	��ʽ:len(2)+SET_SCREEN_DISPLAY(2)+display_type(2)+channel(2)
*/

typedef struct{
	WORD	channel;		//��ʾȫ��ʱ����Ƶͨ���ţ�ȡֵ0,1,2...
	BYTE    peeruser[20];
	BYTE    peeraddr[16];
}set_scr_display_struct;


#define SET_SCREEN_DISPLAY	0x1301

/**********************************************************************************
 *	������¼������������
***********************************************************************************/

/*
	����¼��ģ�����¼��(ֻ�ǿ���¼��ģʽ�����������ļ���)
	��ʽlen(2)+CTRL_HDRECORD(2)+channel(4)+mode(4)
	channel:��Ҫ���Ƶ�ͨ����0,1,2,3
	mode:	0: ��ʾֹͣ¼���߳�
			1: ��ʾ����¼���߳�
			2: ��ʾ��������¼���߳�
*/
struct hdrecord_ctrl_struct{
	DWORD channel;
	DWORD mode;
};
#define HDRECORD_CTRL	0x1401

/*
	����һ�θ�����¼��(���ر������¼�����)
	��ʽ len(2)+TRIG_RECORD_EVENT(2)+channel(4)+trig_flag(4)+reclen(4)
	channel:��Ҫ����¼���ѹ��ͨ����
	trig_flag:������־(��Ҫ�����ļ���)
	reclen:��Ҫ����¼���ʱ��
*/
struct trig_record_event_struct{
	DWORD	channel;
	DWORD	trig_flag;
	DWORD	reclen;
};
#define TRIG_RECORD_EVENT 	0x1402

/*
	Զ�̼���������Ĵ���¼������
	��ʽ len(2)+REMOTE_TRIG_RECORD(2)+channel(4)+mode(4)+reclen(4)
	channel:��Ҫ����¼���ѹ��ͨ����
	mode: 0��ʾֹͣ¼��
		   1��ʾ����¼��
	reclen:��Ҫ¼��ĳ���(����ֹͣ����������)	
	
*/
struct remote_trig_record_struct{
	DWORD	channel;//¼��ͨ��
	DWORD	mode;//1Ϊ¼��0Ϊ��¼
	DWORD	reclen;
};
#define REMOTE_TRIG_RECORD	0x1403

/*
	������ļ�������
	�޲���
*/
#define UNLOCK_FILE		0x1404

/*
	���������־(�˳�����¼��״̬)
	DWORD	channel;//¼��ͨ��
*/
#define CLEAR_TRIG_FLAG	0x1405

/*
	��ʱ��ӽ���
	��ʽ len(2)+USR_LOCK_FILE_TIME(2)+lockfile(20)
	lockfile: struct usr_lock_file_time_struct
*/

#define LOCK_FILE_TIME	0x1406

/*�����󷢸�hdmoduleģ�����ץͼ������
 *��ʽ:len(2)+ALARM_SNAPSHOT(2)+takepic(20)
 *takepic:��takepic_struct�ṹ�����ݣ�����ʱ����������������ͨ����
*/
#define ALARM_SNAPSHOT     0x1407
	
/*����hdmoduleģ����в�ѯ����������
 *��ʽ:QUERY_INDEX(2)+len(2)+channel(4)+query_index(20)
 *queryindex:��query_index_struct�ṹ������(��gate_cmd.h�ж���)��������ʼ�ͽ���ʱ��
*/
#define QUERY_INDEX			0x1408

struct query_index_with_channel{
DWORD						channel;	//Ҫ��ѯ��¼��ͨ���ţ�-1��ʾȫ��ͨ��
struct 	query_index_struct	queryindex;
};

	
	
/*������ͼ���¼ģ���������̷����Լ���״̬��״̬��־λ�������ɸ������洢ģ��ȷ��
 * ��������ͼ���¼ģ�鷢���Լ�״̬�ı���������̷����˲�ѯ״̬������ʹ�����
 * ��ʽ:len(2)+HDMODE_STATE_RETURN(2)+pid(pid_t)+state(4)
 * pid:������ͼ���¼���̵�id����pid_t���͵����ݣ�����Ϊsizeof(pid_t)
 * state:������ͼ��ģ���״̬�����ȫ0��ʾһ������������������ɸ�����¼��ģ��ȷ��
*/
#define HDMOD_STATE_RETURN		0x0401
struct hdmod_state_struct{
	unsigned reserve1		:7;
	unsigned cf_err         :1; //���̹���
	/*unsigned video_enc1_err	:1;	//��0·������¼��ͨ������
	unsigned video_enc2_err	:1;	//��1·������¼��ͨ������
	unsigned video_enc3_err	:1;	//��2·������¼��ͨ������
	unsigned video_enc4_err	:1;	//��3·������¼��ͨ������
	unsigned reserve2		:20;
	*/
	unsigned reserve2		:24; //wsy,���ٴ���ͱ������������
};




/**********************************************************************************
 *	���̹���¼������������
***********************************************************************************/
#define DISKMAN_STATE_RETURN		0x0701
struct diskman_state_struct{
	unsigned cf_err			:1;  //���̹���
	unsigned disk_full			:1;	//��������־
	unsigned reserve			:30;
};



/*��ʱ��μӽ����ļ�
 *������ʽ:
 *lockfile:��usr_lock_file_time_struct�ṹ�����ݣ�
 *         ����ʱ����������������ͨ����
*/
#define LOCK_FILE_BY_TIME	     0x1701



/**********************************************************************************
*	��Ƶ���ݲɼ�ģ���������
***********************************************************************************/
#define VIDEOENC_STATE_RETURN		0x0901
struct videoenc_state_struct{
	unsigned video_enc0_err :1; //��0·��Ƶ����������
	unsigned video_enc1_err	:1;	//��1·��Ƶ����������
	unsigned video_enc2_err	:1;	//��2·��Ƶ����������
	unsigned video_enc3_err	:1;	//��3·��Ƶ����������
	unsigned video_enc4_err	:1;	//��4·��Ƶ����������
	//unsigned video_enc5
	unsigned reserve			:3;		//����
	unsigned video_motion0	:1;	//��0·�ƶ����
	unsigned video_motion1	:1;
	unsigned video_motion2	:1;
	unsigned video_motion3	:1;
//	unsigned reserve1		:4;//11
	unsigned audio_enc0_err :1; //��0·��Ƶ����������
	unsigned audio_enc1_err	:1;	//��1·��Ƶ����������
	unsigned audio_enc2_err	:1;	//��2·��Ƶ����������
	unsigned audio_enc3_err	:1;	//��3·��Ƶ����������
	
	unsigned video_loss0		:1;	//��0·��Ƶ��ʧ
	unsigned video_loss1		:1;	//��1·��Ƶ��ʧ
	unsigned video_loss2		:1;	//��2·��Ƶ��ʧ
	unsigned video_loss3		:1;	//��3·��Ƶ��ʧ
	unsigned reserve2		:4; 
	unsigned video_blind0		:1;	//��0·��Ƶ�ڵ�
	unsigned video_blind1		:1;	//��1·��Ƶ�ڵ�
	unsigned video_blind2		:1;	//��2·��Ƶ�ڵ�
	unsigned video_blind3		:1;	//��3·��Ƶ�ڵ�
	unsigned reserve3		:4; 
	
};

//lc ��ƵVDAģ�����״̬
#define VIDEOENC_VDA_STATE		0x0902
struct videoenc_vda_state_struct{
	unsigned video0_loss   :1;  //��0·ͨ����ʧ
	unsigned video1_loss   :1;  //��1·ͨ����ʧ
	unsigned video2_loss   :1;  //��2·ͨ����ʧ
	unsigned video3_loss   :1;  //��3·ͨ����ʧ
	unsigned video4_loss   :1;  //��4·ͨ����ʧ
	unsigned video5_loss   :1;  //��5·ͨ����ʧ
	unsigned video6_loss   :1;  //��6·ͨ����ʧ
	unsigned video7_loss   :1;  //��7·ͨ����ʧ

	unsigned video0_blind  :1;  //��1·ͨ���ڵ�
	unsigned video1_blind  :1;  //��2·ͨ���ڵ�
	unsigned video2_blind  :1;  //��3·ͨ���ڵ�
	unsigned video3_blind  :1;  //��4·ͨ���ڵ�
	unsigned video4_blind  :1;  //��5·ͨ���ڵ�
	unsigned video5_blind  :1;  //��6·ͨ���ڵ�
	unsigned video6_blind  :1;  //��7·ͨ���ڵ�
	unsigned video7_blind  :1;  //��8·ͨ���ڵ�

	unsigned video0_motion  :1;  //��1·ͨ���ƶ�
	unsigned video1_motion  :1;  //��2·ͨ��motion
	unsigned video2_motion  :1;  //��3·ͨ��motion
	unsigned video3_motion  :1;  //��4·ͨ��motion
	unsigned video4_motion  :1;  //��5·ͨ��motion
	unsigned video5_motion  :1;  //��6·ͨ��motion
	unsigned video6_motion  :1;  //��7·ͨ��motion
	unsigned video7_motion  :1;  //��8·ͨ��motion
};

//lc ��Ƶ��·����״̬
#define VIDEOENC_COAXIAL_ERR		0x0903
struct enc_coaxial_state
{
	unsigned video0_err      :1;   //��1·��ƵԴ����
	unsigned video1_err      :1;
	unsigned video2_err      :1;
	unsigned video3_err      :1;
	unsigned video4_err      :1;
	unsigned video5_err      :1;
	unsigned video6_err      :1;
	unsigned video7_err      :1;
};

//���ؽ��ACK�Ľṹ
struct result_return_struct 
{
	int fd;
	int command;
	int result;
};



/**********************************************************************************
 *	�����ؽ����������
***********************************************************************************/

/*
 *	PPPOE��ؽ��̷��͸�������״̬
 *    ��ʽ:len(2)+HQSAVE_STATE_RETURN(2)+pid(pid_t)+state(4)
 *    pid:PPPOE��ؽ��̵�id����pid_t���͵����ݣ�����Ϊsizeof(pid_t)
 *    state:PPPOE���ģ���״̬��

*/
#define PPPOE_SUCCESS				0	//adsl��������
#define PPPOE_NO_MODEM			1	//�Ҳ���adsl modem
#define PPPOE_PASSWD_ERR			2	//adsl�ʺ��������
#define PPPOE_USR_TWICE			3	//�ʺ��ظ�����
#define PPPOE_USR_INVALID			4	//�ʺ���Ч
#define PPPOE_PAP_FAILED			5	//�ʺ���Ч

#define PPPOE_STATE_RETURN		0x0501


/**********************************************************************************
 *	Ӳ�������̵��������
***********************************************************************************/

#define HW_DIAG_STATE_RETURN        0x0b01  //״̬����
typedef struct {
    unsigned    ide_err                 :1;                    ///���̴���
    unsigned    reserve                :31;

}hw_diag_state_t;
/**********************************************************************************
 *	����ģ���������
 **********************************************************************************/

/*����״̬����(��Ҫָ��������)
   ����������״̬�ı�����״̬�ı�ʱ,����ģ���������ʹ�����
   ���ߵ��յ�MAIN_QUERY_STATE����ʱ���ش�����
*/
#define	ALARM_STATE_RETURN			0x0801	

#ifndef pid_t
#define	pid_t	int	
#endif

typedef struct {
//�������صĽṹ
	pid_t	pid;			//����id
	time_t 	time;		//����ʱ��
	DWORD	state;		//����ģ����Ƶ��豸���״̬�紮�ڵ�
						//��λ��ʾ 1��ʾ�й��� 0��ʾ����
	DWORD	alarm;		//�����ķ���״̬����ʾ32������ 1��ʾ�б��� 0��ʾû�б���
}ALARM_STATE;


//���ñ������
#define SET_ALARM_OUT	 0x1802
typedef struct{
	int	ch;			//����˿ں�0��1��2...
	int	val;			//ֵ 1��ʾ��� 0��ʾ�����
	int	timelong;	//��ʾ��Ҫ���������,0��ʾһֱ���(��ʱû����)
}ALARM_OUT;


/**********************************************************************************
 *	upnpd�˿�ӳ������������
***********************************************************************************/

/*
 *	upnpd�˿�ӳ����̷��͸�������״̬
 *    ��ʽ:len(2)+UPNPD_STATE_RETURN(2)+state(4)
 *    state:UPNPDģ���״̬��

*/
#define UPNPD_SUCCESS				0	//upnpd����ӳ��
#define UPNPD_FAILURE				1   //upnpdӳ��ʧ��
#define UPNPD_STATE_RETURN		0x0c01



/**********************************************************************************
 *	hdplayback�����������
***********************************************************************************/
#define HDPLAYBACK_STATE_RETURN		0x2301
struct hdplayback_state_struct{
	unsigned int    err;         //// 0 ���� ��0�쳣
};









/*
	ת�������������������ģ�齫Զ�����ط������������ֱ��ת������Ӧģ��
	�Լ�����Ӧģ����Ҫ���ظ����ص�����ͳ�ȥ
*/
//ʵ��ת��ʱ����һ���ֶ�Ӧ����4�ֽڵ��ļ�����������ʾ�յ����������
//��ģ�鷢�͵�����
/*��ģ�齫���ط�������Ϣԭ��������ģ���ڲ�ͨѶ����Ϣ���﷢�͸���Ӧģ��*/
/*��ʽ
	len(2)+GATE_BYPASSTO_MOD_CMD(2)+gate_fd(4)+cmd_pkt(len-2-4)
	gate_fd:���������ڱ�ʶ���ص��ļ�����������������ֻҪ�����������ԭ�����ؾͿ�����
*/
#define  GATE_BYPASSTO_MOD_CMD		0x1000//���ط��������������ģ���Լ���������ת������Ӧ��ģ�鴦��

//ģ�鷢�͵�����
/*ģ�齫��Ҫ���͸����صķ�����Ϣ��������Ϣ���ڣ����������̣���������ת��������*/
/*��ʽ
	len(2)+MOD_BYPASSTO_GATE_CMD(2)+gate_fd(4)+cmd_pkt(len-2-4)
	gate_fd:�����̷���GATE_BYPASSTO_MOD_CMD����ʱ����gate_fd����������ģ����Ҫ�ڷ�����Ϣ��ʱ��ԭ���������ֶ�
*/
#define  MOD_BYPASSTO_GATE_CMD		0x2000//ģ�鷴��һЩ��Ϣ������	

//ģ�鷢�͵�����
/*ģ�齫��Ҫ���͸����صķ��ؽ����������Ϣ���ڣ����������̣���������ת��������*/
/*��ʽ
	len(2)+MOD_BYPASSTO_GATE_ACK(2)+gate_fd(4)+command(4)+result(4)
	gate_fd:�����̷���GATE_BYPASSTO_MOD_CMD����ʱ����gate_fd����������ģ����Ҫ�ڷ�����Ϣ��ʱ��ԭ���������ֶ�
*/	
#define  MOD_BYPASSTO_GATE_ACK		0x2001//ģ�鷴��result������
#endif

