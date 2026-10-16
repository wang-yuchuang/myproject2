/**
  ******************************************************************************
  * @file    BF8915A.c
  * @version V1.1.4
  * @date    2021-12-20
  * @brief   This file provides the BF8915A functions(璇ユ枃浠舵彁渚汢F8915A鍔熻兘).
 */
#include "BSPTypeDef.h"
#include "BSPSPI.h"
#include "BF8915A.h"

//=============================================================================================
//鍑芥暟鍚嶇О	: static void BF8915ACmdWrite(void)
//杈撳叆鍙傛暟	: void
//杈撳嚭鍙傛暟	: void
//闈欐�佸彉閲�	: void
//鍔�		鑳�	: SPI鍐欏叆6813鎺у埗鍛戒护鎴栬�匬EC
//娉�		鎰�	:
//=============================================================================================
static void BF8915ACmdWrite(u8* commanddata);

/*!
    \brief      SPI1_ReadWriteByte(SPI1璇诲啓涓�涓瓧鑺�)
    \param[in]  SPI_TxData     :SPI闇�瑕佸彂閫佺殑鏁版嵁
    \retval     SPI_ReceiveData:SPI鎺ユ敹鍒扮殑鏁版嵁
*/
u8 SPI1_ReadWriteByte(u8 TxData) //TxData 鍙互鏄�8浣嶆垨16浣嶇殑锛屽湪鍚敤SPI涔嬪墠灏辩‘瀹氬ソ鏁版嵁甯ф牸寮�
{
	/*
    u8 retry = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)//妫�鏌ユ槸鍚﹁缃簡鎸囧畾鐨凷PI鏍囧織銆備紶杈撶紦鍐插尯绌烘爣蹇� //0:鍙戦�佺紦鍐查潪绌�  绛夊緟鍙戦�佺紦鍐插櫒鍙樼┖
    {
			retry++;
			if(retry>200)return 0;
    }
    SPI_I2S_SendData(SPI1, TxData);//閫氳繃SPI1鍙戦�佹暟鎹��
    retry=0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//绛夊緟鎺ユ敹鏁版嵁瀹屾垚,妫�鏌ユ寚瀹氱殑SPI鏍囧織浣嶈缃笌鍚�:鎺ユ敹缂撳瓨闈炵┖鏍囧織浣�
    {
			retry++;
			if(retry>200)return 0;
    }
    return SPI_I2S_ReceiveData(SPI1); //杩斿洖鏈�杩戞帴鏀剁殑鏁版嵁锛孲PI_DR瀵勫瓨鍣ㄩ噷闈㈢殑
 */
	static u8 errNum = 0;
	u8 rcvdata = 0;

	 //SPI鍙戦�佷俊鎭�
   (void)BSPSPISendData(eSPI2, TxData);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return 0xFF;
		}
    }
    rcvdata = BSPSPIGetRcvData(eSPI2);
    errNum = 0;

    return(rcvdata);
}

/*!
    \brief      //SPI2璇诲啓瀛楄妭锛屾涓篠TM32鐨凷PI浠ｇ爜浣滀负鍙傝�冿紝瀹㈡埛闇�鏍规嵁鑷繁鐨勪富鎺CU鑷淇敼
    \param[in]  SPI_TxData     :SPI2闇�瑕佸彂閫佺殑鏁版嵁
    \retval     SPI_ReceiveData:SPI2鎺ユ敹鍒扮殑鏁版嵁
*/
u8 SPI2_ReadWriteByte(u8 TxData)
{
	/*
	u8 retry=0;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)//妫�鏌ユ寚瀹氱殑SPI鏍囧織浣嶈缃笌鍚�:鍙戦�佺紦瀛樼┖鏍囧織浣�
		{
		retry++;
		if(retry>200)return 0;
		}
	SPI_I2S_SendData(SPI2, TxData);//閫氳繃澶栬SPIx鍙戦�佷竴涓暟鎹�
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)//妫�鏌ユ寚瀹氱殑SPI鏍囧織浣嶈缃笌鍚�:鎺ュ彈缂撳瓨闈炵┖鏍囧織浣�
		{
		retry++;
		if(retry>200)return 0;
		}
	return SPI_I2S_ReceiveData(SPI2);//杩斿洖閫氳繃SPIx鏈�杩戞帴鏀剁殑鏁版嵁
	 */
	static u8 errNum = 0;
	u8 rcvdata = 0;

	 //SPI鍙戦�佷俊鎭�
   (void)BSPSPISendData(eSPI2, TxData);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return 0xFF;
		}
    }
    rcvdata = BSPSPIGetRcvData(eSPI2);
    errNum = 0;

    return(rcvdata);
}

BOOLEAN	isospi_reverse = FALSE;//鏀瑰彉璇ュ彉閲忓彲浠ュ疄鐜拌強鑺遍摼SPI姝ｅ弽鍚戦�氫俊銆�
                                //FALSE:鐩稿鐨勬鍚戦�氫俊锛屾湰SDK涓殑SPI1涓虹浉瀵规鍚戦�氫俊锛孲PI1鑿婅姳閾捐繛鎺ョ殑绗竴棰桰C鐪嬩綔鏄疘C0;
                                //TURE: 鐩稿鐨勫弽鍚戦�氫俊锛屾湰SDK涓殑SPI2涓虹浉瀵瑰弽鍚戦�氫俊锛屼絾涔熸槸SPI1鑿婅姳閾捐繛鎺ョ殑绗竴棰桰C鐪嬩綔鏄疘C0銆�
const u16 Bf8915crc15Table[256] =
{
	0x0000, 0xc599, 0xceab, 0x0b32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,	0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa,
	0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,	0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d,
	0x5b2e,	0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,	0x77e6, 0xb27f, 0xb94d, 0x7cd4,
	0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,	0x2544, 0x02be, 0xc727, 0xcc15, 0x098c, 0xda71, 0x1fe8, 0x14da, 0xd143,
	0xf3c5, 0x365c,	0x3d6e, 0xf8f7, 0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x07c2, 0xc25b, 0xc969, 0x0cf0, 0xdf0d,	0x1a94, 0x11a6, 0xd43f,
	0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,	0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8,
	0xa8eb, 0x6d72, 0x6640,	0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba, 0x4a88, 0x8f11,
	0x057c, 0xc0e5, 0xcbd7, 0x0e4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,	0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286,
	0xa213, 0x678a, 0x6cb8, 0xa921,	0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070, 0x85e9,
	0x0f84, 0xca1d, 0xc12f, 0x04b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528, 0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e,
	0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,	0x2ac0, 0x0d3a, 0xc8a3, 0xc391, 0x0608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7,
	0x54aa, 0x9133, 0x9a01,	0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9, 0x7350,
	0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,	0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c,
	0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,	0x2fbc, 0x0846, 0xcddf, 0xc6ed, 0x0374, 0xd089, 0x1510, 0x1e22, 0xdbbb,
	0x0af8, 0xcf61, 0xc453,	0x01ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,	0x2d02,
	0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,	0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
};//precomputed CRC15 Table(棰勮绠�15浣嶅惊鐜啑浣欐牎楠屽�肩殑琛�)

/*!
    \brief      Wake isoSPI up from idle state(灏嗚強鑺遍摼SPI浠庣┖闂茬姸鎬佸敜閱�)
    \param[in]  total_ic: number of ICs in the daisy chain(鑿婅姳閾句腑IC(8915A)鐨勬暟閲�)
    \retval     none
*/
void wakeup_idle(u8 total_ic)
{
	u8 i =0;

	for (i = 0; i < total_ic; i++)
	{
		if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			BF8915_CS_LOW();		//灏嗙浉瀵规鍚戦�氫俊鐨勭墖閫塁S璁剧疆涓轰綆鐢靛钩
			SPI1_ReadWriteByte(0xff);//Guarantees the isoSPI will be in ready mode(淇濊瘉鑿婅姳閾維PI灏嗗浜庡氨缁ā寮�)
			BF8915_CS_HIGH();		//灏嗙浉瀵规鍚戦�氫俊鐨勭墖閫塁S璁剧疆涓洪珮鐢靛钩
		}
		else
		{
			BF8915_CS_LOW_Reverse();//灏嗙浉瀵瑰弽鍚戦�氫俊鐨勭墖閫塁S璁剧疆涓轰綆鐢靛钩
			SPI2_ReadWriteByte(0xff);//Guarantees the isoSPI will be in ready mode(淇濊瘉鑿婅姳閾維PI灏嗗浜庡氨缁ā寮�)
			BF8915_CS_HIGH_Reverse();//灏嗙浉瀵瑰弽鍚戦�氫俊鐨勭墖閫塁S璁剧疆涓洪珮鐢靛钩
		}
	}
}

/*!
    \brief      Generic wakeup command to wake the BF8915A from sleep(灏咮F8915A浠庣潯鐪犱腑鍞ら啋鐨勯�氱敤鍞ら啋鍛戒护)
    \param[in]  total_ic: the number of ic(涓叉帴鐨処C(8915A)鏁伴噺)
    \retval     none
*/
void wakeup_sleep(u8 total_ic)
{
	u8 i =0;
	for (i = 0; i < total_ic; i++)	//姝ゅ惊鐜綔鐢ㄤ负:璁╂墍鏈塈C(8915A)鑺墖澶勪簬寰呮満鐘舵��
	{
		if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			BF8915_CS_LOW();		//灏嗙浉瀵规鍚戦�氫俊鐨勭墖閫塁S璁剧疆涓轰綆鐢靛钩
			delay_us(300); 			// Guarantees the BF8915A will be in standby(淇濊瘉BF8915A澶勪簬寰呮満鐘舵��),璇ュ欢杩熷簲鍦�200~300us涔嬮棿
			BF8915_CS_HIGH();		//灏嗙浉瀵规鍚戦�氫俊鐨勭墖閫塁S璁剧疆涓洪珮鐢靛钩
		}
		else
		{
			BF8915_CS_LOW_Reverse();//灏嗙浉瀵瑰弽鍚戦�氫俊鐨勭墖閫塁S璁剧疆涓轰綆鐢靛钩
			delay_us(300); 			// Guarantees the BF8915A will be in standby(淇濊瘉BF8915A澶勪簬寰呮満鐘舵��),璇ュ欢杩熷簲鍦�200~300us涔嬮棿
			BF8915_CS_HIGH_Reverse();//灏嗙浉瀵瑰弽鍚戦�氫俊鐨勭墖閫塁S璁剧疆涓洪珮鐢靛钩
		}
	}
}

/*!
    \brief      Writes an array of bytes out of the SPI port(灏嗗瓧鑺傛暟缁勫啓鍏PI绔彛)
    \param[in]  len      :Number of bytes to be written on the SPI port(SPI绔彛涓婅鍐欏叆鐨勫瓧鑺傛暟)
		\param[in]  writeData:Array of bytes to be written on the SPI port(瑕佸啓鍏PI绔彛鐨勫瓧鑺傛暟缁�)
    \retval     none
*/
void spi_write_array(u8 len, u8 writeData[])
{
	u8 i = 0;
	for (i = 0; i < len; i++)		//姝ゅ惊鐜皢writeData[] 鏁扮粍鎵�瀛樼殑鏁版嵁涓�涓�鍙戦�佸嚭鍘�
	{
		if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			SPI1_ReadWriteByte((u8)writeData[i]);
		}
		else
		{
			SPI2_ReadWriteByte((u8)writeData[i]);
		}
	}
}

/*!
    \brief      Writes and read a set number of bytes using the SPI port(浣跨敤SPI绔彛鍐欏叆鍜岃鍙栬瀹氭暟閲忕殑瀛楄妭)
		\param[in]  tx_Data: array of data to be written on SPI port(瑕佸啓鍏PI绔彛鐨勬暟鎹暟缁�)
    \param[in]  tx_len : length of the tx_data arr(tx_data鏁扮粍鐨勯暱搴�)
    \param[in]  rx_data: Input: array that will store the data read by the SPI port(杈撳叆:瀛樺偍SPI绔彛璇诲彇鐨勬暟鎹殑鏁扮粍)
		\param[in]  rx_len : Option: number of bytes to be read from the SPI port(閫夐」:浠嶴PI绔彛瑕佽鍙栫殑瀛楄妭鏁�)
    \retval     none
*/
void spi_write_read(u8 tx_Data[], u8 tx_len, u8 *rx_data, u8 rx_len)
{
	u8 i = 0;
	for (i = 0; i < tx_len; i++)	//姝ゅ惊鐜皢tx_Data[] 鏁扮粍鎵�瀛樼殑鏁版嵁涓�涓�鍙戦�佸嚭鍘�
	{
		if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			SPI1_ReadWriteByte(tx_Data[i]);
		}
		else
		{
			SPI2_ReadWriteByte(tx_Data[i]);
		}
	}

	for (i = 0; i < rx_len; i++)//姝ゅ惊鐜皢璇诲埌鐨勬暟鎹竴涓�瀛樺叆rx_data[] 鏁扮粍涓�
	{
		if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			rx_data[i] = (u8)SPI1_ReadWriteByte(0xFF);
		}
		else
		{
			rx_data[i] = (u8)SPI2_ReadWriteByte(0xFF);
		}
	}
}

//=============================================================================================
//鍑芥暟鍚嶇О	: static void BF8915ACmdWrite(void)
//杈撳叆鍙傛暟	: void
//杈撳嚭鍙傛暟	: void
//闈欐�佸彉閲�	: void
//鍔�		鑳�	: SPI鍐欏叆bf8915A鎺у埗鍛戒护鎴栬�匬EC
//娉�		鎰�	:
//=============================================================================================
static void BF8915ACmdWrite(u8* commanddata)
{
	static u8 errNum = 0;
	//u8 rcvdata[4] = {0};

	 //SPI鍙戦�佷俊鎭�
   (void)BSPSPISendData(eSPI2, commanddata[0]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2, commanddata[1]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2, commanddata[2]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    BSPSPIGetRcvData(eSPI2);
    errNum = 0;
    (void)BSPSPISendData(eSPI2, commanddata[3]);
    while(BSPSPIGetWorkState(eSPI2))
    {
		errNum++;
		if(errNum > 10)
		{
			return;
		}
    }
    BSPSPIGetRcvData(eSPI2);
    errNum = 0;
}

/*!
    \brief      Generic function to write BF8915A commands. Function calculated PEC(The Packet Error Code) for tx_cmd data(缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟锟�:顕款惙鐚紉_cmd鏁版嵁璁＄畻浜嗘閿欑爜)
		\param[in]  tx_cmd: 2 Byte array containing the BMS command to be sent(鍖呭惈瑕佸彂閫佺殑BMS(鐢垫睜绠＄悊绯荤粺)鍛戒护鐨�2瀛楄妭鏁扮粍)
    \retval     none
*/
void BF8915A_cmd(u8 tx_cmd[2])
{
	u8 cmd[4];					//闇�瑕佸彂閫佺殑4瀛楄妭鏁扮粍
	u16 cmd_pec;				//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[0]  = tx_cmd[0];		//鍙戦�佹寚浠ょ殑楂�8浣�
	cmd[1]  = tx_cmd[1];		//鍙戦�佹寚浠ょ殑浣�8浣�
	cmd_pec = Bf8915pec15_calc(2, cmd);//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[2]  = (u8)(cmd_pec >> 8);//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勯珮鍏綅
	cmd[3]  = (u8)(cmd_pec);	//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勪綆鍏綅

	if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
    {
	    BF8915_CS_LOW();		//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
	    //spi_write_array(4,cmd);//灏�4瀛楄妭鏁扮粍鍐欏叆SPI绔彛
	    BF8915ACmdWrite(cmd);
	    BF8915_CS_HIGH();		//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
    }
    else
    {
	    BF8915_CS_LOW_Reverse();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
	    //spi_write_array(4,cmd);//灏�4瀛楄妭鏁扮粍鍐欏叆SPI绔彛
	    BF8915ACmdWrite(cmd);
	    BF8915_CS_HIGH_Reverse();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
    }
}

/*!
    \brief      Generic function to write BF8915A commands and write payload data. Function calculated PEC for tx_cmd data(鍐橞F8915A鍛戒护鍜屽啓鏈夋晥杞借嵎鏁版嵁鐨勯�氱敤鍑芥暟銆倀x_cmd鏁版嵁鐨勫嚱鏁拌绠楁閿欑爜)
    \param[in]  total_ic  : the number of ic(IC(8915A)鐨勬暟閲�)
    \param[in]  tx_cmd    : 2 Byte array containing the BMS command to be sent(鍖呭惈瑕佸彂閫佺殑BMS(鐢垫睜绠＄悊绯荤粺)鍛戒护鐨�2瀛楄妭鏁扮粍)
		\param[in]  write_data: Array containing the data to be written to the BMS ICs(鍖呭惈瑕佸啓鍏MS(鐢垫睜绠＄悊绯荤粺)IC(8915A)鐨勬暟鎹暟缁�)
    \retval     none
*/
void BF8915A_write(u8 total_ic, u8 tx_cmd[2], u8 write_data[])
{
	const u8 BYTES_IN_REG = 6;//瀵勫瓨鍣ㄧ粍涓殑6涓�8浣嶅瘎瀛樺櫒
	const u8 CMD_LEN = 4+(8*total_ic);//4:cmd[0]~cmd[3];(8*total_ic):瀵勫瓨鍣ㄧ粍涓殑6涓�8浣嶅瘎瀛樺櫒 + 2涓�8浣峆EC(妫�閿欑爜)
	u8 *cmd;				//闇�瑕佸彂閫佺殑cmd鏁扮粍
	u16 data_pec;			//璁＄畻閰嶇疆瀵勫瓨鍣ㄦ暟鎹殑PEC(妫�閿欑爜)
	u16 cmd_pec;			//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	u8 cmd_index;			//鐢ㄤ簬鍐欐寚浠や腑 瀵勫瓨鍣ㄧ粍涓殑鏁版嵁
	u8 current_ic = total_ic;// current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 current_byte = 0;	//褰撳墠闇�鍐欏叆鐨勫瘎瀛樺櫒缁� 鐨勫瓧鑺傛暟
	cmd = (u8 *)malloc(CMD_LEN*sizeof(u8));//璁＄畻cmd鏁扮粍鍙戦�佹暟鎹墍闇�鐨勬暟缁勯暱搴�
	cmd[0]  = tx_cmd[0];	//鍙戦�佹寚浠ょ殑楂�8浣�
	cmd[1]  = tx_cmd[1];	//鍙戦�佹寚浠ょ殑浣�8浣�
	cmd_pec = Bf8915pec15_calc(2, cmd);//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[2]  = (u8)(cmd_pec >> 8);//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勯珮鍏綅
	cmd[3]  = (u8)(cmd_pec);//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勪綆鍏綅
	cmd_index = 4;			//浠�4寮�濮嬪啓鍏ュ瘎瀛樺櫒缁勪腑鐨勬暟鎹�

	for (current_ic = total_ic; current_ic > 0; current_ic--)       // executes for each BF8915A, this loops starts with the last IC on the stack(瀵规瘡涓狟F8915A鎵ц锛岃繖涓惊鐜粠鍫嗘爤涓婄殑鏈�鍚庝竴涓�8915A寮�濮�)
	{	                                                              //The first configuration written is received by the last IC in the daisy chain(鑿婅姳閾句腑鐨勬渶鍚庝竴涓狪C(8915A)鎺ユ敹鍐欏叆鐨勭涓�涓厤缃�)

		for (current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)//璇ュ惊鐜皢瀵勫瓨鍣ㄧ粍涓殑6涓�8浣嶅瘎瀛樺櫒鐨勬暟鎹竴涓�鍐欏叆鏁扮粍cmd[]涓�
		{
			cmd[cmd_index] = write_data[((current_ic-1)*BYTES_IN_REG)+current_byte];
			cmd_index = cmd_index + 1;
		}
		data_pec = (u16)Bf8915pec15_calc(BYTES_IN_REG, &write_data[(current_ic-1)*BYTES_IN_REG]);// calculating the PEC for each configuration register data(璁＄畻姣忎釜閰嶇疆瀵勫瓨鍣ㄦ暟鎹殑PEC(妫�閿欑爜))
		cmd[cmd_index] = (u8)(data_pec >> 8);	//閰嶇疆瀵勫瓨鍣ㄦ暟鎹殑PEC(妫�閿欑爜)鐨勯珮鍏綅
		cmd[cmd_index + 1] = (u8)data_pec;		//閰嶇疆瀵勫瓨鍣ㄦ暟鎹殑PEC(妫�閿欑爜)鐨勪綆鍏綅
		cmd_index = cmd_index + 2;				//鐢变簬鏁扮粍姣忎釜瀵勫瓨鍣ㄧ粍鍐欏叆鍚庯紝姣忎釜IC鐨勫熬绔渶鍔�2瀛楄妭鐨凱EC(妫�閿欑爜)锛屽洜姝md_index闇�姣忔閮借嚜澧�2
	}
	if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
	{
		BF8915_CS_LOW();		//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_array(CMD_LEN, cmd);//灏嗗瓧鑺傛暟缁勫啓鍏PI绔彛
		BF8915_CS_HIGH();		//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
	}
	else
	{
		BF8915_CS_LOW_Reverse();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_array(CMD_LEN, cmd);//灏嗗瓧鑺傛暟缁勫啓鍏PI绔彛
		BF8915_CS_HIGH_Reverse();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
	}
	free(cmd);//娓呯┖cmd鏁扮粍
}

/*!
    \brief      Generic function to write BF8915A commands and read data(鍐橞F8915A鍛戒护鍜岃鏁版嵁鐨勯�氱敤鍑芥暟). Function calculated PEC for tx_cmd data(鍙戦�佹寚浠ゆ暟鎹殑鍑芥暟璁＄畻PEC(妫�閿欑爜))
                Issues a command onto the daisy chain and reads back 6*total_ic data in the rx_data array(鍚戣強鑺遍摼鍙戝嚭鍛戒护锛屽洖璇籸x_data鏁扮粍涓殑6鍊岻C(8915A)鏁版嵁)
    \param[in]  total_ic : the number of ic(IC(8915A)鐨勬暟閲�)
    \param[in]  tx_cmd   : 2 Byte array containing the BMS command to be sent(鍖呭惈瑕佸彂閫佺殑BMS(鐢垫睜绠＄悊绯荤粺)鍛戒护鐨�2瀛楄妭鏁扮粍)
		\param[in]  rx_data  : Array that the read back data will be stored(瀛樺偍鍥炶鏁版嵁鐨勬暟缁�)
    \retval     pec_error: 0: Data read back has matching PEC;1: Data read back has incorrect PEC(0:鏁版嵁璇诲洖鍏锋湁鍖归厤鐨勬閿欑爜 1:鏁版嵁璇诲洖鏈変笉姝ｇ‘鐨勬閿欑爜)
*/
u8 BF8915A_read(u8 total_ic, u8 tx_cmd[2], u8 *rx_data)
{
	const u8 BYTES_IN_REG = 8;	//瀵勫瓨鍣ㄧ粍涓殑6涓�8浣嶅瘎瀛樺櫒 + 2涓�8浣峆EC(妫�閿欑爜)
	u8 cmd[4];					//闇�瑕佸彂閫佺殑cmd鏁扮粍
	u8 rxDataTemp[256];			//瀛樺偍SPI绔彛璇诲彇鐨勬暟鎹� 鐨勬暟缁�
	u8 pec_error = 0;			//妫�鏌ユ閿欑爜
	u16 cmd_pec;				//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	u16 data_pec;				//璁＄畻閰嶇疆瀵勫瓨鍣ㄦ暟鎹殑PEC(妫�閿欑爜)
	u16 received_pec;			//瀛樺叆鎺ユ敹鍒扮殑PEC(妫�閿欑爜)
	u8 current_ic = 0;			//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 current_byte = 0;		//褰撳墠闇�璇诲叆鐨勫瘎瀛樺櫒缁�
	cmd[0]  = tx_cmd[0];		//鍙戦�佹寚浠ょ殑楂�8浣�
	cmd[1]  = tx_cmd[1];		//鍙戦�佹寚浠ょ殑浣�8浣�
	cmd_pec = Bf8915pec15_calc(2, cmd);//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[2]  = (u8)(cmd_pec >> 8);//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勯珮鍏綅
	cmd[3]  = (u8)(cmd_pec);	//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勪綆鍏綅
	
	if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
	{
		BF8915_CS_LOW();		//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_read(cmd, 4, rxDataTemp, (total_ic*BYTES_IN_REG));//Read the configuration data of all ICs on the daisy chain into rx_data[] array
		BF8915_CS_HIGH();		//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩                           //(灏嗚強鑺遍摼涓婃墍鏈塈C(8915A)鐨勯厤缃暟鎹鍏x_data[]鏁扮粍)
	}
	else
	{
		BF8915_CS_LOW_Reverse();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_read(cmd, 4, rxDataTemp, (total_ic*BYTES_IN_REG));//Read the configuration data of all ICs on the daisy chain into rx_data[] array
		BF8915_CS_HIGH_Reverse();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩                           //(灏嗚強鑺遍摼涓婃墍鏈塈C(8915A)鐨勯厤缃暟鎹鍏x_data[]鏁扮粍)
	}

	for (current_ic = 0; current_ic < total_ic; current_ic++) //executes for each BF8915A in the daisy chain and packs the data(瀵硅強鑺遍摼涓殑姣忎釜BF8915A鎵ц骞舵墦鍖呮暟鎹�)
																	                          //into the r_comm array as well as check the received data for any bit errors(骞舵鏌ユ帴鏀跺埌鐨勬暟鎹槸鍚︽湁浠讳綍浣嶉敊璇�)
	{
		for (current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)//璇ュ惊鐜皢璇诲洖鐨勫瘎瀛樺櫒缁�(6涓�8浣嶅瘎瀛樺櫒)鏁版嵁涓�涓�鍐欏叆鏁扮粍rx_data[]涓�
		{
			rx_data[(current_ic*BYTES_IN_REG)+current_byte] = rxDataTemp[(current_ic*BYTES_IN_REG)+current_byte];
		}
		received_pec = (rx_data[(current_ic*BYTES_IN_REG)+6]<<8) + rx_data[(current_ic*BYTES_IN_REG)+7];//灏唕x_data[]涓帴鏀跺埌鐨凱EC(妫�閿欑爜)鎸夐珮浣庡叡16浣嶅瓨鍏eceived_pec涓�
		data_pec = Bf8915pec15_calc(6, &rx_data[current_ic*BYTES_IN_REG]);//璁＄畻鎺ユ敹鍥炴潵鐨勫瘎瀛樺櫒缁�(6涓�8浣嶅瘎瀛樺櫒)涓殑PEC(妫�閿欑爜)
		if (received_pec != data_pec)//妫�鏌ユ閿欑爜鏄惁鍖归厤
		{
			pec_error = 1;//妫�閿欑爜涓嶆纭�
		}
	}
	return(pec_error);
}

/*!
    \brief      Calculates and returns the CRC15(璁＄畻骞惰繑鍥�15浣嶅惊鐜啑浣欐牎楠屽��)
		\param[in]  len    : Number of bytes that will be used to calculate a PEC(灏嗙敤浜庤绠桺EC(妫�閿欑爜)鐨勫瓧鑺傛暟)
		\param[in]  pecData: Array of data that will be used to calculate a PEC(灏嗙敤浜庤绠桺EC(妫�閿欑爜)鐨勬暟鎹暟缁�)
    \retval     The calculated pec15 as an unsigned int(璁＄畻鍑烘棤绗﹀彿鏁存暟鐨�15浣嶆閿欑爜)
*/
u16 Bf8915pec15_calc(u8 len, u8 *pecData)
{
	u16 remainder,addr;
	u8 i = 0;
	remainder = 16;		//initialize the PEC(鍒濆鍖栨閿欑爜)
	for (i = 0; i<len; i++) // loops for each byte in data array(鏁版嵁鏁扮粍涓瘡涓瓧鑺傜殑寰幆)
	{
		addr = ((remainder>>7)^pecData[i])&0xff;//calculate PEC table address(璁＄畻妫�閿欑爜鐨勮〃鍦板潃)
		remainder = (remainder<<8)^Bf8915crc15Table[addr];
	}
	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2(15浣嶅惊鐜啑浣欐牎楠屽�肩殑鏈�浣庢湁鏁堜綅涓�0锛屽洜姝や綑鏁板繀椤讳箻浠�2)
}

/*!
    \brief      Helper function to set GPIO bits(璁剧疆GPIO浣嶇殑杈呭姪鍑芥暟)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configAReg: Register group A(瀵勫瓨鍣ㄧ粍A)
    \param[in]  gpio_pd_en: 0: GPIO pin pull-down enable; 1: GPIO pin pull-down disable (default)(0:GPIO寮曡剼涓嬫媺浣胯兘锛�1:GPIO寮曡剼涓嬫媺绂佺敤(榛樿))
    \retval     none
*/
void BF8915A_set_cfga_gpio(u8 nIC, Reg_TypeDef *configAReg, u8 gpio_pd_en)
{
	configAReg[nIC].tx_data[0] = gpio_pd_en;//璁剧疆CFGAR0,0:GPIO寮曡剼涓嬫媺浣胯兘锛�1:GPIO寮曡剼涓嬫媺绂佺敤
}

/*!
    \brief      Helper function to set GPIO IO_MODE(璁剧疆GPIO鐨処O鍙ｆā寮忕殑杈呭姪鍑芥暟)
		\param[in]  nIC           : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configAReg    : Register group A(瀵勫瓨鍣ㄧ粍A)
		\param[in]  gpio_analog_en: 0: analog input;1: Ordinary GPIO(0:妯℃嫙杈撳叆锛�1:鏅�欸PIO)
    \retval     none
*/
void BF8915A_set_cfga_gpio_mode(u8 nIC, Reg_TypeDef *configAReg, u8 gpio_analog_en)
{
	configAReg[nIC].tx_data[1] = gpio_analog_en;//璁剧疆CFGAR1,0:妯℃嫙杈撳叆锛�1:鏅�欸PIO
}

/*!
    \brief      淇濈暀浣嶏紝姘歌繙缃�0
    \retval     none
*/
void BF8915A_set_cfga_rsvd(u8 nIC, Reg_TypeDef *configAReg, BOOLEAN rsvd2, BOOLEAN rsvd1)
{
	configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]&(~(0x01<<3));//淇濈暀浣嶏紝姘歌繙缃�0
	configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]&(~(0x01<<2));//淇濈暀浣嶏紝姘歌繙缃�0
} 

