#ifndef MAIN_NETCMD_PROC_H
#define MAIN_NETCMD_PROC_H
#include <gt_com_api.h>
#include "mod_socket.h"


typedef struct{
int thread_no;	//�̱߳��
int listen_fd;	//����fd
int	dev_no;		//�豸���
}mainnet_info;

/**********************************************************************************************
 * ������	:print_gate_pkt()
 * ����	:���豸������ͨѶ�����ݰ���ʾ����Ļ��(������)
 * ����	:recv:Ҫ��ӡ��ͨѶ���ݰ�
 * ����ֵ	:��
 **********************************************************************************************/
#ifdef	SHOW_GATE_CMD_REC
void print_gate_pkt(struct gt_pkt_struct* recv);
#endif

/**********************************************************************************************
 * ������	:init_mainnetcmd_threads()
 * ����	:��������Զ���������ӵ��̳߳�
 * ����	:attr:�߳�����
 *			 para:�ṩ�������Ķ˿�ָ��
 * ����ֵ	:��
 **********************************************************************************************/
int init_mainnetcmd_threads(pthread_attr_t *attr,int port, int dev_no);
#endif



