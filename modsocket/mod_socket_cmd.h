//GT1000ϵͳ�ڲ�ͨѶ(udp-socket)��������
#ifndef MOD_SOCKET_CMD_H
#define MOD_SOCKET_CMD_H
#include <typedefine.h>
#include "mod_socket.h"
#include "gt_com_api.h"

/************************************************************************************
	�����̷��͸��������̵�����
************************************************************************************/
//�����̲�ѯ�������̵�״̬,�������ʲô����Ҳû��
#define	S_MAIN_QUERY_STATE			0x1001


typedef struct {
	unsigned short	env;	//ǩ��
	unsigned short	enc;	//����
	unsigned short	cmd;	//����
	unsigned short 	len;	//��������
	unsigned char	para[4];//����,���嶨��������岻ͬ����ͬ 	
}mod_socket_cmd_type;    //ģ�������Ľṹ

/**********************************************************************************
	�������̷��͸������̵�����
***********************************************************************************/

/*ʵʱ����Ƶ����ģ���������̷����Լ���״̬,״̬��־λ��������ʵʱͼ��ģ��ȷ��,0��ʾ������1��ʾ�쳣
 *��ʵʱͼ��ģ���Լ�����״̬�仯ʱ���ʹ�������ߵ����յ������̷�����MAIN_QUERY_STATE����ʱ���ʹ����� 
 *��ʽ:S_RTSTREAM_STATE_RETURN(2)+len(2)+state(4)
 *state:ʵʱͼ��ģ���״̬�����ȫ0��ʾһ������������λ��������ʵʱͼ��ģ��ȷ��
 */
#define S_RTSTREAM_STATE_RETURN		0x0301
struct s_rtimage_state_struct{
	unsigned		mod_state		:1;	//ģ��״̬0��ʾ���� 1��ʾ�쳣,��λ����ģ������
	unsigned		net_enc_err		:1;	//�����ѹ��оƬ����
	unsigned 		net_enc_busy	:1;  //�������оƬæ��־
	unsigned 		test_d1_flag		:1; //����d1ͨ����־
	unsigned		reserve			:28;
};

/**********************************************************************************
 *	�����ؽ����������
***********************************************************************************/

/*
 *	PPPOE��ؽ��̷��͸�������״̬
 *    ��ʽ:S_PPPOE_STATE_RETURN(2)+len(2)+state(4)
 *    state:PPPOE���ģ���״̬��

*/
#define S_PPPOE_SUCCESS		0	//adsl��������
#define S_PPPOE_NO_MODEM	1	//�Ҳ���adsl modem
#define S_PPPOE_PASSWD_ERR	2	//adsl�ʺ��������
#define S_PPPOE_USR_TWICE	3	//�ʺ��ظ�����
#define S_PPPOE_USR_INVALID	4	//�ʺ���Ч
#define S_PPPOE_PAP_FAILED	5	//�����ʺ���Ч

#define S_PPPOE_STATE_RETURN		0x0501


/**********************************************************************************
 *	upnp�˿�ӳ������������
***********************************************************************************/

/*
 *	upnp�˿�ӳ����̷��͸�������״̬
 *    ��ʽ:S_UPNPD_STATE_RETURN(2)+len(2)+state(4)
 *    state:UPNPD���ģ���״̬��

*/
#define S_UPNPD_SUCCESS		0	//upnp�˿�����ӳ��
#define S_UPNPD_FAILURE		1	//upnpӳ��ʧ��

#define S_UPNPD_STATE_RETURN		0x0c01





/*
	ת�������������������ģ�齫Զ�����ط������������ֱ��ת������Ӧģ��
	�Լ�����Ӧģ����Ҫ���ظ����ص�����ͳ�ȥ
*/
//ʵ��ת��ʱ����һ���ֶ�Ӧ����4�ֽڵ��ļ�����������ʾ�յ����������
//��ģ�鷢�͵�����
/*��ģ�齫���ط�������Ϣԭ��������ģ���ڲ�ͨѶ����Ϣ���﷢�͸���Ӧģ��*/
/*��ʽ
	S_GATE_BYPASSTO_MOD_CMD(2)+len(2)+gate_fd(4)+cmd_pkt(len-2-4)
	gate_fd:���������ڱ�ʶ���ص��ļ�����������������ֻҪ�����������ԭ�����ؾͿ�����
*/
#define  S_GATE_BYPASSTO_MOD_CMD		0x1000//���ط��������������ģ���Լ���������ת������Ӧ��ģ�鴦��

//ģ�鷢�͵�����
/*ģ�齫��Ҫ���͸����صķ�����Ϣ��������Ϣ���ڣ����������̣���������ת��������*/
/*��ʽ
	S_MOD_BYPASSTO_GATE_CMD(2)+len(2)+gate_fd(4)+cmd_pkt(len-2-4)
	gate_fd:�����̷���GATE_BYPASSTO_MOD_CMD����ʱ����gate_fd����������ģ����Ҫ�ڷ�����Ϣ��ʱ��ԭ���������ֶ�
*/
#define  S_MOD_BYPASSTO_GATE_CMD		0x2000//ģ�鷴��һЩ��Ϣ������	



#endif



