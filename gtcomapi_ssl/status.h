//�����豸״̬�ṹ

#ifndef STATUS_H
#define STATUS_H

#ifndef DWORD
#define DWORD	unsigned long
#endif

struct per_state_struct{			//GT1000��������״̬
	unsigned 		video_loss0		:1;	//bit0��Ƶ���붪ʧ
	unsigned 		video_loss1		:1;
	unsigned		video_loss2		:1;
	unsigned 		video_loss3		:1;
	unsigned 		video_loss4		:1;
	unsigned 		video_loss5		:1;
	unsigned 		video_loss6		:1;
	unsigned 		video_loss7		:1;
	unsigned 		video_loss8		:1;
	unsigned 		video_loss9		:1;
	unsigned 		video_loss10	:1;
	unsigned 		video_loss11	:1;
	unsigned		video_blind0	:1;//bit12 ��1·��Ƶ�ڵ�
	unsigned		video_blind1	:1;
	unsigned 		video_blind2	:1;
	unsigned 		video_blind3	:1;
	unsigned		audio_loss0		:1;	//bit16 ��Ƶ���붪ʧ
	unsigned		audio_loss1		:1;
	unsigned		audio_loss2		:1;
	unsigned		audio_loss3		:1;
	unsigned		disk_full		:1;	//������
	unsigned 		keyboard_err	:1;	//�������ӹ���
	unsigned		err_connect_xvs	:1;	//�����ⲿdvs����
	unsigned		video_blind4to15:1;	//��4-15·��Ƶ�ڵ�
	unsigned 		pwr_loss		:1;	//��Ӵ����õ�Դ����
	unsigned		audio_out_err	:1;	//��Ƶ�������
	unsigned		video_loss12	:1;	//��Ƶ��ʧ
	unsigned		video_loss13	:1;	//��Ƶ��ʧ
	unsigned		video_loss14	:1;	//��Ƶ��ʧ
	unsigned		video_loss15	:1;	//��Ƶ��ʧ
	unsigned		upnp_err	:1; //upnp�˿�ӳ���쳣
	unsigned 		xvslogin_err	:1;	 //��¼���xvs�ʺŴ���	
};
struct dev_state_struct{		//GT1000ϵͳ�ڲ�״̬
	unsigned		link_err		:1;	//����
	unsigned		mem_err			:1;	//�ڴ����
	unsigned		flash_err		:1;	//flash����
	unsigned		hd_err			:1;	//Ӳ�̹���	//��λ������
	unsigned		cf_err			:1;	//�洢������
	unsigned		audio_dec_err	:1;	//��Ƶ�����豸����
	unsigned		reserve			:2;
	unsigned		video_enc0_err	:1;	//netenc
	unsigned		video_enc1_err	:1;	//hq0
	unsigned		video_enc2_err	:1;	//hq1
	unsigned		video_enc3_err	:1;	//hq2
	unsigned		video_enc4_err	:1;	//hq3
	//06.09.06	�¼ӹ���
	unsigned		quad_dev_err	:1;	//����ָ�������
	unsigned 		watch_51_err	:1;	//51ģ�����
	unsigned		reserve1		:17;
};

struct	trig_state_struct{	//GT1000ϵͳ����״̬
	unsigned		trig0			:1;//�ⴥ��
	unsigned		trig1			:1;
	unsigned		trig2			:1;
	unsigned		trig3			:1;
	unsigned		trig4			:1;
	unsigned		trig5			:1;
	unsigned		trig6			:1;//�𶯴���//trig_vib
	unsigned		trig7			:1;//�ⲿ������Դ//trig_pwr
	unsigned		trig8			:1;
	unsigned		trig9			:1;
	unsigned		motion0			:1;//�ƶ�����
	unsigned		motion1			:1;
	unsigned		motion2			:1;
	unsigned		motion3			:1;
	unsigned		motion4			:1;
	unsigned		motion5			:1;
	unsigned		motion6			:1;
	unsigned		motion7			:1;
	unsigned		motion8			:1;
	unsigned		motion9			:1;
	unsigned		motion10		:1;
	unsigned		motion11		:1;
	unsigned		motion12		:1;
	unsigned		motion13		:1;
	unsigned		motion14		:1;
	unsigned		motion15		:1;
	unsigned		trig10			:1; //����10
	unsigned		trig11			:1;
	unsigned		trig12			:1;
	unsigned		trig13			:1;
	unsigned		trig14			:1;
	unsigned		trig15			:1;
};
#endif
