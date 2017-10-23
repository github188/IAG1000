#ifndef FILE_DEF_H
#define FILE_DEF_H
#include <typedefine.h>
#include <stdio.h>
#include <string.h>
#include <gtlog.h>

//��·���ͺŶ���

#define		ALARMIN_MASK			0xffff	// from 0xff lc //from 0x3f,wsy		//������������
#define		TOTAL_ALARMOUT_PORT  		4			//��󱨾����ͨ����
#define		ALARMOUT_MASK			0xff 		//fixme,������Ӹı�ʱ

#define		TOTAL_TRIG_IN			6			//����������
#define 	MAX_TRIG_IN				20 		////��󴥷�������Ŀ,�����ֻ������ռ���,��Ҫ�豸�ľ���ֵʱ����get_trigin_num()

#define 		MAX_TRIG_EVENT		10			//��·������󴥷��¼�����

#define 		MAX_FILE_LEN		30*1024			//�����ļ���󳤶� Զ���趨����ʱ�õ�

#if 0

#ifndef _WIN32
#include <syslog.h>

//����־��¼����
//name��ʾ��־��Ϣ�е�����
#define gtopenlog(name) openlog(name,LOG_CONS|LOG_NDELAY|LOG_PID,LOG_LOCAL0 );//LOG_USER);

//#define gtlog  syslog		//ϵͳ��־��Ϣ��¼
#define gtlog syslog
//һ������Ϣ
#define gtloginfo(args...) syslog(LOG_INFO,##args)	//��¼һ����Ϣ
//���صĴ�����Ϣ
#define gtlogfault(args...) syslog(LOG_CRIT,##args)	//
//������Ϣ
#define gtlogerr(args...) syslog(LOG_ERR,##args)	//
//������Ϣ
#define gtlogwarn(args...) syslog(LOG_WARNING,##args)

#define gtlogdebug(args...) syslog(LOG_DEBUG,##args)
#endif //_WIN32
#endif


/*
	vsmain define
*/

#define		IPMAIN_TMP_DIR			"/tmp"							//ipmain���ڴ����ʱ�ļ��ĵط�
#ifndef FOR_PC_MUTI_TEST
#define	 	DEVINFO_PARA_FILE		"/conf/ipcall.ini"					//����豸�̶���Ϣ���ļ�����Щ��Ϣһ�㲻���޸�
#define	 	IPMAIN_PARA_FILE		"/conf/ipcall.ini"	
#define	 	IPMAIN_PARA_GATE_BAK		"/conf/ipcall_gate.ini" 			//����ָʾ�仯�������ļ�����

#endif

/*
	rtimage define
*/


#ifndef FOR_PC_MUTI_TEST
#define	 	RTIMAGE_PARA_FILE		"/conf/ipcall.ini"//"/conf/rtimage.ini"
#endif

#endif
