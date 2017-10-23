/*	ģ���ͨѶ�ĺ�����
 *		shixin
 *		 2005.1
 *    ���������ṩ��ʵ�ָ�ģ���ͨѶ��Ҫ����ײ�ӿڣ�ʵ�����Ƕ�linux�ṩ����Ϣ���н����˰�װ
 *    �ϲ��Ӧ�ýӿڼ����ݽṹ�����ڱ����ʵ�ַ�Χ
 */
#ifndef MOD_COM_H
#define MOD_COM_H
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include "mod_cmd.h"					//ģ���ͨѶ�������ֶ���
#include "mod_socket.h"
#define		RECV_MOD_CMD_ALL	0	//��������Ŀ���ַ����Ϣ


#ifndef FOR_PC_MUTI_TEST
#define 		MAIN_SEND_CMD_CHANNEL	0x6060		//�����̷��������ͨ������������Ӧ�������ͨ�������������Լ�������
#define 		MAIN_RECV_CMD_CHANNEL	0x6061		//�����̽��������ͨ������������Ӧ�ý����͸������̵���Ϣ���͵����ͨ����

#define		GATE_RECV_CHANNEL		0x6066  		//send msg to remote gateway,used only by vsmain 
#define 	GATE_RECV_CHANNEL_DEV1	0x6067			//�������豸ʹ�õģ�����ͬ��								
#else
extern int		MAIN_SEND_CMD_CHANNEL;				//�����̷��������ͨ������������Ӧ�������ͨ�������������Լ�������
extern int 	MAIN_RECV_CMD_CHANNEL;				//�����̽��������ͨ������������Ӧ�ý����͸������̵���Ϣ���͵����ͨ����

extern int 	GATE_RECV_CHANNEL;			 		//send msg to remote gateway,used only by vsmain 

#endif



#define		MAX_MOD_CMD_LEN		1024			//ģ���ͨѶ��������󳤶�(����������,Ӧ�ò����г�����ֵ������,�����ǳ���bug)


#define 	MSG_INIT_ATTRIB			0				//��ʼ������ͨ����ʱ��flag���������


/**********************************************************************************************
 * �����ֽṹ,��ʵ��Ӧ�������ڸ�������Ĳ���������ͬ,����ʹ�ñ�
 * �ṹ�����ʱ���������ֻ��2,
 * ʹ��ʱ��Ҫ���������ǿ��ת����ָ�򱾽ṹ��ָ���Ա�ʹ��
 **********************************************************************************************/
struct mod_com_type
{
    long int target;                     	//�����Ŀ��ģ���ַ
    long int source;			 	//���������ģ��ĵ�ַ
    unsigned short  env;			//�����㷨���ں�����ͨѶ���ʱ������
    unsigned short  enc;			//ͬ��
    unsigned short cmdlen;		//����ĳ��ȣ����ֽ�Ϊ��λ������cmdlen,cmd�Ͳ���
    unsigned short cmd;			//������
    unsigned char  para[2];   	//����Ĳ���
};


/**********************************************************************************************
 * ������	:mod_com_init()
 * ����	:����һ������ͨ���������ͨ���Ѵ��ڣ���������ͨ���ϣ����ؽ���ͨ����id 
 * ����	:key:  ϣ������������ͨ���Ĺؼ���,�����������ͬ����ͨ����ģ����Ҫʹ����ͬ��key
 *			  flag: ��������ͨ����һЩ��������,дMSG_INIT_ATTRIB����
 * ����ֵ	:��ֵ��ʾʧ��
 *			  ������  ������ҽӵ�����ͨ��id�ţ��Ժ�ͨ���������������ͨ�����ͻ��������      
 **********************************************************************************************/
int mod_com_init(key_t key,int flag);

