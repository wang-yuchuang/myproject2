/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : GroupMsg.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池组主控CAN报文处理模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "PublicPara.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "Authority.h"
#include "ParaUser.h"
#include "CellAFESamp.h"
#include "ParaInterface.h"
#include "RcdDisplayUser.h"
#include "CANInterface.h"
#include "TacUser.h"
#include "CanUser.h"
#include "GroupMsg.h"
#include "GroupInfo.h"
#include "AuxDevTactic.h"
#include "DiagCheck.h"
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
extern u8 gCANTxSinDef_13[];        /*主动更新单帧配置表*/
extern u16 gCANSoxOper_39[];        /*历史记录读写操作命令*/
extern u16 gSysGenParaRO_100[];     /*系统通用参数设置结果*/
extern u16 gGBmuGenPara_102[];  	/*主控通用参数值*/
extern u16 gGBmuHigLevPara_103[];   /*主控高级参数值*/
extern u8 gGHardPara_104[];         /*主控硬件参数配置信息*/

extern u16 gGSysStateInfo_50[];     /*系统状态信息数组*/
extern u16 gGLimCPInfo_51[];        /*电流功率限值数组*/
extern u16 gGLimVInfo_52[];         /*电压限值信息数组*/

extern s16 gGSysMainInfo_60[];      /*系统整体信息数组*/
extern u16 gGSysCapInfo_61[];       /*系统电量信息数组*/
extern u16 gGSysCellInfo_62[];      /*系统单体信息数组*/
extern u16 gGVoltCharInfo_63[];     /*电压特征信息数组*/
extern s16 gGTempCharInfo_64[];     /*温度特征信息数组*/

extern u16 gGSysCmdCtrl_86[];       /*系统控制命令信息数组*/
extern u16 XinMiaoAirPara_216[];    /*新淼空调参数信息*/
extern u16 gImeu3PduInInfo_218[];   /*PDU内部IMEU3信息*/


