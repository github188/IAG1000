/** @file	       avserver.c
 *   @brief 	�ṩ����Ƶ�������غ�������
 *   @date 	2007.03
 */
 #ifndef AVSERVER_H_20070309
 #define AVSERVER_H_20070309

#define	HDDBUFF_MAX_FRAME		(25*90)	//����1��30֡����90֡����
 
 /** 
 *   @brief     ��������Ƶ�����߳�
 *   @return   0��ʾ�ɹ�,��ֵ��ʾʧ��
 */ 
int create_av_server(void);
 
/** 
 *   @brief     ����Ƶ���з�����봦�����
 */ 
void avserver_second_proc(void);

/**
*	@brief		����Ӳ�̻������߳�
*	@return		0��ʾ�ɹ�����ֵ��ʾʧ��
*/
int  create_diskbuff_thread(void);

void  set_playback_en(int no);
void set_playback_cancel(int no);
void set_hddbuf_offset(int offset);
int get_playback_stat(int no);

#endif