/*!
    \brief      Helper function to set osr_sel(璁剧疆ADC杩囬噰鏍风巼閫夋嫨鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configAReg: Register group A(瀵勫瓨鍣ㄧ粍A)
		\param[in]  osr_sel   : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨)
    \retval     none
*/ 
void BF8915A_set_cfga_osr_sel(u8 nIC, Reg_TypeDef *configAReg, BOOLEAN osr_sel)
{
	configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]&(~(0x01<<1));//鍐�0,杩囬噰鏍风巼閫夋嫨64锛�128锛�256锛�512(鍥涚杩囬噰鏍风巼閫氳繃鎸囦护鐨凮SR閫夋嫨)
}

/*!
    \brief      Helper function to set refon(璁剧疆鍩哄噯鐢靛帇閰嶇疆鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configAReg: Register group A(瀵勫瓨鍣ㄧ粍A)
		\param[in]  refon     : 1: VREF1 remains powered on until WDT overflowsVREF1
						                0: VREF1 is turned off after ADC conversion
                          (1:VREF1(鍩哄噯鐢靛帇)淇濇寔涓婄數鐩村埌WDT婧㈠嚭
                            0:ADC杞崲瀹屼箣鍚嶸REF1鍏抽棴 (榛樿))
    \retval     none
*/
void BF8915A_set_cfga_refon(u8 nIC, Reg_TypeDef *configAReg, BOOLEAN refon)
{
    if (refon)//璁剧疆CFGAR2鐨勭1浣�
		configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]|(0x01);//鍐�1,VREF1(鍩哄噯鐢靛帇)淇濇寔涓婄數鐩村埌WDT婧㈠嚭
    else 
		configAReg[nIC].tx_data[2] = configAReg[nIC].tx_data[2]&(~(0x01));//鍐�0,ADC杞崲瀹屼箣鍚嶸REF1鍏抽棴
}

/*!
    \brief      Helper Function to set uv value in CFG register(鍦ㄩ厤缃瘎瀛樺櫒缁勭殑瀵勫瓨鍣ㄤ腑璁剧疆娆犲帇闃堝�肩殑杈呭姪鍑芥暟)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configAReg: Register group A(瀵勫瓨鍣ㄧ粍A)
		\param[in]  uv        : Low 8 bits of undervoltage threshold(浣�8浣嶆瑺鍘嬮槇鍊�)
    \retval     none
*/
void BF8915A_set_cfga_uv(u8 nIC, Reg_TypeDef *configAReg, u16 uv)
{
	configAReg[nIC].tx_data[3] = 0x00FF & uv;						//璁剧疆CFGAR3,娆犲帇闃堝�间綆8浣嶆斁鍏�
	configAReg[nIC].tx_data[4] = configAReg[nIC].tx_data[4]&0xF0;	//灏咰FGAR4浣�4浣嶆竻绌�
	configAReg[nIC].tx_data[4] = configAReg[nIC].tx_data[4]|((0x0F00 & uv)>>8);//璁剧疆CFGAR4浣�4浣嶏紝娆犲帇闃堝�奸珮4浣嶆斁鍏�
}

/*!
    \brief      helper function to set OV value in CFG register(鍦ㄩ厤缃瘎瀛樺櫒缁勭殑瀵勫瓨鍣ㄤ腑璁剧疆杩囧帇闃堝�肩殑杈呭姪鍑芥暟)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configAReg: Register group A(瀵勫瓨鍣ㄧ粍A)
		\param[in]  ov        : Overvoltage threshold lower 4 bits(杩囧帇闃堝�间綆4浣�)
    \retval     none
*/
void BF8915A_set_cfga_ov(u8 nIC, Reg_TypeDef *configAReg, u16 ov)
{
	configAReg[nIC].tx_data[5] = 0x00FF & (ov>>4);					//璁剧疆CFGAR5,杩囧帇闃堝�奸珮8浣嶆斁鍏�
	configAReg[nIC].tx_data[4] = configAReg[nIC].tx_data[4]&0x0F;	//灏咰FGAR4楂�4浣嶆竻绌�
	configAReg[nIC].tx_data[4] = configAReg[nIC].tx_data[4]|((0x000F & ov)<<4);//璁剧疆CFGAR4楂�4浣嶏紝杩囧帇闃堝�间綆4浣嶆斁鍏�
																			 //淇濆瓨CFGAR4浣�4浣嶏紝鍗虫瑺鍘嬮槇鍊奸珮4浣�
}
/*!
    \brief      Helper function to set CFGRA variable(璁剧疆閰嶇疆瀵勫瓨鍣ㄧ粍A鍙橀噺鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC           : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configAReg    : Register group A(瀵勫瓨鍣ㄧ粍A)
		\param[in]  gpio_pd_en    : GPIO pin pull-down enable; 1: GPIO pin pull-down disable (default)(0:GPIO寮曡剼涓嬫媺浣胯兘锛�1:GPIO寮曡剼涓嬫媺绂佺敤(榛樿))
		\param[in]  gpio_analog_en: 0: analog input 1: Ordinary GPIO(0:妯℃嫙杈撳叆锛�1:鏅�欸PIO)
		\param[in]  rsvd2         : 淇濈暀浣嶏紝姘歌繙缃�0
		\param[in]  rsvd1         : 淇濈暀浣嶏紝姘歌繙缃�0
		\param[in]  osr_sel       : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨)
		\param[in]  refon         : 1: VREF1 remains powered on until WDT overflowsVREF1
						                    0: VREF1 is turned off after ADC conversion
                              (1:VREF1(鍩哄噯鐢靛帇)淇濇寔涓婄數鐩村埌WDT婧㈠嚭
                                0:ADC杞崲瀹屼箣鍚嶸REF1鍏抽棴 (榛樿))
		\param[in]  uv_th         : Low 8 bits of undervoltage threshold(浣�8浣嶆瑺鍘嬮槇鍊�)
		\param[in]  ov_th         : Overvoltage threshold lower 4 bits(杩囧帇闃堝�间綆4浣�)
    \retval     none
*/
void BF8915A_set_cfgra(u8 nIC,
						 Reg_TypeDef *configAReg,
						 u8 gpio_pd_en,			//鍐�0:GPIO鑴氫笅鎷変娇鑳斤紱1:GPIO鑴氫笅鎷夊叧闂�(榛樿)璇�0 :GPIO鑴氱殑鍊兼槸0 锛�1:GPIO鑴氱殑鍊兼槸1
						 u8 gpio_analog_en,		//GPIO0~7妯″紡閫夋嫨 0:妯℃嫙杈撳叆 1:鏅�欸PIO
						 BOOLEAN rsvd2,			//淇濈暀浣嶏紝姘歌繙缃�0
						 BOOLEAN rsvd1,			//淇濈暀浣嶏紝姘歌繙缃�0
						 BOOLEAN osr_sel,		//0:杩囬噰鏍风巼閫夋嫨64锛�128锛�256锛�512(鍥涚杩囬噰鏍风巼閫氳繃鎸囦护鐨凮SR閫夋嫨)
											    //杩欎釜瀵勫瓨鍣ㄤ綅瑕佸拰鎸囦护鐨凮SR涓�璧烽厤缃�夋嫨ADC杩囬噰鏍风巼
						 BOOLEAN refon,  		//VREF1(鍩哄噯鐢靛帇)閰嶇疆  1 :VREF1淇濇寔涓婄數鐩村埌WDT婧㈠嚭  0 :ADC杞崲瀹屼箣鍚嶸REF1鍏抽棴 (榛樿)
						 u16 uv_th,				//Under voltage Comparison Voltage(娆犲帇闃堝��)锛屾牴鎹疄闄呮儏鍐佃嚜琛岃瀹�
										 	 	 //Under voltage threshold ADC Code(娆犵數鍘嬮槇鍊兼ā鏁拌浆鎹㈠櫒浠ｇ爜): uv_th = 3000 * 65535U / 5000U. LSB = 0.0001 ---(3V)
						 u16 ov_th 				//Over  voltage Comparison Voltage(杩囧帇闃堝��)锛屾牴鎹疄闄呮儏鍐佃嚜琛岃瀹�
										 	 	 //Over  voltage threshold ADC Code(杩囩數鍘嬮槇鍊兼ā鏁拌浆鎹㈠櫒浠ｇ爜): ov_th = 4100 * 65535U / 5000U. LSB = 0.0001 ---(4.1V)
									 )
{
	BF8915A_set_cfga_gpio(nIC, configAReg, gpio_pd_en);			//璁剧疆CFGAR0
	BF8915A_set_cfga_gpio_mode(nIC, configAReg,gpio_analog_en);	//璁剧疆CFGAR1
	BF8915A_set_cfga_rsvd(nIC, configAReg,rsvd2,rsvd1);			//淇濈暀浣嶏紝姘歌繙缃�0
	BF8915A_set_cfga_osr_sel(nIC, configAReg,osr_sel);			//璁剧疆CFGAR2鐨勭2浣�
	BF8915A_set_cfga_refon(nIC, configAReg,refon);				//璁剧疆CFGAR2鐨勭1浣�
	BF8915A_set_cfga_uv(nIC, configAReg,uv_th);					//璁剧疆CFGAR3鍜孋FGAR4浣�4浣�
	BF8915A_set_cfga_ov(nIC, configAReg,ov_th);					//璁剧疆CFGAR5鍜孋FGAR4楂�4浣�
}

/*!
    \brief      Helper function to control discharge(鎺у埗鏀剧數鐨勮緟鍔╁姛鑳�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configBReg: Register group B(瀵勫瓨鍣ㄧ粍B)
		\param[in]  dcc       :	1: Turn on  the switch of Cell x(鎵撳紑x鍙风數姹犵殑寮�鍏�)
							              0: Turn off the switch of Cell x(鍏抽棴x鍙风數姹犵殑寮�鍏�)
    \retval     none
*/
void BF8915A_set_cfgb_dcc(u8 nIC, Reg_TypeDef *configBReg, u16 dcc)
{
	configBReg[nIC].tx_data[0] = (u8)dcc;			//璁剧疆CFGBR0,鏀剧數鐢垫睜寮�鍏硏=1~8
	configBReg[nIC].tx_data[1] = (u8)(dcc >> 0x8U);	//璁剧疆CFGBR1,鏀剧數鐢垫睜寮�鍏硏=9~16
}

/*!
    \brief      Balanced duty cycle selection(鍧囪　鍗犵┖姣旈�夋嫨)
		\param[in]  nIC        : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configBReg : Register group B(瀵勫瓨鍣ㄧ粍B)
		\param[in]  bl_duty_sel: Active balancing duty cycle selection(鍧囪　鍗犵┖姣旈�夋嫨)
    \retval     none
*/
//bl_duty_sel(4浣�): 鍧囪　鍗犵┖姣旈�夋嫨,鍗犵┖姣�=(BL_DUTY_SEL+1)*10%锛岄厤缃寖鍥�10%~100%锛屽綋澶т簬9鏃舵槸100%銆�
void BF8915A_set_cfgb_bl_duty_sel(u8 nIC, Reg_TypeDef *configBReg, u8 bl_duty_sel)
{
	configBReg[nIC].tx_data[2] = configBReg[nIC].tx_data[2] & 0x0F;//灏咰FGBR2鐨勯珮4浣嶆竻绌�
	configBReg[nIC].tx_data[2] = configBReg[nIC].tx_data[2] | ((0x0F & bl_duty_sel)<<4);//璁剧疆CFGBR2楂�4浣�,鍧囪　鍗犵┖姣旈�夋嫨
                                                                                      //淇濆瓨CFGBR2浣�4浣�
}

/*!
    \brief      Helper function to set dcto(璁剧疆鏀剧數瀹氭椂婧㈠嚭鍊奸厤缃殑杈呭姪鍑芥暟)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configBReg: Register group B(瀵勫瓨鍣ㄧ粍B)
		\param[in]  dcto      : Discharge timing overflow value configuration(鏀剧數瀹氭椂婧㈠嚭鍊奸厤缃紝璇︾粏璇存槑濡侱CTO琛ㄦ墍绀�)
    \retval     none
*/
void BF8915A_set_cfgb_dcto(u8 nIC, Reg_TypeDef *configBReg, u8 dcto)
{
	configBReg[nIC].tx_data[2] = configBReg[nIC].tx_data[2] & 0xF0;//灏咰FGBR2鐨勪綆4浣嶆竻绌�
	configBReg[nIC].tx_data[2] = configBReg[nIC].tx_data[2] | (0x0F & dcto);//璁剧疆CFGBR2浣�4浣�,鏀剧數瀹氭椂婧㈠嚭鍊奸厤缃�
                                                                          //淇濆瓨CFGBR2楂�4浣�
}

/*!
    \brief      Helper function to set vuv_bl(璁剧疆鍧囪　娆犲帇闃堝�肩殑杈呭姪鍑芥暟)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configBReg: Register group B(瀵勫瓨鍣ㄧ粍B)
		\param[in]  vuv_bl    : Lower 8 bits of equalization undervoltage threshold(鍧囪　娆犲帇闃堝�肩殑浣�8浣�)
    \retval     none
*/
void BF8915A_set_cfgb_vuv_bl(u8 nIC, Reg_TypeDef *configBReg, u16 vuv_bl)
{
	configBReg[nIC].tx_data[3] = vuv_bl & 0xFF;						//璁剧疆CFGBR3,鍧囪　娆犲帇闃堝�间綆8浣嶆斁鍏�
	configBReg[nIC].tx_data[4] = configBReg[nIC].tx_data[4] & 0xF0;	//灏咰FGBR4鐨勪綆4浣嶆竻绌�
	configBReg[nIC].tx_data[4] = configBReg[nIC].tx_data[4] | ((vuv_bl & 0x0F00U) >> 0x8U);//璁剧疆CFGBR4浣�4浣�,鍧囪　娆犲帇闃堝�奸珮4浣嶆斁鍏�
                                                                                         //淇濆瓨CFGBR4楂�4浣�
}

/*!
    \brief      Helper function to set bl_dtmen(璁剧疆鍚敤鏀剧數鐩戞祴鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configBReg: Register group B(瀵勫瓨鍣ㄧ粍B)
		\param[in]  dtmen     : Enable discharge monitor(鍚敤鏀剧數鐩戞祴) 1: Enable  discharge monitor(鍚敤鏀剧數鐩戞祴)
														                                         0: Disable discharge monitor(绂佺敤鏀剧數鐩戞祴)
    \retval     none
*/
void BF8915A_set_cfgb_bl_dtmen(u8 nIC, Reg_TypeDef *configBReg, BOOLEAN dtmen)
{
	if (dtmen)//璁剧疆CFGAR4鐨勭5浣�
		configBReg[nIC].tx_data[4] = configBReg[nIC].tx_data[4] | (0x01<<4);//鍐�1,浣胯兘鏀剧數鐩戣鍣�
	else
		configBReg[nIC].tx_data[4] = configBReg[nIC].tx_data[4] & (~(0x01<<4));//鍐�0,涓嶄娇鑳芥斁鐢电洃瑙嗗櫒
}

