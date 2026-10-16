/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : AuxDevTactic.c
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 辅助设备控制策略模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "ParaUser.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "DiagSos.h"
#include "DiagInterface.h"
#include "TacInterface.h"
#include "MbInterface.h"
#include "HeatModbusMap.h"
#include "EventUser.h"
#include "AuxDevMsg.h"
#include "AuxDevTactic.h"
#include "MbUserDef.h"
#include "TacUser.h"
#include "Diaginterface.h"
#include "SocInterface.h"               //手动校准SOC
#include "SlaveMsg.h"                   //重置AFE及BSU状态调用
#include "GroupTactic.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern u16 gEu3AirInfo_202[];       /*EU3空调控制信息*/
extern u16 gEu3AirState_203[];       /*EU3空调状态信息*/
extern u16 gGImeuRcvInfo_204[];			/*IMEU辅助板接收信息*/
extern u16 gLiqudCoolAirInfo_206[];     /*液冷空调控制信息*/
extern u16 gLiqudCoolAirSta_207[];      /*液冷空调状态信息*/
extern u16 gTFAirUserParaInfo_212[];    /*TF液冷空调用户参数信息*/
extern u16 gTFAirStateInfo_213[];       /*TF液冷空调状态信息*/
extern u16 gTFAirErrInfo_214[];         /*TF液冷空调故障信息*/
extern u16 XinMiaoAirSta_215[];      /*新淼空调状态信息*/
extern u16 XinMiaoAirPara_216[];     /*新淼空调配置信息*/
extern u16 gImeu3DeInfo_220[];           /*除湿机信息*/

extern u16 gGBmuGenPara_102[];			/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];		/*主控高级参数值*/
extern u8 gGHardPara_104[]; 			/*主控硬件参数配置信息*/
extern u16 gImeu3PduOutInfo_219[];      /*PDU外部IMEU3信息*/

extern u16 gGSysCmdCtrl_86[];			/*系统控制命令信息数组*/

s16 sAvgCurr1min = 0;			/*1分钟平均电流 0.1A*/
//#if(BMU_IMLU2_EN == 1)
static u8 MU2LightColorCmd = 0;       	/*Imlu2灯带颜色*/
static u8 sRcvImlu2Count = 0;           /*Imlu2信息计数*/

static u16 MU2PcsAirCmd = 0;       	    /*Pcs测空调开关机控制*/
static u16 MU2CellAirCmd_Ctrl = 0;           /*电池侧空调开关机控制*/
static u16 MU2AirNoiseCmd = 6;           /*自研EU3空调降噪控制开关*/
static u16 MU2_TFAirON_OFF_ResetCmd = 0; /*TF空调开关机及报警复位*/
static u16 MU2_TFAir_TypeChoose_Cmd = 0; /*TF空调模式选择  高收益模式、长寿命模式*/
static u16 MU2_XMAir_TypeChoose_Cmd = 0; /*XM空调模式选择  高收益模式、长寿命模式*/
static u8 MU2Cell_AirCmd_Flag = 1;			/*电池侧自研EU3空调开关机控制标志位   默认原来的模式flag = 1*/
static u8 MU2Cell_AirCmd_Rcved = 0;			/*by lxr01202MU2 Cell Air Cmd Received Flag*/
u16 MU2CellTemSpePoinCmd = 0;           /*MU2控制是否屏蔽电池特殊点位温度*/
//以下手动控制的默认初始值都是0
static u16 MU2Eu3CellPump_ManualEn = 0;
static u16 MU2Eu3CellPump_ManualSpd = 0;
static u16 MU2Eu3ModPump_ManualEn = 0;
static u16 MU2Eu3ModPump_ManualSpd = 0;
static u16 MU2ElecValve_ManualEn = 0;
static u16 MU2ElecValve_ManualPulse = 0;
static u16 MU2Fan_ManualEn = 0;
static u16 MU2Fan_ManualSpdPerc = 0;
static u16 MU2Comp_ManualEn = 0;
static u16 MU2Comp_ManualFreq = 0;
static u16 MU2CellHeat_ManualEn = 0;
static u16 MU2CellHeat_ManualOn = 0;
static u16 MU2BatFilm_ManualEn = 0;
static u16 MU2BatFilm_ManualOn = 0;
static u16 MU2CompDrvFan_ManualEn = 0;
static u16 MU2CompDrvFan_ManualSpd = 0;
u16 MU2KeepPowerMode = 0;               //保供电模式
u16 MU2ManualCalibration_SOC = 0;       /*手动校准SOC*/
static u16 MU2ResetAFEstatus = 0;      //重置AFE状态
static u16 MU2ResetBSUstatus = 0;     //重置BSU状态
static u16 MU2ResetAFE_BSU_ID = 0;   //重置AFE状态的从控ID
static u16 MU2ResetBSU_ID = 0;      //重置BSU状态的从控ID
u8 MU2ResetAFE_SOC_Flag = 0;            //重置AFE状态时是否校准SOC标志
//#endif

#if((PRJ_PARA_NUM_INFO > 0) && (1 == BMU_PWM_FAN_EN))//非测试工装程序是RS485控制IMLU1调速风扇(由IMLU1控制PWM调速风扇时)
static u16 sFanCtrlDuty[BMU_PWM_FAN_NUM] = {0};/*风扇转速控制PWM占空比[0,100] 1%*/

#if(IMLU_FAN_BACK_TYPE > 0)				/*需要检测风扇告警*/
static u16 sFanAlarmState = 0;			/*风扇告警状态位*/
#endif
#endif

//#if(1 == BMU_IMEU3_COM_EN)			/*使能IMEU2通信交互*/
#if(1 == BMU_EU_SHUTFAN_EN)				/*使能控制百叶风机*/
static u8 gAuxShutUserCmd = 0;			/*辅助设备泄爆阀用户控制命令 0:关闭 1:开启*/
#endif
#if(1 == BMU_EU_FIRELIG_EN)				/*使能控制照明声光报警消防等*/
static u8 gAuxRackLightCmd = 0;			/*电池柜照明用户强制控制命令 0:自动控制 1:手动开启  2:手动关闭*/
static u8 gAuxZHSAForceCmd = 0;			/*声光报警器用户强制控制命令 0:自动控制 1:强制开启 2:强制关闭*/
//static u8 gAuxFireTrigCmd = 0;			/*气溶胶消防用户触发控制命令 0:自动触发 1:手动触发*/
#endif
static u16 gAuxDevErrFlag = 0;			/*辅助设备控制异常标志 bit0:开启泄爆阀异常 bit1:开启风机异常 bit2:空调告警 bit8:关闭泄爆阀异常 bit9:关闭风机异常 bit10:空调通信中断*/
extern s16 gGTempCharInfo_255[];           /*PACK温度极值信息*/
extern u16 PackAerosolInfo_211[];

u8 PcsFanDuty = 0; //PCS散热风扇占空比
u8 PduFanDuty = 0; //PDU散热风扇占空比
static u8 ExpFanDuty = 0; //泄爆阀风扇占空比
static u8 gAirDebugCmd = 0;            /*液冷空调调试命令 0：自动控制 1：强制制热 2：强制制冷 3：自循环 4:待机*/

//by lxr01202
static u32 SelfLoopStartStimer = 0;   // 记录自循环启动时的值
static u32 LastTriggerStimer = 0;     // 记录上次触发自循环的值
static u8 IsSelfLoopRunning = 0;      // 标记自循环是否在运行（0=停止，1=运行）
#define MAX_RUN_SEC  1800    // 30分钟 = 30*60
#define MIN_TRIG_INTERVAL 3600  // 1小时 = 60*60
//#endif
//#if(BMU_LEAKAGE_EN == 1)
/*=================================================================================================
** @Name      : void TacLeakageConnTask(void)
** @Input     : void
** @Output    : void
** @Function  : 漏电流传感器通信任务
** @The notes : 1s,通过RS485通信执行
**===============================================================================================*/
void TacLeakageConnTask(void);
//#endif
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
//#if(1 == BMU_IMEU3_COM_EN)/*使能IMEU3通信交互*/
#if(1 == BMU_EU_SHUTFAN_EN)/*使能控制百叶风机*/
/*=============================================================================================
** @Name      : static void TacShutFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 百叶排风机通风控制策略任务
** @The notes : 1s任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacShutFanCtrlTask(void);
/*=============================================================================================
** @Name      : static void TacShutFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : DE除湿机湿度控制任务
** @The notes : 1s任务,通过CAN通信控制IMEU3执行
**===========================================================================================*/
static void TacHumManageTask(void);
#endif
/*=============================================================================================
** @Name      : static void TacShutFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : PDU扰流风机和模块散热风机控制策略任务
** @The notes : 1s任务,通过CAN通信控制IMEU3执行
**===========================================================================================*/
static void TacPduPcsFanCtrlTask(void);

#if(1 == BMU_EU_FIRELIG_EN)/*使能控制照明/声光报警/消防等*/
/*=============================================================================================
** @Name      : static void TacFireAlarmCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 消防控制器和声光报警控制器控制策略任务
** @The notes : 500ms任务,通过CAN通信控制IMEU2执行
**===========================================================================================*/
static void TacFireAlarmCtrlTask(void);
#endif

//#endif
//#if(BMU_IMLU2_EN == 1)
/*=================================================================================================
** @Name      : static void TacLightStripCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 灯带控制任务
** @The notes : 1s,通过RS485通信控制IMLU2执行
**===============================================================================================*/
static void TacLightStripCtrlTask(void);
//#endif


//#if(BMU_LIQUIDCOOL_AIR_485_EN == 1)
/*=================================================================================================
** @Name      : static void TacLightStripCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 液冷空调控制任务
** @The notes : 1s,通过RS485通信控制液冷空调控制板执行
**===============================================================================================*/
static void TacLiquidCoolAirCtrlTask(void);
//#endif

/*=================================================================================================
** @Name      : void TacLeakageConnTask(void)
** @Input     : void
** @Output    : void
** @Function  : 漏电流传感器通信任务
** @The notes : 1s,通过RS485通信执行
**===============================================================================================*/
void TacLeakageConnTask(void)
{
	static u8 sCtrlStep  = 0;
	//by lxr01202  屏蔽
    //BMS初始化时等待空调信息不做控制
    // if((eWORK_INIT == GetGWorkStateAPI())          //系统开机
    // 	|| (eWORK_OFF == GetGWorkStateAPI()))      //系统关机
	// {
    // 	sCtrlStep = 0;
	// 	return;                 
	// }
    switch(sCtrlStep)
    {
    	case 0:
    	/*查询漏电流和软件版本*/
    	MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_LEAKAGE, 0x03, MB_LEAKAGE_RO_BASE_ADDR, 2, eSCI1);
    	break;
    	default:
    	break;
    }
    return;
}
/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void TacAuxDevManagTask(void)
** @Input     : void
** @Output    : void
** @Function  : 辅助板管理任务
** @The notes : 500ms驱动1s周期
**===========================================================================================*/
void TacAuxDevManagTask(void)
{
	static u8 sState = 0;
	static u8 sState1 = 0;
	static u8 sAvgCurrNum = 0;
	static u8 sCmdSendTimes1 = 0;
	static u8 sCmdSendTimes2 = 0;
	static s32 sSumCurr1min = 0;
	static u8 sQF1ShutTime = 0;

	static u8 sTimer2S = 0;
	//static u8 sTimer2S1 = 0;
	static u8 sTimer2S2 = 0;

	if(gGHardPara_104[eGHardPara104_AirType] != 0)//使能液冷空调
	{
		TacLiquidCoolAirCtrlTask();
	}
	if(gGHardPara_104[eGHardPara104_Imlu2En] == 1)
	{
		sTimer2S++;
		if(sTimer2S >= 2)
		{
			TacLightStripCtrlTask();
			sTimer2S = 0;
		}
	}
	if(gGHardPara_104[eGHardPara104_LeakageType] == 1)
	{

		sTimer2S2++;
		if(sTimer2S2 >= 2)
		{
			sTimer2S2 = 0;
			TacLeakageConnTask();
		}
	}
	/*使能IMEU2通信交互*/
	//#if(1 == BMU_IMEU3_COM_EN)
	/*系统调试模式仅发送百叶手动控制命令*/
    if(eMODE_DEBUG == GetGWorkModeAPI())
    {
    	//间隔5s发送控制命令
    	if(sState >= 20)
    	{
    		/*使能控制百叶风机*/
    		#if(1 == BMU_EU_SHUTFAN_EN)
    		//调试模式发送控制泄爆阀
    		SendCanImeu3MotorCtrlMsg243(gAuxShutUserCmd);
			#endif
    		sState = 11;
    	}
    	//调试模式用特殊值
    	else if(sState <= 10)
    	{
    		sState = 11;
    	}
    	else
    	{
        	/*使能控制照明/声光报警/消防等*/
        	#if(1 == BMU_EU_FIRELIG_EN)
    		//调试模式发送控制IMEU2外设
    		if(13 == sState)
    		{
        		//调试模式发送控制电池柜照明
    			if(1 == gAuxRackLightCmd)
    			{
					 /*A2415 A2445 A2415降本项目*/
					 if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
						|| (gGHardPara_104[eGHardPara104_ProjectCode] == 6)|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
					 {
						SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,2,1);
					 }
					 //A2419 A2425 Y2403项目
					 else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
					   || (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
					 {
						 //暂不处理
					 }
					 //默认A2415项目
					 else
					 {
						 SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,2,1);
					 }
    			}
    			else
    			{
					 /*A2415 A2445 A2415降本项目*/
					 if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1) 
					 || (gGHardPara_104[eGHardPara104_ProjectCode] == 6)|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
					 {
						 SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,2,0);
					 }
					 //A2419 A2425 Y2403项目
					 else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
					 	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
					 {
						 //暂不处理
					 }
					 //默认A2415项目
					 else
					 {
						 SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,2,0);
					 }
    			}
    		}
    		else if(15 == sState)
    		{
        		//调试模式发送控制电池柜声光报警
    			if(1 == gAuxZHSAForceCmd)
    			{
					 /*A2415 A2445 A2415降本项目*/
					 if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
					 	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 6) || (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
					 {
		    			SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,4,1);
					 }
					 //A2419 A2425 Y2403项目
					 else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
					 	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
					 {
						//新增声光报警   电源柜EU3 DO1
						 SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR,1,1);      
					 }
					 //默认A2415项目
					 else
					 {
		    			SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,4,1);
					 }
    			}
    			else
    			{
					 /*A2415 A2445 A2415降本项目*/
					 if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
					 	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 6) || (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
					 {
		    			SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,4,0);
					 }
					 //A2419 A2425项目
					 else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
					 	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
					 {
						 SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR,1,0);
					 }
					 //默认A2415项目
					 else
					 {
		    			SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,4,0);
					 }
    			}
    		}
    		else if(17 == sState)
    		{
				 /*A2415 A2445 A 2415降本项目*/
				 if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
				 	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 6) || (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
				 {
		    		//调试模式发送PCS风扇和PDU风扇调速
		    		SendCanImeu3PwmCtrlMsg241(PcsFanDuty,PduFanDuty);
				 }
				 //A2419 A2425项目
				 else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
				 	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
				 {
					 //暂不处理
				 }
				 //默认A2415项目
				 else
				 {
		    		//调试模式发送PCS风扇和PDU风扇调速
		    		SendCanImeu3PwmCtrlMsg241(PcsFanDuty,PduFanDuty);
				 }
    		}
    		else if(18 == sState)
    		{
				 /*A2415 A2445 A2415降本项目*/
				 if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
				 	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 6) || (gGHardPara_104[eGHardPara104_ProjectCode] == 7)) 
				 {
		    		//调试模式发送排气扇调速
		    		SendCanExhaustCtrlMsg241(ExpFanDuty);
				 }
				 //A2419 A2425项目
				 else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
				 	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
				 {
		 			 /*由第一组BMS控制，第二组不进行控制*/
		 			if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
		 			{
			    		//调试模式发送排气扇调速
			    		SendCanExhaustCtrlMsg241(ExpFanDuty);
		 			}
				 }
				 //默认A2415项目
				 else
				 {
		    		//调试模式发送排气扇调速
		    		SendCanExhaustCtrlMsg241(ExpFanDuty);
				 }
    		}
    		#endif

    		sState++;
    	}
        return;
    }

	//刚从调试模式退出
	if(sState >= 10)
	{
		/*使能控制百叶风机*/
		#if(1 == BMU_EU_SHUTFAN_EN)
		//清除调试IMEU2百叶风机状态
		gAuxShutUserCmd = 0;
		#endif
		//清除平均电流
		sAvgCurr1min = GetGSampOutCurrAPI();
		sSumCurr1min = 0;
		sAvgCurrNum = 0;
		sState = 0;
	}
	//#endif

	/*使能IMLU1或IMEU2通信交互*/
	//#if(1 == BMU_IMEU3_COM_EN)
	/*PCS散热和PDU热管理任务*/
	//使能PDU内IMEU3通信功能
	if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x01) != 0)
	{
		TacPduPcsFanCtrlTask();
		//检测到QF1断开后下高压
		if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
		{
			 if(BitGet(GetPduInImeu3InfoAPI(ePduIn218_DI1_16),eEU218DI1_QF1) == 1)
			 {
				 if(sQF1ShutTime < 255)
				 {
					 sQF1ShutTime++;
				 }
				 if(sQF1ShutTime >= 10)
				 {
					 gGSysCmdCtrl_86[eSysCmdCtrl86_State] = eCTRL_FORCSTOP;
					 RcvGroupWorkStateCmdAPI(eCMDPORT_INTE);
					 sQF1ShutTime = 0;
				 }
			 }
			 else
			 {
				 sQF1ShutTime = 0;
			 }
		}

	}
	/*使能IMEU2通信交互控制照明/声光报警/消防等*/
	#if(BMU_EU_FIRELIG_EN)
	/*控制消防控制器和声光报警器*/
	TacFireAlarmCtrlTask();
	#endif

	/*转换成1s任务*/
    if(0 == sState)
    {
    	if(sState1 == 0)
    	{
        	/*使能IMEU3通信交互*/
    		#if(1 == BMU_EU_SHUTFAN_EN)
    		//使能PDU外IMEU3通信功能
    		if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02) != 0)
    		{
    			//百叶排风机通风管理
    			TacShutFanCtrlTask();
    			//有倾倒检测功能，需要控制IMEU3进行脱扣
    			if((eErr_B == DiagGetErrLastStateAPI(eERR_ID_DUMP_EXP)) && 
					((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 5))
					&& (gGHardPara_104[eGHardPara104_DumpEn] == 1))
    			{

    				if(sCmdSendTimes1 <= 10)
    				{
    					SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR,1,1); //控制QF1断开
    				}
    				if(sCmdSendTimes1 < 255)
    				{
    					sCmdSendTimes1++;
    				}
    				sCmdSendTimes2 = 0;
    			}
    			else
    			{
    				if(sCmdSendTimes2 <= 10)
    				{
        				SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR,1,0); //停止控制QF1断开
    				}
    				if(sCmdSendTimes2 < 255)
    				{
    					sCmdSendTimes2++;
    				}
    				sCmdSendTimes1 = 0;
    			}
    		}
    		#endif

        	sState1 = 1;
    	}
    	else
    	{
        	/*使能IMEU3通信交互*/
    		#if(1 == BMU_EU_SHUTFAN_EN)
    		//使能PDU外IMEU3通信功能
    		if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02)!= 0)
    		{
            	//除湿机湿度管理任务
        		TacHumManageTask();
    		}
    		#endif
        	sState1 = 0;
    	}
    	sState = 1;
    }
    else
    {
		//累计1.5分钟电流
		sSumCurr1min += GetGSampOutCurrAPI();
		sAvgCurrNum++;

		//求1.5分钟平均电流
		if(sAvgCurrNum >= 90)
		{
			sAvgCurr1min = (s16)(sSumCurr1min / sAvgCurrNum);
			sSumCurr1min = 0;
			sAvgCurrNum = 0;
		}
		//充放电直接切换
		else if(((sAvgCurr1min >= 200) && (GetGSampOutCurrAPI() <= -200))
			|| ((sAvgCurr1min <= -200) && (GetGSampOutCurrAPI() >= 200)))
		{
			sAvgCurr1min = GetGSampOutCurrAPI();
			sSumCurr1min = 0;
			sAvgCurrNum = 0;
		}
    	sState = 0;
    }
    //#endif
}
/*=============================================================================================
** @Name      : void TacSetAuxShutUserCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 百叶风机用户控制命令(0:关闭 1:开启)
** @Function  : 设置百叶风机用户控制命令
** @The notes : IMEU3控制
**===========================================================================================*/
void TacSetAuxShutUserCmdAPI(u8 cmd)
{
	//使能PDU外IMEU3通信功能
	if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02)!= 0)
	{
		if(cmd <= 1)
		{
			gAuxShutUserCmd = cmd;
		}
	}
}

/*=============================================================================================
** @Name      : void TacSetAuxRackLightCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 电池柜照明用户控制命令( 0:自动控制 1:手动开启   2:手动关闭)
** @Function  : 设置电池柜照明用户强制控制命令
** @The notes : IMEU3控制
**===========================================================================================*/
void TacSetAuxRackLightCmdAPI(u8 cmd)
{
	//使能PDU外IMEU3通信功能
	if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02) != 0)
	{
		if(cmd <= 2)
		{
			gAuxRackLightCmd = cmd;
		}
	}
}

