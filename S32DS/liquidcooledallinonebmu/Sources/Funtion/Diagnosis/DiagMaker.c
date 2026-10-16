/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : DiagMaker.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池故障诊断模块启动滤波接口
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "CurrSample.h"
#include "HigAFESamp.h"
#include "LowADCSamp.h"
#include "PortCheck.h"
#include "InsuCheck.h"
#include "TacUser.h"
#include "EnerInterface.h"
#include "SocInterface.h"
#include "DiagInterface.h"
#include "DiagUser.h"
#include "DiagFilter.h"
#include "DiagMaker.h"
#include "DiagCheck.h"
#include "AuxDevTactic.h"
#include "AuxDevMsg.h"      //keppowermode调用
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u16 gGBmuGenPara_102[];                              /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];                           /*主控高级参数值*/
extern u8 gGHardPara_104[];
extern u16 gTFAirStateInfo_213[];            //keeppowermode调用
//by lxr01202
static u8 sPcsFanDelayCounter = 0;  // Pcs风扇延迟检测计数器
static u16 sPduFanDelayCounter = 0;  // Pdu延迟计数器（100ms周期)
static u8 sPduFanDelayFlag = FALSE;   // PDUFan延迟标志位
static u8 sPcsFanDelayFlag = FALSE;   // PCSFan延迟标志位

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void DiagVoltLimFilMakerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电压限制类故障诊断任务
** @The notes : 100ms周期
**===============================================================================================*/
void DiagVoltLimFilMakerTask(void)
{
    u16 maxVolt = 0;
    u16 minVolt = 0;

    /*单体电压异常不检测高低压*/
    if(1 == DiagGetCellVoltExpStateHOOK())
    {
        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CEL_VH1))
        {
            /*清除单体高压1级*/
        	DiagSetErrFilStateAPI(eERR_ID_CEL_VH1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CEL_VH2))
        {
            /*清除单体高压2级*/
        	DiagSetErrFilStateAPI(eERR_ID_CEL_VH2, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CEL_VL1))
        {
            /*清除单体低压1级*/
        	DiagSetErrFilStateAPI(eERR_ID_CEL_VL1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CEL_VL2))
        {
            /*清除单体低压2级*/
        	DiagSetErrFilStateAPI(eERR_ID_CEL_VL2, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CV_DIF1))
        {
            /*清除单体压差过大1级*/
        	DiagSetErrFilStateAPI(eERR_ID_CV_DIF1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CV_DIF2))
        {
            /*清除单体压差过大2级*/
        	DiagSetErrFilStateAPI(eERR_ID_CV_DIF2, eErr_A);
        }
    }
    else
    {
    	/*组单体最大电压/最小电压*/
		maxVolt = GetGCellMaxVoltAPI();
		minVolt = GetGCellMinVoltAPI();

		/*判断单体电压高2级-ID0*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CEL_VH1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_CEL_VH2, eErr_B);
		}
		/*到达单体电压高2级*/
		else if(maxVolt >= gGBmuGenPara_102[eBmuGenPara102_VH2Lim])
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VH2, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(maxVolt <= (gGBmuGenPara_102[eBmuGenPara102_VH2Lim] - gGBmuGenPara_102[eBmuGenPara102_CVH2Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VH2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CEL_VH2))
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VH2, eErr_A);
		}

		/*判断单体电压高1级-ID16*/
		if(maxVolt >= gGBmuGenPara_102[eBmuGenPara102_VH1Lim])
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_CEL_VH2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_CEL_VH1, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(maxVolt <= (gGBmuGenPara_102[eBmuGenPara102_VH1Lim] - gGBmuGenPara_102[eBmuGenPara102_CVH1Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VH1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CEL_VH1))
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VH1, eErr_A);
		}

		/*判断单体电压低2级-ID1*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CEL_VL1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_CEL_VL2, eErr_B);
		}
		/*达到单体电压低2级*/
		else if(minVolt <= gGBmuGenPara_102[eBmuGenPara102_VL2Lim])
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VL2, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(minVolt >= (gGBmuGenPara_102[eBmuGenPara102_VL2Lim] + gGBmuGenPara_102[eBmuGenPara102_CVL2Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VL2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CEL_VL2))
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VL2, eErr_A);
		}

		/*判断单体电压低1级-ID17*/
		if(minVolt <= gGBmuGenPara_102[eBmuGenPara102_VL1Lim])
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_CEL_VL2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_CEL_VL1, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(minVolt >= (gGBmuGenPara_102[eBmuGenPara102_VL1Lim] + gGBmuGenPara_102[eBmuGenPara102_CVL1Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VL1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CEL_VL1))
		{
			DiagSetErrNowStateAPI(eERR_ID_CEL_VL1, eErr_A);
		}

		/*单体电压一致下降2级-ID12*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CV_DIF1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_CV_DIF2, eErr_B);
		}
		/*达到单体电压一致下降2级*/
		else if(ABS(maxVolt, minVolt) >= DiagGetCellVDiffSubVHOOK(gGBmuGenPara_102[eBmuGenPara102_VDiff2SubV]))
		{
			DiagSetErrNowStateAPI(eERR_ID_CV_DIF2, eErr_B);
		}
		/*在回差点内则清除故障标志*/
		else if(ABS(maxVolt, minVolt) <= (gGBmuGenPara_102[eBmuGenPara102_VDiff2SubV] - 20))
		{
			DiagSetErrNowStateAPI(eERR_ID_CV_DIF2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CV_DIF2))
		{
			DiagSetErrNowStateAPI(eERR_ID_CV_DIF2, eErr_A);
		}

		/*单体电压一致下降1级-ID28*/
		if(ABS(maxVolt, minVolt) >= DiagGetCellVDiffSubVHOOK(gGBmuGenPara_102[eBmuGenPara102_VDiff1SubV]))
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_CV_DIF2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_CV_DIF1, eErr_B);
		}
		/*在回差点内则清除故障标志*/
		else if(ABS(maxVolt, minVolt) <= gGBmuGenPara_102[eBmuGenPara102_VDiff2SubV])
		{
			DiagSetErrNowStateAPI(eERR_ID_CV_DIF1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CV_DIF1))
		{
			DiagSetErrNowStateAPI(eERR_ID_CV_DIF1, eErr_A);
		}
    }

    /*总压异常不检测总压高低压*/
    if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_HSAMP_EXP))
        || (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_HSAMP_EXP))
		|| (GetGSampSumVoltAPI() < gGBmuHigLevPara_103[eBmuHigLevPara103_HigVOffV]))
    {
    	/*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_SUM_VH1))
        {
            /*清除总压高压1级*/
        	DiagSetErrFilStateAPI(eERR_ID_SUM_VH1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_SUM_VH2))
        {
            /*清除总压高压2级*/
        	DiagSetErrFilStateAPI(eERR_ID_SUM_VH2, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_SUM_VL1))
        {
            /*清除总压高压1级*/
        	DiagSetErrFilStateAPI(eERR_ID_SUM_VL1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_SUM_VL2))
        {
            /*清除总压高压1级*/
        	DiagSetErrFilStateAPI(eERR_ID_SUM_VL2, eErr_A);
        }
    }
    else
    {
		/*判断总压高2级-ID2*/
    	if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_SUM_VH1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_SUM_VH2, eErr_B);
		}
		/*达到总压高2级*/
		else if(GetGSampSumVoltAPI() >= gGBmuGenPara_102[eBmuGenPara102_SumVH2Lim])
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VH2, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampSumVoltAPI() <= (gGBmuGenPara_102[eBmuGenPara102_SumVH2Lim] - gGBmuGenPara_102[eBmuGenPara102_SVH2Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VH2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_SUM_VH2))
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VH2, eErr_A);
		}

		/*判断总压高1级-ID18*/
		if(GetGSampSumVoltAPI() >= gGBmuGenPara_102[eBmuGenPara102_SumVH1Lim])
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_SUM_VH2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_SUM_VH1, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampSumVoltAPI() <= (gGBmuGenPara_102[eBmuGenPara102_SumVH1Lim] - gGBmuGenPara_102[eBmuGenPara102_SVH1Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VH1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_SUM_VH1))
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VH1, eErr_A);
		}

		/*判断总压低2级-ID3*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_SUM_VL1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_SUM_VL2, eErr_B);
		}
		/*达到总压低2级*/
		else if(GetGSampSumVoltAPI() <= gGBmuGenPara_102[eBmuGenPara102_SumVL2Lim])
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VL2, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampSumVoltAPI() >= (gGBmuGenPara_102[eBmuGenPara102_SumVL2Lim] + gGBmuGenPara_102[eBmuGenPara102_SVL2Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VL2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_SUM_VL2))
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VL2, eErr_A);
		}

		/*判断总压低1级-ID19*/
		if(GetGSampSumVoltAPI() <= gGBmuGenPara_102[eBmuGenPara102_SumVL1Lim])
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_SUM_VL2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_SUM_VL1, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampSumVoltAPI() >= (gGBmuGenPara_102[eBmuGenPara102_SumVL1Lim] + gGBmuGenPara_102[eBmuGenPara102_SVL1Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VL1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_SUM_VL1))
		{
			DiagSetErrNowStateAPI(eERR_ID_SUM_VL1, eErr_A);
		}
    }

    /*绝缘监测异常不检测绝缘告警*/
    if(3 == DiagGetDcSideLeakFlagAPI())
    {
    	/*产生绝缘采样异常告警-ID71*/
		DiagSetErrNowStateAPI(eERR_ID_LEAK_EXP, eErr_B);

    	/*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_LEAK1))
        {
            /*清除绝缘1级*/
        	DiagSetErrFilStateAPI(eERR_ID_LEAK1, eErr_A);
        }

    	/*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_LEAK2))
        {
            /*清除绝缘2级*/
        	DiagSetErrFilStateAPI(eERR_ID_LEAK2, eErr_A);
        }
    }
    else
    {
    	/*消除绝缘采样异常告警-ID71*/
		DiagSetErrNowStateAPI(eERR_ID_LEAK_EXP, eErr_A);

		/*判断绝缘1级-ID27*/
		if(1 == DiagGetDcSideLeakFlagAPI())
		{
			DiagSetErrNowStateAPI(eERR_ID_LEAK2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_LEAK1, eErr_B);
		}
		/*判断绝缘2级-ID11*/
		else if(2 == DiagGetDcSideLeakFlagAPI())
		{
			DiagSetErrNowStateAPI(eERR_ID_LEAK2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_LEAK1, eErr_A);
		}
		/*绝缘正常*/
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_LEAK1, eErr_A);

			/*一级消除二级才能消除*/
			if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_LEAK1))
			{
				DiagSetErrNowStateAPI(eERR_ID_LEAK2, eErr_A);
			}
		}
    }
}