/*!
    \brief      Helper function to set CFGRB variable(璁剧疆閰嶇疆瀵勫瓨鍣ㄧ粍B鍙橀噺鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC        : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configBReg : Register group B(瀵勫瓨鍣ㄧ粍B)
		\param[in]  dcc        :	1: Turn on  the switch of Cell x(鎵撳紑x鐢垫睜鐨勫紑鍏�)
							                0: Turn off the switch of Cell x(鍏抽棴x鐢垫睜鐨勫紑鍏�)
		\param[in]  bl_duty_sel: Active balancing duty cycle selection(鍧囪　鍗犵┖姣旈�夋嫨)
		\param[in]  dcto       : Discharge timing overflow value configuration(鏀剧數瀹氭椂婧㈠嚭鍊奸厤缃�)
		\param[in]  vuv_bl     : Lower 8 bits of equalization undervoltage threshold(鍧囪　娆犲帇闃堝�肩殑浣�8浣�)
		\param[in]  dtmen      : Enable discharge monitor(浣胯兘鏀剧數鐩戞祴鍣�)	1: Enable  discharge monitor(鍚敤鏀剧數鐩戞祴)
														                                            0: Disable discharge monitor(绂佺敤鏀剧數鐩戞祴)
    \retval     none
*/
void BF8915A_set_cfgrb(u8 nIC,
						 Reg_TypeDef *configBReg,
						 u16 dcc,			//鏀剧數鐢垫睜寮�鍏�1:鎵撳紑Cell x 鐨勫紑鍏�  0:鍏抽棴Cell x 鐨勫紑鍏� (榛樿)
						 u8 bl_duty_sel,	//鍧囪　鍗犵┖姣旈�夋嫨 鍗犵┖姣�=(BL_DUTY_SEL+1)*10%锛岄厤缃寖鍥�10%~100%锛屽綋澶т簬9鏃舵槸100%
						 u8 dcto,			//鏀剧數瀹氭椂婧㈠嚭鍊奸厤缃�
						 u16 vuv_bl,		//鍧囪　娆犲帇闃堝��
						 BOOLEAN dtmen		//鍦ㄨ嚜鍔ㄥ潎琛′笅锛屽鏋淒TMEN=1锛屽綋娆犲帇(灏忎簬VUV_BL)鍏抽棴鏀剧數寮�鍏筹紝濡傛灉DTMEN=0锛屼笉杩涜娆犲帇姣旇緝
						 )
{
	BF8915A_set_cfgb_dcc(nIC, configBReg,dcc);					//鎺у埗鏀剧數鐨勮緟鍔╁姛鑳�
	BF8915A_set_cfgb_bl_duty_sel(nIC, configBReg,bl_duty_sel);	//鍧囪　鍗犵┖姣旈�夋嫨
	BF8915A_set_cfgb_dcto(nIC, configBReg,dcto);				//璁剧疆鏀剧數瀹氭椂婧㈠嚭鍊奸厤缃殑杈呭姪鍑芥暟
	BF8915A_set_cfgb_vuv_bl(nIC, configBReg,vuv_bl);			//璁剧疆鍧囪　娆犲帇闃堝�肩殑杈呭姪鍑芥暟
	BF8915A_set_cfgb_bl_dtmen(nIC, configBReg,dtmen);			//璁剧疆鍚敤鏀剧數鐩戞祴鐨勮緟鍔╁嚱鏁�
}

/*!
    \brief      Helper function to set Undervoltage threshold in monitoring mode(鐩戞祴妯″紡涓嬶紝璁剧疆娆犲帇闃堝�肩殑杈呭姪鍑芥暟)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configCReg: Register group C(瀵勫瓨鍣ㄧ粍C)
		\param[in]  vuv_m     : Undervoltage threshold in monitoring mode(鐩戞祴妯″紡涓嬬殑娆犲帇闃堝��)
    \retval     none
*/
void BF8915A_set_cfgc_vuv_m(u8 nIC, Reg_TypeDef *configCReg, u16 vuv_m)
{
	configCReg[nIC].tx_data[0] = 0x00FF & vuv_m;					//璁剧疆CFGCR0,鐩戞祴妯″紡涓嬶紝娆犲帇闃堝�间綆8浣嶆斁鍏�
	configCReg[nIC].tx_data[1] = configCReg[nIC].tx_data[1]&0xF0;	//灏咰FGCR1鐨勪綆4浣嶆竻绌�
	configCReg[nIC].tx_data[1] = configCReg[nIC].tx_data[1]|((0x0F00 & vuv_m)>>8);//璁剧疆CFGCR1浣�4浣�,鐩戞祴妯″紡涓嬶紝娆犲帇闃堝�奸珮4浣嶆斁鍏�
																			//淇濆瓨CFGCR1楂�4浣�
}

/*!
    \brief      Helper function to set Overvoltage  threshold in monitoring mode(鐩戞祴妯″紡涓嬶紝璁剧疆杩囧帇闃堝�肩殑杈呭姪鍑芥暟)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configCReg: Register group C(瀵勫瓨鍣ㄧ粍C)
		\param[in]  vov_m     : Overvoltage threshold in monitoring mode(鐩戞祴妯″紡涓嬬殑杩囧帇闃堝��)
    \retval     none
*/
void BF8915A_set_cfgc_vov_m(u8 nIC, Reg_TypeDef *configCReg, u16 vov_m)
{
	configCReg[nIC].tx_data[2] = 0x00FF & (vov_m>>4);				//璁剧疆CFGCR2,鐩戞祴妯″紡涓嬶紝杩囧帇闃堝�奸珮8浣嶆斁鍏�
	configCReg[nIC].tx_data[1] = configCReg[nIC].tx_data[1]&0x0F;	//灏咰FGCR1鐨勯珮4浣嶆竻绌�
	configCReg[nIC].tx_data[1] = configCReg[nIC].tx_data[1]|((0x000F & vov_m)<<4);//璁剧疆CFGCR1楂�4浣�,鐩戞祴妯″紡涓嬶紝杩囧帇闃堝�间綆4浣嶆斁鍏�
																			//淇濆瓨CFGCR1浣�4浣�
}

/*!
    \brief      Helper function to set Internal over-temperature threshold in monitoring mode(鐩戞祴妯″紡涓嬶紝璁剧疆鍐呴儴杩囨俯闃堝�肩殑杈呭姪鍑芥暟)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configCReg: Register group C(瀵勫瓨鍣ㄧ粍C)
		\param[in]  itov_m    : In monitoring mode, internal over-temperature threshold(鐩戞祴妯″紡涓嬶紝鍐呴儴杩囨俯闃堝��)
    \retval     none
*/
void BF8915A_set_cfgc_itov_m(u8 nIC, Reg_TypeDef *configCReg, u16 itov_m)
{
	configCReg[nIC].tx_data[3] = 0x00FF & itov_m;					//璁剧疆CFGCR3,鐩戞祴妯″紡涓嬶紝鍐呴儴杩囨俯闃堝�间綆8浣嶆斁鍏�
	configCReg[nIC].tx_data[4] = configCReg[nIC].tx_data[4]&0xF0;	//灏咰FGCR4鐨勪綆4浣嶆竻绌�
	configCReg[nIC].tx_data[4] = configCReg[nIC].tx_data[4]|((0x0F00 & itov_m)>>8);//璁剧疆CFGCR4浣�4浣�,鐩戞祴妯″紡涓嬶紝鍐呴儴杩囨俯闃堝�奸珮4浣嶆斁鍏�
																				 //淇濆瓨CFGCR4楂�4浣�
}

/*!
    \brief      Helper function to set external over-temperature threshold in monitoring mode(鐩戞祴妯″紡涓嬶紝璁剧疆澶栭儴杩囨俯闃堝�肩殑杈呭姪鍑芥暟)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configCReg: Register group C(瀵勫瓨鍣ㄧ粍C)
		\param[in]  extov_m   : In monitoring mode, external over-temperature threshold(鐩戞祴妯″紡涓嬶紝澶栭儴杩囨俯闃堝��)
    \retval     none
*/
void BF8915A_set_cfgc_extov_m(u8 nIC, Reg_TypeDef *configCReg, u16 extov_m)
{
	configCReg[nIC].tx_data[5] = 0x00FF & (extov_m>>4);					//璁剧疆CFGCR5,鐩戞祴妯″紡涓嬶紝澶栭儴杩囨俯闃堝�奸珮8浣嶆斁鍏�
	configCReg[nIC].tx_data[4] = configCReg[nIC].tx_data[4]&0x0F;		//灏咰FGCR4鐨勯珮4浣嶆竻绌�
	configCReg[nIC].tx_data[4] = configCReg[nIC].tx_data[4]|((0x000F & extov_m)<<4);//璁剧疆CFGCR4楂�4浣�,鐩戞祴妯″紡涓嬶紝澶栭儴杩囨俯闃堝�间綆4浣嶆斁鍏�
																				  //淇濆瓨CFGCR4浣�4浣�
}

/*!
    \brief      Helper function to set CFGRC variable(璁剧疆閰嶇疆瀵勫瓨鍣ㄧ粍C鍙橀噺鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configCReg: Register group C(瀵勫瓨鍣ㄧ粍C)
		\param[in]  vuv_m     : Undervoltage threshold in monitoring mode(鐩戞祴妯″紡涓嬬殑娆犲帇闃堝��)
		\param[in]  vov_m     : Overvoltage  threshold in monitoring mode(鐩戞祴妯″紡涓嬬殑杩囧帇闃堝��)
		\param[in]  itov_m    : Internal over-temperature threshold in monitoring mode(鐩戞祴妯″紡涓嬶紝鍐呴儴杩囨俯闃堝��)
		\param[in]  extov_m   : external over-temperature threshold in monitoring mode(鐩戞祴妯″紡涓嬶紝澶栭儴杩囨俯闃堝��)
    \retval     none
*/
void BF8915A_set_cfgrc(u8 nIC, Reg_TypeDef *configCReg, u16 vuv_m, u16 vov_m, u16 itov_m, u16 extov_m)
{
	BF8915A_set_cfgc_vuv_m(nIC, configCReg,vuv_m);		//鐩戞祴妯″紡涓嬶紝璁剧疆娆犲帇闃堝�肩殑杈呭姪鍑芥暟
	BF8915A_set_cfgc_vov_m(nIC, configCReg,vov_m);		//鐩戞祴妯″紡涓嬶紝璁剧疆杩囧帇闃堝�肩殑杈呭姪鍑芥暟
	BF8915A_set_cfgc_itov_m(nIC, configCReg,itov_m);	//鐩戞祴妯″紡涓嬶紝璁剧疆鍐呴儴杩囨俯闃堝�肩殑杈呭姪鍑芥暟
	BF8915A_set_cfgc_extov_m(nIC, configCReg,extov_m);	//鐩戞祴妯″紡涓嬶紝璁剧疆澶栭儴杩囨俯闃堝�肩殑杈呭姪鍑芥暟
}

/*!
    \brief      Helper function to set csel_m(璁剧疆鐩戞祴妯″紡涓嬶紝鐢垫睜鐢靛帇鐩戞祴姣旇緝閫夋嫨鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg: Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  csel_m    : In monitoring mode, battery voltage monitoring comparison selection cell1~cell16(鍦ㄧ洃娴嬫ā寮忎笅锛岀數姹犵數鍘嬬洃娴嬫瘮杈冮�夋嫨鐢垫睜1 ~鐢垫睜16)
    \retval     none
*/
void BF8915A_set_cfgd_csel_m(u8 nIC, Reg_TypeDef *configDReg, u16 csel_m)
{
	configDReg[nIC].tx_data[0] = 0x00FF & csel_m;			//璁剧疆CFGDR0锛屽嵆鐢垫睜鐢靛帇鐩戞祴姣旇緝閫夋嫨cell1~cell8
	configDReg[nIC].tx_data[1] = (csel_m & 0xFF00) >> 0x8U;	//璁剧疆CFGDR1锛屽嵆鐢垫睜鐢靛帇鐩戞祴姣旇緝閫夋嫨cell9~cell16
}

/*!
    \brief      Helper function to set gsel_m(璁剧疆鐩戞祴妯″紡涓嬶紝GPIO0~7姣旇緝閫夋嫨鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg: Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  gsel_m    : In monitoring mode, GPIO0~7 is compared and selected(鐩戞祴妯″紡涓嬶紝GPIO0~7姣旇緝閫夋嫨)
    \retval     none
*/
void BF8915A_set_cfgd_gsel_m(u8 nIC, Reg_TypeDef *configDReg, u8 gsel_m)
{
	configDReg[nIC].tx_data[2] = gsel_m;//璁剧疆CFGDR2
}

/*!
    \brief      Helper function to set isospi_wake_t(璁剧疆鑿婅姳閾惧敜閱掓椂闂寸殑杈呭姪鍑芥暟)
		\param[in]  nIC          : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg   : Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  isospi_wake_t: isospi wake-up time, 0: 10us; 1:20us(isospi(鑿婅姳閾維PI)鍞ら啋鏃堕棿锛�0:10us锛�1:20us)
    \retval     none
*/
void BF8915A_set_cfgd_isospi_wake_t(u8 nIC, Reg_TypeDef *configDReg, BOOLEAN isospi_wake_t)
{
  if (isospi_wake_t)//璁剧疆CFGDR3鐨勭8浣�
		configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] | (0x01<<7);//鍐�1
   else 
		configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] & (~(0x01<<7));//鍐�0
}

/*!
    \brief      Helper function to set adc_rst_sel(璁剧疆娴嬮噺绗�1涓�氶亾ADC澶嶄綅閫夋嫨鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC        : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg : Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  adc_rst_sel: 0: All measurement channels reset ADC(鎵�鏈夋祴閲忛�氶亾閮藉浣岮DC)
								             1: Only the first channel resets ADC, then 2~n channels do not reset ADC(鍙湁绗�1涓�氶亾澶嶄綅ADC锛屼箣鍚�2~n閫氶亾涓嶅浣岮DC)
    \retval     none
*/
void BF8915A_set_cfgd_adc_rst_sel(u8 nIC, Reg_TypeDef *configDReg, BOOLEAN adc_rst_sel)
{
  if (adc_rst_sel)//璁剧疆CFGDR3鐨勭7浣�
		configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] | (0x01<<6);//鍐�1
  else 
		configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] & (~(0x01<<6));//鍐�0
}

/*!
    \brief      Helper function to set time_sel_m(璁剧疆鐩戞祴妯″紡涓嬶紝瀹氭椂鐩戞祴鏃堕棿閰嶇疆鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg: Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  time_sel_m: In monitoring mode, timing monitoring time configuration(鐩戞祴妯″紡涓嬶紝瀹氭椂鐩戞祴鏃堕棿閰嶇疆)
    \retval     none
*/
//time_sel_m(5浣�):鐩戞祴妯″紡涓嬶紝瀹氭椂鐩戞祴鏃堕棿閰嶇疆锛屽畾鏃舵椂闂�=(TIME_SEL_M+1)*1s(2~64s)锛屾渶灏忛厤缃�兼槸1(2s)
//杩欎釜鏃堕棿閰嶇疆鍜屽潎琛″懆鏈熼厤缃叡鐢�
void BF8915A_set_cfgd_time_sel_m(u8 nIC, Reg_TypeDef *configDReg, u8 time_sel_m)
{
  
	configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] & 0xC0U;//灏咰FGDR3浣�6浣嶆竻绌�
	configDReg[nIC].tx_data[3] = configDReg[nIC].tx_data[3] | (time_sel_m & 0x3F);//璁剧疆CFGDR3浣�6浣�
                                                                                //淇濆瓨CFGDR3楂�2浣�
}

/*!
    \brief      Helper function to set adc_init_t2(璁剧疆ADC娴嬮噺绗�2~n涓�氶亾ADC鍒濆鍖栨椂闂寸殑杈呭姪鍑芥暟)
		\param[in]  nIC        : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg : Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  adc_init_t2: ADC measurement 2~n channel ADC initialization time(ADC娴嬮噺绗�2~n涓�氶亾ADC鍒濆鍖栨椂闂�)
    \retval     none
*/
//ADC娴嬮噺绗�2~n涓�氶亾ADC鍒濆鍖栨椂闂达紝(ADC_INIT_T2+1)*20us(20~320us)
void BF8915A_set_cfgd_adc_init_t2(u8 nIC, Reg_TypeDef *configDReg, u8 adc_init_t2)
{
  
	configDReg[nIC].tx_data[4] = configDReg[nIC].tx_data[4] & 0x0FU;//灏咰FGDR4楂�4浣嶆竻绌�
	configDReg[nIC].tx_data[4] = configDReg[nIC].tx_data[4] | ((adc_init_t2 & 0x0F) << 0x4U);//璁剧疆CFGDR4楂�4浣�
                                                                                           //淇濆瓨CFGDR4浣�4浣�
}

/*!
    \brief      Helper function to set adc_init_t1(璁剧疆ADC娴嬮噺绗�1涓�氶亾ADC鍒濆鍖栨椂闂寸殑杈呭姪鍑芥暟)
		\param[in]  nIC        : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg : Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  adc_init_t1: ADC measurement 1 channel ADC initialization time(ADC娴嬮噺绗�1涓�氶亾ADC鍒濆鍖栨椂闂�)
    \retval     none
*/
//ADC娴嬮噺绗�1涓�氶亾ADC鍒濆鍖栨椂闂�(ADC_INIT_T1+1)*100us(100~1600us)
void BF8915A_set_cfgd_adc_init_t1(u8 nIC, Reg_TypeDef *configDReg, u8 adc_init_t1)
{
	configDReg[nIC].tx_data[4] = configDReg[nIC].tx_data[4] & 0xF0U;//灏咰FGDR4浣�4浣嶆竻绌�
	configDReg[nIC].tx_data[4] = configDReg[nIC].tx_data[4] | (adc_init_t1 & 0x0F);//璁剧疆CFGDR4浣�4浣�
                                                                                 //淇濆瓨CFGDR4楂�4浣�
}

/*!
    \brief      Helper function to set trim2_en(璁剧疆淇皟2浣胯兘鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg: Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  trim2_en  : Modification 2 enable, 1: enable, 0: disable(淇皟2浣胯兘锛�1:浣胯兘锛�0:涓嶄娇鑳�)
    \retval     none
*/
void BF8915A_set_cfgd_trim2_en(u8 nIC, Reg_TypeDef *configDReg, BOOLEAN trim2_en)
{
  if (trim2_en)//璁剧疆CFGDR5鐨勭8浣�
		configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] | (0x01<<7);//鍐�1
   else 
		configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] & (~(0x01<<7));//鍐�0
}

/*!
    \brief      Helper function to set trim1_en(璁剧疆淇皟1浣胯兘鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC       : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg: Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  trim1_en  : Modification 1 enable, 1: enable, 0: disable(淇皟1浣胯兘锛�1:浣胯兘锛�0:涓嶄娇鑳�)
    \retval     none
*/
void BF8915A_set_cfgd_trim1_en(u8 nIC, Reg_TypeDef *configDReg, BOOLEAN trim1_en)
{
  if (trim1_en)//璁剧疆CFGDR5鐨勭7浣�
		configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] | (0x01<<6);//鍐�1
   else 
		configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] & (~(0x01<<6));//鍐�0
}

/*!
    \brief      Helper function to set trim1_en(璁剧疆鐩戞祴妯″紡涓嬪彂鐢熷紓甯革紝涓婃姤鐨勯棿闅旀椂闂寸殑杈呭姪鍑芥暟)
		\param[in]  nIC        : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg : Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  err_t_sel_m: When an exception occurs in the monitoring mode, the reporting interval(鐩戞祴妯″紡涓嬪彂鐢熷紓甯革紝涓婃姤鐨勯棿闅旀椂闂�)
    \retval     none
*/
//err_t_sel_m(2浣�):鐩戞祴妯″紡涓嬪彂鐢熷紓甯革紝涓婃姤鐨勯棿闅旀椂闂� 00:200ms锛�01:400ms锛�10:600ms锛�11:800ms
void BF8915A_set_cfgd_err_t_sel_m(u8 nIC, Reg_TypeDef *configDReg, u8 err_t_sel_m)
{
  
	configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] & 0xCFU;//灏咰FGDR5鐨勭6~5浣嶆竻绌�
	configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] | ((err_t_sel_m & 0x03) << 0x4U);//璁剧疆CFGDR5绗�6~5浣�
                                                                                           //淇濆瓨CFGDR5绗�8~7浣�
}

/*!
    \brief      Helper function to set CFGRD variable(璁剧疆閰嶇疆瀵勫瓨鍣ㄧ粍D鍙橀噺鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC 	    : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg: A two dimensional array of the configuration data that will be written(灏嗚鍐欏叆閰嶇疆鏁版嵁鐨勪簩缁存暟缁�)
		\param[in]  vd33t_on_m: A two dimensional array of the configuration data that will be written(灏嗚鍐欏叆閰嶇疆鏁版嵁鐨勪簩缁存暟缁�)
    \retval     none   	                  					  
*/
//vd33t_on_m:鐩戞祴妯″紡涓嬶紝VREF1(鍩哄噯鐢靛帇)鍜孷D33T鐨勫惎鍔ㄦ椂闂撮厤缃�(5+VD33T_ON_M*10)ms锛岃寖鍥存槸5~155ms
void BF8915A_set_cfgd_vd33t_on_m(u8 nIC, Reg_TypeDef *configDReg, u8 vd33t_on_m)
{
	configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] & 0xF0U;//灏咰FGDR5鐨勪綆4浣嶆竻绌�
	configDReg[nIC].tx_data[5] = configDReg[nIC].tx_data[5] | (vd33t_on_m & 0x0F);//淇濆瓨CFGDR5楂�4浣嶏紝璁剧疆CFGDR5浣�4浣�
}

