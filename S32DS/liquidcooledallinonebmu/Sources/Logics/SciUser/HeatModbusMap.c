/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : HeatModbusMap.c
** @Founder      : ZSYZ342
** @Date         : 2019.11.1
** @Function     : 热管理Modbus通信地址映射模块
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPUserAPI.h"
#include "GroupPara.h"
#include "PublicInfo.h"
#include "GroupInfo.h"
#include "DiagCheck.h"
#include "GroupTactic.h"
#include "HeatModbusMap.h"

/*=================================================================================================
** @Global variable definition
**===============================================================================================*/

extern u16 gEu3AirInfo_202[];       /*EU3空调控制信息*/

extern u16 gEu3AirState_203[];       /*EU3空调状态信息*/

extern u16 gLiqudCoolAirInfo_206[]; /*液冷空调外机控制信息*/

extern u16 gLiqudCoolAirSta_207[];  /*液冷空调状态信息*/

extern u16 gLiqudCoolAirErr_208[];  /*液冷空调故障信息*/

extern u16 gTFAirUserParaInfo_212[]; /*同飞空调用户参数信息*/

extern u16 gTFAirStateInfo_213[]; 	 /*同飞空调状态信息*/

extern u16 gTFAirErrInfo_214[]; 	 /*同飞空调故障信息*/

extern u16 XinMiaoAirSta_215[];      /*新淼空调状态信息*/

extern u16 XinMiaoAirPara_216[];     /*新淼空调配置信息*/

extern u16 gLeakageInfo_225[];  	 /*漏电流传感器信息*/

static u16 sDefaultValueRO = 0;		/*数据异常默认值*/

