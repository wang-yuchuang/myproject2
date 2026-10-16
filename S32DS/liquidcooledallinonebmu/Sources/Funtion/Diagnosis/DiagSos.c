/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SosDiagSos.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 系统SOS诊断模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "GroupPara.h"
#include "ParaInterface.h"
#include "EnerInterface.h"
#include "DiagUser.h"
#include "DiagFilter.h"
#include "DiagInterface.h"
#include "DiagSos.h"
#include "AuxDevTactic.h"     //keeppowermode调用
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern const t_FilPara gErrFilPara[];                   /*故障诊断滤波参数(故障滤波时间和自恢复次数)*/
extern t_FilState gErrFilTab[];                         /*故障诊断滤波处理(判断故障发生或消除状态)*/

extern u16 gGHisDodInfo_48[];							/*历史DOD记录信息查询数组*/
extern u16 gGFaultInfo_54[];			                /*故障信息数组*/
extern u32 gGStatistHisEner_74[];						/*累计电能统计信息数组*/
extern u16 gGStatistSosInfo_76[];						/*安全等级统计信息数组*/

extern u16 gGBmuGenPara_102[];  		                /*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];  						/*主控高级参数值*/
#if(1 == SOS_5_SystemLock_EN)
static u16 sSosSysLockErrBit = 0;						/*sos5级触发系统锁原因故障位（按bit表示触发系统锁原因）*/
#endif
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
#if(1 == SOS_DIAG_TATIS_EN)/*任务使能*/
/*=================================================================================================
** @Name      : static u8 DiagGroupErrNowSosLev(void)
** @Input     : void
** @Output    : 当前电池组故障告警对应安全等级SOS[0, 4]
** @Function  : 诊断当前电池组故障告警对应安全等级SOS
** @The notes :
**===============================================================================================*/
static u8 DiagGroupErrNowSosLev(void);

/*=================================================================================================
** @Name      : static u8 DiagGroupExpNowSosLev(void)
** @Input     : void
** @Output    : 当前电池组异常信息值对应安全等级SOS[0, 4]
** @Function  : 诊断当前电池组异常信息值安全等级SOS
** @The notes : 判断当前值是否正常
**===============================================================================================*/
static u8 DiagGroupExpNowSosLev(void);

/*=================================================================================================
** @Name      : static u8 DiagGroupStatisSosLev(void)
** @Input     : void
** @Output    : 当前电池组统计信息对应安全等级SOS[0, 4]
** @Function  : 诊断当前电池组统计信息对应安全等级SOS
** @The notes : 多次统计确认异常
**===============================================================================================*/
static u8 DiagGroupStatisSosLev(void);
#if(1 == SOS_5_SystemLock_EN)
/*=================================================================================================
** @Name      : static u8 DiagGroupSafetyLockSosLev(void)
** @Input     : void
** @Output    : 当前电池组系统锁统计信息对应安全等级SOS[0, 5]
** @Function  : 诊断当前电池组系统锁统计信息对应安全等级SOS
** @The notes : 多次统计确认异常
**===============================================================================================*/
static u8 DiagGroupSosSysLockSosLev(void);
#endif
#endif

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void DiagGroupSosInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组安全等级SOS诊断初始化
** @The notes : 读取是否到达锁定等级
**===========================================================================================*/
void DiagGroupSosInit(void)
{
	/*任务使能*/
	#if(1 == SOS_DIAG_TATIS_EN)
	#if(1 == SOS_5_SystemLock_EN)
    u16 info16[5] = {0};

    /*读取安全等级SOS锁定信息*/
    if(TRUE == ParaReadStoreSOSLockInfo(info16, 5))
    {
    	//只记录锁定5级异常,触发系统锁
    	if(5 == info16[1])
    	{
			gGStatistSosInfo_76[eDiagSos76_NowSos] = info16[1];
			gGStatistSosInfo_76[eDiagSos76_NowRsn] = info16[2];
    	}
        else
        {
        	gGStatistSosInfo_76[eDiagSos76_NowSos] = 0;
        	gGStatistSosInfo_76[eDiagSos76_NowRsn] = 0;
        }

    	//只记录锁定5级异常，触发系统锁
    	if(5 == info16[3])
    	{
			gGStatistSosInfo_76[eDiagSos76_PreSos] = info16[3];
			gGStatistSosInfo_76[eDiagSos76_PreRsn] = info16[4];
    	}
        else
        {
        	gGStatistSosInfo_76[eDiagSos76_PreSos] = 0;
        	gGStatistSosInfo_76[eDiagSos76_PreRsn] = 0;
        }
    }
    else
    {
    	gGStatistSosInfo_76[eDiagSos76_NowSos] = 0;
    	gGStatistSosInfo_76[eDiagSos76_NowRsn] = 0;
    	gGStatistSosInfo_76[eDiagSos76_PreSos] = 0;
    	gGStatistSosInfo_76[eDiagSos76_PreRsn] = 0;

    }
	#endif
	#endif
}

