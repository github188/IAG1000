/** @file	       net_aplay.h
 *   @brief 	���ղ�����ͻ��˷���������Ƶ�������������
 *   @date 	2007.03
 */
#ifndef NET_AVSTREAM_H_20070330
#define NET_AVSTREAM_H_20070330


#define RTIMAGE_AUDIO_DEV   2  //��Ƶ�豸���
#define RTIMAGE_AUDIO_DEC   0  //��Ƶ���������
#define RTIMAGE_AUDIO_DEV_CHN 0 //��Ƶ�豸ͨ����

int create_rtnet_aplay_servers(void);

void check_and_play(int no);

#endif
