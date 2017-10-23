#include <stdlib.h>
#include "ime6410api.h"
#include <string.h>
#include "AVIEncoder.h"
#include <mdebug.h>
#include <unistd.h>
#include <errno.h>
#define DBLOCKSIZE			512
#define SIZE_AVI_MOVHEADER		12
#define SIZE_AVI_IDXHEADER		8
#define SIZE_JUNKHDR	8
#define SIZE_CHUNK_HDR  8
#define  VHDR_SCALE				100
#define  AHDR_SAMPLEPERFRAME	1152.	// sample/frame
/**********************************************************************************************
 * 函数名	:FormatAviHeadBufDef()
 * 功能	:avi的简要结构填充一个avi文件头缓冲区
 * 输入	:defval:avi简要结构 指针
 *			:hb_len:目标缓冲区的 长度 
 * 输出	:hb:被填充的目标缓冲区
 * 返回值	:0表示成功负值表示出错
 **********************************************************************************************/
int FormatAviHeadBufDef(struct defAVIVal *defval,unsigned char *hb,int hb_len)
{
#define SIZE_AVI_MOVHEADER		12
#define SIZE_AVI_IDXHEADER		8

#define SIZE_JUNKHDR	8


#define SIZE_CHUNK_HDR  8

#define  VHDR_SCALE				100
#define  AHDR_SAMPLEPERFRAME	1152.	// sample/frame
	AVIMOVHeader *AVI_MOVHeader;
	AVIFixHeader *AVI_FixHeader;	
	struct defAVIVal *pval;
	struct JUNK_HDR junk_hdr;
	int size_AVI_FixHeader;
	if(defval==NULL)
		return -1;
	if(hb==NULL)
		return -1;
	if(hb_len<(MOVI_POS_OFFSET+SIZE_AVI_MOVHEADER))
		return -1;
	pval=defval;	
	// AVI Fixed Header..
	memset(hb,0,sizeof(AVIFixHeader));
	// Video Header
	AVI_FixHeader=(AVIFixHeader *)hb;
	AVI_FixHeader->strh_vd_id = mmioFOURCC('s', 't', 'r', 'h');
	AVI_FixHeader->strh_vd_siz = sizeof(AVISTREAMHEADER);
	AVI_FixHeader->strh_vd.fccType = mmioFOURCC('v', 'i', 'd', 's');
       if(defval->v_avitag[0]<'0')   ///不是字母、数字,可能是旧版本的应用程序
       {        
	        AVI_FixHeader->strh_vd.fccHandler = mmioFOURCC('D','I','V','X');
        }
       else
               AVI_FixHeader->strh_vd.fccHandler = mmioFOURCC(defval->v_avitag[0],defval->v_avitag[1],defval->v_avitag[2],defval->v_avitag[3]); 
	AVI_FixHeader->strh_vd.dwScale = VHDR_SCALE;
	AVI_FixHeader->strh_vd.dwRate=pval->v_frate*VHDR_SCALE;
	AVI_FixHeader->strh_vd.dwSuggestedBufferSize  = pval->v_buffsize;	
	AVI_FixHeader->strh_vd.rcFrame[0] = 0;
	AVI_FixHeader->strh_vd.rcFrame[1] = 0;
	AVI_FixHeader->strh_vd.rcFrame[2] = pval->v_width;
	AVI_FixHeader->strh_vd.rcFrame[3] = pval->v_height;

	// Video Format
	AVI_FixHeader->strf_vd_id = mmioFOURCC('s', 't', 'r', 'f');
	AVI_FixHeader->strf_vd_siz = sizeof(BITMAPINFOHEADER);
	AVI_FixHeader->strf_vd.biSize = sizeof(BITMAPINFOHEADER);
	AVI_FixHeader->strf_vd.biWidth = pval->v_width;
	AVI_FixHeader->strf_vd.biHeight = pval->v_height;
	AVI_FixHeader->strf_vd.biPlanes = 1;
	AVI_FixHeader->strf_vd.biBitCount = 24;

       if(defval->v_avitag[0]<'0')   ///不是字母、数字,可能是旧版本的应用程序
       {        
        	memcpy(&AVI_FixHeader->strf_vd.biCompression, "divx", 4);
        }
        else
        {
             memcpy(&AVI_FixHeader->strf_vd.biCompression, defval->v_avitag, 4);   
        }

    
	AVI_FixHeader->strf_vd.biSizeImage = (pval->v_width)*(pval->v_height)*3;
	AVI_FixHeader->strf_vd.biXPelsPerMeter = 0;
	AVI_FixHeader->strf_vd.biYPelsPerMeter = 0;
	AVI_FixHeader->strf_vd.biClrUsed = 0;
	AVI_FixHeader->strf_vd.biClrImportant = 0;

	// Video List Form
	AVI_FixHeader->strl_vd_id = mmioFOURCC('L', 'I', 'S', 'T');
	AVI_FixHeader->strl_vd_siz = AVI_FixHeader->strh_vd_siz + AVI_FixHeader->strf_vd_siz + 20;	
	AVI_FixHeader->strl_vd_name = mmioFOURCC('s', 't', 'r', 'l');

	// Audio Header
	AVI_FixHeader->strh_au_id = mmioFOURCC('s', 't', 'r', 'h');
	AVI_FixHeader->strh_au_siz = sizeof(AVISTREAMHEADER);
	AVI_FixHeader->strh_au.fccType = mmioFOURCC('a', 'u', 'd', 's');
	AVI_FixHeader->strh_au.fccHandler = 0;
	if(pval->a_wformat == WFORMAT_ADPCM) {
		AVI_FixHeader->strh_au.dwScale = 508;
		AVI_FixHeader->strh_au.dwRate = 4027;
	//	AVI_FixHeader->strh_au.dwLength = 0;
		AVI_FixHeader->strh_au.dwLength = 114;
		AVI_FixHeader->strh_au.dwSampleSize = 508;	
	}
	else {
		AVI_FixHeader->strh_au.dwScale = 1;
		AVI_FixHeader->strh_au.dwRate = pval->a_sampling;
	}
	AVI_FixHeader->strh_au.dwSuggestedBufferSize = (DBLOCKSIZE-4) *(pval->a_nr_frame);

	// Audio Format
	AVI_FixHeader->strf_au_id = mmioFOURCC('s', 't', 'r', 'f');
	if(pval->a_wformat == WFORMAT_ADPCM) {
		// ADPCM
		// DBLOCKSIZE = 512, HEADER = 4
		// nAudioSamplePerFrame = (DBLOCKSIZE-HEADER)*2+1
		// nAudioSampleRate = a_sampling
		// nAudioBitrate = a_bitrate (mono = 32216, stereo = 64000)
		// nAudioFrameCnt = 1
		// nAudioFrameSize = (nAudioBitrate*nAudioSamplePerFrame/
		//                nAudioSampleRate + 7)/8
		// nBlockAlign = wBitsPerSample * nChannels / 8;
		// nAvgBytesPerSec = a_bitrate / 8;
		// fwHeadLayer = nAudioSampleSize*nAudioSampleRate
		// 				/nAvgBytesPerSec

	//	printf("ADPCM, sizeof(ADWAVEFORMATEX) = %d\n",
		//		sizeof(ADWAVEFORMATEX));
		AVI_FixHeader->strf_au_siz = sizeof(ADWAVEFORMATEX);
		AVI_FixHeader->strf_au.wFormatTag = WFORMAT_ADPCM;
		AVI_FixHeader->strf_au.nChannels = pval->a_channel;
		AVI_FixHeader->strf_au.nSamplesPerSec = pval->a_sampling;
		AVI_FixHeader->strf_au.wBitsPerSample = 4;
		AVI_FixHeader->strf_au.nBlockAlign = 508;
		AVI_FixHeader->strf_au.nAvgBytesPerSec = 4027;
		AVI_FixHeader->strf_au.cbSize = 2;
		AVI_FixHeader->strf_au.wSamplesPerBlock = 1009;
	}
	else {
	//	printf("PCM, sizeof(ADWAVEFORMATEX) = %d\n",
		//		sizeof(ADWAVEFORMATEX));
		// U-LAW PCM
		// nAvgBytesPerSec = nSamplesPerSec * nBlockAlign
		//printf("sizeof(WAVEFORMATEX)=%d sizeof(ADWAVEFORMATEX)=%d\n",sizeof(WAVEFORMATEX),sizeof(ADWAVEFORMATEX));
		AVI_FixHeader->strf_au_siz =sizeof(ADWAVEFORMATEX);// - 2;//changed by shixin 
		//AVI_FixHeader->strf_au.wFormatTag = WFORMAT_ULAW;
		AVI_FixHeader->strf_au.wFormatTag = pval->a_wformat;
		
		AVI_FixHeader->strf_au.nChannels = pval->a_channel;
		AVI_FixHeader->strf_au.nSamplesPerSec = pval->a_sampling;
		//AVI_FixHeader->strf_au.wBitsPerSample = 16;
		AVI_FixHeader->strf_au.wBitsPerSample = pval->a_bitrate;
		AVI_FixHeader->strf_au.nBlockAlign = 1;
		AVI_FixHeader->strf_au.nAvgBytesPerSec = AVI_FixHeader->strf_au.nBlockAlign * AVI_FixHeader->strf_au.nSamplesPerSec;
		AVI_FixHeader->strf_au.cbSize = 0;
	}

	// Audio List Form
	AVI_FixHeader->strl_au_id = mmioFOURCC('L', 'I', 'S', 'T');
	AVI_FixHeader->strl_au_siz = AVI_FixHeader->strh_au_siz + AVI_FixHeader->strf_au_siz + 20;	
	AVI_FixHeader->strl_au_name = mmioFOURCC('s', 't', 'r', 'l');

	// AVI Header
	AVI_FixHeader->avih_id = mmioFOURCC('a', 'v', 'i', 'h');
	AVI_FixHeader->avih_siz = sizeof(AVIMAINHEADER);

	AVI_FixHeader->avih.dwMicroSecPerFrame = (unsigned long)(1000000./AVI_FixHeader->strh_vd.dwRate*AVI_FixHeader->strh_vd.dwScale + 0.5);

	
	AVI_FixHeader->avih.dwFlags = AVIF_HASINDEX | AVIF_ISINTERLEAVED;
	AVI_FixHeader->avih.dwTotalFrames = 0;
	AVI_FixHeader->avih.dwStreams = pval->nr_stream;
	AVI_FixHeader->avih.dwSuggestedBufferSize = pval->v_buffsize;
	AVI_FixHeader->avih.dwWidth = pval->v_width;
	AVI_FixHeader->avih.dwHeight = pval->v_height;

	// Header List Form
	if(pval->nr_stream == 1)
	{
		// Video Only
		size_AVI_FixHeader = sizeof(AVIFixHeader) - 28 
				- sizeof(AVISTREAMHEADER)
				- sizeof(ADWAVEFORMATEX);
		AVI_FixHeader->hdlr_siz = AVI_FixHeader->avih_siz 
				+ AVI_FixHeader->strl_vd_siz + 20;
	}
	else {
		// Video & Audio
		size_AVI_FixHeader = sizeof(AVIFixHeader)
			  - sizeof(ADWAVEFORMATEX)
			  + AVI_FixHeader->strf_au_siz;
		AVI_FixHeader->hdlr_siz = AVI_FixHeader->avih_siz
			+ AVI_FixHeader->strl_vd_siz
			+ AVI_FixHeader->strl_au_siz + 28;
	}

	AVI_FixHeader->hdlr_id = mmioFOURCC('L', 'I', 'S', 'T');
	AVI_FixHeader->hdlr_name = mmioFOURCC('h', 'd', 'r', 'l');

	// RIFF Header Form
	AVI_FixHeader->riff_id = mmioFOURCC('R', 'I', 'F', 'F');
	AVI_FixHeader->riff_siz = 0;
	AVI_FixHeader->riff_type = mmioFOURCC('A', 'V', 'I', ' ');


	//printf("size_AVI_FixHeader=%d \n",size_AVI_FixHeader);
	// JUNK Header
	//junk_hdr=(struct JUNK_HDR *)&hb[size_AVI_FixHeader];
	junk_hdr.junk_id = mmioFOURCC('J', 'U', 'N', 'K');
	junk_hdr.junk_siz = MOVI_POS_OFFSET - AVI_FixHeader->hdlr_siz - 28;	// Riff Header.. hdrl Header
	memcpy(&hb[size_AVI_FixHeader],(char*)&junk_hdr,8);


	
	// AVI MOV Header  
	AVI_MOVHeader=(AVIMOVHeader*)(hb+MOVI_POS_OFFSET);
	AVI_MOVHeader->movi_id = mmioFOURCC('L', 'I', 'S', 'T');
	AVI_MOVHeader->movi_siz = 0;
	AVI_MOVHeader->movi_name = mmioFOURCC('m', 'o', 'v', 'i');
	
	//AVI MOV Header 不传送给客户端
	return (MOVI_POS_OFFSET);//+SIZE_AVI_MOVHEADER);
}