/*!
    \brief      Helper function to set CFGRD variable(璁剧疆閰嶇疆瀵勫瓨鍣ㄧ粍D鍙橀噺鐨勮緟鍔╁嚱鏁�)
		\param[in]  nIC          : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDReg   : Register group D(瀵勫瓨鍣ㄧ粍D)
		\param[in]  csel_m       : In monitoring mode, battery voltage monitoring comparison selection cell1~cell16(鍦ㄧ洃娴嬫ā寮忎笅锛岀數姹犵數鍘嬬洃娴嬫瘮杈冮�夋嫨鐢垫睜1~鐢垫睜16)
		\param[in]  gsel_m       : In monitoring mode, GPIO0~7 comparison selection(鐩戞祴妯″紡涓嬶紝GPIO0~7姣旇緝閫夋嫨)
		\param[in]  isospi_wake_t: isospi wake up time(鑿婅姳閾惧敜閱掓椂闂�)
		\param[in]  adc_rst_sel  : 0: All measurement channels reset ADC(鎵�鏈夋祴閲忛�氶亾閮藉浣岮DC)
								                1: Only the first channel resets ADC, then 2~n channels do not reset ADC(鍙湁绗�1涓�氶亾澶嶄綅ADC锛屼箣鍚�2~n閫氶亾涓嶅浣岮DC)
		\param[in]  time_sel_m   : In monitoring mode, timing monitoring time configuration, timing time=(TIME_SEL_M+1)*1s(2~64s), the minimum configuration value is 1(2s);This time configuration is shared with the balance period configuration
                              (鐩戞祴妯″紡涓嬶紝瀹氭椂鐩戞祴鏃堕棿閰嶇疆锛屽畾鏃舵椂闂�=(TIME_SEL_M+1)*1s(2~64s)锛屾渶灏忛厤缃�兼槸1(2s)锛涜鏃堕棿閰嶇疆涓庡潎琛℃椂鏈熺殑閰嶇疆鍏变韩)
		\param[in]  adc_init_t2  : ADC measures the ADC initialization time of the second to n channels, (ADC_INIT_T2+1)*20us (20~320us)(ADC娴嬮噺绗�2~n涓�氶亾ADC鍒濆鍖栨椂闂达紝(ADC_INIT_T2+1)*20us(20~320us))
		\param[in]  adc_init_t1  : ADC measurement of the first channel ADC initialization time (ADC_INIT_T1+1)*100us (100~1600us)(ADC娴嬮噺绗�1涓�氶亾ADC鍒濆鍖栨椂闂�(ADC_INIT_T1+1)*100us(100~1600us))
		\param[in]  trim2_en:1   : enable, 0: disable(淇皟2浣胯兘锛�1:浣胯兘锛�0:涓嶄娇鑳�)
		\param[in]  trim1_en:1   : enable, 0: disable(淇皟1浣胯兘锛�1:浣胯兘锛�0:涓嶄娇鑳�)
		\param[in]  err_t_sel_m  : 00: 200ms, 01: 400ms, 10: 600ms, 11: 800ms(鐩戞祴妯″紡涓嬪彂鐢熷紓甯革紝涓婃姤鐨勯棿闅旀椂闂�  00:200ms锛�01:400ms锛�10:600ms锛�11:800ms)
		\param[in]  vd33t_on_m   : The startup time configuration of VREF1 and VD33T (5+VD33T_ON_M*10)ms, the range is 5~155ms(鐩戞祴妯″紡涓嬶紝VREF1鍜孷D33T鐨勫惎鍔ㄦ椂闂撮厤缃�(5+VD33T_ON_M*10)ms锛岃寖鍥存槸5~155ms)
    \retval     none   	                  					  
*/
void BF8915A_set_cfgrd(u8 nIC,
					 Reg_TypeDef *configDReg, 
					 u16 csel_m,		//鐩戞祴妯″紡涓嬶紝鐢垫睜鐢靛帇鐩戞祴姣旇緝閫夋嫨cell1~cell16
                           	   	   	    //1:鐩稿簲鐨勭數姹犲氨浼氬湪鐩戞祴妯″紡涓嬭繘琛岃繃娆犲帇姣旇緝锛岀疆鏍囧織浣�
                           	   	   	    //0:鐩稿簲鐨勭數姹犲氨浼氬湪鐩戞祴妯″紡涓嬩笉杩涜杩囨瑺鍘嬫瘮杈冿紝涓嶇疆鏍囧織浣�
					 u8 gsel_m ,		//鐩戞祴妯″紡涓嬶紝GPIO0~7姣旇緝閫夋嫨
                           	   	   	    //1:鐩稿簲鐨勭數姹犲氨浼氬湪鐩戞祴妯″紡涓嬭繘琛岃繃娓�(灏忎簬澶栭儴娓╁害闃堝��)姣旇緝锛岀疆鏍囧織浣�
                           	   	   	    //0:鐩稿簲鐨勭數姹犲氨浼氬湪鐩戞祴妯″紡涓嬩笉杩涜杩囨俯(灏忎簬澶栭儴娓╁害闃堝��)姣旇緝锛屼笉缃爣蹇椾綅
					 BOOLEAN isospi_wake_t,	//鑿婅姳閾惧敜閱掓椂闂达紝0: 10us锛�1: 20us
					 BOOLEAN adc_rst_sel,	//娴嬮噺绗�1涓�氶亾ADC澶嶄綅閫夋嫨锛�0:鎵�鏈夋祴閲忛�氶亾閮藉浣岮DC锛�1:鍙湁绗�1涓�氶亾澶嶄綅ADC锛屼箣鍚�2~n閫氶亾涓嶅浣岮DC
					 u8 time_sel_m,			//鐩戞祴妯″紡涓嬶紝瀹氭椂鐩戞祴鏃堕棿閰嶇疆锛屽畾鏃舵椂闂�=(TIME_SEL_M+1)*1s(2~64s)锛屾渶灏忛厤缃�兼槸1(2s)
                              	  	  	  	//杩欎釜鏃堕棿閰嶇疆鍜屽潎琛″懆鏈熼厤缃叡鐢�
					 u8 adc_init_t2,		//ADC娴嬮噺绗�2~n涓�氶亾ADC鍒濆鍖栨椂闂达紝(ADC_INIT_T2+1)*20us(20~320us)
					 u8 adc_init_t1,		//ADC娴嬮噺绗�1涓�氶亾ADC鍒濆鍖栨椂闂�(ADC_INIT_T1+1)*100us(100~1600us)
					 BOOLEAN trim2_en,		//淇皟2浣胯兘锛�1:浣胯兘锛�0:涓嶄娇鑳�
					 BOOLEAN trim1_en,		//淇皟1浣胯兘锛�1:浣胯兘锛�0:涓嶄娇鑳�
					 u8 err_t_sel_m,		//鐩戞祴妯″紡涓嬪彂鐢熷紓甯革紝涓婃姤鐨勯棿闅旀椂闂�
                               	   	   	    //00:200ms锛�01:400ms锛�10:600ms锛�11:800ms
					 u8 vd33t_on_m			//鐩戞祴妯″紡涓嬶紝VREF1鍜孷D33T鐨勫惎鍔ㄦ椂闂撮厤缃�(5+VD33T_ON_M*10)ms锛岃寖鍥存槸5~155ms
					 )
{
	BF8915A_set_cfgd_csel_m(nIC, configDReg,csel_m);				//璁剧疆鐩戞祴妯″紡涓嬶紝鐢垫睜鐢靛帇鐩戞祴姣旇緝閫夋嫨鐨勮緟鍔╁嚱鏁�
	BF8915A_set_cfgd_gsel_m(nIC, configDReg,gsel_m);				//璁剧疆鐩戞祴妯″紡涓嬶紝GPIO0~7姣旇緝閫夋嫨鐨勮緟鍔╁嚱鏁�
	BF8915A_set_cfgd_isospi_wake_t(nIC, configDReg,isospi_wake_t);	//璁剧疆鑿婅姳閾惧敜閱掓椂闂寸殑杈呭姪鍑芥暟
	BF8915A_set_cfgd_adc_rst_sel(nIC, configDReg,adc_rst_sel);		//璁剧疆娴嬮噺绗�1涓�氶亾ADC澶嶄綅閫夋嫨鐨勮緟鍔╁嚱鏁�
	BF8915A_set_cfgd_time_sel_m(nIC, configDReg,time_sel_m);		//璁剧疆鐩戞祴妯″紡涓嬶紝瀹氭椂鐩戞祴鏃堕棿閰嶇疆鐨勮緟鍔╁嚱鏁�
	BF8915A_set_cfgd_adc_init_t2(nIC, configDReg,adc_init_t2);		//璁剧疆ADC娴嬮噺绗�2~n涓�氶亾ADC鍒濆鍖栨椂闂寸殑杈呭姪鍑芥暟
	BF8915A_set_cfgd_adc_init_t1(nIC, configDReg,adc_init_t1);		//璁剧疆ADC娴嬮噺绗�1涓�氶亾ADC鍒濆鍖栨椂闂寸殑杈呭姪鍑芥暟
	BF8915A_set_cfgd_trim2_en(nIC, configDReg,trim2_en);			//璁剧疆淇皟2浣胯兘鐨勮緟鍔╁嚱鏁�
	BF8915A_set_cfgd_trim1_en(nIC, configDReg,trim1_en);			//璁剧疆淇皟1浣胯兘鐨勮緟鍔╁嚱鏁�
	BF8915A_set_cfgd_err_t_sel_m(nIC, configDReg,err_t_sel_m);		//璁剧疆鐩戞祴妯″紡涓嬪彂鐢熷紓甯革紝涓婃姤鐨勯棿闅旀椂闂寸殑杈呭姪鍑芥暟
	BF8915A_set_cfgd_vd33t_on_m(nIC, configDReg,vd33t_on_m);		//璁剧疆閰嶇疆瀵勫瓨鍣ㄧ粍D鍙橀噺鐨勮緟鍔╁嚱鏁�
}

/*!
    \brief      Write the BF8915A CFGRA(鍐橞F8915A鐨勯厤缃瘎瀛樺櫒缁凙)
		\param[in]  total_ic 	 : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configAData: A two dimensional array of the configuration data that will be written(灏嗚鍐欏叆閰嶇疆鏁版嵁鐨勪簩缁存暟缁�)
    \retval     none   	                  					  
*/
void BF8915A_wrcfga(u8 total_ic, ConfigA_TypeDef configAData[])
{
	u8 cmd[2] = {0x00 , 0x01};			//鍙戦�佸啓BF8915A鐨勯厤缃瘎瀛樺櫒缁凙鎸囦护鐨勯珮浣庡叡16浣�
	Reg_TypeDef configAArray[TOTAL_IC];	//瀹氫箟涓�涓粨鏋勪綋涓篟eg_TypeDef鐨勬暟鎹粨鏋凙鏁扮粍,鐢ㄤ簬瀛樺偍鎸囦护鏁版嵁
	u8 write_buffer[TOTAL_IC * 0x6U];	//瀵勫瓨鍣ㄧ粍涓湁6涓�8浣嶅瘎瀛樺櫒
	u8 write_count = 0;					//鏁扮粍璁℃暟鍣紝鏈�澶т负TOTAL_IC * 0x6U
	u8 c_ic = 0;						//褰撳墠姝ｅ湪鍐欏叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 current_ic = 0;					//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 dataTemp = 0;					//璁℃暟鍣紝姣�6娆′竴寰幆锛岀敤浜庤鏁版瘡涓瘎瀛樺櫒缁勫綋鍓嶅惊鐜埌鐨勫瘎瀛樺櫒
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//閰嶇疆姣忎釜8915A鐨勯厤缃瘎瀛樺櫒缁凙
	{
		BF8915A_set_cfgra(current_ic, 
						configAArray,
						configAData[current_ic].gpio_pd_en,		//鍐�0:GPIO鑴氫笅鎷変娇鑳斤紱1:GPIO鑴氫笅鎷夊叧闂�(榛樿)璇�0 :GPIO鑴氱殑鍊兼槸0 锛�1:GPIO鑴氱殑鍊兼槸1
						configAData[current_ic].gpio_analog_en,	//GPIO0~7妯″紡閫夋嫨 0:妯℃嫙杈撳叆 1:鏅�欸PIO
						configAData[current_ic].rsvd2,			//淇濈暀浣嶏紝姘歌繙缃�0
						configAData[current_ic].rsvd1,			//淇濈暀浣嶏紝姘歌繙缃�0
						configAData[current_ic].osr_sel,		//0:杩囬噰鏍风巼閫夋嫨64锛�128锛�256锛�512(鍥涚杩囬噰鏍风巼閫氳繃鎸囦护鐨凮SR閫夋嫨)
															  	//杩欎釜瀵勫瓨鍣ㄤ綅瑕佸拰鎸囦护鐨凮SR涓�璧烽厤缃�夋嫨ADC杩囬噰鏍风巼
						configAData[current_ic].refon,			//VREF1(鍩哄噯鐢靛帇)閰嶇疆  1 :VREF1淇濇寔涓婄數鐩村埌WDT婧㈠嚭  0 :ADC杞崲瀹屼箣鍚嶸REF1鍏抽棴 (榛樿)
						configAData[current_ic].uv_th,			//Under voltage Comparison Voltage(娆犲帇闃堝��)锛屾牴鎹疄闄呮儏鍐佃嚜琛岃瀹�
																//Under voltage threshold ADC Code(娆犵數鍘嬮槇鍊兼ā鏁拌浆鎹㈠櫒浠ｇ爜): uv_th = 3000 * 65535U / 5000U. LSB = 0.0001 ---(3V)
						configAData[current_ic].ov_th 			//Over  voltage Comparison Voltage(杩囧帇闃堝��)锛屾牴鎹疄闄呮儏鍐佃嚜琛岃瀹�
																//Over  voltage threshold ADC Code(杩囩數鍘嬮槇鍊兼ā鏁拌浆鎹㈠櫒浠ｇ爜): ov_th = 4100 * 65535U / 5000U. LSB = 0.0001 ---(4.1V)
		);
	}
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负缁欐瘡涓�8915A鑺墖鍙戦�侀厤缃瘎瀛樺櫒缁凙鎸囦护
	{
		if (isospi_reverse == TRUE)		//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			c_ic = current_ic;			//閫夋嫨姝ｅ簭灏嗛厤缃瘎瀛樺櫒缁凙鎸囦护鏀惧叆鏁扮粍
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗛厤缃瘎瀛樺櫒缁凙鎸囦护鏀惧叆鏁扮粍
		}

		for (dataTemp = 0; dataTemp<6; dataTemp++)//鑻sospi涓�鐩翠负FALSE锛屽垯姝ゅ惊鐜皢閰嶇疆瀵勫瓨鍣ㄧ粍A涓厤缃ソ鐨勬暟缁勬暟鎹竴涓�鍐欏叆鍙戦�佹暟缁勪腑锛屽苟涓旀槸閫氳繃鍊掑簭鍙戦�佺粰姣忎釜8915A鑺墖
		{
			write_buffer[write_count] = configAArray[c_ic].tx_data[dataTemp];
			write_count++;
		}
	}
	BF8915A_write(total_ic, cmd, write_buffer);//鍐橞F8915A鍛戒护鍜屽啓鏈夋晥杞借嵎鏁版嵁鐨勯�氱敤鍑芥暟
}

/*!
    \brief      Write the BF8915A CFGRB(鍐橞F8915A鐨勯厤缃瘎瀛樺櫒缁凚)
		\param[in]  total_ic 	 : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configBData: A two dimensional array of the configuration data that will be written(灏嗚鍐欏叆閰嶇疆鏁版嵁鐨勪簩缁存暟缁�)
    \retval     none   	                  					  
*/
void BF8915A_wrcfgb(u8 total_ic, ConfigB_TypeDef configBData[])
{
	u8 cmd[2] = {0x00 , 0x02};			//鍙戦�佸啓BF8915A鐨勯厤缃瘎瀛樺櫒缁凚鎸囦护鐨勯珮浣庡叡16浣�
	Reg_TypeDef configBArray[TOTAL_IC];	//瀹氫箟涓�涓粨鏋勪綋涓篟eg_TypeDef鐨勬暟鎹粨鏋凚鏁扮粍,鐢ㄤ簬瀛樺偍鎸囦护鏁版嵁
	u8 write_buffer[TOTAL_IC * 0x6U];	//瀵勫瓨鍣ㄧ粍涓殑6涓�8浣嶅瘎瀛樺櫒
	u8 write_count = 0;					//鏁扮粍璁℃暟鍣紝鏈�澶т负TOTAL_IC * 0x6U
	u8 c_ic = 0;						//褰撳墠姝ｅ湪鍐欏叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 current_ic = 0;					//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 dataTemp = 0;					//璁℃暟鍣紝姣�6娆′竴寰幆锛岀敤浜庤鏁版瘡涓瘎瀛樺櫒缁勫綋鍓嶅惊鐜埌鐨勫瘎瀛樺櫒
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//閰嶇疆姣忎釜8915A鐨勯厤缃瘎瀛樺櫒缁凚
	{
		BF8915A_set_cfgrb(current_ic, 
						configBArray,
						configBData[current_ic].dcc,		//鏀剧數鐢垫睜寮�鍏�1:鎵撳紑Cell x 鐨勫紑鍏�  0:鍏抽棴Cell x 鐨勫紑鍏� (榛樿)
						configBData[current_ic].bl_duty_sel,//涓诲姩鍧囪　鍗犵┖姣旈�夋嫨  鍗犵┖姣�=(BL_DUTY_SEL+1)*10%锛岄厤缃寖鍥�10%~100%锛屽綋澶т簬9鏃舵槸100%銆�
						configBData[current_ic].dcto,		//鏀剧數瀹氭椂婧㈠嚭鍊奸厤缃�
						configBData[current_ic].vuv_bl_th,	//鍧囪　娆犲帇闃堝��
						configBData[current_ic].dtmen		//鍦ㄨ嚜鍔ㄥ潎琛′笅锛屽鏋淒TMEN=1锛屽綋娆犲帇(灏忎簬VUV_BL)鍏抽棴鏀剧數寮�鍏筹紝濡傛灉DTMEN=0锛屼笉杩涜娆犲帇姣旇緝
						);
	}	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负缁欐瘡涓�8915A鑺墖鍙戦�侀厤缃瘎瀛樺櫒缁凚鎸囦护
	{
		if (isospi_reverse == TRUE)							//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			c_ic = current_ic;								//閫夋嫨姝ｅ簭灏嗛厤缃瘎瀛樺櫒缁凚鎸囦护鏀惧叆鏁扮粍
		}
		else
		{
			c_ic = total_ic - current_ic - 1;				//閫夋嫨鍊掑簭灏嗛厤缃瘎瀛樺櫒缁凚鎸囦护鏀惧叆鏁扮粍
		}

		for (dataTemp = 0; dataTemp<6; dataTemp++)			//鑻sospi涓�鐩翠负FALSE锛屽垯姝ゅ惊鐜皢閰嶇疆瀵勫瓨鍣ㄧ粍B涓厤缃ソ鐨勬暟缁勬暟鎹竴涓�鍐欏叆鍙戦�佹暟缁勪腑锛屽苟涓旀槸閫氳繃鍊掑簭鍙戦�佺粰姣忎釜8915A鑺墖
		{
			write_buffer[write_count] = configBArray[c_ic].tx_data[dataTemp];
			write_count++;
		}
	}
	BF8915A_write(total_ic, cmd, write_buffer);//鍐橞F8915A鍛戒护鍜屽啓鏈夋晥杞借嵎鏁版嵁鐨勯�氱敤鍑芥暟
}

/*!
    \brief      Write the BF8915A CFGRC(鍐橞F8915A鐨勯厤缃瘎瀛樺櫒缁凜)
		\param[in]  total_ic 	 : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configCData: A two dimensional array of the configuration data that will be written(灏嗚鍐欏叆閰嶇疆鏁版嵁鐨勪簩缁存暟缁�)
    \retval     none   	                  					  
*/
void BF8915A_wrcfgc(u8 total_ic, ConfigC_TypeDef configCData[])
{
	u8 cmd[2] = {0x00 , 0x03};			//鍙戦�佸啓BF8915A鐨勯厤缃瘎瀛樺櫒缁凜鎸囦护鐨勯珮浣庡叡16浣�
	Reg_TypeDef configCArray[TOTAL_IC];	//瀹氫箟涓�涓粨鏋勪綋涓篟eg_TypeDef鐨勬暟鎹粨鏋凜鏁扮粍,鐢ㄤ簬瀛樺偍鎸囦护鏁版嵁
	u8 write_buffer[TOTAL_IC * 0x6U];	//瀵勫瓨鍣ㄧ粍涓殑6涓�8浣嶅瘎瀛樺櫒
	u8 write_count = 0;					//鏁扮粍璁℃暟鍣紝鏈�澶т负TOTAL_IC * 0x6U
	u8 c_ic = 0;						//褰撳墠姝ｅ湪鍐欏叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 current_ic = 0;					//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 data = 0;						//璁℃暟鍣紝姣�6娆′竴寰幆锛岀敤浜庤鏁版瘡涓瘎瀛樺櫒缁勫綋鍓嶅惊鐜埌鐨勫瘎瀛樺櫒

	for (current_ic = 0; current_ic<total_ic; current_ic++)//閰嶇疆姣忎釜8915A鐨勯厤缃瘎瀛樺櫒缁凜
	{
		BF8915A_set_cfgrc(current_ic, 
							configCArray,
							configCData[current_ic].vuv_m_th,//鐩戞祴妯″紡涓嬶紝娆犲帇闃堝��
							configCData[current_ic].vov_m_th,//鐩戞祴妯″紡涓嬶紝杩囧帇闃堝��
							configCData[current_ic].itov_m_th,//鐩戞祴妯″紡涓嬶紝鍐呴儴杩囨俯闃堝��
							configCData[current_ic].extov_m_th//鐩戞祴妯″紡涓嬶紝澶栭儴杩囨俯闃堝��
							);
	}

	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负缁欐瘡涓�8915A鑺墖鍙戦�侀厤缃瘎瀛樺櫒缁凜鎸囦护
	{
		if (isospi_reverse == TRUE)		//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			c_ic = current_ic;			//閫夋嫨姝ｅ簭灏嗛厤缃瘎瀛樺櫒缁凜鎸囦护鏀惧叆鏁扮粍
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗛厤缃瘎瀛樺櫒缁凜鎸囦护鏀惧叆鏁扮粍
		}

		for (data = 0; data<6; data++)	//鑻sospi涓�鐩翠负FALSE锛屽垯姝ゅ惊鐜皢閰嶇疆瀵勫瓨鍣ㄧ粍C涓厤缃ソ鐨勬暟缁勬暟鎹竴涓�鍐欏叆鍙戦�佹暟缁勪腑锛屽苟涓旀槸閫氳繃鍊掑簭鍙戦�佺粰姣忎釜8915A鑺墖
		{
			write_buffer[write_count] = configCArray[c_ic].tx_data[data];
			write_count++;
		}
	}
	BF8915A_write(total_ic, cmd, write_buffer);//鍐橞F8915A鍛戒护鍜屽啓鏈夋晥杞借嵎鏁版嵁鐨勯�氱敤鍑芥暟
}

