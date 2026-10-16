/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : AuxDevMsg.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 电池组与辅助设备通信CAN报文处理模块
** @Instructions : 
**===============================================================================================*/
#ifndef _AUXDEVMSG_H
#define _AUXDEVMSG_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "GroupInfo.h"


/*=================================================================================================
** @Macro definition
**===============================================================================================*/

/*=================================================================================================
** @Data type definition
**===============================================================================================*/
extern u8 sLastPduFanDuty;       // 上次PduFan——PWM占空比
extern u8 sLastPcsFanDuty;       // 上次PcsFan——PWM占空比
extern u8 sPduFanStartDetected;  // Pdu风扇启动检测标志
extern u8 sPcsFanStartDetected;  // Pcs风扇启动检测标志
/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CanRcvImeu2MsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : CAN通信IMSU-D接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
void CanRcvIMSUDMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum);
//#if(BMU_IMEU3_COM_EN == 1)
/*=================================================================================================
** @Name      : void CanRcvImeu2MsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : CAN通信IMEU3接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
void CanRcvImeu3MsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum);
/*=================================================================================================
** @Name      : SendCanImeu3PwmCtrlMsg241(u8 Fan1Duty, u8 Fan2Duty)
** @Input     : Fan1Duty:1号风扇占空比 Fan2Duty:2号风扇占空比
** @Output    : void
** @Function  : 发送IMEU3用户自定义控制信息CAN报文
** @The notes : 主索引241
**===============================================================================================*/
void SendCanImeu3PwmCtrlMsg241(u8 Fan1Duty, u8 Fan2Duty);
/*=================================================================================================
** @Name      : void SendCanExhaustCtrlMsg241(u8 FanDuty)
** @Input     : FanDuty:风扇占空比
** @Output    : void
** @Function  : 发送排气扇控制信息CAN报文
** @The notes : 主索引241
**===============================================================================================*/
void SendCanExhaustCtrlMsg241(u8 FanDuty);
/*=================================================================================================
** @Name      : void SendCanImeu3IoCtrlMsg242(u8 addr,u8 index, u8 cmd)
** @Input     : addr:IMEU3通信地址 index:索引号 cmd:命令号
** @Output    : void
** @Function  : 发送IMEU3IO控制信息CAN报文
** @The notes : 主索引242
**===============================================================================================*/
void SendCanImeu3IoCtrlMsg242(u8 addr,u8 index, u8 cmd);
/*=================================================================================================
** @Name      : void SendCanImeu3MotorCtrlMsg243(u8 state)
** @Input     : state:泄爆阀状态
** @Output    : void
** @Function  : 发送IMEU3泄爆阀控制信息CAN报文
** @The notes : 主索引243
**===============================================================================================*/
void SendCanImeu3MotorCtrlMsg243(u8 state);
/*=================================================================================================
** @Name      : void SendCanImeu3DeParaMsg245(u8 OpenDeHum, u8 CloseDeHum,u8 WorkMode)
** @Input     : state:泄爆阀状态
** @Output    : void
** @Function  : 发送IMEU3除湿机参数设置
** @The notes : 主索引243
**===============================================================================================*/
void SendCanImeu3DeParaMsg245(u8 OpenDeHum, u8 CloseDeHum,u8 WorkMode);
/*=================================================================================================
** @Name      : u32 CanBuildRupAuxMsgID(u8 func, u8 index, u8 dadrr, u8 saddr)
** @Input     : func:功能码 index:主索引 dadrr:目标地址 saddr:源地址
** @Output    : CAN报文ID
** @Function  : 用户创建上传CAN报文ID
** @The notes : 电池组内部下行报文
**===============================================================================================*/
u32 CanBuildRupAuxMsgID(u8 func, u8 index, u8 dadrr, u8 saddr);
//#endif

#endif