/*=============================================================================================
** @Name      : void TacSetAuxZHSAForceCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 声光报警器用户控制命令( 0:自动控制 1:强制开启 2:强制关闭)
** @Function  : 设置声光报警器用户强制控制命令
** @The notes : IMEU2控制
**===========================================================================================*/
void TacSetAuxZHSAForceCmdAPI(u8 cmd)
{
	//使能PDU外IMEU3通信功能
	if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02)!= 0)
	{
		if(cmd <= 2)
		{
			gAuxZHSAForceCmd = cmd;
		}
	}
}

/*=============================================================================================
** @Name      : void TacSetAirDebugCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 空调调试用户控制命令( 0:自动控制 1:强制制热  2:强制制冷 3：自循环  4：待机)
** @Function  : 设置空调用户强制控制命令
** @The notes : IMEU2控制
**===========================================================================================*/
void TacSetAirDebugCmdAPI(u8 cmd)
{
	//使能空调通信功能
	if(gGHardPara_104[eGHardPara104_AirType] != 0)
	{
		if(cmd <= 4)
		{
			gAirDebugCmd = cmd;
		}
	}
}



/*=============================================================================================
** @Name      : void TacSetPwmFanUserCmdAPI(u8 id, u8 duty)
** @Input     : id:[0:2]:风扇ID, 0XFF:所有风扇   duty:控制转速占空比[0,100]1%
** @Output    : 设置调速风扇用户控制命令
** @Function  : 设置声光报警器用户强制控制命令
** @The notes : 第一路：PDU风扇 第二路：PCS散热风扇  第三路：泄爆阀排气扇
**===========================================================================================*/
void TacSetPwmFanUserCmdAPI(u8 id, u8 duty)
{
	//使能PDU外IMEU3通信功能
	if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02)!= 0)
	{
		if(id == 2)
		{
			ExpFanDuty = duty;
		}
		else if(id == 0xff)
		{
			ExpFanDuty = duty;
		}

	}
	//使能PDU内IMEU3通信功能
	if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x01)!= 0)
	{
		if(id == 0)
		{
			PcsFanDuty = duty;
		}
		else if(id == 1)
		{
			PduFanDuty = duty;
		}
		else if(id == 0xff)
		{
			PcsFanDuty = duty;
			PduFanDuty = duty;
		}
	}
}

/*=============================================================================================
** @Name      : void TacSetAuxFireTrigCmdAPI(u8 cmd)
** @Input     : void
** @Output    : 气溶胶消防器用户触发命令( 0:自动触发 1:手动触发)
** @Function  : 设置气溶胶消防器用户触发命令
** @The notes : IMEU2控制
**===========================================================================================*/
void TacSetAuxFireTrigCmdAPI(u8 cmd)
{
	/*使能IMEU2通信交互控制百叶风机*/
	//#if((1 == BMU_IMEU_COM_EN) && (1 == BMU_EU_FIRELIG_EN))
	if(cmd <= 1)
	{
		//gAuxFireTrigCmd = cmd;
	}
	//#endif
}
/*=============================================================================================
** @Name      : u16 TacGetAuxDevErrFlagAPI(void)
** @Input     : void
** @Output    : 辅助设备异常标志(bit0:开启百叶异常 bit1:开启风机异常 bit2:空调告警 bit8:关闭百叶异常 bit9:关闭风机异常 bit10:空调通信中断)
** @Function  : 获取辅助设备异常标志
** @The notes : IMEU2控制设备
**===========================================================================================*/
u16 TacGetAuxDevErrFlagAPI(void)
{
	//使能PDU外IMEU3通信功能
	if((gGHardPara_104[eGHardPara104_Imeu3En] & 0x02)!= 0)
	{
		return(gAuxDevErrFlag);
	}
	else
	{
		return 0;
	}
}

/*=============================================================================================
** @Name      : u8 TacGetAuxComErrFlagAPI(void)
** @Input     : void
** @Output    : 辅助设备通信异常标志 bit0:空调 bit1:温湿度传感器1 bit2:温湿度传感器1 bit3:温湿度传感器 bit4:气体传感器1 bit5:气体传感器2 bit6:气体传感器3
** @Function  : 获取辅助设备通讯异常标志
** @The notes : IMEU2控制设备
**===========================================================================================*/
u8 TacGetAuxComErrFlagAPI(void)
{
	/*使能IMEU2控制*/
	#if(1 == BMU_IMEU_COM_EN)
	return(gAuxComErrFlag);
	#else
	return(0);
	#endif
}

/*=============================================================================================
** @Name      : u8 TacGetSpdErrFlagAPI(void)
** @Input     : void
** @Output    : SPD告警标志 0：正常 1：告警
** @Function  : 获取辅助设备SPD告警标志
** @The notes : IMEU2控制设备
**===========================================================================================*/
u8 TacGetSpdErrFlagAPI(void)
{
	/*使能IMEU2控制*/
	#if(1 == BMU_IMEU_COM_EN)
	if(1 == BitGet(GetGPerImeu2SignInfoAPI(eIMEU2Info204_SenState), eEU204State_SPD))
		gSpdErrFlag = 1;
	else 
		gSpdErrFlag = 0;
	return(gSpdErrFlag);
	#else
	return(0);
	#endif
}
//#if(1 == BMU_IMEU3_COM_EN)/*使能IMEU3通信交互*/
#if(1 == BMU_EU_SHUTFAN_EN)/*使能控制百叶风机*/
/*=============================================================================================
** @Name      : static void TacShutFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 泄爆阀排风机通风控制策略任务
** @The notes : 1s任务,通过CAN通信控制IMEU3执行
**===========================================================================================*/
static void TacShutFanCtrlTask(void)
{
    u8 fanSate = 0;
    static u8 sFanSate = 0;
    static u8 sCtrlStep = 0;
    static u8 sCtrlTimes = 0;
    static u8 sVentTime = 0;
    static u8 sHisDate = 0;
    switch(sCtrlStep)
    {
    	//发送控制泄爆阀报文
		case 0:
			sVentTime++;
			SendCanImeu3MotorCtrlMsg243(sFanSate);
			//上电前3s连续发送
			if(BSPGetOSMillSecTimeAPI() >= 3100)
			{
				sCtrlStep = 1;
			}
			break;

		//判断控制是否完成
		case 1:
			sVentTime++;
			//PDU外IMEU3通信正常时判断控制结果
			if(0 == DiagGetPduOutImeu3OffCommuStateAPI())
			{
		 		/*A2415 A2445 A2516 A2415降本项目*/
		 		if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
					 || (gGHardPara_104[eGHardPara104_ProjectCode] == 5) || (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
					 || (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
		 		{
					//泄爆阀打开状态回检信号为低电平或者泄爆阀关闭状态回检信号为高电平
						if(((sFanSate == 1) && (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI18_Explosion1)))
						 || ((sFanSate == 0) && (1 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI18_Explosion1)))
						 || (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI19_Fan)))
						{
							if(sCtrlTimes >= 15)
							{
								//泄爆阀开启异常
								if((1 == sFanSate) && (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI18_Explosion1)))
								{
									BitSet(gAuxDevErrFlag, eAuxErr_ShutOn);
								}
								else
								{
									BitClr(gAuxDevErrFlag, eAuxErr_ShutOn);
								}
								//泄爆阀关闭异常
								if((0 == sFanSate) && (1 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI18_Explosion1)))
								{
									BitSet(gAuxDevErrFlag, eAuxErr_ShutOff);
								}
								else
								{
									BitClr(gAuxDevErrFlag, eAuxErr_ShutOff);
								}
								//排气扇告警
								if(0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI19_Fan))
								{
									BitSet(gAuxDevErrFlag, eAuxErr_Fan);
								}
								else
								{
									BitClr(gAuxDevErrFlag, eAuxErr_Fan);
								}
								sCtrlStep = 0;
								sCtrlTimes = 0;
							}
							//需要等待
							else
							{
								sCtrlTimes++;
								//返回再次控制
								sCtrlStep = 0;
							}
						}
						else
						{
							//消除泄爆阀和排气扇异常标志
							BitClr(gAuxDevErrFlag, eAuxErr_ShutOn);
							BitClr(gAuxDevErrFlag, eAuxErr_Fan);
							BitClr(gAuxDevErrFlag, eAuxErr_ShutOff);
							sCtrlTimes = 0;
							//下一步
							sCtrlStep = 2;
						}
		 		}
		 		/*A2419 A2425项目*/
		 		else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3) 
					|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
		 		{
					if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
					{
						//泄爆阀打开状态回检信号为低电平或者泄爆阀关闭状态回检信号为高电平
							if(((sFanSate == 1) && (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI18_Explosion1)))
							 || ((sFanSate == 0) && (1 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI18_Explosion1)))
							 || (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI19_Fan)))
							{
								if(sCtrlTimes >= 15)
								{
									//泄爆阀开启异常
									if((1 == sFanSate) && (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI18_Explosion1)))
									{
										BitSet(gAuxDevErrFlag, eAuxErr_ShutOn);
									}
									else
									{
										BitClr(gAuxDevErrFlag, eAuxErr_ShutOn);
									}
									//泄爆阀关闭异常
									if((0 == sFanSate) && (1 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI18_Explosion1)))
									{
										BitSet(gAuxDevErrFlag, eAuxErr_ShutOff);
									}
									else
									{
										BitClr(gAuxDevErrFlag, eAuxErr_ShutOff);
									}
									//排气扇告警
									if(0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI19_Fan))
									{
										BitSet(gAuxDevErrFlag, eAuxErr_Fan);
									}
									else
									{
										BitClr(gAuxDevErrFlag, eAuxErr_Fan);
									}
									sCtrlStep = 0;
									sCtrlTimes = 0;
								}
								//需要等待
								else
								{
									sCtrlTimes++;
									//返回再次控制
									sCtrlStep = 0;
								}
							}
							else
							{
								//消除泄爆阀和排气扇异常标志
								BitClr(gAuxDevErrFlag, eAuxErr_ShutOn);
								BitClr(gAuxDevErrFlag, eAuxErr_Fan);
								BitClr(gAuxDevErrFlag, eAuxErr_ShutOff);
								sCtrlTimes = 0;
								//下一步
								sCtrlStep = 2;
							}
					}
					else if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 51)
					{
						//泄爆阀打开状态回检信号为低电平或者泄爆阀关闭状态回检信号为高电平
						if(((sFanSate == 1) && (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI17_Explosion2)))
						 || ((sFanSate == 0) && (1 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI17_Explosion2))))
						{
							if(sCtrlTimes >= 15)
							{
								//泄爆阀开启异常
								if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 51)
								{
									if((1 == sFanSate) && (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI17_Explosion2)))
									{
										BitSet(gAuxDevErrFlag, eAuxErr_ShutOn);
									}
									else
									{
										BitClr(gAuxDevErrFlag, eAuxErr_ShutOn);
									}
									//泄爆阀关闭异常
									if((0 == sFanSate) && (1 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI17_Explosion2)))
									{
										BitSet(gAuxDevErrFlag, eAuxErr_ShutOff);
									}
									else
									{
										BitClr(gAuxDevErrFlag, eAuxErr_ShutOff);
									}
									sCtrlStep = 0;
									sCtrlTimes = 0;
								}
							}
							//需要等待
							else
							{
								sCtrlTimes++;
								//返回再次控制
								sCtrlStep = 0;
							}
						}
						else
						{
							//消除泄爆阀和排气扇异常标志
							BitClr(gAuxDevErrFlag, eAuxErr_ShutOn);
							BitClr(gAuxDevErrFlag, eAuxErr_Fan);
							BitClr(gAuxDevErrFlag, eAuxErr_ShutOff);
							sCtrlTimes = 0;
							//下一步
							sCtrlStep = 2;
						}
					}
		 		}
		 		//默认A2415项目
		 		else
		 		{
					//泄爆阀打开状态回检信号为低电平或者泄爆阀关闭状态回检信号为高电平
						if(((sFanSate == 1) && (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI18_Explosion1)))
						 || ((sFanSate == 0) && (1 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI18_Explosion1)))
						 || (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI19_Fan)))
						{
							if(sCtrlTimes >= 15)
							{
								//泄爆阀开启异常
								if((1 == sFanSate) && (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI18_Explosion1)))
								{
									BitSet(gAuxDevErrFlag, eAuxErr_ShutOn);
								}
								else
								{
									BitClr(gAuxDevErrFlag, eAuxErr_ShutOn);
								}
								//泄爆阀关闭异常
								if((0 == sFanSate) && (1 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI18_Explosion1)))
								{
									BitSet(gAuxDevErrFlag, eAuxErr_ShutOff);
								}
								else
								{
									BitClr(gAuxDevErrFlag, eAuxErr_ShutOff);
								}
								//排气扇告警
								if(0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8),eEU219DI19_Fan))
								{
									BitSet(gAuxDevErrFlag, eAuxErr_Fan);
								}
								else
								{
									BitClr(gAuxDevErrFlag, eAuxErr_Fan);
								}
								sCtrlStep = 0;
								sCtrlTimes = 0;
							}
							//需要等待
							else
							{
								sCtrlTimes++;
								//返回再次控制
								sCtrlStep = 0;
							}
						}
						else
						{
							//消除泄爆阀和排气扇异常标志
							BitClr(gAuxDevErrFlag, eAuxErr_ShutOn);
							BitClr(gAuxDevErrFlag, eAuxErr_Fan);
							BitClr(gAuxDevErrFlag, eAuxErr_ShutOff);
							sCtrlTimes = 0;
							//下一步
							sCtrlStep = 2;
						}
		 		}
			}
			//IMEU3通信中断则直接控制
			else
			{
				//下一步
				sCtrlTimes = 0;
				sCtrlStep = 2;
			}
			break;
		//百叶控制状态变化必须的时间间隔
		case 2:
			sVentTime++;

	 		/*A2415 A2445 A2516 A2415降本项目*/
	 		if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
				|| (gGHardPara_104[eGHardPara104_ProjectCode] == 5) || (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
				|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
	 		{
				//间隔5s或手动控制百叶开启或关闭
				if((sCtrlTimes >= IMEU2_SHUTFAN_CTRL_T) || ((1 == gAuxShutUserCmd)
					&& (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI18_Explosion1))))	//需要开启但仍处于关闭到位
				{
					//下一步
					sCtrlTimes = 0;
					sCtrlStep = 3;
				}
				else
				{
					sCtrlTimes++;
					//至少5s需要1帧报文
					if(sCtrlTimes == 0)
					{
						SendCanImeu3MotorCtrlMsg243(sFanSate);
					}
					else if(sCtrlTimes == 2)
					{
				 		/*调试模式直接控制*/
				 		if(eMODE_DEBUG == GetGWorkModeAPI())
				 		{
				 			SendCanExhaustCtrlMsg241(ExpFanDuty);
				 		}
				 		else
				 		{
							if(sFanSate == 0)
							{
								ExpFanDuty = 0;
								SendCanExhaustCtrlMsg241(ExpFanDuty);
							}
							else
							{
								ExpFanDuty  = 50;
								SendCanExhaustCtrlMsg241(ExpFanDuty);
							}
				 		}
					}
				}
	 		}
	 		else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
				|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
	 		{
				//间隔5s或手动控制百叶开启或关闭
				if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
				{
					//间隔5s或手动控制百叶开启或关闭
					if((sCtrlTimes >= IMEU2_SHUTFAN_CTRL_T) || ((1 == gAuxShutUserCmd)
						&& (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI18_Explosion1))))	//需要开启但仍处于关闭到位
					{
						//下一步
						sCtrlTimes = 0;
						sCtrlStep = 3;
					}
					else
					{
						sCtrlTimes++;
						//至少5s需要1帧报文
						if(sCtrlTimes == 0)
						{
							SendCanImeu3MotorCtrlMsg243(sFanSate);
						}
						else if(sCtrlTimes == 2)
						{
					 		/*调试模式直接控制*/
					 		if(eMODE_DEBUG == GetGWorkModeAPI())
					 		{
					 			SendCanExhaustCtrlMsg241(ExpFanDuty);
					 		}
					 		else
					 		{
								if(sFanSate == 0)
								{
									ExpFanDuty = 0;
									SendCanExhaustCtrlMsg241(ExpFanDuty);
								}
								else
								{
									ExpFanDuty  = 50;
									SendCanExhaustCtrlMsg241(ExpFanDuty);
								}
					 		}
						}
					}
				}
				else if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 51)
				{
					//间隔5s或手动控制百叶开启或关闭
					if((sCtrlTimes >= IMEU2_SHUTFAN_CTRL_T) || ((1 == gAuxShutUserCmd)
						&& (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI17_Explosion2))))	//需要开启但仍处于关闭到位
					{
						//下一步
						sCtrlTimes = 0;
						sCtrlStep = 3;
					}
					else
					{
						sCtrlTimes++;
						//至少5s需要1帧报文
						if(sCtrlTimes == 0)
						{
							SendCanImeu3MotorCtrlMsg243(sFanSate);
						}
						else if(sCtrlTimes == 2)
						{
					 		/*调试模式直接控制*/
					 		if(eMODE_DEBUG == GetGWorkModeAPI())
					 		{
					 			SendCanExhaustCtrlMsg241(ExpFanDuty);
					 		}
					 		else
					 		{
								if(sFanSate == 0)
								{
									ExpFanDuty = 0;
									if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
									{
										SendCanExhaustCtrlMsg241(ExpFanDuty);
									}
									else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
										|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
									{
										if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
										{
											SendCanExhaustCtrlMsg241(ExpFanDuty);
										}
									}
									else
									{
										SendCanExhaustCtrlMsg241(ExpFanDuty);
									}
								}
								else
								{
									ExpFanDuty  = 50;
									if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
									{
										SendCanExhaustCtrlMsg241(ExpFanDuty);
									}
									else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
										|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
									{
										if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
										{
											SendCanExhaustCtrlMsg241(ExpFanDuty);
										}
									}
									else
									{
										SendCanExhaustCtrlMsg241(ExpFanDuty);
									}
								}
					 		}
						}
					}
				}
	 		}
	 		//默认A2415项目
	 		else
	 		{
				//间隔5s或手动控制百叶开启或关闭
				if((sCtrlTimes >= IMEU2_SHUTFAN_CTRL_T) || ((1 == gAuxShutUserCmd)
					&& (0 == BitGet(GetPduOutImeu3InfoAPI(ePduOut219_DI17_22_DO1_8), eEU219DI18_Explosion1))))	//需要开启但仍处于关闭到位
				{
					//下一步
					sCtrlTimes = 0;
					sCtrlStep = 3;
				}
				else
				{
					sCtrlTimes++;
					//至少5s需要1帧报文
					if(sCtrlTimes == 0)
					{
						SendCanImeu3MotorCtrlMsg243(sFanSate);
					}
					else if(sCtrlTimes == 2)
					{
				 		/*调试模式直接控制*/
				 		if(eMODE_DEBUG == GetGWorkModeAPI())
				 		{
				 			SendCanExhaustCtrlMsg241(ExpFanDuty);
				 		}
				 		else
				 		{
							if(sFanSate == 0)
							{
								ExpFanDuty = 0;
								SendCanExhaustCtrlMsg241(ExpFanDuty);
							}
							else
							{
								ExpFanDuty  = 50;
								SendCanExhaustCtrlMsg241(ExpFanDuty);
							}
				 		}
					}
				}
	 		}
			break;

		//判断泄爆阀排气扇需求状态
		case 3:
			/*优先执行手动控制开启*/
			if(1 == gAuxShutUserCmd)
			{
				//开启百叶风机
				fanSate = 1;
				sVentTime = gGHardPara_104[eGHardPara104_ExplosTime];//用于允许手动及时关闭(无需连续通风1分钟才允许关闭)

				//日期变换消除控制命令(防止忘记关闭百叶)
				if(sHisDate != GetSysDateClockAPI().day)
				{
					gAuxShutUserCmd = 0;
					sHisDate = GetSysDateClockAPI().day;
				}
			}
			/*有3级消防告警，打开泄爆阀*/
			else if(GetFireLevelInfoAPI(eFireInfo254_Level3) == 1)
			{
				//开启百叶风机
				fanSate = 1;
				sVentTime = 0;
				sHisDate = GetSysDateClockAPI().day;
			}
			else
			{
				//关闭百叶风机
				fanSate = 0;
				sVentTime = 0;
			}


			//控制状态变化
			if(fanSate != sFanSate)
			{
				/*产生事件*/
				//EventRcdDisplayUserAPI(eEVN_RCDSORT_USER100, eEvn_User_ShutPwr, GetGSampOutCurrAPI(), fanSate, (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenT1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsT)), (((u16)GetGPerImeu2TempInfoAPI(eIMEU2Info205_THSenH1) << 8) | GetGPerImeu2TempInfoAPI(eIMEU2Info205_AirInsH)));
				sFanSate = fanSate;
			}
			sCtrlStep = 0;
			break;

		default:
			sCtrlStep = 0;
			break;
    }
}
#endif

