/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : DiagSos.h
** @Founder      : ZSY342
** @Date         : 2019.10.30
** @Function     : 系统SOS诊断模块
** @Instructions :
**===========================================================================================*/
#ifndef _DIAGSOS_H
#define _DIAGSOS_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#define SOS_DIAG_TATIS_EN         1             //系统安全等级统计执行使能
#define SOS_5_SystemLock_EN		  0				//5级sos系统锁使能,1为使能系统锁
/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#if(1 == SOS_5_SystemLock_EN)
#define SOS_5_CellOverVoltLimit   		3800			//sos5级单体电芯过压触发值（0.01mV）
#define SOS_5_CellUnderVoltLimit 	 	2000			//sos5级单体电芯欠压触发值（0.01mV）
#define SOS_5_SysOverVoltCellLimit	 	38				//sos5级系统总压过压单体触发值（0.1V）,总压 = 单体电压*总电芯数
#define SOS_5_SysUnderVoltCellLimit  	20				//sos5级系统总压欠压单体触发值（0.1V）,总压 = 单体电压*总电芯数
#define SOS_5_CellChgOverTempLimit	 	600				//sos5级电芯充电过温触发值（0.1℃）
#define SOS_5_CellDhgOverTempLimit		600          	//sos5级电芯放电过温触发值（0.1℃）
#define SOS_5_CellChgUnderTempLimit		0				//sos5级电芯充电欠温触发值（0.1℃）
#define SOS_5_CellDhgUnderTempLimit		-200			//sos5级电芯放电欠温触发值（0.1℃）
#define SOS_5_ChgCurrLimit		  		2850			//sos5级充电电流过流触发值（0.1A）
#define SOS_5_DhgCurrLimit		 		3648			//sos5级放电电流过流触发值（0.1A）
#endif
/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*SOS异常原因*/
typedef enum
{
    eSOS_REASION_ERR = 0,                   /*故障告警原因*/
    eSOS_REASION_VEX,                       /*电压极限值原因*/
    eSOS_REASION_VDI,                       /*电压压差大原因*/
    eSOS_REASION_VDN,                       /*电压压降大原因*/
    eSOS_REASION_TEX,                       /*温度极限值原因*/
    eSOS_REASION_TDI,                       /*温度温差大原因*/
    eSOS_REASION_TUP,                       /*温度温升大原因*/
    eSOS_REASION_BTE,                       /*均衡板温极限值原因*/
    eSOS_REASION_DTE,                       /*电源板温极限值原因*/
    eSOS_REASION_CEX,                       /*电流瞬时极限值原因*/

	//0等级
    eSOS_REASION_CER,                       /*电流待机电流异常原因*/
    eSOS_REASION_CAP,                       /*电池总容量衰减原因*/
    eSOS_REASION_CEL,                       /*单体充放电保护原因*/
    eSOS_REASION_THD,                       /*单体散热失效原因*/
    eSOS_REASION_DOD,                       /*长期DOD未达标原因*/
    eSOS_REASION_EEX,                       /*充放电效率未达标原因*/

    eSOS_REASION_LEN                        /*原因个数*/
}eSos_Reasion;
#if(1 == SOS_5_SystemLock_EN)
/*SOS5级系统锁触发原因*/
typedef enum
{
    eSOSSYSLOCK_REASION_ERR = 0,                   /*故障告警原因*/
    eSOSSYSLOCK_REASION_COV,                       /*电芯电压>=3.8V 持续 1s*/
    eSOSSYSLOCK_REASION_CUV,                       /*电芯电压<=2.0V 持续 1s*/
    eSOSSYSLOCK_REASION_SOV,                       /*系统总压>=912V 持续 1s*/
    eSOSSYSLOCK_REASION_SUV,                       /*系统总压<=480V 持续 1s*/
    eSOSSYSLOCK_REASION_COT,                       /*电芯充电温度>=60℃ 持续 6s*/
    eSOSSYSLOCK_REASION_DOT,                       /*电芯放电温度>=60℃ 持续 6s*/
    eSOSSYSLOCK_REASION_CUT,                       /*电芯充电温度<=0℃ 持续 6s*/
    eSOSSYSLOCK_REASION_DUT,                       /*电芯放电温度<=-20℃  持续 6s*/
    eSOSSYSLOCK_REASION_COC,                       /*充电电流>=285A 持续 6s*/
    eSOSSYSLOCK_REASION_DOC,                       /*放电电流>=364.8A 持续 6s*/

    eSOSSYSLOCK_REASION_LEN                        /*原因个数*/
}eSosSysLock_Reason;
#endif
/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void DiagGroupSosInit(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组安全等级SOS诊断初始化
** @The notes : 读取是否到达锁定等级
**===========================================================================================*/
void DiagGroupSosInit(void);

/*=============================================================================================
** @Name      : void DiagGroupSosTask(void)
** @Input     : void
** @Output    : void
** @Function  : 电池组安全等级SOS诊断任务
** @The notes : 100ms
**===========================================================================================*/
void DiagGroupSosTask(void);

/*=============================================================================================
** @Name      : u8 DiagGetGroupSosLevel(void)
** @Input     : void
** @Output    : 电池组总安全等级SOS
** @Function  : 获取电池组总安全等级SOS
** @The notes : 包括当前等级和推断等级
**===========================================================================================*/
u8 DiagGetGroupSosLevel(void);

/*=============================================================================================
** @Name      : u8 DiagGetGroupSosState(u8 sort)
** @Input     : sort:SOS类型(0:当前SOS 1:预测SOS)
** @Output    : SOS安全等级状态
** @Function  : 获取电池组指定SOS安全等级状态
** @The notes :
**===========================================================================================*/
u8 DiagGetGroupSosState(u8 sort);

/*=============================================================================================
** @Name      : u16 DiagGetGroupSosResion(u8 sort)
** @Input     : sort:SOS类型(0:当前SOS 1:预测SOS)
** @Output    : SOS安全等级异常原因
** @Function  : 获取电池组指定SOS安全等级异常原因
** @The notes : 按bit位表示原因
**===========================================================================================*/
u16 DiagGetGroupSosResion(u8 sort);

/*=============================================================================================
** @Name      : void DiagClrGroupSosLevel(u8 flag)
** @Input     : flag:清除标志(0:清除普通级别 0xff:清除锁定级别)
** @Output    : void
** @Function  : 清除电池组总安全等级SOS
** @The notes : 包括当前等级和推断等级
**===========================================================================================*/
void DiagClrGroupSosLevel(u8 flag);
#if(1 == SOS_5_SystemLock_EN)
/*=================================================================================================
** @Name      : u32 DiagGetSosSysLockErrorBitAPI(void)
** @Input     : void
** @Output    : 5级sos异常标志(按位表示触发系统锁原因)
** @Function  : 获取5级sos异常标志(按位表示触发系统锁原因)
** @The notes : 按位表示1个通道
**===============================================================================================*/
u32 DiagGetSosSysLockErrorBitAPI(void);

/*=================================================================================================
** @Name      : void DiagJudgeSosSysLockBitErrorAPI(u8 err, u8 flag)
** @Input     : err:触发sos系统锁原因标志位 flag:1设置错误标志位 0清除错误标志位
** @Output    : void
** @Function  : 设置sos系统锁异常标志(按位表示触发sos5级系统锁原因)
** @The notes : 按位表示1个原因
**===============================================================================================*/
void DiagJudgeSosSysLockBitErrorAPI(u8 err, u8 flag);
#endif
#endif