/*!
    \brief      Write the BF8915A CFGRD(鍐橞F8915A鐨勯厤缃瘎瀛樺櫒缁凞)
		\param[in]  total_ic 	 : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDData: A two dimensional array of the configuration data that will be written(灏嗚鍐欏叆閰嶇疆鏁版嵁鐨勪簩缁存暟缁�)
    \retval     none   	                  					  
*/
void BF8915A_wrcfgd(u8 total_ic, ConfigD_TypeDef configDData[])
{
	u8 cmd[2] = {0x00 , 0x04};			//鍙戦�佸啓BF8915A鐨勯厤缃瘎瀛樺櫒缁凞鎸囦护鐨勯珮浣庡叡16浣�
	Reg_TypeDef configDArray[TOTAL_IC];	//瀹氫箟涓�涓粨鏋勪綋涓篟eg_TypeDef鐨勬暟鎹粨鏋凞鏁扮粍,鐢ㄤ簬瀛樺偍鎸囦护鏁版嵁
	u8 write_buffer[TOTAL_IC * 0x6U];	//瀵勫瓨鍣ㄧ粍涓殑6涓�8浣嶅瘎瀛樺櫒
	u8 write_count = 0;					//鏁扮粍璁℃暟鍣紝鏈�澶т负TOTAL_IC * 0x6U
	u8 c_ic = 0;						//褰撳墠姝ｅ湪鍐欏叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 current_ic = 0;					//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 data = 0;						//璁℃暟鍣紝姣�6娆′竴寰幆锛岀敤浜庤鏁版瘡涓瘎瀛樺櫒缁勫綋鍓嶅惊鐜埌鐨勫瘎瀛樺櫒
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//閰嶇疆姣忎釜8915A鐨勯厤缃瘎瀛樺櫒缁凞
	{
		BF8915A_set_cfgrd(current_ic, 
							configDArray,
							configDData[current_ic].csel_m, 		//鐩戞祴妯″紡涓嬶紝鐢垫睜鐢靛帇鐩戞祴姣旇緝閫夋嫨 1:鐩稿簲鐨勭數姹犲氨浼氬湪鐩戞祴妯″紡涓嬭繘琛岃繃娆犲帇姣旇緝锛岀疆鏍囧織浣�
																  	//0:鐩稿簲鐨勭數姹犲氨浼氬湪鐩戞祴妯″紡涓嬩笉杩涜杩囨瑺鍘嬫瘮杈冿紝涓嶇疆鏍囧織浣�
							configDData[current_ic].gsel_m,			//鐩戞祴妯″紡涓嬶紝GPIO0~7姣旇緝閫夋嫨
							configDData[current_ic].isospi_wake_t,	//isospi(鑿婅姳閾維PI)鍞ら啋鏃堕棿锛�0:10us		1:20us
							configDData[current_ic].adc_rst_sel,	//娴嬮噺绗�1涓�氶亾ADC澶嶄綅閫夋嫨锛�0:鎵�鏈夋祴閲忛�氶亾閮藉浣岮DC锛�1:鍙湁绗�1涓�氶亾澶嶄綅ADC锛屼箣鍚�2~n閫氶亾涓嶅浣岮DC
							configDData[current_ic].time_sel_m,		//鐩戞祴妯″紡涓嬶紝瀹氭椂鐩戞祴鏃堕棿閰嶇疆锛屽畾鏃舵椂闂�=(TIME_SEL_M+1)*1s(2~64s)锛屾渶灏忛厤缃�兼槸1(2s)
							configDData[current_ic].adc_init_t2,	//ADC娴嬮噺绗�2~n涓�氶亾ADC鍒濆鍖栨椂闂达紝(ADC_INIT_T2+1)*20us(20~320us)
							configDData[current_ic].adc_init_t1,	//ADC娴嬮噺绗�1涓�氶亾ADC鍒濆鍖栨椂闂�(ADC_INIT_T1+1)*100us(100~1600us)
							configDData[current_ic].trim2_en,		//淇皟2浣胯兘锛�1:浣胯兘锛�0:涓嶄娇鑳�
							configDData[current_ic].trim1_en,		//淇皟1浣胯兘锛�1:浣胯兘锛�0:涓嶄娇鑳�
							configDData[current_ic].err_t_sel_m,	//鐩戞祴妯″紡涓嬪彂鐢熷紓甯革紝涓婃姤鐨勯棿闅旀椂闂�  00:200ms锛�01:400ms锛�10:600ms锛�11:800ms
							configDData[current_ic].vd33t_on_m		//鐩戞祴妯″紡涓嬶紝VREF1鍜孷D33T鐨勫惎鍔ㄦ椂闂撮厤缃�(5+VD33T_ON_M*10)ms锛岃寖鍥存槸5~155ms
							);
	}
	
	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负缁欐瘡涓�8915A鑺墖鍙戦�侀厤缃瘎瀛樺櫒缁凞鎸囦护
	{
		if (isospi_reverse == TRUE)		//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			c_ic = current_ic;			//閫夋嫨姝ｅ簭灏嗛厤缃瘎瀛樺櫒缁凞鎸囦护鏀惧叆鏁扮粍
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗙數閰嶇疆瀵勫瓨鍣ㄧ粍D鎸囦护鏀惧叆鏁扮粍
		}

		for (data = 0; data<6; data++)	//鑻sospi涓�鐩翠负FALSE锛屽垯姝ゅ惊鐜皢閰嶇疆瀵勫瓨鍣ㄧ粍D涓厤缃ソ鐨勬暟缁勬暟鎹竴涓�鍐欏叆鍙戦�佹暟缁勪腑锛屽苟涓旀槸閫氳繃鍊掑簭鍙戦�佺粰姣忎釜8915A鑺墖
		{
			write_buffer[write_count] = configDArray[c_ic].tx_data[data];//鍊掑簭鍐欏叆鎸囦护
			write_count++;
		}
	}
	BF8915A_write(total_ic, cmd, write_buffer);//鍐橞F8915A鍛戒护鍜屽啓鏈夋晥杞借嵎鏁版嵁鐨勯�氱敤鍑芥暟
}

/*!
    \brief      Reads the BF8915A CFGRA register(璇籅F8915A鐨勯厤缃瘎瀛樺櫒缁凙)
		\param[in]  total_ic 	 : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configAData: A two dimensional array that the function stores the read configuration data(璇ュ嚱鏁板瓨鍌ㄨ鍙栫殑鏄厤缃暟鎹殑浜岀淮鏁扮粍)
    \retval     pec_error
*/
u8 BF8915A_rdcfga(u8 total_ic, ConfigA_TypeDef configAData[])
{
	u8 cmd[2]= {0x00U, 0x05U};			//鍙戦�佽BF8915A鐨勯厤缃瘎瀛樺櫒缁凙鎸囦护鐨勯珮浣庡叡16浣�
	Reg_TypeDef configAArray[TOTAL_IC];	//瀹氫箟涓�涓粨鏋勪綋涓篟eg_TypeDef鐨勬暟鎹粨鏋凙鏁扮粍,鐢ㄤ簬瀛樺偍鎸囦护鏁版嵁
	u8 read_buffer[256];				//瀛樺偍SPI绔彛璇诲彇鐨勬暟鎹� 鐨勬暟缁�
	u8 pec_error = 0;					//妫�鏌ユ閿欑爜
	u16 data_pec;						//鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒缁凙涓殑PEC(妫�閿欑爜)
	u16 calc_pec;						//璁＄畻鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒缁凙涓寚浠ゆ暟鎹殑PEC(妫�閿欑爜)
	u8 c_ic = 0;						//褰撳墠姝ｅ湪璇诲叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 current_ic = 0;					//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 byte = 0;						//璁℃暟鍣紝姣�8娆′竴寰幆锛岀敤浜庤鏁版瘡涓瘎瀛樺櫒缁凙褰撳墠寰幆鍒扮殑瀵勫瓨鍣ㄤ互鍙婂畠鐨凱EC(妫�閿欑爜)

	pec_error = BF8915A_read(total_ic, cmd, read_buffer);//鎵ц鍐橞F8915A鍛戒护鍜岃鏁版嵁锛屽苟鍥炰紶妫�鏌ラ敊璇爜鏄惁姝ｇ‘

	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负璇诲嚭姣忎釜8915A鑺墖閰嶇疆瀵勫瓨鍣ㄧ粍A涓殑鎸囦护锛屽苟妫�鏌ユ槸鍚︽湁閿欒
	{
		if (isospi_reverse == FALSE)	//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			c_ic = current_ic;			//閫夋嫨姝ｅ簭灏嗛厤缃瘎瀛樺櫒缁凙涓殑鎸囦护鏀惧叆鏁扮粍
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗛厤缃瘎瀛樺櫒缁凙涓殑鎸囦护鏀惧叆鏁扮粍
		}

		for (byte=0; byte<8; byte++)	//鑻sospi涓�鐩翠负FALSE锛屽垯姝ゅ惊鐜皢鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒涓殑鏁版嵁鍜屽畠鐨�2瀛楄妭PEC(妫�閿欑爜)鍐欏叆鎺ユ敹鏁扮粍涓紝鎺ユ敹鐨勬槸鍊掑簭鐨�8915A鑺墖
		{
			configAArray[c_ic].rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		calc_pec = Bf8915pec15_calc(6,&read_buffer[8*current_ic]);//璁＄畻鎺ユ敹鍥炴潵鐨勫瘎瀛樺櫒缁�(6涓�8浣嶅瘎瀛樺櫒)涓殑PEC(妫�閿欑爜)
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);//灏唕ead_buffer[]涓帴鏀跺埌鐨凱EC(妫�閿欑爜)鎸夐珮浣庡叡16浣嶅瓨鍏ata_pec涓�
		if (calc_pec != data_pec )//妫�鏌ユ閿欑爜鏄惁鍖归厤
		{
			configAArray[c_ic].rx_pec_match = 1;//妫�閿欑爜涓嶆纭�
		}
		else configAArray[c_ic].rx_pec_match = 0;//妫�閿欑爜姝ｇ‘
	}
	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负瀛樺叆姣忎釜8915A鑺墖閰嶇疆瀵勫瓨鍣ㄧ粍A涓殑鎸囦护
	{
		configAData[current_ic].gpio_pd_en = configAArray[current_ic].rx_data[0];		//CFGAR0鐨�8浣嶏紝GPIO涓嬫媺浣胯兘锛屾瘡涓�浣嶅搴斾竴涓狪O鍙�
		configAData[current_ic].gpio_analog_en = configAArray[current_ic].rx_data[1];	//CFGAR1鐨�8浣嶏紝GPIO0~7妯″紡閫夋嫨
		configAData[current_ic].rsvd2 = (BOOLEAN)((configAArray[current_ic].rx_data[2] & 0x08U) == 0x08U);//淇濈暀浣嶏紝姘歌繙缃�0
		configAData[current_ic].rsvd1 = (BOOLEAN)((configAArray[current_ic].rx_data[2] & 0x04U) == 0x04U);//淇濈暀浣嶏紝姘歌繙缃�0
		configAData[current_ic].osr_sel = (BOOLEAN)((configAArray[current_ic].rx_data[2] & 0x02U) == 0x02U);//CFGAR2鐨勭2浣�,ADC杩囬噰鏍风巼閫夋嫨
		configAData[current_ic].refon = (BOOLEAN)((configAArray[current_ic].rx_data[2] & 0x01U) == 0x01U);//CFGAR2鐨勭1浣�,VREF1(鍩哄噯鐢靛帇)閰嶇疆
		configAData[current_ic].uv_th = (u16)(configAArray[current_ic].rx_data[3]) + (((u16)((configAArray[current_ic].rx_data[4])&0x0FU)) << 0x8U);
		                                                                                    //CFGAR3鐨�8浣嶅拰CFGAR4鐨勪綆4浣�,灏咰FGAR1鐨勪綆4浣嶆斁鍏�12~9浣嶏紝娆犲帇闃堝��
		configAData[current_ic].ov_th = (((u16)configAArray[current_ic].rx_data[5]) << 0x4U)  + (((configAArray[current_ic].rx_data[4])&0xF0U) >> 0x4U);
		                                                                                    //CFGAR5鐨�8浣嶅拰CFGAR4鐨勯珮4浣�,灏咰FGAR4鐨勯珮4浣嶆斁鍏�12~9浣嶏紝杩囧帇闃堝��
		configAData[current_ic].rx_pec_match = configAArray[current_ic].rx_pec_match;//姣忎釜閰嶇疆瀵勫瓨鍣ㄧ粍A涓殑妫�閿欑爜鏄惁鍖归厤
	}
	return(pec_error);//鍥炰紶妫�鏌ユ閿欑爜鐨勫��
}

/*!
    \brief      Reads the BF8915A CFGRB register(璇籅F8915A鐨勯厤缃瘎瀛樺櫒缁凚)
		\param[in]  total_ic 	 : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configBData: A two dimensional array that the function stores the read configuration data(璇ュ嚱鏁板瓨鍌ㄨ鍙栫殑鏄厤缃暟鎹殑浜岀淮鏁扮粍)
    \retval     pec_error   	                  					  
*/
u8 BF8915A_rdcfgb(u8 total_ic, ConfigB_TypeDef configBData[])
{
	u8 cmd[2]= {0x00 , 0x06};			//鍙戦�佽BF8915A鐨勯厤缃瘎瀛樺櫒缁凚鎸囦护鐨勯珮浣庡叡16浣�
	Reg_TypeDef configBArray[TOTAL_IC];	//瀹氫箟涓�涓粨鏋勪綋涓篟eg_TypeDef鐨勬暟鎹粨鏋凚鏁扮粍,鐢ㄤ簬瀛樺偍鎸囦护鏁版嵁
	u8 read_buffer[256];				//瀛樺偍SPI绔彛璇诲彇鐨勬暟鎹� 鐨勬暟缁�
	u8 pec_error = 0;					//妫�鏌ユ閿欑爜
	u16 data_pec;						//鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒缁凚涓殑PEC(妫�閿欑爜)
	u16 calc_pec;						//璁＄畻鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒缁凚涓寚浠ゆ暟鎹殑PEC(妫�閿欑爜)
	u8 c_ic = 0;						//褰撳墠姝ｅ湪璇诲叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 current_ic = 0;					//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 byte=0; 							//璁℃暟鍣紝姣�8娆′竴寰幆锛岀敤浜庤鏁版瘡涓瘎瀛樺櫒缁凚涓綋鍓嶅惊鐜埌鐨勫瘎瀛樺櫒浠ュ強瀹冪殑PEC(妫�閿欑爜)

	pec_error = BF8915A_read(total_ic, cmd, read_buffer);//鎵ц鍐橞F8915A鍛戒护鍜岃鏁版嵁锛屽苟鍥炰紶妫�鏌ラ敊璇爜鏄惁姝ｇ‘

	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负璇诲嚭姣忎釜8915A鑺墖閰嶇疆瀵勫瓨鍣ㄧ粍B涓殑鎸囦护锛屽苟妫�鏌ユ槸鍚︽湁閿欒
	{
		if (isospi_reverse == FALSE)	//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			c_ic = current_ic;			//閫夋嫨姝ｅ簭灏嗛厤缃瘎瀛樺櫒缁凚涓殑鎸囦护鏀惧叆鏁扮粍
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗛厤缃瘎瀛樺櫒缁凚涓殑鎸囦护鏀惧叆鏁扮粍
		}

		for (byte=0; byte<8; byte++)	//鑻sospi涓�鐩翠负FALSE锛屽垯姝ゅ惊鐜皢鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒涓殑鏁版嵁鍜屽畠鐨�2瀛楄妭PEC(妫�閿欑爜)鍐欏叆鎺ユ敹鏁扮粍涓紝鎺ユ敹鐨勬槸鍊掑簭鐨�8915A鑺墖
		{
			configBArray[c_ic].rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		calc_pec = Bf8915pec15_calc(6,&read_buffer[8*current_ic]);//璁＄畻鎺ユ敹鍥炴潵鐨勫瘎瀛樺櫒缁�(6涓�8浣嶅瘎瀛樺櫒)涓殑PEC(妫�閿欑爜)
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);//灏唕ead_buffer[]涓帴鏀跺埌鐨凱EC(妫�閿欑爜)鎸夐珮浣庡叡16浣嶅瓨鍏ata_pec涓�
		if (calc_pec != data_pec )		//妫�鏌ユ閿欑爜鏄惁鍖归厤
		{
			configBArray[c_ic].rx_pec_match = 1;//妫�閿欑爜涓嶆纭�
		}
		else configBArray[c_ic].rx_pec_match = 0;//妫�閿欑爜姝ｇ‘
	}
	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负瀛樺叆姣忎釜8915A鑺墖閰嶇疆瀵勫瓨鍣ㄧ粍B涓殑鎸囦护
	{
		configBData[current_ic].dcc = (((u16)configBArray[current_ic].rx_data[1]) << 0x8U) + configBArray[current_ic].rx_data[0];
		                                                                                          	//CFGBR0鐨�8浣嶅拰CFGBR1鐨�8浣�,灏咰FGBR1鐨�8浣嶆斁鍏ラ珮8浣嶏紝鏀剧數鐢垫睜寮�鍏�
		configBData[current_ic].bl_duty_sel = (configBArray[current_ic].rx_data[2] & 0xF0U) >> 0x4U;//CFGBR2鐨勯珮4浣�,涓诲姩鍧囪　鍗犵┖姣旈�夋嫨
		configBData[current_ic].dcto = configBArray[current_ic].rx_data[2] & 0x0FU;					//CFGBR2鐨勪綆4浣�,鏀剧數瀹氭椂婧㈠嚭鍊奸厤缃�
		configBData[current_ic].vuv_bl_th = (u16)configBArray[current_ic].rx_data[3] + (((u16)(configBArray[current_ic].rx_data[4] & 0x0F)) << 0x8U);
		                                                                                         	 //CFGBR3鐨�8浣嶅拰CFGBR4鐨勪綆4浣�,灏咰FGBR1鐨勪綆4浣嶆斁鍏�12~9浣嶏紝鍧囪　娆犲帇闃堝��
		configBData[current_ic].dtmen = (BOOLEAN)((configBArray[current_ic].rx_data[4] & 0x10U) == 0x10U);//CFGBR4鐨勭5浣�,浣胯兘鏀剧數鐩戞祴鍣�
		configBData[current_ic].rx_pec_match = configBArray[current_ic].rx_pec_match;				//姣忎釜閰嶇疆瀵勫瓨鍣ㄧ粍B涓殑妫�閿欑爜鏄惁鍖归厤
	}
	return(pec_error);//鍥炰紶妫�鏌ユ閿欑爜鐨勫��
}

/*!
    \brief      Reads the BF8915A CFGRC register(璇籅F8915A鐨勯厤缃瘎瀛樺櫒缁凜)
		\param[in]  total_ic 	 : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configCData: A two dimensional array that the function stores the read configuration data(璇ュ嚱鏁板瓨鍌ㄨ鍙栫殑鏄厤缃暟鎹殑浜岀淮鏁扮粍)
    \retval     none   	                  					  
*/
u8 BF8915A_rdcfgc(u8 total_ic, ConfigC_TypeDef configCData[])
{
	u8 cmd[2]= {0x00 , 0x07};				//鍙戦�佽BF8915A鐨勯厤缃瘎瀛樺櫒缁凜鎸囦护鐨勯珮浣庡叡16浣�
	Reg_TypeDef configCArray[TOTAL_IC];		//瀹氫箟涓�涓粨鏋勪綋涓篟eg_TypeDef鐨勬暟鎹粨鏋凜鏁扮粍,鐢ㄤ簬瀛樺偍鎸囦护鏁版嵁
	u8 read_buffer[256];					//瀛樺偍SPI绔彛璇诲彇鐨勬暟鎹� 鐨勬暟缁�
	u8 pec_error = 0;						//妫�鏌ユ閿欑爜
	u16 data_pec;							//鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒缁凜涓殑PEC(妫�閿欑爜)
	u16 calc_pec;							//璁＄畻鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒缁凜涓寚浠ゆ暟鎹殑PEC(妫�閿欑爜)
	u8 c_ic = 0;							//褰撳墠姝ｅ湪璇诲叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 current_ic = 0;						//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 byte=0;								//璁℃暟鍣紝姣�8娆′竴寰幆锛岀敤浜庤鏁版瘡涓瘎瀛樺櫒缁凚涓綋鍓嶅惊鐜埌鐨勫瘎瀛樺櫒浠ュ強瀹冪殑PEC(妫�閿欑爜)

	pec_error = BF8915A_read(total_ic, cmd, read_buffer);//鎵ц鍐橞F8915A鍛戒护鍜岃鏁版嵁锛屽苟鍥炰紶妫�鏌ラ敊璇爜鏄惁姝ｇ‘

	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负璇诲嚭姣忎釜8915A鑺墖閰嶇疆瀵勫瓨鍣ㄧ粍C涓殑鎸囦护锛屽苟妫�鏌ユ槸鍚︽湁閿欒
	{
		if (isospi_reverse == FALSE)		//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			c_ic = current_ic;				//閫夋嫨姝ｅ簭灏嗛厤缃瘎瀛樺櫒缁凜涓殑鎸囦护鏀惧叆鏁扮粍
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗛厤缃瘎瀛樺櫒缁凜涓殑鎸囦护鏀惧叆鏁扮粍
		}

		for (byte=0; byte<8; byte++)//鑻sospi涓�鐩翠负FALSE锛屽垯姝ゅ惊鐜皢鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒涓殑鏁版嵁鍜屽畠鐨�2瀛楄妭PEC(妫�閿欑爜)鍐欏叆鎺ユ敹鏁扮粍涓紝鎺ユ敹鐨勬槸鍊掑簭鐨�8915A鑺墖
		{
			configCArray[c_ic].rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		calc_pec = Bf8915pec15_calc(6,&read_buffer[8*current_ic]);//璁＄畻鎺ユ敹鍥炴潵鐨勫瘎瀛樺櫒缁�(6涓�8浣嶅瘎瀛樺櫒)涓殑PEC(妫�閿欑爜)
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);//灏唕ead_buffer[]涓帴鏀跺埌鐨凱EC(妫�閿欑爜)鎸夐珮浣庡叡16浣嶅瓨鍏ata_pec涓�
		if (calc_pec != data_pec )//妫�鏌ユ閿欑爜鏄惁鍖归厤
		{
			configCArray[c_ic].rx_pec_match = 1;//妫�閿欑爜涓嶆纭�
		}
		else configCArray[c_ic].rx_pec_match = 0;//妫�閿欑爜姝ｇ‘
	}
	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负瀛樺叆姣忎釜8915A鑺墖閰嶇疆瀵勫瓨鍣ㄧ粍C涓殑鎸囦护
	{
		configCData[current_ic].vuv_m_th = configCArray[current_ic].rx_data[0] + (((u16)(configCArray[current_ic].rx_data[1] & 0x0F)) << 0x8U);
		                                                                                          //CFGCR0鐨�8浣嶅拰CFGCR1鐨勪綆4浣�,灏咰FGCR1鐨勪綆4浣嶆斁鍏�12~9浣嶏紝鐩戞祴妯″紡涓嬶紝娆犲帇闃堝��
		configCData[current_ic].vov_m_th = (((u16)configCArray[current_ic].rx_data[2]) << 0x4U) + ((configCArray[current_ic].rx_data[1] & 0xF0) >> 0x04U);
		                                                                    //CFGCR1鐨勯珮4浣嶅拰CFGCR2鐨�8浣�,灏咰FGCR1楂�4浣嶆斁鍏ヤ綆4浣嶏紝灏咰FGCR2鐨�8浣嶆斁鍏�12~5浣嶏紝鐩戞祴妯″紡涓嬶紝杩囧帇闃堝��
		configCData[current_ic].itov_m_th  = configCArray[current_ic].rx_data[3] + (((u16)(configCArray[current_ic].rx_data[4] & 0x0F)) << 0x8U);
		                                                                                          //CFGBC3鐨�8浣嶅拰CFGCR1鐨勪綆4浣�,灏咰FGCR4鐨勪綆4浣嶆斁鍏�12~9浣嶏紝鐩戞祴妯″紡涓嬶紝鍐呴儴杩囨俯闃堝��
		configCData[current_ic].extov_m_th = (((u16)configCArray[current_ic].rx_data[5]) << 0x4U) + ((configCArray[current_ic].rx_data[4] & 0xF0) >> 0x04U);
		                                                                    //CFGCR4鐨勯珮4浣嶅拰CFGCR5鐨�8浣�,灏咰FGCR4楂�4浣嶆斁鍏ヤ綆4浣嶏紝灏咰FGCR5鐨�8浣嶆斁鍏�12~5浣嶏紝鐩戞祴妯″紡涓嬶紝澶栭儴杩囨俯闃堝��
		configCData[current_ic].rx_pec_match = configCArray[current_ic].rx_pec_match;//姣忎釜閰嶇疆瀵勫瓨鍣ㄧ粍C涓殑妫�閿欑爜鏄惁鍖归厤
	}
	return(pec_error);//鍥炰紶妫�鏌ユ閿欑爜鐨勫��
}