/*=================================================================================================
** @Name      : void DiagTempLimFilMakerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 温度限制类故障诊断任务
** @The notes : 100ms周期
**===============================================================================================*/
void DiagTempLimFilMakerTask(void)
{
    s16 maxTemp = 0;
    s16 minTemp = 0;
	//keeppowermode
	s16 limit_TH1_Keep = 600;    //保供电充电高温1级保护温度点
	s16 thresh_ChgTH1=0;

    /*单体温度异常不检测高低温*/
    if(1 == DiagGetCellTempExpStateHOOK())
    {
        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CHG_TH1))
        {
            /*清除充电高温1级*/
        	DiagSetErrFilStateAPI(eERR_ID_CHG_TH1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CHG_TH2))
        {
            /*清除充电高温1级*/
        	DiagSetErrFilStateAPI(eERR_ID_CHG_TH2, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_DHG_TH1))
        {
            /*清除放电高温1级*/
        	DiagSetErrFilStateAPI(eERR_ID_DHG_TH1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_DHG_TH2))
        {
            /*清除放电高温1级*/
        	DiagSetErrFilStateAPI(eERR_ID_DHG_TH2, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CHG_TL1))
        {
            /*清除充电低温1级*/
        	DiagSetErrFilStateAPI(eERR_ID_CHG_TL1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CHG_TL2))
        {
            /*清除充电低温2级*/
        	DiagSetErrFilStateAPI(eERR_ID_CHG_TL2, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_DHG_TL1))
        {
            /*清除放电低温1级*/
        	DiagSetErrFilStateAPI(eERR_ID_DHG_TL1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_DHG_TL2))
        {
            /*清除放电低温1级*/
        	DiagSetErrFilStateAPI(eERR_ID_DHG_TL2, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CT_DIF1))
        {
            /*清除单体温差过大1级*/
        	DiagSetErrFilStateAPI(eERR_ID_CT_DIF1, eErr_A);
        }
    }
    else
    {
    	/*组单体最大温度/最小温度*/
		maxTemp = GetGCellMaxTempAPI();
		minTemp = GetGCellMinTempAPI();

		/*判断充电高温2级-ID4*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CHG_TH1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_CHG_TH2, eErr_B);
		}
		/*达到充电高温2级*/
		else if(maxTemp >= (gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim] * 10))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TH2, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(maxTemp <= ((s16)gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim] - gGBmuGenPara_102[eBmuGenPara102_CTH2Clr]) * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TH2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CHG_TH2))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TH2, eErr_A);
		}

		/*判断充电高温1级-ID20*/
		thresh_ChgTH1 = (1 == MU2KeepPowerMode) ? limit_TH1_Keep : (gGBmuGenPara_102[eBmuGenPara102_ChgTH1Lim] * 10);   //通过保供电模式判断取值
		if(maxTemp >= thresh_ChgTH1)
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_CHG_TH2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_CHG_TH1, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(maxTemp <= (thresh_ChgTH1 - gGBmuGenPara_102[eBmuGenPara102_CTH1Clr]) * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TH1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CHG_TH1))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TH1, eErr_A);
		}

		/*判断放电高温2级-ID5*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_DHG_TH1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_DHG_TH2, eErr_B);
		}
		/*达到放电高温2级*/
		else if(maxTemp >= (gGBmuGenPara_102[eBmuGenPara102_DhgTH2Lim] * 10))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TH2, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(maxTemp <= ((s16)gGBmuGenPara_102[eBmuGenPara102_DhgTH2Lim] - gGBmuGenPara_102[eBmuGenPara102_DTH2Clr]) * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TH2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_DHG_TH2))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TH2, eErr_A);
		}

		/*判断放电高温1级-ID21*/
		if(maxTemp >= (gGBmuGenPara_102[eBmuGenPara102_DhgTH1Lim] * 10))
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_DHG_TH2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_DHG_TH1, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(maxTemp <= ((s16)gGBmuGenPara_102[eBmuGenPara102_DhgTH1Lim] - gGBmuGenPara_102[eBmuGenPara102_DTH1Clr]) * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TH1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_DHG_TH1))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TH1, eErr_A);
		}

		/*判断充电低温2级-ID6*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CHG_TL1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_CHG_TL2, eErr_B);
		}
		/*达到充电低温2级*/
		else if(minTemp <= (s16)(gGBmuGenPara_102[eBmuGenPara102_ChgTL2Lim] * 10))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TL2, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(minTemp >= ((s16)gGBmuGenPara_102[eBmuGenPara102_ChgTL2Lim] + gGBmuGenPara_102[eBmuGenPara102_CTL2Clr]) * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TL2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CHG_TL2))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TL2, eErr_A);
		}

		/*判断充电低温1级-ID22*/
		if(minTemp <= (s16)(gGBmuGenPara_102[eBmuGenPara102_ChgTL1Lim] * 10))
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_CHG_TL2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_CHG_TL1, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(minTemp >= ((s16)gGBmuGenPara_102[eBmuGenPara102_ChgTL1Lim] + gGBmuGenPara_102[eBmuGenPara102_CTL1Clr]) * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TL1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CHG_TL1))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_TL1, eErr_A);
		}

		/*判断放电低温2级-ID7*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_DHG_TL1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_DHG_TL2, eErr_B);
		}
		/*达到放电低温2级*/
		else if(minTemp <= (s16)(gGBmuGenPara_102[eBmuGenPara102_DhgTL2Lim] * 10))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TL2, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(minTemp >= ((s16)gGBmuGenPara_102[eBmuGenPara102_DhgTL2Lim] + gGBmuGenPara_102[eBmuGenPara102_DTL2Clr]) * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TL2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_DHG_TL2))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TL2, eErr_A);
		}

		/*判断放电低温1级-ID23*/
		if(minTemp <= (s16)(gGBmuGenPara_102[eBmuGenPara102_DhgTL1Lim] * 10))
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_DHG_TL2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_DHG_TL1, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(minTemp >= ((s16)gGBmuGenPara_102[eBmuGenPara102_DhgTL1Lim] + gGBmuGenPara_102[eBmuGenPara102_DTL1Clr]) * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TL1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_DHG_TL1))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_TL1, eErr_A);
		}

		/*单体温度温差过大2级-ID13*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CT_DIF1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_CT_DIF2, eErr_B);
		}
		/*达到单体温度温差过大2级*/
		else if(ABS(maxTemp, minTemp) >= (gGBmuGenPara_102[eBmuGenPara102_TDiff2SubT] * 10))
		{
			DiagSetErrNowStateAPI(eERR_ID_CT_DIF2, eErr_B);
		}
		/*在回差点内则清除故障标志*/
		else if(ABS(maxTemp, minTemp) <= ((gGBmuGenPara_102[eBmuGenPara102_TDiff2SubT] - 3) * 10))
		{
			DiagSetErrNowStateAPI(eERR_ID_CT_DIF2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CT_DIF2))
		{
			DiagSetErrNowStateAPI(eERR_ID_CT_DIF2, eErr_A);
		}

		/*单体温度温差过大1级-ID29*/
		if(ABS(maxTemp, minTemp) >= (gGBmuGenPara_102[eBmuGenPara102_TDiff1SubT] * 10))
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_CT_DIF2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_CT_DIF1, eErr_B);
		}
		/*在回差点内则清除故障标志*/
		else if(ABS(maxTemp, minTemp) <= ((gGBmuGenPara_102[eBmuGenPara102_TDiff1SubT] - 3) * 10))
		{
			DiagSetErrNowStateAPI(eERR_ID_CT_DIF1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CT_DIF1))
		{
			DiagSetErrNowStateAPI(eERR_ID_CT_DIF1, eErr_A);
		}

		/*判断均衡温度异常-ID36*/
		if(GetGBalanMaxTempAPI() >= DIAG_BALAN_PRO_T * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_BAL_TH, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(GetGBalanMaxTempAPI() <= ((s16)DIAG_BALAN_PRO_T - 10) * 10)
		{
			DiagSetErrNowStateAPI(eERR_ID_BAL_TH, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_BAL_TH))
		{
			DiagSetErrNowStateAPI(eERR_ID_BAL_TH, eErr_A);
		}
    }

    /*外设温度异常不检测外设过温*/
    if(((eErr_B == DiagGetErrFilStateAPI(eERR_ID_TSAMP_EXP))
        || (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_TSAMP_EXP)))
    /*使能IMEU2通信交互*/
    #if(1 == BMU_IMEU_COM_EN)
    /*IMEU2通信中断*/
        && (1 == DiagGetPerImeuOffCommuStateAPI())
    #endif
        )
    {
        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_PER_TH1))
        {
            /*清除外围高温1级*/
        	DiagSetErrFilStateAPI(eERR_ID_PER_TH1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_PER_TH2))
        {
            /*清除外围高温2级*/
        	DiagSetErrFilStateAPI(eERR_ID_PER_TH2, eErr_A);
        }
    }
    else
    {
		/*判断外围高温2级(环境温度或分流器温度)-ID10*/
    	if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_PER_TH1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_PER_TH2, eErr_B);
		}
		/*达到外围高温2级(环境温度或分流器温度)*/
		else if((eErr_A == DiagGetErrFilStateAPI(eERR_ID_TSAMP_EXP))
			&& ((GetGEnvirTempAPI() >= gGBmuGenPara_102[eBmuGenPara102_Envir2Lim])
			|| (GetGShuntSampTempAPI() >= gGBmuGenPara_102[eBmuGenPara102_Perip2Lim])))
		{
			DiagSetErrNowStateAPI(eERR_ID_PER_TH2, eErr_B);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_PER_TH2))
		{
			DiagSetErrNowStateAPI(eERR_ID_PER_TH2, eErr_A);
		}

		/*判断外围高温1级(环境温度或分流器温度)-ID26*/
		if((eErr_A == DiagGetErrFilStateAPI(eERR_ID_TSAMP_EXP))
			&& ((GetGEnvirTempAPI() >= gGBmuGenPara_102[eBmuGenPara102_Envir1Lim])
			|| (GetGShuntSampTempAPI() >= gGBmuGenPara_102[eBmuGenPara102_Perip1Lim])))
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_PER_TH2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_PER_TH1, eErr_B);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_PER_TH1))
		{
			DiagSetErrNowStateAPI(eERR_ID_PER_TH1, eErr_A);
		}
    }

    /*低压侧采集异常不检测电源过压*/
    if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_LSAMP_EXP))
    	|| (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_LSAMP_EXP)))
    {
        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_DCV_TH))
        {
            /*清除低压电源过温告警*/
        	DiagSetErrFilStateAPI(eERR_ID_DCV_TH, eErr_A);
        }
    }
    else
    {
		/*判断低压电源过温-ID37*/
		if(GetGDcPwrSampTempAPI() >= DIAG_LVPWR_WAR_T)
		{
			DiagSetErrNowStateAPI(eERR_ID_DCV_TH, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(GetGDcPwrSampTempAPI() <= ((s16)DIAG_LVPWR_WAR_T - 10))
		{
			DiagSetErrNowStateAPI(eERR_ID_DCV_TH, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_DCV_TH))
		{
			DiagSetErrNowStateAPI(eERR_ID_DCV_TH, eErr_A);
		}
    }
}

/*=================================================================================================
** @Name      : void DiagCurrLimFilMakerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电流限制类故障诊断任务
** @The notes : 100ms周期
**===============================================================================================*/
void DiagCurrLimFilMakerTask(void)
{
    /*电流采样异常不检测电流相关告警*/
    if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_CSAMP_ERR))
        || (eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CSAMP_ERR)))
    {
        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CHG_CO1))
        {
            /*清除充电过流1级*/
        	DiagSetErrFilStateAPI(eERR_ID_CHG_CO1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CHG_CO2))
        {
            /*清除充电过流2级*/
        	DiagSetErrFilStateAPI(eERR_ID_CHG_CO2, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_DHG_CO1))
        {
            /*清除放电过流1级*/
        	DiagSetErrFilStateAPI(eERR_ID_DHG_CO1, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_DHG_CO2))
        {
            /*清除放电过流1级*/
        	DiagSetErrFilStateAPI(eERR_ID_DHG_CO2, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CHG_COT))
        {
            /*清除充电电流响应超时*/
        	DiagSetErrFilStateAPI(eERR_ID_CHG_COT, eErr_A);
        }

        /*处于过渡阶段需要恢复正常*/
        if(eErr_AtoB == DiagGetErrFilStateAPI(eERR_ID_CHG_COT))
        {
            /*清除放电电流响应超时*/
        	DiagSetErrFilStateAPI(eERR_ID_DHG_COT, eErr_A);
        }
    }
    else
    {
		/*判断充电过流2级-ID8*/
    	if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CHG_CO1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_CHG_CO2, eErr_B);
		}
		/*达到充电过流2级*/
		else if(GetGSampOutCurrAPI() <= (0 - (s16)gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim]))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_CO2, eErr_B);

			/*达到充电过流极限值则立即产生故障*/
			if(GetGSampOutCurrAPI() <= (0 - (s16)BATT_EXEREM_MAXC))
			{
				DiagSetErrFilStateAPI(eERR_ID_CHG_CO2, eErr_B);
			}
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampOutCurrAPI() >= (0 - (s16)gGBmuGenPara_102[eBmuGenPara102_ChgCH2Lim] + (s16)gGBmuGenPara_102[eBmuGenPara102_ChgCH2Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_CO2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CHG_CO2))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_CO2, eErr_A);
		}

		/*判断充电过流1级-ID24*/
		if(GetGSampOutCurrAPI() <= (0 - (s16)gGBmuGenPara_102[eBmuGenPara102_ChgCH1Lim]))
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_CHG_CO2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_CHG_CO1, eErr_B);

			/*达到充电过流极限值则立即产生故障*/
			if(GetGSampOutCurrAPI() <= (0 - (s16)BATT_EXEREM_MAXC))
			{
				DiagSetErrFilStateAPI(eERR_ID_CHG_CO2, eErr_B);
				DiagSetErrFilStateAPI(eERR_ID_CHG_CO1, eErr_B);
			}
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampOutCurrAPI() >= (0 - (s16)gGBmuGenPara_102[eBmuGenPara102_ChgCH1Lim] + (s16)gGBmuGenPara_102[eBmuGenPara102_ChgCH1Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_CO1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CHG_CO1))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_CO1, eErr_A);
		}

		/*判断放电过流2级-ID9*/
		if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_DHG_CO1))
		{
			/*一级已经发生(未消除)确保二级也发生(不消除)*/
			DiagSetErrFilStateAPI(eERR_ID_DHG_CO2, eErr_B);
		}
		/*达到放电过流2级*/
		else if(GetGSampOutCurrAPI() >= gGBmuGenPara_102[eBmuGenPara102_DhgCH2Lim])
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_CO2, eErr_B);

			/*达到放电过流极限值则立即产生故障*/
			if(GetGSampOutCurrAPI() >= BATT_EXEREM_MAXC)
			{
				DiagSetErrFilStateAPI(eERR_ID_DHG_CO2, eErr_B);
			}
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampOutCurrAPI() <= ((s16)gGBmuGenPara_102[eBmuGenPara102_DhgCH2Lim] - gGBmuGenPara_102[eBmuGenPara102_DhgCH2Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_CO2, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_DHG_CO2))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_CO2, eErr_A);
		}

		/*判断放电过流1级-ID25*/
		if(GetGSampOutCurrAPI() >= gGBmuGenPara_102[eBmuGenPara102_DhgCH1Lim])
		{
			/*一级产生确保二级也要产生*/
			DiagSetErrNowStateAPI(eERR_ID_DHG_CO2, eErr_B);
			DiagSetErrNowStateAPI(eERR_ID_DHG_CO1, eErr_B);

			/*达到放电过流极限值则立即产生故障*/
			if(GetGSampOutCurrAPI() >= BATT_EXEREM_MAXC)
			{
				DiagSetErrFilStateAPI(eERR_ID_DHG_CO2, eErr_B);
				DiagSetErrFilStateAPI(eERR_ID_DHG_CO1, eErr_B);
			}
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampOutCurrAPI() <= ((s16)gGBmuGenPara_102[eBmuGenPara102_DhgCH1Lim] - gGBmuGenPara_102[eBmuGenPara102_DhgCH1Clr]))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_CO1, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_DHG_CO1))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_CO1, eErr_A);
		}

		/*判断充电电流响应超时-ID34*/
		if(GetGChgLimCurrAPI() <= 50)
		{
			/*请求小于5A而输入大于10A*/
			if(GetGSampOutCurrAPI() <= (-100))
			{
				DiagSetErrNowStateAPI(eERR_ID_CHG_COT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_CHG_COT, eErr_A);
			}
		}
		/*判断充电电流响应超时(允许误差5A)*/
		else if(GetGSampOutCurrAPI() <= (0 - (s16)GetGChgLimCurrAPI() - 50))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_COT, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampOutCurrAPI() >= (0 - (s16)GetGChgLimCurrAPI()))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_COT, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CHG_COT))
		{
			DiagSetErrNowStateAPI(eERR_ID_CHG_COT, eErr_A);
		}

		/*判断放电电流响应超时-ID35*/
		if(GetGDhgLimCurrAPI() <= 50)
		{
			/*请求小于5A而输入大于10A*/
			if(GetGSampOutCurrAPI() >= 100)
			{
				DiagSetErrNowStateAPI(eERR_ID_DHG_COT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_DHG_COT, eErr_A);
			}
		}
		/*判断充电电流响应超时(允许误差5A)*/
		else if(GetGSampOutCurrAPI() >= (GetGDhgLimCurrAPI() + 50))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_COT, eErr_B);
		}
		/*在回差点内则清除告警标志*/
		else if(GetGSampOutCurrAPI() <= GetGDhgLimCurrAPI())
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_COT, eErr_A);
		}
		/*在回差区间未产生告警则清除告警标志*/
		else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_DHG_COT))
		{
			DiagSetErrNowStateAPI(eERR_ID_DHG_COT, eErr_A);
		}
    }

	/*判断SOC过高2级-ID14*/
    if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CAP_SH1))
	{
		/*一级已经发生(未消除)确保二级也发生(不消除)*/
		DiagSetErrFilStateAPI(eERR_ID_CAP_SH2, eErr_B);
	}
	/*达到SOC过高2级*/
	else if(GetGBattSocAPI() >= gGBmuGenPara_102[eBmuGenPara102_CapHig2Soc])
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SH2, eErr_B);
	}
	/*在回差点内则清除告警标志*/
	else if((GetGBattSocAPI() <= (gGBmuGenPara_102[eBmuGenPara102_CapHig2Soc] - 50))
		&& (GetGBattSocAPI() <= (gGBmuGenPara_102[eBmuGenPara102_CapHig1Soc] - gGBmuGenPara_102[eBmuGenPara102_ChgLimSubSoc])))
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SH2, eErr_A);
	}
	/*在回差区间未产生告警则清除告警标志*/
	else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CAP_SH2))
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SH2, eErr_A);
	}

	/*判断SOC过高1级-ID30*/
	if((GetGBattSocAPI() >= gGBmuGenPara_102[eBmuGenPara102_CapHig1Soc])
		&& (1000 != gGBmuGenPara_102[eBmuGenPara102_CapHig1Soc]))
	{
		/*一级产生确保二级也要产生*/
		DiagSetErrNowStateAPI(eERR_ID_CAP_SH2, eErr_B);
		DiagSetErrNowStateAPI(eERR_ID_CAP_SH1, eErr_B);
	}
	/*在回差点内则清除告警标志*/
	else if(GetGBattSocAPI() <= (gGBmuGenPara_102[eBmuGenPara102_CapHig1Soc] - gGBmuGenPara_102[eBmuGenPara102_ChgLimSubSoc]))
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SH1, eErr_A);
	}
	/*在回差区间未产生告警则清除告警标志*/
	else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CAP_SH1))
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SH1, eErr_A);
	}

	/*判断SOC过低2级-ID15*/
	if(eErr_B == DiagGetErrLastStateAPI(eERR_ID_CAP_SL1))
	{
		/*一级已经发生(未消除)确保二级也发生(不消除)*/
		DiagSetErrFilStateAPI(eERR_ID_CAP_SL2, eErr_B);
	}
	/*达到SOC过低2级*/
	else if(GetGBattSocAPI() <= gGBmuGenPara_102[eBmuGenPara102_CapLow2Soc])
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SL2, eErr_B);
	}
	/*在回差点内则清除告警标志*/
	else if((GetGBattSocAPI() >= (gGBmuGenPara_102[eBmuGenPara102_CapLow2Soc] + 50))
		&& (GetGBattSocAPI() >= (gGBmuGenPara_102[eBmuGenPara102_CapLow1Soc] + gGBmuGenPara_102[eBmuGenPara102_DhgLimSubSoc])))
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SL2, eErr_A);
	}
	/*在回差区间未产生告警则清除告警标志*/
	else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CAP_SL2))
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SL2, eErr_A);
	}

	/*判断SOC过低1级-ID31*/
	if((GetGBattSocAPI() <= gGBmuGenPara_102[eBmuGenPara102_CapLow1Soc])
		&& (0 != gGBmuGenPara_102[eBmuGenPara102_CapLow1Soc]))
	{
		/*一级产生确保二级也要产生*/
		DiagSetErrNowStateAPI(eERR_ID_CAP_SL2, eErr_B);
		DiagSetErrNowStateAPI(eERR_ID_CAP_SL1, eErr_B);
	}
	/*在回差点内则清除告警标志*/
	else if(GetGBattSocAPI() >= (gGBmuGenPara_102[eBmuGenPara102_CapLow1Soc] + gGBmuGenPara_102[eBmuGenPara102_DhgLimSubSoc]))
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SL1, eErr_A);
	}
	/*在回差区间未产生告警则清除告警标志*/
	else if(eErr_A == DiagGetErrLastStateAPI(eERR_ID_CAP_SL1))
	{
		DiagSetErrNowStateAPI(eERR_ID_CAP_SL1, eErr_A);
	}
}