/*=============================================================================================
** @Name      : void DiagGroupSosTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组安全等级SOS诊断任务
** @The notes : 100ms
**===========================================================================================*/
void DiagGroupSosTask(void)
{
	/*任务使能*/
	#if(1 == SOS_DIAG_TATIS_EN)
    static u8 sHisSosLev = 0;
    u8 nowErrSos = 0;
    u8 nowStaSos = 0;
    /*系统初始化不诊断*/
    if(eWORK_INIT != GetGWorkStateAPI())
    {
		#if(1 == SOS_5_SystemLock_EN)
        /*当前低于5级则诊断当前SOS,否则保持5级锁定，触发系统锁，不诊断*/
        if(gGStatistSosInfo_76[eDiagSos76_NowSos] < 5)
        {
		#endif
            /*当前低于4级则诊断当前4级内SOS,否则保持4级锁定，不触发系统锁*/
            if(gGStatistSosInfo_76[eDiagSos76_NowSos] < 4)
            {
            	/*判定当前各故障告警对应安全等级*/
                nowErrSos = DiagGroupErrNowSosLev();

            	/*判定当前值对应安全等级*/
                nowStaSos = DiagGroupExpNowSosLev();

                /*当前SOS取级别更高的SOS*/
                if(nowErrSos < nowStaSos)
                {
                	nowErrSos = nowStaSos;
                }

            	/*判定当前值或统计值对应安全等级*/
                nowStaSos = DiagGroupStatisSosLev();

                /*当前SOS取级别更高的SOS*/
                if(nowErrSos < nowStaSos)
                {
                	nowErrSos = nowStaSos;
                }
                /*当前SOS级别变化*/
                if(gGStatistSosInfo_76[eDiagSos76_NowSos] != nowErrSos)
                {
            		/*SOS级别清零*/
                	if(0 == nowErrSos)
                	{
                		/*记录SOS级别异常消除*/
                		DiagUserSelfErrRecord(eUDef_ErrRcd_ReSosErr, eERR_RCDSORT_WARCLR, gGStatistSosInfo_76[eDiagSos76_NowSos],  nowErrSos);
                	}
                	else
                	{
                		/*记录SOS级别异常产生*/
                		DiagUserSelfErrRecord(eUDef_ErrRcd_ReSosErr, eERR_RCDSORT_WARSET, gGStatistSosInfo_76[eDiagSos76_NowSos],  nowErrSos);
                	}
                }

        		/*更新SOS级别显示*/
                gGStatistSosInfo_76[eDiagSos76_NowSos] = nowErrSos;

            }
		#if(1 == SOS_5_SystemLock_EN)
        	/*判定当前系统锁安全值对应安全等级,诊断5级锁定*/
            nowStaSos = DiagGroupSosSysLockSosLev();
            /*当前SOS取级别更高的SOS*/
            if(nowErrSos < nowStaSos)
            {
            	nowErrSos = nowStaSos;
            }
            /*当前SOS级别变化*/
            if(gGStatistSosInfo_76[eDiagSos76_NowSos] != nowErrSos)
            {
        		/*SOS级别清零*/
            	if(0 == nowErrSos)
            	{
            		/*记录SOS级别异常消除*/
            		DiagUserSelfErrRecord(eUDef_ErrRcd_ReSosErr, eERR_RCDSORT_WARCLR, gGStatistSosInfo_76[eDiagSos76_NowSos],  nowErrSos);
            	}
            	else
            	{
            		/*记录SOS级别异常产生*/
            		DiagUserSelfErrRecord(eUDef_ErrRcd_ReSosErr, eERR_RCDSORT_WARSET, gGStatistSosInfo_76[eDiagSos76_NowSos],  nowErrSos);
            	}
            }

    		/*更新SOS级别显示*/
            gGStatistSosInfo_76[eDiagSos76_NowSos] = nowErrSos;

        }
		#endif
        /*当前低于4级则诊断预测SOS,否则保持锁定不诊断*/
        if(gGStatistSosInfo_76[eDiagSos76_PreSos] < 4)
        {
            /*推断预测SOS*/
        	//gGStatistSosInfo_76[eDiagSos76_PreSos] = ;
        }
		#if(1 == SOS_5_SystemLock_EN)
    	/*SOS超过或者等于5级，触发系统锁，需要记录*/
    	if(((sHisSosLev >= 5)
    		&& (gGStatistSosInfo_76[eDiagSos76_NowSos] < 5)
			&&  (gGStatistSosInfo_76[eDiagSos76_PreSos] < 5))
    		|| ((sHisSosLev < 5)
    	    && ((gGStatistSosInfo_76[eDiagSos76_NowSos] >= 5)
    		||  (gGStatistSosInfo_76[eDiagSos76_PreSos] >= 5))))

    	{
    		/*SOS重写EEPROM*/
        	ParaWriteStoreSOSLockInfo(1, gGStatistSosInfo_76, 4);

    		/*更新SOS*/
    		if(sHisSosLev < 5)
    		{
    			sHisSosLev = 5;
    		}
    		else
    		{
    			sHisSosLev = 0;
    		}
    	}
		#else
    	/*SOS4级为锁定需要记录*/
    	if(((sHisSosLev >= 4)
    		&& (gGStatistSosInfo_76[eDiagSos76_NowSos] < 4)
			&&  (gGStatistSosInfo_76[eDiagSos76_PreSos] < 4))
    		|| ((sHisSosLev < 4)
    	    && ((gGStatistSosInfo_76[eDiagSos76_NowSos] >= 4)
    		||  (gGStatistSosInfo_76[eDiagSos76_PreSos] >= 4))))
    	{
    		/*SOS重写EEPROM*/
        	ParaWriteStoreSOSLockInfo(1, gGStatistSosInfo_76, 4);

    		/*更新SOS*/
    		if(sHisSosLev < 4)
    		{
    			sHisSosLev = 4;
    		}
    		else
    		{
    			sHisSosLev = 0;
    		}
    	}
		#endif
    }

    //显示到故障码字段(不显示0级实时码)
    gGFaultInfo_54[3] = gGStatistSosInfo_76[eDiagSos76_NowRsn] & 0x03fe;
	#endif
}

/*=============================================================================================
** @Name      : u8 DiagGetGroupSosLevel(void)
** @Input     : void
** @Output    : 电池组总安全等级SOS
** @Function  : 获取电池组总安全等级SOS
** @The notes : 包括当前等级和推断等级
**===========================================================================================*/
u8 DiagGetGroupSosLevel(void)
{
	#if(1 == SOS_DIAG_TATIS_EN)/*任务使能*/
	/*取等级较高者*/
	if(gGStatistSosInfo_76[eDiagSos76_NowSos] > gGStatistSosInfo_76[eDiagSos76_PreSos])
    {
    	return(gGStatistSosInfo_76[eDiagSos76_NowSos]);
    }
	else
	{
		return(gGStatistSosInfo_76[eDiagSos76_PreSos]);
	}
	#else
	return(0);
	#endif
}

/*=============================================================================================
** @Name      : u8 DiagGetGroupSosState(u8 sort)
** @Input     : sort:SOS类型(0:当前SOS 1:预测SOS)
** @Output    : SOS安全等级状态
** @Function  : 获取电池组指定SOS安全等级状态
** @The notes :
**===========================================================================================*/
u8 DiagGetGroupSosState(u8 sort)
{
	#if(1 == SOS_DIAG_TATIS_EN)/*任务使能*/
	if(0 == sort)
	{
		return(gGStatistSosInfo_76[eDiagSos76_NowSos]);
	}
	else
	{
		return(gGStatistSosInfo_76[eDiagSos76_PreSos]);
	}
	#else
	return(0);
	#endif
}