/*!
    \brief      Reads the BF8915A CFGRD register(璇籅F8915A鐨勯厤缃瘎瀛樺櫒缁凞)
		\param[in]  total_ic 	 : Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  configDData: A two dimensional array that the function stores the read configuration data(璇ュ嚱鏁板瓨鍌ㄨ鍙栫殑鏄厤缃暟鎹殑浜岀淮鏁扮粍)
    \retval     none   	                  					  
*/
u8 BF8915A_rdcfgd(u8 total_ic, ConfigD_TypeDef configDData[])
{
	u8 cmd[2]= {0x00 , 0x08};			//鍙戦�佽BF8915A鐨勯厤缃瘎瀛樺櫒缁凞鎸囦护鐨勯珮浣庡叡16浣�
	Reg_TypeDef configDArray[TOTAL_IC];	//瀹氫箟涓�涓粨鏋勪綋涓篟eg_TypeDef鐨勬暟鎹粨鏋凞鏁扮粍,鐢ㄤ簬瀛樺偍鎸囦护鏁版嵁
	u8 read_buffer[256];				//瀛樺偍SPI绔彛璇诲彇鐨勬暟鎹� 鐨勬暟缁�
	u8 pec_error = 0;					//妫�鏌ユ閿欑爜
	u16 data_pec;						//鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒缁凞涓殑PEC(妫�閿欑爜)
	u16 calc_pec;						//璁＄畻鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒缁凞涓寚浠ゆ暟鎹殑PEC(妫�閿欑爜)
	u8 c_ic = 0;						//褰撳墠姝ｅ湪璇诲叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 current_ic = 0;					//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 byte=0; 							//璁℃暟鍣紝姣�8娆′竴寰幆锛岀敤浜庤鏁版瘡涓瘎瀛樺櫒缁凚涓綋鍓嶅惊鐜埌鐨勫瘎瀛樺櫒浠ュ強瀹冪殑PEC(妫�閿欑爜)

	pec_error = BF8915A_read(total_ic, cmd, read_buffer);//鎵ц鍐橞F8915A鍛戒护鍜岃鏁版嵁锛屽苟鍥炰紶妫�鏌ラ敊璇爜鏄惁姝ｇ‘

	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负璇诲嚭姣忎釜8915A鑺墖閰嶇疆瀵勫瓨鍣ㄧ粍D涓殑鎸囦护锛屽苟妫�鏌ユ槸鍚︽湁閿欒
	{
		if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
		{
			c_ic = current_ic;//閫夋嫨姝ｅ簭灏嗛厤缃瘎瀛樺櫒缁凞涓殑鎸囦护鏀惧叆鏁扮粍
		}
		else
		{
			c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗛厤缃瘎瀛樺櫒缁凞涓殑鎸囦护鏁扮粍
		}

		for (byte=0; byte<8; byte++)//鑻sospi涓�鐩翠负FALSE锛屽垯姝ゅ惊鐜皢鏀跺埌鐨勬瘡涓厤缃瘎瀛樺櫒涓殑鏁版嵁鍜屽畠鐨�2瀛楄妭PEC(妫�閿欑爜)鍐欏叆鎺ユ敹鏁扮粍涓紝鎺ユ敹鐨勬槸鍊掑簭鐨�8915A鑺墖
		{
			configDArray[c_ic].rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		calc_pec = Bf8915pec15_calc(6,&read_buffer[8*current_ic]);//璁＄畻鎺ユ敹鍥炴潵鐨勫瘎瀛樺櫒缁�(6涓�8浣嶅瘎瀛樺櫒)涓殑PEC(妫�閿欑爜)
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);//灏唕ead_buffer[]涓帴鏀跺埌鐨凱EC(妫�閿欑爜)鎸夐珮浣庡叡16浣嶅瓨鍏ata_pec涓�
		if (calc_pec != data_pec )//妫�鏌ユ閿欑爜鏄惁鍖归厤
		{
			configDArray[c_ic].rx_pec_match = 1U;//妫�閿欑爜涓嶆纭�
			pec_error = 1U;
		}
		else configDArray[c_ic].rx_pec_match = 0;//妫�閿欑爜姝ｇ‘
	}
	for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负瀛樺叆姣忎釜8915A鑺墖閰嶇疆瀵勫瓨鍣ㄧ粍D涓殑鎸囦护
	{
		configDData[current_ic].csel_m = (((u16)configDArray[current_ic].rx_data[1]) << 0x8U) + configDArray[current_ic].rx_data[0];
		                                                                                            //CFGDR0鐨�8浣嶅拰CFGDR1鐨�8浣�,灏咰FGDR1鐨�8浣嶆斁鍏ラ珮8浣嶏紝璇诲嚭鐩戞祴妯″紡涓嬶紝鐢垫睜鐢靛帇鐩戞祴姣旇緝閫夋嫨
		configDData[current_ic].gsel_m = configDArray[current_ic].rx_data[2];						//CFGDR2鐨�8浣�,璇诲嚭鐩戞祴妯″紡涓嬶紝GPIO0~7姣旇緝閫夋嫨
		configDData[current_ic].isospi_wake_t = (BOOLEAN)((configDArray[current_ic].rx_data[3] & 0x80) == 0x80);//CFGDR3鐨勭8浣�,鑿婅姳閾維PI鐨勫敜閱掓椂闂�
		configDData[current_ic].adc_rst_sel = (BOOLEAN)((configDArray[current_ic].rx_data[3] & 0x40) == 0x40);	//CFGDR3鐨勭7浣�,娴嬮噺绗�1涓�氶亾ADC澶嶄綅閫夋嫨
		configDData[current_ic].time_sel_m = configDArray[current_ic].rx_data[3] & 0x3F;			//CFGDR3鐨�6~1浣�,鐩戞祴妯″紡涓嬶紝瀹氭椂鐩戞祴鏃堕棿閰嶇疆
		configDData[current_ic].adc_init_t2 = (configDArray[current_ic].rx_data[4] & 0xF0) >> 0x4U;	//CFGDR4鐨勯珮4浣�,ADC娴嬮噺绗�2~n涓�氶亾ADC鍒濆鍖栨椂闂�
		configDData[current_ic].adc_init_t1 = configDArray[current_ic].rx_data[4] & 0x0F;			//CFGDR4鐨勪綆4浣�,ADC娴嬮噺绗�1涓�氶亾ADC鍒濆鍖栨椂闂�
		configDData[current_ic].trim2_en = (BOOLEAN)((configDArray[current_ic].rx_data[5] & 0x80) == 0x80U);//CFGDR5鐨勭8浣�,淇皟2浣胯兘
		configDData[current_ic].trim1_en = (BOOLEAN)((configDArray[current_ic].rx_data[5] & 0x40) == 0x40U);//CFGDR5鐨勭7浣�,淇皟1浣胯兘
		configDData[current_ic].err_t_sel_m = (configDArray[current_ic].rx_data[5] & 0x30) >> 0x4U; //CFGDR5鐨勭6~5浣�,鐩戞祴妯″紡涓嬪彂鐢熷紓甯革紝涓婃姤鐨勯棿闅旀椂闂�
		configDData[current_ic].vd33t_on_m = configDArray[current_ic].rx_data[5] & 0x0F;			//CFGDR5鐨勪綆4浣�,鐩戞祴妯″紡涓嬶紝VREF1鍜孷D33T鐨勫惎鍔ㄦ椂闂撮厤缃�
		configDData[current_ic].rx_pec_match = configDArray[current_ic].rx_pec_match;				//姣忎釜閰嶇疆瀵勫瓨鍣ㄧ粍D涓殑妫�閿欑爜鏄惁鍖归厤
	}
	return(pec_error);//鍥炰紶妫�鏌ユ閿欑爜鐨勫��
}

