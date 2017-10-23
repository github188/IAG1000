/*
 *  GT1000��������Զ�̼����ͨѶ���õ��Ĵ����붨��
 *
 */

#ifndef GT_ERRLIST_H
#define GT_ERRLIST_H

#ifdef _WIN32			//add by scott
#include <windows.h>
//#define __inline__
#endif //_WIN32

#include <errno.h>

//��Ƕ��ʽ�豸�����Ĵ�����
#define RESULT_SUCCESS			0	//�ɹ�
#define ERR_DVC_INTERNAL		0x1001	//�豸�ڲ���
#define ERR_DVC_INVALID_REQ	0x1002	//�ͻ��������ݸ�ʽ��
#define ERR_DVC_BUSY			0X1003	//�豸æ
#define ERR_DVC_FAILURE		0x1004  //�豸����
#define ERR_EVC_CRC_ERR		0x1005	//�豸�յ�һ��crc��������ݰ�
#define ERR_EVC_NOT_SUPPORT	0x1006  //�豸�յ�һ����֧�ֵ�����
#define ERR_ENC_NOT_ALLOW		0x1007  //�豸�յ�һ�������������
#define ERR_DVC_NO_RECORD_INDEX			0x1008	//�豸û�в�ѯ������
#define ERR_DVC_NO_DISK		0x1009	//�豸û��Ӳ�� 
#define	ERR_DVC_NO_AUDIO	ERR_EVC_NOT_SUPPORT
#define	ERR_DVC_NO_TRIG		ERR_EVC_NOT_SUPPORT

//update errortype definition 
#define ERR_DVC_INVALID_NAME	0x1010  //�����ļ����ָ�ʽ����
#define ERR_DVC_LOGIN_FTP    	0x1011  //�޷���¼ftp������
#define ERR_DVC_NO_FILE      		0x1012  //ftp����������ָ�����ļ����û������޶�Ȩ��
#define ERR_DVC_UNTAR        		0x1013  //��ѹ�ļ�ʧ��
#define ERR_NO_SPACE         		0x1014  //�豸�洢�ռ䲻�����޷�����
#define ERR_DVC_PKT_NO_MATCH	0x1015	//���������豸�ͺŲ�ƥ��
#define ERR_DVC_UPDATE_FILE	0x1016	//�����豸�ļ�(֤�飬�����ļ�)����
#define ERR_DVC_WRONG_SIZE	0x1017		//�ļ���С����

//��xvs��صĴ�����
#define ERR_XVS_NOT_ACCESSABLE	0x2001	//��Χ�豸���ɼ����޷�����

/**********************************************************************************************
 * ������	:get_gt_errname()
 * ����	:����Զ�̼����ͨѶ�Ĵ�����ת��Ϊ�ַ�����ֵ
 * ����	:err:������
 * ����ֵ	:��������ַ�������
 **********************************************************************************************/
#ifdef _WIN32
static  char* get_gt_errname(WORD err)
#else
static __inline__ char* get_gt_errname(WORD err)
#endif
{
	switch(err)
	{
		case RESULT_SUCCESS:				//�ɹ�
			return "RESULT_SUCCESS";
		break;
		case ERR_DVC_INTERNAL:				//�豸����??
			return "ERR_DVC_INTERNAL";
		break;
		case ERR_DVC_INVALID_REQ:			//�ͻ��������ݸ�ʽ��
			return "ERR_DVC_INVALID_REQ";
		break;
		case ERR_DVC_BUSY:
			return "ERR_DVC_BUSY";
		break;
		case ERR_DVC_FAILURE:
			return "ERR_DVC_FAILURE";
		break;
		case ERR_EVC_CRC_ERR:
			return "ERR_EVC_CRC_ERR";
		break;
		case ERR_EVC_NOT_SUPPORT:
			return "ERR_EVC_NOT_SUPPORT";
		break;
		case ERR_ENC_NOT_ALLOW:
			return "ERR_ENC_NOT_ALLOW";
		break;
		case ERR_DVC_NO_RECORD_INDEX:
			return "ERR_DVC_NO_RECORD_INDEX";
		break;





		case ERR_DVC_INVALID_NAME:
			return "ERR_DVC_INVALID_NAME";
		break;
		case ERR_DVC_LOGIN_FTP:
			return "ERR_DVC_LOGIN_FTP";
		break;
		case ERR_DVC_NO_FILE:
			return "ERR_DVC_NO_FILE";
		break;
		case ERR_DVC_UNTAR:
			return "ERR_DVC_UNTAR";
		break;
		case ERR_NO_SPACE:
			return "ERR_NO_SPACE";
		break;
		case ERR_DVC_PKT_NO_MATCH:
			return "ERR_DVC_PKT_NO_MATCH";
		break;
		case ERR_DVC_WRONG_SIZE:
			return "ERR_DVC_WRONG_SIZE";
		break;
		case ERR_XVS_NOT_ACCESSABLE:
			return "ERR_XVS_NOT_ACCESSABLE";
		break;	
	}

	return "UNKNOW_ERR";
}



#endif



