/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : GroupInfo.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     :
** @Instructions :
**===========================================================================================*/
#ifndef FUNTION_SYSTEMINFO_GROUPINFO_H_
#define FUNTION_SYSTEMINFO_GROUPINFO_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "PublicInfo.h"
#include "HigAFESamp.h"
#include "LowADCSamp.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define SLAVE_MAX_NUM			(MAX_BSU_NUM)											/*从控数目*/
#define SLAVE_MAX_CELVNUM		(MAX_BSU_CELL)											/*一个从控单体电压数目*/
#if(0 == BMU_BSU_COM_TYPE)                  /*BMU主从菊花链通信*/
#define SLAVE_MAX_CELTNUM		(4)														/*一个从控单体温度数目*/
#define SLAVE_MAX_BALTNUM		(2)														/*一个从控板温温度数目*/
#elif(0 == BMU_BSU_CAN_TYPE)                /*BMU主从自研CAN通信*/
#define SLAVE_MAX_CELTNUM		(16)													/*一个从控单体温度数目*/
#define SLAVE_MAX_BALTNUM		(2)														/*一个从控板温温度数目(BYD AFE4+2时均衡温度占用单体温度空间)*/
#else                                       /*BMU主从高特CAN通信*/
#define SLAVE_MAX_CELTNUM		(10)													/*一个从控单体温度数目*/
#define SLAVE_MAX_BALTNUM		(0)														/*一个从控板温温度数目*/
#endif
#define GROUP_MAX_CELVNUM		(SLAVE_MAX_NUM*SLAVE_MAX_CELVNUM)						/*组内单体电压总数目*/
#define GROUP_MAX_CELTNUM		(SLAVE_MAX_NUM*SLAVE_MAX_CELTNUM)						/*组内单体温度总数目*/
#define GROUP_MAX_BALTNUM		(SLAVE_MAX_NUM*SLAVE_MAX_BALTNUM)						/*组内均衡板温总数目*/
#if(SLAVE_MAX_CELTNUM >= 16)
#define GROUP_MAX_SUMTNUM		(SLAVE_MAX_NUM*SLAVE_MAX_CELTNUM)						/*组内从控温度总数目(满16节单体温度时没有均衡板温)*/
#else
#define GROUP_MAX_SUMTNUM		(SLAVE_MAX_NUM*(SLAVE_MAX_CELTNUM+SLAVE_MAX_BALTNUM))	/*组内从控温度总数目(包括单体温度和均衡板温)*/
#endif

#define BMU_EXT_COM_DEV			(0)			/*主控外部(上层)通信设备(0:不检测 1:PCS 2BSMU)*/

#define BMU_PWR_SWH_TYPE		(1)			/*BMU电源接触器类型(0:无电源接触器1:独立电源接触器2:共用负极接触器)*/
#define BMU_DC_VH_TYPE			(1)			/*BMU直流端高压采样值获取方式(0:无高压采样 1:自身采样 2:通讯获取)*/
#define	BMU_NEG_SWH_EN			(1)			/*BMU负极接触器控制使能(0:禁止 1:使能)*/
#define	BMU_FAN_SWH_EN			(1)			/*BMU风扇接触器控制使能(0:禁止 1:使能)*/
#define	BMU_HOT_SWH_EN			(1)			/*BMU加热接触器控制使能(0:禁止 1:使能)*/
#define	BMU_SMG_SWH_EN			(1)			/*BMU烟感接触器控制使能(0:禁止 1:使能)*/
#define	BMU_LED_SWH_EN			(1)			/*BMU状态LED显示控制使能(0:禁止 1:使能)*/
#define	BMU_CDHG_DO_EN			(1)			/*BMU充放电干接点同一DO3使能(0:禁止 1:使能)*/
#define BMU_SWH_BACK_TYPE		(0)			/*BMU接触器回检方式(0:不回检(检控制信号) 1:板级信号回检 2:板级采样回检(驱动电流) 3:外部信号回检*/

#define BMU_CELL_R_TRIG_T		(604800000)	/*BMU触发从机电芯内阻检测周期(7天,单位1ms*/

#define BMU_FAN_PWR_EN			(0)			/*BMU控制PWM调速风扇电源使能[复用风扇接触器管脚](0:禁止(主控全速风扇控制) 1:使能)*/
#define BMU_AIR_PWR_EN			(0)			/*BMU控制空调电源使能[复用加热接触器管脚](0:禁止 1:使能)*/

#define BMU_PWM_FAN_EN			(0)			/*BMU控制IMLU散热PWM调速风扇使能[IMLU1控制](0:禁止 1:使能)*/
#define BMU_PWM_FAN_NUM			(16)		/*BMU控制IMEU3风扇通道数*/
#define BMU_LU_FLOOD_HZ			(0)			/*BMU检测IMLU水浸告警频率(1Hz,0表示不检测)*/

#define BMU_IMEU_COM_EN			(0)			/*BMU与IMEU2通信控制交互使能(0:禁止 1:使能)*/
#define BMU_EU_SHUTFAN_EN		(1)			/*BMU控制IMEU百叶风机使能*/
#define BMU_EU_FIRELIG_EN		(1)			/*BMU控制IMEU照明/声光报警/消防等使能*/
#define BMU_EU_FLOOD_HZ			(300)		/*BMU检测IMEU水浸告警频率(1Hz,0表示不检测)*/
#define BMU_EU_GAS1_CONC		(2500)		/*BMU检测IMEU可燃气体1(H2)浓度告警值(0.01%,0xffff表示不检测)*/
#define BMU_EU_GAS2_CONC		(2500)		/*BMU检测IMEU可燃气体2(CO)浓度告警值(0.01%,0xffff表示不检测)*/
//ZQD808添加，宏定义改为参数
//#define BMU_IMEU3_COM_EN		(0)			/*BMU与IMEU3通信控制交互使能(0:禁止 1:使能)*/

//#define BSU02U1_TEPMER_ALARM    (150)		/*PT1000铜排温度告警温度*/
#define BSU02U1_AERO_ALARM      (5000)		/*PT1000气溶胶浓度告警浓度*/

#define BSU02U1_MSDDUTY_ALARM1   (30)		/*MSD告警占空比下限*/
#define BSU02U1_MSDDUTY_ALARM2   (85)		/*MSD告警占空比上限*/
#define BSU02U1_MSDAMP_ALARM1    (100)		/*MSD告警幅值下限*/
#define BSU02U1_MSDAMP_ALARM2    (200)		/*MSD告警幅值上限*/

#define BSU02U1_AERO_ALARM      (5000)		/*PT1000气溶胶浓度告警浓度*/
//ZQD808添加,宏定义现改为配置参数表
//#define BMU_IMLU2_EN        	  (1)       /*BMU灯带控制功能使能 0：不使能   1：使能*/
//#define BMU_LIQUIDCOOL_AIR_485_EN (1)       /*使能液冷空调485通信*/
//#define BMU_LIQUIDCOOL_AIR_TYPE   (0)       /*液冷空调类型 0：英飞源自研 1：同飞*/
//#define BMU_LEAKAGE_EN            (1)     /*漏电流传感器功能使能 0：不使能 1：使能*/


#define GINDEX45_LEN			(11)		/*历史故障记录信息查询数组长度*/
#define GINDEX46_LEN			(11)		/*历史事件记录信息查询数组长度*/
#define GINDEX47_LEN			(56)		/*历史故障录波信息查询数组长度*/
#define GINDEX48_LEN			(56)		/*历史DOD记录信息查询数组长度*/

#define GINDEX50_LEN			(4)			/*系统状态信息数组长度*/
#define GINDEX51_LEN			(4)			/*电流功率限值信息数组长度*/
#define GINDEX52_LEN			(2)			/*电压限值信息数组长度*/
#define GINDEX53_LEN			(7)			/*告警信息数组长度*/
#define GINDEX54_LEN			(4)			/*故障信息数组长度*/
#define GINDEX55_LEN			(2)			/*故障定位信息数组长度*/
#define GINDEX56_LEN			(4)			/*输入输出口状态信息数组长度*/
#define GINDEX60_LEN			(4)			/*系统整体信息数组长度*/
#define GINDEX61_LEN			(4)			/*系统电量信息数组长度*/
#define GINDEX62_LEN			(4)			/*系统单体信息数组长度*/
#define GINDEX63_LEN			(4)			/*电压特征信息数组长度*/
#define GINDEX64_LEN			(4)			/*温度特征信息数组长度*/
#define GINDEX65_LEN			(4)			/*直流电流温度信息数组长度*/
#define GINDEX66_LEN			(4)			/*低压采样信息数组长度*/
#define GINDEX67_LEN			(4)			/*直流电路信息数组长度*/
#define GINDEX68_LEN			(4)			/*高压驱动电流信息数组长度*/
#define GINDEX69_LEN			(4)			/*高压驱动电流信息数组长度*/
#define GINDEX70_LEN			(4)			/*时间统计信息数组长度*/
#define GINDEX71_LEN			(4)			/*故障统计信息数组长度*/
#define GINDEX72_LEN			(4)			/*当前能量统计信息数组长度*/
#define GINDEX73_LEN			(2)			/*累计电量统计信息数组长度*/
#define GINDEX74_LEN			(2)			/*累计电能统计信息数组长度*/
#define GINDEX75_LEN			(4)			/*放电深度统计信息数组长度*/
#define GINDEX76_LEN			(4)			/*安全等级统计信息数组长度*/
#define GINDEX80_LEN			(2)			/*权限级别状态信息数组长度*/
#define GINDEX81_LEN			(2)			/*控制结果状态信息数组长度*/
#define GINDEX82_LEN			(10)		/*调试结果状态信息数组长度*/
#define GINDEX83_LEN			(3)			/*校准结果状态信息数组长度*/
#define GINDEX85_LEN			(2)			/*权限密码登录信息数组长度*/
#define GINDEX86_LEN			(33)		/*系统控制命令信息数组长度*/
#define GINDEX87_LEN			(13)		/*用户控制命令信息数组长度*/
#define GINDEX88_LEN			(8)			/*系统调试命令信息数组长度*/
#define GINDEX89_LEN			(2)			/*采样校准命令信息数组长度*/
#define GINDEX90_LEN			(25)		/*充电均衡状态信息数组长度*/
#define GINDEX91_LEN			(25)		/*放电均衡状态信息数组长度*/
#define GINDEX110_LEN			(25)		/*Safe状态信息数组长度*/

#if(GROUP_MAX_CELVNUM > 240)
#define GINDEX92_LEN			(240)						/*电池单体电压信息1数组长度*/
#define GINDEX93_LEN			(GROUP_MAX_CELVNUM-240)		/*电池单体电压信息2数组长度*/
#else
#define GINDEX92_LEN			(GROUP_MAX_CELVNUM)			/*电池单体电压信息1数组长度*/
#define GINDEX93_LEN			(0)							/*电池单体电压信息2数组长度*/
#endif

#if(GROUP_MAX_SUMTNUM > 240)
#define GINDEX94_LEN			(240)						/*电池单体温度信息1数组长度(包括均衡板温个数)*/
#define GINDEX95_LEN			(GROUP_MAX_SUMTNUM-240)		/*电池单体温度信息2数组长度(包括均衡板温个数)*/
#else
#define GINDEX94_LEN			(GROUP_MAX_SUMTNUM)			/*电池单体温度信息1数组长度(包括均衡板温个数)*/
#define GINDEX95_LEN			(0)							/*电池单体温度信息2数组长度(包括均衡板温个数)*/
#endif

#define GINDEX115_LEN       	(15)						/*不同温度充电倍率表长度*/
#define GINDEX116_LEN       	(15)						/*不同温度放电倍率表长度*/
#define GINDEX119_LEN      		(44)						/*电压/SOC平滑电流限值参数表长度*/

#define GINDEX200_LEN			(eReferSampInfo200_Len)		/*主控内部参考采集信息*/
#define GINDEX201_LEN			(eCellBalanT201_Len)		/*单体均衡温度特征信息*/
#define GINDEX202_LEN			(eEu3AirInfo202_Len)		/*EU3空调控制信息*/
#define GINDEX203_LEN			(eEu3AirState203_Len)		/*EU3空调状态信息*/

//ZQD808添加
#define GINDEX206_LEN           (eLiquidAirInfo206_Len)     /*液冷空调控制信息*/
#define GINDEX207_LEN           (eLiquidAirSta207_Len)     	/*液冷空调状态信息*/
#define GINDEX208_LEN           (6)     					/*液冷空调告警信息*/
#define GINDEX209_LEN           (ePackEnvInfo209_Len)     	/*液冷PACK环境信息*/
#define GINDEX210_LEN           (ePackMsdInfo210_Len)     	/*液冷PACK高压互锁信息*/
#define GINDEX211_LEN           (eAerosolInfo211_Len)     	/*液冷PACK气溶胶信息*/
#define GINDEX212_LEN           (eATFUserInfo212_Len)     	/*同飞空调用户参数信息*/
#define GINDEX213_LEN           (eATFUserState213_Len)     	/*同飞空调用户状态信息*/
#define GINDEX214_LEN           (eATFErrInfo214_Len)     	/*同飞空调故障信息*/