/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 SendUserUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u16 *data, u8 len, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 data:需发送数组 len:数组长度 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 用户组建和发送主动更新单帧报文帧
** @The notes : 用户自行发送,非通过CAN通信包
**===============================================================================================*/
static u8 SendUserUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u16 *data, u8 len, u8 canNum);

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u8 RcvCanDeviceSerialCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组写设备序列号命令CAN报文处理
** @The notes : CAN通信包接收请求修改(2)处理调用
**===============================================================================================*/
u8 RcvCanDeviceSerialCfgDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_Admin != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > CANINDEX2_LEN) || (index == CANINDEX2_LEN))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
    	(void)RcvDeviceSerialParaCfgAPI(i, eAuthP_Can0 + canNum);
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanDnloadParaWRDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组参数批量配置写数据CAN报文处理
** @The notes : CAN通信包接收请求修改(22)处理调用
**===============================================================================================*/
u8 RcvCanDnloadParaWRDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_Admin != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > CANINDEX22_LEN) || (index == CANINDEX22_LEN))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
		//切换参数配置状态
    	if(0 == i)
		{
			ParaChangDownloadState();
		}
    	//写参数读写信息
    	else
    	{
    	    /*接收到写确认命令时只调用一次即可*/
    	    ParaDownloadWriteParaHandle();
    	}
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanErrRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询故障记录详细信息读取CAN报文处理
** @The notes : CAN通信包接收请求查询(31)处理调用
**===============================================================================================*/
u8 RcvCanErrRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
{
    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > CANINDEX31_LEN) || (index == CANINDEX31_LEN))
    {
        return(eCANERRREAS_INDEXERR);
    }

    /*读一条记录到监测区*/
    (void)ErrRcdUpdateReadInfoAPI();

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanEventRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询事件记录详细信息读取CAN报文处理
** @The notes : CAN通信包接收请求查询(32)处理调用
**===============================================================================================*/
u8 RcvCanEventRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
{
    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > CANINDEX32_LEN) || (index == CANINDEX32_LEN))
    {
        return(eCANERRREAS_INDEXERR);
    }

    /*读一条记录到监测区*/
    (void)EventRcdUpdateReadInfoAPI();

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanWaveRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询故障录波详细信息读取CAN报文处理
** @The notes : CAN通信包接收请求查询(33)处理调用
**===============================================================================================*/
u8 RcvCanWaveRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
{
    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > CANINDEX33_LEN) || (index == CANINDEX33_LEN))
    {
        return(eCANERRREAS_INDEXERR);
    }

    /*读一条记录到监测区*/
    (void)WaveRcdUpdateReadInfoAPI();

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanDodRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询DOD记录详细信息读取CAN报文处理
** @The notes : CAN通信包接收请求查询(34)处理调用
**===============================================================================================*/
u8 RcvCanDodRcdReadInfoDeal(u8 index, u8 len, u8 canNum)
{
    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > CANINDEX34_LEN) || (index == CANINDEX34_LEN))
    {
        return(eCANERRREAS_INDEXERR);
    }

    /*读一条记录到监测区*/
    (void)DodRcdUpdateReadInfoAPI();

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanDataRcdDisplayInfoDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收查询数据记录详细信息显示CAN报文处理
** @The notes : CAN通信包接收请求查询(37)处理调用
**===============================================================================================*/
u8 RcvCanDataRcdDisplayInfoDeal(u8 index, u8 len, u8 canNum)
{
    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > CANINDEX37_LEN) || (index == CANINDEX37_LEN))
    {
        return(eCANERRREAS_INDEXERR);
    }

    /*读一条记录到显示区*/
    (void)DataRcdUpdateDisplayAPI();

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanHisRecordRWDateDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收历史记录读写日期处理
** @The notes : CAN通信包接收请求修改(39)处理调用
**===============================================================================================*/
u8 RcvCanHisRecordRWDateDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*设置命令索引错误*/
    if(((index + len) > CANINDEX39_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        switch(i)
        {
            //设置读取数据记录日期
            case 0:
            	/*CAN口运维权限未开启*/
                if((canNum >= eCANNUM)
                    || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
            	{
            		return(eCANERRREAS_AUTHERR);
            	}

            	/*记录下发日期*/
            	DataRcdSetReadDateAPI();
				break;

            //设置清除数据记录日期
            case 1:
            	/*CAN口高级权限未开启*/
                if((canNum >= eCANNUM)
                    || (eAuthL_Service > GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
            	{
            		return(eCANERRREAS_AUTHERR);
            	}

            	/*清除指定当天数据记录*/
            	DataRcdClrDateInfoAPI();
                break;

			//设置清除故障记录日期
			case 2:
				/*CAN口维护权限未开启(非清除单条记录(读下一条)需要高级权限)*/
				if((canNum >= eCANNUM)
					|| (eAuthL_Operater > GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum)))
					|| ((91 != gCANSoxOper_39[2]) && (eAuthL_Service > GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum)))))
				{
					return(eCANERRREAS_AUTHERR);
				}

				/*清除相应故障记录*/
				ErrRcdClrDateInfoAPI();
				break;

			//设置清除事件记录日期
			case 3:
				/*CAN口高级权限未开启*/
				if((canNum >= eCANNUM)
					|| (eAuthL_Operater > GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum)))
					|| ((91 != gCANSoxOper_39[3]) && (eAuthL_Service > GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum)))))
				{
					return(eCANERRREAS_AUTHERR);
				}

				/*清除相应事件记录*/
				EventRcdClrDateInfoAPI();
				break;

			//设置清除故障录波日期
			case 4:
				/*CAN口高级权限未开启*/
				if((canNum >= eCANNUM)
					|| (eAuthL_Operater > GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum)))
					|| ((91 != gCANSoxOper_39[4]) && (eAuthL_Service > GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum)))))
				{
					return(eCANERRREAS_AUTHERR);
				}

				/*清除相应故障录波记录*/
				WaveRcdClrDateInfoAPI();
				break;

			//设置清除DOD记录命令
			case 5:
				/*CAN口高级权限未开启*/
				if((canNum >= eCANNUM)
					|| (eAuthL_Operater > GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum)))
					|| ((91 != gCANSoxOper_39[5]) && (eAuthL_Service > GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum)))))
				{
					return(eCANERRREAS_AUTHERR);
				}

				/*清除全部DOD记录*/
				DodRcdClrAllInfoAPI();
				break;

            default:
                break;
        }
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupPasswordDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组权限密码命令CAN报文处理
** @The notes : CAN通信包接收请求修改(85)处理调用
**===============================================================================================*/
u8 RcvCanGroupPasswordDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*控制命令索引错误*/
    if(((index + len) > GINDEX85_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        switch(i)
        {
            //权限密码高位
            case 0:
				break;

            //权限密码低位
            case 1:
            	/*执行密码检测*/
            	RcvSysAuthorityCmdAPI(eAuthP_Can0 + canNum);
            	SendCanGroupAuthorityInfoMsg();
                break;

            default:
                break;
        }
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupCtrlCmdDeal(u8 saddr, u8 index, u8 len, u8 canNum)
** @Input     : saddr:命令源地址 index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组控制命令CAN报文处理
** @The notes : CAN通信包接收请求修改(86)处理调用
**===============================================================================================*/
u8 RcvCanGroupCtrlCmdDeal(u8 saddr, u8 index, u8 len, u8 canNum)
{
    u8 i = 0;
	//#if(BMU_IMLU2_EN == 1)
	u16 Color = 0;
	u16 Eu3AirPcsPower = 0;
	u16 Eu3AirCellPower_Auto = 0;
	u16 Eu3AirNoise_cmd = 6;    // 默认6，正常状态
	u16 TFAir_ON_OFF_ResetCmd = 0;  
	u16 TFAir_TypeCmd = 0;         //默认长寿命模式
	u16 XMAir_TypeCmd = 0;         //默认长寿命模式
	u16 CellTemSpe_Poin = 0;
	u16 Eu3CellWaterPump_ManualFlag = 0;  //0：自动控制   1：手动控制
	u16 Eu3CellWaterPump_ManualSpd = 0;
	u16 Eu3ModWaterPump_ManualFlag = 0;
	u16 Eu3ModWaterPump_ManualSpd = 0;
	u16 Eu3ElecExpValve_ManualFlag = 0;
	u16 Eu3ElecExpValve_ManualPulse = 0;
	u16 Eu3Fan_ManualFlag = 0;
	u16 Eu3Fan_ManualSpdPerc = 0;
	u16 Eu3Comp_ManualFlag = 0;
	u16 Eu3Comp_ManualFreq = 0;
	u16 CellHeat_ManualFlag = 0;
	u16 CellHeat_ManualOnOff = 0;
	u16 BatHeatFilm_ManualFlag = 0;
	u16 BatHeatFilm_ManualOnOff = 0;
	u16 CompDrvFan_ManualFlag = 0;
	u16 CompDrvFan_ManualSpd = 0;
	u16 KeepPowerMode = 0;             //保供电模式
	u16 ManualCalibration_SOC = 0;     //手动校准SOC
	u16 ResetAFEstatus = 0;         //重置AFE芯片
	u16 ResetBSUstatus = 0;         //复位BSU
	//#endif
    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > GINDEX86_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        switch(i)
        {
            //工作状态控制
            case eSysCmdCtrl86_State:
                /*多组电池堆控有效时CAN0只接收堆控命令*/
            	if((gSysGenParaRO_100[eSGenPara100_AllGNum] <= 1)
            		|| (saddr == gSysGenParaRO_100[eSGenPara100_ExtAddr])
            		|| (eCAN0 != canNum))
            	{
            		RcvGroupWorkStateCmdAPI(eAuthP_Can0 + canNum);
            	}
                break;

            //工作模式切换
            case eSysCmdCtrl86_Mode:
            	RcvGroupWorkModeCmdAPI(eAuthP_Can0 + canNum);
                break;

            //系统清除命令
            case eSysCmdCtrl86_ClrCmd:
            	RcvGroupSysClrCmdAPI(eAuthP_Can0 + canNum);
                break;
			
			//系统指示灯控制命令
			case eSysCmdCtrl86_LightCmd:
				if(gGHardPara_104[eGHardPara104_Imlu2En] == 1)
				{
					Color = gGSysCmdCtrl_86[eSysCmdCtrl86_LightCmd];
					RcvMU2LightColorCmdAPI(Color);
				}
				break;

			//模块侧空调开关机控制
			case eSysCmdCtrl86_PcsAirCmd:
				if(gGHardPara_104[eGHardPara104_AirType] == 3)
				{
					Eu3AirPcsPower = gGSysCmdCtrl_86[eSysCmdCtrl86_PcsAirCmd];
					RcvMU2PcsAirCmdAPI(Eu3AirPcsPower);
				}
				break;
			//AFE内阻采集控制             by lxr01202
			case eSysCmdCtrl86_ResisTrigCmd:
				if(gGSysCmdCtrl_86[eSysCmdCtrl86_ResisTrigCmd] != 0 && gGHardPara_104[eGHardPara104_BmuSWVerHH] == 2)
				{
					WriteCellRSampIndexHook(gGSysCmdCtrl_86[eSysCmdCtrl86_ResisTrigCmd]);//写入内阻采集序号
					WriteCellRSampRequireFlagHook(1);//开启指定频率内阻检测
				}
				break;

			//AFE检测SR状态
			case eSysCmdCtrl86_AfeSrCmd:
				if(gGSysCmdCtrl_86[eSysCmdCtrl86_AfeSrCmd] == 1 && gGHardPara_104[eGHardPara104_BmuSWVerHH] == 2)
				{
					//上次查询指令没执行完，不清除目前已收集数据
					if(1 != DiagGetRcvAllSrstateFlagAPI())
					{
						DiagInitRcvAllSrstateFlagAPI();
					}
					WriteAfeStateSampRequireFlagHook(1);//开启sr状态检测

				}else
				{
					WriteAfeStateSampRequireFlagHook(0);//关闭sr状态检测
				}
				break;
			//A2425柠檬项目 手动并离网需要接收到上层ESCC下发自研EU3空调关机指令，BMS控制空调关机（保证压缩机不启动）开机：1   关机：0
			case eSysCmdCtrl86_CellAirCmd:
				if((gGHardPara_104[eGHardPara104_AirType] == 3) && (gGHardPara_104[eGHardPara104_ProjectCode] == 3))
				{
					Eu3AirCellPower_Auto = gGSysCmdCtrl_86[eSysCmdCtrl86_CellAirCmd];
					RcvMU2CellAirCmdAPI(Eu3AirCellPower_Auto);
				}
				break;
			//自研EU3空调降噪控制开关命令
			case eSysCmdCtrl86_AirNoiseCmd:
				if(gGHardPara_104[eGHardPara104_AirType] == 3)
				{
					Eu3AirNoise_cmd = gGSysCmdCtrl_86[eSysCmdCtrl86_AirNoiseCmd];
					RcvMU2AirNoiseCmdAPI(Eu3AirNoise_cmd);
				}
				break;
			//TF空调新增开关机命令(开机、关机、报警复位)
			case eSysCmdCtrl86_TFAirON_OFF_ResetCmd:
				if(gGHardPara_104[eGHardPara104_AirType] == 2)
				{
					TFAir_ON_OFF_ResetCmd = gGSysCmdCtrl_86[eSysCmdCtrl86_TFAirON_OFF_ResetCmd];
					RcvMU2_TFAirON_OFF_ResetCmdAPI(TFAir_ON_OFF_ResetCmd);
				}
				break;
			//TF空调新增 高收益模式及长寿命模式   0：长寿命  1：高收益
			case eSysCmdCtrl86_TFAir_Highlife_longMoney:
				if(gGHardPara_104[eGHardPara104_AirType] == 2)
				{
					TFAir_TypeCmd = gGSysCmdCtrl_86[eSysCmdCtrl86_TFAir_Highlife_longMoney];
					RcvMU2_TFAir_Type_CmdAPI(TFAir_TypeCmd);
				}
				break;
			//新淼空调新增 高收益模式及长寿命模式 0：长寿命  1：高收益
			case eSysCmdCtrl86_XMAir_Highlife_longMoney:
				if(gGHardPara_104[eGHardPara104_AirType] == 4)
				{
					XMAir_TypeCmd = gGSysCmdCtrl_86[eSysCmdCtrl86_XMAir_Highlife_longMoney];
					RcvMU2_XMAir_Type_CmdAPI(XMAir_TypeCmd);
				}
				break;
			//by lxr01202 添加
			//屏蔽特殊点位温度选项
			case eSysCmdCtrl86_CellTemCmd:
				CellTemSpe_Poin = gGSysCmdCtrl_86[eSysCmdCtrl86_CellTemCmd];
				RcvMU2CellTemSpeCmdAPI(CellTemSpe_Poin);
				break;
			//EU3水泵控制
			case eSysCmdCtrl86_Eu3AirCell_WaterPump_Cmd:          // 电池侧水泵手动控制标志
                    Eu3CellWaterPump_ManualFlag = gGSysCmdCtrl_86[eSysCmdCtrl86_Eu3AirCell_WaterPump_Cmd];
                    RcvMU2Eu3CellWaterPump_ManualCmdAPI(Eu3CellWaterPump_ManualFlag);
                break;

            case eSysCmdCtrl86_Eu3AirCell_WaterPump_Spd:          //  电池侧水泵手动转速

                    Eu3CellWaterPump_ManualSpd = gGSysCmdCtrl_86[eSysCmdCtrl86_Eu3AirCell_WaterPump_Spd]; // 0~100
                    RcvMU2Eu3CellWaterPump_SpdCmdAPI(Eu3CellWaterPump_ManualSpd);

                break;

            case eSysCmdCtrl86_Eu3AirMod_WaterPump_Cmd:            // 模块侧水泵手动标志

                    Eu3ModWaterPump_ManualFlag = gGSysCmdCtrl_86[eSysCmdCtrl86_Eu3AirMod_WaterPump_Cmd];
                    RcvMU2Eu3ModWaterPump_ManualCmdAPI(Eu3ModWaterPump_ManualFlag);

                break;

            case eSysCmdCtrl86_Eu3AirMod_WaterPump_Spd:            //  模块侧水泵手动转速

                    Eu3ModWaterPump_ManualSpd = gGSysCmdCtrl_86[eSysCmdCtrl86_Eu3AirMod_WaterPump_Spd];
                    RcvMU2Eu3ModWaterPump_SpdCmdAPI(Eu3ModWaterPump_ManualSpd);

                break;

            case eSysCmdCtrl86_ElecHeatFilm_Cmd:                  //  电子膨胀阀手动控制标志
					Eu3ElecExpValve_ManualFlag = gGSysCmdCtrl_86[eSysCmdCtrl86_ElecHeatFilm_Cmd];
					RcvMU2ElecExpValve_ManualCmdAPI(Eu3ElecExpValve_ManualFlag);
                break;

            case eSysCmdCtrl86_ElecHeatFilm_OpenPerc:             //  电子膨胀阀手动开度
                if(Eu3ElecExpValve_ManualFlag == 1)
                {
                    Eu3ElecExpValve_ManualPulse = gGSysCmdCtrl_86[eSysCmdCtrl86_ElecHeatFilm_OpenPerc];
                    RcvMU2ElecExpValve_PulseCmdAPI(Eu3ElecExpValve_ManualPulse);
                }
                break;

            case eSysCmdCtrl86_Fan_Cmd:                           //  风机手动控制标志

                    Eu3Fan_ManualFlag = gGSysCmdCtrl_86[eSysCmdCtrl86_Fan_Cmd];
                    RcvMU2Fan_ManualCmdAPI(Eu3Fan_ManualFlag);

                break;

            case eSysCmdCtrl86_Fan_SpdPerc:                       //  风机手动转速百分比

                    Eu3Fan_ManualSpdPerc = gGSysCmdCtrl_86[eSysCmdCtrl86_Fan_SpdPerc];
                    RcvMU2Fan_SpdCmdAPI(Eu3Fan_ManualSpdPerc);

                break;

            case eSysCmdCtrl86_Compressor_Cmd:                    //  压缩机手动控制标志（关机或故障后自动清0）

                    Eu3Comp_ManualFlag = gGSysCmdCtrl_86[eSysCmdCtrl86_Compressor_Cmd];
                    RcvMU2Compressor_ManualCmdAPI(Eu3Comp_ManualFlag);

                break;

            case eSysCmdCtrl86_Compressor_Freq:                   //  压缩机手动频率

                    Eu3Comp_ManualFreq = gGSysCmdCtrl_86[eSysCmdCtrl86_Compressor_Freq];
                    RcvMU2Compressor_FreqCmdAPI(Eu3Comp_ManualFreq);
                break;

            case eSysCmdCtrl86_CellHeat_Cmd:                      //  电加热手动控制标志
                    CellHeat_ManualFlag = gGSysCmdCtrl_86[eSysCmdCtrl86_CellHeat_Cmd];
                    RcvMU2CellHeat_ManualCmdAPI(CellHeat_ManualFlag);
                break;

            case eSysCmdCtrl86_CellHeat_OnOff:                        //  电加热手动开关（手动开时强开电池侧水泵）
                    CellHeat_ManualOnOff = gGSysCmdCtrl_86[eSysCmdCtrl86_CellHeat_OnOff];
                    RcvMU2CellHeat_OnOffCmdAPI(CellHeat_ManualOnOff);
                break;

            case eSysCmdCtrl86_BatHeatFilm_Cmd:                   //  手动曲轴电加热标志
                    BatHeatFilm_ManualFlag = gGSysCmdCtrl_86[eSysCmdCtrl86_BatHeatFilm_Cmd];
                    RcvMU2BatHeatFilm_ManualCmdAPI(BatHeatFilm_ManualFlag);
                break;

            case eSysCmdCtrl86_BatHeatFilm_OnOff:                 //  手动曲轴电加热
                    BatHeatFilm_ManualOnOff = gGSysCmdCtrl_86[eSysCmdCtrl86_BatHeatFilm_OnOff];
                    RcvMU2BatHeatFilm_OnOffCmdAPI(BatHeatFilm_ManualOnOff);
                break;

            case eSysCmdCtrl86_CompressorDrv_Fan_Cmd:             //  压缩机驱动器散热风机手动标志
                    CompDrvFan_ManualFlag = gGSysCmdCtrl_86[eSysCmdCtrl86_CompressorDrv_Fan_Cmd];
                    RcvMU2CompDrvFan_ManualCmdAPI(CompDrvFan_ManualFlag);
                break;

            case eSysCmdCtrl86_CompressorDrv_Fan_SpdPerc:         //  驱动器散热风机手动转速

                    CompDrvFan_ManualSpd = gGSysCmdCtrl_86[eSysCmdCtrl86_CompressorDrv_Fan_SpdPerc];
                    RcvMU2CompDrvFan_SpdCmdAPI(CompDrvFan_ManualSpd);
                break;
			case eSysCmdCtrl86_KeepPowerMode:                    //保供电模式
					KeepPowerMode = gGSysCmdCtrl_86[eSysCmdCtrl86_KeepPowerMode];
					RcvMU2DiagKeepPowerModeCmdAPI(KeepPowerMode);
				break;	
			case eSysCmdCtrl86_Manual_Calibration_SOC:             //  手动校准SOC
					ManualCalibration_SOC = gGSysCmdCtrl_86[eSysCmdCtrl86_Manual_Calibration_SOC];
					RcvMU2ManualCalibration_SOCCmdAPI(ManualCalibration_SOC);
                break;	
			case eSysCmdCtrl86_ResetAFEstatus:                    //远程重置AFE状态
					ResetAFEstatus = gGSysCmdCtrl_86[eSysCmdCtrl86_ResetAFEstatus];
					RcvMU2ResetAFEstatusCmdAPI(ResetAFEstatus);
                break;
			case eSysCmdCtrl86_ResetBSU:                            //远程复位BSU
					ResetBSUstatus = gGSysCmdCtrl_86[eSysCmdCtrl86_ResetBSU];
					RcvMU2ResetBSUstatusCmdAPI(ResetBSUstatus);
                break;	
            default:
                break;
        }
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanUserCtrlCmdDeal(u8 index, u8 len)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收用户控制命令CAN报文处理
** @The notes : CAN通信包接收请求修改(87)处理调用
**===============================================================================================*/
u8 RcvCanUserCtrlCmdDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*参数错误*/
    if(canNum >= eCANNUM)
    {
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > GINDEX87_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        switch(i)
        {
            //单体信息触发
            case eUserCtrl87_CellTrig:
            	RcvGroupTrigInfoCmdAPI(eAuthP_Can0 + canNum);
                break;

            //内部报文转发命令
            case eUserCtrl87_InTran:
            	RcvGroupTransInfoCmdAPI(eAuthP_Can0 + canNum);
                break;

            //历史故障切换命令
            case eUserCtrl87_HisErrSw:
            	RcvGroupDispHisErrCmdAPI(eAuthP_Can0 + canNum);
                break;

			//历史事件切换命令
			case eUserCtrl87_HisEvnSw:
				RcvGroupDispHisEventCmdAPI(eAuthP_Can0 + canNum);
                break;

			//故障录波切换命令
			case eUserCtrl87_HisWavSw:
				RcvGroupDispHisWaveRcdCmdAPI(eAuthP_Can0 + canNum);
				break;

			//DOD记录换命令
			case eUserCtrl87_HisDodSw:
				RcvGroupDispHisDodCmdAPI(eAuthP_Can0 + canNum);
				break;

			//粘连检测命令
			case eUserCtrl87_AdhCheck:
				RcvGroupReqAdhCheckCmdAPI(eAuthP_Can0 + canNum);
                break;

			//绝缘检测命令
			case eUserCtrl87_LeakCheck:
				RcvGroupReqLeakCheckCmdAPI(eAuthP_Can0 + canNum);
				break;

			//百叶风机控制命令
			case eUserCtrl87_ShutFan:
				RcvGroupShutFanCtrlCmdAPI(eAuthP_Can0 + canNum);
				break;

			//电池柜照明控制命令
			case eUserCtrl87_Lighting:
				RcvGroupLightCtrlCmdAPI(eAuthP_Can0 + canNum);
				break;

			//声光报警强制控制命令
			case eUserCtrl87_ZHSAForce:
				RcvGroupZHSAForceCmdAPI(eAuthP_Can0 + canNum);
				break;

			//气溶胶消防主动触发命令(预留)
			case eUserCtrl87_FireTrig:
				//RcvGroupFireTrigCmdAPI(eAuthP_Can0 + canNum);
				break;

			//液冷备电QF3控制命令
			case eUserCtrl87_QF3Ctr:
				RcvGroupQF3CtrlCmdAPI(eAuthP_Can0 + canNum);
				break;
            default:
                break;
        }
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupDebugCmdDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组调试命令CAN报文处理
** @The notes : CAN通信包接收请求修改(88)处理调用
**===============================================================================================*/
u8 RcvCanGroupDebugCmdDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_Admin != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*调试命令索引错误*/
    if(((index + len) > GINDEX88_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
    	//调试命令设置
        if(i < eDbgCmd88_Affirm)
        {
            RcvGroupDebugOperCmdAPI(i);
        }
        //调试确认
        else if(i == eDbgCmd88_Affirm)
        {
            RcvGroupDebugAffirmCmdAPI(eAuthP_Can0 + canNum);
        }
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanSystemGenParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收系统通用参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(105)处理调用
**===============================================================================================*/
u8 RcvCanSystemGenParaCfgDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;
    u8 reault = TRUE;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*设置命令索引错误*/
    if(((index + len) > SINDEX105_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        /*写入参数处理*/
    	reault &= RcvSystemGenParaCfgAPI(i, eAuthP_Can0 + canNum);
    }

    /*有写失败*/
    if(FALSE == reault)
    {
        /*参数非法*/
    	return(eCANERRREAS_RANGEERR);
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanSystemHWParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : void
** @Function  : 接收系统硬件参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(106)处理调用
**===============================================================================================*/
u8 RcvCanSystemHWParaCfgDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;
    u8 reault = TRUE;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_Admin != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*设置命令索引错误*/
    if(((index + len) > SINDEX106_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        /*写入参数处理*/
    	reault &= RcvSystemHWParaCfgAPI(i, eAuthP_Can0 + canNum);
    }

    /*有写失败*/
    if(FALSE == reault)
    {
        /*参数非法*/
    	return(eCANERRREAS_RANGEERR);
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupGenParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收主控通用参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(107)处理调用
**===============================================================================================*/
u8 RcvCanGroupGenParaCfgDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;
    u8 reault = TRUE;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*设置命令索引错误*/
    if(((index + len) > GINDEX107_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        /*写入参数处理*/
    	reault &= RcvGroupGenParaCfgAPI(i, eAuthP_Can0 + canNum);
    }

    /*有写失败*/
    if(FALSE == reault)
    {
        /*参数非法*/
    	return(eCANERRREAS_RANGEERR);
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupHigParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收主控高级参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(108)处理调用
**===============================================================================================*/
u8 RcvCanGroupHigParaCfgDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;
    u8 reault = TRUE;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_User == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum)))
    	|| (eAuthL_Operater == GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*设置命令索引错误*/
    if(((index + len) > GINDEX108_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        /*写入参数处理*/
    	reault &= RcvGroupHigParaCfgAPI(i, eAuthP_Can0 + canNum);
    }

    /*有写失败*/
    if(FALSE == reault)
    {
        /*参数非法*/
    	return(eCANERRREAS_RANGEERR);
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupHWParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收主控硬件参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(109)处理调用
**===============================================================================================*/
u8 RcvCanGroupHWParaCfgDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;
    u8 reault = TRUE;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_Admin != GetSysNowAuthorityAPI((e_AuthOpenPort)(eAuthP_Can0 + canNum))))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*设置命令索引错误*/
    if(((index + len) > GINDEX109_LEN) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        /*写入参数处理*/
    	reault &= RcvGroupHWParaCfgAPI(i, eAuthP_Can0 + canNum);
    }

    /*有写失败*/
    if(FALSE == reault)
    {
        /*参数非法*/
    	return(eCANERRREAS_RANGEERR);
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : void GroupSendUpMsg1sBy50ms_API(void)
** @Input     : void
** @Output    : void
** @Function  : 主控上传1s周期CAN通信报文任务
** @The notes : 用户50ms周期接口调用
**===============================================================================================*/
void GroupSendUpMsg1sBy50ms_API(void)
{
	/*使能自研协议*/
	#if(1 == BMU_SELF_CAN_EN)
	static u8 sTime1s = 0;

	/*使用CAN#0作为监控口时主要关键信息发送到CAN#3(IMSU-D)*/
	#if(2 != CAN_UPDATE_PORT)
	switch(sTime1s)
	{
		/*500ms发送状态值信息*/
		case 0:
		case 10:
			/*发送单帧*/
			(void)SendUserUpsMsg_API(0xff, 0, 50, gGSysStateInfo_50, GINDEX50_LEN, eCAN2);
			break;

		/*1s发送自研空调控制信息或者TF空调参数信息*/
		case 2:
			if(gGHardPara_104[eGHardPara104_AirType] == 1)//自研空调
			{
				CANBuildSendUpmMsg_API(0xff, 0, 206, 0, GINDEX206_LEN, eCAN0);
			}
			else if(gGHardPara_104[eGHardPara104_AirType] == 2)//TF空调
			{
				CANBuildSendUpmMsg_API(0xff, 0, 212, 0, GINDEX212_LEN, eCAN0);
			}
			else if(gGHardPara_104[eGHardPara104_AirType] == 3)//EU3空调
			{
				CANBuildSendUpmMsg_API(0xff, 0, 202, 0, GINDEX202_LEN, eCAN0);
			}
			else if(gGHardPara_104[eGHardPara104_AirType] == 4)//新淼空调
			{
				CANBuildSendUpmMsg_API(0xff, 0, 215, 0, GINDEX215_LEN, eCAN0);
			}
			break;

		/*500ms发送电流限值值信息*/
		case 4:
		case 14:
			/*发送单帧*/
			(void)SendUserUpsMsg_API(0xff, 0, 51, gGLimCPInfo_51, GINDEX51_LEN, eCAN2);
			break;

		/*1s发送自研空调状态信息或TF空调状态信息*/
		case 6:
			/*发送长帧*/
			if(gGHardPara_104[eGHardPara104_AirType] == 1)//自研空调
			{
				CANBuildSendUpmMsg_API(0xff, 0, 207, 0, GINDEX207_LEN, eCAN0);
			}
			else if(gGHardPara_104[eGHardPara104_AirType] == 2)//TF空调
			{
				CANBuildSendUpmMsg_API(0xff, 0, 213, 0, GINDEX213_LEN, eCAN0);
			}
			else if(gGHardPara_104[eGHardPara104_AirType] == 3)//eu3空调
			{
				CANBuildSendUpmMsg_API(0xff, 0, 203, 0, GINDEX203_LEN, eCAN0);
			}
			else if(gGHardPara_104[eGHardPara104_AirType] == 4)//新淼空调
			{
				SendUserUpsMsg_API(0xff, 0, 216, (u16*)XinMiaoAirPara_216, GINDEX216_LEN, eCAN0);
			}
			break;

		/*500ms发送总体值信息*/
		case 8:
		case 18:
			/*发送单帧*/
			(void)SendUserUpsMsg_API(0xff, 0, 60, (u16*)gGSysMainInfo_60, GINDEX60_LEN, eCAN2);
			break;

		/*1s发送PDU内IMEU3 DI信息给IMSU-D*/
		case 12:
			if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
		 		|| (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
				|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))//A2415 A2445 A2415降本
			{
				SendUserUpsMsg_API(0xff, 0, 218,  (u16*)gImeu3PduInInfo_218, 2, eCAN2);
			}
			else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
					|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))//A2419 A2425 Y2403
			{

			}
			else
			{
				SendUserUpsMsg_API(0xff, 0, 218,  (u16*)gImeu3PduInInfo_218, 2, eCAN2);
			}
			break;

		/*1s发送液冷空调告警信息或TF空调告警信息*/
		case 16:
			if(gGHardPara_104[eGHardPara104_AirType] == 1)//自研空调
			{
				CANBuildSendUpmMsg_API(0xff, 0, 208, 0, GINDEX208_LEN, eCAN0);
			}
			else if(gGHardPara_104[eGHardPara104_AirType] == 2)//TF空调
			{
				CANBuildSendUpmMsg_API(0xff, 0, 214, 0, GINDEX214_LEN, eCAN0);
			}
			break;

		/*1s发送电压限值信息*/
		case 3:
			/*发送单帧*/
			(void)SendUserUpsMsg_API(0xff, 0, 52, gGLimVInfo_52, GINDEX52_LEN, eCAN2);
			break;

		/*1s发送SOC信息*/
		case 7:
			(void)SendUserUpsMsg_API(0xff, 0, 61, gGSysCapInfo_61, GINDEX61_LEN, eCAN2);
			break;

		/*1s发送液冷PACK高压互锁信息*/
		case 11:
			//CANBuildSendUpmMsg_API(0xff, 0, 210, 0, GINDEX210_LEN, eCAN0);
			break;

		/*1s发送PACK气溶胶信息*/
		case 17:
			//CANBuildSendUpmMsg_API(0xff, 0, 210, 0, GINDEX210_LEN, eCAN0);
			break;

		default:
			break;
	}
	#endif

	//by lxr01202
	//循环检测是否有单体内阻实部和虚部发送需求，只发送一次
	if(1 == GetCellRSampRealFinFlagAPI() && 1 == GetCellRSampImagFinFlagAPI() && 0 != GetCellRSampIndexAPI() && 2 == GetCellRSampRequireFlagAPI())
	{
		//发送单体内阻实部和虚部，只发送一次
		SendCanGroupCellResisRIInfoMsg();
		WriteCellRSampRequireFlagHook(0);//内阻采集指令已发送，清空内阻采样需求
	}
	/*禁止CATL协议时CAN#1发送单体信息(非CAN#1监控)*/
	#if((0 == BMU_CATL_CAN_EN) && (eCAN0 != CAN_UPDATE_PORT))
	/*500ms一帧单体电压信息*/
	if((1 == sTime1s) || (5 == sTime1s))
	{
		/*采集完成才开始上发*/
		if(GetCellVSampFinFlagAPI() > 0)
		{
			SendCanGroupCellVoltInfoMsg();
		}
	}
	/*1s一帧单体温度信息*/
	else if(9 == sTime1s)
	{
		/*采集完成才开始上发*/
		if(GetCellTSampFinFlagAPI() > 0)
		{
			SendCanGroupCellTempInfoMsg();
		}
	}
	/*1s一帧单体均衡状态*/
	else if(15 == sTime1s)
	{
		/*采集完成才开始上发*/
		if(GetCellVSampFinFlagAPI() > 0)
		{
			SendCanGroupCellBalanInfoMsg();
		}
	}
	//使能自研从控BSU CAN通信信息才测内阻
	#if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))
	/*1s一帧单体内阻信息*/
	else if(13 == sTime1s)
	{
		/*采集完成才开始上发*/
		if(GetCellRSampFinFlagAPI() > 0)
		{
			SendCanGroupCellResisInfoMsg();
		}
	}
	#endif
	#endif

	/*50ms计时20次转化为1s周期*/
	if(sTime1s >= 19)
	{
		sTime1s = 0;
	}
	else
	{
		sTime1s++;
	}
	#endif
}

/*=================================================================================================
** @Name      : void SendCanGroupSystemStateInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送系统整体状态信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupSystemStateInfoMsg(void)
{
	/*自研内部CAN协议使能*/
	#if(1 == BMU_SELF_CAN_EN)
	/*发送单帧*/
	(void)CANBuildSendUpsMsg_API(0xff, 0, 50, 0, GINDEX50_LEN, eCAN0);

	/*监控不在CAN0口*/
	#if(eCAN0 != CAN_UPDATE_PORT)
	(void)CANBuildSendUpsMsg_API(0xff, 0, 50, 0, GINDEX50_LEN, CAN_UPDATE_PORT);
	#endif
	#endif
}

/*=================================================================================================
** @Name      : void SendCanGroupPortStateInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送输入输出端口状态信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupPortStateInfoMsg(void)
{
	/*自研内部CAN协议使能*/
	#if(1 == BMU_SELF_CAN_EN)
	/*发送单帧*/
	(void)CANBuildSendUpsMsg_API(0xff, 0, 56, 0, GINDEX56_LEN, eCAN0);

	/*监控不在CAN0口*/
	#if(eCAN0 != CAN_UPDATE_PORT)
	(void)CANBuildSendUpsMsg_API(0xff, 0, 56, 0, GINDEX56_LEN, CAN_UPDATE_PORT);
	#endif
	#endif
}

/*=================================================================================================
** @Name      : void SendCanGroupAuthorityInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送当前权限状态信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupAuthorityInfoMsg(void)
{
	/*自研内部CAN协议使能*/
	#if(1 == BMU_SELF_CAN_EN)
	/*发送单帧*/
	(void)CANBuildSendUpsMsg_API(0xff, 0, 80, 0, GINDEX80_LEN, eCAN0);

	/*监控不在CAN0口*/
	#if(eCAN0 != CAN_UPDATE_PORT)
	(void)CANBuildSendUpsMsg_API(0xff, 0, 80, 0, GINDEX80_LEN, CAN_UPDATE_PORT);
	#endif
	#endif
}

/*=================================================================================================
** @Name      : void SendCanGroupHisErrInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送历史故障信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupHisErrInfoMsg(void)
{
	/*自研内部CAN协议使能*/
	#if(1 == BMU_SELF_CAN_EN)
	/*发送长帧*/
	(void)CANBuildSendUpmMsg_API(0xff, 0, 45, 0, GINDEX45_LEN, eCAN0);

	/*监控不在CAN0口*/
	#if(eCAN0 != CAN_UPDATE_PORT)
	(void)CANBuildSendUpmMsg_API(0xff, 0, 45, 0, GINDEX45_LEN, CAN_UPDATE_PORT);
	#endif
	#endif
}

/*=================================================================================================
** @Name      : void SendCanGroupHisEventInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送历史事件信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupHisEventInfoMsg(void)
{
	/*自研内部CAN协议使能*/
	#if(1 == BMU_SELF_CAN_EN)
	/*发送长帧*/
	(void)CANBuildSendUpmMsg_API(0xff, 0, 46, 0, GINDEX46_LEN, eCAN0);

	/*监控不在CAN0口*/
	#if(eCAN0 != CAN_UPDATE_PORT)
	(void)CANBuildSendUpmMsg_API(0xff, 0, 46, 0, GINDEX46_LEN, CAN_UPDATE_PORT);
	#endif
	#endif
}

/*=================================================================================================
** @Name      : void SendCanGroupHisWaveInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送历史故障录波信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupHisWaveInfoMsg(void)
{
	/*自研内部CAN协议使能*/
	#if(1 == BMU_SELF_CAN_EN)
	/*发送长帧*/
	(void)CANBuildSendUpmMsg_API(0xff, 0, 47, 0, GINDEX47_LEN, eCAN0);

	/*监控不在CAN0口*/
	#if(eCAN0 != CAN_UPDATE_PORT)
	(void)CANBuildSendUpmMsg_API(0xff, 0, 47, 0, GINDEX47_LEN, CAN_UPDATE_PORT);
	#endif
	#endif
}

/*=================================================================================================
** @Name      : void SendCanGroupHisDodInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送历史DOD记录信息CAN报文
** @The notes : 用户需要发送时调用(信息变化时)
**===============================================================================================*/
void SendCanGroupHisDodInfoMsg(void)
{
	/*自研内部CAN协议使能*/
	#if(1 == BMU_SELF_CAN_EN)
	/*发送长帧*/
	(void)CANBuildSendUpmMsg_API(0xff, 0, 48, 0, GINDEX48_LEN, eCAN0);

	/*监控不在CAN0口*/
	#if(eCAN0 != CAN_UPDATE_PORT)
	(void)CANBuildSendUpmMsg_API(0xff, 0, 48, 0, GINDEX48_LEN, CAN_UPDATE_PORT);
	#endif
	#endif
}

/*使能自研协议,禁止CATL协议时CAN#1发送单体信息(非CAN#1监控)*/
#if((1 == BMU_SELF_CAN_EN) && (0 == BMU_CATL_CAN_EN) && (eCAN0 != CAN_UPDATE_PORT))
/*=================================================================================================
** @Name      : void SendCanGroupCellVoltInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送单体电压信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupCellVoltInfoMsg(void)
{
	static u8 sTime = 0;
	u16 index = 0;
	u8 num = 0;

	index = sTime * 4;
	num = gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] - index;
	if(num > 4)
	{
		num = 4;
	}

	/*不超过240节电池*/
	if(gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] <= 240)
	{
		/*发送单体信息帧*/
		CANBuildSendUpcMsg_API(0xff, 0, 92, index, num, 50 + sTime, eCAN0);
	}
	/*超过240节电池*/
	else
	{
		/*前240节*/
		if(index < 240)
		{
			/*发送单体信息帧*/
			CANBuildSendUpcMsg_API(0xff, 0, 92, index, num, 50 + sTime, eCAN0);
		}
		else
		{
			/*发送单体信息帧*/
			CANBuildSendUpcMsg_API(0xff, 0, 93, (index - 240), num, 50 + sTime, eCAN0);
		}
	}

	sTime++;
	if((sTime * 4) >= gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
	{
		sTime = 0;
	}
}

/*=================================================================================================
** @Name      : void SendCanGroupCellTempInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送单体温度和均衡温度信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupCellTempInfoMsg(void)
{
	static u8 sTime = 0;
	u16 index = 0;
	u8 num = 0;

	index = sTime * 4;
	num = (gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum]) - index;
	if(num > 4)
	{
		num = 4;
	}

	/*不超过240节电池*/
	if((gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum]) <= 240)
	{
		/*发送单体信息帧*/
		CANBuildSendUpcMsg_API(0xff, 0, 94, index, num, 150 + sTime, eCAN0);
	}
	/*超过240节电池*/
	else
	{
		/*前240节*/
		if(index < 240)
		{
			/*发送单体信息帧*/
			CANBuildSendUpcMsg_API(0xff, 0, 94, index, num, 150 + sTime, eCAN0);
		}
		else
		{
			/*发送单体信息帧*/
			CANBuildSendUpcMsg_API(0xff, 0, 95, (index - 240), num, 150 + sTime, eCAN0);
		}
	}

	sTime++;
	if((sTime * 4) >= (gGBmuHigLevPara_103[eBmuHigLevPara103_TSumNum] + gGBmuHigLevPara_103[eBmuHigLevPara103_BTSumNum]))
	{
		sTime = 0;
	}
}

