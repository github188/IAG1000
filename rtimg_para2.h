#ifndef RTIMG_PARA2_H_20070302
#define RTIMG_PARA2_H_20070302
#include "serv_info.h"



/** 
 *   @brief     ��tcprtimage2ģ������в�����״̬����Ϊ��ʼֵ
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int init_server_para(void);

int read_server_para_file(void);

tcprtimg_svr_t *get_rtimg_para(void);
#endif

