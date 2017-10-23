/*	Ƕ��ʽ��Ƶ��������������ͨѶ�ĵײ㺯����
 *		
 *		 2005.1
 *    ���������ṩ��ʵ��Ƕ��ʽϵͳ��Զ�̼����ͨѶ�ĵײ�ӿڣ�
 *    �ϲ��Ӧ�ýӿڼ����ݽṹ�����ڱ����ʵ�ַ�Χ
 *    ���������ṩ�Ĺ�����
 *    1.���������ݱ�����GT1000��ͨѶЭ���ʽ���д�������ܣ�ǩ����crc32У�飬֮���͸�ָ����tcp�˿�
 *    2.��ָ����tcp�˿ڽ������ݰ�������crcУ�飬��֤ǩ���������ı任�����ķ��ظ����ý���                        
 *    ���յ�ʱ�򱾿����ͨ��Э��ͷ�еĶ����ж����ݰ���ʹ�õ�ǩ����ʽ�����ܷ�ʽ��
 *    ���͵�ʱ����ݲ���ѡ��ǩ����ʽ�����ܷ�ʽ��	
 */
/*
 * �������ݽṹ����
 */
#ifndef GT_COM_API_H
#define GT_COM_API_H
//#include <typedefine.h>
//#include <gt_errlist.h>