/*=================================================================================================
** @Name      : void DiagPortExpFilMakerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 输入输出端口异常类故障诊断任务
** @The notes : 100ms周期
**===============================================================================================*/
void DiagPortExpFilMakerTask(void)
{
    u8 ctrlState = 0;
    u8 backState = 0;

    /*主接触器控制状态与回检状态*/
    ctrlState = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Mai);
    backState = TacGetGroupSwhBackStateAPI(eGroupSwh_Mai);

    /*控制状态与回检状态不一致*/
    if(ctrlState != backState)
    {
    	DiagSetErrNowStateAPI(eERR_ID_K0RE_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_K0RE_EXP, eErr_A);
    }

	if(gGHardPara_104[eGHardPara104_ProjectCode] == 6)       //A2415降本
	{
		// 项目6：只包含正极接触器，不处理其他接触器
	}
	else if(gGHardPara_104[eGHardPara104_ProjectCode] == 7)    //A2415降本项目2
	{
		// 项目7：包含正极接触器和负极接触器，不处理预充接触器
		
		/*负极接触器控制状态与回检状态*/
		ctrlState = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg);
		backState = TacGetGroupSwhBackStateAPI(eGroupSwh_Neg);

		/*控制状态与回检状态不一致*/
		if(ctrlState != backState)
		{
			DiagSetErrNowStateAPI(eERR_ID_K2RE_EXP, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_K2RE_EXP, eErr_A);
		}
	}
	else
	{
		// 其余项目：包含正极接触器、负极接触器和预充接触器
		
		/*预充接触器控制状态与回检状态*/
		ctrlState = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Pre);
		backState = TacGetGroupSwhBackStateAPI(eGroupSwh_Pre);

		/*控制状态与回检状态不一致*/
		if(ctrlState != backState)
		{
			DiagSetErrNowStateAPI(eERR_ID_K1RE_EXP, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_K1RE_EXP, eErr_A);
		}
		
		/*负极接触器控制状态与回检状态*/
		ctrlState = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Neg);
		backState = TacGetGroupSwhBackStateAPI(eGroupSwh_Neg);

		/*控制状态与回检状态不一致*/
		if(ctrlState != backState)
		{
			DiagSetErrNowStateAPI(eERR_ID_K2RE_EXP, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_K2RE_EXP, eErr_A);
		}
	}

	#if(0)
    /*电源接触器控制状态与回检状态*/
    ctrlState = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Pwr);
    backState = TacGetGroupSwhBackStateAPI(eGroupSwh_Pwr);

    /*控制状态与回检状态不一致*/
    if(ctrlState != backState)
    {
    	DiagSetErrNowStateAPI(eERR_ID_K3RE_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_K3RE_EXP, eErr_A);
    }
	#endif

    /*其它接触器控制状态与回检状态*/
    ctrlState = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Run);
    ctrlState |= TacGetGroupSwhCtrlStateAPI(eGroupSwh_Alm);
    ctrlState |= TacGetGroupSwhCtrlStateAPI(eGroupSwh_Smg);
    ctrlState |= TacGetGroupSwhCtrlStateAPI(eGroupSwh_Hot);
    backState = TacGetGroupSwhBackStateAPI(eGroupSwh_Run);
    backState |= TacGetGroupSwhBackStateAPI(eGroupSwh_Alm);
    backState |= TacGetGroupSwhBackStateAPI(eGroupSwh_Smg);
    backState |= TacGetGroupSwhBackStateAPI(eGroupSwh_Hot);

    /*控制状态与回检状态不一致*/
    if(ctrlState != backState)
    {
    	DiagSetErrNowStateAPI(eERR_ID_K7RE_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_K7RE_EXP, eErr_A);
    }

    /*风扇接触器控制状态与回检状态*/
    ctrlState = TacGetGroupSwhCtrlStateAPI(eGroupSwh_Fan);
    backState = TacGetGroupSwhBackStateAPI(eGroupSwh_Fan);

    /*控制状态与回检状态不一致*/
    if(ctrlState != backState)
    {
    	DiagSetErrNowStateAPI(eERR_ID_K8RE_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_K8RE_EXP, eErr_A);
    }

    /*主正接触器负极控制状态与回检状态*/
    ctrlState = DEVGPIOGetOutPortState(ePOut_LDRVK0);
    backState = DEVGPIOGetInPortState(ePIn_LK1_Sta);

    /*控制状态与回检状态不一致*/
    if(ctrlState != backState)
    {
    	DiagSetErrNowStateAPI(eERR_ID_L0RE_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_L0RE_EXP, eErr_A);
    }
	
	if(gGHardPara_104[eGHardPara104_ProjectCode] == 6)       //A2415降本
	{
		;
	}
   	else                                                     //其余项目
	{
		/*负极接触器负极控制状态与回检状态*/
		ctrlState = DEVGPIOGetOutPortState(ePOut_LDRVK2);
		backState = DEVGPIOGetInPortState(ePIn_LK3_Sta);

		/*控制状态与回检状态不一致*/
		if(ctrlState != backState)
		{
			DiagSetErrNowStateAPI(eERR_ID_L2RE_EXP, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_L2RE_EXP, eErr_A);
		}
	}
}
/*=================================================================================================
** @Name      : void DiagInfoExpFilMakerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 数据信息异常类故障诊断任务
** @The notes : 100ms周期
**===============================================================================================*/
void DiagInfoExpFilMakerTask(void)
{
    /*SOC/SOE显示与真实值误差8%*/
    if((ABS(GetGBattSocAPI(), GetGRealSocMilliAPI()) >= 80)
        || (ABS(GetGBattSoeAPI(), GetGRealSoeMilliAPI()) >= 80))
    {
    	DiagSetErrNowStateAPI(eERR_ID_SOC_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SOC_EXP, eErr_A);
    }

    /*容量检测异常(检测值与原值误差20%或长时间未检测到容量)*/
    if(GetCapEnerDropExpAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_CAP_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_CAP_EXP, eErr_A);
    }

    /*均衡温度采样异常(读取到温度为断线值)*/
    if(DiagGetBalTOffCellBitAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_BAL_TEXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_BAL_TEXP, eErr_A);
    }

    /*均衡控制异常(回检状态不一致或均衡温度异常)*/
    if(DiagGetBalErrCellBitAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_BAL_CEXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_BAL_CEXP, eErr_A);
    }

    /*主控单体采样异常(采样通信无回复)*/
	if(DiagGetCellSampErrFlagAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SAMP_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SAMP_EXP, eErr_A);
    }

    /*主控单体电压采样断线(包括未插线束)*/
   	if(DiagGetCellVOffCellBitAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SAMP_VOFF, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SAMP_VOFF, eErr_A);
    }

    /*主控单体温度采样断线(包括未插线束)*/
    if(DiagGetCellTOffCellBitAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SAMP_TOFF, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SAMP_TOFF, eErr_A);
    }

    /*主控单体信息缺失(未全部接收单体电压和温度)*/
    if(2 != DiagGetRcvAllCellFlagAPI())
    {
    	DiagSetErrNowStateAPI(eERR_ID_CELL_MISS, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_CELL_MISS, eErr_A);
    }

    /*主控外部RTC异常*/
    if(FALSE == DEVEXTRTCGetReadyFlagAPI())
    {
    	DiagSetErrNowStateAPI(eERR_ID_MRTC_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_MRTC_EXP, eErr_A);
    }

    /*主控外部EEPROM异常*/
    if(FALSE == DEVEXTEEPGetReadyFlag())
    {
    	DiagSetErrNowStateAPI(eERR_ID_MEEP_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_MEEP_EXP, eErr_A);
    }

    /*主控外部FLASH异常*/
    if(FALSE == DEVEXTFLASHGetReadyFlag())
    {
    	DiagSetErrNowStateAPI(eERR_ID_MFLA_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_MFLA_EXP, eErr_A);
    }

    /*主控电流采样异常(空闲时任一路故障则故障而充放电时其中一路正常则不报故障)*/
    if(((eCURR_IDLE == GetGChgDhgStateAPI()) && (SampGetCurrSampExpFlagAPI(0xff) > 0))
        || (SampGetCurrSampExpFlagAPI(eCCHAN_Num) > 0))
    {
    	DiagSetErrNowStateAPI(eERR_ID_CSAMP_ERR, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_CSAMP_ERR, eErr_A);
    }

    /*主控高压采样异常*/
    if((SampGetHigVSampExpFlagAPI() > 0) || (DiagGetHigVoltExpFlagAPI() > 0))
    {
    	/*历史故障记录告警信息产生*/
    	if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_HSAMP_EXP))
    	{
    		DiagRcdHisErrInfoRecordAPI(eERR_ID_HSAMP_EXP, eERR_RCDSORT_WARSET);
    	}

    	DiagSetErrNowStateAPI(eERR_ID_HSAMP_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_HSAMP_EXP, eErr_A);
    }

    /*主控低压采样异常*/
    if(SampGetLowVSampExpFlagAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_LSAMP_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_LSAMP_EXP, eErr_A);
    }

    /*主控外围温度采样异常*/
    if((GetGEnvirTempAPI() <= (-41)) || (GetGShuntSampTempAPI() <= (-41))
    	|| (GetGEnvirTempAPI() >= (126)) || (GetGShuntSampTempAPI() >= (182)))
    {
    	DiagSetErrNowStateAPI(eERR_ID_TSAMP_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_TSAMP_EXP, eErr_A);
    }

}

