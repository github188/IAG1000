
///GTlog��,��windows��Ϊ���г���ǰĿ¼��gtlog.txt�ļ�
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#ifndef _WIN32
#include <syslog.h>
#else
#include <time.h>
#define	GTLOG_NAME	"./gtlog.txt"
FILE	*log_fp=NULL;				///<��־��¼ָ��
#endif

/**********************************************************************************
 *      ������: log_headstring()
 *      ����:   ������ͷ��־��Ϣ���ɱ��������Ϣ��¼����־�ļ�
 *      ����:   head_str:ͷ������Ϣ�ַ���,��"[debug]"
 *                              format:������ʽ��ͬprintf��
 *      ����ֵ: 
 *               ע���ַ������ܳ��Ȳ�Ҫ����450�ֽ�
 *                        ʹ�ñ�����Ҫ����gtlog��
 *      Ӧ�ó���Ӧ�ò���ֱ��ʹ�ô˺���
 **********************************************************************************/
int log_headstring(const char *head_str,const char *format,...)
{
#ifdef _WIN32
	struct tm* lt=NULL;
	time_t	curtime;  
#endif
	char	data[5120];
	char	*wp=data;	///��ʼ��Ÿ�ʽ�б��ָ��
	va_list ap;			///�����б�

#ifdef _WIN32
	curtime=time(NULL);
	lt=localtime(&curtime);
	if(lt!=NULL)
	{
		sprintf(data,"<%04d-%02d-%02d %02d:%02d:%02d>",
				lt->tm_year+1900,
				lt->tm_mon+1,
				lt->tm_mday,
				lt->tm_hour,
				lt->tm_min,
				lt->tm_sec			
			);
		wp+=strlen(data);
	}
#endif
	///д��ͷ��Ϣ
	if(head_str!=NULL)
	{
		sprintf(wp,"%s",head_str);
		wp+=strlen(head_str);
	}

	///д�������Ϣ
	va_start(ap,format);
	vsprintf(wp,format, ap);
	va_end(ap);
#ifndef _WIN32
	syslog(LOG_INFO,data);
#else
	if(log_fp==NULL)
	{
		log_fp=fopen( GTLOG_NAME, "a" );
	}
	if(log_fp!=NULL)
	{
		fwrite(data,1,strlen(data),log_fp);
		fflush(log_fp);
	}
#endif
	return 0;
}


