#ifndef PROCESS_GATECMD_H
#define PROCESS_GATECMD_H

#include <gate_cmd.h>
#include "playback.h"
#include "mod_socket.h"

//int process_net_index(int channel,char *index,struct query_index_struct *query);

//ץͼ��Ӧ�͸�����
int get_hq_pic_answer(gateinfo *gate, WORD result, BYTE *timeprint,char* indexname);
//��ʼ����������ͨѶ������ͨ��
int init_com_channel(void);
int creat_modcmdproc_thread(pthread_attr_t *attr,void *arg);
int send_state2main(void);
int creat_playback_modsocket_thread(void);
void *playback_listen_thread(void *para);

#endif