#define GINDEX215_LEN			(eXinMiaoAirSta215_Len)		/*IMEU辅助板接收信息1*/
#define GINDEX216_LEN			(eXinMiaoAirPar216_Len)		/*IMEU辅助板温湿度信息2*/

#define GINDEX217_LEN           (eIMEU3217_Len)     		/*IMEU3温湿度信息*/
#define GINDEX218_LEN           (ePduIn218_Len)     		/*PDU内IMEU3信息*/
#define GINDEX219_LEN           (ePduOut219_Len)     		/*PDU外IMEU3信息*/
#define GINDEX220_LEN           (eDeInfo220_Len)     		/*除湿机信息*/

#define GINDEX223_LEN           (eBsuBotTopCount223_Len)    /*每个bsu从控bot和top枚举计数*/

#define GINDEX225_LEN           (eLeakageInfo225_Len)       /*漏电流传感器信息*/


#define GINDEX254_LEN			(eFireInfo254_Len)	        /*消防信息数组长度*/
#define GINDEX255_LEN			(ePackTempInfo255_Len)	    /*PACK温度特征信息数组长度*/




/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*协议48子索引列表*/
typedef enum
{
	eHisDodInfo48_Id  = 0,				    /*DOD记录序号*/
	eHisDodInfo48_Num = 1,				    /*DOD记录显示个数*/
	eHisDodInfo48_Y   = 2,				    /*DOD记录时间-年*/
	eHisDodInfo48_MD  = 3,				    /*DOD记录时间-月日*/
	eHisDodInfo48_HM  = 4,				    /*DOD记录时间-时分*/
	eHisDodInfo48_S   = 5,				    /*DOD记录时间-秒*/
	eHisDodInfo48_Dod = 6,				    /*DOD记录值1*/

}e_HisDodInfo48;

/*协议50子索引列表*/
typedef enum
{
	eSysStateInfo50_SysPro = 0,				/*系统保护状态[0,5]		0:未就绪 1:正常 2:充电截止 3:放电截止 4:充放电限制 5:充放电保护*/
	eSysStateInfo50_SysWor = 1,				/*系统工作状态[0,6]		0:开机,1:待命,2:启动,3:运行,4:停机,5:故障 6:关机*/
	eSysStateInfo50_SysChg = 2,				/*电池充放电状态[0,2]	0:空闲 1:充电 2:放电*/
	eSysStateInfo50_SysMod = 3				/*系统工作模式[0,5]		0:自动 1:手动 2:紧急 5:调试*/

}e_SysStateInfo50;

/*协议51子索引列表*/
typedef enum
{
	eLimCPInfo51_ChgC = 0,					/*最大允许充电电流限值(PCS充电输入最大电流)[0,10000]0.1A*/
	eLimCPInfo51_DhgC = 1,					/*最大允许放电电流限值(PCS放电输出最大电流)[0,10000]0.1A*/
	eLimCPInfo51_ChgP = 2,					/*最大允许充电功率限值(PCS充电输入最大功率)[0,10000]0.1kW*/
	eLimCPInfo51_DhgP = 3,					/*最大允许放电功率限值(PCS放电输出最大功率)[0,10000]0.1kW*/

}e_LimCPInfo_51;

/*协议52子索引列表*/
typedef enum
{
	eLimVInfo52_ChgV = 0,					/*最大允许充电电压限值(PCS充电截止电压)[0,10000]0.1V*/
	eLimVInfo52_DhgV = 1					/*最大允许放电电压限值(PCS放电截止电压)[0,10000]0.1V*/

}e_LimVInfo_52;

/*协议53子索引列表*/
typedef enum
{
	eAlarmInfo53_0 = 0,						/*电池告警信息1*/
	eAlarmInfo53_1 = 1,						/*电池告警信息2*/
	eAlarmInfo53_2 = 2						/*电池告警信息3*/

}e_AlarmInfo53;

/*协议54子索引列表*/
typedef enum
{
	eFaultInfo54_0 = 0,						/*电池故障信息1*/
	eFaultInfo54_1 = 1,						/*电池故障信息2*/
	eFaultInfo54_2 = 2						/*电池别故障信息3*/

}e_FaultInfo54;

/*协议55子索引列表*/
typedef enum
{
	eErrDetail55_Code = 0,				    /*电池故障定位码*/
	eErrDetail55_Info = 1,					/*电池故障定位信息*/

}e_ErrDetail55;

/*协议56子索引列表*/
typedef enum
{
	ePortStateInfo56_Input = 0,				/*输入端口回检状态*/
	ePortStateInfo56_Output = 1,			/*输出端口控制状态*/
	ePortStateInfo56_InInside = 2,			/*输入内部回检状态*/
	ePortStateInfo56_OutInside = 3			/*输出内部控制状态*/

}e_PortStateInfo56;

/*协议60子索引列表*/
typedef enum
{
	eSysMainInfo60_SumV = 0,				/*总电压采样值[0,10000]0.1V*/
	eSysMainInfo60_OutC = 1,				/*总电流采样值[-30000,30000]0.1A*/
	eSysMainInfo60_OutP = 2,				/*输出功率值[-30000,30000]0.1kW*/
	eSysMainInfo60_PreV = 3					/*预充电压值[0,10000]0.1V*/

}e_SysMainInfo60;

/*协议61子索引列表*/
typedef enum
{
	eSysCapInfo61_SOC = 0,					/*电池SOC值[0,1000]0.1%*/
	eSysCapInfo61_SOE = 1,					/*电池SOE值[0,1000]0.1%*/
	eSysCapInfo61_SOH = 2,					/*电池SOH值[0,1000]0.1%*/
	eSysCapInfo61_AllCap = 3				/*电池组总容量[0,1000]0.1AH*/

}e_SysCapInfo61;

/*协议62子索引列表*/
typedef enum
{
	eSysCellInfo62_SumV = 0,				/*单体总电压值[0,10000]0.1V*/
	eSysCellInfo62_AvgV = 1,				/*单体平均电压值[0,5000]1mV*/
	eSysCellInfo62_AvgT = 2,				/*单体平均温度值[-40,105]1℃*/
	eSysCellInfo62_EnvT = 3		    		/*系统环境温度值[-40,105]1℃*/

}e_SysCellInfo62;


/*协议63子索引列表*/
typedef enum
{
	eVoltCharInfo63_MaxVNum = 0,			/*单体最大电压序号[1,400]*/
	eVoltCharInfo63_MaxVolt = 1,			/*单体最大电压值[0,5000]1mV*/
	eVoltCharInfo63_MinVNum = 2,			/*单体最小电压序号[1,400]*/
	eVoltCharInfo63_MinVolt = 3				/*单体最小电压值[0,5000]1mV*/

}e_VoltCharInfo63;

/*协议64子索引列表*/
typedef enum
{
	eTempCharInfo64_MaxTNum = 0,			/*单体最高温度序号[1,400]*/
	eTempCharInfo64_MaxTemp = 1,			/*单体最高温度值[-40,105]1℃*/
	eTempCharInfo64_MinTNum = 2,			/*单体最低温度序号[1,400]*/
	eTempCharInfo64_MinTemp = 3				/*单体最低温度值[-40,105]1℃*/

}e_TempCharInfo64;

/*协议65子索引列表*/
typedef enum
{
	eDcCTInfo65_HallSignV = 0,				/*霍尔信号原始电压采样值[0,5000]1mV*/
	eDcCTInfo65_ShutSignV = 1,				/*分流器信号原始电压采样值[-7500,7500]0.01mV*/
	eDcCTInfo65_ShutT = 2,					/*分流器温度采样值[-40,180]1℃*/
	eDcCTInfo65_PowrT = 3					/*电源侧温度采样值[-40,126]1℃*/

}e_DcCTInfo65;

/*协议66子索引列表*/
typedef enum
{
	eLVSamp66_NegV = 0,						/*负极电压采样值[0,10000]0.1V*/
	eLVSamp66_PwrV = 1,						/*低压电源电压采样值[0,3300]0.01V*/
	eLVSamp66_AccV = 2,						/*激活信号ACC电压采样值[0,3300]0.01V*/
	eLVSamp66_Hall5V = 3					/*霍尔电源电压采样值[0,500]0.01V*/

}e_LVSampInfo66;

/*协议67子索引列表*/
typedef enum
{
	eFstLeakInfo67_FcV = 0,					/*充电枪KC供入电压采样值[0,3300]0.01V*/
	eFstLeakInfo67_CC2V = 1,				/*充电枪CC2信号电压采样值[0,500]0.01V*/
	eFstLeakInfo67_PasInsu = 2,				/*正极对地绝缘阻值[0,32767]1kΩ*/
	eFstLeakInfo67_NegInsu = 3				/*负极对地绝缘阻值[0,32767]1kΩ*/

}e_FstLeakInfo67;

/*协议68子索引列表*/
typedef enum
{
	eHsCurrInfo68_MHS0C = 0,				/*HS1通道1驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo68_MHS1C = 1,				/*HS1通道2驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo68_MHS2C = 2,				/*HS2通道1驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo68_MHS3C = 3					/*HS2通道2驱动电流采样值[0,500]0.01A*/

}e_HsCurrInfo68;

/*协议69子索引列表*/
typedef enum
{
	eHsCurrInfo69_MHS4C = 0,				/*HS3通道1驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo69_MHS5C = 1,				/*HS3通道2驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo69_MHS6C = 2,				/*HS4通道1驱动电流采样值[0,500]0.01A*/
	eHsCurrInfo69_MHS7C = 3					/*HS4通道2驱动电流采样值[0,500]0.01A*/

}e_HsCurrInfo69;

/*协议70子索引列表*/
typedef enum
{
	eTime70_SysRun = 0,						/*系统运行时长[0,65535]1Hour*/
	eTime70_CanChg = 1,						/*可充电时间[0,65535]1Min*/
	eTime70_CanDhg = 2,						/*可放电时间[0,65535]1Min*/
	eTime70_ChgCycle = 3				    /*充放电循环次数[0,65535]1次*/

}e_StatistTime70;

/*协议71子索引列表*/
typedef enum
{
	eErr71_MonWarNum = 0,					/*单月告警数目[0,65535]*/
	eErr71_MonErrNum = 1,					/*单月故障数目[0,65535]*/
	eErr71_HisWarNum = 2,					/*累计告警数目[0,65535]*/
	eErr71_HisErrNum = 3					/*累计故障数目[0,65535]*/

}e_StatistErr71;

/*协议72子索引列表*/
typedef enum
{
	eCapEner72_NowEner = 0,					/*电池组当前电能[0,65535]0.1kWh*/
	eCapEner72_AllEner = 1,					/*电池组总电能[0,65535]0.1kWh*/
	eCapEner72_ChgCap = 2,					/*电池组单次充电电量[0,65535]1AH*/
	eCapEner72_DhgCap = 3,					/*电池组单次放电电量[0,655351]AH*/

}e_StatistCapEner72;

/*协议73子索引列表*/
typedef enum
{
	eHisCap73_HisChgCap = 0,				/*电池组累计充电电量[0,65535]1AH*/
	eHisCap73_HisDhgCap = 1					/*电池组累计放电电量[0,65535]1AH*/

}e_StatistHisCap73;

/*协议74子索引列表*/
typedef enum
{
	eHisEner74_HisChgEner = 0,				/*电池组累计充电电能[0,65535]0.1kWh*/
	eHisEner74_HisDhgEner = 1				/*电池组累计放电电能[0,65535]0.1kWh*/

}e_StatistHisEner74;

/*协议75子索引列表*/
typedef enum
{
	eDodStat75_NowDod = 0,				   /*最新放电深度DOD(单次)[0,1000]0.1%*/
	eDodStat75_FstDod = 1,				   /*最新放电起始DOD(单次)[0,1000]0.1%*/
	eDodStat75_MaxDod = 2,				   /*最大放电深度DOD(单次)[0,1000]0.1%*/
	eDodStat75_AvgDod = 3				   /*平均放电深度DOD(平均)[0,1000]0.1%*/

}e_StatistDodInfo75;

/*协议76子索引列表*/
typedef enum
{
	eDiagSos76_NowSos = 0,				   /*当前安全等级SOS[0,4](等级越低越安全)*/
	eDiagSos76_NowRsn = 1,				   /*当前SOS等级异常原因[按bit位表示]*/
	eDiagSos76_PreSos = 2,				   /*预测安全等级SOS[0,4](等级越低越安全)*/
	eDiagSos76_PreRsn = 3				   /*预测SOS等级异常原因[按bit位表示]*/

}e_StatistSosInfo76;

