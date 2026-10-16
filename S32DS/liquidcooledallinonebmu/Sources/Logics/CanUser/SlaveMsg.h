/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : SlaveMsg.h
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : 从控CAN报文处理模块
** @Instructions : 
**===============================================================================================*/
#ifndef _SLAVEMSG_H
#define _SLAVEMSG_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "PublicInfo.h"

//使能自研从控BSU CAN通信信息
#if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))
/*=================================================================================================
** @Macro definition
**===============================================================================================*/
typedef enum                                /*功能码*/
{
    RequestModif = 2,                       /*请求修改*/
    AnswerModif = 3,                        /*修改回复*/
    RequestQuery = 5,                       /*请求查询*/
    AnswerQuery = 6,                        /*查询回复*/
}e_BsuBarcodeMsgFunCode;


typedef enum                                /*信息类型*/
{
    BsuBarcode = 0xE6,                      /*BSU板子条码*/
    PackBarcode = 0xE7,                     /*PACK条码*/
    CellBarcode = 0xE8,                     /*电芯条码*/
    WritePermission = 0xE9,                 /*写入权限*/
}e_BsuBarcodeMsgTypeCode;
/*=================================================================================================
** @Data type definition
**===============================================================================================*/

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void GroupCheckRcvBsuMsgAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 主控判断从控信息采集完成任务
** @The notes : 用户10ms周期接口调用
**===============================================================================================*/
void GroupCheckRcvBsuMsgAPI(void);

/*=================================================================================================
** @Name      : void CanRcvSlaveMsgHandleAPI(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 自研从控协议CAN报文接收处理
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvSlaveMsgHandleAPI(u32 ID, u8 *pdata, u8 DLC, u8 canNum);
/*=================================================================================================
** @Name      : void SendResetAFEstatus(u16 slave, u16 Data)
** @Input     : slave:从控序号[0,9],0xff:所有从控  Data:重置AFE状态标志(1:重置)
** @Output    : void
** @Function  : 发送请求从控重置AFE状态命令CAN报文(按单个从控[通信板]控制)
** @The notes : 50ms周期
**===============================================================================================*/
void SendResetAFEstatus(u16 slave, u16 Data);
/*=================================================================================================
** @Name      : void SendResetBSUstatus(u16 slave, u16 Data)
** @Input     : slave:从控序号[0,9],0xff:所有从控  Data:重置BSU状态标志(1:重置)
** @Output    : void
** @Function  : 发送请求从控重置BSU状态命令CAN报文(按单个从控[通信板]控制)
** @The notes : 50ms周期
**===============================================================================================*/
void SendResetBSUstatus(u16 slave, u16 Data);

//XZL40506 251227
/*=================================================================================================
** @Name      : void CanRcvBsuBarcodeMsgHandleAPI(t_CANMSG *msg, u8 canNum)
** @Input     : msg:接收到的报文 canNum:接收CAN通道号
** @Output    : void
** @Function  : 转发BSU条码相关报文
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvBsuBarcodeMsgHandleAPI(t_CANMSG *msg, u8 canNum);
#endif

#endif