extern u8 gGHardPara_104[];        /*主控硬件参数配置信息*/

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : u16 *ModbusFindSlaveInfoMaping(u8 fncCode, u8 sciNum, u16 address, u8 stationID)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找所有信息寄存器所对应的地址指针
** @The notes : 放在Modbus通讯包服务端(主机)接收处理HOOK
**===============================================================================================*/
u16 *ModbusFindSlaveInfoMaping(u8 fncCode, u8 sciNum, u16 address, u8 stationID)
{
    u16* temp = (void *)0;
    //接收液冷空调信息
    if((stationID == MODBUS_SALVE_ADDR_AIR) && (sciNum == eSCI2))
    {
    	//通信检测
    	DiagSciRcvDevMsgAddAPI(AIR_BAS_DEVADDR);
    	/*英飞源自研空调*/
		//#if(BMU_LIQUIDCOOL_AIR_TYPE == 0)
    	//自研空调
    	if(gGHardPara_104[eGHardPara104_AirType] == 1)
    	{
            //获取接收从机信息寄存器地址
            if(((address >= MB_AIR_WR_BASE_ADDR) && (address < MB_AIR_WR_END_ADDR))
                ||((address >= MB_AIR_RO_BASE_ADDR) && (address < MB_AIR_RO_END_ADDR))
                ||((address >= MB_AIR_RO_BASE1_ADDR) && (address < MB_AIR_RO_END1_ADDR)))
            {
                temp= ModbusFindAirCtrlInfoROAddrMap(fncCode, sciNum, address);
            }
    	}
        /*同飞空调*/
    	else if(gGHardPara_104[eGHardPara104_AirType] == 2)
    	{
            //获取接收从机信息寄存器地址
            if(((address >= MB_TF_AIR_WR_BASE_ADDR) && (address < MB_TF_AIR_WR_END_ADDR))
                ||((address >= MB_TF_AIR_RO_BASE_ADDR) && (address < MB_TF_AIR_RO_END_ADDR))
    			 ||((address >= MB_TF_AIR_RO_BASE1_ADDR) && (address < MB_TF_AIR_RO_END1_ADDR))
    			 ||((address >= MB_TF_AIR_RO_BASE2_ADDR) && (address < MB_TF_AIR_RO_END2_ADDR))
    			 ||((address >= MB_TF_AIR_RO_BASE3_ADDR) && (address < MB_TF_AIR_RO_END3_ADDR))
    			 ||((address >= MB_TF_AIR_RO_BASE4_ADDR) && (address < MB_TF_AIR_RO_END4_ADDR))
    			 ||((address >= MB_TF_AIR_RO_BASE5_ADDR) && (address < MB_TF_AIR_RO_END5_ADDR))
                ||((address >= MB_TF_AIR_RO_BASE6_ADDR) && (address < MB_TF_AIR_RO_END6_ADDR)))
            {
                temp= ModbusFindAirCtrlInfoROAddrMap(fncCode, sciNum, address);
            }
    	}
    	/*自研EU3空调*/
    	else if(gGHardPara_104[eGHardPara104_AirType] == 3)
    	{
            //获取接收从机信息寄存器地址
            if(((address >= MB_EU3_AIR_WR_BASE_ADDR) && (address < MB_EU3_AIR_WR_END_ADDR))
                ||((address >= MB_EU3_AIR_WR_BASE1_ADDR) && (address < MB_EU3_AIR_WR_END1_ADDR))
    			 ||((address >= MB_EU3_AIR_RO_BASE_ADDR) && (address < MB_EU3_AIR_RO_END_ADDR))
    			 ||((address >= MB_EU3_AIR_RO_BASE1_ADDR) && (address < MB_EU3_AIR_RO_END1_ADDR))
    			 ||((address >= MB_EU3_AIR_RO_BASE2_ADDR) && (address < MB_EU3_AIR_RO_END2_ADDR))
    			 ||((address >= MB_EU3_AIR_RO_BASE3_ADDR) && (address < MB_EU3_AIR_RO_END3_ADDR))
    			 ||((address >= MB_EU3_AIR_RO_BASE4_ADDR) && (address < MB_EU3_AIR_RO_END4_ADDR))
				 ||((address >= MB_EU3_AIR_RO_END4_ADDR) && (address < MB_EU3_AIR_RO_BASE5_ADDR))
				 ||((address >= MB_EU3_AIR_RO_BASE5_ADDR) && (address < MB_EU3_AIR_RO_END5_ADDR)))
            {
                temp= ModbusFindAirCtrlInfoROAddrMap(fncCode, sciNum, address);
            }
    	}    	/*新淼空调*/
    	else if(gGHardPara_104[eGHardPara104_AirType] == 4)
    	{
            //获取接收从机信息寄存器地址
            if(((address >= MB_XinMiao_AIR_WR_BASE_ADDR) && (address < MB_Xinmiao_AIR_WR_END_ADDR))
                ||((address >= MB_XinMiao_AIR_WR_BASE1_ADDR) && (address < MB_Xinmiao_AIR_WR_END1_ADDR))
    			 ||((address >= MB_XinMiao_AIR_RO_BASE_ADDR) && (address < MB_Xinmiao_AIR_RO_END_ADDR)))
            {
                temp= ModbusFindAirCtrlInfoROAddrMap(fncCode, sciNum, address);
            }
    	}
		//#else

		//#endif
    }
    //接收漏电流传感器信息
    else if((stationID == MODBUS_SALVE_ADDR_LEAKAGE) && (sciNum == eSCI1))
    {
    	DiagSciRcvLeakageMsgAddAPI(LEAKAGE_BAS_DEVADDR);
		//#if(BMU_LEAKAGE_EN == 1)
    	if(gGHardPara_104[eGHardPara104_LeakageType] == 1)
    	{
        	if((address >= MB_LEAKAGE_RO_BASE_ADDR) && (address < MB_LEAKAGE_RO_END_ADDR))
        	{
        		temp = ModbusFindLeakageInfoROAddrMap(fncCode, sciNum, address);
        	}
    	}
		//#endif
    }
    return(temp);
}

