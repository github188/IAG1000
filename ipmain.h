
/*
	ipmain.h 	created by lc
	
*/
//������־�ػ�����syslogd -m 0 -O /conf/log/gtlog.txt -s 512 -b 10 -S
#ifndef IPMAIN_H
#define IPMAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/time.h>
#include <string.h>
#include <syslog.h>
#include <typedefine.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mod_socket.h"

//#include "hi3515/wtdg/watchdog.h"

//#include "exdrv_3520Ademo/hi_wtdg/watchdog.h"
//lc change from 30 to 150
#define WTDG_TIMEOUT  150

//ifconfig eth0 hw ether 00:0F:EA:05:B2:77
//note �޸�/proc/sys/kernel/msgmni  �ļ���������ϵͳ���Դ����������Ϣ������
#define EMBEDED	 1	//Ƕ��ʽϵͳ:1 PCϵͳ 0

#define  VERSION		"0.52"



/*
 *0.51            when check aac head error break not continue 
 *0.44  15-5-11   when alarm happened ,do not related to hdmodule and diskman
 *0.43  15-3-24   recv iptalk alarm msg as 8-11 chn alarm
 *0.42  15-1-30   send alarm chn when trig audio output
 *0.41  14-10-20  when gate down,set relay output due to config
 *0.40  14-9-25   when alarm happen, net led flash or/and buz
 *0.39  14-9-19   fix send ini to gate bug lockfile
 *0.38  14-9-12   disable/enable wdt with serial mode
 *0.37  14-8-18   fix set_time bug
 *0.36  14-8-4    fix multichannel audio down and usr_lock_time cmd bug(also mutlichannel) fix notice.wav
 *0.35  14-7-29   fix single channel cancle alarm playback bug
 *0.34  14-7-28   fix multichannel cancel alarm clear trig hdmod bug and update failed reboot
 *0.33  14-7-21   add power monitor
 *0.32  14-7-9    ����3520D���壬��Ƶѡ���뿴�Ź����Ƹı�
 *0.31  14-3-25   ԭ���ı�����ack��־���ƴ������⣬�����bug
 *0.30  14-2-25   ��rtimage����playback���ȡ����netencoder:playback_enableѡ��
 *0.29  14-2-21   ���������յ������ļ��󣬿�ʼֹͣ�����������������������⣬�����豸�϶���������
 *0.28  14-2-11   ֧��¼���ͨ������ع��ܣ������в���������ƽ̨�ṩ��Ӧ�޸Ĳſ�֧��
 *0.27  14-2-7    ������ͨ����һ��bug��ȥ��0.26�汾�������޸�
 *0.26  14-1-22   �����devinfo��ȡ��Ƶͨ�����ƣ������豸ע��ʱ��Ӹò����ֶ�
 *0.25  14-1-10   ����Զ�ͨ��֧�֣�ͨ�������ļ���multichannel��Ŀǰֻ֧�ֱ���ʱ����Ӧһ·��Ƶ��ͬʱ�����hdplayback������ת��
 *0.24  13-12-27  �����encbox��飬�����������coaxial err�������豸
 *0.23  13-12-24  ��ʱ����֮ǰһֱĬ�Ϸ��ز�����ȷ�����ڽ�����ʵ��ֵ
 *0.22  13-12-20  �Ա����������������Χ��ͬʱ��������λ������ǰ����֤�����ڱ�����������ִ���в��뱨����λ
 *0.21  13-11-07  ��killencbox����sleep5�뱣֤ipcrm�ɹ��󣬽���tar
 *0.20  13-11-05  �޸������У�tar��������ȼ�Ϊ�ϵͣ��������tar��ѹʱ���豸��ס���
 *0.19  13-11-05  ֧�ַ���һ�Զ࣬���Ҽ���Է��������˾�֧��
 *0.18  13-10-25  ����encbox��������ƵԴ���ϵ����������־
 *0.17  13-9-22   �Ķ�������ʽ����ɾ��ip1004�����ݣ����ж�������ͬʱ���������������ݴ�����֤���������ɹ�
 *0.16  13-9-17   ȥ��multichannel���ܣ�ͬʱ���gpio�Գ��մ������
 *0.15  13-9-13   �����¹��ܣ��Դ��ڲ����������ļ�������ֹ��������ʹ�ô��ڣ����ظ�ռ�ô���
 *0.14  13-9-9    �����¹��ܣ�֧��multi-channel
 *0.13  13-9-3    ����ƶ����ֻ��1·��bug
 *0.12  13-8-31   �ڵ���aplay֮ǰ����ɱ��һ��aplay
 *0.11  13-8-29   ������ڲ����ڵ������ͽ�������
 *0.10  13-8-15   ��������err�Ʋ������õ�����
 *0.09  13-8-2    ʹ�����ͻ�opensslԴ�룬no-asmѡ������libssl libcrypto�⣬�����ܲ��ԣ�ͬʱ�޸ĶԸ�ʽ��ʱ��Ĺ��㹦��
 *0.08  13-7-24   �޸�����ʱ�жϿռ��bug
 *0.07  13-7-04   �˾��ڼ�˫��������˾�ͨ������һ��
 *0.06  13-7-04   ����vda��ع���
 *0.05  13-6-25   �޸ı�����¼�У�ʱ��Ϊ��ǰʱ����Ǳ���ʱ���bug������aplay��ع���
 *0.04  13-6-21   ����������������
 *0.03  13-6-18   ���Ե�һ�ֽ�����ͳһ�޸ģ����bug
 *0.02  13-6-14   �������Բ��Ų��ԣ�������4����ʱ���߼���������
 *0.01  12-11-15  ����ipmain��Ŀ��ʵ���豸���ã���ʼ������������ֹ���
 */


