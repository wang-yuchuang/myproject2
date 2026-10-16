/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : AuxDevMsg.c
** @Founder      : ZQD808
** @Date         : 2024.09.20
** @Function     : 电池组与辅助设备通信CAN报文处理模块
** @Instructions : 
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "GroupInfo.h"
#include "DiagInterface.h"
#include "TacInterface.h"
#include "AuxDevTactic.h"
#include "AuxDevMsg.h"
#include "DiagMaker.h"
//#if(1 == BMU_IMEU3_COM_EN)
extern u16 gGBmuGenPara_102[];  		/*主控通用参数值*/
extern s8 gImeu3TempHumInfo_217[];  	/*IMEU3温度信息*/
extern u16 gImeu3PduInInfo_218[];       /*PDU内部IMEU3信息*/
extern u16 gImeu3PduOutInfo_219[];      /*PDU外部IMEU3信息*/
extern u8 gImeu3DeInfo_220[]; 	        /*PDU外部IMEU3除湿机信息*/
extern u8 gGHardPara_104[];             /*主控硬件参数配置信息*/
u8 sLastPduFanDuty = 0;       // 上次PduFan——PWM占空比
u8 sLastPcsFanDuty = 0;       // 上次PcsFan——PWM占空比
u8 sPduFanStartDetected = FALSE;  // 风扇启动检测标志
u8 sPcsFanStartDetected = FALSE;  // 风扇启动检测标志
/*=================================================================================================
** @Name      : void CanRcvImeu2MsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : CAN通信IMEU3接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
void CanRcvImeu3MsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	u8 i = 0;
	u8 saddr = 0;
	u8 daddr = 0;
	u8 index = 0;
	u8 funct = 0;
	/*解析CAN帧ID*/
	saddr = ID & 0xff;
	daddr = (ID >> 8) & 0xff;
	index = (ID >> 16) & 0xff;
	funct = (ID >> 25) & 0x0f;


	/*非IMEU3报文,非发送给BMU报文,非对应CAN通道*/
	if((canNum != eCAN0) || ((IMEU3_PDUIN_DEVADDR != saddr) && (IMEU3_PDUOUT_DEVADDR != saddr))
		|| ((gGBmuGenPara_102[eBmuGenPara102_BmuDev] != daddr) && (0xff  != daddr))
		|| (DLC < 8) || (funct != 10))
	{
		return;
	}
	/*接收IMEU3报文计数*/
	DiagRcvDevMsgAddAPI(saddr);

	/*按信息索引解析接收到的IMEU3报文信息*/
	switch(index)
	{
		/*1# ~ 8#NTC或者PT1000温度值*/
		case 203:
			/*PDU内温度，偏移量-50℃*/
			if(saddr == IMEU3_PDUIN_DEVADDR)
			{
				for(i = 0; i< 3; i++)
				{
					if(pdata[i] > 177)
					{
						gImeu3TempHumInfo_217[eIMEU3217_PcsAiroutT + i] = 127;
					}
					else if(pdata[i] < 10)
					{
						gImeu3TempHumInfo_217[eIMEU3217_PcsAiroutT + i] = -40;
					}
					else
					{
						gImeu3TempHumInfo_217[eIMEU3217_PcsAiroutT + i] = (s8)(pdata[i] - 50);
					}
				}
			}
			else if(saddr == IMEU3_PDUOUT_DEVADDR)
			{
				if((gGHardPara_104[eGHardPara104_ProjectCode]== 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
					|| (gGHardPara_104[eGHardPara104_ProjectCode]== 5) || (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
					|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
				{
					for(i = 0; i< 2; i++) 
					{
						if(pdata[i] > 177)
						{
							gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i] = 127;
						}
						else if(pdata[i] < 10)
						{
							gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i] = -40;
						}
						else
						{
							gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i] = (s8)(pdata[i] - 50);
						}
					}
				}
				else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
					|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4) )
				{
					//为了兼容A2425两组BMU共用一个IMEU3
					for(i = 0; i< 2; i++)
					{
						if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
						{
							if(pdata[i] > 177)
							{
								gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i] = 127;
							}
							else if(pdata[i] < 10)
							{
								gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i] = -40;
							}
							else
							{
								gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i] = (s8)(pdata[i] - 50);
							}
						}
					}
					for(i = 2; i< 4; i++)
					{
						if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 51)
						{
							if(pdata[i] > 177)
							{
								gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i - 2] = 127;
							}
							else if(pdata[i] < 10)
							{
								gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i - 2] = -40;
							}
							else
							{
								gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i - 2] = (s8)(pdata[i] - 50);
							}
						}
					}
				}
				else
				{
					for(i = 0; i< 2; i++)
					{
						if(pdata[i] > 177)
						{
							gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i] = 127;
						}
						else if(pdata[i] < 10)
						{
							gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i] = -40;
						}
						else
						{
							gImeu3TempHumInfo_217[eIMEU3217_CabT1 + i] = (s8)(pdata[i] - 50);
						}
					}
				}
			}
			else
			{
				//预留
			}
			break;
		/*水浸频率*/
		case 204:
		if(saddr == IMEU3_PDUOUT_DEVADDR)
		{
			gImeu3PduOutInfo_219[ePduOut219_FloodHz] = (u16)((pdata[7] << 8) | (pdata[6]));
		}
		break;
		/*DIDO状态*/
		case 205:
			if(saddr == IMEU3_PDUIN_DEVADDR)
			{
				gImeu3PduInInfo_218[ePduIn218_DI1_16] = (u16)((pdata[1] << 8) | (pdata[0]));
				gImeu3PduInInfo_218[ePduIn218_DI17_22_DO1_8] = (u16)((pdata[3] << 8) | (pdata[2]));
				gImeu3PduInInfo_218[ePduIn218_DO9_24] = (u16)((pdata[5] << 8) | (pdata[4]));
				gImeu3PduInInfo_218[ePduIn218_X1DI] = (u16)pdata[6];
			}
			else if(saddr == IMEU3_PDUOUT_DEVADDR)
			{
				gImeu3PduOutInfo_219[ePduOut219_DI1_16] = (u16)((pdata[1] << 8) | (pdata[0]));
				gImeu3PduOutInfo_219[ePduOut219_DI17_22_DO1_8] = (u16)((pdata[3] << 8) | (pdata[2]));
				gImeu3PduOutInfo_219[ePduOut219_DO9_24] = (u16)((pdata[5] << 8) | (pdata[4]));
				gImeu3PduOutInfo_219[ePduOut219_X1DI] = (u16)pdata[6];
			}
			else
			{
				//预留
			}
			break;
		/*485温湿度传感器数据*/
		case 211:
			if(saddr == IMEU3_PDUOUT_DEVADDR)
			{
				for(i = 0; i < 4; i++)
				{
					//温度取有效值
					if(0 == (i % 2))
					{
						if(pdata[i] > 177)
						{
							gImeu3TempHumInfo_217[eIMEU3217_485T1 + i] = 127;
						}
						else if(pdata[i] < 10)
						{
							gImeu3TempHumInfo_217[eIMEU3217_485T1 + i] = -40;
						}
						else
						{
							gImeu3TempHumInfo_217[eIMEU3217_485T1 + i] = (s8)(pdata[i] - 50);
						}
					}
					//湿度取有效值
					else
					{
						if(pdata[i] > 110)
						{
							gImeu3TempHumInfo_217[eIMEU3217_485T1 + i] = 100;
						}
						else if(pdata[i] < 10)
						{
							gImeu3TempHumInfo_217[eIMEU3217_485T1 + i] = 0;
						}
						else
						{
							gImeu3TempHumInfo_217[eIMEU3217_485T1 + i] = pdata[i];
						}
					}
				}
			}
			break;
		/*可燃气体浓度信息*/
		case 212:
			if(saddr == IMEU3_PDUOUT_DEVADDR)
			{
				gImeu3PduOutInfo_219[ePduOut219_Gas1] = (u16)((pdata[1] << 8) | (pdata[0]));
				gImeu3PduOutInfo_219[ePduOut219_Gas2] = (u16)((pdata[3] << 8) | (pdata[2]));
			}
			break;
		/*告警*/
		case 213:
			if(saddr == IMEU3_PDUIN_DEVADDR)
			{
				gImeu3PduInInfo_218[ePduIn218_Alarm1] = pdata[1];
				gImeu3PduInInfo_218[ePduIn218_Alarm2] = pdata[3];
			}
			else if(saddr == IMEU3_PDUOUT_DEVADDR)
			{
				gImeu3PduOutInfo_219[ePduOut219_Alarm1] = pdata[0];
				gImeu3PduOutInfo_219[ePduOut219_Alarm2] = pdata[1];
				gImeu3PduOutInfo_219[ePduOut219_Alarm3] = pdata[3];
			}
			break;
		/*除湿机信息*/
		case 215:
			if(saddr == IMEU3_PDUOUT_DEVADDR)
			{
				gImeu3DeInfo_220[eDeInfo220_Hum] = ((pdata[1] << 8) | (pdata[0]));
				gImeu3DeInfo_220[eDeInfo220_OpenDeHum] = ((pdata[3] << 8) | (pdata[2]));
				gImeu3DeInfo_220[eDeInfo220_CloseDeHum] = ((pdata[5] << 8) | (pdata[4]));
				gImeu3DeInfo_220[eDeInfo220_State] = pdata[6];
				gImeu3DeInfo_220[eDeInfo220_Addr] = pdata[7];
			}
			break;
		default:
			break;
	}
}
/*=================================================================================================
** @Name      : void CanRcvImeu2MsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域  DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : CAN通信IMSU-D接收处理任务
** @The notes : 用户放在CAN接收处理HOOK接口调用
**===============================================================================================*/
void CanRcvIMSUDMsgHandle_API(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	u8 saddr = 0;
	u8 daddr = 0;
	u8 funct = 0;
	u8 index = 0;
	/*解析CAN帧ID*/
	saddr = ID & 0xff;
	daddr = (ID >> 8) & 0xff;
	funct = (ID >> 25) & 0x0f;
	index = (ID >> 16) & 0xff;
	/*非IMSUD报文,非发送给PDU内IMEU3报文,非对应CAN通道*/
	if((canNum != eCAN2) || (IMEU3_PDUIN_DEVADDR != daddr)
		|| ((0xf0  != saddr))
		|| (DLC < 8) || (funct != 2)
		|| (index != 0xF2))
	{
		return;
	}
	/*控制IMEU3控制MPPT光伏接触器控制*/
	if((pdata[0] == 0xc1) && (pdata[1] == 0x05) && (pdata[2] < 2))
	{
		SendCanImeu3IoCtrlMsg242(IMEU3_PDUIN_DEVADDR, pdata[1],pdata[2]);
	}
}
/*=================================================================================================
** @Name      : u32 CanBuildRupAuxMsgID(u8 func, u8 index, u8 dadrr, u8 saddr)
** @Input     : func:功能码 index:主索引 dadrr:目标地址 saddr:源地址
** @Output    : CAN报文ID
** @Function  : 用户创建上传CAN报文ID
** @The notes : 电池组内部下行报文
**===============================================================================================*/
u32 CanBuildRupAuxMsgID(u8 func, u8 index, u8 dadrr, u8 saddr)
{
	u32 id = 0;

	id = ((u32)func << 25) | ((u32)1 << 24) | ((u32)index << 16) | ((u32)dadrr << 8) | saddr;

	return(id);
}
/*=================================================================================================
** @Name      : void SendCanImeu3PwmCtrlMsg241(u8 Fan1Duty, u8 Fan2Duty)
** @Input     : Fan1Duty:1号风扇占空比 Fan2Duty:2号风扇占空比
** @Output    : void
** @Function  : 发送IMEU3PWM控制信息CAN报文
** @The notes : 主索引241
**===============================================================================================*/
void SendCanImeu3PwmCtrlMsg241(u8 Fan1Duty, u8 Fan2Duty)
{
	t_CANMSG msg;
	//参数有效
	if(Fan1Duty > 100 || Fan2Duty > 100)
	{
		return;
	}
	//A2445项目禁止超过70%占空比
	if((Fan1Duty > 70) && (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
	{
		Fan1Duty = 70;
	}
	if((Fan2Duty > 70) && (gGHardPara_104[eGHardPara104_ProjectCode] == 1))
	{
		Fan2Duty = 70;
	}

    /*请求修改帧发送221索引*/
    msg.ID = CanBuildRupAuxMsgID(2, 241, IMEU3_PDUIN_DEVADDR, (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev]);

    /*报文长度*/
    msg.Len = 8;

    /*241索引信息组成请求修改报文*/
    msg.Data[0] = 0xc2;
	msg.Data[1] = 0x00;
	msg.Data[2] = Fan1Duty;
	msg.Data[3] = Fan2Duty;
	msg.Data[4] = 0;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = 0;

	(void)BSPCANSendQin(eCAN0, &msg);
	
	//by lxr01202 检测PWM从0变为非0时设置启动检测标志
    if (sLastPduFanDuty == 0 && PduFanDuty != 0)
    {
        sPduFanStartDetected = TRUE;  // 设置风扇启动检测标志
    }
    
    sLastPduFanDuty = PduFanDuty;  // 更新历史值

	if (sLastPcsFanDuty == 0 && PcsFanDuty != 0)
    { 
        sPcsFanStartDetected = TRUE;  // 设置风扇启动检测标志
    }
    
    sLastPcsFanDuty = PcsFanDuty;  // 更新历史值

}
/*=================================================================================================
** @Name      : void SendCanExhaustCtrlMsg241(u8 FanDuty)
** @Input     : FanDuty:风扇占空比
** @Output    : void
** @Function  : 发送排气扇控制信息CAN报文
** @The notes : 主索引241
**===============================================================================================*/
void SendCanExhaustCtrlMsg241(u8 FanDuty)
{
	t_CANMSG msg;
	//参数有效
	if(FanDuty > 100)
	{
		return;
	}
    /*请求修改帧发送221索引*/
    msg.ID = CanBuildRupAuxMsgID(2, 241, IMEU3_PDUOUT_DEVADDR, (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev]);

    /*报文长度*/
    msg.Len = 8;

    /*241索引信息组成请求修改报文*/
    msg.Data[0] = 0xc1;
	msg.Data[1] = 0x00;
	msg.Data[2] = FanDuty;
	msg.Data[3] = 0;
	msg.Data[4] = 0;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = 0;

	(void)BSPCANSendQin(eCAN0, &msg);
}
/*=================================================================================================
** @Name      : void SendCanImeu3IoCtrlMsg242(u8 addr,u8 index, u8 cmd)
** @Input     : addr:IMEU3通信地址 index:索引号 cmd:命令号
** @Output    : void
** @Function  : 发送IMEU3IO控制信息CAN报文
** @The notes : 主索引242
**===============================================================================================*/
void SendCanImeu3IoCtrlMsg242(u8 addr,u8 index, u8 cmd)
{
	t_CANMSG msg;
	//参数有效
	if((addr != IMEU3_PDUIN_DEVADDR) && (addr != IMEU3_PDUOUT_DEVADDR))
	{
		return;
	}
	//参数有效
	if((index > 24) || (index < 1))
	{
		return;
	}
	//参数有效
	if(cmd > 1)
	{
		return;
	}
    /*请求修改帧发送242索引*/
    msg.ID = CanBuildRupAuxMsgID(2, 242, addr, (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev]);

    /*报文长度*/
    msg.Len = 8;

    /*242索引信息组成请求修改报文*/
	//兼容A2425,一组BMU控制一路泄爆阀
	 /*A2415 A2445 A2516 A2415降本项目*/
	if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
		|| (gGHardPara_104[eGHardPara104_ProjectCode] == 5) || (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
		|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
	{
	    /*242索引信息组成请求修改报文*/
	    msg.Data[0] = 0xc1;
	}
	/*A2419 A2425 Y2403项目*/
	else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
			|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
	{
	    if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
	    {
	        /*242索引信息组成请求修改报文*/
	        msg.Data[0] = 0xc2;
		}	
	}	
	msg.Data[1] = index;
	msg.Data[2] = cmd;
	msg.Data[3] = 0;
	msg.Data[4] = 0;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = 0;

	(void)BSPCANSendQin(eCAN0, &msg);
}
/*=================================================================================================
** @Name      : void SendCanImeu3MotorCtrlMsg243(u8 state)
** @Input     : state:泄爆阀状态
** @Output    : void
** @Function  : 发送IMEU3泄爆阀控制信息CAN报文
** @The notes : 主索引243
**===============================================================================================*/
void SendCanImeu3MotorCtrlMsg243(u8 state)
{
	t_CANMSG msg;
	//参数有效
	if(state > 1)
	{
		return;
	}
    /*请求修改帧发送242索引*/
    msg.ID = CanBuildRupAuxMsgID(2, 243, IMEU3_PDUOUT_DEVADDR, (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev]);

    /*报文长度*/
    msg.Len = 8;

    //兼容A2425,一组BMU控制一路泄爆阀
	 /*A2415 A2445 A2516 A2415降本项目*/
	if((gGHardPara_104[eGHardPara104_ProjectCode] == 0) || (gGHardPara_104[eGHardPara104_ProjectCode] == 1)
		|| (gGHardPara_104[eGHardPara104_ProjectCode] == 5) || (gGHardPara_104[eGHardPara104_ProjectCode] == 6)
		|| (gGHardPara_104[eGHardPara104_ProjectCode] == 7))
	{
	    /*242索引信息组成请求修改报文*/
	    msg.Data[0] = 0xc1;
		msg.Data[1] = 0x00;
		msg.Data[2] = state;
		msg.Data[3] = 0;
	}
	/*A2419 A2425 Y2403项目*/
	else if((gGHardPara_104[eGHardPara104_ProjectCode] == 2) || (gGHardPara_104[eGHardPara104_ProjectCode] == 3)
			|| (gGHardPara_104[eGHardPara104_ProjectCode] == 4))
	{
	    if(gGBmuGenPara_102[eBmuGenPara102_BmuDev] == 50)
	    {
	        /*242索引信息组成请求修改报文*/
	        msg.Data[0] = 0xc2;
	    	msg.Data[1] = 0x00;
	    	msg.Data[2] = state;
	    	msg.Data[3] = state;
	    }
	}
	else
	{
		msg.Data[0] = 0xc1;
		msg.Data[1] = 0x00;
    	msg.Data[2] = state;
    	msg.Data[3] = 0;
	}
	msg.Data[4] = 0;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = 0;

	(void)BSPCANSendQin(eCAN0, &msg);
}
/*=================================================================================================
** @Name      : void SendCanImeu3DeParaMsg245(u8 OpenDeHum, u8 CloseDeHum,u8 WorkMode)
** @Input     : state:泄爆阀状态
** @Output    : void
** @Function  : 发送IMEU3除湿机参数设置
** @The notes : 主索引245
**===============================================================================================*/
void SendCanImeu3DeParaMsg245(u8 OpenDeHum, u8 CloseDeHum,u8 WorkMode)
{
	t_CANMSG msg;
	//参数有效
	if((WorkMode != 0) && (WorkMode != 0xff))
	{
		return;
	}
	if((OpenDeHum > 100) || (OpenDeHum < 0))
	{
		return;
	}
	if((CloseDeHum > 100) || (CloseDeHum < 0))
	{
		return;
	}
	if(OpenDeHum <= CloseDeHum)
	{
		return;
	}
    /*请求修改帧发送242索引*/
    msg.ID = CanBuildRupAuxMsgID(2, 245, IMEU3_PDUOUT_DEVADDR, (u8)gGBmuGenPara_102[eBmuGenPara102_BmuDev]);

    /*报文长度*/
    msg.Len = 8;

    /*242索引信息组成请求修改报文*/
    msg.Data[0] = 0xc3;
	msg.Data[1] = 0x00;
	msg.Data[2] = OpenDeHum;
	msg.Data[3] = CloseDeHum;
	msg.Data[4] = WorkMode;
	msg.Data[5] = 0;
	msg.Data[6] = 0;
	msg.Data[7] = 0;

	(void)BSPCANSendQin(eCAN0, &msg);
}
//#endif