/*=================================================================================================
** @Name      : void SendCanGroupCellBalanInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送单体均衡状态信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupCellBalanInfoMsg(void)
{
	static u8 sTime = 0;
	u8 index = 0;
	u8 len = 0;
	u8 num = 0;

	index = sTime * 4;
	len = (gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] + 15) / 16 ;
	num = len - index;
	if(num > 4)
	{
		num = 4;
	}

	/*非被动均衡*/
	if(gGHardPara_104[eGHardPara104_BalCirc] > 0)
	{
		/*发送主动充电均衡状态*/
		CANBuildSendUpcMsg_API(0xff, 0, 90, index, num, sTime, eCAN0);
	}

	/*发送被动放电均衡状态*/
	CANBuildSendUpcMsg_API(0xff, 0, 91, index, num, 10 + sTime, eCAN0);

	sTime++;
	if((sTime * 4) >= len)
	{
		sTime = 0;
	}
}

//使能自研从控BSU CAN通信信息才测内阻
#if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))
/*=================================================================================================
** @Name      : void SendCanGroupCellResisInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送单体内阻信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupCellResisInfoMsg(void)
{
	static u8 sTime = 0;
	u16 index = 0;
	u8 num = 0;

	index = sTime * 3;
	num = gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] - index;
	if(num > 3)
	{
		num = 3;
	}

	/*不超过240节电池*/
	if(gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum] <= 240)
	{
		/*发送主动更新长帧(单体信息帧不够映射索引分配)*/
		CANBuildSendUpmMsg_API(0xff, 0, 96, index, num, eCAN0);
	}
	/*超过240节电池*/
	else
	{
		/*前240节*/
		if(index < 240)
		{
			/*发送主动更新长帧(单体信息帧不够映射索引分配)*/
			CANBuildSendUpmMsg_API(0xff, 0, 96, index, num, eCAN0);
		}
		else
		{
			/*发送主动更新长帧(单体信息帧不够映射索引分配)*/
			CANBuildSendUpmMsg_API(0xff, 0, 97, (index - 240), num, eCAN0);
		}
	}

	sTime++;
	if((sTime * 3) >= gGBmuHigLevPara_103[eBmuHigLevPara103_VSumNum])
	{
		sTime = 0;
	}
}
#endif
#endif
//by lxr01202
/*=================================================================================================
** @Name      : void SendCanGroupCellResisRIInfoMsg(void)
** @Input     : void
** @Output    : void
** @Function  : 用户自行发送单体内阻实部和虚部信息CAN报文
** @The notes : 用户需要发送时调用
**===============================================================================================*/
void SendCanGroupCellResisRIInfoMsg(void)
{
	/*发送主动更新长帧(单体信息帧不够映射索引分配)*/
	CANBuildSendUpmMsg_API(0xff, 0, 226, 0, GINDEX92_LEN, CAN_UPDATE_PORT);
	CANBuildSendUpmMsg_API(0xff, 0, 228, 0, GINDEX92_LEN, CAN_UPDATE_PORT);
    #if(GINDEX93_LEN > 0)
	/*发送主动更新长帧(单体信息帧不够映射索引分配)*/
	CANBuildSendUpmMsg_API(0xff, 0, 227, 0, GINDEX93_LEN, CAN_UPDATE_PORT);
	CANBuildSendUpmMsg_API(0xff, 0, 229, 0, GINDEX93_LEN, CAN_UPDATE_PORT);
	#endif	
}
/*=================================================================================================
** @Static function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u8 SendUserUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u16 *data, u8 len, u8 canNum)
** @Input     : destAddr:目标地址 attrib:帧属性 mIndex:主索引号 data:需发送数组 len:数组长度 canNum:CAN通道号
** @Output    : 发送结果(0:正常,其它:错误码)
** @Function  : 用户组建和发送主动更新单帧报文帧
** @The notes : 用户自行发送,非通过CAN通信包
**===============================================================================================*/
static u8 SendUserUpsMsg_API(u8 destAddr, u8 attrib, u8 mIndex, u16 *data, u8 len, u8 canNum)
{
	u8 i = 0;
    t_CANMSG msg;

    if(canNum >= eCANNUM)
    {
        return(FALSE);
    }

    /*已禁止周期发送*/
    if(0 == gCANTxSinDef_13[0])
    {
        return(FALSE);
    }

    /*主动发送单帧ID*/
    msg.ID = 0x14000000 | (((u32)(attrib & 0x01)) << 24)   | (((u32)mIndex) << 16)  | (((u16)destAddr) << 8) | (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev];

    /*报文长度*/
    msg.Len = 8;

    /*单帧数据域(低位在前)*/
    for(i = 0; i < len; i++)
    {
    	if(i < 4)
    	{
			msg.Data[i * 2] = data[i] & 0xff;
			msg.Data[1 + (i * 2)] = (data[i] >> 8) & 0xff;
    	}
    	else
    	{
    		break;
    	}
    }

    /*向指定CAN通道发送*/
    return(BSPCANSendQin(canNum, &msg));
}