//�豸�����̱�ʶ����
#define	VENDOR_GTALARM		0x41435447		//��ͨ����

#define 	DEVICE_VENDOR 			VENDOR_GTALARM


//���غ��豸ͨѶЭ����ڲ��汾��
#define	INTERNAL_PROTOCAL		2

//ָʾ��GPG_12

/*
	���ʹ������
*/


//#define     USE_IO

//#define	DEAMON_MODE			//���ػ����̷�ʽ���г���
//#define 		MULTICAST_USE		//ʹ���鲥��ʽ�����豸
//#define  		USE_LEDS			//ʹ�����LED����
//#define ARCH_3520A
//#define    ARCH_3520D

#define    GATE_PERIOD_TIME_DEF			0		//������Ч��Ĭ��ֵ�������GTIP1004��������ӣ�������г������ʱ����Ͽ�����
#define    REMOTE_GATE_CMD_PORT		 	8090	    //Զ�����ؿ��ŵ�����˿�
#define    DEV_MAIN_CMD_PORT	 			8095    //0x4791
#define    DEV_MAIN_IMAGE_PORT   			8096
#define    DEV_MAIN_AUDIO_PORT   			8097
#define    DEV_DEFAULT_WEB_PORT			8094
#define	   DEV_DEFAULT_FTP_PORT			  21
#define	   DEV_DEFAULT_TELNET_PORT		  23

#define    GTIP_IO_VALID_DELAY    0   //����->�����ӳ�
#define    GTIP_IO_INVALID_DELAY  3   //����->�����ӳ�



//#define		RMT_PERIOD				10	//Զ������(����)����ָ��ʱ����Զ��Ͽ�����


//test function
#define		DISPLAY_REC_NETCMD	//��ʾ�������յ���������Ϣ
#define		SHOW_WORK_INFO		//��ʾһЩ����������Ϣ
#define		SHOW_GATE_CMD_REC	//��ʾ���յ�����������
//#define		NOT_INCLUDE_WATCH_BD	//������51��ذ������
//#define		SHOW_ALL_WATCHBD_INFO	//��ʾ���е�51��ذ�ͨѶ��Ϣ
//#define		TEST_FROM_TERM			//���ն�ģ��������Ϣ
//#define		TEST_REGIST				//��������ע��
//#define 		NOT_CHECK_REMOTEIP			//�����Զ������ip��ַ�Ƿ�Ϸ� 
#define		REMOTE_NOT_TIMEOUT		//�����г�ʱ���������Զ�̼�����������Ӻ󵽴�ָ��ʱ����û�з���������Ͽ�����
//#define     DONT_CONNECT_GATE   		//����������
//#define		TEST_SEND_STATE			//���Բ��Ϸ���״̬������



#define printdbg	printf

#include <file_def.h>
#include <mod_com.h>
#include <gtsocket.h>


#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned long

#ifndef OUT
#define OUT
#define INOUT
#define IN
#endif



#if BOARD==INTIME_RDK5

#elif BOARD==GT1000_1_0

#else
	#error "Rtimage software can't support this board"
#endif


#include "netinfo.h"

//function define

#if 0
static __inline__ int show_time(void)
{
	struct timeval tv;
	struct tm *ptime;
	char pbuf[60];
	time_t ctime;
	if(gettimeofday(&tv,NULL)<0)
	{
		return -1;
	}
	ctime=tv.tv_sec;
	ptime=localtime(&ctime);
	if(ptime!=NULL)
	{
		sprintf(pbuf,"%d-%d-%d %d:%d:%d.%03d\n",ptime->tm_year+1900,ptime->tm_mon+1,ptime->tm_mday,ptime->tm_hour,ptime->tm_min,ptime->tm_sec,(int)tv.tv_usec/1000);	
		printf("%s",pbuf);
	}
	return 0;
	
}
#endif







DWORD atohex(char *buffer);

int reinstall_localimedrv(int ch);

//��ȡһ��Ĭ�����Խṹ
//������Ӧ���ͷ�
int get_gtthread_attr(pthread_attr_t *attr);
int posix_memalign(void **memptr, size_t alignment, size_t size); 

int init_wtdg_dev(int timeout);
int feed_watch_dog();

#endif



















//����1,2��Ӧ��Э���еĴ���0,1