//协议81子索引列表
typedef enum
{
	eCtrlRes81_AdhCheck = 0,				/*粘连检测控制结果状态 0:自动检测 1:正在检测 2:停止检测 3:检测完成*/
	eCtrlRes81_LeakCheck					/*绝缘检测控制结果状态 0:自动检测 1:正在检测 2:停止检测 3:检测完成*/

}e_CtrlHandleResult81;

//协议83子索引列表
typedef enum
{
	eCorrRes83_CorrType = 0,				/*采样校准类型	0:电流零飘手动校准 1:充电电流单点校准 2:放电电流单点校准 3:充电电流多点校准 4:放电电流多点校准 5:高压单点校准 6:高压多点校准*/
	eCorrRes83_CorrState,					/*采样校准状态	0:未开始 [1,99]:校准过程 100:校准成功 101:校准失败*/
	eCorrRes83_CorrResult					/*采样校准结果	按位表示,每bit为1个校准类型[0:未校准,1:已校准] bit0:电流零飘校准 bit1:充电电流校准 bit2:放电电流校准 bit3:高压单点校准*/

}e_SampCorrResult83;

/*协议87子索引列表*/
typedef enum
{
	eUserCtrl87_CellTrig = 0,				/*单体信息触发命令	"每bit(0-退出,1-触发):bit0:单体电压信息触发;bit1:单体温度信息触发;bit2:单体均衡状态信息触发."*/
	eUserCtrl87_InTran = 1,					/*内部报文转发命令	0-不触发不转发,1-触发内部协议 2-转发内部报文 3-全部触发转发*/
	eUserCtrl87_HisErrSw = 2,				/*历史故障切换命令	0-最新记录,[1,30]-指定日期记录,90-首页记录,91-下一页记录*/
	eUserCtrl87_HisEvnSw = 3,				/*历史事件切换命令	0-最新记录,[1,30]-指定日期记录,90-首页记录,91-下一页记录*/
	eUserCtrl87_HisWavSw = 4,				/*故障录波切换命令	0-最新记录,[1,30]-指定日期记录,90-首条记录,91-下一条记录*/
	eUserCtrl87_HisDodSw = 5,				/*DOD记录切换命令	0-最新记录,90-首页记录,91-下一页记录*/
	eUserCtrl87_AdhCheck = 6,				/*粘连检测控制命令	0:自动检测 1:开始检测(连续检测) 2:停止检测*/
	eUserCtrl87_LeakCheck = 7,				/*绝缘检测控制命令	0:自动检测 1:开始检测(连续检测) 2:停止检测*/
	eUserCtrl87_ShutFan = 8,				/*百叶风机控制命令	0:关闭 1:开启*/
	eUserCtrl87_Lighting = 9,				/*电池柜照明控制命令	0:关闭 1:手动开启   2:手动关闭)*/
	eUserCtrl87_ZHSAForce = 10,				/*声光报警强制控制命令	0:关闭 1:强制开启 2:强制关闭*/
	eUserCtrl87_FireTrig = 11,				/*气溶胶消防主动触发命令	0:关闭 1:触发 (预留)*/
	eUserCtrl87_QF3Ctr = 12					/*液冷备电QF3控制命令	0:关闭 1:开启*/
}e_UserCtrl87;

/*协议89子索引列表*/
typedef enum
{
    eCorrCmd89_Type = 0,					/*采样校准类型	0:电流零飘手动校准 1:充电电流单点校准 2:放电电流单点校准 3:充电电流多点校准 4:放电电流多点校准 5:高压单点校准 6:高压多点校准 255:终止校准*/
	eCorrCmd89_Value						/*校准点基准值	基准电压 0.1V /基准电流 0.01A*/

}e_SampCorrCmd89;

/*协议200子索引列表*/
typedef enum
{
	eReferSampInfo200_McuPwr5V = 0,			/*MCU供电电源电压采样值[0,500]0.01V*/
	eReferSampInfo200_SbcAux5V,				/*SBC芯片5V输出AUX通道采样值[0,500]0.01V*/
	eReferSampInfo200_SbcMuxV,				/*SBC芯片MUX通道电压采集[0,500]0.01V*/
	eReferSampInfo200_PwmAdcV,				/*PWM电压ADC通道采样值[0,500]0.01V*/
	eReferSampInfo200_PwmCycle,				/*PWM输入捕获通道频率*/
	eReferSampInfo200_Ltc2949Vref,			/*LTC2949芯片基准源,基准回采[0,300]0.01V*/
	eReferSampInfo200_Ltc2949_5V,			/*LTC2949芯片高压侧5V电压采集[0,500]0.01V*/
	eReferSampInfo200_BattHV,				/*电池总电压采样,BATT电压采集[0,10000]0.1V*/
	eReferSampInfo200_PChgHV,				/*预充电压采样,外端电压采集[0,10000]0.1V*/
	eReferSampInfo200_Link1,				/*link+电压采样,预充电压正端电压采集[0,5000]1mV*/
	eReferSampInfo200_Link2,				/*link-电压采样,预充电压负端电压采集[0,5000]1mV*/
	eReferSampInfo200_LoadV7,				/*负载电压原始采样值,V7电压采集[0,10000]1mV*/

	eReferSampInfo200_Len					/*采样中间参考值长度*/
}e_ReferSampInfo200;

/*协议201子索引列表*/
typedef enum
{
	eCellBalanT201_GMaxTemp = 0,			/*单体均衡温度组内最大值[-40,125]1℃*/
	eCellBalanT201_GMaxTSlav = 1,			/*单体均衡温度最大值从控号[0,239]*/
	eCellBalanT201_SMaxTemp0 = 2,			/*单体均衡温度1#从控内最大值[-40,125]1℃*/
	eCellBalanT201_SMaxTemp1 = 3,			/*单体均衡温度2#从控内最大值[-40,125]1℃*/

	eCellBalanT201_Len = MAX_BSU_NUM+2,		/*单体均衡温度特征信息*/
}e_CellBalanTInfo201;

/*协议202子索引列表*/
typedef enum
{
	eEu3AirInfo202_CellPwrState = 0,		/*设置电池测液路开关机  0：关机    1：开机*/
	eEu3AirInfo202_PcsPwrState = 1,			/*设置模块测液路开关机  0：关机    1：开机*/
	eEu3AirInfo202_Mode = 2,			    /*设置模式   1：制冷模式 2：制热模式 3：自循环模式 4：自动模式 5：单独运行模块侧水路模式，只运行模块侧水路和风机*/
	eEu3AirInfo202_ControlObj = 3,		    /*进出水控制方式 0：控制出水温度 1：控制进水温度*/
	eEu3AirInfo202_CoolTemp = 4,		    /*设置制冷温度，单位0.1℃*/
	eEu3AirInfo202_HeatTemp = 5,		    /*设置制热温度，单位0.1℃*/
	eEu3AirInfo202_NoiseState = 6,         /*设置降噪模式6：正常  0：自动  1-5：不同HZ*/
	eEu3AirInfo202_CoolOpenT = 7,		    /*设置制冷启动温差，单位0.1℃*/
	eEu3AirInfo202_CoolCloseT = 8,		    /*设置制冷停止温度，单位0.1℃*/
	eEu3AirInfo202_HeatOpenT = 9,		    /*设置制热启动温差，单位0.1℃*/
	eEu3AirInfo202_HeatCloseT = 10,		    /*设置制热停止温度，单位0.1℃*/
	eSysCmdCtrl86_Eu3AirCell_WaterPump_CmdManual = 11,        //电池侧水泵手动标志          0=自动 1=手动
    eSysCmdCtrl86_Eu3AirCell_WaterPump_SpdManual = 12,        //电池侧水泵手动转速          0~100%
    eSysCmdCtrl86_Eu3AirMod_WaterPump_CmdManual = 13,         //模块侧水泵手动标志          0=自动 1=手动
    eSysCmdCtrl86_Eu3AirMod_WaterPump_SpdManual = 14,         //模块侧水泵手动转速          0~100%
    eSysCmdCtrl86_ElecHeatFilm_CmdManual = 15,                //电子膨胀阀手动标志          0=自动 1=手动
    eSysCmdCtrl86_ElecHeatFilm_OpenPercManual = 16,           //电子膨胀阀手动开度          0~480脉冲
    eSysCmdCtrl86_Fan_CmdManual = 17,                         //风机手动标志               0=自动 1=手动
    eSysCmdCtrl86_Fan_SpdPercManual = 18,                     //风机手动转速               0~100%
    eSysCmdCtrl86_Compressor_CmdManual = 19,                  //压缩机手动标志             0=自动 1=手动（故障或关机自动清0）
    eSysCmdCtrl86_Compressor_FreqManual = 20,                 //压缩机手动频率             0~80Hz
    eSysCmdCtrl86_CellHeat_CmdManual = 21,                    //电加热手动标志             0=自动 1=手动
    eSysCmdCtrl86_CellHeat_OnOffManual = 22,                  //电加热手动开关             0=关 1=开
    eSysCmdCtrl86_BatHeatFilm_CmdManual = 23,                  // 电池加热膜手动标志         0=自动 1=手动
    eSysCmdCtrl86_BatHeatFilm_OnOffManual = 24,               //电池加热膜手动开关          0=关 1=开
    eSysCmdCtrl86_CompressorDrv_Fan_CmdManual = 25,           //压缩机驱动器散热风机手动标志
    eSysCmdCtrl86_CompressorDrv_Fan_SpdPercManual = 26,      //驱动器散热风机手动转速     0~100%
	eEu3AirInfo202_Len		                /*EU3空调控制信息数组长度*/
}e_Eu3AirInfo202;

/*协议203子索引列表*/
typedef enum
{
	eEu3AirState203_SV = 0,			    	/*程序软件版本*/
	eEu3AirState203_PwrState = 1,			/*实际开关状态  0：关机    1：开机*/
	eEu3AirState203_RunMode = 2,			/*实际运行模式 1：制冷 2：制热 3：自循环 4：自动  5：单独运行模块侧水路模式，只运行模块侧水路和风机*/
	eEu3AirState203_Runstep = 3,		    /*实际运行阶段 0：待机 1：制冷启动 2：制冷运行 3：制冷停机 4：自然冷却运行 5：自然冷却停止*/
	eEu3AirState203_ErrCode = 4,		    /*故障ASCII码*/
	eEu3AirState203_TarTemp = 5,		    /*实际目标温度，单位0.1℃*/
	eEu3AirState203_FeedTemp = 6,		    /*实际反馈水温，单位0.1℃*/
	eEu3AirState203_Overheated = 7,		    /*实际吸气过热度，单位0.1℃*/
	eEu3AirState203_LowPreSatTemp = 8,		/*低压对应饱和温度，单位0.1℃*/
	eEu3AirState203_OpenVal = 9,		    /*电子膨胀阀开度，单位步*/
	eEu3AirState203_OutCap = 10,		    /*总输出能力，单位%*/
	eEu3AirState203_ComTarFre = 11,		    /*压缩机目标频率，HZ*/
	eEu3AirState203_ComRunFre = 12,		    /*压缩机运行频率，HZ*/
	eEu3AirState203_ComRunStep = 13,		/*压缩机运行阶段 0:停止阶段 1：启动阶段 2：回油阶段 3：运行阶段*/
	eEu3AirState203_ErrLevel = 14,		    /*故障等级*/
	eEu3AirState203_ExhTemp = 15,		    /*排气温度，单位0.1℃*/
	eEu3AirState203_InhTemp = 16,		    /*吸气温度，单位0.1℃*/
	eEu3AirState203_CoolWatT = 17,		    /*空调冷却水进水温度，单位0.1℃*/
	eEu3AirState203_PCSOutTemp = 18,		/*模块出水温度，单位0.1℃*/
	eEu3AirState203_PCSInTemp = 19,		    /*模块进水温度，单位0.1℃*/
	eEu3AirState203_CellInTemp = 20,		/*电池进水温度，单位0.1℃*/
	eEu3AirState203_CellOutTemp = 21,		/*电池出水温度，单位0.1℃*/
	eEu3AirState203_CoolWatPcsT = 22,		/*空调冷却水模块出水温度，单位0.1℃*/
	eEu3AirState203_CoolWatCellT = 23,		/*空调冷却水电池出水温度，单位0.1℃*/
	eEu3AirState203_EnvTemp = 24,		    /*环境温度，单位0.1℃*/
	eEu3AirState203_HighPre = 25,		    /*高压压力，单位0.1bar*/
	eEu3AirState203_LowPre = 26,		    /*低压压力，单位0.1bar*/
	eEu3AirState203_CellOutPre = 27,		/*电池出水压力，单位0.1bar*/
	eEu3AirState203_PcsOutPre = 28,		    /*模块出水压力，单位0.1bar*/
	eEu3AirState203_CoolWatInP = 29,		/*空调冷却水进水压力，单位0.1bar*/
	eEu3AirState203_CoolWatOutP = 30,		/*空调冷却水出水压力，单位0.1bar*/
	eEu3AirState203_SensorErr1 = 31,		/*传感器故障1*/
	eEu3AirState203_SensorErr2 = 32,		/*传感器故障2*/
	eEu3AirState203_SysComErr = 33,		    /*系统部件故障*/
	eEu3AirState203_CompressErr1 = 34,		/*压缩机故障1*/
	eEu3AirState203_CompressErr2 = 35,		/*压缩机故障2*/
	eEu3AirState203_SysStaErr = 36,		    /*系统状态故障*/

	eEu3AirState203_CellPumpHz = 37,		/*电池水泵输出频率*/
	eEu3AirState203_CellPumpDuty = 38,		/*电池水泵输出占空比*/
	eEu3AirState203_PcsPumpHz = 39,		    /*模块水泵输出频率*/
	eEu3AirState203_PcsPumpDuty = 40,		/*模块水泵输出占空比*/
	eEu3AirState203_CoolFanHz = 41,		    /*驱动器散热风机输出频率*/
	eEu3AirState203_CoolFanDuty = 42,		/*驱动器散热风机输出占空比*/
	eEu3AirState203_FanHz = 43,		        /*风机输出频率*/
	eEu3AirState203_FanDuty = 44,		    /*风机输出占空比*/

	eEu3AirState203_Len		                /*EU3空调状态信息数组长度*/
}e_Eu3AirState203;

