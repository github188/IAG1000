#ifdef __cplusplus 
extern "C" { 
#endif
typedef struct _NaluUnit  
{ 
	int type;  
    int size;  
	unsigned char *data;  
}NaluUnit;

/**
 * ��ʼ�������ӵ�������
 *
 * @param url �������϶�Ӧwebapp�ĵ�ַ
 *					
 * @�ɹ��򷵻�1 , ʧ���򷵻�0
 */ 
int RTMP264_Connect(const char* url);    
    
/**
 * ���ڴ��е�һ��H.264�������Ƶ��������RTMPЭ�鷢�͵�������
 *
 * @param read_buffer �ص������������ݲ����ʱ��ϵͳ���Զ����øú�����ȡ�������ݡ�
 *					2���������ܣ�
 *					uint8_t *buf���ⲿ���������õ�ַ
 *					int buf_size���ⲿ���ݴ�С
 *					����ֵ���ɹ���ȡ���ڴ��С
 * @�ɹ��򷵻�1 , ʧ���򷵻�0
 */ 
//int RTMP264_Send(int fd,int (*read_buffer)(int fd,unsigned char *buf, int buf_size));
int RTMP264_Send(NaluUnit *nalu, int buf_size);

/**
 * �Ͽ����ӣ��ͷ���ص���Դ��
 *
 */    
void RTMP264_Close();  

#ifdef __cplusplus 
}
#endif