/*=================================================================================================
** @Name      : void DiagStateExpFilMakerTask(void)
** @Input     : void
** @Output    : void
** @Function  : 状态信息异常类故障诊断任务
** @The notes : 100ms周期
**===============================================================================================*/
void DiagStateExpFilMakerTask(void)
{
    /*回采从控级联PWM线异常(回采电压低于1V或PWM捕获频率低于1K)*/
    if((GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmAdcV) < 100)
    	|| (GetGDcSideSampRealInfoAPI(eReferSampInfo200_PwmCycle) < 1000))
    {
    	DiagSetErrNowStateAPI(eERR_ID_INT_LOFF, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_INT_LOFF, eErr_A);
    }

    /*软件急停*/
    if((TacGetGEmergStopFlagAPI() & 0x03) > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SW_EMERG, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SW_EMERG, eErr_A);
    }

    /*电池组自行保护*/
    if(TacGetGHappenProFlagAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SELF_PRO, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SELF_PRO, eErr_A);
    }

    /*SOS诊断异常(已显示SOS异常原因)*/
    if(DiagGetGroupSosLevel() > 4)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SOS_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SOS_EXP, eErr_A);
    }
#if(1 == SOS_5_SystemLock_EN)
    /*SOS5级诊断异常触发系统锁(显示SOS异常原因)*/
    if(DiagGetGroupSosLevel() > 4)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SYS_LOCK, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SYS_LOCK, eErr_A);
    }
