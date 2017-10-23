/*	ģ�����û���udp��socketͨѶ�ĺ�����
 *		wsy
 *		 2007.9
 *    ���������ṩ��ʵ�ָ�ģ�����û���udp��socketͨѶ��Ҫ����ײ�ӿ�
 *    �ϲ��Ӧ�ýӿڼ����ݽṹ�����ڱ����ʵ�ַ�Χ
 */
#ifndef MOD_SOCKET_H
#define MOD_SOCKET_H
#include <sys/types.h> 
//#include <sys/ipc.h> 
//#include <sys/msg.h> 
#include "typedefine.h"
#include "mod_cmd.h"
#include "gtthread.h"


#ifndef IN
	#define	IN
#endif

#ifndef OUT
	#define	OUT
#endif

#ifndef INOUT
	#define INOUT
#endif

#ifndef IO
	#define	IO
#endif

typedef struct {
	long	gatefd;	//����fd����<=0��ʾ����Ҫ���ظ�����
	unsigned short 	dev_no;	//�豸��ţ�����Ѱ������
	unsigned short	env;	//ǩ��
	unsigned short	enc;	//����
	
}gateinfo;

typedef struct {	
	gateinfo	gate;		//��������ص���Ϣ
	unsigned short	cmd;	//����
	unsigned short 	len;	//����(para)����
	unsigned char	para[4];//����,���嶨��������岻ͬ����ͬ 	
}mod_socket_cmd_type;    //ģ�������Ľṹ

#define MAX_MODULE_NAME_LEN		16//ģ�����Ƶ���󳤶ȣ��ݶ�

typedef struct {
	int		mod_id;				//ģ���ID����mod_cmd.h
	int		com_fd;				//ģ���modsocketͨ��id
	char	module_name[MAX_MODULE_NAME_LEN];	//ģ�����ƣ���"diskman"
	int 	(*fn)(int sourceid, mod_socket_cmd_type *modsocket);//�ص�����ָ��
}mod_socket_thread_data;//���ڸ�ģ�鴴������mod_socket������̵߳�����


#define		MAX_MOD_SOCKET_CMD_LEN		2048			//ģ���ͨѶ��������󳤶�(����������,Ӧ�ò����г�����ֵ������,�����ǳ���bug)

/**********************************************************************************************
 * ������	:mod_socket_init()
 * ����	:	��ָ���Ķ˿ڽ�������udp socket��������֮
 * ����	:	send_flag,	��������ʱʹ�õ�����flags,һ���趨,��������������
 *			recv_flag,	��������ʱʹ�õ�����flags
 * ����ֵ	:��ֵ��ʾʧ��
 *			  ������  �������ļ����������Ժ�ͨ���������ͻ��������      
 **********************************************************************************************/
int mod_socket_init(IN int send_flag, IN int recv_flag);

/**********************************************************************************************
 * ������	:mod_socket_send()
 * ����	:��ָ����������ַ����һ��������Ϣ
 * ����	:com_fd:�ɵ��� 'mod_socket_init()' �ķ���ֵ�õ�
 *		 target:Ŀ��ģ���id����Ϊ0���ʾ���͸�����ģ��
 *		 source:����ģ���id
 *		 cmdbuf:ָ��Ҫ���͵�����Ļ�������ָ��(�Ѿ����������Ϣ)
 *		 cmdlen:����������Ч���ݵĳ���
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
 **********************************************************************************************/
int mod_socket_send(IN int com_fd,IN int target,IN int source,IN void *cmdbuf,IN int cmdlen);

/**********************************************************************************************
 * ������: mod_socket_recv()
 * ����:  ��socket�н���һ�������
 * ����:
 *        com_fd:�ɵ��� 'mod_socket_init()' �ķ���ֵ�õ�
 *		 myid:��ģ���id
 *		 source: ���������ģ��id
 *		 cmdbuf:ָ��Ҫ���͵�����Ļ�������ָ��(�Ѿ����������Ϣ)
 *		 cmd_maxlen:����������󳤶�
 *		 source_addr:��ŷ���ģ���ip����ΪNULL��ʾ����ȡ
 * ����ֵ:
 *         ��ֵ:���յ�����Ϣ���ֽ���
 *         ��ֵ:����
 ************************************************************************************************/
int mod_socket_recv(IN int com_fd, IN int myid, OUT int *source, OUT void *cmdbuf,IN int cmd_maxlen, OUT char *source_addr);

int send_ack_to_main(int com_fd, int mod_id, int cmd, int result, gateinfo *gate);

/**********************************************************************************************
 * ������: mod_socket_req_recv()
 * ����:  ��socket�н���һ�������
 * ����:
 *          com_fd:�ɵ��� 'mod_socket_init()' �ķ���ֵ�õ�
 *		 req_id:����ʱ�Ǳ�ģ���id,���ʱ��ʾ���յ������ݰ���Ŀ��id
 *		 source: ���������ģ��id
 *		 cmdbuf:ָ��Ҫ���͵�����Ļ�������ָ��(�Ѿ����������Ϣ)
 *		 cmd_maxlen:����������󳤶�
 *		 source_addr:��ŷ���ģ���ip����ΪNULL��ʾ����ȡ
 * ����ֵ:
 *         ��ֵ:���յ�����Ϣ���ֽ���
 *         ��ֵ:����
 ************************************************************************************************/
int mod_socket_req_recv(IN int com_fd, INOUT int *req_id, OUT int *source, OUT void *cmdbuf,IN int cmd_maxlen, OUT char *source_addr);


int creat_modsocket_thread(pthread_t *thread_id, int com_fd, int mod_id, char *mod_name, int (*fn)(int sourceid, mod_socket_cmd_type *modsocket));
#endif
