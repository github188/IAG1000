/*
 * conf�ļ�������
 *
 */
/**
   @file    confparser.c
   @author  shixin
*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common/commonlib.h"
#include "confparser.h"

#define CONF_INVALID_KEY     ((char*)-1)
/*
 *��ȡ�����н��д���,����:ȥ�����е�"��,�ո���Ʊ��,����,����ע��
 *
 */
static int process_line(char *buf,int len)
{
	char *p;
	if(buf==NULL)
		return -1;
	//����ע��
	p=index(buf,'#');
	if(p!=NULL)
	{
		*p='\0';
		if(p==buf)
			return 0;
	}
	//�������
	while(1)
	{
		p=index(buf,'"');
		if(p==NULL)
			break;
		memcpy(p,(p+1),len-(p-buf));
	}
	//����ո�
	while(1)
	{
		p=index(buf,' ');
		if(p==NULL)
			break;
		memcpy(p,(p+1),len-(p-buf));
	}
	//����Ʊ��
	while(1)
	{
		p=index(buf,'\t');
		if(p==NULL)
			break;
		memcpy(p,(p+1),len-(p-buf));
	}
	//�������
	while(1)
	{
		p=strchr(buf,'\n');
		if(p==NULL)
			break;
		memcpy(p,(p+1),len-(p-buf));
	}
	return 0;
}

 /**********************************************************************************************
 * ������	:confparser_load_lockfile()
 * ����		:����conf�ļ������ݽṹ������ס�ļ�,����ʱlockf�������ļ�ָ��
 * ����		:confname:Ҫ���򿪵�conf�ļ���
 * ����ֵ	:��������conf�ṹ��ָ�룬�Ժ��ø�ָ���������ļ��еı�����
 *				NULL��ʾ����
 **********************************************************************************************/
#include <errno.h>
confdict * confparser_load_lockfile(char * confname,int wait,FILE**lockf)
{
	FILE *fp;
	confdict *conf;
	int lock;

	if(confname==NULL)
	{
		return NULL;
	}
	//if(lockf==NULL)
	//	return NULL;
	*lockf=NULL;
	fp=fopen(confname,"r");
	if(fp==NULL)
	{
		return NULL;
	}
	lock=lock_file(fileno(fp),wait);
	fsync(fileno(fp));
	if(lock<0)
	{	
		fclose(fp);
		return NULL;
	}
	else
	{		
		conf=confparser_load(confname);
		if(conf==NULL)
		{
			fclose(fp);
		}
		
		else
			*lockf=fp;
		return conf;
	}
}


 /**********************************************************************************************
 * ������	:confparser_load()
 * ����	:��conf�ļ��л�ȡ���ݽṹ������ָ�����ݽṹ��ָ��
 * ����	:confname:Ҫ���򿪵�conf�ļ���
 * ����ֵ	:��������conf�ṹ��ָ�룬�Ժ��ø�ָ���������ļ��еı�����
 *				NULL��ʾ����,
 **********************************************************************************************/
confdict * confparser_load(char * confname)
{
        FILE *conf=NULL;
        int i;
        int lines=0;
        confdict * dict=NULL;
	 char *get=NULL;
        if(confname==NULL)
           	return NULL;
        conf=fopen(confname,"r");
        if(conf==NULL)
        {
        	printf("confparser_load open %s error!!\n",confname);
              return NULL;
        }
	dict=malloc(sizeof(confdict));
        if(dict==NULL)
	{
	 	fclose(conf);
		printf("confparser_load malloc failed!!\n");
		return NULL;
	}
		
	for(i=0;i<CONF_MAXLINE;i++)
	{
		get=fgets(dict->filebuf[i],MAX_CHAR_LINE,conf);
		if(get==NULL)
			break;
		else
		{
			process_line(dict->filebuf[i],MAX_CHAR_LINE);
			lines++;
		}
	}
	
	dict->lines=lines;
	fclose(conf);
	return dict;
}

/**********************************************************************************************
 * ������	:confparser_freedict()
 * ����		:�ͷ��Ѿ��ù��Ҳ���ʹ�õ����ݽṹ
 * ����		:dict:֮ǰ��confparser_load���ص�ָ��
 * ���		:��
 * ����ֵ	:��
 **********************************************************************************************/
void confparser_freedict(confdict *dict)
{
	if(dict!=NULL)
		free(dict);

}

 /**********************************************************************************************
 * ������	:confparser_getstring()
 * ����		:�����ݽṹ�л�ȡһ��ָ�����ֵı������ַ�����ʽ�������û�иñ����򷵻�def
 * ����		:dict:֮ǰ��confparser_load���ص�ָ��
 *			:key:������
 *			 def:Ĭ��ֵ
 * ���		:��
 * ����ֵ	:����ֵ�ַ���
 **********************************************************************************************/
