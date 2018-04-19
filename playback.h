#ifndef PLAYBACK_H
#define PLAYBACK_H
#include "avilib.h"
#include <gate_cmd.h>


#define VERSION         "0.09"
/*********************************************�汾����****************************************/
//ver:0.01  �����ļ�
//ver:0.02  ����ģʽ����������,��Ȼ��������
//ver:0.03  ����:ʹ��VLC��������ĳ̨�豸ʱ��������֡������;
//              ԭ��:�ڼ�⵽���ӹرյ���Ϣ�󣬹ر�socket�����Ƿ����̹߳ر�Ҫ��һ������ʱ
//              �������ʱ���µ����ӣ��������ӵ�fd����һ�ε�һ�����ͻ�����ϴεķ����߳�
//              û�йرա�
//ver:0.04  ��gtsfͷ�У���Ƶ��ʽ�������������⣬���޷�����ip�Խ�ģʽ���ڴ��޸�
//ver:0.05  ����Թر�����ʱ��select�Ĵ�������
//ver:0.06  ������β���ʱ����Ƶ��ʽ��������
//ver:0.07  ����㲥��indexʱ��û�и����Ƿ��ͨ������ch����
//ver:0.08  support 1080P and 720P
//ver:0.09  �޸�multichannel ����
/*****************************************�汾��������**************************************/


#define         PLAYBACK_TRUE                    1
#define         PLAYBACK_FALSE                  0

#define PLAYBACK_NUM                                16
#define PLAYBACK_FRAMERATE_25              25
#define PRE_CONNECT_SEC_MAX                 30  /*Ԥ���ӵ�����*/

#define PLAYBACK_STAT_IDLE                     0     /* û��ʹ��*/
#define PLAYBACK_STAT_USED                    1     /* ռ����Դ������û��׼����*/
#define PLAYBACK_STAT_OK                    2     /* ׼������OK*/

#define PLAYBACK_CTRL_IDLE                0     /* �޲���*/
#define PLAYBACK_CTRL_START             1     /* ������*/
#define PLAYBACK_CTRL_CLOSE             2     /* �ر�����*/
#define PLAYBACK_CTRL_PAUSE             3     /* ��ͣ����*/
#define PLAYBACK_CTRL_RESUME             4     /* ���²���*/
#define PLAYBACK_CTRL_SPEED             5     /* ��������*/
#define PLAYBACK_CTRL_SEEK             6     /* ���ҹ���*/


#define PLAYBACK_SOURCE_FILE                 0     /* ����Դ�Ǳ����ļ�*/
#define PLAYBACK_SOURCE_POOL                1     /* ����Դ�Ǳ��ػ����*/


#define MAX_FILE_NAME_SIZE       256
#define PLAYBACK_BUFF_LEN         400*1024


#define PLAYBACK_SUCCESS                    0     /* OK*/
#define PLAYBACK_ERR_BUF_SIZE           -1     /*buffer���󣬿���̫С*/
#define PLAYBACK_ERR_PARAM               -2    /* ��������*/
#define PLAYBACK_ERR_USER_FULL        -3     /* �û����� */
#define PLAYBACK_ERR_NO_FILE            -4     /* û�в鵽�ļ�*/
#define PLAYBACK_ERR_FILE_ACCESS     -5     /* û�в鵽�ļ�*/
#define PLAYBACK_ERR_NO_OPEN           -6     /* û�в鵽�ļ�*/
#define PLAYBACK_ERR_CONNECT            -7     /* �ֳ����Ӵ���*/
#define PLAYBACK_ERR_POLL_NODATA    -8     /* �ֳ�û������*/
#define PLAYBACK_ERR_FILE_NOINDEX    -9     /* �ط�û�������ļ�*/



enum transpeed
{
    PLAYBACK_QSPEED = 0,
    PLAYBACK_HSPEED,
    PLAYBACK_NSPEED,
    PLAYBACK_2SPEED,
    PLAYBACK_4SPEED,
    PLAYBACK_ISPEED
    
};
/*
typedef enum
{
	PLAYBACK_CTRL_PAUSE,
	PLAYBACK_CTRL_RESUME,
	PLAYBACK_CTRL_SPEED,
	PLAYBACK_CTRL_SEEK
}recordctl_t;
*/

typedef struct hd_playback_struct{ 
    int           channel;
    int           sourcefrom;
    int           frames;                               //��ȡ����֡��
    char        Indexfilename[256];           //��ѯ�����������ļ���
    int           fileindex;                            //��ǰ�طŵ�¼���ļ�������
    int           recordfiletotal;                   //���λطŵ�¼���ļ�����
    int           lastframe;                           // /*����֡������ֻ�в��ŵ����һ���ļ�ʱʹ��*/
    FILE 	    *index_fp;                           //�򿪵������ļ���
    avi_t *     aviinfo;                               //��ǰ¼���ļ���Ϣ
    struct gt_time_struct    starttime;  //��ʼ����ʱ�䣨��Чʱ����ڣ�
    struct gt_time_struct    endtime;   //��ֹ����ʱ�䣨��Чʱ����ڣ�    
    int	    start;                                   //�طŵĿ�ʼʱ��
    int	    stop;                                    //�طŵĽ���ʱ��
    int           framrate;                            //�ط�֡��
    int           state;                                  //��ǰ�ı�״̬0��ʾû��׼���ã�1ռ����Դ��2׼������
    int           oper;                                  //��ǰ����0��ʾ�ر� 1���ڻط�
    media_source_t media;                       //�����ֳ���ʱ��ʹ��
    int         pre_connect;                        //�����ֳ�ʱԤ��ȡ��ʱ��
    char      peeraddr[32];                             //���ӵĶԶ˵�ַ
    short     peerport;                              //���ӵĶԶ˶˿�
    int         speed;                                
    int         socket;                                 //���ط��͵�socket
    int         packetsum;                         //���������
    int         playbackindex;                    //�ط�����
	int         audio_source;                     //��Ƶ��Դ����ʱ�Ŵˣ����뷽ʽ��ÿ���ļ����Խ���
    
}playback_struct;




typedef struct playback_open_struct{ 
unsigned int peer_ip;			//�����ip��ַ
unsigned short peer_port;		//����˼���port�˿�
unsigned short channel;		//0-31   ¼��ͨ����
unsigned int speed;			//��������Enum transpeed����

struct gt_time_struct    starttime;  //��ʼ����ʱ�䣨��Чʱ����ڣ�
struct gt_time_struct    endtime;   //��ֹ����ʱ�䣨��Чʱ����ڣ�
    
}playback_open_struct;



int playbackInit();
int playbackClose(int index);
int playbackOpen(viewer_subscribe_record_struct *pplaybackopen);
//int playbackReadFrame(int playbackId, int start,  int stop);
DWORD get_playbackmodstat(void);
playback_struct * getplayback(int index);
int playbackfileOpen(viewer_subscribe_record_struct *pplaybackopen);
int playbackConnectOK(int playbackId, int fd);
#endif