/*=============================================================================================
** @Name      : u16 DiagGetGroupSosResion(u8 sort)
** @Input     : sort:SOS类型(0:当前SOS 1:预测SOS)
** @Output    : SOS安全等级异常原因
** @Function  : 获取电池组指定SOS安全等级异常原因
** @The notes : 按bit位表示原因
**===========================================================================================*/
u16 DiagGetGroupSosResion(u8 sort)
{
	#if(1 == SOS_DIAG_TATIS_EN)/*任务使能*/
	if(0 == sort)
	{
		return(gGStatistSosInfo_76[eDiagSos76_NowRsn]);
	}
	else
	{
		return(gGStatistSosInfo_76[eDiagSos76_PreRsn]);
	}
	#else
	return(0);
	#endif
}

/*=============================================================================================
** @Name      : void DiagClrGroupSosLevel(u8 flag)
** @Input     : flag:清除标志(0:清除普通级别 0xff:清除锁定级别)
** @Output    : void
** @Function  : 清除电池组总安全等级SOS
** @The notes : 包括当前等级和推断等级
**===========================================================================================*/
void DiagClrGroupSosLevel(u8 flag)
{
	/*区分清除锁定还是普通级别*/
	if((0xff == flag) || ((0 == flag)
		&& (gGStatistSosInfo_76[eDiagSos76_NowSos] < 4)
		&& (gGStatistSosInfo_76[eDiagSos76_PreSos] < 4)))
	{
		gGStatistSosInfo_76[eDiagSos76_NowSos] = 0;
		gGStatistSosInfo_76[eDiagSos76_NowRsn] = 0;
		gGStatistSosInfo_76[eDiagSos76_PreSos] = 0;
		gGStatistSosInfo_76[eDiagSos76_PreRsn] = 0;
	}
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/
#if(1 == SOS_DIAG_TATIS_EN)/*任务使能*/
/*=================================================================================================
** @Name      : static u8 DiagGroupErrNowSosLev(void)
** @Input     : void
** @Output    : 当前电池组故障告警对应安全等级SOS[0, 4]
** @Function  : 诊断当前电池组故障告警对应安全等级SOS
** @The notes :
**===============================================================================================*/
static u8 DiagGroupErrNowSosLev(void)
{
    u8 noeSosLev = 0;
    u8 i = 0;
	e_ErrSort filsort = 0;
	/*判定各故障告警对应安全等级*/
	for(i = 0; i < eERR_ID_FINISH; i++)
	{
		/*故障告警发生*/
		if(eErr_B == DiagGetErrLastStateAPI((e_ERR_ID)i))
		{
			if(MU2KeepPowerMode == 1)
			{
				filsort = DiagGetFilErrSortAPI((e_ERR_ID)i);
			}
			else
			{
				filsort = gErrFilPara[i].errSort;
			}

			//保供电模式
			u8 max = gErrFilPara[i].maxTimes;
			if (MU2KeepPowerMode == 1) 
			{
				 if ((i == 24) || (i == 25) )    //充电过流[8]、放电过流[9]、绝缘一级故障[27]
				{
					max = 6;  // 可恢复次数改为5次
				}
			}
			/*4级[故障保护且锁定]:指定高风险故障*/
			if((eErr_SysErr == filsort)
				&& ((eERR_ID_FIRE_EXP == i)
				|| (eERR_ID_BATT_SMOG == i)))
			{
				noeSosLev = 4;               //不可恢复  高压下电标志位
				break;
			}
			/*3级[故障保护不恢复]:故障保护且不可恢复*/
			else if((eErr_SysErr == filsort)
				&& (255 != max))
			{
				/*到达不自动消除次数*/
				if(gErrFilTab[i].resTimes >= max)
				{
					if(noeSosLev < 3)
					{
						noeSosLev = 3;
					}
				}
				else
				{
					if(noeSosLev < 2)
					{
						noeSosLev = 2;
					}
				}
			}
			/*2级[故障保护自恢复]:故障保护可恢复或充放电保护自恢复*/
			else if((eErr_SysErr == filsort)
				/*|| (eErr_SysPro == gErrFilPara[i].errSort)*/)
			{
				if(noeSosLev < 2)
				{
					noeSosLev = 2;
				}
			}
			/*1级[充放电限制自恢复]:充放电限制自恢复*/
			else if((eErr_SysLim == filsort)
				|| (eErr_SysPro == filsort))
			{
				if(noeSosLev < 1)
				{
					noeSosLev = 1;
				}
			}
		}
	}

	/*记录SOS等级异常原因*/
	if(noeSosLev > 0)
	{
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_ERR);
	}
	else
	{
		BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_ERR);
	}

	return(noeSosLev);
}

