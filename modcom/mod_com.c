/*	ģ���ͨѶ�ĺ�����
 *		shixin
 *		 2005.1
 *    ���������ṩ��ʵ�ָ�ģ���ͨѶ��Ҫ����ײ�ӿڣ�ʵ�����Ƕ�linux�ṩ����Ϣ���н����˰�װ
 *    �ϲ��Ӧ�ýӿڼ����ݽṹ�����ڱ����ʵ�ַ�Χ
 */
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include "mod_com.h"
#include <errno.h>
#include <syslog.h>
/**********************************************************************************************
 * ������	:mod_com_init()
 * ����	:����һ������ͨ���������ͨ���Ѵ��ڣ���������ͨ���ϣ����ؽ���ͨ����id 
 * ����	:key:  ϣ������������ͨ���Ĺؼ���,�����������ͬ����ͨ����ģ����Ҫʹ����ͬ��key
 *			  flag: ��������ͨ����һЩ��������,дMSG_INIT_ATTRIB����
 * ����ֵ	:��ֵ��ʾʧ��
 *			  ������  ������ҽӵ�����ͨ��id�ţ��Ժ�ͨ���������������ͨ�����ͻ��������      
 **********************************************************************************************/
int mod_com_init(key_t key,int flag)
{
    int queue_id;
    queue_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (queue_id <0 ) {//��ʾ�Ѿ������˸ü�ֵ����Ϣ����
        queue_id=msgget(key,0);
        if(queue_id <0 )
        {				//��ʾ����Ϣ����ʧ��
            printf("mod_com_init can't create message:0x%x!\n",key);
            return -1;
        }
    }
    return queue_id;    
}

/**********************************************************************************************
 * ������	:mod_com_send()
 * ����	:��ָ������ͨ������һ��������Ϣ
 * ����	:com_id:����ͨ����id,�ɵ��� 'mod_com_init()' �ķ���ֵ�õ�
 *	  		 send:ָ��Ҫ���͵�����Ļ�������ָ��(�Ѿ�����target��source��len��msg�ֶ�)
 *			  flag:��Ҫ��ʲô������ִ�б�����,�����IPC_NOWAIT�򲻹���������,
 * 	      			  �������Ϸ���,�����0��ʾ���û�з����������������
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
 **********************************************************************************************/
int  mod_com_send(int com_id,struct mod_com_type *send,int flag)
{
	int rc;
	if(com_id<0)
	{
		printf("get a error comid:%d\n",com_id);
		return -1;
	}
	//rc = msgsnd(com_id,send, send->cmdlen+8, MSG_NOERROR|IPC_NOWAIT|flag);//��Ϣ�����������msgsz�ֽ��򱻽ص�
	rc = msgsnd(com_id,send, send->cmdlen+sizeof(struct mod_com_type)-sizeof(send->para)-4, MSG_NOERROR|IPC_NOWAIT|flag);//��Ϣ�����������msgsz�ֽ��򱻽ص�

	return rc;
}

/**********************************************************************************************
 * ������: mod_com_recv()
 * ����:  ��ָ��������ͨ���������ַ�н���һ�����ݰ�

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
int  mod_com_recv(int com_id,long int addr,struct mod_com_type *recv,int buf_len,int flag)
{
	char msg[50] = {0};
	int rc;
	if(com_id<0)
	{
		printf("mod_com_recv get a error com_id:%d\n",com_id);
		return -EINVAL;
	}
	recv->target=addr;
	rc = msgrcv(com_id, recv, buf_len, addr, flag|MSG_NOERROR);
	if(rc>0)
		return (rc-8);
	else
		return -errno;
}


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
		rec_len=mod_com_recv(main_send_ch,UPDATE_PROCESS_ID,(struct mod_com_type *)(rec_cmd_buf),MAX_MOD_CMD_LEN,MSG_NOERROR);//���û���յ�����,����Ӧ���������������
		if(rec_len >0)//��ʾ�յ���һ����������
		{
			recv=(struct mod_com_type *)(rec_cmd_buf)��
			memcpy(send_cmd_buf,rec_cmd_buf,rec_len);
			send=(struct mod_com_type *)(send_cmd_buf)��
			send->target=recv->source;
			send->source=UPDATE_PROCESS_ID;
			ret=mod_com_send(main_recv_ch,send,rec_len,IPC_NOWAIT);//ֱ�ӷ��أ�������
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