#endif
    //使能PDU外IMEU3通信功能且通信正常
    if(((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02) != 0) && (DiagGetPduOutImeu3OffCommuStateAPI() == 0))
    {
    	//泄爆阀回检状态判断
    	 if((TacGetAuxDevErrFlagAPI() & 0x03) != 0)
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_EXP_EXP, eErr_B);
    	 }
    	 else
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_EXP_EXP, eErr_A);
    	 }
    	 //排气扇回检状态判断
    	 if((TacGetAuxDevErrFlagAPI() & 0x04) != 0)
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_FAN3_WAR, eErr_B);
    	 }
    	 else
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_FAN3_WAR, eErr_A);
    	 }
    	 //电池仓温度1高温判断
    	 if(GetImeu3TempInfoAPI(eIMEU3217_485T1) > (s8)gGHardPara_104[eGHardPara104_CompTH])
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_COMP_TH1, eErr_B);
    	 }
    	 else
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_COMP_TH1, eErr_A);
    	 }
    	 //电池仓温度2高温判断
    	 if(GetImeu3TempInfoAPI(eIMEU3217_CabT1) > (s8)gGHardPara_104[eGHardPara104_CompTH])
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_COMP_TH2, eErr_B);
    	 }
    	 else
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_COMP_TH2, eErr_A);
    	 }
    	 //电池仓温度3高温判断
    	 if(GetImeu3TempInfoAPI(eIMEU3217_CabT2) > (s8)gGHardPara_104[eGHardPara104_CompTH])
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_COMP_TH3, eErr_B);
    	 }
    	 else
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_COMP_TH3, eErr_A);
    	 }
    	 //可燃气体通信中断
    	 if((GetPduOutImeu3InfoAPI(ePduOut219_Alarm1) & 0x05) != 0)
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_COM_CON, eErr_B);
    	 }
    	 else
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_COM_CON, eErr_A);
    	 }
    	 //温湿度传感器通信中断
    	 if((GetPduOutImeu3InfoAPI(ePduOut219_Alarm1) & 0x02) != 0)
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_TH_CON, eErr_B);
    	 }
    	 else
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_TH_CON, eErr_A);
    	 }
    	 //除湿机通信中断
    	 if((GetPduOutImeu3InfoAPI(ePduOut219_Alarm1) & 0x08) != 0)
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_DE_CON, eErr_B);
    	 }
    	 else
    	 {
    		 DiagSetErrNowStateAPI(eERR_ID_DE_CON, eErr_A);
    	 }

		//倾倒检测(A2445项目配置且使能状态下允许告警)   A2415  A2445 A2516
 		if((0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI1_16),eEU219DI2_Pour)) && ((gGHardPara_104[eGHardPara104_ProjectCode] == 0)
 			|| (gGHardPara_104[eGHardPara104_ProjectCode] == 1) || (gGHardPara_104[eGHardPara104_ProjectCode] == 5)) && (gGHardPara_104[eGHardPara104_DumpEn] == 1))
		{
			 DiagSetErrNowStateAPI(eERR_ID_DUMP_EXP, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_DUMP_EXP, eErr_A);
		}

		//检测(A2445项目配置)
		if((1 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI1_16),15)) && ((gGHardPara_104[eGHardPara104_ProjectCode] == 0) 
			|| (gGHardPara_104[eGHardPara104_ProjectCode] == 1)))
		{
			 DiagSetErrNowStateAPI(eERR_ID_SPD_EXP, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_SPD_EXP, eErr_A);
		}
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_EXP_EXP, eErr_A);
    	DiagSetErrNowStateAPI(eERR_ID_FAN3_WAR, eErr_A);
    	DiagSetErrNowStateAPI(eERR_ID_COMP_TH1, eErr_A);
    	DiagSetErrNowStateAPI(eERR_ID_COMP_TH2, eErr_A);
    	DiagSetErrNowStateAPI(eERR_ID_COMP_TH3, eErr_A);
    	DiagSetErrNowStateAPI(eERR_ID_DE_CON, eErr_A);
    	DiagSetErrNowStateAPI(eERR_ID_TH_CON, eErr_A);
    	DiagSetErrNowStateAPI(eERR_ID_COM_CON, eErr_A);
    	DiagSetErrNowStateAPI(eERR_ID_DUMP_EXP, eErr_A);
    	DiagSetErrNowStateAPI(eERR_ID_SPD_EXP, eErr_A);
    }

    //使能PDU内IMEU3通信功能且通信正常
    if(((gGHardPara_104[eGHardPara104_Imeu3En] & 0x01) != 0) && (DiagGetPduInImeu3OffCommuStateAPI() == 0))
    {
		 //PCS出风口高温二级判断
		 if(GetImeu3TempInfoAPI(eIMEU3217_PcsAiroutT) > (s8)gGHardPara_104[eGHardPara104_PcsFanTH2])
		 {
			 DiagSetErrNowStateAPI(eERR_ID_PCS_TH2, eErr_B);
		 }
		 else
		 {
			 DiagSetErrNowStateAPI(eERR_ID_PCS_TH2, eErr_A);
		 }
		 //PCS出风口高温一级判断
		 if(GetImeu3TempInfoAPI(eIMEU3217_PcsAiroutT) > (s8)gGHardPara_104[eGHardPara104_PcsFanTH1])
		 {
			 DiagSetErrNowStateAPI(eERR_ID_PCS_TH1, eErr_B);
			 DiagSetErrNowStateAPI(eERR_ID_PCS_TH2, eErr_B);//一级告警也触发2级告警
		 }
		 else
		 {
			 DiagSetErrNowStateAPI(eERR_ID_PCS_TH1, eErr_A);
		 }
		 //PDU环境高温二级判断
		 if(GetImeu3TempInfoAPI(eIMEU3217_PduEnvT) > (s8)gGHardPara_104[eGHardPara104_PduEnvTH2])
		 {
			 DiagSetErrNowStateAPI(eERR_ID_PDU_TH2, eErr_B);
		 }
		 else
		 {
			 DiagSetErrNowStateAPI(eERR_ID_PDU_TH2, eErr_A);
		 }
		 //PDU环境高温一级判断
		 if(GetImeu3TempInfoAPI(eIMEU3217_PduEnvT) > (s8)gGHardPara_104[eGHardPara104_PduEnvTH1])
		 {
			 DiagSetErrNowStateAPI(eERR_ID_PDU_TH1, eErr_B);
			 DiagSetErrNowStateAPI(eERR_ID_PDU_TH2, eErr_B);//一级告警2级也告警
		 }
		 else
		 {
			 DiagSetErrNowStateAPI(eERR_ID_PDU_TH1, eErr_A);
		 }

		 //PDU线缆高温二级判断
		 if(GetImeu3TempInfoAPI(eIMEU3217_PcsCableT) > (s8)gGHardPara_104[eGHardPara104_PduCabTH2])
		 {
			 DiagSetErrNowStateAPI(eERR_ID_CAB_TH2, eErr_B);
		 }
		 else
		 {
			 DiagSetErrNowStateAPI(eERR_ID_CAB_TH2, eErr_A);
		 }
		 //PDU线缆高温一级判断
		 if(GetImeu3TempInfoAPI(eIMEU3217_PcsCableT) > (s8)gGHardPara_104[eGHardPara104_PduCabTH1])
		 {
			 DiagSetErrNowStateAPI(eERR_ID_CAB_TH1, eErr_B);
			 DiagSetErrNowStateAPI(eERR_ID_CAB_TH2, eErr_B);//一级告警2级也告警
		 }
		 else
		 {
			 DiagSetErrNowStateAPI(eERR_ID_CAB_TH1, eErr_A);
		 }
#if (0)
		 //PCS散热风扇状态回检
		if(0 == BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI17_22_DO1_8),eEU218DI17_PcsFan))
		{
			 DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_A);
		}