/*=================================================================================================
** @Name      : static u8 DiagGroupExpNowSosLev(void)
** @Input     : void
** @Output    : 当前电池组异常信息值对应安全等级SOS[0, 4]
** @Function  : 诊断当前电池组异常信息值安全等级SOS
** @The notes : 判断当前值是否正常
**===============================================================================================*/
static u8 DiagGroupExpNowSosLev(void)
{
    u8 i = 0;
    u8 noeSosLev = 0;
    static u16 sIdleTime = 0;
    static u16 sVDnTime = 0;
    static u16 sTUpTime = 0;
    static u16 sHisMinVolt = 0;
    static s8 sHisMaxTemp = 0;
    static u16 sCurrTime = 0;
    //[3级]单体电压极限值[恢复正常](不可自动恢复)
    /*if((GetGCellMaxVoltAPI() < gGBmuGenPara_102[eBmuGenPara102_VH2Lim])
        && (GetGCellMinVoltAPI() > gGBmuGenPara_102[eBmuGenPara102_VL2Lim]))
    {
    	//清除原因(电压极限值)
		BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VEX);
    }
    //单体电压到达极限值(排除采样异常)
    else */if(((GetGCellMaxVoltAPI() >= DEFAULT_EX_MAXV) && (eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VH1)))
    	|| ((GetGCellMinVoltAPI() <= DEFAULT_EX_MINV) && (eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VL1)))
		|| (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VEX)))
    {
    	/*记为3级*/
		if(noeSosLev < 3)
		{
			noeSosLev = 3;
		}

		//新产生
		if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VEX))
		{
			//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_CellVExt, eERR_RCDSORT_WARSET, 0,  0);
		}

		//记录原因(电压极限值)
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VEX);
    }

    //[3级]单体温度极限值[恢复正常](不可自动恢复)
    /*if((GetGCellMaxTempAPI() < gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim])
        && (GetGCellMinTempAPI() > gGBmuGenPara_102[eBmuGenPara102_ChgTL2Lim]))
    {
    	//清除原因(温度极限值)
		BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TEX);
    }
    //单体温度到达极限值(排除采样异常)
    else */if(((((GetGCellMaxTempAPI() + 5) / 10) >= BATT_EXEREM_MAXT) && (eErr_B == DiagGetErrFilStateAPI(eERR_ID_DHG_TH1)))
    	|| ((((GetGCellMinTempAPI() + 5) / 10) <= BATT_EXEREM_MINT) && (eErr_B == DiagGetErrFilStateAPI(eERR_ID_DHG_TL1)))
		|| (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TEX)))
    {
    	/*记为3级*/
		if(noeSosLev < 3)
		{
			noeSosLev = 3;
		}

		//新产生
		if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TEX))
		{
			//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_CellTExt, eERR_RCDSORT_WARSET, 0,  0);
		}

		//记录原因(温度极限值)
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TEX);
    }

    //[3级]均衡温度极限值[恢复正常](不可自动恢复)
    /*if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_BAL_TH))
    {
    	//清除原因(均衡板温极限值)
		BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_BTE);
    }
    //均衡温度到达极限值(排除采样异常)
    else */if(((((GetGBalanMaxTempAPI() + 5) / 10) >= DIAG_BALAN_EXT_T) && (eErr_B == DiagGetErrFilStateAPI(eERR_ID_BAL_TH)))
		|| (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_BTE)))
    {
    	/*记为3级*/
		if(noeSosLev < 3)
		{
			noeSosLev = 3;
		}

    	/*记录均衡关闭控制异常*/
		DiagRecordBalErrFlagAPI(GetGBalanMaxTSlaveAPI(), (0xffffffff >> (32 - gGBmuHigLevPara_103[eBmuHigLevPara103_BsuVNum])));

		//新产生
		if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_BTE))
		{
			//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_BalaTExt, eERR_RCDSORT_WARSET, 0,  0);
		}

		//记录原因(均衡板温极限值)
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_BTE);
    }
    else
    {
    	/*有异常则清除*/
    	if(eErr_B == DiagGetErrFilStateAPI(eERR_ID_BAL_CEXP))
    	{
    		for(i = 0; i < gGBmuHigLevPara_103[eBmuHigLevPara103_BsuNum]; i++)
    		{
    			DiagRecordBalErrFlagAPI(i, 0);
    		}
    	}
    }

    //[3级]低压板温极限值[恢复正常](不可自动恢复)
    /*if(eErr_A == DiagGetErrFilStateAPI(eERR_ID_BAL_TH))
    {
    	//清除原因(均衡板温极限值)
		BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_DTE);
    }
    //低压板温到达极限值(排除采样异常)
    else */if(((((GetGBalanMaxTempAPI() + 5) / 10) >= DIAG_LVPWR_EXT_T) && (eErr_B == DiagGetErrFilStateAPI(eERR_ID_DCV_TH)))
		|| (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_DTE)))
    {
    	/*记为3级*/
		if(noeSosLev < 3)
		{
			noeSosLev = 3;
		}

		//新产生
		if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_DTE))
		{
			//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_DCBTExt, eERR_RCDSORT_WARSET, 0,  0);
		}

		//记录原因(均衡板温极限值)
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_DTE);
    }

    /*[3级]电压压差极大(同时过压欠压)*/
    if(((eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VH2))
    	&& (eErr_B == DiagGetErrFilStateAPI(eERR_ID_CEL_VL2)))
    	|| (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDI)))
    {
    	/*记为3级*/
		if(noeSosLev < 3)
		{
			noeSosLev = 3;
		}

		//记录原因(电压压差极大)
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDI);
    }

    /*[0级]温度温差极大(同时高低温)*/
    if(((eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_TH2))
    	&& (eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_TL2)))
    	|| (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TDI)))
    {
    	/*记为3级*/
		if(noeSosLev < 3)
		{
			noeSosLev = 3;
		}

		//记录原因(温度温差极大)
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TDI);
    }

    //[4级]电压温度均突变(且有气体告警)
    if((1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN))
    	&& (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP))
		 && (eErr_B == DiagGetErrFilStateAPI(eERR_ID_CGAS_EXP)))
    {
        //进入4级锁定状态
    	noeSosLev = 4;
    	sIdleTime = 0;
    }
    //[3级]电压温度同时突变(已发生则不自动清除)
    else if((1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN))
    	&& (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP)))
    {
    	/*保持为3级*/
		if(noeSosLev < 3)
		{
			noeSosLev = 3;
		}
    	sIdleTime = 0;
    }
    //[2级]电压温度之一突变(自动清除)
    //空闲状态采样正常
    else if((eCURR_IDLE == GetGChgDhgStateAPI()) && (0 == DiagGetCellVTExpStateHOOKk()) && (sIdleTime > 0))
	{
    	//静置1min
    	if(sIdleTime > 600)
    	{
    		//电压下降或已检测到电压下降
    		if((GetGCellMinVoltAPI() < sHisMinVolt) || (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN)))
    		{
				//下降250mV或3.1V以上下降150mV
				if(((GetGCellMinVoltAPI() + 250) <= sHisMinVolt) || (((GetGCellMinVoltAPI() + 150) <= sHisMinVolt)
					&& (sHisMinVolt >= (gGBmuGenPara_102[eBmuGenPara102_CellPlaDnV] - 50))))
				{
			    	/*记为2级*/
					if(noeSosLev < 2)
					{
						noeSosLev = 2;
					}

					//新产生
					if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN))
					{
						//记录事件
						DiagUserSelfErrRecord(eUDef_ErrRcd_CellVDn, eERR_RCDSORT_WARSET, (sHisMinVolt - GetGCellMinVoltAPI()),  0);
					}

					//记录原因(压降异常)
					BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN);

					//记录最低电压
					sVDnTime = 0;
					sHisMinVolt = GetGCellMinVoltAPI() + 50;
				}
				//下降未达100mv
				else
				{
					//再等待1min仍未下降100mv
					if((sVDnTime >= 1800) || ((sVDnTime >= 600)
						&& (0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN))))
					{
						//记录最低电压
						sVDnTime = 0;
						sHisMinVolt = GetGCellMinVoltAPI();

						//清除原因(自动消除)
						BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN);
					}
					else
					{
						sVDnTime++;
						//已记录压降未到消除时间
						if(1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN))
						{
					    	/*保持2级*/
							if(noeSosLev < 2)
							{
								noeSosLev = 2;
							}
						}
					}
				}
    		}
    		//非电压下降
        	else
        	{
            	//记录最低电压
				sVDnTime = 0;
            	sHisMinVolt = GetGCellMinVoltAPI();
        	}

    		//15℃以上温度上升或已记录温升
    		if(((((GetGCellMaxTempAPI() + 5) / 10) > sHisMaxTemp) && (sHisMaxTemp >= 15))
    			|| (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP)))
    		{
				//上升10℃或30℃以上上升5℃
				if((((GetGCellMaxTempAPI() + 5) / 10) >= (sHisMaxTemp + 10))
					|| ((((GetGCellMaxTempAPI() + 5) / 10) >= (sHisMaxTemp + 5))
					&& (sHisMaxTemp >= 30)))
				{
			    	/*温度较高时记为2级*/
					if((noeSosLev < 2) && (((GetGCellMaxTempAPI() + 5) / 10) >= (gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim] - gGBmuGenPara_102[eBmuGenPara102_CTH2Clr])))
					{
						noeSosLev = 2;
					}
			    	/*记为1级*/
					else if(noeSosLev < 1)
					{
						noeSosLev = 1;
					}

					//新产生
					if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP))
					{
						//记录事件
						DiagUserSelfErrRecord(eUDef_ErrRcd_CellTUp, eERR_RCDSORT_WARSET, (((GetGCellMaxTempAPI() + 5) / 10) - sHisMaxTemp),  0);
					}

					//记录原因(温升异常)
					BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP);

					//记录最高温度
					sTUpTime = 0;
					sHisMaxTemp = ((GetGCellMaxTempAPI() + 5) / 10) - 2;
				}
				//上升未达5℃
				else
				{
					//再等待2min仍未上升5℃
					if((sTUpTime >= 3000) || ((sTUpTime >= 1200)
						&& (0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP))))
					{
						//记录最高温度
						sTUpTime = 0;
						sHisMaxTemp = (GetGCellMaxTempAPI() + 5) / 10;

						//清除原因(自动恢复)
						BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP);
					}
					else
					{
						sTUpTime++;
						//已记录温升未到消除时间
						if(1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP))
						{
							/*温度较高时记为2级*/
							if((noeSosLev < 2) && (((GetGCellMaxTempAPI() + 5) / 10) >= (gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim] - gGBmuGenPara_102[eBmuGenPara102_CTH2Clr])))
							{
								noeSosLev = 2;
							}
							/*记为1级*/
							else if(noeSosLev < 1)
							{
								noeSosLev = 1;
							}
						}
					}
				}
    		}
    		//非温度上升
        	else
        	{
            	//记录最高温度
				sTUpTime = 0;
            	sHisMaxTemp = (GetGCellMaxTempAPI() + 5) / 10;
        	}
    	}
		else if(sIdleTime == 600)//记录静置1min时的电压
		{
			sHisMinVolt = GetGCellMinVoltAPI();
			sHisMaxTemp = (GetGCellMaxTempAPI() + 5) / 10;
		}
    	//静置未满足1min
    	else
    	{
        	//空闲计时
        	sIdleTime++;
			sVDnTime = 0;
			sTUpTime = 0;

			//静置1min内下降300mV(充电刚停止下降500mV)或已记录压降
			if(((GetGCellMinVoltAPI() + 500) <= sHisMinVolt) || (((GetGCellMinVoltAPI() + 300) <= sHisMinVolt) && (sHisMinVolt <= gGBmuGenPara_102[eBmuGenPara102_CellPlaUpV]))
				|| (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN)))
			{
		    	/*记为2级*/
				if(noeSosLev < 2)
				{
					noeSosLev = 2;
				}

				//新产生
				if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN))
				{
					//记录事件
					DiagUserSelfErrRecord(eUDef_ErrRcd_CellVDn, eERR_RCDSORT_WARSET, (sHisMinVolt - GetGCellMinVoltAPI()),  0);
				}

				//记录原因(压降异常)
				BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_VDN);
			}

        	//静置1min内15℃以上温度突变15℃或30℃以上温度突变8℃或已记录温升
        	if(((((GetGCellMaxTempAPI() + 5) / 10) >= (sHisMaxTemp + 15)) && (sHisMaxTemp >= 15))
        		|| ((((GetGCellMaxTempAPI() + 5) / 10) >= (sHisMaxTemp + 8)) && (sHisMaxTemp >= 30))
				|| (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP)))
        	{
        		/*温度较高时记为2级*/
				if((noeSosLev < 2) && (((GetGCellMaxTempAPI() + 5) / 10) >= (gGBmuGenPara_102[eBmuGenPara102_ChgTH2Lim] - gGBmuGenPara_102[eBmuGenPara102_CTH2Clr])))
				{
					noeSosLev = 2;
				}
				/*记为1级*/
				else if(noeSosLev < 1)
				{
					noeSosLev = 1;
				}

				//新产生
				if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP))
				{
					//记录事件
					DiagUserSelfErrRecord(eUDef_ErrRcd_CellTUp, eERR_RCDSORT_WARSET, (((GetGCellMaxTempAPI() + 5) / 10) - sHisMaxTemp),  0);
				}

				//记录原因(温升异常)
				BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_TUP);
        	}
    	}
	}
    //未初始化或非空闲状态
    else
    {
    	//记录最低电压和最高温度
		sVDnTime = 0;
		sTUpTime = 0;
    	sIdleTime = 1;
    	sHisMinVolt = (GetGCellMinVoltAPI() + 5) / 10;
    	sHisMaxTemp = (GetGCellMaxTempAPI() + 5) / 10;
    }

    /*[4级]电流过流极大(极限值)*/
    if((GetGSampOutCurrAPI() <= (0 - (s32)BATT_EXEREM_MAXC))
        || (GetGSampOutCurrAPI() >= BATT_EXEREM_MAXC)
	    || (1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEX)))
    {
    	/*记为4级*/
		noeSosLev = 4;

		//新产生
		if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEX))
		{
			//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_CurrExt, eERR_RCDSORT_WARSET, 0,  0);
		}

		//记录原因(电流过流极大)
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEX);
    }

    /*[0级]电流待机异常(无法零电流待机)*/
    if(((eCURR_CHG == GetGChgDhgStateAPI())
        && (0 == GetGChgLimCurrAPI()))
	    || ((eCURR_DHG == GetGChgDhgStateAPI())
	    && (0 == GetGDhgLimCurrAPI())))
    {
    	//电流持续1min
    	if(sCurrTime >= 600)
    	{
			//[0级]只记录事件
    		if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CER))
    		{
    			//新产生则记录事件
    			DiagUserSelfErrRecord(eUDef_ErrRcd_CurrIdle, eERR_RCDSORT_WARSET, 0,  0);
    		}

			//[0级]只记录原因(电流待机异常)
			BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CER);
    	}
    	else
    	{
    		sCurrTime++;
    	}
    }
    else
    {
    	sCurrTime = 0;
		BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CER);
    }

	//已发生3级总容量异常