char * confparser_getstring(confdict *dict,char * key, char * def)
{
	int i;
	char *p,*name=NULL,*val=NULL;
	char *q;
	int qflag;
	
	if((dict==NULL)||(key==NULL))
		return def;
	
	for(i=0;i<dict->lines;i++)
	{
		p=dict->filebuf[i];
		name=strstr(p,key);
		if((name==NULL)||(name!=p))
			continue;
		else
		{
			q=name+strlen(key);
			if(*q!='=')		//��������
				continue;
			if(*(q+1)=='\0') //��Ϊ"USER=" 
				return def;
			break;
		}
	}
	if((name==NULL)||(name!=p))
		return def;
	
	val=strchr(name,'=');
	if(val==NULL)
		return def;
	val++;
	return val;	
}


 /**********************************************************************************************
 * ������	:confparser_setstr()
 * ����	:��һ���ַ����������õ�ָ���ı�����
 * ����	:dict:֮ǰ��confparser_load���ص�ָ��
 *			:key:������
 *			 val:Ҫ���õ��ַ���ָ��
 * ���	:��
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/
int confparser_setstr(confdict *dict, char * key, char * val)
{
	char *oldval;
	int num;
	int i;
	char *p,*name=NULL;
	char *q;
	int qflag;
	if((dict==NULL)||(key==NULL))
		return -1;

	for(i=0;i<dict->lines;i++)
	{
		p=dict->filebuf[i];
		name=strstr(p,key);
		if(name==NULL)
			continue;
		else
		{
			q=name+strlen(key);
			if((*q!='=')&&(*q!=' '))		//��������
				continue;
			sprintf(dict->filebuf[i],"%s=%s",key,val);	//shixin changed 2006.11.16
			//sprintf(q+1,"%s",val);//�иý���
			return 0;
		}
	}
	//�޽���
	{
		num=dict->lines;
		sprintf(dict->filebuf[num],"%s=%s",key,val);
		dict->lines++;
	}
	return 0;
}
/**********************************************************************************************
 * ������	:confparser_getint()
 * ����	:�����ݽṹ�л�ȡһ�����εı���ֵ�����û���ҵ��򷵻�notfound
 * ����	:dict:֮ǰ��confparser_load���ص�ָ��
 *			:key:������
 *			 notfound:Ĭ��ֵ
 * ���	:��
 * ����ֵ	:������ֵ
 **********************************************************************************************/
int confparser_getint(confdict *dict, char * key, int notfound)
{
	char *str;
	if((dict==NULL)||(key==NULL))
		return notfound;
	str=confparser_getstring(dict,key,CONF_INVALID_KEY);
	if(str==CONF_INVALID_KEY)
		return notfound;
	else
		return atoi(str);
}

 /**********************************************************************************************
 * ������	:confparser_dump_conf()
 * ����	: �����ݽṹ�����һ���򿪵��ļ���
 * ����	:	name:�ļ�������
 			dict:֮ǰ��confparser_load���ص�ָ��
 *			 f:�Ѿ��򿪵��ļ���
 * ���	:��
 * ����ֵ	:��
 **********************************************************************************************/
void confparser_dump_conf(char *name,confdict* dict, FILE * f)
{
	int i,num;
	if((dict==NULL)||(f==NULL)||(name==NULL))
		return;
	//printf("confparser_dump_conf name=%s f=%x!!\n",name,f);
	f=freopen(name,"w",f);
	if(f==NULL)
		return;
	num=dict->lines;
	for(i=0;i<num;i++)
		{
			if(dict->filebuf[i][0]!='\0')
				fprintf(f,"%s\n",dict->filebuf[i]);
		}
	return;
}
#if 0
 /**********************************************************************************************
 * ������	:confparser_getstring_no_enter_no_space()
 * ����	: ��conf�ļ�ȡ���ַ������������Ŀո��س���or '#'!
 * ����	: dict:֮ǰ��confparser_load���ص�ָ��
 *		  key:������
 *		  def:Ĭ��ֵ		
 * ���	:��
 * ����ֵ	:����ֵ�ַ��� add by wsy
 **********************************************************************************************/
char *confparser_getstring_no_enter_no_space(confdict * dict, char * key, char * def)
{
	
	char *str=NULL;
	char *lp;
	str=confparser_getstring(dict,key,def);
	
	lp=index(str,'\n');
	if(lp!=NULL)
		*lp='\0';
	lp=index(str,' ');
	if(lp!=NULL)
		*lp='\0';
	lp=index(str,'#');
	if(lp!=NULL)
		*lp='\0';
	return str;
}

 /**********************************************************************************************
 * ������	:confparser_setstr_with_enter()
 * ����	:��һ���ַ����������õ�ָ���ı���������ĩβ���ϻس�
 * ����	:	dict:֮ǰ��confparser_load���ص�ָ��
 *			:key:������
 *			 val:Ҫ���õ��ַ���ָ��
 * ���	:��
 * ����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 **********************************************************************************************/

int confparser_setstr_with_enter(confdict * dict, char * key, char * val)
{
	char *lp;
	lp=index(val,'\n');
	if(lp==NULL)
		{
			lp=index(val,'\0');
			*lp='\n';
			lp++;
			*lp='\0';
		}
	confparser_setstr(dict,key,val);
	return 0;
}

int main(viod)
{
	int i;
	confdict *dict;
	dict=confparser_load("config");
	if(dict==NULL)
	{
		printf("open config failed!\n");
		exit(1);
	}
	else
		printf("open config success lines=%d\n",dict->lines);	
	printf("ETH0_IPADDR=%s\n",confparser_getstring(dict,"ETH0_IPADDR",NULL));
	printf("INTERNET_MODE=%d\n",confparser_getint(dict,"INTERNET_MODE",100));

	confparser_dump_conf(dict,stdout);

	return 0;
}
//by wsy, tried to patch confparser_functions
#endif