#endif
#if(0)
		if(0 == BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI17_22_DO1_8),eEU218DI17_PcsFan)) 
		{ 
			// by lxr01202 风扇硬件更换，增加转速控制判断：当上层下发的PcsFanDuty为0且风扇回检电平为低电平时不告警
			if(PcsFanDuty != 0) 
			{ 
				DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_B); 
			} 
			else 
			{ 
				DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_A); 
			} 
		} 
		else 
		{ 
			DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_A); 
		}
#endif
//by lxr01202   新硬件风扇Pcs风扇回检告警策略
		if (sPcsFanDelayFlag && sPcsFanDelayCounter > 0)
		{
			sPcsFanDelayCounter--;
		}
		if(0 == BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI17_22_DO1_8), eEU218DI17_PcsFan)) 
		{ 
			if (sPcsFanStartDetected)    //风扇从0开始启动时候才会进来
			{
				// 首次检测到风扇启动后的低电平信号，启动2秒延迟检测
				sPcsFanDelayCounter = 100;    
				sPcsFanDelayFlag = TRUE;     // 设置延迟标志
				sPcsFanStartDetected = FALSE;   // 清除启动检测标志
			}
			
			if (sPcsFanDelayFlag && sPcsFanDelayCounter > 0)
			{
				// 延迟期间：强制设为正常状态，不告警
				DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_A);
			}
			else
			{
				if(PcsFanDuty != 0) 
				{ 
					// PWM不为0但风扇低电平
					DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_B); 
				} 
				else 
				{ 
					// PWM为0且风扇低电平
					DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_A); 
				}
				sPcsFanDelayFlag = FALSE;
			}
		} 
		else 
		{ 
			sPcsFanDelayFlag = FALSE;
			sPcsFanDelayCounter = 0;
			sPcsFanStartDetected = FALSE;
			
			// 设为正常状态
			DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_A); 
		} 

#if (0)
		 //PDU扰流风机状态回检
		if(0 == BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI17_22_DO1_8),eEU218DI17_PduFan))
		{
			 DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_A);
		}
#endif
#if(0)

		if(0 == BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI17_22_DO1_8),eEU218DI17_PduFan)) 
		{ 
			// by lxr01202 风扇硬件更换，增加转速控制判断：当上层下发的PduFanDuty为0且风扇回检电平为低电平时不告警
			if(PduFanDuty != 0) 
			{ 
				DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_B); 
			} 
			else 
			{ 
				DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_A); 
			} 
		} 
		else 
		{ 
			DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_A); 
		}
