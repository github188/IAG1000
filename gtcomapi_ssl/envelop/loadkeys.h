/*************************************************************
File:   loadkeys.h
Author: Zhangtao 
Date: 	2005-2-23
Description:����֤���˽Կ
*************************************************************/

#ifndef _GT_ALARM_TAO_LOADKEYS_20050223_
#define _GT_ALARM_TAO_LOADKEYS_20050223_

#include <openssl/evp.h>

//��֤���ļ�(PEM��ʽ)�ж�ȡ��Կ
EVP_PKEY * read_pub_key(const char *certfile);

//��˽Կ�ļ�(PEM��ʽ)�ж�ȡ��Կ
EVP_PKEY * read_private_key(const char *keyfile);

#endif   //_GT_ALARM_TAO_LOADKEYS_20050223_

