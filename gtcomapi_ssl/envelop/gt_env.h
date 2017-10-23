/**
 * File:   gt_env.h
 * Author: Zhangtao
 * Date: 	2005-2-2
 * Description:	�����ŷ⹤��
 */
#ifndef _GT_ALARM_TAO_ENV_20050223_
#define _GT_ALARM_TAO_ENV_20050223_

#ifdef WIN32
#include <windows.h>
#include <string.h>
#ifdef ENVELOP_EXPORTS
#define ENVELOP_API __declspec(dllexport)
#else
#define ENVELOP_API __declspec(dllimport)
#endif
#else
#include <unistd.h>
#include <string.h>
#define ENVELOP_API
#endif

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define IN 
#define OUT

#define ERR_ENV_BUF_INVALID  1   ///< ��������Ļ��治����
#define ERR_ENV_CERT_INVALID 2   ///< ֤����Ч
#define ERR_ENV_KEY_INVALID  3   ///< ��Կ��Ч
#define ERR_ENV_MEMORY       4   ///< �����ڴ�ʧ��
#define ERR_ENV_DATA_EXCEED  5   ///< �������ݳ������
#define ERR_ENV_ENV_INVALID  6   ///< ��Ч���ŷ�
#define ERR_ENV_ENCRYPT_FAILED 7 ///< ����ʧ��
#define ERR_ENV_DECRYPT_FAILED 8 ///< ����ʧ��
#define ERR_ENV_NO_CERT	    9    ///< ֤�鲻����

#define ERR_CREATE_DIR  21
#define ERR_OPEN_FILE  22
#define ERR_WRITE_FILE 23

#ifdef __cplusplus
extern "C" {
#endif

///���֤���˽Կ�Ƿ����	
ENVELOP_API int env_check(
						 IN const char * cert, ///< ֤���ļ���
						 IN const char * key   ///< ˽Կ�ļ���
						 ); 

///ʹ��֤���˽Կ�ļ���ʼ���ŷ⺯����,��Ӧ�ó������ʱ����
ENVELOP_API int env_init(
						 IN const char * cert, ///< �Է���֤���ļ���
						 IN const char * key   ///< ������˽Կ�ļ���
						 ); 

///�ŷ���
ENVELOP_API int env_pack(
						 IN	int etype,    ///< �����㷨
						 IN char * pbuf,  ///< ����
						 IN int plen,     ///< ���ĳ���
						 IN char * ebuf,  ///< �����������(�ŷ�)�Ļ���
						 IN OUT int * elen///< ����(�ŷ�)����,����Ϊebuf�Ĵ�С,���Ϊʵ�ʵ����Ĵ�С
						 );

///�ŷ���
ENVELOP_API int env_unpack(
						   IN int etype,   ///< �����㷨
						   IN char * ebuf, ///< ����(�ŷ�)
						   IN int elen,    ///< ����(�ŷ�)�ĳ���
						   IN char * pbuf, ///< ����������ĵĻ���
						   IN OUT int * plen///< ���ĳ���,����Ϊpbuf�Ĵ�С,���Ϊʵ�ʵ����Ĵ�С
						   ); 

///�ͷ��ŷ⺯����,��Ӧ�ó����˳�ʱ����
ENVELOP_API void env_release();

#ifdef WIN32
///��ʹ��˽Կ��ʼ���ŷ⺯����
ENVELOP_API int env_init_only_key(
								  IN const char * key ///< ������˽Կ�ļ���
								  ); 

///���öԷ���֤���ļ�
ENVELOP_API int	env_set_peer_cert(
							  IN int id,   ///< �Է���ID
							  IN const char * name,///< �Է���֤���ļ���
							  IN const char * cert
							  );
///ָ���Է�ID,�����ŷ���
ENVELOP_API int env_pack_by_peer_id(
									IN int id,    ///< �Է���ID
									IN int etype,       ///< �����㷨
									IN char * pbuf,     ///< ����
									IN int plen,        ///< ���ĳ���
									IN char * ebuf,     ///< �����������(�ŷ�)�Ļ���f,
									IN OUT int *  elen  ///< ����(�ŷ�)����,����Ϊebuf�Ĵ�С,���Ϊʵ�ʵ����Ĵ�С
									);

int env_release_cert_list();
#endif
				 			
#ifdef __cplusplus
}
#endif
				 			
#endif//_GT_ALARM_TAO_ENV_20050223_