#endif
//by lxr01202   新硬件风扇PDU风扇回检告警策略
		if (sPduFanDelayFlag && sPduFanDelayCounter > 0)
		{
			sPduFanDelayCounter--;
		}
		if(0 == BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI17_22_DO1_8), eEU218DI17_PduFan)) 
		{ 
			if (sPduFanStartDetected)    //风扇从0开始启动时候才会进来
			{
				// 首次检测到风扇启动后的低电平信号，启动2秒延迟检测
				sPduFanDelayCounter = 100;    
				sPduFanDelayFlag = TRUE;     // 设置延迟标志
				sPduFanStartDetected = FALSE;   // 清除启动检测标志
			}
			
			if (sPduFanDelayFlag && sPduFanDelayCounter > 0)
			{
				// 延迟期间：强制设为正常状态，不告警
				DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_A);
			}
			else
			{
				if(PduFanDuty != 0) 
				{ 
					// PWM不为0但风扇低电平
					DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_B); 
				} 
				else 
				{ 
					// PWM为0且风扇低电平
					DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_A); 
				}
				sPduFanDelayFlag = FALSE;
			}
		} 
		else 
		{ 
			sPduFanDelayFlag = FALSE;
			sPduFanDelayCounter = 0;
			sPduFanStartDetected = FALSE;
			
			// 设为正常状态
			DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_A); 
		} 

	}
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_PCS_TH2, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PCS_TH1, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PDU_TH2, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PDU_TH1, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_CAB_TH2, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_CAB_TH1, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_FAN2_WAR, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_FAN1_WAR, eErr_A);
    }

    //消防一级告警
    if(GetFireLevelInfoAPI(eFireInfo254_Level1) == 1)
    {
    	DiagSetErrNowStateAPI(eERR_ID_FIRE1_WAR, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_FIRE1_WAR, eErr_A);
    }
    //消防二级告警
    if(GetFireLevelInfoAPI(eFireInfo254_Level2) == 1)
    {
    	DiagSetErrNowStateAPI(eERR_ID_FIRE2_WAR, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_FIRE2_WAR, eErr_A);
    }
    //消防三级告警
    if(GetFireLevelInfoAPI(eFireInfo254_Level3) == 1)
    {
    	DiagSetErrNowStateAPI(eERR_ID_FIRE3_WAR, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_FIRE3_WAR, eErr_A);
    }

    /*电池柜配电板外设(温湿度采集器/气体检测/空调)通信中断告警(提示)*/
    if(TacGetAuxComErrFlagAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_THC_OFF, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_THC_OFF, eErr_A);
    }
    /*配置了空调才进行检测*/
    if(gGHardPara_104[eGHardPara104_AirType] != 0)
    {
        if(DiagGetPerAirOffCommuStateAPI() != 0)
        {
        	DiagSetErrNowStateAPI(eERR_ID_AIR_CON, eErr_B);
        }
        else
        {
        	DiagSetErrNowStateAPI(eERR_ID_AIR_CON, eErr_A);
        }
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_AIR_CON, eErr_A);
    }
    /*配置了漏电流传感器才进行检测*/
    if(gGHardPara_104[eGHardPara104_LeakageType] != 0)
    {
    	if(DiagGetLeakageOffCommuStateAPI() != 0)
    	{
    		DiagSetErrNowStateAPI(eERR_ID_LEAK_CON, eErr_B);
    	}
    	else
    	{
    		DiagSetErrNowStateAPI(eERR_ID_LEAK_CON, eErr_A);
    	}
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_LEAK_CON, eErr_A);
    }
    /*配置了漏电流传感器且通信正常*/
    if((gGHardPara_104[eGHardPara104_LeakageType] != 0) && (DiagGetLeakageOffCommuStateAPI() == 0))
    {
    	if((GetLeakageInfoAPI(eLeakageInfo225_Curr) & 0x7FFF) > gGHardPara_104[eGHardPara104_LeakageC] * 100)
    	{
    		DiagSetErrNowStateAPI(eERR_ID_LEAK_ERR, eErr_B);
    	}
    	else
    	{
    		DiagSetErrNowStateAPI(eERR_ID_LEAK_ERR, eErr_A);
    	}
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_LEAK_ERR, eErr_A);
    }
    //#if(BMU_IMEU3_COM_EN == 1)
    //配置了PDU内IMEU3才进行检测
    if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x01) != 0)
    {
        if(DiagGetPduInImeu3OffCommuStateAPI() != 0)
        {
        	DiagSetErrNowStateAPI(eERR_ID_EU31_CON, eErr_B);
        }
        else
        {
        	DiagSetErrNowStateAPI(eERR_ID_EU31_CON, eErr_A);
        }
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_EU31_CON, eErr_A);
    }
    //配置了PDU外IMEU3才进行检测
    if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02) != 0)
    {
        if(DiagGetPduOutImeu3OffCommuStateAPI() != 0)
        {
        	DiagSetErrNowStateAPI(eERR_ID_EU32_CON, eErr_B);
        }
        else
        {
        	DiagSetErrNowStateAPI(eERR_ID_EU32_CON, eErr_A);
        }
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_EU32_CON, eErr_A);
    }
    //#endif

	/*电池柜配电板外设（SPD）告警（充放电保护）*/
    if(TacGetSpdErrFlagAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SPD_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SPD_EXP, eErr_A);
    }

    /*主控主接触器状态异常*/
    if((1 == BitGet(GetSwhAdhesErrFlagAPI(), 0))
        || (1 == BitGet(DiagGetSwhStateVExpFlagAPI(), 0))
    	|| (1 == BitGet(DiagGetSwhStateVExpFlagAPI(), 4))
		|| (1 == BitGet(TacGetGSwhCtrlFailFlagAPI(), 0))
		|| (1 == BitGet(TacGetGSwhCtrlFailFlagAPI(), 4)))
    {
    	/*历史故障记录告警信息产生*/
    	if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_K0MAI_EXP))
    	{
    		DiagRcdHisErrInfoRecordAPI(eERR_ID_K0MAI_EXP, eERR_RCDSORT_WARSET);
    	}
		//by lxr01202
		//如果EU3检测到主正反馈断开 && BMU主正又是闭合的，则实际是没闭合的，因此会产生告警
		if((1 == BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI1_16),eEU218DI8_BmuMainCon)) && (ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Mai))
			&& ((gGHardPara_104[eGHardPara104_ProjectCode] == 7) || (gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)))
		{
			DiagSetErrNowStateAPI(eERR_ID_K0MAI_EXP, eErr_B);
		}
		else
		{
    		DiagSetErrNowStateAPI(eERR_ID_K0MAI_EXP, eErr_A);
		}
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_K0MAI_EXP, eErr_A);
    }

	
    if(gGHardPara_104[eGHardPara104_ProjectCode] == 6)    //A2415降本
	{
		;
	}
	else if(gGHardPara_104[eGHardPara104_ProjectCode] == 7)     //A2415降本项目2
	{
		/*主控负极接触器状态异常*/
		if((1 == BitGet(GetSwhAdhesErrFlagAPI(), 2))
			|| (1 == BitGet(DiagGetSwhStateVExpFlagAPI(), 2))
			|| (1 == BitGet(DiagGetSwhStateVExpFlagAPI(), 6))
			|| (1 == BitGet(TacGetGSwhCtrlFailFlagAPI(), 2))
			|| (1 == BitGet(TacGetGSwhCtrlFailFlagAPI(), 6)))
		{
			/*历史故障记录告警信息产生*/
			if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_K2NEG_EXP))
			{
				DiagRcdHisErrInfoRecordAPI(eERR_ID_K2NEG_EXP, eERR_RCDSORT_WARSET);
			}
			//by lxr01202
		    //如果EU3检测到主负反馈断开 && BMU主负又是闭合的，则实际是没闭合的，因此会产生告警
		    if((1 == BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI1_16),eEU218DI9_BmuNegCon)) && (ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg)))
			{
				DiagSetErrNowStateAPI(eERR_ID_K2NEG_EXP, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_K2NEG_EXP, eErr_A);
			}
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_K2NEG_EXP, eErr_A);
		}
	}
	else
	{
		/*主控预充接触器状态异常*/
		if((1 == BitGet(GetSwhAdhesErrFlagAPI(), 1))
			|| (1 == BitGet(DiagGetSwhStateVExpFlagAPI(), 1))
			|| (1 == BitGet(DiagGetSwhStateVExpFlagAPI(), 5))
			|| (1 == BitGet(TacGetGSwhCtrlFailFlagAPI(), 1))
			|| (1 == BitGet(TacGetGSwhCtrlFailFlagAPI(), 5)))
		{
			/*历史故障记录告警信息产生*/
			if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_K1PRE_EXP))
			{
				DiagRcdHisErrInfoRecordAPI(eERR_ID_K1PRE_EXP, eERR_RCDSORT_WARSET);
			}

			DiagSetErrNowStateAPI(eERR_ID_K1PRE_EXP, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_K1PRE_EXP, eErr_A);
		}

		/*主控负极接触器状态异常*/
		if((1 == BitGet(GetSwhAdhesErrFlagAPI(), 2))
			|| (1 == BitGet(DiagGetSwhStateVExpFlagAPI(), 2))
			|| (1 == BitGet(DiagGetSwhStateVExpFlagAPI(), 6))
			|| (1 == BitGet(TacGetGSwhCtrlFailFlagAPI(), 2))
			|| (1 == BitGet(TacGetGSwhCtrlFailFlagAPI(), 6)))
		{
			/*历史故障记录告警信息产生*/
			if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_K2NEG_EXP))
			{
				DiagRcdHisErrInfoRecordAPI(eERR_ID_K2NEG_EXP, eERR_RCDSORT_WARSET);
			}
			//by lxr01202
		    //如果EU3检测到主负反馈断开 && BMU主负又是闭合的，则实际是没闭合的，因此会产生告警
		    if((1 == BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI1_16),eEU218DI9_BmuNegCon)) && (ePSTATE_ON == TacGetGroupSwhBackStateAPI(eGroupSwh_Neg))
		       && ((gGHardPara_104[eGHardPara104_ProjectCode] == 7) || (gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)))
			{
				DiagSetErrNowStateAPI(eERR_ID_K2NEG_EXP, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_K2NEG_EXP, eErr_A);
			}
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_K2NEG_EXP, eErr_A);
		}
	}
	//#if((BMU_IMEU3_COM_EN > 0) && (BMU_EU_FLOOD_HZ > 0))
    /*主控水浸告警状态异常(低于300Hz,接收到外设报文或非初始化(水浸值非0)时检测)*/
    //if((GetPduOutImeu3InfoAPI(ePduOut219_FloodHz) < BMU_EU_FLOOD_HZ)
    //	&& ((2 == DiagGetRcvAllPeripFlagAPI())
    //	|| ((eWORK_INIT != GetGWorkStateAPI())
    //	&& (GetPduOutImeu3InfoAPI(ePduOut219_FloodHz) > 0))))
    /*IMEU3无通信中断切水浸频率小于300切不为0且不是初始化状态*/
    //使能PDU外IMEU3通信功能
    if(((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02) != 0) && (BMU_EU_FLOOD_HZ > 0))
    {
        if((GetPduOutImeu3InfoAPI(ePduOut219_FloodHz) < BMU_EU_FLOOD_HZ)
            && (GetPduOutImeu3InfoAPI(ePduOut219_FloodHz) > 0)
    		&& (0 == DiagGetPduOutImeu3OffCommuStateAPI())
    		&& (eWORK_INIT != GetGWorkStateAPI())
    		)
        {
        	DiagSetErrNowStateAPI(eERR_ID_FLOOD_ERR, eErr_B);
        }
        else if(GetPduOutImeu3InfoAPI(ePduOut219_FloodHz) >= (BMU_EU_FLOOD_HZ + 100))
        {
        	DiagSetErrNowStateAPI(eERR_ID_FLOOD_ERR, eErr_A);
        }
    }
	//#endif

    /*主控可燃气体浓度告警状态异常(高于20%)(上电前1H数据不可信需要额外处理)*/
    //if((GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1) >= BMU_EU_GAS1_CONC)
    //	|| (GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2) >= BMU_EU_GAS2_CONC)
    //	|| (1 == GetGGasConIoExpStateAPI()))
    if(DiagGetPerGasErrFlagAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_CGAS_EXP, eErr_B);
    }
    //else if((GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc1) <= (BMU_EU_GAS1_CONC / 2))
    //	&& (GetGPerImeu2SignInfoAPI(eIMEU2Info204_GasConc2) <= (BMU_EU_GAS2_CONC / 2)))
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_CGAS_EXP, eErr_A);
    }

    /*主控消防告警状态异常*/
    if(1 == GetGFireIoExpStateAPI())
    {
    	DiagSetErrNowStateAPI(eERR_ID_FIRE_EXP, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_FIRE_EXP, eErr_A);
    }

    /*主控熔断器状态异常*/
    if((1 == BitGet(DiagGetPeripDevExpFlagAPI(), 0))
		|| (1 == GetGFusIoExpStateAPI()))
    {
    	DiagSetErrNowStateAPI(eERR_ID_FUSE_OFF, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_FUSE_OFF, eErr_A);
    }

    /*主控烟感告警状态异常*/
    //if(1 == GetGSmogIoWarStateAPI())
    if(2 == TacGetGSmogAlarmFlagAPI())
    {
    	DiagSetErrNowStateAPI(eERR_ID_BATT_SMOG, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_BATT_SMOG, eErr_A);
    }

    /*主控断路器状态异常*/
    if((1 == BitGet(DiagGetPeripDevExpFlagAPI(), 1))
		|| (0 == GetGIsoIoReadyStateAPI()))
    {
    	DiagSetErrNowStateAPI(eERR_ID_ISO_OFF, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_ISO_OFF, eErr_A);
    }

    /*主控启动预充未满*/
    if(1 == BitGet(TacGetGStartFailFlagAPI(), eStartFailReason_PChgFail))
    {
    	/*历史故障记录告警信息产生*/
    	if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_STARTUP_VL))
    	{
    		DiagRcdHisErrInfoRecordAPI(eERR_ID_STARTUP_VL, eERR_RCDSORT_WARSET);
    	}

    	DiagSetErrNowStateAPI(eERR_ID_STARTUP_VL, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_STARTUP_VL, eErr_A);
    }

    /*主控启动预充过温*/
    if(1 == BitGet(TacGetGStartFailFlagAPI(), eStartFailReason_PChgTOvr))
    {
    	DiagSetErrNowStateAPI(eERR_ID_STARTUP_TH, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_STARTUP_TH, eErr_A);
    }

    /*主控启动故障失败*/
    if((TacGetGStartFailFlagAPI() & 0x0f) > 0)
    {
    	/*历史故障记录告警信息产生*/
    	if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_STARTUP_ERR))
    	{
    		DiagRcdHisErrInfoRecordAPI(eERR_ID_STARTUP_ERR, eERR_RCDSORT_WARSET);
    	}

    	DiagSetErrNowStateAPI(eERR_ID_STARTUP_ERR, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_STARTUP_ERR, eErr_A);
    }

    /*主控停机带载超时*/
    if(1 == BitGet(TacGetGStopFailFlagAPI(), 1))
    {
    	DiagSetErrNowStateAPI(eERR_ID_STOP_CH, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_STOP_CH, eErr_A);
    }

    /*主控停机故障失败*/
    if((TacGetGStopFailFlagAPI() & 0x0d) > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_STOP_ERR, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_STOP_ERR, eErr_A);
    }

    /*按钮急停*/
    if(((1 == GetGEmergIoButtonStateAPI())
        || (TacGetGEmergStopFlagAPI() & 0x04) > 0))
    {
    	DiagSetErrNowStateAPI(eERR_ID_IO_EMERG, eErr_B);
		/*A2415 A2445项目，急停按下后，控制交流自杀*/
		if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
		{
			SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR,1,1); //控制QF1断开
		}
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_IO_EMERG, eErr_A);
		/*A2415 A2445项目，急停按下后，控制交流自杀*/
		if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
		{
			SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR,1,0); //控制QF1闭合
		}
    }

    /*高压端内外压差过大*/
    if(DiagGetHigVSubDiffFlagAPI() > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_DC_HV_DIFF, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_DC_HV_DIFF, eErr_A);
    }

    /*接触器带载闭合*/
    if((DiagGetSwhChangCExpFlagAPI() & 0xf0) > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SWHON_CH, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SWHON_CH, eErr_A);
    }

    /*接触器带载断开*/
    if((DiagGetSwhChangCExpFlagAPI() & 0x0f) > 0)
    {
    	DiagSetErrNowStateAPI(eERR_ID_SWHOFF_CH, eErr_B);
    }
    else
    {
    	DiagSetErrNowStateAPI(eERR_ID_SWHOFF_CH, eErr_A);
    }
	 //使用自研BSU
    #if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))
	/*从控通信正常*/
	if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_INT_COFF))
	{
		//保供电模式   系统水浸且PACK水浸组合判断
		if (MU2KeepPowerMode == 1) 
		{
			/*PACK1水浸异常*/
			if(((DiagGetBsu02u1FloodErrorBitAPI() & 0x01) > 0) && (((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 2)&&(gGHardPara_104[eGHardPara104_AirType] == 2))||\
				((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 3)&&(gGHardPara_104[eGHardPara104_AirType] == 3))))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK1_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK1_WAT, eErr_A);
			}

			/*PACK2水浸异常*/
			if(((DiagGetBsu02u1FloodErrorBitAPI() & 0x02) > 0) && (((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 2)&&(gGHardPara_104[eGHardPara104_AirType] == 2))||\
				((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 3)&&(gGHardPara_104[eGHardPara104_AirType] == 3))))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK2_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK2_WAT, eErr_A);
			}

			/*PACK3水浸异常*/
			if(((DiagGetBsu02u1FloodErrorBitAPI() & 0x04) > 0) && (((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 2)&&(gGHardPara_104[eGHardPara104_AirType] == 2))||\
				((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 3)&&(gGHardPara_104[eGHardPara104_AirType] == 3))))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK3_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK3_WAT, eErr_A);
			}

			/*PACK4水浸异常*/
			if(((DiagGetBsu02u1FloodErrorBitAPI() & 0x08) > 0) && (((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 2)&&(gGHardPara_104[eGHardPara104_AirType] == 2))||\
				((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 3)&&(gGHardPara_104[eGHardPara104_AirType] == 3))))			
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK4_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK4_WAT, eErr_A);
			}

			/*PACK5水浸异常*/
			if(((DiagGetBsu02u1FloodErrorBitAPI() & 0x10) > 0) && (((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 2)&&(gGHardPara_104[eGHardPara104_AirType] == 2))||\
				((gTFAirStateInfo_213[eATFUserState213_RetpWatPre] < 3)&&(gGHardPara_104[eGHardPara104_AirType] == 3))))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK5_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK5_WAT, eErr_A);
			}
		}
		else     //正常模式
		{
			/*PACK1水浸异常*/
			if((DiagGetBsu02u1FloodErrorBitAPI() & 0x01) > 0)
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK1_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK1_WAT, eErr_A);
			}

			/*PACK2水浸异常*/
			if((DiagGetBsu02u1FloodErrorBitAPI() & 0x02) > 0)
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK2_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK2_WAT, eErr_A);
			}

			/*PACK3水浸异常*/
			if((DiagGetBsu02u1FloodErrorBitAPI() & 0x04) > 0)
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK3_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK3_WAT, eErr_A);
			}

			/*PACK4水浸异常*/
			if((DiagGetBsu02u1FloodErrorBitAPI() & 0x08) > 0)
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK4_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK4_WAT, eErr_A);
			}

			/*PACK5水浸异常*/
			if((DiagGetBsu02u1FloodErrorBitAPI() & 0x10) > 0)
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK5_WAT, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK5_WAT, eErr_A);
			}
		}

		/*PACK1正极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x01) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK1_TH1, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK1_TH1, eErr_A);
		}
		/*PACK1负极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x02) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK1_TH2, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK1_TH2, eErr_A);
		}

		/*PACK2正极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x04) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK2_TH1, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK2_TH1, eErr_A);
		}
		/*PACK2负极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x08) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK2_TH2, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK2_TH2, eErr_A);
		}

		/*PACK3正极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x10) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK3_TH1, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK3_TH1, eErr_A);
		}
		/*PACK3负极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x20) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK3_TH2, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK3_TH2, eErr_A);
		}

		/*PACK4正极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x40) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK4_TH1, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK4_TH1, eErr_A);
		}
		/*PACK4负极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x80) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK4_TH2, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK4_TH2, eErr_A);
		}

		/*PACK5正极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x100) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK5_TH1, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK5_TH1, eErr_A);
		}
		/*PACK5负极连接器高温告警*/
		if((DiagGetBsu02u1TemperErrorBitAPI() & 0x200) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK5_TH2, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK5_TH2, eErr_A);
		}

		/*PACK1气溶胶浓度异常*/
		if((DiagGetBsu02u1AeroErrorBitAPI() & 0x01) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK1_AERS, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK1_AERS, eErr_A);
		}

		/*PACK2气溶胶浓度异常*/
		if((DiagGetBsu02u1AeroErrorBitAPI() & 0x02) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK2_AERS, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK2_AERS, eErr_A);
		}

		/*PACK3气溶胶浓度异常*/
		if((DiagGetBsu02u1AeroErrorBitAPI() & 0x04) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK3_AERS, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK3_AERS, eErr_A);
		}

		/*PACK4气溶胶浓度异常*/
		if((DiagGetBsu02u1AeroErrorBitAPI() & 0x08) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK4_AERS, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK4_AERS, eErr_A);
		}

		/*PACK5气溶胶浓度异常*/
		if((DiagGetBsu02u1AeroErrorBitAPI() & 0x10) > 0)
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK5_AERS, eErr_B);
		}
		else
		{
			DiagSetErrNowStateAPI(eERR_ID_PACK5_AERS, eErr_A);
		}

		//保供电模式下，和总压组合判断MSD维修开关异常状态
		if (MU2KeepPowerMode == 1)
		{
			/*PACK1 MSD状态异常且总压和单体累加电压相差20V以上*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x01) > 0) && (ABS(GetGSampSumVoltAPI(), GetGCellSumVoltAPI()) > (gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK1_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK1_MSD, eErr_A);
			}
			/*PACK2 MSD状态异常*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x02) > 0) && (ABS(GetGSampSumVoltAPI(), GetGCellSumVoltAPI()) > (gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK2_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK2_MSD, eErr_A);
			}
			/*PACK3 MSD状态异常*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x04) > 0) && (ABS(GetGSampSumVoltAPI(), GetGCellSumVoltAPI()) > (gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK3_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK3_MSD, eErr_A);
			}
			/*PACK4 MSD状态异常*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x08) > 0) && (ABS(GetGSampSumVoltAPI(), GetGCellSumVoltAPI()) > (gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK4_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK4_MSD, eErr_A);
			}
			/*PACK5 MSD状态异常*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x10) > 0) && (ABS(GetGSampSumVoltAPI(), GetGCellSumVoltAPI()) > (gGBmuHigLevPara_103[eBmuHigLevPara103_SumDiffV])))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK5_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK5_MSD, eErr_A);
			}
		}
		else                // 正常模式
		{
			/*PACK1 MSD状态异常*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x01) > 0))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK1_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK1_MSD, eErr_A);
			}
			/*PACK2 MSD状态异常*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x02) > 0))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK2_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK2_MSD, eErr_A);
			}
			/*PACK3 MSD状态异常*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x04) > 0))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK3_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK3_MSD, eErr_A);
			}
			/*PACK4 MSD状态异常*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x08) > 0))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK4_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK4_MSD, eErr_A);
			}
			/*PACK5 MSD状态异常*/
			if(((DiagGetBsu02u1MsdErrorBitAPI() & 0x10) > 0))
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK5_MSD, eErr_B);
			}
			else
			{
				DiagSetErrNowStateAPI(eERR_ID_PACK5_MSD, eErr_A);
			}
		}
		/*PACK1 AFE枚举异常*/