/*协议206子索引列表，液冷空调外机控制信息*/
typedef enum
{
	eLiquidAirInfo206_PwrState = 0,				/*液冷空调开关状态 0：关机 1：开机*/
	eLiquidAirInfo206_Mode,						/*空调模式 0：待机模式 1：自动模式 2：制冷模式  3：制热模式 4：自动模式*/
	eLiquidAirInfo206_CoolTemp,					/*制冷设定温度,10℃*/
	eLiquidAirInfo206_CoolOpenOffT,				/*制冷开启回差,1℃*/
	eLiquidAirInfo206_CoolCloseOffT,			/*制冷停止回差,1℃*/
	eLiquidAirInfo206_HeatTemp,					/*加热设定温度,10℃*/
	eLiquidAirInfo206_HeatTempOffT,				/*加热温度设定回差,10℃*/
	eLiquidAirInfo206_RefriType,				/*冷媒类型 0：R410A 1:R404A 3:R134A*/
	eLiquidAirInfo206_Baudrate,					/*上位机波特率 2：4800 3:9600 4:19200 5:38400 6:57600*/
	eLiquidAirInfo206_CommAddress,				/*上位机通信地址 1-255*/

	eLiquidAirInfo206_Len   			        /*液冷空调信息数组长度*/
}e_LiquidAirInfo206;

/*协议207子索引列表，液冷空调外机机型信息，状态*/
typedef enum
{
	eLiquidAirSta207_Specification = 0,				/*室外机规格能力, 0=3HP  1=5HP  2=8HP  3=10HP  4=12HP  5=14HP  6=16HP   7=18HP*/
	eLiquidAirSta207_SoftwareVer,				    /*软件版本*/
	eLiquidAirSta207_PLS,				    		/*系统1主电子膨胀阀开度(PLS)*/
	eLiquidAirSta207_CompressorTarFre,				/*系统1变频压缩机目标频率*/
	eLiquidAirSta207_CompressorRunFre,				/*系统1变频压缩机运行频率*/
	eLiquidAirSta207_FanTarSpeed,				    /*风机目标转速*/
	eLiquidAirSta207_ExhaustOverheated,				/*排气过热度*/
	eLiquidAirSta207_SysEnvTemp,					/*系统1外环温度,10℃*/
	eLiquidAirSta207_SysExhTemp,				    /*系统1排气温度,10℃*/
	eLiquidAirSta207_SysInhTemp,				    /*系统1吸气温度,10℃*/
	eLiquidAirSta207_PcsInWatTemp,					/*模块进水温度,10℃*/
	eLiquidAirSta207_PcsOutWatTemp,				    /*模块出水温度,10℃*/
	eLiquidAirSta207_CellInWatTemp,				    /*电池进水温度,10℃*/
	eLiquidAirSta207_CellOutWatTemp,				/*电池出水温度,10℃*/

	eLiquidAirSta207_PumpSpeed,						/*水泵实际转速,%*/
	eLiquidAirSta207_PcsOutWatPre,					/*模块出水压力,bar*/
	eLiquidAirSta207_CellOutWatPre,					/*电池出水压力,bar*/
	eLiquidAirSta207_SysHigPreTemp,					/*系统1高压饱和温度*/
	eLiquidAirSta207_SysLowPreTemp,					/*系统1低压饱和温度*/
	eLiquidAirSta207_InhOverheated,					/*吸气过热度*/
	eLiquidAirSta207_LowPressure,					/*系统1低压压力*/
	eLiquidAirSta207_HighPressure,					/*系统1高压压力*/
	eLiquidAirSta207_ACVoltage,						/*系统1AC输入电压*/
	eLiquidAirSta207_ACCurrent,						/*系统1AC输入电流*/
	eLiquidAirSta207_PumpCurrent,					/*系统1压缩机相电流*/
	eLiquidAirSta207_DCBusVolt,						/*系统1直流母线电压*/

	eLiquidAirSta207_Len   			        		/*液冷空调信息数组长度*/
}e_LiquidAirSta207;

/*协议209子索引列表，液冷PACK温度、水浸信息*/
typedef enum
{
	ePackEnvInfo209_1CatBarsTemp = 0,					/*PACK1正极铜排温度*/
	ePackEnvInfo209_1NegBarsTemp,						/*PACK1正极铜排温度*/
	ePackEnvInfo209_1WaterSenHz,				    	/*PACK1水浸频率*/
	ePackEnvInfo209_2CatBarsTemp,						/*PACK2正极铜排温度*/
	ePackEnvInfo209_2NegBarsTemp,						/*PACK2正极铜排温度*/
	ePackEnvInfo209_2WaterSenHz,				    	/*PACK2水浸频率*/
	ePackEnvInfo209_3CatBarsTemp,						/*PACK3正极铜排温度*/
	ePackEnvInfo209_3NegBarsTemp,						/*PACK3正极铜排温度*/
	ePackEnvInfo209_3WaterSenHz,				    	/*PACK3水浸频率*/
	ePackEnvInfo209_4CatBarsTemp,						/*PACK4正极铜排温度*/
	ePackEnvInfo209_4NegBarsTemp,						/*PACK4正极铜排温度*/
	ePackEnvInfo209_4WaterSenHz,				    	/*PACK4水浸频率*/
	ePackEnvInfo209_5CatBarsTemp,						/*PACK5正极铜排温度*/
	ePackEnvInfo209_5NegBarsTemp,						/*PACK5正极铜排温度*/
	ePackEnvInfo209_5WaterSenHz,				    	/*PACK5水浸频率*/

	ePackEnvInfo209_Len   			        			/*液冷PACK�?境信�?*/
}e_ePackEnvInfo209;
/*协议210子索引列表，液冷PACK MSD状态*/
typedef enum
{
	ePackMsdInfo210_1Amplitude = 0,						/*PACK1MSD幅值，0.01V*/
	ePackMsdInfo210_1Duty,								/*PACK1MSD占空比，%*/
	ePackMsdInfo210_2Amplitude,							/*PACK2MSD幅值，0.01V*/
	ePackMsdInfo210_2Duty,								/*PACK2MSD占空比，%*/
	ePackMsdInfo210_3Amplitude,							/*PACK3MSD幅值，0.01V*/
	ePackMsdInfo210_3Duty,								/*PACK3MSD占空比，%*/
	ePackMsdInfo210_4Amplitude,							/*PACK4MSD幅值，0.01V*/
	ePackMsdInfo210_4Duty,								/*PACK4MSD占空比，%*/
	ePackMsdInfo210_5Amplitude,							/*PACK5MSD幅值，0.01V*/
	ePackMsdInfo210_5Duty,								/*PACK5MSD占空比，%*/

	ePackMsdInfo210_Len   			        			/*液冷PACK MSD状态信�?*/
}e_PackMsdInfo210;
/*协议211子索引列表，液冷PACK气溶胶状态*/
typedef enum
{
	eAerosolInfo211_1AerCon = 0,							/*PACK1气溶胶浓度，%*/
	eAerosolInfo211_1H2Con,									/*PACK1H2浓度,%*/
	eAerosolInfo211_1AerSta,								/*PACK1气溶胶状态和故障信息*/
	eAerosolInfo211_2AerCon,								/*PACK2气溶胶浓,%*/
	eAerosolInfo211_2H2Con,									/*PACK2H2浓度,%*/
	eAerosolInfo211_2AerSta,								/*PACK2气溶胶状态和故障信息*/
	eAerosolInfo211_3AerCon,								/*PACK3气溶胶浓度，0.01V*/
	eAerosolInfo211_3H2Con,									/*PACK3H2浓度,%*/
	eAerosolInfo211_3AerSta,								/*PACK3气溶胶状态和故障信息*/
	eAerosolInfo211_4AerCon,								/*PACK4气溶胶浓度，%*/
	eAerosolInfo211_4H2Con,									/*PACK4H2浓度,%*/
	eAerosolInfo211_4AerSta,								/*PACK4气溶胶状态和故障信息*/
	eAerosolInfo211_5AerCon,								/*PACK5气溶胶浓度，%*/
	eAerosolInfo211_5H2Con,									/*PACK5H2浓度,%*/
	eAerosolInfo211_5AerSta,								/*PACK5气溶胶状态和故障信息*/

	eAerosolInfo211_Len   			        				/*液冷PACK气溶胶传感器信息*/
}e_AerosolInfo211;
/*协议212子索引列表，同飞空调用户参数信息*/
typedef enum
{
	eATFUserInfo212_StartMode = 0,							/*启动模式*/
	eATFUserInfo212_WorkMode,								/*工作模式*/
	eATFUserInfo212_ControlObj,								/*供回水控制目标*/
	eATFUserInfo212_ColdTemp,								/*制冷温度*/
	eATFUserInfo212_ColdDiff,								/*制冷温差*/
	eATFUserInfo212_HeatTemp,								/*制热温度*/
	eATFUserInfo212_HeatDiff,								/*制热温差*/

	eATFUserInfo212_Len   			        				/*同飞空调用户参数信息*/
}e_ATFUserInfo212;
/*协议213子索引列表，同飞空调状态信息*/
typedef enum
{
	eATFUserState213_RunSta = 0,							/*机组运行状态*/
	eATFUserState213_SwitchIn,								/*开关量输入*/
	eATFUserState213_Relays,								/*继电器输出*/
	eATFUserState213_FeedTemp,								/*供液温度，单位0.1℃*/
	eATFUserState213_ReturnTemp,							/*回液温度，单位0.1℃ */
	eATFUserState213_SteamTemp,								/*系统蒸进，单位0.1℃*/
	eATFUserState213_EvapTemp,								/*蒸出温度，单位0.1℃*/
	eATFUserState213_AmbiTemp,								/*环境温度，单位0.1℃*/
	eATFUserState213_CondTemp,								/*系统冷凝温度，单位0.1℃*/
	eATFUserState213_OpenExpen,								/*节流电膨当前开度，单位步*/
	eATFUserState213_PumpSpeed,								/*水泵当前转速，单位1%*/
	eATFUserState213_ComSpeed,								/*压缩机当前转速，单位1RPS*/
	eATFUserState213_FanSpeed,								/*风机转速，单位1RPS*/
	eATFUserState213_SuppWatPre,							/*供水压力，单位0.1bar*/
	eATFUserState213_RetpWatPre,							/*回水压力，单位0.1bar*/
	eATFUserState213_ExhPre,							    /*排气压力，单位0.1bar*/

	eATFUserState213_Len   			        				/*同飞空调状态信息*/
}e_ATFUserState;

/*协议214子索引列表，同飞空调故障信息*/
typedef enum
{
	eATFErrInfo214_ErrSta1 = 0,							    /*故障状态1*/
	eATFErrInfo214_ErrSta2,								    /*故障状态2*/
	eATFErrInfo214_ErrSta3,								    /*故障状态3*/
	eATFErrInfo214_ErrSta4,								    /*故障状态4*/
	eATFErrInfo214_ErrSta5,							        /*故障状态5*/
	eATFErrInfo214_ErrSta6,								 	/*故障状态6*/
	eATFErrInfo214_ErrSta7,								 	/*故障状态7*/
	eATFErrInfo214_FreConvert,								/*变频器故障代码*/
	eATFErrInfo214_Level,								    /*系统总故障等级*/
	eATFErrInfo214_TotalCode,								/*系统总故障代码*/
	eATFErrInfo214_Fan1,								    /*变频风机故障1*/
	eATFErrInfo214_Fan2,								    /*变频风机故障2*/
	eATFErrInfo214_PFC,								        /*三相PFC故障号*/

	eATFErrInfo214_Len   			        				/*同飞空调故障信息长度*/
}e_ATFErrSta;

