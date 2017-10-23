#ifndef REMOTE_FILE_H
#define REMOTE_FILE_H
#include <gt_com_api.h>
#include "mod_socket.h"
//�����ش��������ļ�ʹ�õ��ļ���ʽ 
/*�����豸�����ļ� USR_RW_DEV_PARA
ʹ�ô���������д�豸�Ĳ��������ļ���Ŀǰֻ�ڹ��̰�װ�������ʹ�ã�,
�豸�յ��������Ӧ����DEV_PARA_RETURN����
   0x0108 USR_RW_DEV_PARA FTP�ļ�����
  {
  	  type(2)         ���ͣ�2:ip1004.ini 3:alarm.ini����ֵ������
	  mode(2)		����ģʽ:0���� 1��д
	  filelen(4)		�ļ�����(��дģʽ����Ч,���Ȳ��ܳ���60k)
	  file(n)			�ļ�����(����дģʽ����Ч)
}

*/
int usr_rw_para_file(int fd,struct gt_usr_cmd_struct *cmd,int env,int enc,int dev_no);
/**********************************************************************************************
 * ������	:send_para_to_rmt()
 * ����	:���豸�������ļ����͸�Զ�̼����
 * ����	:fd ���ӵ�Զ�̼�������ļ���������fd�������Ǹ�ֵ
 *			 type:�ļ� ֵ����ͬstruct usr_rwdevpara_struct�еĶ��� 
 *         			��2:ip1004.ini 3:alarm.ini����ֵ������
 *			env:ʹ�õ������ŷ��ʽ
 *			enc:Ҫʹ�õļ��ܸ�ʽ
 *			enack:�Ƿ���Ҫȷ��
 *����ֵ	:0��ʾ�ɹ���ֵ��ʾʧ��
 *			: -10 ��������
 *		   	: -11��֧�ֵĸ�ʽ
 **********************************************************************************************/
int send_para_to_rmt(int fd,int type,int env,int enc,int enack,int dev_no);

#endif