//		if((DiagGetBsu02u1AfeEnumErrorBitAPI() & 0x01) > 0)
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK1_ENUM, eErr_B);
//		}else
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK1_ENUM, eErr_A);
//		}
//		/*PACK2 AFE枚举异常*/
//		if((DiagGetBsu02u1AfeEnumErrorBitAPI() & 0x02) > 0)
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK2_ENUM, eErr_B);
//		}else
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK2_ENUM, eErr_A);
//		}
//		/*PACK3 AFE枚举异常*/
//		if((DiagGetBsu02u1AfeEnumErrorBitAPI() & 0x04) > 0)
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK3_ENUM, eErr_B);
//		}else
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK3_ENUM, eErr_A);
//		}
//		/*PACK4 AFE枚举异常*/
//		if((DiagGetBsu02u1AfeEnumErrorBitAPI() & 0x08) > 0)
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK4_ENUM, eErr_B);
//		}else
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK4_ENUM, eErr_A);
//		}
//		/*PACK5 AFE枚举异常*/
//		if((DiagGetBsu02u1AfeEnumErrorBitAPI() & 0x10) > 0)
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK5_ENUM, eErr_B);
//		}else
//		{
//			DiagSetErrNowStateAPI(eERR_ID_PACK5_ENUM, eErr_A);
//		}
	}
	/*从控通信中断*/
	else
	{
		DiagSetErrNowStateAPI(eERR_ID_PACK1_AERS, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK2_AERS, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK3_AERS, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK4_AERS, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK5_AERS, eErr_A);

		DiagSetErrNowStateAPI(eERR_ID_PACK1_TH1, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK2_TH1, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK3_TH1, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK4_TH1, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK5_TH1, eErr_A);

		DiagSetErrNowStateAPI(eERR_ID_PACK1_TH2, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK2_TH2, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK3_TH2, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK4_TH2, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK5_TH2, eErr_A);

		DiagSetErrNowStateAPI(eERR_ID_PACK1_WAT, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK2_WAT, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK3_WAT, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK4_WAT, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK5_WAT, eErr_A);

		DiagSetErrNowStateAPI(eERR_ID_PACK1_MSD, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK2_MSD, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK3_MSD, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK4_MSD, eErr_A);
		DiagSetErrNowStateAPI(eERR_ID_PACK5_MSD, eErr_A);

		//DiagSetErrNowStateAPI(eERR_ID_PACK1_ENUM, eErr_A);
		//DiagSetErrNowStateAPI(eERR_ID_PACK2_ENUM, eErr_A);
		//DiagSetErrNowStateAPI(eERR_ID_PACK3_ENUM, eErr_A);
		//DiagSetErrNowStateAPI(eERR_ID_PACK4_ENUM, eErr_A);
		//DiagSetErrNowStateAPI(eERR_ID_PACK5_ENUM, eErr_A);
	}
	#endif
}
