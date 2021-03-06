#include <jv_common.h>
#include "hicommon.h"
#include <mpi_ai.h>
#include <mpi_adec.h>
#include <jv_ai.h>
#include <jv_ao.h>
#include "jv_gpio.h"
#define AUDIO_ADPCM_TYPE ADPCM_TYPE_DVI4/* ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4*/
#define G726_BPS MEDIA_G726_40K         /* MEDIA_G726_16K, MEDIA_G726_24K ... */

#define DEV_AO	0
static jv_audio_attr_t gAudioAttr;
/******************************************************************************
* function : Start Ao
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt,
        AIO_ATTR_S *pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, HI_BOOL bResampleEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    s32Ret = HI_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
               AoDevId,s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_AO_Enable(AoDevId);
    if(HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
        return HI_FAILURE;
    }

    for (i=0; i<s32AoChnCnt; i++)
    {
        s32Ret = HI_MPI_AO_EnableChn(AoDevId, i);
        if(HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
            return HI_FAILURE;
        }

        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, i);
            s32Ret |= HI_MPI_AO_EnableReSmp(AoDevId, i, enInSampleRate);
            if(HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, i, s32Ret);
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Stop Ao
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, HI_BOOL bResampleEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    for (i=0; i<s32AoChnCnt; i++)
    {
        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_DisableReSmp failed with %#x!\n", __FUNCTION__, s32Ret);
                return s32Ret;
            }
        }

        s32Ret = HI_MPI_AO_DisableChn(AoDevId, i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
    }

    s32Ret = HI_MPI_AO_Disable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Start Adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
    HI_S32 s32Ret;
    ADEC_CHN_ATTR_S stAdecAttr;

    stAdecAttr.enType = enType;
    stAdecAttr.u32BufSize = 20;
    stAdecAttr.enMode = ADEC_MODE_STREAM;/* propose use pack mode in your app */

    if (PT_ADPCMA == stAdecAttr.enType)
    {
        ADEC_ATTR_ADPCM_S stAdpcm;
        stAdecAttr.pValue = &stAdpcm;
        stAdpcm.enADPCMType = AUDIO_ADPCM_TYPE ;
    }
    else if (PT_G711A == stAdecAttr.enType || PT_G711U == stAdecAttr.enType)
    {
        ADEC_ATTR_G711_S stAdecG711;
        stAdecAttr.pValue = &stAdecG711;
    }
    else if (PT_G726 == stAdecAttr.enType)
    {
        ADEC_ATTR_G726_S stAdecG726;
        stAdecAttr.pValue = &stAdecG726;
        stAdecG726.enG726bps = G726_BPS ;
    }
    else if (PT_LPCM == stAdecAttr.enType)
    {
        ADEC_ATTR_LPCM_S stAdecLpcm;
        stAdecAttr.pValue = &stAdecLpcm;
        stAdecAttr.enMode = ADEC_MODE_PACK;/* lpcm must use pack mode */
    }
    else
    {
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAdecAttr.enType);
        return HI_FAILURE;
    }

    /* create adec chn*/
    s32Ret = HI_MPI_ADEC_CreateChn(AdChn, &stAdecAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_ADEC_CreateChn(%d) failed with %#x!\n", __FUNCTION__,\
               AdChn,s32Ret);
        return s32Ret;
    }
    return 0;
}

/******************************************************************************
* function : Stop Adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn)
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_ADEC_DestroyChn(AdChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_ADEC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
               AdChn, s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Ao bind Adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_ADEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = AdChn;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Ao unbind Adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_ADEC;
    stSrcChn.s32ChnId = AdChn;
    stSrcChn.s32DevId = 0;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn); 
}

int jv_ao_start(int channelid ,jv_audio_attr_t *attr)
{
	AIO_ATTR_S	stAioAttr= {0};
	memset(&gAudioAttr, 0, sizeof(jv_audio_attr_t));
	memcpy(&gAudioAttr, attr, sizeof(jv_audio_attr_t));
    /* init stAio. all of cases will use it */
    stAioAttr.enSamplerate = attr->sampleRate;//AUDIO_SAMPLE_RATE_8000;//
    stAioAttr.enBitwidth = attr->bitWidth;//AUDIO_BIT_WIDTH_16;//
    stAioAttr.enWorkmode = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
    stAioAttr.u32EXFlag = 0;
    stAioAttr.u32FrmNum = 30;
	if(attr->sampleRate == JV_AUDIO_SAMPLE_RATE_24000)
		stAioAttr.u32PtNumPerFrm = 320 * 3;
	else
		stAioAttr.u32PtNumPerFrm = 320;
    stAioAttr.u32ChnCnt = 1;
    stAioAttr.u32ClkSel = 0;
	PAYLOAD_TYPE_E payload;
	switch(attr->encType)
	{
	default:
	case JV_AUDIO_ENC_G711_A:
		payload = PT_G711A;
		break;
	case JV_AUDIO_ENC_G711_U:
		payload = PT_G711U;
		break;
	case JV_AUDIO_ENC_PCM:
		payload = PT_PCMU;
		break;
	}