#if(1 == BMU_EU_FIRELIG_EN)
/*=============================================================================================
** @Name      : static void TacFireAlarmCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 消防控制器和声光报警控制器控制策略任务
** @The notes : 500ms任务,通过CAN通信控制IMEU3执行
**===========================================================================================*/
static void TacFireAlarmCtrlTask(void)
{
    static u8 sTime = 0;
    static u8 sCtrlStep = 0;
	static u8 FirstPowerOnFlag = 0;
	static u8 sFireLevel1Pos = 0;//一级消防所在PACK位置
	static u8 sFireLevel2Pos = 0;//二级消防所在PACK位置

	static u8 sPack1HighTPos = 0;  //PACK1高温电芯位置
	static s16 sPack1HTRise = 0;   //PACK1高温电芯温升
	static s16 sPack1NewT = 0;
	static u8 sPack1Time5s = 0;
	static u8 sPack1TRiseTime5s = 200;

	static u8 sPack2HighTPos = 0;  //PACK2高温电芯位置
	static s16 sPack2HTRise = 0;   //PACK2高温电芯温升
	static s16 sPack2NewT = 0;
	static u8 sPack2Time5s = 0;
	static u8 sPack2TRiseTime5s = 200;

	static u8 sPack3HighTPos = 0;  //PACK3高温电芯位置
	static s16 sPack3HTRise = 0;   //PACK3高温电芯温升
	static s16 sPack3NewT = 0;
	static u8 sPack3Time5s = 0;
	static u8 sPack3TRiseTime5s = 200;

	static u8 sPack4HighTPos = 0;  //PACK4高温电芯位置
	static s16 sPack4HTRise = 0;   //PACK4高温电芯温升
	static s16 sPack4NewT = 0;
	static u8 sPack4Time5s = 0;
	static u8 sPack4TRiseTime5s = 200;

	static u8 sPack5HighTPos = 0;  //PACK5高温电芯位置
	static s16 sPack5HTRise = 0;   //PACK5高温电芯温升
	static s16 sPack5NewT = 0;
	static u8 sPack5Time5s = 0;
	static u8 sPack5TRiseTime5s = 200;

	//消防2级判断
	static s16 sHighTempRise = 0;
	static s16 sNewTemp = 0;
	static u8 sFire2LevTime5s = 0;
	static u8 sRiseTime5s = 0;
	static u8 sFire2TRiseTime5s = 200;
	static s16 sHighTempRise1 = 0;  //最大电芯相邻位置1温升
	static s16 sNewTemp1 = 0;       //最大电芯相邻位置1温度
	static s16 sHighTempRise2 = 0;  //最大电芯相邻位置2温升
	static s16 sNewTemp2 = 0;       //最大电芯相邻位置2温度
	static u8 sFire2TRiseTime5s1 = 200;//最大电芯相邻位置温升
	static u8 sTRiseTime10s = 0;     //温升升持续时间
	static u8 sPpmAlarmTime = 0;     //CO或者气溶胶告警时间
	static u8 PerfluState = 0;       //全氟己酮喷射状态
	static u8 PunctureTime = 0;      //刺破阀动作喷射时间

	//消防3级判断
	static u8 sGasAlarmTime = 0; //气体告警时间

	//消防4级判断
	static u8 sFire4TTimse10s = 0;
	static u8 sFire4TTimse5s1 = 0;
	static u8 sFire4TTimse5s2 = 0;
	static u8 sFire4TTimse5s3 = 0;
	static u8 sFire4TTimse5s4 = 0;
	static u8 sDcPowerFlag = 0;




    //上电前3s不检测
    if((BSPGetOSMillSecTimeAPI() <= 3000) && (FirstPowerOnFlag == 0))
    {
    	return;
    }
	FirstPowerOnFlag = 1;
    sTime++;
    //判断PACK1是否有消防1级告警
	if(gGTempCharInfo_255[eTempInfo255_Pack1MaxTemp] > 800)
	{
		sPack1Time5s++;
		if(sPack1TRiseTime5s == 200)
		{
			sPack1NewT = gGTempCharInfo_255[eTempInfo255_Pack1MaxTemp];//记录该温度
			sPack1HighTPos = gGTempCharInfo_255[eTempInfo255_Pack1MaxTNum];//记录该位置
			sPack1TRiseTime5s = 0;
			sPack1HTRise = 0;
		}
		sPack1TRiseTime5s++;
		/*记录同一位置5s内温升*/
		if((sPack1TRiseTime5s >= 10) && (sPack1HighTPos == gGTempCharInfo_255[eTempInfo255_Pack1MaxTNum]))
		{
			if(sPack1HighTPos >= 1)
			{
				sPack1HTRise = GetGCellTempAPI(sPack1HighTPos - 1) - sPack1NewT;
				sPack1NewT = GetGCellTempAPI(sPack1HighTPos - 1);
			}
			sPack1TRiseTime5s = 0;
		}
		//最大电芯温度≥80度且持续5s且电芯温升≥3℃/5s
		if((sPack1Time5s >= 10) && (sPack1HTRise >= 30))
		{
			sPack1Time5s = 0;
			SetFireLevelInfoAPI(eFireInfo254_Level1,1); //触发消防1级告警，不允许自恢复，可远程恢复
			sFireLevel1Pos = 1;
		}
	}
	else
	{
		sPack1Time5s = 0;
		sPack1TRiseTime5s = 200;
		sPack1HTRise = 0;
	}

    //判断PACK2是否有消防1级告警
	if(gGTempCharInfo_255[eTempInfo255_Pack2MaxTemp] > 800)
	{
		sPack2Time5s++;
		if(sPack2TRiseTime5s == 200)
		{
			sPack2NewT = gGTempCharInfo_255[eTempInfo255_Pack2MaxTemp];//记录该温度
			sPack2HighTPos = gGTempCharInfo_255[eTempInfo255_Pack2MaxTNum];//记录该位置
			sPack2TRiseTime5s = 0;
			sPack2HTRise = 0;
		}
		sPack2TRiseTime5s++;
		/*记录同一位置5s内温升*/
		if((sPack2TRiseTime5s >= 10) && (sPack2HighTPos == gGTempCharInfo_255[eTempInfo255_Pack2MaxTNum]))
		{
			if(sPack2HighTPos >= 1)
			{
				sPack2HTRise = GetGCellTempAPI(sPack2HighTPos + 48 - 1) - sPack2NewT;
				sPack2NewT = GetGCellTempAPI(sPack2HighTPos + 48 - 1);
			}
			sPack2TRiseTime5s = 0;
		}
		//最大电芯温度≥80度且持续5s且电芯温升≥3℃/5s
		if((sPack2Time5s >= 10) && (sPack2HTRise >= 30))
		{
			sPack2Time5s = 0;
			SetFireLevelInfoAPI(eFireInfo254_Level1,1); //触发消防1级告警，不允许自恢复，可远程恢复
			sFireLevel1Pos = 2;
		}
	}
	else
	{
		sPack2Time5s = 0;
		sPack2TRiseTime5s = 200;
		sPack2HTRise = 0;
	}

    //判断PACK3是否有消防1级告警
	if(gGTempCharInfo_255[eTempInfo255_Pack3MaxTemp] > 800)
	{
		sPack3Time5s++;
		if(sPack3TRiseTime5s == 200)
		{
			sPack3NewT = gGTempCharInfo_255[eTempInfo255_Pack3MaxTemp];//记录该温度
			sPack3HighTPos = gGTempCharInfo_255[eTempInfo255_Pack3MaxTNum];//记录该位置
			sPack3TRiseTime5s = 0;
			sPack3HTRise = 0;
		}
		sPack3TRiseTime5s++;
		/*记录同一位置5s内温升*/
		if((sPack3TRiseTime5s >= 10) && (sPack3HighTPos == gGTempCharInfo_255[eTempInfo255_Pack3MaxTNum]))
		{
			if(sPack3HighTPos >= 1)
			{
				sPack3HTRise = GetGCellTempAPI(sPack3HighTPos + 48 + 48 - 1) - sPack3NewT;
				sPack3NewT = GetGCellTempAPI(sPack3HighTPos + 48 + 48 - 1);
			}
			sPack3TRiseTime5s = 0;
		}
		//最大电芯温度≥80度且持续5s且电芯温升≥3℃/5s
		if((sPack3Time5s >= 10) && (sPack3HTRise >= 30))
		{
			sPack3Time5s = 0;
			SetFireLevelInfoAPI(eFireInfo254_Level1,1); //触发消防1级告警，不允许自恢复，可远程恢复
			sFireLevel1Pos = 3;
		}
	}
	else
	{
		sPack3Time5s = 0;
		sPack3TRiseTime5s = 200;
		sPack3HTRise = 0;
	}

    //判断PACK4是否有消防1级告警
	if(gGTempCharInfo_255[eTempInfo255_Pack4MaxTemp] > 800)
	{
		sPack4Time5s++;
		if(sPack4TRiseTime5s == 200)
		{
			sPack4NewT = gGTempCharInfo_255[eTempInfo255_Pack4MaxTemp];//记录该温度
			sPack4HighTPos = gGTempCharInfo_255[eTempInfo255_Pack4MaxTNum];//记录该位置
			sPack4TRiseTime5s = 0;
			sPack4HTRise = 0;
		}
		sPack4TRiseTime5s++;
		/*记录同一位置5s内温升*/
		if((sPack4TRiseTime5s >= 10) && (sPack4HighTPos == gGTempCharInfo_255[eTempInfo255_Pack4MaxTNum]))
		{
			if(sPack4HighTPos >= 1)
			{
				sPack4HTRise = GetGCellTempAPI(sPack4HighTPos + 48 + 48 + 48 - 1) - sPack4NewT;
				sPack4NewT = GetGCellTempAPI(sPack4HighTPos + 48 + 48 + 48 - 1);
			}
			sPack4TRiseTime5s = 0;
		}
		//最大电芯温度≥80度且持续5s且电芯温升≥3℃/5s
		if((sPack4Time5s >= 10) && (sPack4HTRise >= 30))
		{
			sPack4Time5s = 0;
			SetFireLevelInfoAPI(eFireInfo254_Level1,1); //触发消防1级告警，不允许自恢复，可远程恢复
			sFireLevel1Pos = 4;
		}
	}
	else
	{
		sPack4Time5s = 0;
		sPack4TRiseTime5s = 200;
		sPack4HTRise = 0;
	}

    //判断PACK5是否有消防1级告警
	if(gGTempCharInfo_255[eTempInfo255_Pack5MaxTemp] > 800)
	{
		sPack5Time5s++;
		if(sPack5TRiseTime5s == 200)
		{
			sPack5NewT = gGTempCharInfo_255[eTempInfo255_Pack5MaxTemp];//记录该温度
			sPack5HighTPos = gGTempCharInfo_255[eTempInfo255_Pack5MaxTNum];//记录该位置
			sPack5TRiseTime5s = 0;
			sPack5HTRise = 0;
		}
		sPack5TRiseTime5s++;
		/*记录同一位置5s内温升*/
		if((sPack5TRiseTime5s >= 10) && (sPack5HighTPos == gGTempCharInfo_255[eTempInfo255_Pack5MaxTNum]))
		{
			if(sPack5HighTPos >= 1)
			{
				sPack5HTRise = GetGCellTempAPI(sPack5HighTPos + 48 + 48 + 48 + 48 - 1) - sPack5NewT;
				sPack5NewT = GetGCellTempAPI(sPack5HighTPos + 48 + 48 + 48 +48 - 1);
			}
			sPack5TRiseTime5s = 0;
		}
		//最大电芯温度≥80度且持续5s且电芯温升≥3℃/5s
		if((sPack5Time5s >= 10) && (sPack5HTRise >= 30))
		{
			sPack5Time5s = 0;
			SetFireLevelInfoAPI(eFireInfo254_Level1,1); //触发消防1级告警，不允许自恢复，可远程恢复
			sFireLevel1Pos = 5;
		}
	}
	else
	{
		sPack5Time5s = 0;
		sPack5TRiseTime5s = 200;
		sPack5HTRise = 0;
	}

	//消防2级判断
	if(GetFireLevelInfoAPI(eFireInfo254_Level1) == 1)
	{
		//PACK1判断
		if(sFireLevel1Pos == 1)
		{
			//根据最大电芯温度和温升判断是否有2级消防告警
			if(GetGCellTempAPI(sPack1HighTPos - 1) >= 900)
			{
					sFire2LevTime5s++;
					if(sFire2TRiseTime5s == 200)
					{
						if(sPack1HighTPos >= 1)
						{
							sNewTemp = GetGCellTempAPI(sPack1HighTPos - 1);//记录该温度
						}
						sFire2LevTime5s = 0;
						sHighTempRise = 0;
						sFire2TRiseTime5s = 0;
					}
					sFire2TRiseTime5s++;
					if(sFire2TRiseTime5s >= 10)
					{
						if(sPack1HighTPos >= 1)
						{
							sHighTempRise = GetGCellTempAPI(sPack1HighTPos - 1) - sNewTemp;
							sNewTemp = GetGCellTempAPI(sPack1HighTPos - 1);
						}
						sFire2TRiseTime5s = 0;
					}
					if((sFire2LevTime5s >= 10) && (sHighTempRise >= 30))
					{
						sRiseTime5s++;
						if(sRiseTime5s >= 5)
						{
							SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
							sFireLevel2Pos = 1;
							sRiseTime5s = 0;
						}
					}
					else
					{
						sRiseTime5s = 0;
					}
			}
			else
			{
				sFire2TRiseTime5s = 200;
				sHighTempRise = 0;
				sFire2LevTime5s = 0;
				sRiseTime5s = 0;
			}
			//根据相邻位置的温升或者温度和气溶胶告警判断是否触发2级消防告警
			//特殊位置，只需记录下一个电芯温升或者温度
			if((sPack1HighTPos == 1) || (sPack1HighTPos == 13) || (sPack1HighTPos == 25) || (sPack1HighTPos == 37))
			{
				//根据相邻电芯温度和PACK内气溶胶浓度判断
				if((PackAerosolInfo_211[eAerosolInfo211_1AerCon] >= 600) && (GetGCellTempAPI(sPack1HighTPos) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 1;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				//根据相邻位置电芯温升
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack1HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack1HighTPos);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				//5s判断一次温升
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack1HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack1HighTPos) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack1HighTPos);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 1;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//特殊位置，只需记录上一个电芯温升或者温度
			else if((sPack1HighTPos == 12) || (sPack1HighTPos == 24) || (sPack1HighTPos == 36) || (sPack1HighTPos == 48))
			{
				if((PackAerosolInfo_211[eAerosolInfo211_1AerCon] >= 600) && (GetGCellTempAPI(sPack1HighTPos - 2) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 1;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}

				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack1HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack1HighTPos - 2);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack1HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack1HighTPos - 2) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack1HighTPos - 2);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 1;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//正常位置，需要记录前后两个电芯的温升或者温度
			else
			{
				if((PackAerosolInfo_211[eAerosolInfo211_1AerCon] >= 600) && (GetGCellTempAPI(sPack1HighTPos - 2) >= 800) && (GetGCellTempAPI(sPack1HighTPos) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 1;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}

				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack1HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack1HighTPos - 2);//记录该温度
						sNewTemp2 = GetGCellTempAPI(sPack1HighTPos);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack1HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack1HighTPos - 2) - sNewTemp1;
						sHighTempRise2 = GetGCellTempAPI(sPack1HighTPos) - sNewTemp2;
						sNewTemp1 = GetGCellTempAPI(sPack1HighTPos - 2);
						sNewTemp2 = GetGCellTempAPI(sPack1HighTPos);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if((sHighTempRise1 >= 30) && (sHighTempRise2 >= 30))
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 1;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
		}
		//PACK2位置判断
		else if(sFireLevel1Pos == 2)
		{
			//根据最大电芯温度和温升判断是否有2级消防告警
			if(GetGCellTempAPI(sPack2HighTPos + 48 - 1) >= 900)
			{
					sFire2LevTime5s++;
					if(sFire2TRiseTime5s == 200)
					{
						if(sPack2HighTPos >= 1)
						{
							sNewTemp = GetGCellTempAPI(sPack2HighTPos + 48 - 1);//记录该温度
						}
						sFire2LevTime5s = 0;
						sHighTempRise = 0;
						sFire2TRiseTime5s = 0;
					}
					sFire2TRiseTime5s++;
					if(sFire2TRiseTime5s >= 10)
					{
						if(sPack2HighTPos >= 1)
						{
							sHighTempRise = GetGCellTempAPI(sPack2HighTPos + 48 - 1) - sNewTemp;
							sNewTemp = GetGCellTempAPI(sPack2HighTPos + 48 - 1);
						}
						sFire2TRiseTime5s = 0;
					}
					if((sFire2LevTime5s >= 10) && (sHighTempRise >= 30))
					{
						sRiseTime5s++;
						if(sRiseTime5s >= 5)
						{
							SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
							sFireLevel2Pos = 2;
							sRiseTime5s = 0;
						}
					}
					else
					{
						sRiseTime5s = 0;
					}
			}
			else
			{
				sFire2TRiseTime5s = 200;
				sHighTempRise = 0;
				sFire2LevTime5s = 0;
				sRiseTime5s = 0;
			}
			//根据相邻位置的温升或者温度和气溶胶告警判断是否触发2级消防告警
			//特殊位置，只需记录下一个电芯温升或者温度
			if((sPack2HighTPos == 1) || (sPack2HighTPos == 13) || (sPack2HighTPos == 25) || (sPack2HighTPos == 37))
			{
				if((PackAerosolInfo_211[eAerosolInfo211_2AerCon] >= 600) && (GetGCellTempAPI(sPack2HighTPos + 48) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 2;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack2HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack2HighTPos + 48);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack2HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack2HighTPos + 48) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack2HighTPos + 48);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 2;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//特殊位置，只需记录上一个电芯温升或者温度
			else if((sPack2HighTPos == 12) || (sPack2HighTPos == 24) || (sPack2HighTPos == 36) || (sPack2HighTPos == 48))
			{
				if((PackAerosolInfo_211[eAerosolInfo211_2AerCon] >= 600) && (GetGCellTempAPI(sPack2HighTPos + 48 - 2) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 2;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack2HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack2HighTPos + 48 - 2);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack2HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack2HighTPos + 48 - 2) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack2HighTPos + 48  - 2);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 2;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//正常位置，需要记录前后两个电芯的温升或者温度
			else
			{
				if((PackAerosolInfo_211[eAerosolInfo211_2AerCon] >= 600) && (GetGCellTempAPI(sPack2HighTPos + 48) >= 800)&& (GetGCellTempAPI(sPack2HighTPos + 48 - 2) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 2;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack2HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack2HighTPos + 48 - 2);//记录该温度
						sNewTemp2 = GetGCellTempAPI(sPack2HighTPos + 48);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack2HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack2HighTPos + 48 - 2) - sNewTemp1;
						sHighTempRise2 = GetGCellTempAPI(sPack2HighTPos + 48) - sNewTemp2;
						sNewTemp1 = GetGCellTempAPI(sPack2HighTPos + 48 - 2);
						sNewTemp2 = GetGCellTempAPI(sPack2HighTPos + 48);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if((sHighTempRise1 >= 30) && (sHighTempRise2 >= 30))
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 2;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
		}
		//第三个PACK
		else if(sFireLevel1Pos == 3)
		{
			//根据最大电芯温度和温升判断是否有2级消防告警
			if(GetGCellTempAPI(sPack3HighTPos + 96 - 1) >= 900)
			{
				{
					sFire2LevTime5s++;
					if(sFire2TRiseTime5s == 200)
					{
						if(sPack3HighTPos >= 1)
						{
							sNewTemp = GetGCellTempAPI(sPack3HighTPos + 96 - 1);//记录该温度
						}
						sFire2LevTime5s = 0;
						sHighTempRise = 0;
						sFire2TRiseTime5s = 0;
					}
					sFire2TRiseTime5s++;
					if(sFire2TRiseTime5s >= 10)
					{
						if(sPack3HighTPos >= 1)
						{
							sHighTempRise = GetGCellTempAPI(sPack3HighTPos + 96 - 1) - sNewTemp;
							sNewTemp = GetGCellTempAPI(sPack3HighTPos + 96 - 1);
						}
						sFire2TRiseTime5s = 0;
					}
					if((sFire2LevTime5s >= 10) && (sHighTempRise >= 30))
					{
						sRiseTime5s++;
						if(sRiseTime5s >= 5)
						{
							SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
							sFireLevel2Pos = 3;
							sRiseTime5s = 0;
						}
					}
					else
					{
						sRiseTime5s = 0;
					}
				}
			}
			else
			{
				sFire2TRiseTime5s = 200;
				sHighTempRise = 0;
				sFire2LevTime5s = 0;
				sRiseTime5s = 0;
			}
			//根据相邻位置的温升或者温度和气溶胶告警判断是否触发2级消防告警
			//特殊位置，只需记录下一个电芯温升或者温度
			if((sPack3HighTPos == 1) || (sPack3HighTPos == 13) || (sPack3HighTPos == 25) || (sPack3HighTPos == 37))
			{
				if((PackAerosolInfo_211[eAerosolInfo211_3AerCon] >= 600) && (GetGCellTempAPI(sPack3HighTPos + 96) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 3;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack3HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack3HighTPos + 96);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack3HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack3HighTPos + 96) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack3HighTPos + 96);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 3;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//特殊位置，只需记录上一个电芯温升或者温度
			else if((sPack3HighTPos == 12) || (sPack3HighTPos == 24) || (sPack3HighTPos == 36) || (sPack3HighTPos == 48))
			{
				if((PackAerosolInfo_211[eAerosolInfo211_3AerCon] >= 600) && (GetGCellTempAPI(sPack3HighTPos + 96 - 2) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 3;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack3HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack3HighTPos + 96 - 2);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack3HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack3HighTPos + 96 - 2) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack3HighTPos + 96 - 2);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 3;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//正常位置，需要记录前后两个电芯的温升或者温度
			else
			{
				if((PackAerosolInfo_211[eAerosolInfo211_3AerCon] >= 600) && (GetGCellTempAPI(sPack3HighTPos + 96 - 2) >= 800)&& (GetGCellTempAPI(sPack3HighTPos + 96) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 3;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack3HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack3HighTPos + 96 - 2);//记录该温度
						sNewTemp2 = GetGCellTempAPI(sPack3HighTPos + 96);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack3HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack3HighTPos + 96 - 2) - sNewTemp1;
						sHighTempRise2 = GetGCellTempAPI(sPack3HighTPos + 96) - sNewTemp2;
						sNewTemp1 = GetGCellTempAPI(sPack3HighTPos + 96 - 2);
						sNewTemp2 = GetGCellTempAPI(sPack3HighTPos + 96);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if((sHighTempRise1 >= 30) && (sHighTempRise2 >= 30))
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 3;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
		}
		//第4个PACK
		else if(sFireLevel1Pos == 4)
		{
			//根据最大电芯温度和温升判断是否有2级消防告警
			if(GetGCellTempAPI(sPack4HighTPos + 144- 1) >= 900)
			{
				{
					sFire2LevTime5s++;
					if(sFire2TRiseTime5s == 200)
					{
						if(sPack4HighTPos >= 1)
						{
							sNewTemp = GetGCellTempAPI(sPack4HighTPos + 144 - 1);//记录该温度
						}
						sFire2LevTime5s = 0;
						sHighTempRise = 0;
						sFire2TRiseTime5s = 0;
					}
					sFire2TRiseTime5s++;
					if(sFire2TRiseTime5s >= 10)
					{
						if(sPack4HighTPos >= 1)
						{
							sHighTempRise = GetGCellTempAPI(sPack4HighTPos + 144 - 1) - sNewTemp;
							sNewTemp = GetGCellTempAPI(sPack4HighTPos + 144 - 1);
						}
						sFire2TRiseTime5s = 0;
					}
					if((sFire2LevTime5s >= 10) && (sHighTempRise >= 30))
					{
						sRiseTime5s++;
						if(sRiseTime5s >= 5)
						{
							SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
							sFireLevel2Pos = 4;
							sRiseTime5s = 0;
						}
					}
					else
					{
						sRiseTime5s = 0;
					}
				}
			}
			else
			{
				sFire2TRiseTime5s = 200;
				sHighTempRise = 0;
				sFire2LevTime5s = 0;
				sRiseTime5s = 0;
			}
			//根据相邻位置的温升或者温度和气溶胶告警判断是否触发2级消防告警
			//特殊位置，只需记录下一个电芯温升或者温度
			if((sPack4HighTPos == 1) || (sPack4HighTPos == 13) || (sPack4HighTPos == 25) || (sPack4HighTPos == 37))
			{
				if((PackAerosolInfo_211[eAerosolInfo211_4AerCon] >= 600) && (GetGCellTempAPI(sPack4HighTPos + 144) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 4;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack4HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack4HighTPos + 144);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack1HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack4HighTPos + 144) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack4HighTPos + 144);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 4;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//特殊位置，只需记录上一个电芯温升或者温度
			else if((sPack4HighTPos == 12) || (sPack4HighTPos == 24) || (sPack4HighTPos == 36) || (sPack4HighTPos == 48))
			{
				if((PackAerosolInfo_211[eAerosolInfo211_4AerCon] >= 600) && (GetGCellTempAPI(sPack4HighTPos + 144 - 2) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 4;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack4HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack4HighTPos + 144 - 2);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack4HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack4HighTPos + 144 - 2) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack4HighTPos + 144  - 2);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 4;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//正常位置，需要记录前后两个电芯的温升或者温度
			else
			{
				if((PackAerosolInfo_211[eAerosolInfo211_4AerCon] >= 600) && (GetGCellTempAPI(sPack4HighTPos + 144) >= 800) && (GetGCellTempAPI(sPack4HighTPos + 144 - 2) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 4;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack4HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack4HighTPos + 144- 2);//记录该温度
						sNewTemp2 = GetGCellTempAPI(sPack4HighTPos + 144);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack4HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack4HighTPos + 144 - 2) - sNewTemp1;
						sHighTempRise2 = GetGCellTempAPI(sPack4HighTPos + 144) - sNewTemp2;
						sNewTemp1 = GetGCellTempAPI(sPack4HighTPos + 144 - 2);
						sNewTemp2 = GetGCellTempAPI(sPack4HighTPos + 144);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if((sHighTempRise1 >= 30) && (sHighTempRise2 >= 30))
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 4;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
		}
		//第5个PACK
		else if(sFireLevel1Pos == 5)
		{
			//根据最大电芯温度和温升判断是否有2级消防告警
			if(GetGCellTempAPI(sPack5HighTPos + 192- 1) >= 900)
			{
				{
					sFire2LevTime5s++;
					if(sFire2TRiseTime5s == 200)
					{
						if(sPack1HighTPos >= 1)
						{
							sNewTemp = GetGCellTempAPI(sPack5HighTPos + 192 - 1);//记录该温度
						}
						sFire2LevTime5s = 0;
						sHighTempRise = 0;
						sFire2TRiseTime5s = 0;
					}
					sFire2TRiseTime5s++;
					if(sFire2TRiseTime5s >= 10)
					{
						if(sPack5HighTPos >= 1)
						{
							sHighTempRise = GetGCellTempAPI(sPack5HighTPos + 192 - 1) - sNewTemp;
							sNewTemp = GetGCellTempAPI(sPack5HighTPos + 192 - 1);
						}
						sFire2TRiseTime5s = 0;
					}
					if((sFire2LevTime5s >= 10) && (sHighTempRise >= 30))
					{
						sRiseTime5s++;
						if(sRiseTime5s >= 5)
						{
							SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
							sFireLevel2Pos = 5;
							sRiseTime5s = 0;
						}
					}
					else
					{
						sRiseTime5s = 0;
					}
				}
			}
			else
			{
				sFire2TRiseTime5s = 200;
				sHighTempRise = 0;
				sFire2LevTime5s = 0;
				sRiseTime5s = 0;
			}
			//根据相邻位置的温升或者温度和气溶胶告警判断是否触发2级消防告警
			//特殊位置，只需记录下一个电芯温升或者温度
			if((sPack5HighTPos == 1) || (sPack5HighTPos == 13) || (sPack5HighTPos == 25) || (sPack5HighTPos == 37))
			{
				if((PackAerosolInfo_211[eAerosolInfo211_5AerCon] >= 600) && (GetGCellTempAPI(sPack5HighTPos + 192) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 5;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack5HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack5HighTPos + 192);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack5HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack5HighTPos + 192) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack5HighTPos + 192);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 5;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//特殊位置，只需记录上一个电芯温升或者温度
			else if((sPack5HighTPos == 12) || (sPack5HighTPos == 24) || (sPack5HighTPos == 36) || (sPack5HighTPos == 48))
			{
				if((PackAerosolInfo_211[eAerosolInfo211_5AerCon] >= 600) && (GetGCellTempAPI(sPack5HighTPos + 192 - 2) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 5;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack5HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack5HighTPos + 192 - 2);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack5HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack5HighTPos + 192 - 2) - sNewTemp1;
						sNewTemp1 = GetGCellTempAPI(sPack5HighTPos + 192  - 2);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if(sHighTempRise1 >= 30)
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 5;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
			//正常位置，需要记录前后两个电芯的温升或者温度
			else
			{
				if((PackAerosolInfo_211[eAerosolInfo211_5AerCon] >= 600) && (GetGCellTempAPI(sPack5HighTPos + 192) >= 800)&& (GetGCellTempAPI(sPack5HighTPos + 192 - 2) >= 800))
				{
					sPpmAlarmTime++;
					if(sPpmAlarmTime >= 10)
					{
						sPpmAlarmTime = 0;
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 5;
					}
				}
				else
				{
					sPpmAlarmTime = 0;
				}
				if(sFire2TRiseTime5s1 == 200)
				{
					if(sPack5HighTPos >= 1)
					{
						sNewTemp1 = GetGCellTempAPI(sPack5HighTPos + 192 - 2);//记录该温度
						sNewTemp2 = GetGCellTempAPI(sPack5HighTPos + 192);//记录该温度
					}
					sFire2LevTime5s = 0;
					sHighTempRise1 = 0;
					sFire2TRiseTime5s1 = 0;
				}
				sFire2TRiseTime5s1++;
				if(sFire2TRiseTime5s1 >= 10)
				{
					if(sPack5HighTPos >= 1)
					{
						sHighTempRise1 = GetGCellTempAPI(sPack5HighTPos + 192 - 2) - sNewTemp1;
						sHighTempRise2 = GetGCellTempAPI(sPack5HighTPos + 192) - sNewTemp2;
						sNewTemp1 = GetGCellTempAPI(sPack5HighTPos + 192 - 2);
						sNewTemp2 = GetGCellTempAPI(sPack5HighTPos + 192);
					}
					sFire2TRiseTime5s1 = 0;
				}
				if((sHighTempRise1 >= 30) && (sHighTempRise2 >= 30))
				{
					sTRiseTime10s++;
					if(sTRiseTime10s >= 20)
					{
						SetFireLevelInfoAPI(eFireInfo254_Level2,1); //触发消防2级告警，不可恢复
						sFireLevel2Pos = 5;
						sTRiseTime10s = 0;
					}
				}
				else
				{
					sTRiseTime10s = 0;
				}
			}
		}
	}

	//by lxr01202  消防3级告警判断(且二级消防告警才告3级告警)
	if(((GetPduOutImeu3InfoAPI(ePduOut219_Gas1) >= 2500) || (GetPduOutImeu3InfoAPI(ePduOut219_Gas2) >= 2500))
		&& (GetFireLevelInfoAPI(eFireInfo254_Level2) == 1))
	{
		sGasAlarmTime++;
		if(sGasAlarmTime >= 10)
		{
			SetFireLevelInfoAPI(eFireInfo254_Level3,1); //触发消防3级告警，不可恢复
		}
	}
	else
	{
		sGasAlarmTime = 0;
		if(GetFireLevelInfoAPI(eFireInfo254_Level2) == 0)//无二级消防，可能为误触发
		{
			SetFireLevelInfoAPI(eFireInfo254_Level3,0);
		}
	}

	//消防四级判断
	//烟雾告警且至少一个柜内温度≥70度且全氟己酮已喷射
	if(((1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE))))
		&& (PerfluState == 1)
		&& ((GetImeu3TempInfoAPI(eIMEU3217_485T1) >= 70) || (GetImeu3TempInfoAPI(eIMEU3217_CabT1) >= 70) || (GetImeu3TempInfoAPI(eIMEU3217_CabT2) >= 70))
		)
	{
		sFire4TTimse10s++;
		if(sFire4TTimse10s >= 20)
		{
			SetFireLevelInfoAPI(eFireInfo254_Level4,1);
			sFire4TTimse10s = 0;
		}
	}
	else
	{
		sFire4TTimse10s = 0;
	}
	//柜内H2浓度≥800ppm且至少一个柜内温度≥70度且全氟己酮已喷射，持续5s
	if((GetPduOutImeu3InfoAPI(ePduOut219_Gas1) >= 2500)
		& ((GetImeu3TempInfoAPI(eIMEU3217_485T1) >= 70) || (GetImeu3TempInfoAPI(eIMEU3217_CabT1) >= 70) || (GetImeu3TempInfoAPI(eIMEU3217_CabT2) >= 70))
		& (PerfluState == 1))
	{
		sFire4TTimse5s1++;
		if(sFire4TTimse5s1 >= 20)
		{
			SetFireLevelInfoAPI(eFireInfo254_Level4,1);
			sFire4TTimse5s1 = 0;
		}
	}
	else
	{
		sFire4TTimse5s1 = 0;
	}

	//柜内CO浓度≥600ppm且至少一个柜内温度≥70度且全氟己酮已喷射，持续5s
	if((GetPduOutImeu3InfoAPI(ePduOut219_Gas2) >= 2500)
		& ((GetImeu3TempInfoAPI(eIMEU3217_485T1) >= 70) || (GetImeu3TempInfoAPI(eIMEU3217_CabT1) >= 70) || (GetImeu3TempInfoAPI(eIMEU3217_CabT2) >= 70))
		& (PerfluState == 1))
	{
		sFire4TTimse5s2++;
		if(sFire4TTimse5s1 >= 20)
		{
			SetFireLevelInfoAPI(eFireInfo254_Level4,1);
			sFire4TTimse5s2 = 0;
		}
	}
	else
	{
		sFire4TTimse5s2 = 0;
	}

	//柜内H2浓度≥800ppm且烟雾告警且全氟己酮已喷射，持续5s
	if((GetPduOutImeu3InfoAPI(ePduOut219_Gas1) >= 2500)
		& (1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
		& (PerfluState == 1))
	{
		sFire4TTimse5s3++;
		if(sFire4TTimse5s3 >= 20)
		{
			SetFireLevelInfoAPI(eFireInfo254_Level4,1);
			sFire4TTimse5s3 = 0;
		}
	}
	else
	{
		sFire4TTimse5s3 = 0;
	}

	//柜内CO浓度≥600ppm且烟雾告警且全氟己酮已喷射，持续5s
	if((GetPduOutImeu3InfoAPI(ePduOut219_Gas2) >= 2500)
		& (1 == BitGet(GetGFaultInfoAPI((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) / FAULT_DATATYPE), ((eERR_ID_BATT_SMOG - eERR_ID_ERRSTART) % FAULT_DATATYPE)))
		& (PerfluState == 1))
	{
		sFire4TTimse5s4++;
		if(sFire4TTimse5s4 >= 20)
		{
			SetFireLevelInfoAPI(eFireInfo254_Level4,1);
			sFire4TTimse5s4 = 0;
		}
	}
	else
	{
		sFire4TTimse5s4 = 0;
	}


    switch(sCtrlStep)
    {
    	//发送控制声光报警器报文
		case 0:
			/*手动控制强制开启声光报警*/
			if(1 == gAuxZHSAForceCmd)
			{
				//by lxr01202 新增A2419声光告警
				/*A2419和A2425、Y2403项目*/
				if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
				|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
				{
					SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR,1,1);
				}
				else
				{
					//发送控制开启声光报警器命令
					SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR, 4 ,1);					
				}
			}
			/*手动控制强制关闭声光报警*/
			else if(2 == gAuxZHSAForceCmd)
			{
				//已经开启声光报警或30s定时放送(防止声光报警无反馈信号)
				if(sTime >= 60)
				{
					//by lxr01202 新增A2419声光告警关闭
					/*A2419和A2425、Y2403项目*/
					if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
					|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
					{
						SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR,1,0);
					}
					else
					{
						//发送控制关闭声光报警器命令
						SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR, 4 ,0);
					}
				}
			}
			//一级、二级消防告警，开启声光告警
			if((GetFireLevelInfoAPI(eFireInfo254_Level2) == 1) || (GetFireLevelInfoAPI(eFireInfo254_Level1) == 1))
			{
				/*A2415项目，主控控制声光报警器*/
				/*A2415、A2445、A2516、A2415降本项目，主控控制声光报警器*/
				if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
				|| (gGHardPara_104[eGHardPara104_ProjectCode] == 5) || (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
				|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
				{
					gAuxZHSAForceCmd = 1;
				}
				/*A2419和A2425、Y2403项目*/
				else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
				|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
				{
					//暂不处理
				}
				/*默认按A2415项目*/
				else
				{
					gAuxZHSAForceCmd = 1;
				}
			}
			sCtrlStep = 1;
			break;

		//发送触发全氟己酮消防报文
		case 1:
			if(GetFireLevelInfoAPI(eFireInfo254_Level2) == 1)
			{
				if(gGHardPara_104[eGHardPara104_PerfluorEn] != 0)//使能全氟己酮消防功能
				{
					/*A2415 A2445 A2516 A2415降本项目*/
					if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
						|| (gGHardPara_104[eGHardPara104_ProjectCode] == 5) || (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
						|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
					{
						SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,sFireLevel2Pos + 7,1); //控制对应PACK电磁阀刺破阀打开
					}
					/*A2419 A2425 Y2403项目*/
					else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
					|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
					{
						if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
						{
							SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,sFireLevel2Pos + 7,1); //控制对应PACK电磁阀刺破阀打开
						}
						else if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 51)
						{
							SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,sFireLevel2Pos + 13,1); //控制对应PACK电磁阀刺破阀打开
						}
					}
					/*默认A2415项目*/
					else
					{
						SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR,sFireLevel2Pos + 7,1); //控制对应PACK电磁阀刺破阀打开
					}
					PunctureTime++;
				}
				if(PunctureTime >= 3)
				{
					/*A2415 A2445 A2516 A2415降本项目*/
					if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
					|| (gGHardPara_104[eGHardPara104_ProjectCode] == 5)	|| (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
					|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
					{
						SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR, 7,1); //控制电磁阀总阀打开
					}
					/*A2419 A2425 Y2403项目*/
					else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
					|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
					{
						if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
						{
							SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR, 7,1); //控制电磁阀总阀打开
						}
						else if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 51)
						{
							SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR, 13,1); //控制电磁阀总阀打开
						}
					}
					else
					{
						SendCanImeu3IoCtrlMsg242(IMEU3_PDUOUT_DEVADDR, 7,1); //控制电磁阀总阀打开
					}
					PerfluState = 1;
				}
			}
			//触发消防4级告警，“自杀”下电
			if(GetFireLevelInfoAPI(eFireInfo254_Level4) == 1)
			{
				if(sDcPowerFlag == 0)
				{
					sDcPowerFlag = 1;
					TacCtrlGroupSwhStateAPI(eGroupSwh_Pwr, ePSTATE_ON);             /*闭合主控电源接触器电源(高电平弹开使断电)*/
				}
				else
				{
					sDcPowerFlag = 0;
					TacCtrlGroupSwhStateAPI(eGroupSwh_Pwr, ePSTATE_OFF);
				}
				/*A2415 A2445项目，控制交流自杀*/
				if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
				{
					SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR,1,1); //控制QF1断开
				}
			}

			sCtrlStep = 0;
			break;

		default:
			sCtrlStep = 0;
			break;
    }
}
#endif
//#endif

