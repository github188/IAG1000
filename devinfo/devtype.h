#ifndef DEV_TYPE_H
#define DEV_TYPE_H
#include <devinfo.h>
#include "subdevdef.h"
typedef struct{//GTϵ��DVSR��������
        int     type;           //#�豸�ͺ�
        char *comment;  //���ڸ��ͺŵ�˵��
        int     trignum;        //#�豸��߱���������
        int     outnum;         //���������
        int     com;            //#�豸����������Ҫ�ڽ���������
        int     quad;           //#�Ƿ��л���ָ�����1��ʾ�� 0��ʾû��
        int     osd;            //�Ƿ���osd,1��ʾ��,0��ʾû��
        int     videonum;       //#ϵͳ�����Ƶ������(���ƶ�����й�),��quad=0ʱvideonumӦ��Ϊ1 
	 int     videoencnum;    //#��Ƶ����������
        int     hqencnum;       //¼���ͨ����
	 int	   use_cpu_iic;		//ʹ��cpu��iic�ܽſ���tw9903��Ƶ������
	 int     ide;            //1��ʾcf����Ӳ�̵���Ŀ 0��ʾû��,2��ʾ��SD��,3��ʾTF��
	 	int 	audionum;			//#��ʾ����ͨ���ĸ�����0��ʾû��
        int     eth_port;       //#������ 1��ʾһ�� 2��ʾ����
        int     list_num;       //���豸�б��е���Ч���豸��
        DevType_T **list;       //���豸�б�
}GTSeriesDVSR;

GTSeriesDVSR *get_dvsr_by_typestr(char *dev_type_str);
int conv_dev_str2type(char *type_str);
char *conv_dev_type2str(int type);


#endif