/*协议215子索引列表*/
typedef enum
{
	eXinMiaoAirSta215_WatPumpSta = 0,        /*水泵运行状态*/
	eXinMiaoAirSta215_Com1State = 1,         /*1#压缩机运行状态*/
	eXinMiaoAirSta215_Com2State = 2,         /*2#压缩机运行状态*/
	eXinMiaoAirSta215_EleHeatSta = 3,        /*电加热运行状态*/
	eXinMiaoAirSta215_CondFanSta = 4,        /*冷凝风机运行状态*/
	eXinMiaoAirSta215_OutWatTemp = 5,        /*机组出水温度*/
	eXinMiaoAirSta215_ReWatTemp = 6,         /*机组回水温度*/
	eXinMiaoAirSta215_OutWatPre = 7,         /*机组出水压力*/
	eXinMiaoAirSta215_ReWatPre = 8,          /*机组回水压力*/
	eXinMiaoAirSta215_EnvTemp = 9,           /*机组外环境温度*/
	eXinMiaoAirSta215_MakeuoWatSta = 10,     /*补液水泵状态*/
	eXinMiaoAirSta215_FaultCode = 11,        /*故障告警代码*/

	eXinMiaoAirSta215_Len		             /*新淼空调参数信息数组长度*/
}e_XinMiaoAirSta;

/*协议216子索引列表*/
typedef enum
{
	eXinMiaoAirPar216_Mode = 0,              /*液冷机组控制模式  查询 1：自循环模式  2：制冷模式 3：制热模式 4：自动模式*/
	eXinMiaoAirPar216_PwrSta = 1,            /*开关机 0：关机 1：开机*/
	eXinMiaoAirPar216_SetTemp = 2, 			 /*机组设置温度 0.1℃*/

	eXinMiaoAirPar216_Len
}e_XinMiaoAirPar;

/*协议217子索引列表，IMEU3温湿度信息*/
typedef enum
{
	eIMEU3217_PcsAiroutT= 0,								/*PCS出风口温度*/
	eIMEU3217_PduEnvT,								        /*PDU内部环温*/
	eIMEU3217_PcsCableT,								    /*PCS进线线缆温度*/
	eIMEU3217_CabT1,								        /*柜内温度1*/
	eIMEU3217_CabT2,										/*柜内温度2*/
	eIMEU3217_485T1,										/*485温湿度传感器1温度*/
	eIMEU3217_485H1,										/*485温湿度传感器1湿度*/
	eIMEU3217_485T2,										/*485温湿度传感器2温度*/
	eIMEU3217_485H2,   			        					/*485温湿度传感器2温度*/

	eIMEU3217_Len   			        				    /*IMEU3温湿度信息长度*/
}e_IMEU3TempHumInfo;

/*协议218子索引列表，PDU内IMEU3信息*/
typedef enum
{
	ePduIn218_DI1_16= 0,									 /*DI1-16*/
	ePduIn218_DI17_22_DO1_8,								 /*BIT0-BIT6: DI17-DI22 BIT8-BIT15:DO1-DO8*/
	ePduIn218_DO9_24,								    	 /*DO9-DO24*/
	ePduIn218_X1DI,								             /*BIT0:BIT4 4路X1板子连接DI*/
	ePduIn218_FAN1,										 	 /*散热风机PWM占空比*/
	ePduIn218_FAN2,										 	 /*扰流风机PWM占空比*/
	ePduIn218_Alarm1,										 /*NTC短路告警，按BIT表示*/
	ePduIn218_Alarm2,										 /*NTC开路告警，按BIT表示*/

	ePduIn218_Len   			        				    /*PDU内IMEU3信息*/
}e_PduInInfo;

/*PDU内IMEU3DI1——16位对应信息(ePduIn218_DI1_16)*/
typedef enum
{
    eEU218DI1_QF1 = 0,                  		/*QF1开关状态 */
	eEU218DI2_AcMainCon,                    	/*交流主接触器状态*/
	eEU218DI3_NCon,                    		    /*N线接触器状态*/
	eEU218DI4_NGCon,                    	    /*N线接地接触器状态*/
	eEU218DI5_MpptMainCon,                      /*MPPT正负接触器状态*/
	eEU218DI6_MpptDcAccSw,                      /*光伏侧直流接入开关状态*/
	eEU218DI7_MpptSpd,                    	    /*光伏防雷状态*/
	eEU218DI8_BmuMainCon,                       /*BMU正极接触器状态*/
	eEU218DI9_BmuNegCon,                    	/*BMU负极接触器状态*/
	eEU218DI10_BmuPreCon,                       /*BMU预充接触器状态*/
	eEU218DI11_AcAuxSour,                       /*交流辅源状态*/
	eEU218DI16_AcSpd = 15,                      /*交流防雷器开关及防雷器状态*/
    eEU218DI1_16_Bit                         	/*PDU内IMEU3DI1_DI16BIT信息长度*/
}e_EU218DI1_16Bit;

/*PDU内IMEU3DI17——DI22_DO1_DO6对应信息(ePduIn218_DI17_22_DO1_8)*/
typedef enum
{
	eEU218DI17_PcsFan = 0,                  	/*PCS散热风扇*/
	eEU218DI17_PduFan = 1,                  	/*PDU扰流风机*/
    eEU218DO1_QF1 = 8,                  		/*QF1开关控制*/
	eEU218DO2_AcMainCon,                    	/*交流主接触器控制*/
	eEU218DO3_NCon,                    		    /*N线接触器控制*/
	eEU218DO4_NGCon,                    	    /*N线接地接触器控制*/
	eEU218DO5_MpptMainCon,                      /*MPPT正负接触器控制*/
	eEU218DO6_MpptDcAccSw,                      /*光伏侧直流接入开关控制*/
	eEU218DO_Bit								/*PDU内IMEU3DOBIT信息长度*/
}e_EU218DO1_6Bit;

/*协议219子索引列表，PDU外IMEU3信息*/
typedef enum
{
	ePduOut219_DI1_16= 0,									 /*DI1-16*/
	ePduOut219_DI17_22_DO1_8,								 /*BIT0-BIT6: DI17-DI22 BIT8-BIT15:DO1-DO8*/
	ePduOut219_DO9_24,								    	 /*DO9-DO24*/
	ePduOut219_X1DI,								         /*BIT0:BIT4 4路X1板子连接DI*/
	ePduOut219_Gas1,										 /*可燃气体浓度 1 H2*/
	ePduOut219_Gas2,										 /*可燃气体浓度 2 CO*/
	ePduOut219_FAN1,										 /*排气扇PWM占空比*/
	ePduOut219_FloodHz,										 /*水浸频率*/
	ePduOut219_MotorSta,									 /*泄爆阀状态*/
	ePduOut219_Alarm1,										 /*EU3外设通信告警，按BIT表示*/
	ePduOut219_Alarm2,										 /*NTC短路告警，按BIT表示*/
	ePduOut219_Alarm3,										 /*NTC开路告警，按BIT表示*/

	ePduOut219_Len   			        				    /*PDU外IMEU3信息*/
}e_PduOutInfo;
/*PDU外IMEU3DI1——16位对应信息(ePduOut219_DI1_16)*/
typedef enum
{
    eEU219DI1_Door = 0,                  		/*门禁状态*/
	eEU219DI2_Pour,                    			/*倾倒状态*/
	eEU219DI3_Combu,                    		/*可燃传感器状态（国外）*/
	eEU219DI4_Air,                    	    	/*空调状态*/

	eEU219DI1_16Bit                         	/*PDU内IMEU3DI1_DI16BIT信息长度*/
}e_EU19DI1_16Bit;
/*PDU外IMEU3DI17_DI22位对应信息(ePduOut219_DI17_22_DO1_8)*/
typedef enum
{
    eEU219DI17_Explosion2 = 0,                  /*泄爆阀状态2*/
	eEU219DI18_Explosion1,                    	/*泄爆阀状态1*/
	eEU219DI19_Fan,                    			/*排气扇状态*/
	eEU219DI17_PressureRel,                    	/*泄压装置状态（预留）*/

	eEU219DI17_22Bit                         	/*PDU内IMEU3DI1_DI16BIT信息长度*/
}e_EU19DI17_22Bit;
/*PDU外IMEU3DI17——DI22_DO1_DO6对应信息(ePduOut219_DI17_22_DO1_8)*/
typedef enum
{
    eEU219DO1_Obli1 = 8,                  		/*预留1*/
	eEU219DO2_Light,                    		/*照明控制*/
	eEU219DO3_Obli2,                    		/*预留2*/
	eEU219DO4_FireAlarm,                    	/*声光报警控制*/
	eEU219DO5_ActiveFire,                       /*主动触发系统消防控制*/
	eEU219DO6_Obli3,                      		/*光伏侧直流接入开关控制*/
	eEU219DO7_TotalSolen,                       /*全氟乙酮总电磁阀控制*/
	eEU219DO8_Pack1Solen,                      	/*全氟乙酮PACK1电磁阀控制*/
	eEU219DO1_6_Bit								/*PDU内IMEU3DOBIT信息长度*/
}e_EU204DO1_6Bit;

/*PDU外IMEU3DIO9_24对应信息(ePduOut219_DO9_24)*/
typedef enum
{
    eEU219DO9_Pack2Solen = 0,                  	/*全氟乙酮PACK2电磁阀控制*/
	eEU219DO10_Pack3Solen,                    	/*全氟乙酮PACK3电磁阀控制*/
	eEU219DO11_Pack4Solen,                    	/*全氟乙酮PACK4电磁阀控制*/
	eEU219DO12_Pack5Solen,                    	/*全氟乙酮PACK5电磁阀控制*/
	eEU219DO9_12_Bit							/*PDU内IMEU3DOBIT信息长度*/
}e_EU219DO9_12Bit;

/*协议220子索引列表，PDU外IMEU3所接除湿机信息*/
typedef enum
{
	eDeInfo220_Hum= 0,							 /*湿度值*/
	eDeInfo220_OpenDeHum,						 /*除湿启动值*/
	eDeInfo220_CloseDeHum,						 /*除湿停止值*/
	eDeInfo220_State,							 /*工作状态*/
	eDeInfo220_Addr,							 /*通信地址*/

	eDeInfo220_Len   			        		 /*PDU外IMEU3信息*/
}e_DeInfo;

/*协议223子索引列表，漏电流传感器信息*/
typedef enum
{
	eBsu1info223_BotCount = 0,					 /*第一个bsu的bot枚举个数*/
	eBsu1info223_topCount,						 /*第一个bsu的top枚举个数*/
	eBsu2info223_BotCount,						 /*第二个bsu的bot枚举个数*/
	eBsu2info223_topCount,						 /*第二个bsu的top枚举个数*/
	eBsu3info223_BotCount,						 /*第三个bsu的bot枚举个数*/
	eBsu3info223_topCount,						 /*第三个bsu的top枚举个数*/
	eBsu4info223_BotCount,						 /*第四个bsu的bot枚举个数*/
	eBsu4info223_topCount,						 /*第四个bsu的top枚举个数*/
	eBsu5info223_BotCount,						 /*第五个bsu的bot枚举个数*/
	eBsu5info223_topCount,						 /*第五个bsu的top枚举个数*/

	eBsuBotTopCount223_Len   			         /*每个bsu从控bot和top枚举计数*/
}e_BsuBotTopCountInfo;

/*协议225子索引列表，漏电流传感器信息*/
typedef enum
{
	eLeakageInfo225_Curr = 0,						 /*漏电流*/
	eLeakageInfo225_Ver,						     /*软件版本号*/

	eLeakageInfo225_Len   			        		 /*漏电流传感器信息*/
}e_LeakageInfo;

/*协议254子索引列表*/
typedef enum
{
	eFireInfo254_Level1 = 0,			    /*一级消防标志*/
	eFireInfo254_Level2 = 1,				/*二级消防标志*/
	eFireInfo254_Level3 = 2,				/*三级消防标志*/
	eFireInfo254_Level4 = 3,				/*四级消防标志*/

	eFireInfo254_Len   			            /*消防等级信息*/

}e_FireInfo254;