/*=================================================================================================
** @Name      : u16* ModbusFindAirCtrlInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找在设备信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindAirCtrlInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
	u8 index = 0;
    u16* temp = (void *)0;

    //只读
    if((3 != fncCode) && (4 != fncCode) && (1 != fncCode))
    {
        temp = (void*)0;
        return(temp);
    }
    /*英飞源自研空调*/
	//#if(BMU_LIQUIDCOOL_AIR_TYPE == 0)
    if(gGHardPara_104[eGHardPara104_AirType] == 1)
    {
        if(fncCode == 3)
        {
        	/*查询外设控制信息*/
            if((address >= MB_AIR_WR_BASE_ADDR) && (address < MB_AIR_WR_END_ADDR))
            {
            	index = address - MB_AIR_WR_BASE_ADDR;
                temp = SciGetAirCtrlInfoRO(index);
            }
        }
        else if(fncCode == 4)
        {
            /*查询液冷空调状态信息*/
             if((address >= MB_AIR_RO_BASE_ADDR) && (address < MB_AIR_RO_END_ADDR))
             {
            	 index = address - MB_AIR_RO_BASE_ADDR;
            	 temp = SciGetAirStaInfoRO(index);
             }
        }
        else
        {
        	/*查询BIT信息*/
        	if((address >= MB_AIR_RO_BASE1_ADDR) && (address < MB_AIR_RO_END1_ADDR))
        	{
        		index = address - MB_AIR_RO_BASE1_ADDR;
        		temp = SciGetAirErrInfoRO(index);
        	}
        }
    }
    else if(gGHardPara_104[eGHardPara104_AirType] == 2)
    {
        if((fncCode == 3) || (fncCode == 4))
        {
        	 /*用户参数信息*/
        	 if((address >= MB_TF_AIR_WR_BASE_ADDR) && (address < MB_TF_AIR_WR_BASE_ADDR + eATFUserInfo212_Len))
        	 {
        		 index = address - MB_TF_AIR_WR_BASE_ADDR;
        		 temp = SciGetTfAirUserInfoWR(index);
        	 }
        	 else if((address >= MB_TF_AIR_RO_BASE_ADDR) && (address < MB_TF_AIR_RO_END_ADDR))
        	 {
        		 index = address - MB_TF_AIR_RO_BASE_ADDR;
        		 temp = SciGetTfAirStateRO(index);
        	 }
        	 else if((address >= MB_TF_AIR_RO_BASE1_ADDR) && (address < MB_TF_AIR_RO_END1_ADDR))
        	 {
        		 index = address - MB_TF_AIR_RO_BASE1_ADDR + eATFUserState213_EvapTemp;
        		 temp = SciGetTfAirStateRO(index);
        	 }
        	 else if((address >= MB_TF_AIR_RO_BASE2_ADDR) && (address < MB_TF_AIR_RO_END2_ADDR))
        	 {
        		 index = address - MB_TF_AIR_RO_BASE2_ADDR + eATFUserState213_OpenExpen;
        		 temp = SciGetTfAirStateRO(index);
        	 }
        	 else if((address >= MB_TF_AIR_RO_BASE3_ADDR) && (address < MB_TF_AIR_RO_END3_ADDR))
        	 {
        		 index = address - MB_TF_AIR_RO_BASE3_ADDR + eATFUserState213_SuppWatPre;
        		 temp = SciGetTfAirStateRO(index);
        	 }
        	 else if((address >= MB_TF_AIR_RO_BASE4_ADDR) && (address < MB_TF_AIR_RO_END4_ADDR))
        	 {
        		 index = address - MB_TF_AIR_RO_BASE4_ADDR + eATFUserState213_ExhPre;
        		 temp = SciGetTfAirStateRO(index);
        	 }
        	 else if((address >= MB_TF_AIR_RO_BASE5_ADDR) && (address < MB_TF_AIR_RO_END5_ADDR))
        	 {
        		 index = address - MB_TF_AIR_RO_BASE5_ADDR;
        		 temp = SciGetTfAirErrInfoRO(index);
        	 }
        	 else if((address >= MB_TF_AIR_RO_BASE6_ADDR) && (address < MB_TF_AIR_RO_END6_ADDR))
        	 {
        		 index = address - MB_TF_AIR_RO_BASE6_ADDR + eATFErrInfo214_Level;
        		 temp = SciGetTfAirErrInfoRO(index);
        	 }
        }
    }
    /*自研EU3版本空调*/
    else if(gGHardPara_104[eGHardPara104_AirType] == 3)
    {
    	//查询设置信息
    	if(fncCode == 3)
    	{
    		if((address >= MB_EU3_AIR_WR_BASE_ADDR) && (address < MB_EU3_AIR_WR_END_ADDR))
    		{
    			index = address - MB_EU3_AIR_WR_BASE_ADDR;
    			temp = SciGetEu3AirCtrlInfoRO(index);
    		}
    		else if((address >= MB_EU3_AIR_WR_BASE1_ADDR) && (address < MB_EU3_AIR_WR_END1_ADDR))
    		{
    			index = address - MB_EU3_AIR_WR_BASE1_ADDR + eEu3AirInfo202_CoolOpenT;
    			temp = SciGetEu3AirCtrlInfoRO(index);
    		}
    	}
    	else if(fncCode == 4)
    	{
    		if((address >= MB_EU3_AIR_RO_BASE_ADDR) && (address < MB_EU3_AIR_RO_END_ADDR))
    		{
    			index = address - MB_EU3_AIR_RO_BASE_ADDR;
    			temp = SciGetEu3AirStaInfoRO(index);			
    		}
    		else if((address >= MB_EU3_AIR_RO_BASE1_ADDR) && (address < MB_EU3_AIR_RO_END1_ADDR))
    		{
    			index = address - MB_EU3_AIR_RO_BASE1_ADDR + eEu3AirState203_ComTarFre;
    			temp = SciGetEu3AirStaInfoRO(index);				
    		}
    		else if((address >= MB_EU3_AIR_RO_BASE2_ADDR) && (address < MB_EU3_AIR_RO_END2_ADDR))
    		{
    			index = address - MB_EU3_AIR_RO_BASE2_ADDR + eEu3AirState203_ErrLevel;
    			temp = SciGetEu3AirStaInfoRO(index);				
    		}
    		else if((address >= MB_EU3_AIR_RO_BASE3_ADDR) && (address < MB_EU3_AIR_RO_END3_ADDR))
    		{
    			index = address - MB_EU3_AIR_RO_BASE3_ADDR + eEu3AirState203_ExhTemp;
    			temp = SciGetEu3AirStaInfoRO(index);				
    		}
    		else if((address >= MB_EU3_AIR_RO_BASE4_ADDR) && (address < MB_EU3_AIR_RO_END4_ADDR))
    		{
    			index = address - MB_EU3_AIR_RO_BASE4_ADDR + eEu3AirState203_HighPre;
    			temp = SciGetEu3AirStaInfoRO(index);				
    		}
    		else if((address >= MB_EU3_AIR_RO_BASE5_ADDR) && (address < MB_EU3_AIR_RO_END5_ADDR))
    		{
    			index = address - MB_EU3_AIR_RO_BASE5_ADDR + eEu3AirState203_SensorErr1;
    			temp = SciGetEu3AirStaInfoRO(index);				
    		}
    		else if((address >= MB_EU3_AIR_RO_BASE6_ADDR) && (address < MB_EU3_AIR_RO_END6_ADDR))
    		{
    			index = address - MB_EU3_AIR_RO_BASE6_ADDR + eEu3AirState203_CellPumpHz;
    			temp = SciGetEu3AirStaInfoRO(index);				
    		}
    	}
    }
    /*新淼空调*/
    else if(gGHardPara_104[eGHardPara104_AirType] == 4)
    {
    	//查询设置参数信息
    	if(fncCode == 3)
    	{
    		if((address >= MB_XinMiao_AIR_WR_BASE_ADDR) && (address < MB_Xinmiao_AIR_WR_END_ADDR))
    		{
    			index = address - MB_XinMiao_AIR_WR_BASE_ADDR;
    			temp = SciGetXinMiaoAirCtrlInfoRO(index);
    		}
    		else if((address >= MB_XinMiao_AIR_WR_BASE1_ADDR) && (address < MB_Xinmiao_AIR_WR_END1_ADDR))
    		{
    			index = address - MB_XinMiao_AIR_WR_BASE1_ADDR + eXinMiaoAirPar216_SetTemp;
    			temp = SciGetXinMiaoAirCtrlInfoRO(index);
    		}
    	}
    	//查询状态信息
    	else if(fncCode == 4)
    	{
    		if((address >= MB_XinMiao_AIR_RO_BASE_ADDR) && (address < MB_Xinmiao_AIR_RO_END_ADDR))
    		{
    			index = address - MB_XinMiao_AIR_RO_BASE_ADDR;
    			temp = SciGetXinMiaoAirStaInfoRO(index);
    		}
    	}
    }
	//#else

	//#endif
    return(temp);
}
//#if(BMU_LIQUIDCOOL_AIR_TYPE == 0)
/*=================================================================================================
** @Name      : u16* SciGetXinMiaoAirStaInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetXinMiaoAirStaInfoRO(u8 index)
{
	if(index < eXinMiaoAirSta215_Len)
	{
		return(&XinMiaoAirSta_215[index]);
	}

	return(&sDefaultValueRO);
}
/*=================================================================================================
** @Name      : u16* SciGetXinMiaoAirCtrlInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetXinMiaoAirCtrlInfoRO(u8 index)
{
	if(index < eXinMiaoAirPar216_Len)
	{
		return(&XinMiaoAirPara_216[index]);
	}

	return(&sDefaultValueRO);
}
/*=================================================================================================
** @Name      : u16* SciGetEu3AirCtrlInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetEu3AirCtrlInfoRO(u8 index)
{
	if(index < eEu3AirInfo202_Len)
	{
		return(&gEu3AirInfo_202[index]);
	}

	return(&sDefaultValueRO);
}
/*=================================================================================================
** @Name      : u16* SciGetEu3AirStaInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetEu3AirStaInfoRO(u8 index)
{
	if(index < eEu3AirState203_Len)
	{
		return(&gEu3AirState_203[index]);
	}

	return(&sDefaultValueRO);
}
/*=================================================================================================
** @Name      : u16* SciGetAirCtrlInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetAirCtrlInfoRO(u8 index)
{
	if(index < eLiquidAirInfo206_Len)
	{
		return(&gLiqudCoolAirInfo_206[index]);
	}

	return(&sDefaultValueRO);
}
/*=================================================================================================
** @Name      : u16* SciGetAirStaInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetAirStaInfoRO(u8 index)
{
	if(index < eLiquidAirSta207_Len)
	{
		return(&gLiqudCoolAirSta_207[index]);
	}

	return(&sDefaultValueRO);
}
/*=================================================================================================
** @Name      : u16* SciGetAirErrInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetAirErrInfoRO(u8 index)
{
	if(index < GINDEX208_LEN)
	{
		return(&gLiqudCoolAirErr_208[index]);
	}
	return(&sDefaultValueRO);
}
//#else
/*=================================================================================================
** @Name      : u16* SciGetTfAirUserInfoWR(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetTfAirUserInfoWR(u8 index)
{
	if(index < GINDEX212_LEN)
	{
		return(&gTFAirUserParaInfo_212[index]);
	}
	return(&sDefaultValueRO);
}
/*=================================================================================================
** @Name      : u16* SciGetTfAirStateRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetTfAirStateRO(u8 index)
{
	if(index < GINDEX213_LEN)
	{
		return(&gTFAirStateInfo_213[index]);
	}
	return(&sDefaultValueRO);
}
/*=================================================================================================
** @Name      : u16* SciGetTfAirErrInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetTfAirErrInfoRO(u8 index)
{
	if(index < GINDEX214_LEN)
	{
		return(&gTFAirErrInfo_214[index]);
	}
	return(&sDefaultValueRO);
}
//#endif
//#if(BMU_LEAKAGE_EN == 1)
/*=================================================================================================
** @Name      : u16* ModbusFindLeakageInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
** @Input     : fncCode:功能码 sciNum:串口号 address:信息地址
** @Output    : 映射到信息寄存器的实际地址指针
** @Function  : 查找在设备信息寄存器所对应的地址指针
** @The notes : 读
**===============================================================================================*/
u16* ModbusFindLeakageInfoROAddrMap(u8 fncCode, u8 sciNum, u16 address)
{
	u8 index = 0;
    u16* temp = (void *)0;

    //只读
    if(3 != fncCode)
    {
        temp = (void*)0;
        return(temp);
    }
    //查询漏电流传感器信息
    if((address >= MB_LEAKAGE_RO_BASE_ADDR) && (address < MB_LEAKAGE_RO_END_ADDR))
    {
    	 index = address - MB_LEAKAGE_RO_BASE_ADDR;
    	 temp = SciGetLeakageInfoRO(index);
    }
    return(temp);
}
/*=================================================================================================
** @Name      : u16* SciGetLeakageInfoRO(u8 index)
** @Input     : index:索引号
** @Output    : 读回检信息寄存器地址指针
** @Function  : 获取读回检信息寄存器地址指针
** @The notes : 只读,取寄存器地址
**===============================================================================================*/
u16* SciGetLeakageInfoRO(u8 index)
{
	if(index < GINDEX225_LEN)
	{
		return(&gLeakageInfo_225[index]);
	}
	return(&sDefaultValueRO);
}
//#endif
/*=================================================================================================
** @Static function definition
**===============================================================================================*/