/**********************************************************************************************
 * ������	:mod_com_send()
 * ����	:��ָ������ͨ������һ��������Ϣ
 * ����	:com_id:����ͨ����id,�ɵ��� 'mod_com_init()' �ķ���ֵ�õ�
 *	  		 send:ָ��Ҫ���͵�����Ļ�������ָ��(�Ѿ�����target��source��len��msg�ֶ�)
 *			  flag:��Ҫ��ʲô������ִ�б�����,�����IPC_NOWAIT�򲻹���������,
 * 	      			  �������Ϸ���,�����0��ʾ���û�з����������������
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
 **********************************************************************************************/
int  mod_com_send(int com_id,struct mod_com_type *send,int flag);

/**********************************************************************************************
 * ������: mod_com_recv()
 * ����:  ��ָ��������ͨ���������ַ�н���һ�������
 * ����:
 *         com_id:����ͨ����id,�ɵ��� 'mod_com_init()' �ķ���ֵ�õ�
 *         addr:Ҫ������ͨ���л�ȡ�����ݰ��ĵ�ַ,���Ϊ0����յ����͸�ȫ��Ŀ���ַ����Ϣ,����Ϣ��
 *                ����ʱһ��������Լ���õĹ��ܿ�ĵ�ַ
 *         recv:ָ��Ҫ��Ž������ݵĻ�����,������Ӧ����һ��������������buffer
 *         buf_len:���յ���Ϣ����󳤶�,һ��дһ���Ƚϴ��ֵ�绺������С
 *         flag:MSG_NOERROR ��ʾ������յ������ݳ��ȴ���buf_len��ض���Ϣ,���Ҳ�֪ͨ������,һ��
 *                          Ӧ��ʱӦ����������,����ʱ���Բ���������
 *              IPC_NOWAIT  ��ʾ���û�н��յ���ϢҲ������������
 *              ��������ǿ���һ��ʹ��(MSG_NOERROR|IPC_NOWAIT)
 * ����ֵ:
 *         ��ֵ:���յ�����Ϣ���ֽ���(ȥ��target,source�����ֶκ�)
 *         ��ֵ:����
 ************************************************************************************************/
int  mod_com_recv(int com_id,long int addr,struct mod_com_type *recv,int buf_len,int flag);



//��:UPDATE���̰ѽ��յ�������ͻ�ȥ
/*
char rec_cmd_buf[MAX_MOD_CMD_LEN];
char send_cmd_buf[MAX_MOD_CMD_LEN];
void main(void)
{
	int main_send_ch,main_recv_ch;
	int rec_len,ret;
	struct mod_com_type *recv,*send;
	main_send_ch=mod_com_init(MAIN_SEND_CMD_CHANNEL,MSG_INIT_ATTRIB);
	if(main_send_ch<0)
	{
		//������
	}
	main_recv_ch=mod_com_init(MAIN_RECV_CMD_CHANNEL,MSG_INIT_ATTRIB);
	if(main_recv_ch<0)
	{
		//������
	}
	while(1)
	{
		rec_len=mod_com_recv(main_send_ch,UPDATE_PROCESS_ID,(struct mod_com_type *)(rec_cmd_buf),MSG_NOERROR);//���û���յ�����,����Ӧ���������������
		if(rec_len >0)//��ʾ�յ���һ����������
		{
			recv=(struct mod_com_type *)(rec_cmd_buf)��
			memcpy(send_cmd_buf,rec_cmd_buf,rec_len);
			send=(struct mod_com_type *)(send_cmd_buf)��
			send->target=recv->source;
			send->source=UPDATE_PROCESS_ID;
			send->cmdlen=recv->cmdlen;//��������ʱ��Ӧ�ó�����Ҫ���������ֶ�			
			ret=mod_com_send(main_recv_ch,send,IPC_NOWAIT);//ֱ�ӷ��أ�������
			if(ret<0)
			{
				//���ͳ���
			}			
		}
		else
		{
			//�����ǳ�����bug
		}
	}
	
}

*/

#endif
