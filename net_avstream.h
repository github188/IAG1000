/** @file	       net_avstream.h
 *   @brief 	���ղ�����ͻ��˷���������Ƶ��������
 *   @date 	2007.03
 */
#ifndef NET_AVSTREAM_H_20070306
#define NET_AVSTREAM_H_20070306

int init_net_avstream(void);
int create_rtnet_av_servers(void);
/** 
 *   @brief     ��������Ƶ���з������Ӧ��Ϣ
 *   @param  fd Ŀ��������
 *   @param  result ���صĴ������
 *   @param  ��Ҫ���ڷ�����Ϣanswer_data�������Ϣ
 *   @param  datalen buf����Ч���ݵĸ���
 *   @return   ��ֵ��ʾ����,�Ǹ���ʾ�ɹ�
 */ 
int send_rtstream_ack_pkt(int fd,WORD result,char* buf,int datalen);
#endif