//	payload = PT_LPCM;
	if(attr->encType != JV_AUDIO_ENC_PCM)
	{
	    CHECK_RET( SAMPLE_COMM_AUDIO_StartAo(DEV_AO, stAioAttr.u32ChnCnt, &stAioAttr, 0, HI_FALSE));
	    CHECK_RET( SAMPLE_COMM_AUDIO_StartAdec(0, payload));
	    CHECK_RET( SAMPLE_COMM_AUDIO_AoBindAdec(DEV_AO, 0, 0));
	}
	else
	{
	    CHECK_RET( SAMPLE_COMM_AUDIO_StartAo(DEV_AO, stAioAttr.u32ChnCnt, &stAioAttr, 0, HI_FALSE));
	    //CHECK_RET( SAMPLE_COMM_AUDIO_StartAdec(0, payload));
	    //CHECK_RET( SAMPLE_COMM_AUDIO_AoBindAdec(DEV_AO, 0, 0));
	}
//printf("...............ao start success..............\n");
	return 0;
}

int jv_ao_stop(int channelid)
{

	SAMPLE_COMM_AUDIO_AoUnbindAdec(DEV_AO, 0, 0);
	SAMPLE_COMM_AUDIO_StopAdec(0);
	SAMPLE_COMM_AUDIO_StopAo(DEV_AO, 1, HI_FALSE);
	HI_MPI_AO_ClrPubAttr(DEV_AO);
	return 0;
}

int jv_ao_send_frame(int channelid, jv_audio_frame_t *frame)
{
	int ret;
	if(gAudioAttr.encType == JV_AUDIO_ENC_PCM)
	{
		AUDIO_FRAME_S stFrame;
		if(gAudioAttr.sampleRate == JV_AUDIO_SAMPLE_RATE_8000)
			if(frame->u32Len != 640)
				return 0;
		if(gAudioAttr.sampleRate == JV_AUDIO_SAMPLE_RATE_24000)
			if(frame->u32Len != 640*3)
				return 0;
		memcpy(stFrame.pVirAddr[0], frame->aData, frame->u32Len);
		stFrame.enBitwidth = AUDIO_BIT_WIDTH_16;
		stFrame.enSoundmode = AUDIO_SOUND_MODE_MONO;
		stFrame.u32Len = frame->u32Len;
		stFrame.u32Seq = frame->u32Seq;
		ret = HI_MPI_AO_SendFrame(DEV_AO, 0, &stFrame, TRUE);
		if (ret != HI_SUCCESS)
		{
			printf("HI_MPI_AO_SendFrame failed: %x,frame->u32Len=%d\n", ret,frame->u32Len);
			return -1;
		}
	}
	else
	{
		AUDIO_STREAM_S stStream;
		unsigned char aData[JV_MAX_AUDIO_FRAME_LEN];

		stStream.pStream = aData;
		unsigned char header[4] = {0,1,0xa0, 0};
		header[3] = frame->u32Len/2;

		memcpy(aData, header, 4);
		memcpy(&aData[4], frame->aData, frame->u32Len);
		stStream.u32Len = frame->u32Len+4;
		stStream.u32Seq = frame->u32Seq;
		ret = HI_MPI_ADEC_SendStream(DEV_AO, &stStream, TRUE);
		if (ret != HI_SUCCESS)
		{
			printf("send faile: %x\n", ret);
			return -1;
		}
	}
	return 0;
}

int jv_ao_get_status(int channelid, jv_ao_status *aoStatus)
{
	AO_CHN_STATE_S stStatus;
	int ret = HI_MPI_AO_QueryChnStat(DEV_AO, channelid, &stStatus);
	if (ret != HI_SUCCESS)
	{
		printf("HI_MPI_AO_QueryChnStat failed: %x\n", ret);
		return -1;
	}
	aoStatus->aoBufBusyNum = stStatus.u32ChnBusyNum;
	aoStatus->aoBufFreeNum = stStatus.u32ChnFreeNum;
	aoStatus->aoBufTotalNum = stStatus.u32ChnTotalNum;
	return 0;
}

int jv_ao_mute(BOOL bMute)
{
	return 0;
}

BOOL jv_ao_get_mute()
{
	return TRUE;
}

int jv_ao_ctrl(int volCtrl)
{
	return 0;
}

int jv_ao_get_attr(int channelid, jv_audio_attr_t *attr)
{
	memcpy(attr, &gAudioAttr, sizeof(jv_audio_attr_t));
	return 0;
}