//	if((1 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CAP))
//		&& (3 == gGStatistSosInfo_76[eDiagSos76_NowSos]))
//	{
//		//3级不自动消除
//		if(noeSosLev < 3)
//		{
//			noeSosLev = 3;
//		}
//	}

    /*[3级]实际总容量小于标称容量60%*/
    if((GetGroupTotalCapAPI() * 100 / GetGroupStandCapAPI()) < 60)
    {
//		//记录3级异常(容量异常)
//		if(noeSosLev < 3)
//		{
//			noeSosLev = 3;
//		}

		//新产生
		if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CAP))
		{
			//记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_CapDrop, eERR_RCDSORT_WARSET, 0,  0);
		}

		//[3级]记录原因(容量异常)
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CAP);
    }
    /*[0级]实际总容量小于标称容量80%*/
    else if((GetGroupTotalCapAPI() * 100 / GetGroupStandCapAPI()) < 80)
    {
		//[0级]只记录事件
		if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CAP))
		{
			//新产生则记录事件
			DiagUserSelfErrRecord(eUDef_ErrRcd_CapDrop, eERR_RCDSORT_WARSET, 0,  0);
		}

		//[0级]只记录原因(容量异常)
		BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CAP);
    }
    else
    {
		//清除原因(容量异常)
		BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CAP);
    }
		
    return(noeSosLev);
}