/*协议255子索引列表*/
typedef enum
{
	eTempInfo255_Pack1MaxTNum = 0,			/*PACK1最高温度序号[1,48]*/
	eTempInfo255_Pack1MaxTemp = 1,			/*PACK1最高温度值[-400,1050]0.1℃*/
	eTempInfo255_Pack2MaxTNum = 2,			/*PACK2最高温度序号[1,48]*/
	eTempInfo255_Pack2MaxTemp = 3,			/*PACK2最高温度值[-400,1050]0.1℃*/
	eTempInfo255_Pack3MaxTNum = 4,			/*PACK3最高温度序号[1,400]*/
	eTempInfo255_Pack3MaxTemp = 5,			/*PACK3最高温度值[-400,1050]0.1℃*/
	eTempInfo255_Pack4MaxTNum = 6,			/*PACK4最高温度序号[1,400]*/
	eTempInfo255_Pack4MaxTemp = 7,			/*PACK4最高温度值[-400,1050]0.1℃*/
	eTempInfo255_Pack5MaxTNum = 8,			/*PACK5最高温度序号[1,400]*/
	eTempInfo255_Pack5MaxTemp = 9,			/*PACK5最高温度值[-400,1050]0.1℃*/

	ePackTempInfo255_Len   			        /*PACK温度最值信息*/

}e_PackTempCharInfo255;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
#if(1 == BMU_BSU_COM_TYPE)//使能CAN通信
#if(0 == BMU_BSU_CAN_TYPE)//使能自研CAN通信
/*=============================================================================================
** @Name      : void InputBsuBalanceState(u8 cell, u8 state)
** @Input     : cell:组内电池号[0,399] state:连续16节电池充电均衡开启状态值[0关闭 1:开启(均充)]
** @Output    : void
** @Function  : 输入接收到从机充电均衡状态值
** @The notes : 按连续16Bit位表示连续16节电池顺序(非一个数组索引表示1个从控)
**===========================================================================================*/
void InputSlaveChgBalanState(u16 cell, u16 state);

/*=============================================================================================
** @Name      : void InputSlaveDhgBalanState(u8 cell, u8 state)
** @Input     : cell:组内电池号[0,399] state:连续16节电池放电均衡开启状态值[0关闭 1:开启(均放)]
** @Output    : void
** @Function  : 输入接收到从机放电均衡状态值
** @The notes : 按连续16Bit位表示连续16节电池顺序(非一个数组索引表示1个从控)
**===========================================================================================*/
void InputSlaveDhgBalanState(u16 cell, u16 state);
/*=============================================================================================
** @Name      : void InputSlaveSafeState(u8 cell, u8 state)
** @Input     : cell:组内电池号[0,399] state:连续16节电池放电均衡开启状态值[0关闭 1:开启(均放)]
** @Output    : void
** @Function  : 输入接收到从机Safe状态值
** @The notes : 按连续16Bit位表示连续16节电池顺序(非一个数组索引表示1个从控)
**===========================================================================================*/
void InputSlaveSafeState(u16 cell, u16 state);
/*=============================================================================================
** @Name      : void InputBsuCellVolt(u8 ecu, u8 cell, u16 volt)
** @Input     : ecu:从控节点号[0,31] cell:从控内电池号[0,15] resist:内阻值 0.1μΩ
** @Output    : void
** @Function  : 输入接收到从控单体内阻值
** @The notes :
**===========================================================================================*/
void InputBsuCellResis(u8 ecu, u8 cell, u16 resist);
// by lxr01202
/*=============================================================================================
** @Name      : void InputBsuCellRealResis(u8 ecu, u8 cell, u16 volt)
** @Input     : ecu:从控节点号[0,31] cell:从控内电池号[0,15] resist:实部内阻值 1μΩ
** @Output    : void
** @Function  : 输入接收到从控单体内阻实部值
** @The notes :
**===========================================================================================*/

void InputBsuCellRealResis(u8 ecu, u8 cell, u16 resist);

/*=============================================================================================
** @Name      : void InputBsuCellImaginaryResis(u8 ecu, u8 cell, u16 volt)
** @Input     : ecu:从控节点号[0,31] cell:从控内电池号[0,15] resist:虚部内阻值 1μΩ
** @Output    : void
** @Function  : 输入接收到从控单体内阻虚部值
** @The notes :
**===========================================================================================*/
void InputBsuCellImaginaryResis(u8 ecu, u8 cell, u16 resist);

/*=============================================================================================
** @Name      : void InputAfeSrState(u8 ecu, u8 cell, u16 state)
** @Input     : ecu:从控节点号[0,31] cell:从控内电池号[0,15] state:采样前端sr状态值
** @Output    : void
** @Function  : 输入接收到从控采样前端sr状态
** @The notes :
**===========================================================================================*/
void InputAfeSrState(u8 ecu, u8 cell, u16 state);

/*=============================================================================================
** @Name      : void InputAfeWorkMode(u8 cell, u8 mode)
** @Input     : cell:组内电池号[0,399] mode:连续2个afe采样前端工作模式状态值
** @Output    : void
** @Function  : 输入接收到从机采样前端工作模式状态值
** @The notes : 按连续8Bit位表示连续2个afe采样前端顺序(非一个数组索引表示1个采样前端)
**===========================================================================================*/
void InputAfeWorkMode(u16 cell, u8 mode);
#if(0)
/*=============================================================================================
** @Name      : void InputBsuBalanceState(u8 ecu, u8 cell, u8 state)
** @Input     : ecu:从控节点号[0,31] cell:从控电池号[0,23] state:均衡状态值[0关闭 1:均充 2均放]
** @Output    : void
** @Function  : 输入接收到从控均衡状态值
** @The notes : 按连续Bit位表示电池顺序(非一个数组索引表示1个从控),每个Pack从控电压数目必须一样
**===========================================================================================*/
void InputBsuBalanceState(u8 ecu, u8 cell, u8 state);
#endif

#else//使能高特CAN通信
/*=============================================================================================
** @Name      : void InputBsuChgBalanState(u8 ecu, u32 cellState)
** @Input     : ecu:从控节点号[0,31] cellState:从控各电池均衡状态值(按bit位表示电池[0关闭 1:均充]
** @Output    : void
** @Function  : 输入接收到从控充电均衡的电池
** @The notes : 按连续Bit位表示电池顺序(非一个数组索引表示1个从控),每个Pack从控电压数目必须一样
**===========================================================================================*/
void InputBsuChgBalanState(u8 ecu, u32 cellState);

/*=============================================================================================
** @Name      : void InputBsuDhgBalanState(u8 ecu, u32 cellState)
** @Input     : ecu:从控节点号[0,31] cellState:从控各电池放电均衡状态值(按bit位表示电池[0关闭 1:均放]
** @Output    : void
** @Function  : 输入接收到从控放电均衡的电池
** @The notes : 按连续Bit位表示电池顺序(非一个数组索引表示1个从控),每个Pack从控电压数目必须一样
**===========================================================================================*/
void InputBsuDhgBalanState(u8 ecu, u32 cellState);
#endif
#endif

/*=============================================================================================
** @Name      : void InputBsuCellVolt(u8 ecu, u8 cell, u16 volt)
** @Input     : ecu:从控节点号[0,31] cell:从控内电池号[0,15] volt:电压值 1mv
** @Output    : void
** @Function  : 输入接收到从控单体电压值
** @The notes :
**===========================================================================================*/
void InputBsuCellVolt(u8 ecu, u8 cell, u16 volt);

/*=============================================================================================
** @Name      : void InputBsuCellTemp(u8 ecu, u8 num, s8 temp)
** @Input     : ecu:从控节点号[0,31] num:从控内单体温度序号[0,3] temp:温度值(-40~126℃)
** @Output    : void
** @Function  : 输入接收到BSU单体温度值
** @The notes : 不包括均衡板温
**===========================================================================================*/
void InputBsuCellTemp(u8 ecu, u8 num, s16 temp);

/*=============================================================================================
** @Name      : void InputBsuBalanTemp(u8 ecu, u8 num, s8 temp)
** @Input     : ecu:BSU节点号[0,31] num:从控内均衡板温序号[0,1] temp:温度值(-40~126℃)
** @Output    : void
** @Function  : 输入接收到BSU均衡板温温度值
** @The notes : 放在全部单体温度数组之后
**===========================================================================================*/
void InputBsuBalanTemp(u8 ecu, u8 num, s16 temp);

/*=============================================================================================
** @Name      : void InputGroupHVSideData(e_HAfeSampChan index, s32 data)
** @Input     : index:高压端信息索引 data:高压端信息值
** @Output    : void
** @Function  : 输入高压端采样信息值
** @The notes :
**===========================================================================================*/
void InputGroupHVSideData(e_HAfeSampChan index, s32 data);

/*=============================================================================================
** @Name      : void InputGroupDaisyPwmData(u32 freq)
** @Input     : freq:PWM频率
** @Output    : void
** @Function  : 输入PWM频率采样值
** @The notes :
**===========================================================================================*/
void InputGroupDaisyPwmData(u32 freq);

/*=============================================================================================
** @Name      : void InputGroupLVSideData(e_BMULVSampADC index, s32 data)
** @Input     : index:低压端信息索引 data:低压端信息值
** @Output    : void
** @Function  : 输入低压端AD采样信息值
** @The notes :
**===========================================================================================*/
void InputGroupLVSideData(e_BMULVSampADC index, s32 data);
/*=============================================================================================
** @Name      : void ClrGPerAirAllInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除所有液冷空调接收信息
** @The notes : 液冷空调通信中断时调用
**===========================================================================================*/
void ClrGPerAirAllInfoAPI(void);
/*=============================================================================================
** @Name      : u16 GetBsuCellVolt(u8 ecu, u8 cell)
** @Input     : ecu:从控节点号[0,31] cell:从控内电池号[0,15]
** @Output    : 从控单体当前电压值:1mV
** @Function  : 获取指定从控内部单体当前电压值
** @The notes :
**===========================================================================================*/
u16 GetBsuCellVolt(u8 ecu, u8 cell);

/*=============================================================================================
** @Name      : s8 GetBsuCellTemp(u8 ecu, u8 num)
** @Input     : ecu:从控节点号[0,31] num:从控内单体温度序号[0,3]
** @Output    : 从控单体当前温度值:1℃
** @Function  : 获取指定从控内部单体当前温度值
** @The notes :
**===========================================================================================*/
s16 GetBsuCellTemp(u8 ecu, u8 num);

/*=============================================================================================
** @Name      : s8 GetBsuBalanTemp(u8 ecu, u8 num)
** @Input     : ecu:从控节点号[0,31] num:从控内均衡板温序号[0,1]
** @Output    : 从控均衡板温当前温度值:1℃
** @Function  : 获取指定从控内部均衡板温当前温度值
** @The notes :
**===========================================================================================*/
s16 GetBsuBalanTemp(u8 ecu, u8 num);

/*=================================================================================================
** @Name      : s16 GetGDcSideSampRealInfoAPI(e_ReferSampInfo200 index)
** @Input     : index:直流侧采样信息序号
** @Output    : 直流侧实际采样信息值
** @Function  : 获取电池组直流侧实际采样信息值
** @The notes : 采样中间参考值
**===============================================================================================*/
s16 GetGDcSideSampRealInfoAPI(e_ReferSampInfo200 index);
/*=================================================================================================
** @Name      : u16 GetGProtectStateAPI(void)
** @Input     : void
** @Output    : 电池组保护状态[0,5]
** @Function  : 获取电池组当前保护状态
** @The notes : e_ProState
**===============================================================================================*/
u16 GetGProtectStateAPI(void);

/*=================================================================================================
** @Name      : u16 GetGWorkStateAPI(void)
** @Input     : void
** @Output    : 电池组工作状态[0,6]
** @Function  : 获取电池组当前工作状态
** @The notes : e_WorkState
**===============================================================================================*/
u16 GetGWorkStateAPI(void);

/*=================================================================================================
** @Name      : u16 GetGChgDhgStateAPI(void)
** @Input     : void
** @Output    : 电池组充放电状态[0,2]
** @Function  : 获取电池组当前充放电状态
** @The notes : e_CurrState
**===============================================================================================*/
u16 GetGChgDhgStateAPI(void);