//#if(BMU_IMLU2_EN == 1)
/*=================================================================================================
** @Name      : void RcvMU2LightColorCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取灯带颜色
** @The notes : 通过MU2下发指令和BMU自身充放电状态综合判断
**===============================================================================================*/
void RcvMU2LightColorCmdAPI(u16 Data)
{
	MU2LightColorCmd = Data & 0xff;
	return;
}
/*=================================================================================================
** @Name      : void RcvMU2PcsAirCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取模块侧空调开关机指令
** @The notes : 通过MU2下发指令控制空调模块测液路开关机
**===============================================================================================*/
void RcvMU2PcsAirCmdAPI(u16 Data)
{
	MU2PcsAirCmd = Data & 0xff;
	return;
}
/*=================================================================================================
** @Name      : void RcvMU2CellAirCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取电池侧空调开关机指令
** @The notes : 通过MU2下发指令控制空调电池侧液路开关机
**===============================================================================================*/
void RcvMU2CellAirCmdAPI(u16 Data)
{
	MU2CellAirCmd_Ctrl = Data & 0xff;
	MU2Cell_AirCmd_Rcved = 1;
	if(MU2CellAirCmd_Ctrl == 0)      //关机
	{
		MU2Cell_AirCmd_Flag = 0;     
	}
	else
	{
		MU2Cell_AirCmd_Flag = 1;     //默认模式
	}
	return;
}
/*=================================================================================================
** @Name      : void RcvMU2AirNoiseCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取空调降噪指令
** @The notes : 通过MU2下发指令控制空调降噪开启或关闭
**===============================================================================================*/
void RcvMU2AirNoiseCmdAPI(u16 Data)
{
	MU2AirNoiseCmd = Data & 0xff;
	return;
}
/*=================================================================================================
** @Name      : void void RcvMU2_TFAirON_OFF_ResetCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取TF空调开关机及报警复位指令
** @The notes : 通过MU2下发指令控制TF空调开关机及报警复位
**===============================================================================================*/
void RcvMU2_TFAirON_OFF_ResetCmdAPI(u16 Data)
{
	MU2_TFAirON_OFF_ResetCmd = Data & 0xff;
	return;
}
/*=================================================================================================
** @Name      : void  RcvMU2_TFAir_Type_CmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取TF空调模式选择指令
** @The notes : 通过MU2下发指令控制TF空调选择高收益还是长寿命模式
**===============================================================================================*/
void RcvMU2_TFAir_Type_CmdAPI (u16 Data)
{
	MU2_TFAir_TypeChoose_Cmd = Data & 0xff;
}
/*=================================================================================================
** @Name      : void  RcvMU2_XMAir_Type_CmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取XM空调模式选择指令
** @The notes : 通过MU2下发指令控制XM空调选择高收益还是长寿命模式
**===============================================================================================*/
void RcvMU2_XMAir_Type_CmdAPI (u16 Data)
{
	MU2_XMAir_TypeChoose_Cmd = Data & 0xff;
}
/*=================================================================================================
** @Name      : void RcvMU2CellTemSpeCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取电池特殊点位温度屏蔽指令
** @The notes : 通过MU2下发指令控制是否屏蔽电池特殊点位的温度
**===============================================================================================*/
void RcvMU2CellTemSpeCmdAPI(u16 Data)
{
	MU2CellTemSpePoinCmd = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2Eu3CellWaterPump_ManualCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制电池水泵标志
** @The notes : 通过MU2下发指令控制是否手动控电池水泵标志
**===============================================================================================*/
void RcvMU2Eu3CellWaterPump_ManualCmdAPI(u16 Data)      
{
	 MU2Eu3CellPump_ManualEn = Data & 0xff;
	return; 
}
/*=================================================================================================
** @Name      : void RcvMU2Eu3CellWaterPump_SpdCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制电池水泵转速 单位：1%
** @The notes : 通过MU2下发指令手动控制水泵转速
**===============================================================================================*/
void RcvMU2Eu3CellWaterPump_SpdCmdAPI(u16 Data)         
{
	MU2Eu3CellPump_ManualSpd = Data & 0xff;
	return;
}
/*=================================================================================================
** @Name      : void RcvMU2Eu3ModWaterPump_ManualCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制模块水泵标志
** @The notes : 通过MU2下发指令手动控制模块水泵
**===============================================================================================*/
void RcvMU2Eu3ModWaterPump_ManualCmdAPI(u16 Data)       
{ 
	MU2Eu3ModPump_ManualEn = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2Eu3ModWaterPump_SpdCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制模块水泵转速 单位：1%
** @The notes : 通过MU2下发指令手动控制模块水泵转速
**===============================================================================================*/
void RcvMU2Eu3ModWaterPump_SpdCmdAPI(u16 Data)         
{ 
	MU2Eu3ModPump_ManualSpd = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2ElecExpValve_ManualCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制主电子膨胀阀标志
** @The notes : 通过MU2下发指令手动控制主电子膨胀阀标志
**===============================================================================================*/
void RcvMU2ElecExpValve_ManualCmdAPI(u16 Data)         
{ 
	MU2ElecValve_ManualEn = Data & 0xff;
	return; 
}

/*=================================================================================================
** @Name      : void RcvMU2ElecExpValve_PulseCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制主电子膨胀阀开度
** @The notes : 通过MU2下发指令手动控制主电子膨胀阀开度[0-480]
**===============================================================================================*/
void RcvMU2ElecExpValve_PulseCmdAPI(u16 Data)         
{ 
	//MU2ElecValve_ManualPulse = ((Data & 0xFF) << 8) | ((Data >> 8) & 0xFF);   //手动交换高低字节 ，否则上层发送过来480，这边会解析为E001,导致数据错误
	//MU2ElecValve_ManualPulse = Data & 0xff;
	MU2ElecValve_ManualPulse = Data & 0xFFFF;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2Fan_ManualCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制风机标志
** @The notes : 通过MU2下发指令手动控制风机标志
**===============================================================================================*/
void RcvMU2Fan_ManualCmdAPI(u16 Data)                  
{ 
	MU2Fan_ManualEn = Data & 0xff;
	return; 
}

/*=================================================================================================
** @Name      : void RcvMU2Fan_SpdCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制风机转速
** @The notes : 通过MU2下发指令手动控制风机转速
**===============================================================================================*/
void RcvMU2Fan_SpdCmdAPI(u16 Data)                      
{ 
	MU2Fan_ManualSpdPerc = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2Compressor_ManualCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制压缩机频率标志
** @The notes : 通过MU2下发指令手动控制压缩机频率标志
**===============================================================================================*/
void RcvMU2Compressor_ManualCmdAPI(u16 Data)            
{ 
	MU2Comp_ManualEn = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2Compressor_FreqCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制压缩机频率
** @The notes : 通过MU2下发指令手动控制压缩机频率(HZ)
**===============================================================================================*/
void RcvMU2Compressor_FreqCmdAPI(u16 Data)              
{ 
	MU2Comp_ManualFreq = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2CellHeat_ManualCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制电加热标志
** @The notes : 通过MU2下发指令手动控制电加热标志
**===============================================================================================*/
void RcvMU2CellHeat_ManualCmdAPI(u16 Data)             
{
	MU2CellHeat_ManualEn = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2CellHeat_OnOffCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制电加热
** @The notes : 通过MU2下发指令手动控制电加热
**===============================================================================================*/
void RcvMU2CellHeat_OnOffCmdAPI(u16 Data)               
{
	MU2CellHeat_ManualOn = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2BatHeatFilm_ManualCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制曲轴电加热标志
** @The notes : 通过MU2下发指令手动控制曲轴电加热标志
**===============================================================================================*/
void RcvMU2BatHeatFilm_ManualCmdAPI(u16 Data)           
{
	MU2BatFilm_ManualEn = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2BatHeatFilm_OnOffCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制曲轴电加热
** @The notes : 通过MU2下发指令手动控制曲轴电加热
**===============================================================================================*/
void RcvMU2BatHeatFilm_OnOffCmdAPI(u16 Data)           
{ 
	MU2BatFilm_ManualOn = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2CompDrvFan_ManualCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制压缩机驱动器散热风机标志
** @The notes : 通过MU2下发指令手动控制压缩机驱动器散热风机标志
**===============================================================================================*/
void RcvMU2CompDrvFan_ManualCmdAPI(u16 Data)            
{
	MU2CompDrvFan_ManualEn = Data & 0xff;
	return;
}

/*=================================================================================================
** @Name      : void RcvMU2CompDrvFan_SpdCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动控制压缩机驱动器散热风机转速
** @The notes : 通过MU2下发指令手动控制压缩机驱动器散热风机转速
**===============================================================================================*/
void RcvMU2CompDrvFan_SpdCmdAPI(u16 Data)               
{
	MU2CompDrvFan_ManualSpd = Data & 0xff;
	return;
}
/*=============================================================================
** @Name      : void RcvMU2DiagKeepPowerModeCmdAPI(u16 Data)
** @Input     : Data: MU2下发的保供电模式指令数据 (0: 正常模式, 1: 保供电模式)
** @Output    : void
** @Function  : 接收并设置保供电模式状态
**===========================================================================*/
void RcvMU2DiagKeepPowerModeCmdAPI(u16 Data)
{
    MU2KeepPowerMode = Data & 0xff;  
    return;
}
/*=================================================================================================
** @Name      : void RcvMU2ManualCalibration_SOCCmdAPI(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取手动校准SOC
** @The notes : 通过MU2下发指令手动校准SOC
**===============================================================================================*/
void RcvMU2ManualCalibration_SOCCmdAPI(u16 Data)               
{
	MU2ManualCalibration_SOC = Data & 0xffff;
	/* 校准当前容量 */
    CorrGNowCapBySocAPI(MU2ManualCalibration_SOC/10);
    
    /* 校准剩余容量 */
    CorrGRemainCapBySocAPI(MU2ManualCalibration_SOC/10);
    
    /* 更新显示值 */
    CorrGDisplaySocByUser(MU2ManualCalibration_SOC);
	return;
}
/*=================================================================================================
** @Name      : void RcvMU2ResetAFEstatusCmdAPI(u16 Data)
** @Input     : Data:下发的从控序号
** @Output    : void
** @Function  : 获取手动控制重置AFE状态
** @The notes : 通过MU2下发指令手动控制重置AFE状态
**===============================================================================================*/
void RcvMU2ResetAFEstatusCmdAPI(u16 Data)               
{
	MU2ResetAFEstatus = Data & 0xff;           
    MU2ResetAFE_BSU_ID = (Data >> 8) & 0xff;   
	if(MU2ResetAFEstatus)
	{
		MU2ResetAFE_SOC_Flag = 1;                //重置AFE状态时不能校准SOC
	}
	SendResetAFEstatus(MU2ResetAFE_BSU_ID,MU2ResetAFEstatus);
	return;
}
/*=================================================================================================
** @Name      : void RcvMU2ResetBSUstatusCmdAPI(u32 Data)
** @Input     : Data:下发的从控序号
** @Output    : void
** @Function  : 获取手动控制重置BSU状态
** @The notes : 通过MU2下发指令手动控制重置BSU状态
**===============================================================================================*/
void RcvMU2ResetBSUstatusCmdAPI(u16 Data)               
{
	MU2ResetBSUstatus = Data & 0xff;
	MU2ResetBSU_ID = (Data >> 8) & 0xff;
	SendResetBSUstatus(MU2ResetBSU_ID,MU2ResetBSUstatus);
	if(MU2ResetBSUstatus)
	{
		MU2ResetAFE_SOC_Flag = 0;                //恢复BSU状态时恢复校准SOC
	}
	return;
}
/*=================================================================================================
** @Name      : u8 GetImlu2LightStripColor(u16 Data)
** @Input     : Data:MU2下发的指示灯状态
** @Output    : void
** @Function  : 获取灯带颜色
** @The notes : 通过MU2下发指令和BMU自身充放电状态综合判断
**===============================================================================================*/
u8 JudgeImlu2LightStripColor(void)
{
	u8 ErrorSta = 0; 		/*故障指示灯状态*/
	u8 ChargeSta = 0;		/*充电指示灯状态*/
	u8 DischargeSta = 0;	/*放电指示灯状态*/
	u8 OffGirdSta = 0;      /*离网指示灯状态*/
	//u8 StandbySta = 0;		/*待机指示灯状态*/

	u8 Color = 0;
	/*判断故障状态*/
	if(1 == BitGet(MU2LightColorCmd,2))
		ErrorSta = 1;
	else 
		ErrorSta = 0;
	/*判断离网状态*/
	if(1 == BitGet(MU2LightColorCmd,4))
		OffGirdSta = 1;
	else 
		OffGirdSta = 0;
	/*判断充放电状态*/
	if(eCURR_CHG == GetGChgDhgStateAPI())
	{
		ChargeSta = 1;
		DischargeSta = 0;
	}
	else if(eCURR_DHG == GetGChgDhgStateAPI())
	{
		ChargeSta = 0;
		DischargeSta= 1;
	}
	else
	{
		ChargeSta = 0;
		DischargeSta = 0;
	}

	/*判断亮哪种指示灯,故障指示灯优先级最高*/
	if(ErrorSta == 1)
	{
		Color = e_Imlu2_Red;
	}
	else
	{
		/*离网状态*/
		if(OffGirdSta == 1)
		{
			Color = e_Imlu2_Orange;
		}
		else
		{
			if(ChargeSta ==  1)
			{
				Color = e_Imlu2_Yellow;
			}
			else if(DischargeSta == 1)
			{
				Color = e_Imlu2_Blue;
			}
			else
			{
				Color = e_Imlu2_Green;
			}
		}
	}
	return (Color);
}
/*=================================================================================================
** @Name      : static void TacLightStripCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 灯带控制任务
** @The notes : 1s,通过RS485通信控制IMLU2执行
**===============================================================================================*/
static void TacLightStripCtrlTask(void)
{
	u8 color = 0;
	u8 black_buffer[] = "~20C362427036FF0E0101000D00000007D014101400000079101010000000000193F2CD\r";
	u8 white_buffer[] = "~20C362427036FF0E0101000DFFFFFF07D014101400000079101010000000000193F249\r";
	u8 yellow_buffer[] = "~20C362427036FF0E0101000DFFFF0007D014101400000079101010000000000193F275\r";
	u8 blue_buffer[] = "~20C362427036FF0E0101000D0000FF07D014101400000079101010000000000193F2A1\r";
	u8 red_buffer[] = "~20C362427036FF0E0101000D00FF0007D014101400000079101010000000000193F2A1\r";
	u8 green_buffer[] = "~20C362427036FF0E0101000DFF000007D014101400000079101010000000000193F2A1\r";
	u8 orange_buffer[] = "~20C362427036FF0E0101000DA5FF0007D014101400000079101010000000000193F28B\r";
	/*获取上层MU2下发的灯带颜色指令*/
	color = JudgeImlu2LightStripColor();
	(void)DEVGPIOSetPortState(ePOut_485EN0, ePSTATE_ON);
	switch(color)
	{
		case e_Imlu2_Black:
		ModbusSendDataUserHook(black_buffer,sizeof(black_buffer)/sizeof(black_buffer[0]),eSCI0);
		ClearImeu2MsgAddAPI();
		break;

		case e_Imlu2_White:
		ModbusSendDataUserHook(white_buffer,sizeof(white_buffer)/sizeof(white_buffer[0]),eSCI0);
		ClearImeu2MsgAddAPI();
		break;

		case e_Imlu2_Yellow:
		ModbusSendDataUserHook(yellow_buffer,sizeof(yellow_buffer)/sizeof(yellow_buffer[0]),eSCI0);
		ClearImeu2MsgAddAPI();
		break;

		case e_Imlu2_Blue:
		ModbusSendDataUserHook(blue_buffer,sizeof(blue_buffer)/sizeof(blue_buffer[0]),eSCI0);
		ClearImeu2MsgAddAPI();
		break;

		case e_Imlu2_Red:
		ModbusSendDataUserHook(red_buffer,sizeof(red_buffer)/sizeof(red_buffer[0]),eSCI0);
		ClearImeu2MsgAddAPI();
		break;

		case e_Imlu2_Green:
		ModbusSendDataUserHook(green_buffer,sizeof(green_buffer)/sizeof(green_buffer[0]),eSCI0);
		ClearImeu2MsgAddAPI();
		break;

		case e_Imlu2_Orange:
		ModbusSendDataUserHook(orange_buffer,sizeof(orange_buffer)/sizeof(orange_buffer[0]),eSCI0);
		ClearImeu2MsgAddAPI();
		break;

		default:
		break;
	}
	(void)DEVGPIOSetPortState(ePOut_485EN0, ePSTATE_OFF);
	return;
}
/*=================================================================================================
** @Name      : void RcvImeu2MsgAddAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 接收IMLU2信息时调用
** @The notes : 接收到信息时调用
**===============================================================================================*/
void RcvImeu2MsgAddAPI(void)
{
	if(sRcvImlu2Count < 255)
		sRcvImlu2Count++;
}
/*=================================================================================================
** @Name      : void ClearImeu2MsgAddAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清空IMLU2通信计数
** @The notes : 接收到信息时调用
**===============================================================================================*/
void ClearImeu2MsgAddAPI(void)
{
	sRcvImlu2Count = 0;
}
/*=================================================================================================
** @Name      : u8 GetImeu2MsgAddAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 获取IMLU2通信计数
** @The notes :
**===============================================================================================*/
u8 GetImeu2MsgAddAPI(void)
{
	return(sRcvImlu2Count);
}
//#endif
//#if(1 == BMU_IMEU3_COM_EN)
/*=============================================================================================
** @Name      : static void TacShutFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : PDU扰流风机和模块散热风机控制策略任务
** @The notes : 1s任务,通过CAN通信控制IMEU3执行
**===========================================================================================*/
static u8 AverTimes = 0;
static s8 Aver5sPduTemp = 0;//5s时间的PDU环温
static s8 Aver5sPcsTemp = 0;//5s时间的PCS出风口环温
static s8 PduTemp[10] = {0};
static s8 PcsTemp[10] = {0};
static void TacPduPcsFanCtrlTask(void)
{
	 static u8 sCtrlStep = 0;  //控制步骤
	 static u8 sPcsStopDelayTimes = 6;
	 static u8 sPcsStartDelayTimes = 0;
	 static u8 sIsPcsFanStarted = 0;

	 static u8 sPduStopDelayTimes = 6;
	 static u8 sPduStartDelayTimes = 0;
	 static u8 sIsPduFanStarted = 0;

	 u8 TempDuty = 0;
	 u8 i = 0;
	 /*Pdu内IMEU3无通信中断*/
	 if(0 == DiagGetPduInImeu3OffCommuStateAPI())
	 {
		 if(AverTimes < 10)
		 {
			 PduTemp[AverTimes] = GetImeu3TempInfoAPI(eIMEU3217_PduEnvT);
			 PcsTemp[AverTimes] = GetImeu3TempInfoAPI(eIMEU3217_PcsAiroutT);
			 AverTimes++;
		 }
		 else
		 {
			 for(i = 0 ;i < 9 ;i++)
			 {
				 PduTemp[i] = PduTemp[i + 1];
				 PcsTemp[i] = PcsTemp[i + 1];
			 }
			 PduTemp[9] = GetImeu3TempInfoAPI(eIMEU3217_PduEnvT);
			 PcsTemp[9] = GetImeu3TempInfoAPI(eIMEU3217_PcsAiroutT);
			 Aver5sPduTemp = (PduTemp[0] + PduTemp[1] +  PduTemp[2] + PduTemp[3] +  PduTemp[4] + PduTemp[5] + PduTemp[6] + PduTemp[7] + PduTemp[8] + PduTemp[9]) / 10;
			 Aver5sPcsTemp = (PcsTemp[0] + PcsTemp[1] +  PcsTemp[2] + PcsTemp[3] +  PcsTemp[4] + PcsTemp[5] + PcsTemp[6] + PcsTemp[7] + PcsTemp[8] + PcsTemp[9]) / 10;
		 }
	 }
	 else
	 {
		 AverTimes = 0;
		 Aver5sPduTemp = 0;
		 Aver5sPcsTemp = 0;
	 }
	 switch(sCtrlStep)
	 {
	 	 case 0:
	 		if(BSPGetOSMillSecTimeAPI() >= 3000)
	 		{
	 			sCtrlStep = 1;
	 		}
	 		else
	 		{
	 			PcsFanDuty = 100;
	 			PduFanDuty = 100;
	 			sCtrlStep = 2;
	 		}
	 	break;
	 	case 1:
	 		/*调试模式直接控制*/
	 		if(eMODE_DEBUG == GetGWorkModeAPI())
	 		{
	 			sCtrlStep = 2;
	 		}
	 		else
	 		{
		 		 /*判断PDU内IMEU3是否通信中断*/
		 		if(0 == DiagGetPduInImeu3OffCommuStateAPI())
		 		{
		 			sCtrlStep = 2;
		 			/*PCS散热风机控制策略*/
		 			if(Aver5sPcsTemp < (s8)gGHardPara_104[eGHardPara104_PcsFanOpenT] - 5) //5℃回差
		 			{
		 				sPcsStartDelayTimes = 0;
		 				if(sPcsStopDelayTimes < 5)
		 				{
		 					sPcsStopDelayTimes++;
		 				}
		 				else
		 				{
		 					PcsFanDuty = 0;
		 					sIsPcsFanStarted = 0;
		 				}
		 			}
		 			else if(Aver5sPcsTemp >= (s8)gGHardPara_104[eGHardPara104_PcsFanOpenT])
		 			{
		 				sPcsStopDelayTimes = 0;
		 				if(sPcsStartDelayTimes < 5)
		 				{
		 					sPcsStartDelayTimes++;
		 				}
		 				else
		 				{
		 					if(((s8)gGHardPara_104[eGHardPara104_PcsFanFullT] > (s8)gGHardPara_104[eGHardPara104_PcsFanOpenT]) && (Aver5sPcsTemp <= (s8)gGHardPara_104[eGHardPara104_PcsFanFullT]))
		 					{
		 						TempDuty = 30 * (u8)((Aver5sPcsTemp - (s8)gGHardPara_104[eGHardPara104_PcsFanOpenT]) / (((s8)gGHardPara_104[eGHardPara104_PcsFanFullT] - (s8)gGHardPara_104[eGHardPara104_PcsFanOpenT])*1.0));
		 						if(TempDuty > 30)
		 							TempDuty = 30;
		 						PcsFanDuty = 30 + TempDuty;
		 					}
		 					else
		 					{
		 						PcsFanDuty = 100;
		 					}
		 				}
		 				sIsPcsFanStarted = 1;
		 			}
		 			else if(sIsPcsFanStarted == 1)
		 			{
		 				TempDuty = 1 * (Aver5sPcsTemp - ((s8)gGHardPara_104[eGHardPara104_PcsFanOpenT] - 5));
		 				if(TempDuty > 5)
		 					TempDuty = 5;
		 				PcsFanDuty = 25 + TempDuty;

		 			}
	 				else
	 				{
	 					PcsFanDuty = 0;
	 				}

		 			/*PDU散热风机控制策略*/
		 			if(Aver5sPduTemp < (s8)gGHardPara_104[eGHardPara104_PduFanOpenT] - 5) //5℃回差
		 			{
		 				sPduStartDelayTimes = 0;
		 				if(sPduStopDelayTimes < 5)
		 				{
		 					sPduStopDelayTimes++;
		 				}
		 				else
		 				{
		 					PduFanDuty = 0;
		 					sIsPduFanStarted = 0;
		 				}
		 			}
		 			else if(Aver5sPduTemp >= (s8)gGHardPara_104[eGHardPara104_PduFanOpenT])
		 			{
		 				sPduStopDelayTimes = 0;
		 				if(sPduStartDelayTimes < 5)
		 				{
		 					sPduStartDelayTimes++;
		 				}
		 				else
		 				{
		 					if(((s8)gGHardPara_104[eGHardPara104_PduFanFullT] > (s8)gGHardPara_104[eGHardPara104_PduFanOpenT]) && (Aver5sPduTemp <= (s8)gGHardPara_104[eGHardPara104_PduFanFullT]))
		 					{
		 						TempDuty = 30 * (u8)((Aver5sPduTemp - (s8)gGHardPara_104[eGHardPara104_PduFanOpenT]) / (((s8)gGHardPara_104[eGHardPara104_PduFanFullT] - (s8)gGHardPara_104[eGHardPara104_PduFanOpenT])*1.0));
		 						if(TempDuty > 30)
		 							TempDuty = 30;
		 						PduFanDuty = 30 + TempDuty;
		 					}
		 					else
		 					{
		 						PduFanDuty = 100;
		 					}
		 				}
		 				sIsPduFanStarted = 1;
		 			}
		 			else if(sIsPduFanStarted == 1)
		 			{
		 				TempDuty =  (Aver5sPduTemp - ((s8)gGHardPara_104[eGHardPara104_PduFanOpenT] - 5));
		 				if(TempDuty > 5)
		 					TempDuty = 5;
		 				PduFanDuty = 25 + TempDuty;

		 			}
	 				else
	 				{
	 					PduFanDuty = 0;
	 				}
		 		}
		 		else
		 		{
		 			PcsFanDuty = 100;
		 			PduFanDuty = 100;
		 		}
		 	sCtrlStep = 2;
	 		}
		break;
	 	case 2:
			SendCanImeu3PwmCtrlMsg241(PcsFanDuty,PduFanDuty);
			sCtrlStep = 1;
	 	break;
	 	default:
		 	sCtrlStep = 0;
	 	break;
	 }
}
/*=============================================================================================
** @Name      : static void TacShutFanCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : DE除湿机湿度控制任务
** @The notes : 1s任务,通过CAN通信控制IMEU3执行
**===========================================================================================*/
static void TacHumManageTask(void)
{
	 static u8 sCtrlStep = 0;  //控制步骤
	 switch(sCtrlStep)
	 {
	 	 case 0:
		 if(BSPGetOSMillSecTimeAPI() >= 3000)
		 {
		 	sCtrlStep = 1;
		 }
	 	 break;
	 	 case 1:
		 /*调试模式直接控制*/
		 if(eMODE_DEBUG == GetGWorkModeAPI())
		 {
			 sCtrlStep = 0;
		 }
		 else
		 {
		 	 /*PDU外IMEU3以及除湿机通信无通信中断*/
		 	 if((0 == DiagGetPduOutImeu3OffCommuStateAPI()) && (BitGet(gImeu3PduOutInfo_219[ePduOut219_Alarm1],3) == 0))
		 	 {
		 		 /*A2415 A2445 A2516 A2415降本项目*/
		 		 if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
				 || (gGHardPara_104[eGHardPara104_ProjectCode] == 5) || (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
				 || (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
		 		 {
			 		 /*读取的配置信息和下发的设置信息不一样*/
			 		 if((gImeu3DeInfo_220[eDeInfo220_OpenDeHum] != gGHardPara_104[eGHardPara104_AirDeHumOn_H] * 10) || (gImeu3DeInfo_220[eDeInfo220_CloseDeHum] != gGHardPara_104[eGHardPara104_AirDeHumOff_H] * 10) || ((gImeu3DeInfo_220[eDeInfo220_State] & 0x08) != 0))
			 		 {
			 			SendCanImeu3DeParaMsg245(gGHardPara_104[eGHardPara104_AirDeHumOn_H],gGHardPara_104[eGHardPara104_AirDeHumOff_H],0xff);
			 		 }
		 		 }
		 		 /*A2419 A2425 Y2403项目*/
		 		 else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
				 || (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
		 		 {
		 			 /*由第一组BMS控制，第二组不进行控制*/
		 			if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
		 			{
				 		 /*读取的配置信息和下发的设置信息不一样*/
				 		 if((gImeu3DeInfo_220[eDeInfo220_OpenDeHum] != gGHardPara_104[eGHardPara104_AirDeHumOn_H] * 10) || (gImeu3DeInfo_220[eDeInfo220_CloseDeHum] != gGHardPara_104[eGHardPara104_AirDeHumOff_H] * 10) || ((gImeu3DeInfo_220[eDeInfo220_State] & 0x08) != 0))
				 		 {
				 			SendCanImeu3DeParaMsg245(gGHardPara_104[eGHardPara104_AirDeHumOn_H],gGHardPara_104[eGHardPara104_AirDeHumOff_H],0xff);
				 		 }
		 			}
		 		 }
		 		 //默认A2415项目
		 		 else
		 		 {
			 		 /*读取的配置信息和下发的设置信息不一样*/
			 		 if((gImeu3DeInfo_220[eDeInfo220_OpenDeHum] != gGHardPara_104[eGHardPara104_AirDeHumOn_H] * 10) || (gImeu3DeInfo_220[eDeInfo220_CloseDeHum] != gGHardPara_104[eGHardPara104_AirDeHumOff_H] * 10) || ((gImeu3DeInfo_220[eDeInfo220_State] & 0x08) != 0))
			 		 {
			 			SendCanImeu3DeParaMsg245(gGHardPara_104[eGHardPara104_AirDeHumOn_H],gGHardPara_104[eGHardPara104_AirDeHumOff_H],0xff);
			 		 }
		 		 }
		 	 }
		 	 sCtrlStep = 0;
		 }
	 	 break;
	 	 default:
	     break;
	 }
}
//#endif
//#if(BMU_LIQUIDCOOL_AIR_485_EN == 1)
/*=================================================================================================
** @Name      : static void TacLiquidCoolAirCtrlTask(void)
** @Input     : void
** @Output    : void
** @Function  : 液冷空调控制任务
** @The notes : 1s,通过RS485通信控制液冷空调控制板执行
**===============================================================================================*/
//#if(BMU_LIQUIDCOOL_AIR_TYPE == 0)
typedef enum
{
	eAirConnStep_InquControlInfo  = 0,		/*查询外机控制信息*/
	eAirConnStep_InquStateInfo = 1,			/*查询外机状态信息*/
	eAirConnStep_InquBITInfo = 2,			/*查询BIT状态量*/
	eAirConnStep_SetTactics = 3,			/*设置空调策略*/
	eAirConnStep_SetModeTemp = 4,			/*设置工作模式和温度温差等*/
	eAirConnStep_SetPowerOn = 5,			/*设置开机*/
	eAirConnStep_SetPowerOff = 6,			/*设置关机*/
}e_AirConnStep;
//#else
typedef enum
{
	eTfAirConnStep_InquUserParaInfo  = 0,   /*查询用户参数信息*/
	eTfAirConnStep_InquStateInfo1 = 1,		/*查询外机状态1信息*/
	eTfAirConnStep_InquStateInfo2 = 2,		/*查询外机状态2信息*/
	eTfAirConnStep_InquStateInfo3 = 3,		/*查询外机状态3信息*/
	eTfAirConnStep_InquStateInfo4 = 4,		/*查询外机状态4信息*/
	eTfAirConnStep_InquStateInfo5 = 5,		/*查询外机状态5信息*/
	eTfAirConnStep_InquErrInfo1 = 6,		/*查询故障信息1*/
	eTfAirConnStep_InquErrInfo2 = 7,		/*查询故障信息1*/
	eTfAirConnStep_SetTactics = 8,			/*设置空调策略*/
	eTfAirConnStep_SetModeTemp = 9,		    /*设置工作模式和温度温差等*/
	//eTfAirConnStep_SetOnOffCmd = 10,        /*设置空调报警复位*/
	eTfAirConnStep_SetStartMode = 10,		/*设置启动模式*/
}e_TfAirConnStep;
typedef enum
{
	eEu3AirStep_InquCtrlInfo1 = 0,				/*查询控制信息1（设置开关机-设置制热温度）*/
	eEu3AirStep_InquCtrlInfo2 = 1,				/*查询控制信息2（制冷启动温差-制热停止温差）*/
	eEu3AirStep_InquStateInfo1 = 2,   			/*查询状态信息1（程序软件版本-能力总输出）*/
	eEu3AirStep_InquStateInfo2 = 3,				/*查询状态信息2（压缩机目标频率-压缩机运行阶段）*/
	eEu3AirStep_InquStateInfo3 = 4,		    	/*查询状态信息3（故障等级）*/
	eEu3AirStep_InquStateInfo4 = 5,				/*查询状态信息4（排气温度-环境温度）*/
	eEu3AirStep_InquStateInfo5 = 6,				/*查询状态信息5（高压压力-空调冷却水出水压力）*/
	eEu3AirStep_InquStateInfo6 = 7,				/*查询状态信息6（故障信息）*/
	eEu3AirStep_SetTactics = 8,					/*设置空调策略*/
	eEu3AirStep_SetModeTemp = 9,				/*设置工作模式和温度*/
	eEu3AirStep_SetTempDiff = 10,		    	/*制冷制热温差等*/
	eEu3AirStep_InquPwmInfo = 11,		    	/*查询电池水泵，模块水泵，驱动器散热风机、风机频率和占空比信息*/
	eEu3AirStep_ManualControlCmd = 12,          /*手动设置电池水泵、模块水泵、电子膨胀阀、风机、压缩机、电加热、压缩器散热风机开启及转速频率等*/
}e_Eu3AirStep;

typedef enum
{
	eXinMiaoAirStep_InquStateInfo = 0,			/*查询状态信息*/
	eXinMiaoAirStep_InquCtrlInfo1 = 1,			/*查询控制信息1（机组控制模式-开关机）*/
	eXinMiaoAirStep_InquCtrlInfo2 = 2,			/*查询控制信息2（机组设定温度）*/
	eXinMiaoAirStep_SetTactics = 3,				/*设置空调策略*/
	eXinMiaoAirStep_SetModePwr = 4,			    /*设置工作模式和温度*/
	eXinMiaoAirStep_SetTemp = 5,		        /*设定机组温度*/
}e_XinMiaoAirStep;
//#endif
u16 Eu3AirSetBuff1[7] = {0x00};		//设置空调缓冲区
s16 Eu3AirSetBuff2[4] = {0x00};		//设置空调缓冲区
u16 Eu3AirManualControlBuff3[16] = {0x00}; //设置空调缓冲区
static void TacLiquidCoolAirCtrlTask(void)
{
	u8 i = 0;
	static u8 FirstPowerOnFlag = 0;
	//#if(BMU_LIQUIDCOOL_AIR_TYPE == 0)
	static u8 sAirModeSet = 0;			/*设置空调模式(0:待机模式 1:自循环模式 2:制冷模式 3:制热模式 4:自动模式)*/
	static u8 sCtrlStep = 0;  			/*空调执行步骤*/

	static u16 AirColdTemp = 250;		/*制冷设置温度，25℃*/
	static u16 AirColdOnDiff = 2;		/*制冷启动回差， 2℃*/
	static u16 AirColdOffDiff = 3;		/*制冷停止回差，3℃*/
	static u16 AirHeatTemp = 100;		/*制热设置温度，10℃*/
	static u16 AirHeatDiff = 30;		/*制热开启和停止回差，3℃*/
	u16 AirSetBuff[7] = {0x00};			//设置工作模式，制冷温度，启动温差，停止温差，制热温度，制热温差、开关机命令\报警复位
	static u16 TfControlObj = 0;		/*控制供水*/
	static u16 TfAirColdTemp = 25;		/*制冷设置温度，25℃*/
	static u16 TfAirColdDiff = 2;		/*制冷回差， 2℃*/
	static u16 TfAirHeatTemp = 10;		/*制热设置温度，10℃*/
	static u16 TfAirHeatDiff = 3;		/*制热回差，3℃*/
	static u8 StepControl = 0;          /*加快控制*/

	static u8 Eu3CellPower = 0;			/*EU3空调电池测开关机控制(BMS控制)*/
	static u8 Eu3CellPower_Mu2 = 1;   	/*EU3空调电池测开关机控制(MU2控制)*/
	static u8 Eu3PcsPower = 0;			/*EU3空调模块侧开关机控制*/
	static u8 Eu3AirModeSet = 0;		/*设置EU3空调模式(1：制冷模式 2：制热模式 3：自循环模式 4：自动模式 5：单独运行模块侧水路模式)*/
	static s16 Eu3AirControlObj = 0;	/*控制出水温度*/
	static s16 Eu3AirColdTmep = 250;    /*制冷设置温度，25℃*/
	static s16 Eu3AirHeatTmep = 100;    /*制热设置温度，25℃*/
	static s16 Eu3ColdOpenDiff = 20;    /*制冷开启温差，2℃*/
	static s16 Eu3ColdCloseDiff = -30;  /*制冷关闭温差，-3℃*/
	static s16 Eu3HeatOpenDiff = 30;    /*制热开启温差，3℃*/
	static s16 Eu3HeatCloseDiff = -20;  /*制热关闭温差，-2℃*/
	//static u16 last_sent_cmd = 0xFFFF;    	// 增加静态变量记录上一次发送的命令值，初始值设为一个无效值(0xFFFF)

	static u8 XinMiaoPower = 0;         /*新淼空调开关机控制*/
	static u8 XinMiaoMode = 0;          /*新淼空调模式控制*/
	static u16 XinMiaoTemp = 0;          /*新淼空调温度设置*/
	u16 XinMiaoAirSetBuff[2] = {0x00};	//新淼空调设置缓冲区
	Eu3PcsPower = MU2PcsAirCmd;
	Eu3CellPower_Mu2 = MU2CellAirCmd_Ctrl;    //EU3空调电池侧液路开关机控制
    //BMS初始化时等待空调信息不做控制
    // if((eWORK_INIT == GetGWorkStateAPI())          //系统开机
    // 	|| (eWORK_OFF == GetGWorkStateAPI()))      //系统关机
	// {
    // 	sCtrlStep = 0;
	// 	return; 
	// }
    if(gGHardPara_104[eGHardPara104_AirType] == 1)//弓通空调
    {
    	switch(sCtrlStep)
    	{
    		/*查询开关机状态，模式设置和制冷制热温度，温差冷媒类型等信息*/
    		case eAirConnStep_InquControlInfo:
    		(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_AIR_WR_BASE_ADDR, 10, HEAT_MANANG_MODBUS_PORT);
    		if((BSPGetOSMillSecTimeAPI() >= 5000) || (FirstPowerOnFlag == 1))
    		{
    			FirstPowerOnFlag = 1;
    			/*液冷空调通信中断*/
    			if(1 == DiagGetPerAirOffCommuStateAPI())
    			{
    				sCtrlStep = 0;
    				TacCtrlGroupSwhStateAPI(eGroupSwh_Sta,ePSTATE_ON);
    			}
    			/*通信正常*/
    			else
    			{
    				TacCtrlGroupSwhStateAPI(eGroupSwh_Sta,ePSTATE_OFF);
    				/*关机状态*/
    				if(gLiqudCoolAirInfo_206[eLiquidAirInfo206_PwrState] == 0)
    				{
    					sCtrlStep = eAirConnStep_SetPowerOn;//发送开机指令
    				}
    				/*开机状态*/
    				else
    				{
    					sCtrlStep = eAirConnStep_InquStateInfo;
    				}
    			}
    		}
    		break;
    		/*查询外机机型信息和状态信息*/
    		case eAirConnStep_InquStateInfo:
    		(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x04, MB_AIR_RO_BASE_ADDR, 26, HEAT_MANANG_MODBUS_PORT);
    		sCtrlStep = eAirConnStep_InquBITInfo;
    		break;
    		break;
    		/*查询状态量（BIT）*/
    		case eAirConnStep_InquBITInfo:
    		(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x01, MB_AIR_RO_BASE1_ADDR, 46, HEAT_MANANG_MODBUS_PORT);
    		sCtrlStep = eAirConnStep_SetTactics;
    		break;
    		/*根据系统状态制冷空调运行策略*/
    		case eAirConnStep_SetTactics:
    		//从控信息就绪，处于空闲状态
    		if((TRUE == TacGetGAllSlaveReadyHook()) &&  (eCURR_IDLE == GetGChgDhgStateAPI()))
    		{
    			/*设置制冷开启温度为30℃，平均温度超过35度或者最大温度超过37℃*/
    			if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 300;//设置制冷温度为30℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
				/*最小单体温度小于15℃或者平均小于17℃*/
    			else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
        		|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirHeatTemp = 150;//设置制热温度为10℃
    				AirHeatDiff = 30;//设置加热温差
    			}
    			else
    			{
    				/*待机模式*/
    				sAirModeSet = 0;	//待机模式
    			}
    		}
    		//从控信息就绪，处于工作状态
    		else if((TRUE == TacGetGAllSlaveReadyHook()) &&  (eCURR_IDLE != GetGChgDhgStateAPI()))
    		{
    			/*设置制冷温度为25℃，平均温度超过26度或者最大温度超过27℃且电流超过250A*/
    			if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
    			 && ((sAvgCurr1min <= -3000)||(sAvgCurr1min >= 3000)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 180;//设置制冷温度为18℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为25℃，平均温度超过35度或者最大温度超过40℃且电流超过200A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 15)))
    			 && ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 180;//设置制冷温度为18℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为25℃，平均温度超过32度或者最大温度超过35℃且电流超过200A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10)))
    			 && ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 190;//设置制冷温度为19℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为25℃，平均温度超过29度或者最大温度超过31℃且电流超过200A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 4))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 6)))
    			 && ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 200;//设置制冷温度为20℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为25℃，平均温度超过26度或者最大温度超过27℃且电流超过200A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
    			 && ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 210;//设置制冷温度为21℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为25℃，平均温度超过26度或者最大温度超过27℃且电流超过200A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
    			 && ((sAvgCurr1min <= -2000)||(sAvgCurr1min >= 2000)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 220;//设置制冷温度为22℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为20℃，平均温度超过35度或者最大温度超过38℃且电流超过150A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
    			 && ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 200;//设置制冷温度为20℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为23℃，平均温度超过27度或者最大温度超过29℃且电流超过150A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 4)))
    			 && ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 230;//设置制冷温度为23℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为20℃，平均温度超过35度或者最大温度超过38℃且电流超过95A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
    			 && ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 200;//设置制冷温度为20℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为24℃，平均温度超过27度或者最大温度超过29℃且电流超过95A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 4)))
    			 && ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 240;//设置制冷温度为24℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为22℃，平均温度超过35度或者最大温度超过38℃且电流超过55A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
    			 && ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 220;//设置制冷温度为22℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为25℃，平均温度超过29度或者最大温度超过31℃且电流超过55A*/
    			else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 4))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 6)))
    			 && ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 250;//设置制冷温度为25℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为24℃，平均温度超过30度或者最大温度超过32℃*/
    			else if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 240;//设置制冷温度为24℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
    			/*设置制冷温度为25℃，平均温度超过28度或者最大温度超过30℃*/
    			else if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 3))
    			 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5)))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirColdTemp = 250;//设置制冷温度为25℃
    				AirColdOnDiff = 2;//设置开启制冷温差为2℃
    				AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			}
				/*最小单体温度小于15℃或者平均小于17℃*/
    			else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
        		|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
    			{
    				sAirModeSet = 4;//设置为自动模式
    				AirHeatTemp = 150;//设置制热温度为10℃
    				AirHeatDiff = 30;//设置加热温差
    			}
    			else
    			{
					if(gGHardPara_104[eGHardPara104_ProjectCode] == 0)
					{
						sAirModeSet = 0;	//待机模式
					}
					else if(gGHardPara_104[eGHardPara104_ProjectCode] == 1)
					{
						sAirModeSet = 4;//设置为自动模式
					}
					else
					{
						sAirModeSet = 0;	//待机模式
					}
    				AirColdTemp = 280; //设置制冷温度为28℃
    				AirHeatTemp = 100; //设置制热温度为10℃
    				AirHeatDiff = 30;//设置加热温差
    			}
    		}
    		//从控信息未就绪
    		else
    		{
    			sAirModeSet = 0;	//设置为待机模式
    			AirColdTemp = 270; //设置制冷温度为28℃
    			AirHeatTemp = 100; //设置制热温度为10℃
    			AirColdOnDiff = 2;//设置开启制冷温差为2℃
    			AirColdOffDiff = 3;//设置关闭制冷温差为3℃
    			AirHeatDiff = 30;//设置加热温差
    		}
    		sCtrlStep = eAirConnStep_SetModeTemp;
    		break;
    		/*发送开机指令*/
    		case eAirConnStep_SetPowerOn:
    		(void)MbClientSendWriteSingReqAPI(MODBUS_SALVE_ADDR_AIR, 0x06, MB_AIR_WR_BASE_ADDR, 1, HEAT_MANANG_MODBUS_PORT);
    		sCtrlStep = 0;
    		break;
    		/*发送关机指令*/
    		case eAirConnStep_SetPowerOff:
    		(void)MbClientSendWriteSingReqAPI(MODBUS_SALVE_ADDR_AIR, 0x06, MB_AIR_WR_BASE_ADDR, 0, HEAT_MANANG_MODBUS_PORT);
    		sCtrlStep = 0;
    		break;
    		/*发送模式设置指令*/
    		case eAirConnStep_SetModeTemp:
        	AirSetBuff[0] = sAirModeSet;  	//工作模式
        	AirSetBuff[1] = AirColdTemp;  	//制冷温度
        	AirSetBuff[2] = AirColdOnDiff;	//制冷开启温度
        	AirSetBuff[3] = AirColdOffDiff; //制冷停止温差
        	AirSetBuff[4] = AirHeatTemp;    //制热温度
        	AirSetBuff[5] = AirHeatDiff;    //制热温差
    		for(i = 0; i < 6; i++)
    		{
    			/*设置内容有更新,发送设置指令*/
    			if(AirSetBuff[i] != gLiqudCoolAirInfo_206[i + 1])
    			{
    				(void)MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_AIR, MB_AIR_WR_BASE_ADDR + 1, AirSetBuff, 6,HEAT_MANANG_MODBUS_PORT);
    				break;
    			}
    		}
    		sCtrlStep = eAirConnStep_InquControlInfo;
    		break;

    		default:
    		sCtrlStep = eAirConnStep_InquControlInfo;
    		break;
    	}
    }

	//#else


else if(gGHardPara_104[eGHardPara104_AirType] == 2)//TF空调
	{
			switch(sCtrlStep)
			{
				/*查询用户参数*/
				case eTfAirConnStep_InquUserParaInfo:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_TF_AIR_WR_BASE_ADDR, 7, HEAT_MANANG_MODBUS_PORT);
				if((BSPGetOSMillSecTimeAPI() >= 5000) || (FirstPowerOnFlag == 1))
				{
					FirstPowerOnFlag = 1;
					/*液冷空调通信中断*/
					if(1 == DiagGetPerAirOffCommuStateAPI())
					{
						sCtrlStep = eTfAirConnStep_InquUserParaInfo;
					}
					/*液冷空调通信正常*/
					else
					{
						/*启动模式为手动，设置为自动模式*/
						if(gTFAirUserParaInfo_212[eATFUserInfo212_StartMode] == 0)
						{
							sCtrlStep = eTfAirConnStep_SetStartMode;//设置启动模式
						}
						/*自动模式*/
						else
						{
							if(StepControl == 0)
							{
								sCtrlStep = eTfAirConnStep_InquStateInfo1;
								StepControl = 1;
							}
							else if(StepControl == 1)
							{
								sCtrlStep = eTfAirConnStep_InquStateInfo4;
								StepControl = 2;
							}
							else if(StepControl == 2)
							{
								sCtrlStep = eTfAirConnStep_InquErrInfo1;
								StepControl = 0;
							}
						}
					}
				}
				break;
				/*查询运行状态、开关量输入、继电器输出、供液温度，回液温度、系统蒸进温度*/
				case eTfAirConnStep_InquStateInfo1:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_TF_AIR_RO_BASE_ADDR, 6, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eTfAirConnStep_InquStateInfo2;
				break;
				/*查询系统蒸出温度、环境温度和系统冷凝温度*/
				case eTfAirConnStep_InquStateInfo2:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_TF_AIR_RO_BASE1_ADDR, 3, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eTfAirConnStep_InquStateInfo3;
				break;
				/*查询节流电膨当前开度、水泵当前转速、压缩机当前转速、风机转速*/
				case eTfAirConnStep_InquStateInfo3:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_TF_AIR_RO_BASE2_ADDR, 4, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eTfAirConnStep_SetTactics;
				break;
				/*查询供水压力、回水压力*/
				case eTfAirConnStep_InquStateInfo4:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_TF_AIR_RO_BASE3_ADDR, 2, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eTfAirConnStep_InquStateInfo5;
				break;
				/*查询排气压力*/
				case eTfAirConnStep_InquStateInfo5:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_TF_AIR_RO_BASE4_ADDR, 1, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eTfAirConnStep_SetTactics;
				break;
				/*查询故障信息1*/
				case eTfAirConnStep_InquErrInfo1:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_TF_AIR_RO_BASE5_ADDR, 8, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eTfAirConnStep_InquErrInfo2;
				break;
				/*查询故障信息2*/
				case eTfAirConnStep_InquErrInfo2:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_TF_AIR_RO_BASE6_ADDR, 5, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eTfAirConnStep_SetTactics;
				break;
				case eTfAirConnStep_SetTactics:
				//从控信息就绪，处于空闲状态
				if((TRUE == TacGetGAllSlaveReadyHook()) &&  (eCURR_IDLE == GetGChgDhgStateAPI()))
				{
					if(MU2_TFAir_TypeChoose_Cmd == 0)       //长寿命模式
					{
						/*设置制冷开启温度为25℃，平均温度超过30度或者最大温度超过32℃*/
						if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5))
							|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 25;//设置制冷温度为25℃
							TfAirColdDiff = 3; //设置制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*最小单体温度小于15℃或者平均小于17℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
						|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirHeatTemp = 15;//设置制热温度为15℃
							TfAirHeatDiff = 3;//设置加热温差
							TfControlObj= 0;   //设置供回水目标为供水
						}
						else
						{
							/*待机模式*/
							sAirModeSet = 0;	//待机模式
							TfControlObj= 0;   //设置供回水目标为供水
						}
					}
					else                    //高收益模式
					{
						/*设置制冷开启温度为30℃，平均温度超过35度或者最大温度超过37℃*/
						if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 30;//设置制冷温度为30℃
							TfAirColdDiff = 3; //设置制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*最小单体温度小于15℃或者平均小于17℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
						|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirHeatTemp = 15;//设置制热温度为15℃
							TfAirHeatDiff = 3;//设置加热温差
							TfControlObj= 0;   //设置供回水目标为供水
						}
						else
						{
							/*待机模式*/
							sAirModeSet = 0;	//待机模式
							TfControlObj= 0;   //设置供回水目标为供水
						}
					}
				}
				//从控信息就绪，处于工作状态
				else if((TRUE == TacGetGAllSlaveReadyHook()) &&  (eCURR_IDLE != GetGChgDhgStateAPI()))
				{
					if(MU2_TFAir_TypeChoose_Cmd == 0)       //长寿命模式
					{
						/*设置制冷温度为20℃，平均温度超过35度或者最大温度超过38℃且电流超过150A*/
						if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
							|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
							&& ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 18;//设置制冷温度为20℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为20℃，平均温度超过27度或者最大温度超过29℃且电流超过150A*/
						if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 4)))
						&& ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 20;//设置制冷温度为20℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为20℃，平均温度超过35度或者最大温度超过38℃且电流超过95A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 20;//设置制冷温度为20℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为22℃，平均温度超过27度或者最大温度超过29℃且电流超过95A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 4)))
						&& ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 22;//设置制冷温度为22℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为22℃，平均温度超过35度或者最大温度超过38℃且电流超过55A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 22;//设置制冷温度为22℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为25℃，平均温度超过29度或者最大温度超过31℃且电流超过55A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 4))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 6)))
						&& ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 25;//设置制冷温度为25℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为25℃，平均温度超过30度或者最大温度超过32℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 24;//设置制冷温度为24℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为25℃，平均温度超过28度或者最大温度超过30℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 3))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 25;//设置制冷温度为25℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*平均温度小于15℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) <= 15))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirHeatTemp = 15;//设置制热温度为15℃
							TfAirHeatDiff = 3;//设置加热温差
							TfControlObj= 0;   //设置供回水目标为供水
						}
						else
						{
							if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
							|| (gGHardPara_104[eGHardPara104_ProjectCode] == 6) || (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
							{
								sAirModeSet = 0;	//待机模式
							}
							else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
							|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
							{
								sAirModeSet = 4;//设置为自动模式
							}
							else
							{
								sAirModeSet = 0;	//待机模式
							}
							TfAirColdTemp = 25;   //设置制冷温度为25℃
							TfAirColdDiff = 3;	//设置制冷温差
							TfAirHeatTemp = 15;   //设置制热温度为15℃
							TfAirHeatDiff = 3;	//设置加热温差
							TfControlObj= 0;   //设置供回水目标为供水
						}
					}
					else                    //高收益模式
					{ 
						/*设置制冷温度为20℃，平均温度超过35度或者最大温度超过38℃且电流超过150A*/
						if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 18;//设置制冷温度为18℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为20℃，平均温度超过27度或者最大温度超过29℃且电流超过150A*/
						if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7)))
						&& ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 20;//设置制冷温度为20℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为20℃，平均温度超过35度或者最大温度超过38℃且电流超过95A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 20;//设置制冷温度为20℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为22℃，平均温度超过33度或者最大温度超过35℃且电流超过95A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 8))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10)))
						&& ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 22;//设置制冷温度为22℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为22℃，平均温度超过35度或者最大温度超过38℃且电流超过55A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 22;//设置制冷温度为22℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为25℃，平均温度超过29度或者最大温度超过31℃且电流超过55A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 8))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10)))
						&& ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 24;//设置制冷温度为24℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*设置制冷温度为25℃，平均温度超过35度或者最大温度超过37℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12)))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirColdTemp = 27;//设置制冷温度为27℃
							TfAirColdDiff = 3;//设置开启制冷温差为3℃
							TfControlObj= 0;   //设置供回水目标为供水
						}
						/*最小单体温度小于15℃或者平均小于17℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
						|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
						{
							sAirModeSet = 4;//设置为自动模式
							TfAirHeatTemp = 15;//设置制热温度为15℃
							TfAirHeatDiff = 3;//设置加热温差
							TfControlObj= 0;   //设置供回水目标为供水
						}
						else
						{
							if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
							|| (gGHardPara_104[eGHardPara104_ProjectCode] == 6) || (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
							{
								sAirModeSet = 0;	//待机模式
							}
							else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
							|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
							{
								sAirModeSet = 4;//设置为自动模式
							}
							else
							{
								sAirModeSet = 0;	//待机模式
							}
							TfAirColdTemp = 25;   //设置制冷温度为25℃
							TfAirColdDiff = 3;	//设置制冷温差
							TfAirHeatTemp = 15;   //设置制热温度为15℃
							TfAirHeatDiff = 3;	//设置加热温差
							TfControlObj= 0;   //设置供回水目标为供水
						}
					}
				}
				//从控信息未就绪
				else
				{
					if(MU2_TFAir_TypeChoose_Cmd == 0)       //长寿命模式
					{
						sAirModeSet = 0;	//待机模式
						TfAirColdTemp = 25;   //设置制冷温度为25℃
						TfAirHeatTemp = 15;   //设置制热温度为15℃
						TfAirColdDiff = 3;	//设置制冷温差
						TfAirHeatDiff = 3;	//设置加热温差
						TfControlObj= 0;   //设置供回水目标为供水
					}
					else                                            //高收益模式
					{
						sAirModeSet = 0;	//待机模式
						TfAirColdTemp = 30;   //设置制冷温度为30℃
						TfAirHeatTemp = 15;   //设置制热温度为15℃
						TfAirColdDiff = 3;	//设置制冷温差
						TfAirHeatDiff = 3;	//设置加热温差
						TfControlObj= 0;   //设置供回水目标为供水
					}

				}
				sCtrlStep = eTfAirConnStep_SetModeTemp;
				break;
				case eTfAirConnStep_SetModeTemp:
				if(eMODE_DEBUG == GetGWorkModeAPI())
				{
					//1:强制制热  2:强制制冷 3：自循环  4：待机
					if(gAirDebugCmd == 1)//强制制热
					{
						sAirModeSet = 2;
						//40℃以上不允许开启制热
						if(gTFAirStateInfo_213[eATFUserState213_FeedTemp] <= 400)
						{
							TfAirHeatTemp = gTFAirStateInfo_213[eATFUserState213_FeedTemp]/10 + 10;
							TfAirColdTemp = TfAirHeatTemp + 5;
							TfAirColdDiff = 3;
							TfAirHeatDiff = 3;
						}
					}
					else if(gAirDebugCmd == 2)//强制制冷
					{
						sAirModeSet = 1;
						//10℃以下不允许制冷
						if(gTFAirStateInfo_213[eATFUserState213_FeedTemp] >= 100)
						{
							TfAirColdTemp = gTFAirStateInfo_213[eATFUserState213_FeedTemp]/10 - 10;
							TfAirHeatTemp = 0;
							TfAirColdDiff = 3;
							TfAirHeatDiff = 3;
						}
					}
					else if(gAirDebugCmd == 3)//自循环
					{
						sAirModeSet = 3;
					}
					else if(gAirDebugCmd == 4)//待机
					{
						sAirModeSet = 0;
					}
				}
				AirSetBuff[0] = sAirModeSet;  	//工作模式
				AirSetBuff[1] = TfControlObj;  	//供回水目标
				AirSetBuff[2] = TfAirColdTemp;  	//制冷温度
				AirSetBuff[3] = TfAirColdDiff;	//制冷温差
				AirSetBuff[4] = TfAirHeatTemp;    //制热温度
				AirSetBuff[5] = TfAirHeatDiff;    //制热温差
				//AirSetBuff[6] = MU2_TFAirON_OFF_ResetCmd;    //设置TF空调开机0x01、关机0x02、报警复位0x04命令；
				for(i = 0; i < 6; i++)
				{
					/*设置内容有更新,发送设置指令*/
					if(AirSetBuff[i] != gTFAirUserParaInfo_212[i + 1])
					{
						(void)MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_AIR, MB_TF_AIR_WR_BASE_ADDR + 1, AirSetBuff, 6,HEAT_MANANG_MODBUS_PORT);
						break; 
					}
				}
				// sCtrlStep = eTfAirConnStep_SetOnOffCmd;
				// break;
				// //by lxr01202    新增空调报警复位
				// case eTfAirConnStep_SetOnOffCmd:
				// 	if(MU2_TFAirON_OFF_ResetCmd != 0 && MU2_TFAirON_OFF_ResetCmd != last_sent_cmd)
				// 	{
				// 		(void)MbClientSendWriteSingReqAPI(MODBUS_SALVE_ADDR_AIR, 0x06, 0x300, MU2_TFAirON_OFF_ResetCmd, HEAT_MANANG_MODBUS_PORT);
				// 		last_sent_cmd = MU2_TFAirON_OFF_ResetCmd;
				// 	}
				sCtrlStep = eTfAirConnStep_InquUserParaInfo;
			    break;
				case eTfAirConnStep_SetStartMode:
				(void)MbClientSendWriteSingReqAPI(MODBUS_SALVE_ADDR_AIR, 0x06, MB_TF_AIR_WR_BASE_ADDR, 1, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eTfAirConnStep_InquUserParaInfo;
				break;
				default:
				sCtrlStep = eTfAirConnStep_InquUserParaInfo;
				break;
			}
	}