/*=================================================================================================
** @Name      : static u8 DiagGroupStatisSosLev(void)
** @Input     : void
** @Output    : 当前电池组统计信息对应安全等级SOS[0, 4]
** @Function  : 诊断当前电池组统计信息对应安全等级SOS
** @The notes : 多次统计确认异常
**===============================================================================================*/
static u8 DiagGroupStatisSosLev(void)
{
    u8 i = 0;
    u8 dod60Flag = 0;
    u8 noeSosLev = 0;
    u32 chgEner = 0;
    u32 dhgEner = 0;
    static u8 sHisChgFull = 0;
    static u8 sHisDhgEmpty = 0;
    static u16 sHisFullCell = 0;
    static u16 sHisEmptyCell = 0;
    static u8 sHisTHProTime = 0;
    static u16 sHisTHProCell = 0;
    static u8 sHisDhgState = 0;
    static u16 sChgLastTime = 0;
    static u32 sHisChgEner = 0;
    static u32 sHisDhgEner = 0;

    //充电截止
    if(ePRO_FULL == GetGProtectStateAPI())
    {
    	//初次检测到
    	if(0 == sHisChgFull)
    	{
    		//记录充电截止电芯
    		sHisFullCell = GetGCellMaxVNumAPI();

    		//[0级]已记录放电截止电芯且与充电截止电芯相同并电压差异过大
    		if((sHisEmptyCell > 0) && (sHisEmptyCell == sHisFullCell)
    			&& (GetGCellMaxVoltAPI() >= GetGCellAvgVoltAPI() + 1500))
    		{
				//[0级]只记录事件
    			if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEL))
    			{
    				//新产生则记录事件
    				DiagUserSelfErrRecord(eUDef_ErrRcd_CellVPro, eERR_RCDSORT_WARSET, 0,  0);
    			}

				//记录原因(单体充放电保护)
				BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEL);
    		}
    		else
    		{
				//清除原因(单体充放电保护)
				BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEL);
    		}
    	}

    	sHisChgFull = 1;
    	sHisDhgEmpty = 0;
    }
    //非充电截止
    else
    {
    	sHisChgFull = 0;

        //放电截止
        if(ePRO_EMPTY == GetGProtectStateAPI())
        {
        	//初次检测到
        	if(0 == sHisDhgEmpty)
        	{
        		//记录放电截止电芯
        		sHisEmptyCell = GetGCellMaxVNumAPI();

        		//[0级]已记录充电截止电芯且与放电截止电芯相同并电压差异过大
        		if((sHisFullCell > 0) && (sHisFullCell == sHisEmptyCell)
        			&& ((GetGCellMinVoltAPI() + 1500) <= GetGCellAvgVoltAPI()))
        		{
    				//[0级]只记录事件
        			if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEL))
        			{
        				//新产生则记录事件
        				DiagUserSelfErrRecord(eUDef_ErrRcd_CellVPro, eERR_RCDSORT_WARSET, 0,  0);
        			}

    				//记录原因(单体充放电保护)
    				BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEL);
        		}
        		else
        		{
    				//清除原因(单体充放电保护)
    				BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEL);
        		}
        	}

        	sHisDhgEmpty = 1;
        }
        else
        {
        	sHisDhgEmpty = 0;
        }
    }

    //充放电过温保护
    if((eErr_B == DiagGetErrFilStateAPI(eERR_ID_CHG_TH2))
    	|| (eErr_B == DiagGetErrFilStateAPI(eERR_ID_DHG_TH2)))
    {
    	//初次检测到
    	if((0 == sHisTHProCell) || (0 == sHisTHProTime) || (1 == (sHisTHProTime % 2)))
    	{
    		//记录充放电保护电芯
    		sHisTHProCell = GetGCellMaxVNumAPI();

    		//[0级]已记录放电截止电芯且与充电截止电芯相同并电压差异过大
    		if((sHisEmptyCell > 0) && (sHisEmptyCell == sHisFullCell)
    			&& (GetGCellMaxVoltAPI() >= GetGCellAvgVoltAPI() + 1500))
    		{
				//[0级]只记录事件
    			if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEL))
    			{
    				//新产生则记录事件
    				DiagUserSelfErrRecord(eUDef_ErrRcd_CellVPro, eERR_RCDSORT_WARSET, 0,  0);
    			}

				//记录原因(单体充放电保护)
				BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEL);
    		}
    		else
    		{
				//清除原因(单体充放电保护)
				BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_CEL);
    		}
    	}

    	sHisChgFull = 1;
    	sHisDhgEmpty = 0;
    }
    //充放电过温保护消除
    else
    {
    	//偶数代表未记录过温消除
    	if(0 == (sHisTHProTime % 2))
    	{
    		sHisTHProTime++;
    	}
    }

    //放电转为充电(记录30个DOD后记录一次DOD则检测一次)
    if((2 == sHisDhgState) && (eCURR_CHG == GetGChgDhgStateAPI()) && (gGHisDodInfo_48[eHisDodInfo48_Num] >= 30))
    {
    	//充电持续1min
    	if(sChgLastTime >= 600)
    	{
    		//检测30个DOD
    		for(i = 0; i < 30; i++)
    		{
				//有DOD超过60%
				if(gGHisDodInfo_48[eHisDodInfo48_Dod + i] >= 60)
				{
					//记录有达标60%的DOD
					dod60Flag = 1;

					//清除原因(DOD未达标)
					BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_DOD);
					break;
				}
    		}

    		//[0级]无到达60%的DOD
    		if(0 == dod60Flag)
    		{
				//[0级]只记录事件
    			if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_DOD))
    			{
    				//新产生则记录事件
    				DiagUserSelfErrRecord(eUDef_ErrRcd_DodLack, eERR_RCDSORT_WARSET, 0,  0);
    			}

				//记录原因(DOD未达标)
				BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_DOD);
    		}

    		sChgLastTime = 0;
    		sHisDhgState = 0;
    	}
    	else
    	{
    		sChgLastTime++;
    	}
    }
    else
    {
    	//记录放电状态
    	if(eCURR_DHG == GetGChgDhgStateAPI())
    	{
    		sHisDhgState = 2;
    	}
    	sChgLastTime = 0;
    }

	//已发生3级充放电效率异常