/*=================================================================================================
** @Name      : u16 GetGWorkModeAPI(void)
** @Input     : void
** @Output    : 电池组工作模式[0,5]
** @Function  : 获取电池组当前工作模式
** @The notes : e_WorkMode
**===============================================================================================*/
u16 GetGWorkModeAPI(void);
/*=================================================================================================
** @Name      : u16 GetGChgLimCurrAPI(void)
** @Input     : void
** @Output    : 实时充电电流限值 0.1A
** @Function  : 获取电池组当前充电电流限值
** @The notes : 最大充电电流请求值
**===============================================================================================*/
u16 GetGChgLimCurrAPI(void);
/*=================================================================================================
** @Name      : u16 GetGDhgLimCurrAPI(void)
** @Input     : void
** @Output    : 实时放电电流限值 0.1A
** @Function  : 获取电池组当前放电电流限值
** @The notes : 最大放电电流请求值
**===============================================================================================*/
u16 GetGDhgLimCurrAPI(void);
/*=============================================================================================
** @Name      : u16 GetGDhgLimPowerAPI(void)
** @Input     : void
** @Output    : 放电功率限值 0.1kW
** @Function  : 返回放电功率限值
** @The notes : 最大放电功率请求值
**===========================================================================================*/
u16 GetGChgLimPowerAPI(void);
/*=============================================================================================
** @Name      : u16 GetGChgLimPowerAPI(void)
** @Input     : void
** @Output    : 充电功率限值 0.1kW
** @Function  : 返回充电功率限值
** @The notes : 最大充电功率请求值
**===========================================================================================*/
u16 GetGDhgLimPowerAPI(void);
/*=============================================================================================
** @Name      : u16 GetGChgLimSumVAPI(void)
** @Input     : void
** @Output    : 充电总压限值 0.1V
** @Function  : 返回充电总压限值
** @The notes :
**===========================================================================================*/
u16 GetGChgLimSumVAPI(void);
/*=============================================================================================
** @Name      : u16 GetGDhgLimSumVAPI(void)
** @Input     : void
** @Output    : 放电总压限值 0.1V
** @Function  : 返回放电总压限值
** @The notes :
**===========================================================================================*/
u16 GetGDhgLimSumVAPI(void);
/*=============================================================================================
** @Name      : u16 GetGDhgLimSumVAPI(void)
** @Input     : index:告警信息索引号[0,2]
** @Output    : 电池组告警信息
** @Function  : 返回电池组告警信息
** @The notes : 16位告警码
**===========================================================================================*/
u16 GetGAlarmInfoAPI(u8 index);
/*=============================================================================================
** @Name      : u16 GetGFaultInfoAPI(void)
** @Input     : index:故障信息索引号[0,2]
** @Output    : 电池组故障信息
** @Function  : 返回电池组故障信息
** @The notes : 16位故障码
**===========================================================================================*/
u16 GetGFaultInfoAPI(u8 index);
/*=============================================================================================
** @Name      : u16 GetGInPortStateAPI(void)
** @Input     : void
** @Output    : 各输出IO�?口当前状态�?(按bit位表�? 0�? 1�?)
** @Function  : 返回输出IO�?口当前状态�?
** @The notes :
**===========================================================================================*/
u16 GetGInPortStateAPI(void);
/*=============================================================================================
** @Name      : u16 GetGInPortStateAPI(void)
** @Input     : void
** @Output    : 各输出IO端口当前状态值(按bit位表示 0分 1合)
** @Function  : 返回输出IO端口当前状态值
** @The notes :
**===========================================================================================*/
u16 GetGOutPortStateAPI(void);
/*=================================================================================================
** @Name      : u16 GetGSampSumVoltAPI(void)
** @Input     : void
** @Output    : 电池组采样总电压 0.1V
** @Function  : 获取电池组采样总电压
** @The notes : 单体累加值
**===============================================================================================*/
u16 GetGSampSumVoltAPI(void);
/*=================================================================================================
** @Name      : s16 GetGSampOutCurrAPI(void)
** @Input     : void
** @Output    : 电池组采样输出电流 0.1A
** @Function  : 获取电池组采样输出电流
** @The notes : 负号为充电
**===============================================================================================*/
s16 GetGSampOutCurrAPI(void);
/*=================================================================================================
** @Name      : s16 GetGSampOutPowerAPI(void)
** @Input     : void
** @Output    : 电池组输出功率 0.1kW
** @Function  : 获取电池组输出功率值
** @The notes : 负号为充电
**===============================================================================================*/
s16 GetGSampOutPowerAPI(void);
/*=================================================================================================
** @Name      : u16 GetGSampPreVoltAPI(void)
** @Input     : void
** @Output    : 电池组外端预充电压 0.1V
** @Function  : 获取电池组外端预充电压值
** @The notes : 采样值
**===============================================================================================*/
u16 GetGSampPreVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattSocAPI(void)
** @Input     : void
** @Output    : 电池组SOC 0.1%
** @Function  : 获取电池组SOC?
** @The notes :
**===============================================================================================*/
u16 GetGBattSocAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattSoeAPI(void)
** @Input     : void
** @Output    : 电池组SOE 0.1%
** @Function  : 获取电池组SOE?
** @The notes :
**===============================================================================================*/
u16 GetGBattSoeAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattSoeAPI(void)
** @Input     : void
** @Output    : 电池组SOH 0.1%
** @Function  : 获取电池组SOH?
** @The notes :
**===============================================================================================*/
u16 GetGBattSohAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattAllCapAPI(void)
** @Input     : void
** @Output    : 电池组总容量 0.1AH
** @Function  : 获取电池组总容量值
** @The notes : 实测值
**===============================================================================================*/
u16 GetGBattAllCapAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellSumVoltAPI(void)
** @Input     : void
** @Output    : 电池组求和总电压 0.1V
** @Function  : 获取电池组求和总电压
** @The notes : 单体累加值
**===============================================================================================*/
u16 GetGCellSumVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellAvgVoltAPI(void)
** @Input     : void
** @Output    : 电池组单体平均电压 1mV
** @Function  : 获取电池组单体平均电压
** @The notes : 单体累加平均值
**===============================================================================================*/
u16 GetGCellAvgVoltAPI(void);

/*=================================================================================================
** @Name      : s16 GetGCellAvgTempAPI(void)
** @Input     : void
** @Output    : 电池组单体平均温度 1℃
** @Function  : 获取电池组单体平均温度
** @The notes : 单体累加平均值
**===============================================================================================*/
s16 GetGCellAvgTempAPI(void);

/*=================================================================================================
** @Name      : s16 GetGEnvirTempAPI(void)
** @Input     : void
** @Output    : 电池组环境温度 1℃
** @Function  : 获取电池组环境温度
** @The notes : 第1路外围温度
**===============================================================================================*/
s16 GetGEnvirTempAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMaxVNumAPI(void)
** @Input     : void
** @Output    : 电池组单体最大电压电池号[1,400]
** @Function  : 获取电池组单体最大电压电池号
** @The notes :
**===============================================================================================*/
u16 GetGCellMaxVNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMaxVoltAPI(void)
** @Input     : void
** @Output    : 电池组单体最大电压值 1mV
** @Function  : 获取电池组单体最大电压值
** @The notes :
**===============================================================================================*/
u16 GetGCellMaxVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMaxVNumAPI(void)
** @Input     : void
** @Output    : 电池组单体最小电压电池号[1,400]
** @Function  : 获取电池组单体最小电压电池号
** @The notes :
**===============================================================================================*/
u16 GetGCellMinVNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMinVoltAPI(void)
** @Input     : void
** @Output    : 电池组单体最小电压值 1mV
** @Function  : 获取电池组单体最小电压值
** @The notes :
**===============================================================================================*/
u16 GetGCellMinVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMaxTNumAPI(void)
** @Input     : void
** @Output    : 电池组单体最高温度序号[1,400]
** @Function  : 获取电池组单体最高温度序号
** @The notes :
**===============================================================================================*/
u16 GetGCellMaxTNumAPI(void);

/*=================================================================================================
** @Name      : s16 GetGCellMaxTempAPI(void)
** @Input     : void
** @Output    : 电池组单体最高温度值 0.1℃
** @Function  : 获取电池组单体最高温度值
** @The notes :
**===============================================================================================*/
s16 GetGCellMaxTempAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCellMinTNumAPI(void)
** @Input     : void
** @Output    : 电池组单体最低温度序号[1,400]
** @Function  : 获取电池组单体最低温度序号
** @The notes :
**===============================================================================================*/
u16 GetGCellMinTNumAPI(void);

/*=================================================================================================
** @Name      : s16 GetGCellMinTempAPI(void)
** @Input     : void
** @Output    : 电池组单体最低温度值 1℃
** @Function  : 获取电池组单体最低温度值
** @The notes :
**===============================================================================================*/
s16 GetGCellMinTempAPI(void);

/*=================================================================================================
** @Name      : u16 GetGHallSignVoltAPI(void)
** @Input     : void
** @Output    : 电池组霍尔电流源信号电压采样值 1mV
** @Function  : 获取电池组霍尔电流源信号电压采样值
** @The notes :
**===============================================================================================*/
u16 GetGHallSignVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGHallSignVoltAPI(void)
** @Input     : void
** @Output    : 电池组霍尔电流源信号电压采样值 1mV
** @Function  : 获取电池组霍尔电流源信号电压采样值
** @The notes :
**===============================================================================================*/
s16 GetGShuntSampCurrAPI(void);

/*=================================================================================================
** @Name      : s16 GetGShuntSampTempAPI(void)
** @Input     : void
** @Output    : 电池组分流器温度采样值 1℃
** @Function  : 获取电池组分流器温度采样值
** @The notes :
**===============================================================================================*/
s16 GetGShuntSampTempAPI(void);

/*=================================================================================================
** @Name      : s16 GetGDcPwrSampTempAPI(void)
** @Input     : void
** @Output    : 电池组主控低压电源温度采样值 1℃
** @Function  : 获取电池组主控低压电源温度采样值
** @The notes :
**===============================================================================================*/
s16 GetGDcPwrSampTempAPI(void);

/*=================================================================================================
** @Name      : u16 GetGPgndNegVoltAPI(void)
** @Input     : void
** @Output    : 电池组负母线电压值[0,5000]0.1V
** @Function  : 获取电池组负母线电压值
** @The notes :
**===============================================================================================*/
u16 GetGPgndNegVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGLowAccVoltAPI(void)
** @Input     : void
** @Output    : 电池组单激活信号电压值[0,3600]0.01V
** @Function  : 获取电池组激活信号电压值
** @The notes :
**===============================================================================================*/
u16 GetGLowAccVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGLowHall5VVoltAPI(void)
** @Input     : void
** @Output    : 电池组霍尔5V基准电压值[0,500]0.01V
** @Function  : 获取电池组霍尔5V基准电压采样值
** @The notes :
**===============================================================================================*/
u16 GetGLowHall5VVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGFstChgFCVoltAPI(void)
** @Input     : void
** @Output    : 电池组快充枪接入电压值[0,3600]0.01V
** @Function  : 获取电池组快充枪接入电压值
** @The notes :
**===============================================================================================*/
u16 GetGFstChgFCVoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGFstChgCC2VoltAPI(void)
** @Input     : void
** @Output    : 电池组快充枪CC2信号电压值[0,3600]0.01V
** @Function  : 获取电池组快充枪CC2信号电压值
** @The notes :
**===============================================================================================*/
u16 GetGFstChgCC2VoltAPI(void);

/*=================================================================================================
** @Name      : u16 GetGPasInsuResistAPI(void)
** @Input     : void
** @Output    : 电池组正极绝缘值[0,30000]1kΩ
** @Function  : 获取电池组电池正极绝缘值
** @The notes :
**===============================================================================================*/
u16 GetGPasInsuResistAPI(void);

/*=================================================================================================
** @Name      : u16 GetGNegInsuResistAPI(void)
** @Input     : void
** @Output    : 电池组负极绝缘值[0,30000]1kΩ
** @Function  : 获取电池组电池负极绝缘值
** @The notes :
**===============================================================================================*/
u16 GetGNegInsuResistAPI(void);

/*=================================================================================================
** @Name      : u16 GetGNegInsulationAPI(void)
** @Input     : channel:高边开关通道号[0,7]
** @Output    : 电池组高边开关驱动电流值[-1000,1000] 1mA
** @Function  : 获取电池组高边开关驱动电流值
** @The notes :
**===============================================================================================*/
s16 GetGHSDriverCurrAPI(u8 channel);

/*=================================================================================================
** @Name      : u16 GetGCanChgTimeAPI(void)
** @Input     : void
** @Output    : 电池组可充电时间 1min
** @Function  : 获取电池组可充电时间
** @The notes :
**===============================================================================================*/
u16 GetGCanChgTimeAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCanDhgTimeAPI(void)
** @Input     : void
** @Output    : 电池组可放电时间 1min
** @Function  : 获取电池组可放电时间
** @The notes :
**===============================================================================================*/
u16 GetGCanDhgTimeAPI(void);

/*=================================================================================================
** @Name      : u16 GetGChgCycleTimesAPI(void)
** @Input     : void
** @Output    : 电池组充放电循环次数 1次
** @Function  : 获取电池组充放电循环次数
** @The notes :
**===============================================================================================*/
u16 GetGChgCycleTimesAPI(void);

/*=================================================================================================
** @Name      : u16 GetGMonWarNumAPI(void)
** @Input     : void
** @Output    : 电池组当月告警数目 1个
** @Function  : 获取电池组当月告警数目
** @The notes :
**===============================================================================================*/
u16 GetGMonWarNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGMonErrNumAPI(void)
** @Input     : void
** @Output    : 电池组当月故障数目 1个
** @Function  : 获取电池组当月故障数目
** @The notes :
**===============================================================================================*/
u16 GetGMonErrNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGHisWarNumAPI(void)
** @Input     : void
** @Output    : 电池组累计告警数目 1个
** @Function  : 获取电池组累计告警数目
** @The notes :
**===============================================================================================*/
u16 GetGHisWarNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGHisErrNumAPI(void)
** @Input     : void
** @Output    : 电池组累计故障数目 1个
** @Function  : 获取电池组累计故障数目
** @The notes :
**===============================================================================================*/
u16 GetGHisErrNumAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisErrSumNumAPI(void)
** @Input     : void
** @Output    : 电池组累计故障总数目 1个
** @Function  : 获取电池组累计故障总数目
** @The notes : 包括告警数目和故障数目
**===============================================================================================*/
u32 GetGHisErrSumNumAPI(void);