else if(gGHardPara_104[eGHardPara104_AirType] == 3)//自研EU3空调
	{
		switch(sCtrlStep)
		{
		 	/*查询控制信息1（设置开关机-设置制热温度-降噪模式）*/
			case eEu3AirStep_InquCtrlInfo1:
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_EU3_AIR_WR_BASE_ADDR, 7, HEAT_MANANG_MODBUS_PORT);
			if((BSPGetOSMillSecTimeAPI() >= 5000) || (FirstPowerOnFlag == 1))
			{
				FirstPowerOnFlag = 1;
				/*液冷空调通信中断*/
				if(1 == DiagGetPerAirOffCommuStateAPI())
				{
					sCtrlStep = eEu3AirStep_InquCtrlInfo1;
				}
				/*液冷空调通信正常*/
				else
				{
					/*加快控制策略*/
					if(StepControl == 0)
					{
						StepControl = 1;
						sCtrlStep = eEu3AirStep_InquCtrlInfo2;
					}
					else if(StepControl == 1)
					{
						StepControl = 2;
						sCtrlStep = eEu3AirStep_InquStateInfo2;
					}
					else if(StepControl == 2)
					{
						StepControl = 0;
						sCtrlStep = eEu3AirStep_InquStateInfo5;
					}
				}
			}
			break;
			/*查询控制信息2（制冷启动温差-制热停止温差）*/
			case eEu3AirStep_InquCtrlInfo2:
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_EU3_AIR_WR_BASE1_ADDR, 4, HEAT_MANANG_MODBUS_PORT);
			sCtrlStep = eEu3AirStep_InquStateInfo1;
			break;
			/*查询状态信息1（程序软件版本-能力总输出）*/
			case eEu3AirStep_InquStateInfo1:
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x04, MB_EU3_AIR_RO_BASE_ADDR, 11, HEAT_MANANG_MODBUS_PORT);
			sCtrlStep = eEu3AirStep_SetTactics;
			break;
			/*查询状态信息2（压缩机目标频率-压缩机运行阶段）*/
			case eEu3AirStep_InquStateInfo2:
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x04, MB_EU3_AIR_RO_BASE1_ADDR, 3, HEAT_MANANG_MODBUS_PORT);
			sCtrlStep = eEu3AirStep_InquStateInfo3;
			break;
			 /*查询状态信息3（故障等级）*/
			case eEu3AirStep_InquStateInfo3:
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x04, MB_EU3_AIR_RO_BASE2_ADDR, 1, HEAT_MANANG_MODBUS_PORT);
			sCtrlStep = eEu3AirStep_InquStateInfo4;
			break;
			/*查询状态信息4（排气温度-环境温度）*/
			case eEu3AirStep_InquStateInfo4:
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x04, MB_EU3_AIR_RO_BASE3_ADDR, 10, HEAT_MANANG_MODBUS_PORT);
			sCtrlStep = eEu3AirStep_SetTactics;
			break;
			/*查询状态信息5（高压压力-空调冷却水出水压力）*/
			case eEu3AirStep_InquStateInfo5:
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x04, MB_EU3_AIR_RO_BASE4_ADDR, 6, HEAT_MANANG_MODBUS_PORT);
			sCtrlStep = eEu3AirStep_InquStateInfo6;
			break;
			/*查询状态信息6（故障信息）*/
			case eEu3AirStep_InquStateInfo6:
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x04, MB_EU3_AIR_RO_BASE5_ADDR, 6, HEAT_MANANG_MODBUS_PORT);
			sCtrlStep = eEu3AirStep_InquPwmInfo;
			break;
			/*查询状态信息7（水泵信息）*/
			case eEu3AirStep_InquPwmInfo:
			(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x04, MB_EU3_AIR_RO_BASE6_ADDR, 8, HEAT_MANANG_MODBUS_PORT);
			sCtrlStep = eEu3AirStep_SetTactics;
			break;
			/*设置空调策略*/
			case eEu3AirStep_SetTactics:
	    	//从控信息就绪，处于空闲状态
	    	if((TRUE == TacGetGAllSlaveReadyHook()) &&  (eCURR_IDLE == GetGChgDhgStateAPI()))
	    	{
	    		/*设置制冷开启温度为35℃，平均温度超过35度或者最大温度超过37℃*/
	    		if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 300;//设置制冷温度为30℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
				/*最小单体温度小于15℃或者平均小于17℃*/
    			else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
        		|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirHeatTmep = 150;//设置制热温度为15℃
	    			Eu3HeatOpenDiff = 30;//设置加热启动温差3℃
	    			Eu3HeatCloseDiff = -30;//设置加热停止温差-3℃
	    		}