//	if((1 ==  BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_EEX))
//		&& (3 == gGStatistSosInfo_76[eDiagSos76_NowSos]))
//	{
//		//3级不自动消除
//		if(noeSosLev < 3)
//		{
//			noeSosLev = 3;
//		}
//	}

    //充电到3.5V时计算充放电效率
    if(eCURR_CHG == GetGChgDhgStateAPI())
    {
        //最大电压到3.5V
    	if(GetGCellMaxVoltAPI() >= 3500)
    	{
    		//未初始化
    		if((0 == sHisChgEner) && (0 == sHisDhgEner))
    		{
        		//初始化
    			sHisChgEner = gGStatistHisEner_74[eHisEner74_HisChgEner];
    			sHisDhgEner = gGStatistHisEner_74[eHisEner74_HisDhgEner];
    		}
    		//有充放电量超100kWh
    		else if((gGStatistHisEner_74[eHisEner74_HisChgEner] >= (sHisChgEner + 1000))
    			&& (gGStatistHisEner_74[eHisEner74_HisDhgEner] >= (sHisDhgEner + 1000)))
    		{
    			/*求充放电能量*/
    			chgEner = gGStatistHisEner_74[eHisEner74_HisChgEner] - sHisChgEner;
    			dhgEner = gGStatistHisEner_74[eHisEner74_HisDhgEner] - sHisDhgEner;

    			/*[3级]充放电能量比不足60%*/
				if(((chgEner > dhgEner) && ((dhgEner * 100 / chgEner) < 60))
					|| ((dhgEner > chgEner) && ((chgEner * 100 / dhgEner) < 60)))
				{
					//[3级]安全等级
//					if(noeSosLev < 3)
//					{
//						noeSosLev = 3;
//					}

					//记录事件
	    			if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_EEX))
	    			{
	    				//新产生则记录事件
	    				DiagUserSelfErrRecord(eUDef_ErrRcd_ChgEffe, eERR_RCDSORT_WARSET, chgEner,  dhgEner);
	    			}

					//[3级]记录原因(充放电效率未达标)
					BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_EEX);
				}
    			/*[0级]充放电能量比不足80%*/
				else if(((chgEner > dhgEner) && ((dhgEner * 100 / chgEner) < 80))
					|| ((dhgEner > chgEner) && ((chgEner * 100 / dhgEner) < 80)))
				{
					//[0级]只记录事件
	    			if(0 == BitGet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_EEX))
	    			{
	    				//新产生则记录事件
	    				DiagUserSelfErrRecord(eUDef_ErrRcd_ChgEffe, eERR_RCDSORT_WARSET, chgEner,  dhgEner);
	    			}

					//[0级]只记录原因(充放电效率未达标)
					BitSet(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_EEX);
				}
				else
				{
					//清除原因(充放电效率未达标)
					BitClr(gGStatistSosInfo_76[eDiagSos76_NowRsn], eSOS_REASION_EEX);
				}

        		//重新记录
    			sHisChgEner = gGStatistHisEner_74[eHisEner74_HisChgEner];
    			sHisDhgEner = gGStatistHisEner_74[eHisEner74_HisDhgEner];
    		}
    		//历史值复位
    		else if((gGStatistHisEner_74[eHisEner74_HisChgEner] < sHisChgEner)
    			|| (gGStatistHisEner_74[eHisEner74_HisDhgEner] < sHisDhgEner))
    		{
        		//重新初始化
    			sHisChgEner = gGStatistHisEner_74[eHisEner74_HisChgEner];
    			sHisDhgEner = gGStatistHisEner_74[eHisEner74_HisDhgEner];
    		}
    	}
    }

    return(noeSosLev);
}