/*!
    \brief      ADCV CMD(鐢垫睜鐢靛帇ADC杞崲鎸囦护)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨:0銆�1銆�2銆�3)
		\param[in]  DISCP : DISCP_DISABLE銆丏ISCP_ENABLE(鏀剧數鍏佽浣�:鏀剧數鍏佽浣嶅啓0鍜�1)
		\param[in]  CSEL  : CSEL_CH_ALL銆丆SEL_CH_1and9銆丆SEL_CH_2and10銆丆SEL_CH_3and11銆丆SEL_CH_4and12
												             CSEL_CH_5and13銆丆SEL_CH_6and14銆丆SEL_CH_7and15銆丆SEL_CH_8and16
   (ADC杞崲鐨勭數姹犻�夋嫨:鎵�鏈夌數姹�(0)銆佺數姹�1鍜�9(1)銆佺數姹�2鍜�10(2)銆佺數姹�3鍜�11(3)銆佺數姹�4鍜�12(4)
												             鐢垫睜5鍜�13(5)銆佺數姹�6鍜�14(6)銆佺數姹�7鍜�15(7)銆佺數姹�8鍜�16(8))
    \retval     none   	                  					  
*/
void BF8915A_adcv(u8 CLKSEL, u8 OSR, u8 DISCP, u8 CSEL)
{
	u8 cmd[2] = {0U};			//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;		//璁剧疆CLKSEL鐨�2涓綅 鐨勫彉閲�
	clksel_bits = (CLKSEL & 0x02) >> 1;			//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADCV << 0x1U);	//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_ADCV	0x1U
	clksel_bits = (CLKSEL & 0x01) << 7;			//璁剧疆CLKSEL[0]
	cmd[1] = clksel_bits | (OSR<<5) | (DISCP<<4) | CSEL;//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 ADCV 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);							//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      CVST CMD(鐢垫睜鐢靛帇鑷祴璇曞懡浠�)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 銆丱SR_128銆丱SR_2566銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨:0銆�1銆�2銆�3)
		\param[in]  SFTEST: SELFTEST_6666		SELFTEST_9999(鑷祴璇曟ā寮忛�夋嫨:鑷祴璇�1 鑷祴璇�2)
    \retval     none   	                  				
*/  
void BF8915A_cvst(u8 CLKSEL, u8 OSR, u8 SFTEST)
{
	u8 cmd[2] = {0U};			//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;		//璁剧疆CLKSEL鐨�2涓綅 鐨勫彉閲�
	clksel_bits = (CLKSEL & 0x02) >> 1;			//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_CVST << 0x1U);	//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_CVST	0x03U
	clksel_bits = (CLKSEL & 0x01) << 7;			//璁剧疆CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (SFTEST<<4);//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 CVST 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);							//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      ADOL CMD(鐢垫睜閲嶅彔鍗曞厓娴嬮噺鎸囦护)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨:0銆�1銆�2銆�3)
		\param[in]  DISCP : DISCP_DISABLE銆丏ISCP_ENABLE	(鏀剧數鍏佽浣�:鏀剧數鍏佽浣嶅啓0鍜�1)
    \retval     none   		                  				  
*/
void BF8915A_adol(u8 CLKSEL, u8 OSR, u8 DISCP)
{
	u8 cmd[2] = {0U};			//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;		//璁剧疆CLKSEL鐨�2涓綅 鐨勫彉閲�
	clksel_bits = (CLKSEL & 0x02) >> 1;			//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADOL << 0x1U);	//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_ADOL	0x04U
	clksel_bits = (CLKSEL & 0x01) << 7;			//璁剧疆CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (DISCP<<4) | 0x01U;//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 ADOL 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);							//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      ADGP CMD(GPIO ADC杞崲鎸囦护)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨)
		\param[in]  GSEL 	: GSEL_CH_ALL銆�	 GSEL_CH_GPIO0銆丟SEL_CH_GPIO1銆丟SEL_CH_GPIO2銆丟SEL_CH_GPIO3
												GSEL_CH_VD33T銆丟SEL_CH_GPIO4銆丟SEL_CH_GPIO5銆丟SEL_CH_GPIO6銆丟SEL_CH_GPIO7
  (ADC杞崲鐨凣PIO閫夋嫨 :鎵�鏈塆PIO0~GPIO7,VD33T銆丟PIO0銆丟PIO1銆丟PIO2銆丟PIO3
											                 VD33T銆丟PIO4銆丟PIO5銆丟PIO6銆丟PIO7)
    \retval     none   	                  				  
*/
void BF8915A_adgp(u8 CLKSEL, u8 OSR, u8 GSEL)
{
	u8 cmd[2] = {0U};		//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;	//璁剧疆CLKSEL鐨�2涓綅 鐨勫彉閲�
	clksel_bits = (CLKSEL & 0x02) >> 1;			//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADGP << 0x1U);	//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_ADGP	0x05U
	clksel_bits = (CLKSEL & 0x01) << 7;			//璁剧疆CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | GSEL;	//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 ADGP 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);							//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      GPST CMD(GPIO杈撳叆鑷祴璇曟寚浠�)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨:0銆�1銆�2銆�3)
		\param[in]  SFTEST: SELFTEST_6666		SELFTEST_9999(鑷祴璇曟ā寮忛�夋嫨:鑷祴璇�1 鑷祴璇�2)
    \retval     none   	                  				
*/  
void BF8915A_gpst(u8 CLKSEL, u8 OSR, u8 SFTEST )
{
	u8 cmd[2] = {0U};				//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;			//璁剧疆CLKSEL鐨�2涓綅 鐨勫彉閲�
	clksel_bits = (CLKSEL & 0x02) >> 1;				//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_GPST << 0x1U);		//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_GPST	0x07U
	clksel_bits = (CLKSEL & 0x01) << 7;				//璁剧疆CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (SFTEST<<4);	//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 GPST 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);								//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      ADSTAT CMD(娴嬮噺鍐呴儴璁惧鍙傛暟鎸囦护)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨:0銆�1銆�2銆�3)
		\param[in]  STSEL : STATA銆丼TATB銆丼TATC銆丼TATD(ADC杞崲鐨勫唴閮ㄥ弬鏁伴�夋嫨)
    \retval     none   	                  				
*/                                  
void BF8915A_adstat(u8 CLKSEL, u8 OSR, u8 STSEL)
{
	u8 cmd[2] = {0U};				//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;			//璁剧疆CLKSEL鐨�2涓綅 鐨勫彉閲�
	clksel_bits = (CLKSEL & 0x02) >> 1;			//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADSTAT << 0x1U);//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_ADSTAT	0x08U
	clksel_bits = (CLKSEL & 0x01) << 7;			//璁剧疆CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | STSEL;	//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 ADSTAT 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);							//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      STATST CMD(鑷祴璇曞唴閮ㄨ澶囧弬鏁版寚浠�)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨:0銆�1銆�2銆�3)
		\param[in]  SFTEST: SELFTEST_6666		SELFTEST_9999(鑷祴璇曟ā寮忛�夋嫨:鑷祴璇�1 鑷祴璇�2)
    \retval     none   	                  				
*/  
void BF8915A_statst(u8 CLKSEL, u8 OSR, u8 SFTEST)
{
	u8 cmd[2] = {0U};				//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;			//璁剧疆CLKSEL 浣� 鐨勫彉閲�
	clksel_bits = (CLKSEL & 0x02) >> 1;				//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_STATST << 0x1U);	//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_STATST	0x09U
	clksel_bits = (CLKSEL & 0x01) << 7;				//璁剧疆CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (SFTEST<<4);	//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 CMD_STATST 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);								//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      ADCVGP CMD(娴嬮噺鐢垫睜鐢靛帇鍜孏PIO杈撳叆鎸囦护)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨:0銆�1銆�2銆�3)
		\param[in]  DISCP : DISCP_DISABLE銆丏ISCP_ENABLE(鏀剧數鍏佽浣�:鏀剧數鍏佽浣嶅啓0鍜�1)
    \retval     none   	                  				
*/  
void BF8915A_adcvgp(u8 CLKSEL, u8 OSR, u8 DISCP)
{
	u8 cmd[2] = {0U};						//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;					//璁剧疆CLKSEL鐨�2涓綅 鐨勫彉閲�
	clksel_bits = (CLKSEL & 0x02) >> 1;		//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_ADCVGP << 0x1U);	//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_ADCVGP	0x0aU
	clksel_bits = (CLKSEL & 0x01) << 7;				//璁剧疆CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (DISCP<<4);	//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 CMD_ADCVGP 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);								//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      MNT CMD(鐩戞祴鎸囦护)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   : OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨:0銆�1銆�2銆�3)
		\param[in]  MNMOD : MNMOD_DISABLE銆丮NMOD_1銆丮NMOD_2銆丮NMOD_3(鐩戞祴妯″紡閫夋嫨:涓嶈繘鍏ョ洃娴嬫ā寮忥紝鎸囦护鏃犳晥(00)銆佹ā寮�1(01)銆佹ā寮�2(02)銆佹ā寮�3(03))
		\param[in]  STRMN : STRMN_ENABLE銆丼TRMN_DISABLE(鐩戞祴妯″紡浣胯兘:寮�鍚洃娴嬫ā寮�(1)銆佸叧闂洃娴嬫ā寮�(2))
		\param[in]  BLEN  : BLEN_ON銆丅LEN_OFF	(鍧囪　寮�鍏虫�绘帶鍒朵俊鍙�2:鍏抽棴(0)銆佸紑鍚�(1))
    \retval     none                     				
*/                                       				
void BF8915A_mnt(u8 CLKSEL, u8 OSR, u8 MNMOD, u8 STRMN, u8 BLEN)
{
	u8 cmd[2] = {0U};				//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;			//璁剧疆CLKSEL鐨�2涓� 浣嶇殑鍙橀噺
	clksel_bits = (CLKSEL & 0x02) >> 1;			//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_MNT << 0x1U);	//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_MNT	0x0cU
	clksel_bits = (CLKSEL & 0x01) << 7;			//璁剧疆CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (MNMOD<<3) | (STRMN << 2) | BLEN ;//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 CMD_MNT 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);							//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      STRBL CMD(琚姩鍧囪　鎸囦护)
		\param[in]  CLKSEL: CLKSEL_2M
		\param[in]  OSR   :	OSR_64 銆丱SR_128銆丱SR_256銆丱SR_512(ADC杩囬噰鏍烽鐜囬�夋嫨)
		\param[in]  OESEL :	OESEL_ODD銆丱ESEL_EVEN	(鍧囪　鏀剧數鐢垫睜濂囧伓閫夋嫨:濂囨暟鑺�(0)銆佸伓鏁拌妭(1))
		\param[in]  BLEN  : BLEN_ON銆丅LEN_OFF(鍧囪　寮�鍏虫�绘帶鍒朵俊鍙� 2:寮�鍚�(1)銆佸叧闂�(0))
    \retval     none                     
*/                      
void BF8915A_strbl(u8 CLKSEL, u8 OSR, u8 OESEL,u8 BLEN)
{
	u8 cmd[2] = {0U};				//鍒濆鍖栨暟缁刢md[]
	u8 clksel_bits = 0x0U;			//璁剧疆CLKSEL鐨�2涓綅 鐨勫彉閲�
	clksel_bits = (CLKSEL & 0x02) >> 1;			//璁剧疆CLKSEL[1]
	cmd[0] = clksel_bits + (CMD_STRBL << 0x1U);	//鍙戦�佹寚浠ょ殑楂�8浣�,#define CMD_STRBL	0x0dU
	clksel_bits = (CLKSEL & 0x01) << 7;			//璁剧疆CLKSEL[0]
	cmd[1] =  clksel_bits | (OSR<<5) | (OESEL<<4) | BLEN;//鍙戦�佹寚浠ょ殑浣�8浣�,璇︽儏璇峰弬鑰冭鏍间功涓� 鎿嶄綔鎸囦护閰嶇疆 CMD_STRBL 鐨勬寚浠ゆ弿杩�
	BF8915A_cmd(cmd);							//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      Reads the raw cell voltage register data(璇诲彇鍘熷鐢垫睜鐢靛帇瀵勫瓨鍣ㄦ暟鎹�)
		\param[in]  reg     : Determines which  register is read back(纭畾鍥炶鍝釜鐢垫睜鐢靛帇瀵勫瓨鍣�)
		\param[in]  total_ic: the Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  cellData: Array of the unparsed codes(鏈В鏋愪唬鐮佺殑鏁扮粍)
    \retval     none
*/
void BF8915A_rdcv_reg(u8 reg, u8 total_ic, u8 *cellData)
{
	const u8 REG_LEN = 8; 	//number of 6 bytes in each ICs register + 2 bytes for the PEC(姣忎釜闆嗘垚鐢佃矾瀵勫瓨鍣ㄤ腑鐨�6瀛楄妭鏁板姞PEC(妫�閿欑爜)鐨�2涓瓧鑺�)
	u8 cmd[4];				//鍒濆鍖栨暟缁刢md[]
	u16 cmd_pec;			//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[0] = 0x00;			//鍙戦�佹寚浠ょ殑楂�8浣�
	cmd[1] = reg + 0x08U;	//鍙戦�佹寚浠ょ殑浣�8浣嶏紝璇︽儏鍦�8915A.h鏂囦欢涓� 鐢垫睜鐢靛帇瀵勫瓨鍣ㄥ搴旂殑鍛戒护
	cmd_pec = Bf8915pec15_calc(2, cmd);	//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[2] = (u8)(cmd_pec >> 8);		//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勯珮鍏綅
	cmd[3] = (u8)(cmd_pec);				//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勪綆鍏綅

	if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
	{
		BF8915_CS_LOW();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_read(cmd,4,cellData,(REG_LEN*total_ic));//浣跨敤SPI绔彛鍐欏叆鍜岃鍙栬瀹氭暟閲忕殑瀛楄妭
		BF8915_CS_HIGH();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
	}
	else
	{
		BF8915_CS_LOW_Reverse();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_read(cmd,4,cellData,(REG_LEN*total_ic));//浣跨敤SPI绔彛鍐欏叆鍜岃鍙栬瀹氭暟閲忕殑瀛楄妭
		BF8915_CS_HIGH_Reverse();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
	}
}

/*!
    \brief      reads BF8915A GPIO registers(璇诲彇BF8915A鐨凣PIO瀵勫瓨鍣�)
                The function reads a single GPIO voltage register and stores the read data in the *data point as a byte array. 
                This function is rarely used outside of the BF8915A_rdgv() command.
               (璇ュ嚱鏁拌鍙栧崟涓狦PIO鐢靛帇瀵勫瓨鍣紝骞跺皢璇诲彇鐨勬暟鎹綔涓哄瓧鑺傛暟缁勫瓨鍌ㄥ湪*data鏁版嵁涓�傛鍔熻兘寰堝皯鍦˙F8915A_rdgv()鍛戒护涔嬪浣跨敤銆�)
		\param[in]  reg     : Determines which  register is read back(纭畾鍥炶鍝釜鐢垫睜鐢靛帇瀵勫瓨鍣�)
		\param[in]  total_ic: the Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  gpioData: Array of the unparsed codes(鏈В鏋愪唬鐮佺殑鏁扮粍)
    \retval     none
*/
void BF8915A_rdgv_reg(u8 reg, u8 total_ic, u8 *gpioData)
{
	const u8 REG_LEN = 8; // number of 6 bytes in the register + 2 bytes for the PEC(瀵勫瓨鍣ㄤ腑鐨�6瀛楄妭鏁板姞涓奝EC(妫�閿欑爜)鐨�2涓瓧鑺�)
	u8 cmd[4];			//鍒濆鍖栨暟缁刢md[]
	u16 cmd_pec;		//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[0] = 0x00;		//鍙戦�佹寚浠ょ殑楂�8浣�
	cmd[1] = reg + 0x0EU;				//鍙戦�佹寚浠ょ殑浣�8浣嶏紝璇︽儏鍦�8915A.h鏂囦欢涓� GPIOG鐢靛帇
	cmd_pec = Bf8915pec15_calc(2, cmd);	//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[2] = (u8)(cmd_pec >> 8);		//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勯珮鍏綅
	cmd[3] = (u8)(cmd_pec);				//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勪綆鍏綅
	
	if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
	{
		BF8915_CS_LOW();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_read(cmd,4,gpioData,(REG_LEN*total_ic));//浣跨敤SPI绔彛鍐欏叆鍜岃鍙栬瀹氭暟閲忕殑瀛楄妭
		BF8915_CS_HIGH();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
	}
	else
	{
		BF8915_CS_LOW_Reverse();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_read(cmd,4,gpioData,(REG_LEN*total_ic));//浣跨敤SPI绔彛鍐欏叆鍜岃鍙栬瀹氭暟閲忕殑瀛楄妭
		BF8915_CS_HIGH_Reverse();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
	}
}

/*!
    \brief      Reads BF8915A stat registers(璇诲彇BF8915A鐘舵�佸瘎瀛樺櫒)
		\param[in]  reg     : Determines which register is read back(纭畾鍥炶鍝釜瀵勫瓨鍣�)
		\param[in]  total_ic: the Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  statData: Array of the unparsed codes(鏈В鏋愪唬鐮佺殑鏁扮粍)
    \retval     none
*/
void BF8915A_rdstat_reg(u8 reg, u8 total_ic, u8 *statData)
{
	const u8 REG_LEN = 8;	//number of 6 bytes in the register + 2 bytes for the PEC(瀵勫瓨鍣ㄤ腑鐨�6瀛楄妭鏁板姞涓奝EC(妫�閿欑爜)鐨�2涓瓧鑺�)
	u8 cmd[4];				//鍒濆鍖栨暟缁刢md[]
	u16 cmd_pec;			//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[0] = 0x00;			//鍙戦�佹寚浠ょ殑楂�8浣�
	cmd[1] = reg + 0x11;	//鍙戦�佹寚浠ょ殑浣�8浣嶏紝璇︽儏鍦�8915A.h鏂囦欢涓� 鐘舵�佸瘎瀛樺櫒RDSTAT
	cmd_pec = Bf8915pec15_calc(2, cmd);	//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[2] = (u8)(cmd_pec >> 8);		//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勯珮鍏綅
	cmd[3] = (u8)(cmd_pec);				//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勪綆鍏綅

	if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
	{
		BF8915_CS_LOW();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_read(cmd,4,statData,(REG_LEN*total_ic));//浣跨敤SPI绔彛鍐欏叆鍜岃鍙栬瀹氭暟閲忕殑瀛楄妭
		BF8915_CS_HIGH();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
	}
	else
	{
		BF8915_CS_LOW_Reverse();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_read(cmd,4,statData,(REG_LEN*total_ic));//浣跨敤SPI绔彛鍐欏叆鍜岃鍙栬瀹氭暟閲忕殑瀛楄妭
		BF8915_CS_HIGH_Reverse();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
	}
}

/*!
    \brief      Helper function that parses voltage measurement registers(瑙ｆ瀽鐢靛帇娴嬮噺瀵勫瓨鍣ㄧ殑杈呭姪鍑芥暟)
		\param[in]  current_ic: Number of ics in current cycle(褰撳墠寰幆鐨処C(8915A)鏁�)
		\param[in]  cell_reg  : Controls which cell voltage register is read back(鎺у埗鍥炶鍝釜鍗曚綋鐢靛帇瀵勫瓨鍣�)
		\param[in]  cell_data : Number of bytes of data received(鎺ユ敹鍒版暟鎹殑瀛楄妭鏁帮紝6涓暟鎹�+2涓狿EC(妫�閿欑爜))
		\param[in]  cell_codes: cell voltage value(鐢垫睜鐢靛帇鍊�)
		\param[in]  ic_pec    : If a PEC error was detected during most recent read cmd(濡傛灉鍦ㄦ渶杩戠殑璇诲彇cmd(鎸囦护)鏈熼棿妫�娴嬪埌PEC(妫�閿欑爜)閿欒)
    \retval     pec_error
*/
u8 parse_cells(u8 current_ic, u8 cell_reg, u8 cell_data[], u16 *cell_codes, u8 *ic_pec)
{
	const u8 BYT_IN_REG = 6;	//瀵勫瓨鍣ㄧ粍涓殑6涓�8浣嶅瘎瀛樺櫒
	u8 CELL_IN_REG = 3;			//姣忎釜鐢靛帇瀵勫瓨鍣ㄧ粍涓殑 3涓數鍘嬪��
	u8 pec_error = 0;			//妫�鏌ユ閿欑爜
	u16 parsed_cell;			//鍗曚釜鐢靛帇鍊�
	u16 received_pec;			//鏀跺埌鐨勬瘡涓瘎瀛樺櫒缁勪腑鐨凱EC(妫�閿欑爜)
	u16 data_pec;				//璁＄畻鏀跺埌鐨勬瘡涓瘎瀛樺櫒缁勪腑鎸囦护鏁版嵁鐨凱EC(妫�閿欑爜)
	u8 data_counter = current_ic*NUM_RX_BYT; //data counter(鏁版嵁璁℃暟鍣�)
	u8 current_cell = 0;		//姣忎釜鐢靛帇瀵勫瓨鍣ㄧ粍褰撳墠寰幆鍒扮殑鍗曚綋鐢垫睜鐢靛帇
	
	for (current_cell = 0; current_cell < CELL_IN_REG; current_cell++)  // This loop parses the read back data into cell voltages, it loops once for each of the 3 cell voltage codes in the register
	{																																		//(杩欎釜寰幆灏嗚鍥炵殑鏁版嵁瑙ｆ瀽鎴愮數鍘嬶紝瀵瑰瘎瀛樺櫒涓殑姣�3涓數鍘嬬爜寰幆涓�娆�)
		parsed_cell = (u16)cell_data[data_counter] + ((u16)cell_data[data_counter + 1] << 8);//Each cell codes is received as two bytes and is combined to create the parsed cell voltage code
																		   //(姣忎釜鐢靛帇浠ｇ爜琚帴鏀朵负涓や釜瀛楄妭锛屽苟涓旇缁勫悎浠ュ垱寤鸿В鏋愮殑鍗曚綋鐢靛帇浠ｇ爜)
		cell_codes[current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;//灏嗚В鏋愬嚭鐨勫崟涓數鍘嬬爜渚濇瀛樻斁鍦ㄦ暟缁刢ell_codes[]涓�
		data_counter = data_counter + 2;//Because cell voltage codes are two bytes the data counter must increment by two for each parsed cell code
																//(鍥犱负鍗曚綋鐢靛帇鐮佹槸涓や釜瀛楄妭锛屾墍浠ュ浜庢瘡涓В鏋愮殑鐢垫睜鐮侊紝鏁版嵁璁℃暟鍣ㄥ繀椤诲鍔�2)
		if(cell_reg == CVFR)//#define CVFR	0x06U
		{
			current_cell = 0x3U;//瀛樻斁鐢垫睜鐢靛帇鐨勭粨鏋勪綋鏁扮粍鏈�16涓厓绱狅紝姝ゅ浠ｇ爜涓轰簡闃叉鏁扮粍瑙ｆ瀽CVFR(鐢垫睜鐢靛帇瀵勫瓨鍣ㄧ粍F)鏃讹紝鏁扮粍婧㈠嚭闂
		}
	}
	if(cell_reg == CVFR)//鑻ュ瘎瀛樺櫒涓篊VFR锛屽垯鍙渶寰幆涓�娆″嵆鍙烦鍑哄惊鐜�
	{
		data_counter = data_counter + 4;
	}
	received_pec = (cell_data[data_counter] << 8) | cell_data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th after the 6 cell voltage data bytes
																																						 //(鎺ユ敹鍒扮殑current_ic(褰撳墠IC(8915A)鏁伴噺)鐨凱EC(妫�閿欑爜)浣滀负6涓崟浣撶數鍘嬫暟鎹瓧鑺備箣鍚庣殑绗�7涓拰绗�8涓瓧鑺傚彂閫�)
	data_pec = Bf8915pec15_calc(BYT_IN_REG, &cell_data[(current_ic) * NUM_RX_BYT]);//璁＄畻鎺ユ敹鍥炴潵鐨勫瘎瀛樺櫒缁�(6涓�8浣嶅瘎瀛樺櫒)涓殑PEC(妫�閿欑爜)

	if (received_pec != data_pec)//妫�鏌ユ閿欑爜鏄惁鍖归厤
	{
		pec_error = 1;     //The pec_error variable is simply set 1 if any PEC errors(濡傛灉鏈変换浣昿ec閿欒锛宲ec_error鍙橀噺绠�鍗曞湴璁剧疆涓�1)
		ic_pec[cell_reg-1]=1;//灏哻ellVoltage[c_ic].pec_match[0]涓殑鍊肩疆1
	}
	else
	{
		ic_pec[cell_reg-1]=0;//妫�閿欑爜姝ｇ‘锛屽皢cellVoltage[c_ic].pec_match[0]涓殑鍊肩疆0
	}
	data_counter=data_counter+2;//Because the transmitted PEC code is 2 bytes long the data_counter must be incremented by 2 bytes to point to the next ICs state voltage data
														  //(鍥犱负浼犺緭鐨凱EC(妫�閿欑爜)浠ｇ爜鏄�2瀛楄妭闀匡紝鎵�浠ユ暟鎹鏁板櫒蹇呴』澧炲姞2瀛楄妭鎵嶈兘鎸囧悜涓嬩竴涓�8915A鐘舵�佺數鍘嬫暟鎹�)
	return(pec_error);//鍥炰紶妫�鏌ユ閿欑爜鐨勫��
}

/*!
    \brief      Reads and parses the BF8915A cell voltage registers(璇诲彇骞惰В鏋怋F8915A鐢垫睜鐢靛帇瀵勫瓨鍣�)
		\param[in]  reg                : Controls which cell voltage register is read back(鎺у埗鍥炶鍝釜鐢垫睜鐢靛帇瀵勫瓨鍣�)
		\param[in]  total_ic           : the Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  CellVoltage_TypeDef: CellVoltage Struct(鐢垫睜鐢靛帇缁撴瀯)
    \retval     the time of pec_error(pec_error鐨勬暟)
*/
u8 BF8915A_rdcv(u8 reg, u8 total_ic, CellVoltage_TypeDef *cellVoltage)
{
	u8 pec_error = 0;		//妫�鏌ユ閿欑爜
	u8 *cell_data;			//鍒濆鍖栦竴涓�8浣嶇殑鎸囬拡cell_data
	u8 c_ic = 0;			//褰撳墠姝ｅ湪璇诲叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 cell_reg = 1;		//鐢垫睜鐢靛帇瀵勫瓨鍣ㄨ鏁板櫒
	u8 current_ic = 0;		//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	cell_data = (u8 *) malloc((NUM_RX_BYT*total_ic)*sizeof(u8));//#define NUM_RX_BYT 0x8U(鎺ユ敹鍒版暟鎹殑瀛楄妭鏁帮紝6涓瓧鑺傛暟鎹�+2涓狿EC(妫�閿欑爜)瀛楄妭)

	if (reg == 0)//濡傛灉閫夋嫨璇诲彇鎵�鏈夌數姹犵數鍘�
	{
		for (cell_reg = 1; cell_reg < NUM_CV_REG + 1; cell_reg++) //#define NUM_CV_REG 6U,瀵规瘡涓狟F8915A鐢垫睜鐢靛帇瀵勫瓨鍣ㄦ墽琛屼竴娆�
		{
			BF8915A_rdcv_reg(cell_reg, total_ic,cell_data );//璇诲彇鍘熷鐢垫睜鐢靛帇瀵勫瓨鍣ㄦ暟鎹�
			for (current_ic = 0; current_ic < total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负璇诲彇骞惰В鏋愭瘡涓狟F8915A鎵�閫夋嫨鐨勭數姹犵數鍘嬪瘎瀛樺櫒
			{
				if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
				{
					c_ic = current_ic;		//閫夋嫨姝ｅ簭灏嗙數姹犵數鍘嬫斁鍏ユ暟缁�
				}
				else
				{
					c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗙數姹犵數鍘嬫斁鍏ユ暟缁�
				}
				pec_error = pec_error + parse_cells(current_ic,cell_reg, cell_data,
												&cellVoltage[c_ic].cellVoltage[0],
												&cellVoltage[c_ic].pec_match[0]);//瑙ｆ瀽鐢靛帇娴嬮噺瀵勫瓨鍣ㄧ殑杈呭姪鍑芥暟,骞跺洖浼犳鏌ユ閿欑爜鐨勫��
			}
		}
	}
	else//濡傛灉涓嶉�夋嫨璇诲彇鎵�鏈夌數姹犵數鍘�
	{
		BF8915A_rdcv_reg(reg, total_ic,cell_data);//璇诲彇鍘熷鐢垫睜鐢靛帇瀵勫瓨鍣ㄦ暟鎹�

		for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负璇诲彇骞惰В鏋愭瘡涓狟F8915A鎵�閫夋嫨鐨勭數姹犵數鍘嬪瘎瀛樺櫒
		{
			if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
			{
				c_ic = current_ic;		//閫夋嫨姝ｅ簭灏嗙數姹犵數鍘嬫斁鍏ユ暟缁�
			}
			else
			{
				c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗙數姹犵數鍘嬫斁鍏ユ暟缁�
			}
			pec_error = pec_error + parse_cells(current_ic,reg, cell_data,
                                          &cellVoltage[c_ic].cellVoltage[0],
                                          &cellVoltage[c_ic].pec_match[0]);//瑙ｆ瀽鐢靛帇娴嬮噺瀵勫瓨鍣ㄧ殑杈呭姪鍑芥暟,骞跺洖浼犳鏌ユ閿欑爜鐨勫��
		}
	}
	free(cell_data);	//娓呯┖鎸囬拡
	return(pec_error);	//鍥炰紶妫�鏌ユ閿欑爜鐨勫��
}

/*!
    \brief      Reads and parses the BF8915A GPIO registers(璇诲彇骞惰В鏋怋F8915A GPIO瀵勫瓨鍣�)
		\param[in]  reg        : Determines which GPIO voltage register is read back(纭畾鍥炶鍝釜GPIO鐢靛帇瀵勫瓨鍣�)
		\param[in]  total_ic   : the Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  gpioVoltage: A two dimensional array of the gpio voltage codes(GPIO鐢靛帇鐨勪簩缁存暟缁�)
    \retval     pec_error
*/
u8 BF8915A_rdgv(u8 reg, u8 total_ic, GPIOReg_TypeDef *gpioVoltage)
{
	u8 *dataPtr;		//鍒濆鍖栦竴涓�8浣嶇殑鎸囬拡dataPtr
	u8 pec_error = 0;	//妫�鏌ユ閿欑爜
	u8 c_ic =0;			//褰撳墠姝ｅ湪璇诲叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 gpio_reg = 1;	//GPIO瀵勫瓨鍣ㄨ鏁板櫒
	u8 current_ic = 0;	//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	dataPtr = (u8 *) malloc((NUM_RX_BYT*total_ic)*sizeof(u8));//璁＄畻鎸囬拡dataPtr闇�瑕佹帴鏀剁殑闀垮害
																		//#define NUM_RX_BYT 0x8U(鎺ユ敹鍒版暟鎹殑瀛楄妭鏁帮紝6涓瓧鑺傛暟鎹�+2涓狿EC(妫�閿欑爜)瀛楄妭)
	if (reg == 0)//濡傛灉閫夋嫨璇诲彇鎵�鏈塆PIO鐢靛帇瀵勫瓨鍣�
	{
		for (gpio_reg = 1; gpio_reg < NUM_GOIO_REG + 1; gpio_reg++) //#define NUM_STAT_REG	3U,瀵规瘡涓狟F8915A鐨凣PIO鐢靛帇瀵勫瓨鍣ㄦ墽琛屼竴娆�
		{
			BF8915A_rdgv_reg(gpio_reg, total_ic,dataPtr);//Reads the raw gpio register data into the data[] array(灏嗗師濮婫PIO瀵勫瓨鍣ㄦ暟鎹鍏ata[]鏁扮粍)
			for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负璇诲彇骞惰В鏋愭瘡涓狟F8915A鎵�閫夋嫨鐨凣PIO鐢靛帇瀵勫瓨鍣�
			{
				if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
				{
					c_ic = current_ic;//閫夋嫨姝ｅ簭灏咷PIO鐢靛帇鏀惧叆鏁扮粍
				}
				else
				{
					c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏咷PIO鐢靛帇鏀惧叆鏁扮粍
				}
				pec_error = parse_cells(current_ic,gpio_reg, dataPtr,
                                &gpioVoltage[c_ic].gpioVoltage[0],
                                &gpioVoltage[c_ic].pec_match[0]);//瑙ｆ瀽鐢靛帇娴嬮噺瀵勫瓨鍣ㄧ殑杈呭姪鍑芥暟,骞跺洖浼犳鏌ユ閿欑爜鐨勫��
			}
		}
	}
	else//濡傛灉涓嶉�夋嫨璇诲彇鎵�鏈塆PIO鐢靛帇瀵勫瓨鍣�
	{
		BF8915A_rdgv_reg(reg, total_ic, dataPtr);//Reads the raw gpio register data into the data[] array(灏嗗師濮婫PIO瀵勫瓨鍣ㄦ暟鎹鍏ata[]鏁扮粍)

		for (current_ic = 0; current_ic<total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负璇诲彇骞惰В鏋愭瘡涓狟F8915A鎵�閫夋嫨鐨凣PIO鐢靛帇瀵勫瓨鍣�
		{
			if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
			{
				c_ic = current_ic;		//閫夋嫨姝ｅ簭灏咷PIO鐢靛帇鏀惧叆鏁扮粍
			}
			else
			{
				c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏咷PIO鐢靛帇鏀惧叆鏁扮粍
			}
			pec_error = parse_cells(current_ic,reg, dataPtr,
                              &gpioVoltage[c_ic].gpioVoltage[0],
                              &gpioVoltage[c_ic].pec_match[0]);//瑙ｆ瀽鐢靛帇娴嬮噺瀵勫瓨鍣ㄧ殑杈呭姪鍑芥暟,骞跺洖浼犳鏌ユ閿欑爜鐨勫��
		}

	}
	free(dataPtr);		//娓呯┖鎸囬拡
	return (pec_error);	//鍥炰紶妫�鏌ユ閿欑爜鐨勫��
}

/*!
    \brief      Reads and parses the BF8915A stat registers(璇诲彇鍜岃В鏋怋F8915A鐘舵�佸瘎瀛樺櫒)
		\param[in]  reg             : Determines which Stat register is read back(纭畾鍥炶鍝釜鐘舵�佸瘎瀛樺櫒)
		\param[in]  total_ic        : the Number of ICs in the system(绯荤粺涓璉C(8915A)鐨勬暟閲�)
		\param[in]  StateReg_TypeDef: A two dimensional array of the state codes(鐘舵�佸瘎瀛樺櫒鐨勪簩缁存暟缁�)
    \retval     pec_error
*/
u8 BF8915A_rdstat(u8 reg, u8 total_ic, StateReg_TypeDef *stateReg)
{
	const u8 BYT_IN_REG = 6;		//瀵勫瓨鍣ㄧ粍涓殑6涓�8浣嶅瘎瀛樺櫒
	const u8 GPIO_IN_REG = 3;		//姣忎釜瀵勫瓨鍣ㄧ粍涓殑 3涓數鍘嬪��

	u8 *dataPtr;					//鍒濆鍖栦竴涓�8浣嶇殑鎸囬拡dataPtr
	u8 data_counter = 0;			//data counter(鏁版嵁璁℃暟鍣�)
	u8 pec_error = 0;				//妫�鏌ユ閿欑爜
	u16 parsed_stat;				//鍗曚釜鐘舵�佸��
	u16 received_pec;				//鏀跺埌鐨勬瘡涓瘎瀛樺櫒缁勪腑鐨凱EC(妫�閿欑爜)
	u16 data_pec;					//璁＄畻鏀跺埌鐨勬瘡涓瘎瀛樺櫒缁勪腑鎸囦护鏁版嵁鐨凱EC(妫�閿欑爜)
	u8 c_ic = 0;					//褰撳墠姝ｅ湪璇诲叆鏁扮粍鐨�8915A鑺墖搴忓彿
	u8 stat_reg = 1;				//褰撳墠姝ｅ湪璇诲彇鐨勭姸鎬佸瘎瀛樺櫒缁�
	u8 current_ic = 0;				//current_ic is used as the IC counter(褰撳墠IC(8915A)鐢ㄤ綔IC(8915A)璁℃暟鍣�)
	u8 current_gpio = 0;			//褰撳墠姝ｅ湪璇诲叆鐨勭數鍘嬪��
	u8 stateD_byte = 0;				//鐘舵�佸瘎瀛樺櫒鐨勫瓧鑺傝鏁板櫒
	u8 stateD_bit = 0;				//鐘舵�佸瘎瀛樺櫒鐨勪綅璁℃暟鍣�
	dataPtr = (u8 *) malloc((NUM_RX_BYT*total_ic)*sizeof(u8));//璁＄畻鎸囬拡dataPtr闇�瑕佹帴鏀剁殑闀垮害

	if (reg == 0)//濡傛灉閫夋嫨璇诲彇鎵�鏈夌姸鎬佸瘎瀛樺櫒
	{
		for (stat_reg = 1; stat_reg < NUM_STAT_REG + 0x1U; stat_reg++)//#define NUM_STAT_REG	4U,瀵规瘡涓狟F8915A鐨勭姸鎬佺數鍘嬪瘎瀛樺櫒鎵ц涓�娆�
		{
			data_counter = 0;//data counter(鏁版嵁璁℃暟鍣�)
			BF8915A_rdstat_reg(stat_reg, total_ic,dataPtr);//Reads the raw status register data into the dataPtr[] array(灏嗗師濮嬬姸鎬佸瘎瀛樺櫒鏁版嵁璇诲叆dataPtr[]鏁扮粍)

			for (current_ic = 0 ; current_ic < total_ic; current_ic++)//璇ュ惊鐜綔鐢ㄤ负璇诲彇骞惰В鏋愭瘡涓狟F8915A鎵�閫夋嫨鐨勭姸鎬佺數鍘嬪瘎瀛樺櫒
			{
				if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
				{
					c_ic = current_ic;//閫夋嫨姝ｅ簭灏嗙姸鎬佸瘎瀛樺櫒鏀惧叆鏁扮粍
				}
				else
				{
					c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗙姸鎬佸瘎瀛樺櫒鏀惧叆鏁扮粍
				}
				if ((stat_reg > 0) && (stat_reg < 4))//濡傛灉璇诲彇鐨勫瘎瀛樺櫒涓篠TATA銆丼TATB銆丼TATC
				{
					for (current_gpio = 0; current_gpio < GPIO_IN_REG; current_gpio++) // This loop parses the read back data into state voltages, it loops once for each of the 3 state voltage codes in the register
					{                                                                  //(杩欎釜寰幆灏嗚鍥炵殑鏁版嵁瑙ｆ瀽涓虹姸鎬佺數鍘嬶紝瀹冨瀵勫瓨鍣ㄤ腑鐨�3涓姸鎬佺數鍘嬩唬鐮佷腑鐨勬瘡涓�涓惊鐜竴娆�)
						parsed_stat = dataPtr[data_counter] + (dataPtr[data_counter+1]<<8);//Each state codes is received as two bytes and is combined to create the parsed state voltage code
						                                                                   //(姣忎釜鐘舵�佺爜琚帴鏀朵负涓や釜瀛楄妭锛屽苟涓旇缁勫悎鏉ュ垱寤鸿В鏋愮殑鐘舵�佺數鍘嬩唬鐮�)
						stateReg[c_ic].stat_codes[current_gpio + (stat_reg - 1) * GPIO_IN_REG] = parsed_stat;//灏嗚В鏋愬嚭鐨勫崟涓數鍘嬬爜渚濇瀛樻斁鍦ㄦ暟缁剆tateReg[c_ic].stat_codes[]涓�
						data_counter=data_counter+2;//Because cell voltage codes are two bytes the data counter must increment by two for each parsed cell code
																	    	//(鍥犱负鍗曚綋鐢靛帇鐮佹槸涓や釜瀛楄妭锛屾墍浠ュ浜庢瘡涓В鏋愮殑鐢垫睜鐮侊紝鏁版嵁璁℃暟鍣ㄥ繀椤诲鍔�2)
						if((reg == STATC) &&(current_gpio == 0x1U))	//姝ゅ鍒ゆ柇鏈簡闃叉璇籗TATC鏃�,瀵艰嚧StateReg_TypeDef缁撴瀯浣撲腑stat_codes[]鏁扮粍婧㈠嚭闂
						{
							current_gpio = GPIO_IN_REG;//瀵勫瓨鍣ㄤ负STATC涓斿凡缁忓惊鐜埌绗簩娆★紝璺宠繃涓�娆″惊鐜�
							data_counter = data_counter+2;//Because cell voltage codes are two bytes the data counter must increment by two for each parsed cell code
																		        //(鍥犱负鍗曚綋鐢靛帇鐮佹槸涓や釜瀛楄妭锛屾墍浠ュ浜庢瘡涓В鏋愮殑鐢垫睜鐮侊紝鏁版嵁璁℃暟鍣ㄥ繀椤诲鍔�2)
						}
					}
				}
				else if (stat_reg == 4)//濡傛灉璇诲彇鐨勫瘎瀛樺櫒涓篠TATD
				{
					stateReg[c_ic].uvOvFlags[0] = dataPtr[data_counter++];//STDR0鐨�8浣嶏紝Cell1~Cell4鐨勬瑺/杩囧帇鏍囧織
					stateReg[c_ic].uvOvFlags[1] = dataPtr[data_counter++];//STDR1鐨�8浣嶏紝Cell5~Cell8鐨勬瑺/杩囧帇鏍囧織
					stateReg[c_ic].uvOvFlags[2] = dataPtr[data_counter++];//STDR2鐨�8浣嶏紝Cell9~Cell12鐨勬瑺/杩囧帇鏍囧織
					stateReg[c_ic].uvOvFlags[3] = dataPtr[data_counter++];//STDR3鐨�8浣嶏紝Cell12~Cell16鐨勬瑺/杩囧帇鏍囧織
					stateReg[c_ic].cfg_err = ((dataPtr[data_counter] & 0x40) >> 0x6U);	//STDR4鐨勭7浣嶏紝閰嶇疆瀛楁牎楠屽嚭閿欐爣蹇�(鍖呮嫭濂囧伓寮傛垨鏍￠獙鍜孍CC鏍￠獙)
					stateReg[c_ic].pec_err = ((dataPtr[data_counter] & 0x20) >> 0x5U);	//STDR4鐨勭6浣嶏紝鎸囦护鏁版嵁鐨凱EC鏍￠獙鍑洪敊鏍囧織
					stateReg[c_ic].extov = ((dataPtr[data_counter] & 0x10) >> 0x4U);	//STDR4鐨勭5浣嶏紝鐩戞祴妯″紡涓嬶紝澶栭儴娓╁害(gpio)杩囨俯鏍囧織
					stateReg[c_ic].itov = ((dataPtr[data_counter] & 0x08) >> 0x3U);		//STDR4鐨勭4浣嶏紝鐩戞祴妯″紡涓嬶紝鍐呴儴娓╁害(vtemp)杩囨俯鏍囧織
					stateReg[c_ic].htov2 = ((dataPtr[data_counter] & 0x04) >> 0x2U);	//STDR4鐨勭3浣嶏紝纭欢娓╁害(95鈩�)杩囨俯鏍囧織
					stateReg[c_ic].htov1 = ((dataPtr[data_counter] & 0x02) >> 0x1U);	//STDR4鐨勭2浣嶏紝纭欢娓╁害(125鈩�)杩囨俯鏍囧織
					stateReg[c_ic].mux_fail = (dataPtr[data_counter++] & 0x01);			//STDR4鐨勭1浣嶏紝寮�鍏宠瘧鐮佹祴璇曠粨鏋�
					stateReg[c_ic].mode_state = ((dataPtr[data_counter] & 0xC0U) >> 0x6U);//STDR5鐨勭8~7浣嶏紝妯″紡鐘舵�佸瘎瀛樺櫒
					stateReg[c_ic].rst_state = (dataPtr[data_counter++] & 0x3FU);		//STDR5鐨勭6~1浣嶏紝澶嶄綅鏍囧織瀵勫瓨鍣�
				
					stateReg[c_ic].cellOvFlag = 0x00U;	//鐢垫睜娆犲帇鏍囧織浣嶏紝姣忎釜浣嶇敤鏉ユ爣蹇楁瘡涓數姹犵殑娆犲帇鏍囧織浣�
					stateReg[c_ic].cellUvFlag = 0x00U;	//鐢垫睜杩囧帇鏍囧織浣嶏紝姣忎釜浣嶇敤鏉ユ爣蹇楁瘡涓數姹犵殑杩囧帇鏍囧織浣�
					
					for(stateD_byte = 0;stateD_byte < 4;stateD_byte ++)//鐘舵�佸瘎瀛樺櫒鐨勫瓧鑺傚惊鐜�
					{
						for(stateD_bit = 0;stateD_bit < 4;stateD_bit ++)//鐘舵�佸瘎瀛樺櫒鐨勪綅寰幆
						{
							if((stateReg[c_ic].uvOvFlags[stateD_byte] & (0x01 << (stateD_bit * 0x2U))) == (0x01 << stateD_bit * 0x2U))//鍒ゆ柇璇ellx鐨勬瑺鍘嬫爣蹇椾綅鏄惁涓�1
								stateReg[c_ic].cellUvFlag |= 0x0001 << ((stateD_byte * 0x4U) + stateD_bit);//璇ellx鐨勬瑺鍘嬫爣蹇椾綅缃�1
							else
								stateReg[c_ic].cellUvFlag &= ~(0x0001 << ((stateD_byte * 0x4U) + stateD_bit));//璇ellx鐨勬瑺鍘嬫爣蹇椾綅缃�0
							
							if((stateReg[c_ic].uvOvFlags[stateD_byte] & (0x01 << (stateD_bit * 0x2U + 0x1U))) == (0x01 << (stateD_bit * 0x2U + 0x1U)))//鍒ゆ柇璇ellx鐨勮繃鍘嬫爣蹇椾綅鏄惁涓�1
								stateReg[c_ic].cellOvFlag |= 0x0001 << ((stateD_byte * 0x4U) + stateD_bit);//璇ellx鐨勮繃鍘嬫爣蹇椾綅缃�1
							else
								stateReg[c_ic].cellOvFlag &= ~(0x0001 << ((stateD_byte * 0x4U) + stateD_bit));//璇ellx鐨勮繃鍘嬫爣蹇椾綅缃�0
						}
					}					
				}
				received_pec = (dataPtr[data_counter]<<8)+ dataPtr[data_counter+1];//鎺ユ敹鍒扮殑褰撳墠IC(8915A)鐨凱EC(妫�閿欑爜)浣滀负6涓姸鎬佺數鍘嬫暟鎹瓧鑺備箣鍚庣殑绗�7涓拰绗�8涓瓧鑺傚彂閫�
				data_pec = Bf8915pec15_calc(BYT_IN_REG, &dataPtr[current_ic*NUM_RX_BYT]);//璁＄畻鎺ユ敹鍥炴潵鐨勫瘎瀛樺櫒缁�(6涓�8浣嶅瘎瀛樺櫒)涓殑PEC(妫�閿欑爜)

				if (received_pec != data_pec)//妫�鏌ユ閿欑爜鏄惁鍖归厤
				{
					pec_error = 1; 			//濡傛灉鏈変换浣昉EC(妫�閿欑爜)閿欒锛宲ec_error鍙橀噺绠�鍗曞湴璁剧疆涓�1
					stateReg[c_ic].pec_match[stat_reg-1]=1;//stateReg[c_ic].pec_match[stat_reg-1]涓殑妫�閿欑爜鍊肩疆1
				}
				else
				{
					stateReg[c_ic].pec_match[stat_reg-1]=0;//stateReg[c_ic].pec_match[stat_reg-1]涓殑妫�閿欑爜鍊肩疆0
				}
				data_counter = data_counter + 2;//Because the transmitted PEC code is 2 bytes long the data_counter must be incremented by 2 bytes to point to the next ICs state voltage data
																		    //(鍥犱负浼犺緭鐨凱EC(妫�閿欑爜)浠ｇ爜鏄�2瀛楄妭闀匡紝鎵�浠ユ暟鎹鏁板櫒蹇呴』澧炲姞2瀛楄妭鎵嶈兘鎸囧悜涓嬩竴涓泦鎴愮數璺姸鎬佺數鍘嬫暟鎹�)
			}
		}
	}
	else
	{
		BF8915A_rdstat_reg(reg, total_ic, dataPtr);//Reads the raw status register data into the dataPtr[] array(灏嗗師濮嬬姸鎬佸瘎瀛樺櫒鏁版嵁璇诲叆dataPtr[]鏁扮粍)
		for (current_ic = 0 ; current_ic < total_ic; current_ic++) //璇ュ惊鐜綔鐢ㄤ负璇诲彇骞惰В鏋愭瘡涓狟F8915A鎵�閫夋嫨鐨勭姸鎬佺數鍘嬪瘎瀛樺櫒
		{
			if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
			{
				c_ic = current_ic;		//閫夋嫨姝ｅ簭灏嗙數姹犵數鍘嬫斁鍏ユ暟缁�
			}
			else
			{
				c_ic = total_ic - current_ic - 1;//閫夋嫨鍊掑簭灏嗙數姹犵數鍘嬫斁鍏ユ暟缁�
			}
			if ((reg > 0) && (reg <4))	//濡傛灉璇诲彇鐨勫瘎瀛樺櫒涓篠TATA銆丼TATB銆丼TATC
			{
				for (current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++) //This loop parses the read back data into state voltages, it loops once for each of the 3 state voltage codes in the register
				{																		                              //(杩欎釜寰幆灏嗚鍥炵殑鏁版嵁瑙ｆ瀽涓虹姸鎬佺數鍘嬶紝瀹冨瀵勫瓨鍣ㄤ腑鐨�3涓姸鎬佺數鍘嬬爜涓殑姣忎竴涓惊鐜竴娆�)
          
					parsed_stat = dataPtr[data_counter] + (dataPtr[data_counter+1]<<8);//Each state codes is received as two bytes and is combined to create the parsed state voltage code
																					                                   //(姣忎釜鐘舵�佺爜琚帴鏀朵负涓や釜瀛楄妭锛屽苟涓旇缁勫悎鏉ュ垱寤鸿В鏋愮殑鐘舵�佺數鍘嬩唬鐮�)
					stateReg[c_ic].stat_codes[current_gpio + (reg - 1) * GPIO_IN_REG] = parsed_stat;
					data_counter = data_counter+2;//Because state voltage codes are two bytes the data counter must increment by two for each parsed state voltage code
																			  //(鍥犱负鐘舵�佺數鍘嬬爜鏄袱涓瓧鑺傦紝鎵�浠ュ浜庢瘡涓В鏋愮殑鐘舵�佺數鍘嬬爜锛屾暟鎹鏁板櫒蹇呴』澧炲姞2)
					if((reg == STATC) &&(current_gpio == 0x1U))	//姝ゅ鍒ゆ柇鏈簡闃叉璇籗TATC鏃�,瀵艰嚧StateReg_TypeDef缁撴瀯浣撲腑stat_codes[]鏁扮粍婧㈠嚭闂
					{
						current_gpio = GPIO_IN_REG;		//鑻ュ瘎瀛樺櫒涓篠TATC涓斿凡缁忓惊鐜埌绗簩娆★紝鍒欒烦杩囦竴娆″惊鐜�
						data_counter = data_counter+2;	//Because cell voltage codes are two bytes the data counter must increment by two for each parsed cell code
																		      //(鍥犱负鍗曚綋鐢靛帇鐮佹槸涓や釜瀛楄妭锛屾墍浠ュ浜庢瘡涓В鏋愮殑鐢垫睜鐮侊紝鏁版嵁璁℃暟鍣ㄥ繀椤诲鍔�2)
					}
				}
			}
			else if (reg == 4)//濡傛灉璇诲彇鐨勫瘎瀛樺櫒涓篠TATD
			{
				stateReg[c_ic].uvOvFlags[0] = dataPtr[data_counter++];//STDR0鐨�8浣嶏紝Cell1~Cell4鐨勬瑺/杩囧帇鏍囧織
				stateReg[c_ic].uvOvFlags[1] = dataPtr[data_counter++];//STDR1鐨�8浣嶏紝Cell5~Cell8鐨勬瑺/杩囧帇鏍囧織
				stateReg[c_ic].uvOvFlags[2] = dataPtr[data_counter++];//STDR2鐨�8浣嶏紝Cell9~Cell12鐨勬瑺/杩囧帇鏍囧織
				stateReg[c_ic].uvOvFlags[3] = dataPtr[data_counter++];//STDR3鐨�8浣嶏紝Cell12~Cell16鐨勬瑺/杩囧帇鏍囧織
				stateReg[c_ic].cfg_err = ((dataPtr[data_counter] & 0x40) >> 0x6U);	//STDR4鐨勭7浣嶏紝閰嶇疆瀛楁牎楠屽嚭閿欐爣蹇�(鍖呮嫭濂囧伓寮傛垨鏍￠獙鍜孍CC鏍￠獙)
				stateReg[c_ic].pec_err = ((dataPtr[data_counter] & 0x20) >> 0x5U);	//STDR4鐨勭6浣嶏紝鎸囦护鏁版嵁鐨凱EC鏍￠獙鍑洪敊鏍囧織
				stateReg[c_ic].extov = ((dataPtr[data_counter] & 0x10) >> 0x4U);	//STDR4鐨勭5浣嶏紝鐩戞祴妯″紡涓嬶紝澶栭儴娓╁害(gpio)杩囨俯鏍囧織
				stateReg[c_ic].itov = ((dataPtr[data_counter] & 0x08) >> 0x3U);		//STDR4鐨勭4浣嶏紝鐩戞祴妯″紡涓嬶紝鍐呴儴娓╁害(vtemp)杩囨俯鏍囧織
				stateReg[c_ic].htov2 = ((dataPtr[data_counter] & 0x04) >> 0x2U);	//STDR4鐨勭3浣嶏紝纭欢娓╁害(95鈩�)杩囨俯鏍囧織
				stateReg[c_ic].htov1 = ((dataPtr[data_counter] & 0x02) >> 0x1U);	//STDR4鐨勭2浣嶏紝纭欢娓╁害(125鈩�)杩囨俯鏍囧織
				stateReg[c_ic].mux_fail = (dataPtr[data_counter++] & 0x01);			//STDR4鐨勭1浣嶏紝寮�鍏宠瘧鐮佹祴璇曠粨鏋�
				stateReg[c_ic].mode_state = ((dataPtr[data_counter] & 0xC0U) >> 0x6U);//STDR5鐨勭8~7浣嶏紝妯″紡鐘舵�佸瘎瀛樺櫒
				stateReg[c_ic].rst_state = (dataPtr[data_counter++] & 0x3FU);		//STDR5鐨勭6~1浣嶏紝澶嶄綅鏍囧織瀵勫瓨鍣�
				for(stateD_byte = 0;stateD_byte < 4;stateD_byte ++)//鐘舵�佸瘎瀛樺櫒鐨勫瓧鑺傚惊鐜�
				{
					for(stateD_bit = 0;stateD_bit < 4;stateD_bit ++)//鐘舵�佸瘎瀛樺櫒鐨勪綅寰幆
					{
						if((stateReg[c_ic].uvOvFlags[stateD_byte] & (0x01 << (stateD_bit * 0x2U))) == (0x01 << stateD_bit * 0x2U))//鍒ゆ柇璇ellx鐨勬瑺鍘嬫爣蹇椾綅鏄惁涓�1
							stateReg[c_ic].cellUvFlag |= 0x0001 << ((stateD_byte * 0x4U) + stateD_bit);//璇ellx鐨勬瑺鍘嬫爣蹇椾綅缃�1
						else
							stateReg[c_ic].cellUvFlag &= ~(0x0001 << ((stateD_byte * 0x4U) + stateD_bit));//璇ellx鐨勬瑺鍘嬫爣蹇椾綅缃�0

						if((stateReg[c_ic].uvOvFlags[stateD_byte] & (0x01 << (stateD_bit * 0x2U + 0x1U))) == (0x01 << (stateD_bit * 0x2U + 0x1U)))//鍒ゆ柇璇ellx鐨勮繃鍘嬫爣蹇椾綅鏄惁涓�1
							stateReg[c_ic].cellOvFlag |= 0x0001 << ((stateD_byte * 0x4U) + stateD_bit);//璇ellx鐨勮繃鍘嬫爣蹇椾綅缃�1
						else
							stateReg[c_ic].cellOvFlag &= ~(0x0001 << ((stateD_byte * 0x4U) + stateD_bit));//璇ellx鐨勮繃鍘嬫爣蹇椾綅缃�0
					}
				}
			}
			received_pec = (dataPtr[data_counter]<<8)+ dataPtr[data_counter+1];//The received PEC for the current_ic is transmitted as the 7th and 8th after the 6 gpio voltage data bytes
																		                                     //(鎺ユ敹鍒扮殑闆嗘垚鐢佃矾璁℃暟鍣ㄧ殑妫�閿欑爜浣滀负6涓狦PIO鐢靛帇鏁版嵁瀛楄妭涔嬪悗鐨勭7涓拰绗�8涓瓧鑺傚彂閫�)
			data_pec = Bf8915pec15_calc(BYT_IN_REG, &dataPtr[current_ic*NUM_RX_BYT]);//璁＄畻鎺ユ敹鍥炴潵鐨勫瘎瀛樺櫒缁�(6涓�8浣嶅瘎瀛樺櫒)涓殑PEC(妫�閿欑爜)
			if (received_pec != data_pec)//妫�鏌ユ閿欑爜鏄惁鍖归厤
			{
				pec_error = 1;//濡傛灉鏈変换浣昉EC(妫�閿欑爜)閿欒锛宲ec_error鍙橀噺绠�鍗曞湴璁剧疆涓�1
				stateReg[c_ic].pec_match[reg-1]=1;//stateReg[c_ic].pec_match[stat_reg-1]涓殑妫�閿欑爜鍊肩疆1
			}
			data_counter = data_counter + 2;//Because the transmitted PEC code is 2 bytes long the data_counter must be incremented by 2 bytes to point to the next ICs state voltage data
																		  //(鍥犱负浼犺緭鐨凱EC(妫�閿欑爜)浠ｇ爜鏄�2瀛楄妭闀匡紝鎵�浠ユ暟鎹鏁板櫒蹇呴』澧炲姞2瀛楄妭鎵嶈兘鎸囧悜涓嬩竴涓泦鎴愮數璺姸鎬佺數鍘嬫暟鎹�)
		}
	}
	free(dataPtr);//娓呯┖鎸囬拡
	return (pec_error);//鍥炰紶妫�鏌ユ閿欑爜鐨勫��
}

/*!
    \brief      Clear Cell Register(娓呴櫎鐢垫睜鐢靛帇瀵勫瓨鍣�)
		\param[in]  none
    \retval     none
*/
void BF8915A_clrcell(void)
{
	u8 cmd[2]= {0x00 , 0x81};
	BF8915A_cmd(cmd);//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      Clear GPIO Register(娓呴櫎GPIO瀵勫瓨鍣�)
		\param[in]  none
    \retval     none
*/
void BF8915A_clrgp(void)
{
	u8 cmd[2]= {0x00 , 0x82};
	BF8915A_cmd(cmd);//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      Clear STATE Register(娓呴櫎鐘舵�佸瘎瀛樺櫒)
		\param[in]  none
    \retval     none
*/
void BF8915A_clrstat(void)
{
	u8 cmd[2]= {0x00 , 0x83};
	BF8915A_cmd(cmd);//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}

/*!
    \brief      PLADC CMD(杞ADC杞崲鐘舵�佹寚浠�)
		\param[in]  none
    \retval     none
*/
u8 BF8915A_pladc(void)
{
	u8 cmd[4];			//鍒濆鍖栨暟缁刢md[]
	u8 adc_state = 0xFF;//鍒濆鍖朼dc_state
	u16 cmd_pec;		//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)

	cmd[0] = 0x00;		//鍙戦�佹寚浠ょ殑楂�8浣�
	cmd[1] = 0x85;		//鍙戦�佹寚浠ょ殑浣�8浣�
	cmd_pec = Bf8915pec15_calc(2, cmd);//璁＄畻鍑哄彂閫佹寚浠ょ殑PEC(妫�閿欑爜)
	cmd[2] = (u8)(cmd_pec >> 8);//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勯珮鍏綅
	cmd[3] = (u8)(cmd_pec);//鍙戦�佹寚浠ょ殑PEC(妫�閿欑爜)鐨勪綆鍏綅
	
	if (isospi_reverse == FALSE)//鍒ゆ柇閫夋嫨鐨勮強鑺遍摼閫氫俊鏂瑰悜鏄浉瀵规鍚戣繕鏄浉瀵瑰弽鍚�
    {
		BF8915_CS_LOW();		//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_array(4,cmd);	//灏嗗瓧鑺傛暟缁勫啓鍏PI绔彛
		adc_state = SPI1_ReadWriteByte(0xFF);//杩斿洖SPI鎺ユ敹鍒扮殑鏁版嵁
		if(adc_state == 0xff)
		{
			BF8915_CS_HIGH();	//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
		}
    }
    else
    {
		BF8915_CS_LOW_Reverse();//灏嗙墖閫塁S璁剧疆涓轰綆鐢靛钩
		spi_write_array(4,cmd);	//灏嗗瓧鑺傛暟缁勫啓鍏PI绔彛
		adc_state = SPI2_ReadWriteByte(0xFF);//杩斿洖SPI鎺ユ敹鍒扮殑鏁版嵁
	    if(adc_state == 0xff)
	    {
	    	BF8915_CS_HIGH_Reverse();//灏嗙墖閫塁S璁剧疆涓洪珮鐢靛钩
	    }
    }
	return(adc_state);//鍥炰紶SPI鎺ユ敹鍒扮殑鏁版嵁
}

/*!
    \brief      soft reset(杞欢澶嶄綅)
		\param[in]  none
    \retval     none
*/
void BF8915A_softrst(void)
{
	u8 cmd[2]= {0x00 , 0x55};
	BF8915A_cmd(cmd);//缂栧啓BF8915A鍛戒护鐨勯�氱敤鍑芥暟銆傚嚱鏁颁负cmd鏁版嵁璁＄畻浜嗘閿欑爜
}