//最大单体温度与最小单体温度温差大于6℃
				else if((((GetGCellMaxTempAPI() + 5) / 10) - ((GetGCellMinTempAPI() + 5) / 10)) >= 6)
				{
					// 检查是否满足触发间隔：当前SInfy_Idletime1 - 上次触发SInfy_Idletime1 ≥ 1小时
					u32 triggerInterval = SInfy_Idletime1 - LastTriggerStimer;
					//  检查自循环是否已超30分钟运行（若正在运行则先判断超时）
					u32 runDuration = SInfy_Idletime1 - SelfLoopStartStimer;

					// 未在运行 且 触发间隔≥1小时 → 启动自循环
					if((IsSelfLoopRunning == 0) && ((LastTriggerStimer == 0) || (triggerInterval >= MIN_TRIG_INTERVAL)))
					{
						IsSelfLoopRunning = 1;          // 标记运行状态
						SelfLoopStartStimer = SInfy_Idletime1;   // 记录启动时间
						LastTriggerStimer = SInfy_Idletime1;     // 记录本次触发时间
						Eu3CellPower = 1;
						Eu3AirControlObj = 0;
						Eu3AirModeSet = 3;  //设置为自循环模式
					}
					// 若已在运行中，检查是否超30分钟 → 超时则强制停止
					else if((IsSelfLoopRunning == 1) && (runDuration >= MAX_RUN_SEC))
					{
						IsSelfLoopRunning = 0;          // 标记停止
						Eu3CellPower = 0;
						Eu3AirControlObj = 0;
						Eu3AirModeSet = 4;             // 超时强制切自动模式
					}
				}
				else if ((((GetGCellMaxTempAPI() + 5) / 10) - ((GetGCellMinTempAPI() + 5) / 10)) < 4)
				{
					IsSelfLoopRunning = 0;              // 标记停止
					Eu3CellPower = 0;
					Eu3AirControlObj = 0;
					Eu3AirModeSet = 4;  /*自动模式*/
				}
	   			else
	    		{
	   				Eu3CellPower = 0;
	   				Eu3AirControlObj = 0;
	   				Eu3AirModeSet = 4;	/*自动模式*/
	    		}
	    	}
	    	//从控信息就绪，处于工作状态
	    	else if((TRUE == TacGetGAllSlaveReadyHook()) &&  (eCURR_IDLE != GetGChgDhgStateAPI()))
	    	{
	    		/*设置制冷温度为18℃，平均温度超过35度或者最大温度超过37℃且电流超过250A*/
	    		if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
	    		 && ((sAvgCurr1min <= -3000)||(sAvgCurr1min >= 3000)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 180;//设置制冷温度为18℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为25℃，平均温度超过26度或者最大温度超过27℃且电流超过250A*/
	    		if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
	    		 && ((sAvgCurr1min <= -3000)||(sAvgCurr1min >= 3000)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 200;//设置制冷温度为20℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为19℃，平均温度超过35度或者最大温度超过38℃且电流超过200A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
	    		 && ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 190;//设置制冷温度为19℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为21℃，平均温度超过26度或者最大温度超过27℃且电流超过200A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
	    		 && ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 210;//设置制冷温度为21℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为20℃，平均温度超过35度或者最大温度超过38℃且电流超过200A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
	    		 && ((sAvgCurr1min <= -2000)||(sAvgCurr1min >= 2000)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 200;//设置制冷温度为20℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为22℃，平均温度超过26度或者最大温度超过27℃且电流超过200A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
	    		 && ((sAvgCurr1min <= -2000)||(sAvgCurr1min >= 2000)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 220;//设置制冷温度为22℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为20℃，平均温度超过35度或者最大温度超过38℃且电流超过150A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
	    		 && ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 200;//设置制冷温度为20℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为23℃，平均温度超过27度或者最大温度超过29℃且电流超过150A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7)))
	    		 && ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 230;//设置制冷温度为23℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为21℃，平均温度超过35度或者最大温度超过38℃且电流超过95A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
	    		 && ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 210;//设置制冷温度为21℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为24℃，平均温度超过27度或者最大温度超过29℃且电流超过95A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7)))
	    		 && ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 240;//设置制冷温度为24℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为22℃，平均温度超过35度或者最大温度超过38℃且电流超过55A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
	    		 && ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 220;//设置制冷温度为22℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*设置制冷温度为25℃，平均温度超过33度或者最大温度超过35℃且电流超过55A*/
	    		else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 8))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10)))
	    		 && ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 250;//设置制冷温度为25℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
	    		/*55A以下电流充放电，平均温度超过35度或者最大温度超过37℃，设置制冷温度为27℃，*/
	    		else if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
	    		 || (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12)))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirColdTmep = 270;//设置制冷温度为27℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    		}
				/*最小单体温度小于15℃或者平均小于17℃*/
    			else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
        		|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirControlObj = 0;
	    			Eu3AirModeSet = 4;//设置为自动模式
	    			Eu3AirHeatTmep = 150;//设置制热温度为15℃
	    			Eu3HeatOpenDiff = 30;//设置加热启动温差3℃
	    			Eu3HeatCloseDiff = -30;//设置加热停止温差-3℃
	    		}
	    		else
	    		{
					Eu3CellPower = 1;            //不满足任何条件  电池液路也要打开   by lxr01202_change at 20260116
					Eu3AirControlObj = 0;
					Eu3AirModeSet = 4;	/*自动模式*/
					Eu3AirColdTmep = 280; //设置制冷温度为28℃
					Eu3AirHeatTmep = 150; //设置制热温度为15℃
	    			Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    			Eu3ColdCloseDiff = -70;//设置关闭制冷温差为-7℃
	    			Eu3HeatOpenDiff = 30;//设置加热启动温差3℃
	    			Eu3HeatCloseDiff = -30;//设置加热停止温差-3℃
	    		}
	    	}
	    	//从控信息未就绪
	    	else
	    	{
	    		Eu3CellPower = 1;
	    		Eu3AirControlObj = 0;
	    		Eu3AirModeSet = 4;	/*自动模式*/
	    		Eu3AirColdTmep = 300; //设置制冷温度为30℃
	    		Eu3AirHeatTmep = 150; //设置制热温度为15℃
	    		Eu3ColdOpenDiff = 20;//设置开启制冷温差为2℃
	    		Eu3ColdCloseDiff = -70;//设置关闭制冷温差为7℃
    			Eu3HeatOpenDiff = 30;//设置加热启动温差3℃
    			Eu3HeatCloseDiff = -30;//设置加热停止温差-3℃
	    	}
	    	sCtrlStep = eEu3AirStep_SetModeTemp;
			break;
			case eEu3AirStep_SetModeTemp:
			if(eMODE_DEBUG == GetGWorkModeAPI())
			{
	    		//1:强制制热  2:强制制冷 3：自循环  4：待机
	    		if(gAirDebugCmd == 1)//强制制热
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirModeSet = 2;
	    			if(gEu3AirState_203[eEu3AirState203_CellInTemp] <= 400)
	    			{
	    				Eu3AirHeatTmep = gEu3AirState_203[eEu3AirState203_CellInTemp] + 100;
	    				Eu3AirColdTmep = Eu3AirHeatTmep + 50;
	    			}
	    		}
	    		else if(gAirDebugCmd == 2)//强制制冷
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirModeSet = 1;
	    			if(gEu3AirState_203[eEu3AirState203_CellInTemp] >= 100)
	    			{
	    				Eu3AirColdTmep = gEu3AirState_203[eEu3AirState203_CellInTemp] - 100;
	    				Eu3AirHeatTmep = 0;
	    			}
	    		}
	    		else if(gAirDebugCmd == 3)//自循环
	    		{
	    			Eu3CellPower = 1;
	    			Eu3AirModeSet = 3;
	    		}
	    		else if(gAirDebugCmd == 4)//待机
	    		{
	    			Eu3CellPower = 0;
	    		}
			}

			// if((MU2Cell_AirCmd_Flag == 0) && (MU2Cell_AirCmd_Rcved == 1))   //MU2控制电池侧液路关机
			// {
			// 	Eu3AirSetBuff1[0] = Eu3CellPower_Mu2;
			// }
			// else                            //否则按原来BMS控制电池侧液路策略执行
			{
				Eu3AirSetBuff1[0] = Eu3CellPower;
			}
			Eu3AirSetBuff1[1] = Eu3PcsPower;
			Eu3AirSetBuff1[2] = Eu3AirModeSet;
			Eu3AirSetBuff1[3] = Eu3AirControlObj;
			Eu3AirSetBuff1[4] = Eu3AirColdTmep;
			Eu3AirSetBuff1[5] = Eu3AirHeatTmep;
			Eu3AirSetBuff1[6] = MU2AirNoiseCmd;     //空调降噪命令开关    默认是6  正常模式

			for(i = 0; i < 7; i++)
			{
				/*设置内容有更新,发送设置指令*/
				if(Eu3AirSetBuff1[i] != gEu3AirInfo_202[i])
				{
					(void)MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_AIR, MB_EU3_AIR_WR_BASE_ADDR, Eu3AirSetBuff1, 7,HEAT_MANANG_MODBUS_PORT);
					break;
				}
			}
				// u8 needSend = 0;
				// for(i = 0; i < 7; i++)
				// {
				// 	if(i == 0)
				// 	{
				// 		if(Eu3AirSetBuff1[0] != gEu3AirState_203[eEu3AirState203_PwrState])
				// 		{
				// 			needSend = 1;
				// 		}
				// 	}
				// 	else
				// 	{
				// 		if(Eu3AirSetBuff1[i] != gEu3AirInfo_202[i])
				// 		{
				// 			needSend = 1;
				// 		}
				// 	}
				// }
				// if(needSend == 1)
				// {
				// 	(void)MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_AIR, MB_EU3_AIR_WR_BASE_ADDR, Eu3AirSetBuff1, 7,HEAT_MANANG_MODBUS_PORT);
				// 	break;
				// }
			sCtrlStep =  eEu3AirStep_SetTempDiff;
			break;
			/*制冷制热温差等*/
			case eEu3AirStep_SetTempDiff:
			Eu3AirSetBuff2[0] = Eu3ColdOpenDiff;
			Eu3AirSetBuff2[1] = Eu3ColdCloseDiff;
			Eu3AirSetBuff2[2] = Eu3HeatOpenDiff;
			Eu3AirSetBuff2[3] = Eu3HeatCloseDiff;
			for(i = 0; i < 4; i++)
			{
				/*设置内容有更新,发送设置指令*/
				if((s16)Eu3AirSetBuff2[i] != (s16)gEu3AirInfo_202[i + 7])
				{
					(void)MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_AIR, MB_EU3_AIR_WR_BASE1_ADDR, (u16*)Eu3AirSetBuff2, 4,HEAT_MANANG_MODBUS_PORT);
					break;
				}
			}
			sCtrlStep = eEu3AirStep_ManualControlCmd;
			break;
			case eEu3AirStep_ManualControlCmd:
			Eu3AirManualControlBuff3[0] = MU2Eu3CellPump_ManualEn;
			Eu3AirManualControlBuff3[1] = MU2Eu3CellPump_ManualSpd;
			Eu3AirManualControlBuff3[2] = MU2Eu3ModPump_ManualEn;
			Eu3AirManualControlBuff3[3] = MU2Eu3ModPump_ManualSpd;
			Eu3AirManualControlBuff3[4] = MU2ElecValve_ManualEn;
			Eu3AirManualControlBuff3[5] = MU2ElecValve_ManualPulse;
			Eu3AirManualControlBuff3[6] = MU2Fan_ManualEn;
			Eu3AirManualControlBuff3[7] = MU2Fan_ManualSpdPerc;
			Eu3AirManualControlBuff3[8] = MU2Comp_ManualEn;
			Eu3AirManualControlBuff3[9] = MU2Comp_ManualFreq;
			Eu3AirManualControlBuff3[10] = MU2CellHeat_ManualEn;
			Eu3AirManualControlBuff3[11] = MU2CellHeat_ManualOn;
			Eu3AirManualControlBuff3[12] = MU2BatFilm_ManualEn;
			Eu3AirManualControlBuff3[13] = MU2BatFilm_ManualOn;
			Eu3AirManualControlBuff3[14] = MU2CompDrvFan_ManualEn;
			Eu3AirManualControlBuff3[15] = MU2CompDrvFan_ManualSpd;
			for(i = 0; i < 16; i++)
			{
				/*设置内容有更新,发送设置指令*/
				if((s16)Eu3AirManualControlBuff3[i] != gEu3AirInfo_202[i + 11])
				{
					(void)MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_AIR, 600, Eu3AirManualControlBuff3, 16,HEAT_MANANG_MODBUS_PORT);
					break;
				}
			}
			sCtrlStep = eEu3AirStep_InquCtrlInfo1;
			break;
			default:
			sCtrlStep = eEu3AirStep_InquCtrlInfo1;
			break;
		}
	}
	else if(gGHardPara_104[eGHardPara104_AirType] == 4)//新淼空调
	{
			switch(sCtrlStep)
			{
				/*查询状态信息*/
				case eXinMiaoAirStep_InquStateInfo:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x04, MB_XinMiao_AIR_RO_BASE_ADDR, 12, HEAT_MANANG_MODBUS_PORT);
				if((BSPGetOSMillSecTimeAPI() >= 5000) || (FirstPowerOnFlag == 1))
				{
					FirstPowerOnFlag = 1;
					/*液冷空调通信中断*/
					if(1 == DiagGetPerAirOffCommuStateAPI())
					{
						sCtrlStep = eXinMiaoAirStep_InquStateInfo;
					}
					/*液冷空调通信正常*/
					else
					{
						sCtrlStep = eXinMiaoAirStep_InquCtrlInfo1;
					}
				}
				break;
				/*查询控制信息1（控制模式，开关机）*/
				case eXinMiaoAirStep_InquCtrlInfo1:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_XinMiao_AIR_WR_BASE_ADDR, 2, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eXinMiaoAirStep_InquCtrlInfo2;
				break;
				/*查询控制信息2（设置机组温度）*/
				case eXinMiaoAirStep_InquCtrlInfo2:
				(void)MbClientSendReadReqAPI(MODBUS_SALVE_ADDR_AIR, 0x03, MB_XinMiao_AIR_WR_BASE1_ADDR, 1, HEAT_MANANG_MODBUS_PORT);
				sCtrlStep = eXinMiaoAirStep_SetTactics;
				break;
				case eXinMiaoAirStep_SetTactics:
				//从控信息就绪，处于空闲状态
				if((TRUE == TacGetGAllSlaveReadyHook()) &&  (eCURR_IDLE == GetGChgDhgStateAPI()))
				{
					if(MU2_XMAir_TypeChoose_Cmd == 0)      //长寿命模式
					{
						/*平均温度超过35度或者最大温度超过37℃，设置制冷开启温度为30℃*/
						if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 300;//设置制冷温度为30℃
						}
						/*最小单体温度小于15℃或者平均小于17℃，设置制热温度为15℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
						|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 150;//设置制热温度为15℃
						}
						else
						{
							XinMiaoPower = 0; //关机模式
							XinMiaoMode = 3;//设置为自循环模式
							XinMiaoTemp = 270;//设置制冷温度为27℃
						}
					}
					else                   //高收益模式（采用东莞恒华站点高收益策略）
					{
						/*平均温度超过35度或者最大温度超过37℃，设置制冷开启温度为30℃*/
						if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 300;//设置制冷温度为30℃
						}
						/*最小单体温度小于15℃或者平均小于17℃，设置制热温度为15℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
						|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 150;//设置制热温度为15℃
						}
						else
						{
							XinMiaoPower = 0; //关机模式
							XinMiaoMode = 3;//设置为自循环模式
							XinMiaoTemp = 270;//设置制冷温度为27℃
						}
					}
				}
				//从控信息就绪，处于工作状态
				else if((TRUE == TacGetGAllSlaveReadyHook()) &&  (eCURR_IDLE != GetGChgDhgStateAPI()))
				{
					if(MU2_XMAir_TypeChoose_Cmd == 0)      //长寿命模式
					{
						/*平均温度超过35度或者最大温度超过38℃且电流超过300A*/
						if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -3000)||(sAvgCurr1min >= 3000)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 180;//设置制冷温度为18℃
						}
						/*设置制冷温度为20℃，平均温度超过26度或者最大温度超过27℃且电流超过250A*/
						if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
						&& ((sAvgCurr1min <= -3000)||(sAvgCurr1min >= 3000)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 200;//设置制冷温度为20℃
						}
						/*设置制冷温度为19℃，平均温度超过35度或者最大温度超过37℃且电流超过250A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 190;//设置制冷温度为19℃
						}
						/*设置制冷温度为21℃，平均温度超过26度或者最大温度超过27℃且电流超过200A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
						&& ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 210;//设置制冷温度为21℃
						}
						/*设置制冷温度为19℃，平均温度超过35度或者最大温度超过28℃且电流超过200A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -2000)||(sAvgCurr1min >= 2000)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 190;//设置制冷温度为19℃
						}
						/*设置制冷温度为25℃，平均温度超过30度或者最大温度超过32℃且电流超过200A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7)))
						&& ((sAvgCurr1min <= -2000)||(sAvgCurr1min >= 2000)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 220;//设置制冷温度为22℃
						}
						/*设置制冷温度为21℃，平均温度超过35度或者最大温度超过38℃且电流超过150A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 210;//设置制冷温度为21℃
						}
						/*设置制冷温度为25℃，平均温度超过30度或者最大温度超过32℃且电流超过150A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7)))
						&& ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 230;//设置制冷温度为23℃
						}
						/*设置制冷温度为21℃，平均温度超过35度或者最大温度超过38℃且电流超过95A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 210;//设置制冷温度为21℃
						}
						/*设置制冷温度为25℃，平均温度超过33度或者最大温度超过35℃且电流超过95A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 8))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10)))
						&& ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 240;//设置制冷温度为24℃
						}
						/*设置制冷温度为22℃，平均温度超过35度或者最大温度超过38℃且电流超过55A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 250;//设置制冷温度为25℃
						}
						/*设置制冷温度为25℃，平均温度超过33度或者最大温度超过35℃且电流超过55A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 8))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10)))
						&& ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 270;//设置制冷温度为27℃
						}
						/*55A电流以下设置制冷温度为27℃，平均温度超过35度或者最大温度超过37℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 270;//设置制冷温度为27℃
						}
						/*最小单体温度小于15℃或者平均小于17℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
						|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 150;//设置制热温度为15℃
						}
						else
						{
							if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
							{
								XinMiaoPower = 0; //关机模式
								XinMiaoMode = 0;//设置为自动模式
								XinMiaoTemp = 250;//设置制冷温度为25℃
							}
							else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
							|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
							{
								XinMiaoPower = 0; //关机模式
								XinMiaoMode = 3;//设置为自动模式
								XinMiaoTemp = 250;//设置制冷温度为25℃
							}
							else
							{
								XinMiaoPower = 0; //关机模式
								XinMiaoMode = 0;//设置为自动模式
								XinMiaoTemp = 250;//设置制冷温度为25℃
							}
						}
					}
					else                   //高收益模式
					{
	                    /*平均温度超过35度或者最大温度超过38℃且电流超过300A*/
						if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -3000)||(sAvgCurr1min >= 3000)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 180;//设置制冷温度为18℃
						}
						/*设置制冷温度为20℃，平均温度超过26度或者最大温度超过27℃且电流超过250A*/
						if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
						&& ((sAvgCurr1min <= -3000)||(sAvgCurr1min >= 3000)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 200;//设置制冷温度为20℃
						}
						/*设置制冷温度为19℃，平均温度超过35度或者最大温度超过37℃且电流超过250A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 190;//设置制冷温度为19℃
						}
						/*设置制冷温度为21℃，平均温度超过26度或者最大温度超过27℃且电流超过200A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 1))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 2)))
						&& ((sAvgCurr1min <= -2500)||(sAvgCurr1min >= 2500)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 210;//设置制冷温度为21℃
						}
						/*设置制冷温度为19℃，平均温度超过35度或者最大温度超过28℃且电流超过200A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 10))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13)))
						&& ((sAvgCurr1min <= -2000)||(sAvgCurr1min >= 2000)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 190;//设置制冷温度为19℃
						}
						/*设置制冷温度为25℃，平均温度超过30度或者最大温度超过32℃且电流超过200A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 5))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 7)))
						&& ((sAvgCurr1min <= -2000)||(sAvgCurr1min >= 2000)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 220;//设置制冷温度为22℃
						}
						/*设置制冷温度为28℃，平均温度超过38度或者最大温度超过40℃且电流超过150A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 15)))
						&& ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 280;//设置制冷温度为28℃
						}
						/*设置制冷温度为30℃，平均温度超过37度或者最大温度超过39℃且电流超过150A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 14)))
						&& ((sAvgCurr1min <= -1500)||(sAvgCurr1min >= 1500)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 290;//设置制冷温度为29℃
						}
						/*设置制冷温度为29℃，平均温度超过38度或者最大温度超过40℃且电流超过95A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 15)))
						&& ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 290;//设置制冷温度为29℃
						}
						/*设置制冷温度为31℃，平均温度超过37度或者最大温度超过39℃且电流超过95A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 14)))
						&& ((sAvgCurr1min <= -950)||(sAvgCurr1min >= 950)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 310;//设置制冷温度为31℃
						}
						/*设置制冷温度为31℃，平均温度超过38度或者最大温度超过40℃且电流超过55A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 13))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 15)))
						&& ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 310;//设置制冷温度为31℃
						}
						/*设置制冷温度为33℃，平均温度超过37度或者最大温度超过39℃且电流超过55A*/
						else if(((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 12))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 14)))
						&& ((sAvgCurr1min <= -550)||(sAvgCurr1min >= 550)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 330;//设置制冷温度为33℃
						}
						/*55A电流以下设置制冷温度为35℃，平均温度超过39度或者最大温度超过41℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 14))
						|| (((GetGCellMaxTempAPI() + 5) / 10) >= ((s8)gGHardPara_104[eGHardPara104_AirColdOn_CT] + 16)))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 350;//设置制冷温度为35℃
						}
						/*最小单体温度小于15℃或者平均小于17℃*/
						else if((((GetGCellAvgTempAPI() + 5) / 10) <= 17)
						|| (((GetGCellMinTempAPI() + 5) / 10) <= 15))
						{
							XinMiaoPower = 1; //开机模式
							XinMiaoMode = 0;//设置为自动模式
							XinMiaoTemp = 150;//设置制热温度为15℃
						}
						else
						{
							if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
							{
								XinMiaoPower = 0; //关机模式
								XinMiaoMode = 0;//设置为自动模式
								XinMiaoTemp = 250;//设置制冷温度为25℃
							}
							else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
							|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
							{
								XinMiaoPower = 0; //关机模式
								XinMiaoMode = 3;//设置为自动模式
								XinMiaoTemp = 250;//设置制冷温度为25℃
							}
							else
							{
								XinMiaoPower = 0; //关机模式
								XinMiaoMode = 0;//设置为自动模式
								XinMiaoTemp = 250;//设置制冷温度为25℃
							}
						}
					}
				}
				//从控信息未就绪
				else
				{
					if(MU2_XMAir_TypeChoose_Cmd == 0)      //长寿命模式
					{
						XinMiaoPower = 1; //开机模式
						XinMiaoMode = 0;//设置为自动模式
						XinMiaoTemp = 300;//设置制冷温度为30℃
					}
					else            //高收益模式
					{
						XinMiaoPower = 1; //开机模式
						XinMiaoMode = 0;//设置为自动模式
						XinMiaoTemp = 300;//设置制冷温度为30℃
					}
				}
				sCtrlStep = eXinMiaoAirStep_SetModePwr;
				break;

				case eXinMiaoAirStep_SetModePwr:
				if(eMODE_DEBUG == GetGWorkModeAPI())
				{
					//1:强制制热  2:强制制冷 3：自循环  4：待机
					if(gAirDebugCmd == 1)//强制制热
					{
						XinMiaoPower = 1;
						XinMiaoMode = 2;
						//出水温度40度以上不允许制热
						if(XinMiaoAirSta_215[eXinMiaoAirSta215_OutWatTemp] <= 400)
						{
							XinMiaoTemp = XinMiaoAirSta_215[eXinMiaoAirSta215_OutWatTemp] + 100;
						}
					}
					else if(gAirDebugCmd == 2)//强制制冷
					{
						XinMiaoPower = 1;
						XinMiaoMode = 1;
						//出水温度10度以下不允许制冷
						if(XinMiaoAirSta_215[eXinMiaoAirSta215_OutWatTemp] >= 100)
						{
							XinMiaoTemp = XinMiaoAirSta_215[eXinMiaoAirSta215_OutWatTemp] - 100;
						}
					}
					else if(gAirDebugCmd == 3)//自循环
					{
						XinMiaoPower = 1;
						XinMiaoMode = 3;
					}
					else if(gAirDebugCmd == 4)//
					{
						XinMiaoPower = 0;
					}
				}
					XinMiaoAirSetBuff[0] = XinMiaoMode;
					XinMiaoAirSetBuff[1] = XinMiaoPower;
					(void)MbClientSendWriteMultReqAPI(MODBUS_SALVE_ADDR_AIR, MB_XinMiao_AIR_WR_BASE_ADDR, XinMiaoAirSetBuff, 2,HEAT_MANANG_MODBUS_PORT);
					sCtrlStep = eXinMiaoAirStep_SetTemp;
					break;

					case eXinMiaoAirStep_SetTemp:
					(void)MbClientSendWriteSingReqAPI(MODBUS_SALVE_ADDR_AIR, 0x06, MB_XinMiao_AIR_WR_BASE1_ADDR, XinMiaoTemp, HEAT_MANANG_MODBUS_PORT);
					sCtrlStep = eXinMiaoAirStep_InquStateInfo;
					break;
					default:
					sCtrlStep = eXinMiaoAirStep_InquStateInfo;
					break;
		}
	}
}
//#endif