/*=================================================================================================
** @Name      : u16 GetGRemainEnerAPI(void)
** @Input     : void
** @Output    : 电池组当前剩余电能 0.1kWh
** @Function  : 获取电池组当前剩余电能
** @The notes :
**===============================================================================================*/
u16 GetGRemainEnerAPI(void);

/*=================================================================================================
** @Name      : u16 GetGBattAllEnerAPI(void)
** @Input     : void
** @Output    : 电池组电池总电能 0.1kWh
** @Function  : 获取电池组电池总电能
** @The notes :
**===============================================================================================*/
u16 GetGBattAllEnerAPI(void);

/*=================================================================================================
** @Name      : u16 GetGNowChgCapAPI(void)
** @Input     : void
** @Output    : 电池组当前单次充电量 1Ah
** @Function  : 获取电池组当前单次充电量
** @The notes :
**===============================================================================================*/
u16 GetGNowChgCapAPI(void);

/*=================================================================================================
** @Name      : u16 GetGNowDhgCapAPI(void)
** @Input     : void
** @Output    : 电池组当前单次放电量 1Ah
** @Function  : 获取电池组当前单次放电量
** @The notes :
**===============================================================================================*/
u16 GetGNowDhgCapAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisChgCapAPI(void)
** @Input     : void
** @Output    : 电池组累计充电电量 1Ah
** @Function  : 获取电池组累计充电电量
** @The notes :
**===============================================================================================*/
u32 GetGHisChgCapAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisDhgCapAPI(void)
** @Input     : void
** @Output    : 电池组累计放电电量 1Ah
** @Function  : 获取电池组累计放电电量
** @The notes :
**===============================================================================================*/
u32 GetGHisDhgCapAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisChgEnerAPI(void)
** @Input     : void
** @Output    : 电池组累计充电电能 0.1kWh
** @Function  : 获取电池组累计充电电能
** @The notes :
**===============================================================================================*/
u32 GetGHisChgEnerAPI(void);

/*=================================================================================================
** @Name      : u32 GetGHisDhgEnerAPI(void)
** @Input     : void
** @Output    : 电池组累计放电电能 0.1kWh
** @Function  : 获取电池组累计放电电能
** @The notes :
**===============================================================================================*/
u32 GetGHisDhgEnerAPI(void);

/*=================================================================================================
** @Name      : u16 GetGStatistNowDodAPI(void)
** @Input     : void
** @Output    : 电池组当前放电深度DOD 0.1%
** @Function  : 获取电池组当前放电深度DOD
** @The notes :
**===============================================================================================*/
u16 GetGStatistNowDodAPI(void);

/*=================================================================================================
** @Name      : u16 GetGStatistNowDodAPI(void)
** @Input     : void
** @Output    : 电池组最大放电深度DOD 0.1%
** @Function  : 获取电池组最大放电深度DOD
** @The notes :
**===============================================================================================*/
u16 GetGStatistMaxDodAPI(void);

/*=================================================================================================
** @Name      : u16 GetGCtrlResultAPI(u8 index)
** @Input     : index:子索引号[0,1]
** @Output    : 电池组控制结果信息
** @Function  : 获取电池组控制结果信息
** @The notes :
**===============================================================================================*/
u16 GetGCtrlResultAPI(u8 index);

/*=================================================================================================
** @Name      : u16 GetGDebugResultAPI(u8 index)
** @Input     : index:子索引号[0,8]
** @Output    : 电池组调试结果信息
** @Function  : 获取电池组调试结果信息
** @The notes :
**===============================================================================================*/
u16 GetGDebugResultAPI(u8 index);

/*=================================================================================================
** @Name      : u16 GetGSbcAux5VVoltAPI(void)
** @Input     : void
** @Output    : 电池组SBC5V基准电压采样值 0.01V
** @Function  : 获取电池组SBC5V基准电压采样值
** @The notes :
**===============================================================================================*/
u16 GetGSbcAux5VVoltAPI(void);

/*=================================================================================================
** @Name      : s8 GetGBalanMaxTempAPI(void)
** @Input     : void
** @Output    : 电池组均衡板温最大值 1℃
** @Function  : 获取电池组均衡板温最大值
** @The notes :
**===============================================================================================*/
s16 GetGBalanMaxTempAPI(void);

/*=================================================================================================
** @Name      : u8 GetGBalanMaxTSlaveAPI(void)
** @Input     : void
** @Output    : 电池组最大均衡板温从控号[1,400]
** @Function  : 获取电池组最大均衡板温从控
** @The notes :
**===============================================================================================*/
u8 GetGBalanMaxTSlaveAPI(void);

/*=================================================================================================
** @Name      : u8 GetGHigPGNDSwhStateAPI(void)
** @Input     : void
** @Output    : 电池组负极PGND继电器状态 0:断开 1:闭合
** @Function  : 获取电池组负极PGND继电器状态
** @The notes :
**===============================================================================================*/
u8 GetGHigPGNDSwhStateAPI(void);

/*=================================================================================================
** @Name      : u16 GetGChgBalanStateAPI(u16 index)
** @Input     : index:索引号[0,24]
** @Output    : 电池组单体充电均衡状态(按bit位表示 0关闭 1均充)
** @Function  : 获取电池组单体充电均衡状态
** @The notes : 指定索引非从控序号
**===============================================================================================*/
u16 GetGChgBalanStateAPI(u16 index);

/*=================================================================================================
** @Name      : u16 GetGDhgBalanStateAPI(u16 index)
** @Input     : index:索引号[0,24]
** @Output    : 电池组单体放电均衡状态(按bit位表示 0关闭 1均放)
** @Function  : 获取电池组单体放电均衡状态
** @The notes : 指定索引非从控序号
**===============================================================================================*/
u16 GetGDhgBalanStateAPI(u16 index);

/*=================================================================================================
** @Name      : u16 GetGCellVoltAPI(void)
** @Input     : cell:电池号[0,399]
** @Output    : 电池组单体电压值 1mV
** @Function  : 获取电池组单体电压值
** @The notes :
**===============================================================================================*/
u16 GetGCellVoltAPI(u16 cell);

/*=================================================================================================
** @Name      : s8 GetGCellTempAPI(u16 cell)
** @Input     : cell:电池号[0,399]
** @Output    : 电池组单体温度值 1℃
** @Function  : 获取电池组单体温度值
** @The notes :
**===============================================================================================*/
s16 GetGCellTempAPI(u16 cell);

#if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))
/*=================================================================================================
** @Name      : u16 GetGCellVoltAPI(void)
** @Input     : cell:电池号[0,399]
** @Output    : 电池组单体内阻值 0.01mΩ
** @Function  : 获取电池组单体内阻值
** @The notes :
**===============================================================================================*/
u16 GetGCellRsisAPI(u16 cell);
#endif

/*=================================================================================================
** @Name      : s8 GetGBalanTempAPI(u16 cell)
** @Input     : cell:电池号[0,63]
** @Output    : 电池组单体均衡温度值 1℃
** @Function  : 获取电池组单体均衡温度值
** @The notes :
**===============================================================================================*/
s16 GetGBalanTempAPI(u16 cell);

/*=================================================================================================
** @Name      : s8 GetGBalanSlavMaxTAPI(u8 slave)
** @Input     : slave:从控号[0,31]
** @Output    : 电池组单体均衡温度从控内部最大温度值 1℃
** @Function  : 获取电池组单体均衡温度从控内部最大温度值
** @The notes :
**===============================================================================================*/
s16 GetGBalanSlavMaxTAPI(u8 slave);

/*=================================================================================================
** @Name      : u8 GetGroupOffsetDevAddrAPI(void)
** @Input     : void
** @Output    : 电池组自身设备地址偏移量
** @Function  : 获取电池组自身偏移设备地址
** @The notes : 主控设备地址偏移量
**===============================================================================================*/
u8 GetGroupOffsetDevAddrAPI(void);

/*=================================================================================================
** @Name      : u16 ChangSlaveVidToGroupVid(u8 slave, u8 cell)
** @Input     : slave:从控号[0,31] cell:从控内电池号[0,17]
** @Output    : 从控内电压序号对应组内电压序号[0,399],0xffff:错误序号
** @Function  : 将从控内电压序号转换成电池组内电压序号
** @The notes : 每个从控电压数目不一样的需要特殊处理
**===============================================================================================*/
u16 ChangSlaveVidToGroupVid(u8 slave, u8 cell);

/*=================================================================================================
** @Name      : u16 ChangSlaveTidToGroupVid(u8 slave, u8 cell)
** @Input     : slave:从控号[0,31] cell:从控内温度号[0,8]
** @Output    : 从控内温度序号对应组内温度序号[0,199],0xffff:错误序号
** @Function  : 将从控内温度序号转换成电池组内温度序号
** @The notes : 每个从控温度数目不一样的需要特殊处理
**===============================================================================================*/
u16 ChangSlaveTidToGroupTid(u8 slave, u8 cell);

/*=================================================================================================
** @Name      : u8 ChangGroupTidToSlaveNum(u16 id)
** @Input     : id:组内温度序号[0,399]
** @Output    : 组内温度序号所在从控序号[0,31],0xffff:错误序号
** @Function  : 将电池组内温度序号转换成所在从控序号
** @The notes : 每个从控温度数目不一样的需要特殊处理
**===============================================================================================*/
u8 ChangGroupTidToSlaveNum(u16 id);

/*=================================================================================================
** @Name      : u8 ChangGroupTidToSlaveTid(u16 id)
** @Input     : id:组内温度序号[0,399]
** @Output    : 组内温度序号所在从控内温度序号[0,4],0xffff:错误序号
** @Function  : 将电池组内温度序号转换成所在从控内温度序号
** @The notes : 每个从控温度数目不一样的需要特殊处理
**===============================================================================================*/
u8 ChangGroupTidToSlaveTid(u16 id);
//#if(1 == BMU_IMEU3_COM_EN)
/*=================================================================================================
** @Name      : s8 GetImeu3TempInfoAPI(e_IMEU3TempHumInfo index)
** @Input     : index:IMEU3信号信息序号
** @Output    : IMEU3温湿度信息值
** @Function  : 获取电池组IMEU3温湿度信息值
** @The notes :
**===============================================================================================*/
s8 GetImeu3TempInfoAPI(e_IMEU3TempHumInfo index);
/*=================================================================================================
** @Name      : u16 GetLeakageInfoAPI(e_LeakageInfo index)
** @Input     : index:漏电流传感器信息序号
** @Output    : 漏电流传感器信息
** @Function  : 获取漏电流传感器信息
** @The notes :
**===============================================================================================*/
u16 GetLeakageInfoAPI(e_LeakageInfo index);
/*=================================================================================================
** @Name      : u16 GetPduInImeu3InfoAPI(e_PduInInfo index)
** @Input     : index:IMEU3信号信息序号
** @Output    : PDU内IMEU3信息值
** @Function  : 获取PDU内IMEU3信息值
** @The notes :
**===============================================================================================*/
u16 GetPduInImeu3InfoAPI(e_PduInInfo index);
/*=================================================================================================
** @Name      : u16 GetPduInImeu3InfoAPI(e_PduInInfo index)
** @Input     : index:IMEU3信号信息序号
** @Output    : PDU外IMEU3信息值
** @Function  : 获取PDU外IMEU3信息值
** @The notes :
**===============================================================================================*/
u16 GetPduOutImeu3InfoAPI(e_PduOutInfo index);
/*=============================================================================================
** @Name      : void ClrPduInImeu3AllInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除PDU内IMEU3信息
** @The notes : PDU内IMEU3信息接收中断时使用
**===========================================================================================*/
void ClrPduInImeu3AllInfoAPI(void);
/*=============================================================================================
** @Name      : void ClrPduOutImeu3AllInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除PDU外IMEU3信息
** @The notes : PDU外IMEU3信息接收中断时使用
**===========================================================================================*/
void ClrPduOutImeu3AllInfoAPI(void);
/*=============================================================================================
** @Name      : void ClrGPerLeakageAllInfoAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除所有漏电流传感器接收信息
** @The notes : 漏电流传感器通讯中断时使用
**===========================================================================================*/
void ClrGPerLeakageAllInfoAPI(void);
/*=================================================================================================
** @Name      : u8 GetFireLevelInfoAPI(e_FireInfo254 index)
** @Input     : index:消防等级信息数组
** @Output    : 消防等级标志
** @Function  : 获取不同等级消防标志
** @The notes :
**===============================================================================================*/
u8 GetFireLevelInfoAPI(e_FireInfo254 index);
/*=================================================================================================
** @Name      : void SetFireLevelInfoAPI(e_FireInfo254 index, u8 Flag)
** @Input     : index:消防等级,Flag:标志
** @Output    :
** @Function  : 设置不同消防等级标志
** @The notes :
**===============================================================================================*/
void SetFireLevelInfoAPI(e_FireInfo254 index, u8 Flag);
//#endif


#endif