#ifdef __cplusplus
extern "C" {
#endif
#undef EXPORT_DLL
#ifdef _WIN32
	//windows ʹ��

	#define EXPORT_DLL __declspec(dllexport)

#else

	//linux ʹ��

	#define EXPORT_DLL

#endif


#define 	GT_COM_VERSION	2			//�汾��
#define	printerr	printf


//MSG_TYPE define
#define	GT_CMD_NO_AUTH	0			//����ʹ�����Ĵ���
#define   MSG_AUTH_SSL		0x60		//����ʹ�������ŷ⴫��

#define 	SYS_MSG_TYPE		GT_CMD_NO_AUTH


//ENCRYPT_TYPE define
//���ݼ�������ѡ��
#define GT_CMD_NO_ENCRYPT	0		//�޼���
#define DES_EDE_CBC			0x01	//des-ede-cbc des-ede-cbc
#define DES_EDE3_CBC		0x02	//des_ede3_cbc des3.des-ede3-cbc
#define IDEA_CBC			0x03	//idea_cbc idea.idea-cbc
#define RC2_CBC				0x04	//rc2_cbc rc2.rc2-cbc
#define BF_CBC				0x05	//bf_cbc bf.blowfish.bf-cbc
#define CAST5_CBC			0x06	//cast5_cbc cast.cast-cbc.cast5-cbc
#define RC5_32_12_16_CBC	0x07	//rc5_32_12_16_cbc rc5.rc5-cbc


#ifndef BYTE
#define BYTE	unsigned char
#define WORD	unsigned short
#define DWORD	unsigned long
#endif



/**************�ײ�Э�鶨��************************************/
struct gt_pkt_head{		//������İ�ͷ�ṹ����
	BYTE	sync_head[4];	//���ݰ�ͬ��ͷ,
	WORD	pkt_len;			//ȥ��sync_head��pkt_len�Լ�tail�����ֶκ����ݰ��ĳ��ȣ��ֽڣ�
	BYTE	version;			//ͨѶЭ��İ汾��
	BYTE	msg_type;		//��֤����
	BYTE	encrypt_type;	//��������
	BYTE	reserve0;		//�����ֶ�
	WORD	reserve1;		//�����ֶ�
	DWORD	xor32;				//���ݰ�ͷ��xor32У����(У���pkt_len��encrypt_len)
};
struct gt_pkt_struct{		//���ݰ��ṹ
	struct gt_pkt_head head;	//���ݰ��İ�ͷ�ṹ
	BYTE	msg[2];			//���ݰ�����,�����Ǳ䳤������ֻ�ܶ����������ʽ,ʹ�õ�ʱ����Ҫ�����������ǿ��ת����ָ������ṹ��ָ��;
	//�������ݰ��Ǳ䳤��,����ͨѶЭ���е����¼����ֶζ���������msg��
	//DWORD	crc32;		//������ĵ�һ���ֽڵ�����ǩ����CRC32У����
	//BYTE	tail[4];			//���ݰ���β��־	
};
#define SIZEOF_GT_USR_CMD_STRUCT 10 
struct gt_usr_cmd_struct{	//PC��GT1000�豸ͨѶ������ṹ
	WORD	len;				//cmd�Լ��������Ч�ֶεĳ���
	WORD	cmd;			//����
	BYTE	en_ack;			//�Ƿ���Ҫ��Ӧ��־
	BYTE	reserve0;		//����
	WORD	reserve1;
	BYTE	para[2];			//����
};
/*************************************************************************/


/***********************�����ӿڶ���************************************/

 //�û����зǼ��ܷ��ͣ��������ݵ��õļ򻯽ӿ�

#define gt_cmd_send_noencrypt(fd,buf,len,tbuf,flag)       gt_cmd_pkt_send(fd,(struct gt_pkt_struct *)buf,len,tbuf,flag,GT_CMD_NO_AUTH,GT_CMD_NO_ENCRYPT)
//�������ݰ�
#define gt_cmd_pkt_recv(fd,recv,buf_len,tempbuf,flag)	 gt_cmd_pkt_recv_env(fd,recv,buf_len,tempbuf,flag,NULL,NULL)
#define gt_cmd_recv 									 gt_cmd_pkt_recv	




 /**********************************************************************************************
 * ������	:gt_cmd_pkt_send()
 * ����	:��һ�����İ���ָ���ļ���,ǩ����ʽ�γ��������ݰ�,���͵�ָ����socket
 * ����	:fd:Ŀ��socket�ļ�������
 *			 send:ָ��Ҫ���͵�����Ļ�������ָ��
 *			 msg_len:Ҫ���͵Ļ������е���Ч��Ϣ�����ֽ���(msg�ֶ�,������crc,tail�ȵĳ���)
 *			 tempbuf:(����)������ʱ��NULL�Ϳ�����
 *			 flag:(����)������ʱ��0�Ϳ�����
 *			 msg_type:�������ݰ�����֤����,0��ʾ����Ҫ��֤,MSG_AUTH_SSL��ʾʹ�������ŷ⣬����ֵ����
 *			 encrypt_type:�������ݰ��ļ�������,���嶨���"���ݼ�������ѡ��"
 * ����ֵ	:��ֵ��ʾ���ͳ�ȥ���ֽ�������ֵ��ʾ����
 * ע		: ������Ӧ�������㹻��Ļ�����������Ҫ���͵�����,��������ֱ��ʹ�øû��������и������,�任�Ȳ���
 *	  		  ������ֻ�����send->msg�ֶμ��ɣ������ֶ��ɿ⺯���Լ����
 *	 		 send�Ի���������ʼ��ַ������4�ֽڶ���ģ������ϲ�Ӧ�ô�����ܻ������
 **********************************************************************************************/
EXPORT_DLL int  gt_cmd_pkt_send(int fd,struct gt_pkt_struct *send,int msg_len,void *tempbuf,int flag,BYTE msg_type,BYTE encrypt_type);


/*
 * ������: gt_cmd_pkt_recv()
 * ����:  ��ָ����socket����һ�����ݰ�,���н���,��֤ǩ���Ȳ�����,��������䵽recvָ��Ļ�����
 * ����ֵ:
 *         0:���յ�һ������
 *         ��ֵ:����
 *		-1:�����ݴ����������������Ѿ��Ͽ�		
 *      -2:���������
 *		-3:CRC��
 *		-4:��֧�ֵļ��ܸ�ʽ
 *		-5:���ܴ���
 *
 *         	����ֵ����
 *         recv:�Ѿ����õ����ݻ�����
 * ����:
 *         fd:  Ŀ��socket�ļ�������
 *         recv:ָ��Ҫ��Ž������ݵĻ�����,������Ӧ����һ��������������buffer
 *         buf_len:recv�������Ĵ�С,���������ݴ������ֵ��ʱ��,������Ӧ�÷��س�����Ϣ,��ʾ���������(�������Ӧ�ò��ᷢ��)
 *         flag:��Ҫ��ʲô������ִ�б�����,����ֵ��linux socket��̵���Ӧ����������ͬ,ֻҪԭ�����͸�socket��������
 *	     tempbuf:(����)������ʱ��NULL�Ϳ�����
 * ˵��:   ������Ӧ�������㹻��Ļ���������ñ�����,������������ֽ��յ����ݴ��ڻ�������С,Ӧ�����ػ����������Ϣ
 * 	   ������ֻ����ķ���ֵ��recv->msg�ֶμ��ɣ������ֶζԵ�������˵����Ч��
 *	   recv�Լ�tempbuf��������������ʼ��ַ������4�ֽڶ���ģ������ϲ�Ӧ�ô�����ܻ������
 */

EXPORT_DLL int  gt_cmd_pkt_recv_env(int fd,struct gt_pkt_struct *recv,int buf_len,void *tempbuf,int flag,int *msg_type,int *encrypt_type);

/**********************************************************************************************
 * ������	:env_init()
 * ����	:��ʼ���������ͨѶ�õ������ŷ⣬��ϵͳ��ʼ����ʱ�����һ��
 * ����	:cert:֤���ļ�
 *			 key:˽Կ�ļ�
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
 **********************************************************************************************/
int env_init(const char * cert,const char * key);
///���֤���˽Կ�Ƿ����       ^M
int env_check(const char * cert, const char * key); 

#ifdef __cplusplus
}
#endif

#endif