#if(1 == SOS_5_SystemLock_EN)
/*=================================================================================================
** @Name      : static u8 DiagGroupSosSysLockSosLev(void)
** @Input     : void
** @Output    : 当前电池组系统锁统计信息对应安全等级SOS[0, 5]
** @Function  : 诊断当前电池组系统锁统计信息对应安全等级SOS
** @The notes : 多次统计确认是否存在5级sos异常
**===============================================================================================*/
static u8 DiagGroupSosSysLockSosLev(void)
{
	u8 noeSosLev = 0;
	static u16 sCellOverVoltTime = 0;
	static u16 sCellUnderVoltTime = 0;
	static u16 sSysOverVoltTime = 0;
	static u16 sSysUnderVoltTime = 0;
	static u16 sChgCellOverTempTime = 0;
	static u16 sDhgCellOverTempTime = 0;
	static u16 sChgCellUnderTempTime = 0;
	static u16 sDhgCellUnderTempTime = 0;
	static u16 sChgCurrTime = 0;
	static u16 sDhgCurrTime = 0;
	/*[5级]电芯电压过压异常(电芯电压>=3.8V 持续 100ms),系统延迟较大，到实际接触器完全断开接近1s*/
	if(GetGCellMaxVoltAPI() >= SOS_5_CellOverVoltLimit)
	{
		if(sCellOverVoltTime >= 1)
		{
			DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_COV,1);
			noeSosLev = 5;
		}
		else
		{
			sCellOverVoltTime++;
		}
	}
	else
	{
		sCellOverVoltTime = 0;
		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_COV,0);
	}

	/*[5级]电芯电压欠压异常(电芯电压<=2.0V 持续 100ms)*/
	if(GetGCellMinVoltAPI() <= SOS_5_CellUnderVoltLimit)
	{
		if(sCellUnderVoltTime >= 1)
		{
			DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_CUV,1);
			noeSosLev = 5;
		}
		else
		{
			sCellUnderVoltTime++;
		}
	}
	else
	{
		sCellUnderVoltTime = 0;
		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_CUV,0);
	}

	/*[5级]系统总压过压异常(系统总压>=912V 持续 100ms)*/
	if(GetGCellSumVoltAPI() >= SOS_5_SysOverVoltCellLimit*gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
	{
		if(sSysOverVoltTime >= 1)
		{
			DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_SOV,1);
			noeSosLev = 5;
		}
		else
		{
			sSysOverVoltTime++;
		}
	}
	else
	{
		sSysOverVoltTime = 0;
		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_SOV,0);
	}

	/*[5级]系统总压欠压异常(系统总压<=480V 持续 100ms)*/
	if(GetGCellSumVoltAPI() <= SOS_5_SysUnderVoltCellLimit*gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
	{
		if(sSysUnderVoltTime >= 1)
		{
			DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_SUV,1);
			noeSosLev = 5;
		}
		else
		{
			sSysUnderVoltTime++;
		}
	}
	else
	{
		sSysUnderVoltTime = 0;
		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_SUV,0);
	}

	/*[5级]电芯充电温度过温异常(电芯充电温度>=60℃ 持续 4.5s)*/
	if(GetGCellMaxTempAPI() >= SOS_5_CellChgOverTempLimit)
	{
		if(sChgCellOverTempTime >= 45)
		{
			DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_COT,1);
			sChgCellOverTempTime = 0;
			noeSosLev = 5;
		}
		else
		{
			sChgCellOverTempTime++;
		}
	}
	else
	{
		sChgCellOverTempTime = 0;
		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_COT,0);
	}

	/*[5级]电芯放电温度过温异常(电芯放电温度>=60℃ 持续 4.5s)*/
	if(GetGCellMaxTempAPI() >= SOS_5_CellDhgOverTempLimit)
	{
		if(sDhgCellOverTempTime >= 45)
		{
			DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_DOT,1);
			sDhgCellOverTempTime = 0;
			noeSosLev = 5;
		}
		else
		{
			sDhgCellOverTempTime++;
		}
	}
	else
	{
		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_DOT,0);
		sDhgCellOverTempTime = 0;
	}

	/*[5级]电芯充电温度欠温异常(电芯充电温度<=0℃ 持续 4.5s)*/
	if(GetGCellMinTempAPI() <= SOS_5_CellChgUnderTempLimit)
	{
		if(sChgCellUnderTempTime >= 45)
		{
			DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_CUT,1);
			noeSosLev = 5;
		}
		else
		{
			sChgCellUnderTempTime++;
		}
	}
	else
	{
		sChgCellUnderTempTime = 0;
		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_CUT,0);
	}

	/*[5级]电芯放电温度欠温异常(电芯放电温度<=-20℃  持续  4.5s)*/
	if(GetGCellMinTempAPI() <= (s16)SOS_5_CellDhgUnderTempLimit)
	{
		if(sDhgCellUnderTempTime >= 45)
		{
			DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_DUT,1);
			noeSosLev = 5;
		}
		else
		{
			sDhgCellUnderTempTime++;
		}
	}
	else
	{
		sDhgCellUnderTempTime = 0;
		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_DUT,0);
	}

    /*[5级]充电电流异常(充电电流>=285A 持续 4.5s)*/
    if((eCURR_CHG == GetGChgDhgStateAPI())
        && (GetGSampOutCurrAPI() <= (0 - (s16)SOS_5_ChgCurrLimit)))
    {
    	//电流持续4.5S
    	if(sChgCurrTime >= 45)
    	{
    		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_COC,1);
    		noeSosLev = 5;
    	}
    	else
    	{
    		sChgCurrTime++;
    	}
    }
    else
    {
    	sChgCurrTime = 0;
    	DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_COC,0);
    }

    /*[5级]放电电流异常(放电电流>=364.8A 持续 4.5s)*/
    if((eCURR_DHG == GetGChgDhgStateAPI())
        && (GetGSampOutCurrAPI() >= (s16)SOS_5_DhgCurrLimit))
    {
    	//电流持续4.5S
    	if(sDhgCurrTime >= 45)
    	{
    		DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_DOC,1);
    		noeSosLev = 5;
    	}
    	else
    	{
    		sDhgCurrTime++;
    	}
    }else
    {
    	sDhgCurrTime = 0;
    	DiagJudgeSosSysLockBitErrorAPI(eSOSSYSLOCK_REASION_DOC,0);
    }

	return(noeSosLev);


}

/*=================================================================================================
** @Name      : u32 DiagGetSosSysLockErrorBitAPI(void)
** @Input     : void
** @Output    : 5级sos异常标志(按位表示触发系统锁原因)
** @Function  : 获取5级sos异常标志(按位表示触发系统锁原因)
** @The notes : 按位表示1个通道
**===============================================================================================*/
u32 DiagGetSosSysLockErrorBitAPI(void)
{
	return (sSosSysLockErrBit);
}

/*=================================================================================================
** @Name      : void DiagJudgeSosSysLockBitErrorAPI(u8 err, u8 flag)
** @Input     : err:触发sos系统锁原因标志位 flag:1设置错误标志位 0清除错误标志位
** @Output    : void
** @Function  : 设置sos系统锁异常标志(按位表示触发sos5级系统锁原因)
** @The notes : 按位表示1个原因
**===============================================================================================*/
void DiagJudgeSosSysLockBitErrorAPI(u8 err, u8 flag)
{
	/*参数异常*/
	if(flag > 1||err >= eSOSSYSLOCK_REASION_LEN)
	{
		return;
	}

	if(flag == 0)
	{
		BitClr(sSosSysLockErrBit, err);
	}
	else
	{
		BitSet(sSosSysLockErrBit, err);
	}
}
#endif
#endif
