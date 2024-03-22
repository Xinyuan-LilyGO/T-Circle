/**
 *Name        : tp_hyn.c
 */

/******************************************************************
**头文件
******************************************************************/
#include "ms_customize_trc.h"
#include "i2c_api.h"
#include "tp_multi_cfg.h"
#include "tp_cst716se.h"
#include "gpio_prod_api.h"
#include "gpio_drv.h"

/******************************************************************
**函数extern
******************************************************************/
extern PUBLIC uint32 OS_TickDelay(uint32 ticks);

/******************************************************************
**宏定义
******************************************************************/
//芯片升级
//#define HYN_EN_AUTO_UPDATE_CST716
#define HYN_EN_AUTO_UPDATE_CST716SE_B
//#define HYN_EN_AUTO_UPDATE_CST816s

//触摸芯片升级I2C地址
#define TP_HYN_I2C_UPDATE_WRITE_ADDR       (0x6A<<1)  //0x15
#define TP_HYN_I2C_UPDATE_READ_ADDR        ((0x6A<<1)|1)

//触摸I2C端口获取
#define TP_I2C_PORT     TPC_GetCtrlPort()

//触摸芯片I2C地址
#define TP_IIC_ADDR     (0x15)//(0x38)
#define TP_WRITE_ADDR   (TP_IIC_ADDR<<1)         
#define TP_READ_ADDR    ((TP_IIC_ADDR<<1)+1)

//芯片升级使用
#if defined(HYN_EN_AUTO_UPDATE_CST716SE_B)||defined(HYN_EN_AUTO_UPDATE_CST716)||defined(HYN_EN_AUTO_UPDATE_CST816s)
#define REG_LEN_1B    1
#define REG_LEN_2B    2
#endif


/******************************************************************
**全局变量定义
******************************************************************/
//I2C总线配置
LOCAL I2C_DEV HYN_I2C_cfg =
{
	2,          
	100000,                   // freq is standard for FT5206
	0,                          // bus
	TP_WRITE_ADDR,  // slave_addr
	1,                         // reg_addr_num
	1,                          // check_ack
	1                           // no_stop
};

//芯片升级I2C总线配置
#if defined(HYN_EN_AUTO_UPDATE_CST716SE_B)||defined(HYN_EN_AUTO_UPDATE_CST716)
LOCAL I2C_DEV HYN_I2C_fm_cfg =
{
	2,          
	100000,                   // freq is standard for FT5206
	0,                          // bus
	TP_HYN_I2C_UPDATE_WRITE_ADDR,  // slave_addr
	2,                          // reg_addr_num
	1,                          // check_ack
	1                           // no_stop
};
#endif

//I2C-handler定义
LOCAL int32 HYN_I2C_Handler = -1;

//芯片升级I2C总线配置
#if defined(HYN_EN_AUTO_UPDATE_CST716SE_B)||defined(HYN_EN_AUTO_UPDATE_CST716)
//升级I2C-handler定义
LOCAL int32 HYN_FM_I2C_Handler = -1;
#endif


/******************************************************************
**函数预定义
******************************************************************/
LOCAL uint32 _Get_HYN_I2C_Status(void);
LOCAL uint32 HYN_I2C_Close(void);
LOCAL uint8 HYN_get_key(void);
LOCAL uint32 HYN_I2C_Open(void);
#if defined(HYN_EN_AUTO_UPDATE_CST716SE_B)||defined(HYN_EN_AUTO_UPDATE_CST716)
LOCAL uint32 HYN_Update_I2C_Status(void);
#endif


/******************************************************************
**函数实体区
******************************************************************/
#if defined(HYN_EN_AUTO_UPDATE_CST716SE_B)||defined(HYN_EN_AUTO_UPDATE_CST716)||defined(HYN_EN_AUTO_UPDATE_CST816s)
/*****************************************************************/
/*
 *
 */
LOCAL uint32 HYN_Update_I2C_Open(void)
{
	HYN_I2C_fm_cfg.id =  TP_I2C_PORT;

	if(HYN_FM_I2C_Handler == -1) 
	{
		if((HYN_FM_I2C_Handler = I2C_HAL_Open(&HYN_I2C_fm_cfg)) == -1)
		{
			return TP_MULTI_ERROR;    
		}
		else
		{
			return TP_MULTI_SUCCESS;
		}
	}
	
	return TP_MULTI_SUCCESS;
}

/*****************************************************************/
/*
 *
 */
LOCAL uint32 HYN_Update_I2C_Close(void)
{
	if(HYN_Update_I2C_Status() == TP_MULTI_SUCCESS)
	{
		I2C_HAL_Close(HYN_FM_I2C_Handler);
		HYN_FM_I2C_Handler = -1;
	}	

	return TP_MULTI_SUCCESS;
}

/*****************************************************************/
/*
 *
 */
LOCAL uint32 HYN_Update_I2C_Status(void)
{
	if(HYN_FM_I2C_Handler==-1)
	{
	    SCI_TRACE_LOW("HYN_Update_I2C_Status: fail");		
		return TP_MULTI_ERROR;
	}
	else
	{
	    SCI_TRACE_LOW("HYN_Update_I2C_Status: success");		
		return TP_MULTI_SUCCESS;
	}
}

/*****************************************************************/
/*
 *
 */
LOCAL int hctp_write_bytes(uint16 reg,uint8 *buf,uint16 len,uint8 reg_len)
{
	int ret=0;
	uint8 reg_num[2];

	reg_num[1] = reg&0xff;
	reg_num[0] = (reg>>8)&0xff;
	SCI_TRACE_LOW("hctp_write_bytes:reg_num[0] = 0x%x",reg_num[0]);		
	SCI_TRACE_LOW("hctp_write_bytes:reg_num[1] = 0x%x",reg_num[1]);		
	if(HYN_Update_I2C_Status() == TP_MULTI_SUCCESS)
	{		
	    SCI_TRACE_LOW("hctp_write_bytes:buf[0] = 0x%x",buf[0]);				
		ret = I2C_HAL_Write(HYN_FM_I2C_Handler, reg_num, buf, len);
	    SCI_TRACE_LOW("hctp_write_bytes1:buf[0] = 0x%x",buf[0]);					
	    SCI_TRACE_LOW("hctp_write_bytes:ret = %d",ret);				
	}
	
	return ret;
}

/*
 *
 */
LOCAL int hctp_read_bytes(uint16 reg,uint8* buf,uint16 len,uint8 reg_len)
{
	int ret=0;
	uint8 reg_num[2];

	reg_num[1] = reg&0xff;
	reg_num[0] = (reg>>8)&0xff;

	SCI_TRACE_LOW("hctp_read_bytes:reg_num[0] = 0x%x",reg_num[0]);		
	SCI_TRACE_LOW("hctp_read_bytes:reg_num[1] = 0x%x",reg_num[1]);		
	if(HYN_Update_I2C_Status() == TP_MULTI_SUCCESS)
	{
	    SCI_TRACE_LOW("hctp_read_bytes:buf[0] = 0x%x",buf[0]);		
		
		ret = I2C_HAL_Read(HYN_FM_I2C_Handler,reg_num, buf, len); 
		
	    SCI_TRACE_LOW("hctp_read_bytes1:buf[0] = 0x%x",buf[0]);		
	    SCI_TRACE_LOW("hctp_read_bytes:ret = %d",ret);			
	}
	
	return ret;
}

/*****************************************************************/

/*
** Parameters: void
** Return Values: 0: enter cst816s boot mode success -1:enter cst816s boot mode fail
*/
LOCAL int32 cst816s_enter_bootmode(void)
{
	uint8 retryCnt=10;
	
	GPIO_ResetTP(1,10);
	
	//SCI_Sleep(10);
	
	while(retryCnt--)
	{
		uint8 cmd[3];

		#ifdef HYN_EN_AUTO_UPDATE_CST816s
			cmd[0] = 0xAB;
		#elif defined(HYN_EN_AUTO_UPDATE_CST716SE_B)		
			cmd[0] = 0xAB;
		#elif defined(HYN_EN_AUTO_UPDATE_CST716)
			cmd[0] = 0xAA;
		#endif
		SCI_TRACE_LOW("cst816s_enter_bootmode: start = 0x%x",cmd[0]);

		// enter program mode
		if (-1 == hctp_write_bytes(0xA001,cmd,1,REG_LEN_2B))
		{
            SCI_TRACE_LOW("cst816s_enter_bootmode: 0xA001");		
			SCI_Sleep(4);
			continue;                   
		}

		// read flag
		if (-1 == hctp_read_bytes(0xA003,cmd,1,REG_LEN_2B)) 
		{
            SCI_TRACE_LOW("cst816s_enter_bootmode: 0xA003");		
			SCI_Sleep(4);
			continue;                           
		}
		else
		{
			if (cmd[0] != 0x55)
			{
                SCI_TRACE_LOW("cst816s_enter_bootmode: contine");
                SCI_TRACE_LOW("cst816s_enter_bootmode: fail cmd[0] = 0x%x",cmd[0]);					
				SCI_Sleep(4);
				continue;
			}
			else
			{	
                SCI_TRACE_LOW("cst816s_enter_bootmode: success cmd[0] = 0x%x",cmd[0]);				
				return 0;
			}
		}
	}

	SCI_TRACE_LOW("cst816s_enter_bootmode: return -1"); 

	return -1;
}

 /*
  *
  */
LOCAL void cst816s_update(uint16 startAddr, uint16 length, uint8 *data)
{
	uint16 sum_len;
	uint8 cmd[10];

	if (cst816s_enter_bootmode() == -1)
	{
		return ;
	}
	
	sum_len = 0;

	#define PER_LEN	512

	do{
		if (sum_len >= length)
		{
			return;
		}

		// send address
		cmd[0] = startAddr&0xFF;
		cmd[1] = startAddr>>8;
		
		hctp_write_bytes(0xA014,cmd,2,REG_LEN_2B);
		hctp_write_bytes(0xA018,data,PER_LEN,REG_LEN_2B);

		cmd[0] = 0xEE;
		
		hctp_write_bytes(0xA004,cmd,1,REG_LEN_2B);
		
		SCI_Sleep(100);

		{
			uint8 retrycnt = 50;
			
			while(retrycnt--)
			{
				cmd[0] = 0;
				
				hctp_read_bytes(0xA005,cmd,1,REG_LEN_2B);
                SCI_TRACE_LOW("cst716se_update: Chip Id start = 0x%x",cmd[0]);	

				//success
				if (cmd[0] == 0x55)
				{
                    SCI_TRACE_LOW("cst716se_update: Chip Id end = 0x%x",cmd[0]);					
					break;
				}
				
				SCI_Sleep(20);
			}
		}
		
		startAddr += PER_LEN;
		
		data += PER_LEN;
		
		sum_len += PER_LEN;
	}while(1);

	// exit program mode
	cmd[0] = 0x00;
	hctp_write_bytes(0xA003,cmd,1,REG_LEN_2B);
}
 
/*
 *
 */
LOCAL uint16 cst816s_read_checksum(uint16 startAddr,uint16 length)
{
	union
	{
		uint32 sum;
		uint8 buf[4];
	}checksum;
	
	char cmd[3];
	char readback[4] = {0};

	if (cst816s_enter_bootmode() == -1)
	{
		return -1;
	}

	cmd[0] = 0;
	
	if (-1 == hctp_write_bytes(0xA003,cmd,1,REG_LEN_2B))
	{
		return -1;
	}
	
	SCI_Sleep(500);

	checksum.sum = 0;
	
	if (-1 == hctp_read_bytes(0xA008,checksum.buf,2,REG_LEN_2B))
	{
		return -1;
	}

	return checksum.sum;
}

LOCAL int32 ctp_hynitron_update(void)
{
	//close data transfer I2C
	HYN_I2C_Close();

	SCI_Sleep(100);
	
	//change the iic addr to 0x15
	HYN_Update_I2C_Open();
	
	if(HYN_Update_I2C_Status() == TP_MULTI_ERROR)
	{
	    SCI_TRACE_LOW("ctp_hynitron_update: fail"); 	
		return TP_MULTI_ERROR;    
	}
	
	SCI_TRACE_LOW("ctp_hynitron_update: start"); 

	if (cst816s_enter_bootmode() == 0)
	{ 
		//SCI_TRACE_LOW("cst816s_enter_bootmode: start"); 	
		#if 0//def HYN_EN_AUTO_UPDATE_CST816s
			#include "capacitive_hynitron_cst816s_update.h"
        #elif defined(HYN_EN_AUTO_UPDATE_CST716SE_B)
			#include "capacitive_hynitron_cst716se_b_update.h"
		#elif defined(HYN_EN_AUTO_UPDATE_CST716)
			#include "capacitive_hynitron_cst7xx_update.h"
		#endif
		SCI_TRACE_LOW("cst816s_enter_bootmode: entry"); 	

		if(sizeof(app_bin) > 10)
		{
			uint16 startAddr = app_bin[1];
			uint16 length = app_bin[3];
			uint16 checksum = app_bin[5];
			
			startAddr <<= 8; startAddr |= app_bin[0];
			length <<= 8; length |= app_bin[2];
			checksum <<= 8; checksum |= app_bin[4];
			
			SCI_TRACE_LOW("ctp_hynitron_update: app_bin"); 	
			if(cst816s_read_checksum(startAddr, length) != checksum)
			{
				SCI_TRACE_LOW("ctp_hynitron_update: read_checksum"); 	
				cst816s_update(startAddr, length, app_bin+6);
				
				length = cst816s_read_checksum(startAddr, length);
                SCI_TRACE_ID(TRACE_TOOL_CONVERT,"ctp_hynitron_update",(uint8*)"d",length );	
			}
		}

		goto re;
	}

re:
	//close tp data update I2C
	HYN_Update_I2C_Close();

	//SCI_Sleep(100);

	
	if(_Get_HYN_I2C_Status() == TP_MULTI_ERROR)
	{
		return TP_MULTI_ERROR;    
	}

	return 0;	
}
#endif

/******************************************************************************/
// Description: HYN_I2C_Open
// Dependence: 
// Note:
/******************************************************************************/
LOCAL uint32 HYN_I2C_Open(void)
{
	HYN_I2C_cfg.id =  TP_I2C_PORT;

	if(HYN_I2C_Handler == -1) 
	{
		if((HYN_I2C_Handler = I2C_HAL_Open(&HYN_I2C_cfg)) == -1)
		{
	        SCI_TRACE_LOW("tp_cst716se_test: tp is open fail");			
			return TP_MULTI_ERROR;    
		}
		else
		{
	        SCI_TRACE_LOW("tp_cst716se_test: tp is open success");				
			return TP_MULTI_SUCCESS;
		}
	}
	else
	{
		return TP_MULTI_SUCCESS;
	}
}

/******************************************************************************/
// Description: HYN_I2C_Close
// Dependence: 
// Note:
/******************************************************************************/
LOCAL uint32 HYN_I2C_Close(void)
{
	if(_Get_HYN_I2C_Status() == TP_MULTI_SUCCESS)
	{
		I2C_HAL_Close(HYN_I2C_Handler);
		HYN_I2C_Handler = -1;
	}	
	
    SCI_TRACE_LOW("tpcst716se_test:tp i2c is start close");
	
	return TP_MULTI_SUCCESS;
}

/******************************************************************************/
// Description: HYN_I2C_Write
// Dependence: 
// Note:
/******************************************************************************/
LOCAL void HYN_I2C_Write(uint8 *reg, uint8 *addr, uint32 cnt)
{
	if(_Get_HYN_I2C_Status() == TP_MULTI_SUCCESS)
	{
		I2C_HAL_Write(HYN_I2C_Handler, reg, addr, cnt);
	}
}

/******************************************************************************/
// Description: HYN_I2C_Read
// Dependence: 
// Note:
/******************************************************************************/
LOCAL void HYN_I2C_Read(uint8 *reg, uint8 *addr, uint32 cnt)
{
	if(_Get_HYN_I2C_Status() == TP_MULTI_SUCCESS)
	{
		I2C_HAL_Read(HYN_I2C_Handler,reg, addr, cnt);
	}
}

/******************************************************************************/
// Description: _Get_HYN_I2C_Status
// Dependence:  It can get the status from i2c
// Note:
/******************************************************************************/
LOCAL uint32 _Get_HYN_I2C_Status(void)
{
	if(HYN_I2C_Handler==-1)
	{
		return TP_MULTI_ERROR;
	}
	else
	{
		return TP_MULTI_SUCCESS;
	}
}

#ifdef __CUSTOM_DRV_TP_POWER_USE_SD_CARD__	
#include "ldo_drvapi.h"
extern PUBLIC void TPisPowerOn(void);
#endif
/******************************************************************************/
// Description: HYN_Initial
// Dependence: 
// Note:
/******************************************************************************/
LOCAL uint32 HYN_Initial(void)
{
	uint32 delay_time = TP_MAX_SLEEPTIME;

	//default is ok,if HYN_Deinitial called,then initial must be reset.
#ifdef __CUSTOM_DRV_TP_POWER_USE_SD_CARD__	 
    //TPisPowerOn();
    LDO_SetVoltValue(LDO_LDO_SD, 2800);
    LDO_SetVoltValue(LDO_LDO_SDIO, 2800);
    LDO_TurnOnLDO(LDO_LDO_SD);
    LDO_TurnOnLDO(LDO_LDO_SDIO);   
	//SCI_Sleep(20);
#endif

	GPIO_ResetTP(1,10);
	HYN_I2C_Open();
	SCI_TRACE_LOW("tp_cst716se_test: tp is open");
	if(_Get_HYN_I2C_Status() == TP_MULTI_SUCCESS)
	{
		#if defined(HYN_EN_AUTO_UPDATE_CST716SE_B)||defined(HYN_EN_AUTO_UPDATE_CST716)
			ctp_hynitron_update();
		#endif
		
		GPIO_ResetTP(1,10 ); //复位芯片，延时0秒
		
			//change iic addr to 0x38
	    HYN_I2C_Open();
		SCI_TRACE_LOW("tp_cst716se_test: tp is read success");

		return TP_MULTI_SUCCESS;
	}
	else
	{
		SCI_TRACE_LOW("tp_cst716se_test: tp is read fail");
		return TP_MULTI_ERROR;
	}    
}

/*
 *
 */
LOCAL uint8 HYN_get_key(void)
{
	uint8 regData[6]={0,0};
	uint8 reg =2;
	
	HYN_I2C_Read(&reg,regData, 6);//here read two registers, gesture_id, finger_num
	
	return regData[0];
}

/******************************************************************************/
// Description: HYN_Read
// Dependence:  GETTING the data about touchpanel. the up-layer calls the 
//              function to get the data.
// Note:
/******************************************************************************/
LOCAL uint32 HYN_Read(TPDSVR_SIG_T *data)
{
	uint8 regData[6]={0,0};
	uint8 reg =2;
	
	HYN_I2C_Read(&reg,regData, 6);//here read two registers, gesture_id, finger_num
	
    SCI_TRACE_LOW("HYN_Read is Entry regData[1]=0x%x,regData[2]=0x%x,regData[3]=0x%x,regData[4]=0x%x,regData[5]=0x%x,regData[6]=0x%x",regData[1],regData[2],regData[3],regData[4],regData[5],regData[6]);
	
	data->x_key = regData[2];
	data->y_key = regData[4];
	data->cur_index = 0;

	data->gesture_type = 0;//HYN_get_key();//tpc_gesture_id;
	data->num_of_point = regData[0];

	switch(regData[1]&0xC0)
	{
		case 0x00:
			data->SignalCode = TP_DOWN_MSG; 
			break;
			
		case 0x80:
			data->SignalCode = TP_MOVE_MSG;
			break;
			
		case 0x40:
			data->SignalCode = TP_UP_MSG;
			break;
			
		default:
			data->SignalCode = TP_UP_MSG;
			break;
	}
	
}

/******************************************************************************/
// Description: HYN_Open
// Dependence:  Open capacitive touchpanel.
// Note:
/******************************************************************************/
LOCAL int HYN_Open(void)
{
	return TP_MULTI_SUCCESS;
}
/******************************************************************************/
// Description: HYN_Write
// Dependence:  
// Note:
/******************************************************************************/
LOCAL int HYN_Write(void)
{
	return TP_MULTI_SUCCESS;
}

/*
 *
 */
LOCAL uint32 HYN_Ioctl(uint32 cmd, void *arg)
{
    BOOLEAN gpio_interrupt_value;
     uint8 reg_addr,val_read, val_write;
     TP_SPEED_MODE_E speed_mode;
    switch(cmd)
    {
        case TP_IOCTL_CMD_SPEED_MODE:
            speed_mode = *(TP_SPEED_MODE_E *)arg;
            gpio_interrupt_value = GPIO_GetValue(GPIO_TPInterruptPin());
            while((uint16)!gpio_interrupt_value == GPIO_TPGetInterruptStatus())
            {
                reg_addr = REG_PERIODACTIVE;
                if(speed_mode == TP_SPEED_MODE_FAST)
                    val_write = 6;// about only 100uA
                 else 
                    val_write = 4;
                HYN_I2C_Write(&reg_addr, &val_write, 1);
				 
                SCI_TRACE_LOW("HYN_Ioctl is Entry val_write= 0x%x",val_write);

					 
                break;
            }
            break;
        case TP_IOCTL_GET_INT_TIME:
            
            reg_addr = REG_PERIODACTIVE;
            
            HYN_I2C_Read(&reg_addr, &val_read, 1);
            
            *((uint8 *)arg) = 100/val_read;
            SCI_TRACE_LOW(" HYN_Ioctlvalue freq_cycle time = 0x%x\n",arg);
  
            //SCI_TRACE_ID(TRACE_TOOL_CONVERT,TP_FT5206_689_112_2_18_0_35_3_2085,(uint8*)"d", *((uint8 *)arg));
            
            break;

        case TP_IOCTL_SET_THRES_ZOOM:
            reg_addr = REG_ZOOM_DISTANCE;
            val_write = *((uint16 *)arg);
            HYN_I2C_Write(&reg_addr, &val_write, 1);
            HYN_I2C_Read(&reg_addr, &val_read, 1);
            //SCI_TRACE_LOW:"value zoom_distance = 0x%x\n"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,TP_FT5206_698_112_2_18_0_35_3_2086,(uint8*)"d", val_read);
            break;

            default:
                break;
    }
    return TP_MULTI_SUCCESS;
}


/******************************************************************************/
// Description: HYN_Suspend
// Dependence:  When the phone enter sleep mode for saving.
// Note:
/******************************************************************************/
LOCAL void HYN_Suspend(void)
{
	uint8 reg_addr, val_write;

	reg_addr = 0xA5;
	val_write = 0x03;
	
	HYN_I2C_Write(&reg_addr, &val_write, 1);
	
    SCI_TRACE_LOW("HYN_Suspend is Entry reg_addr=0x%x,val_write=0x%x",reg_addr,val_write);	
}

/******************************************************************************/
// Description: HYN_Resume
// Dependence:  
// Note:
/******************************************************************************/
LOCAL void HYN_Resume(void)
{
	//GPIO_ResetTP(1,10);

    uint8 reg_addr, val_read;
    uint32 delay_time = TP_MAX_SLEEPTIME+5;
	uint32 i =0;

	/*CHIP WAKE PIN  least time is 5ms in LOW level when reset according to SPEC.*/
	GPIO_ResetTP(1,10);
	
  	reg_addr = REG_CHIP_MODE;
	HYN_I2C_Read(&reg_addr, &val_read, 1);
	
	SCI_TRACE_LOW("TP: HYN_Resume 0x%x \r\n", val_read);	

	/*here ,if tp wake failure, do it(suspend/resume) again */
	
	for(i=0; i<5; i++)
	{
		if(val_read != 0x0)//work mode
		{
			HYN_Suspend();
			OS_TickDelay(10);
			GPIO_ResetTP(1,10 );
			reg_addr = REG_CHIP_MODE;
			HYN_I2C_Read(&reg_addr, &val_read, 1);
			if(val_read != 0x0)
			{
				SCI_TRACE_LOW("HYN_Resume is Entry val_read=0x%x",val_read);	
			}
			//if(val_read != 0x0)
			//{
			//	SCI_PASSERT(SCI_FALSE, ("reg 0xa5 =  %d", val_read));
			//}
		}
		else
		{
			break;
		}
	}

}

/******************************************************************************/
// Description: HYN_Close
// Dependence:  Close capacitive touchpanel.
// Note:
/******************************************************************************/
LOCAL int HYN_Close(void)
{
	HYN_I2C_Close();
	
	return TP_MULTI_SUCCESS;
}

/******************************************************************************/
// Description: HYN_Deinitial
// Dependence:  Deinitial capacitive touchpanel.
// Note:
/******************************************************************************/
LOCAL int HYN_Deinitial(void)
{
	HYN_I2C_Close();
	
	return TP_MULTI_SUCCESS;
}

//展讯触摸驱动接口
PUBLIC TPC_OPERATION_T tpc_cst716se_ops = {
	HYN_Initial,
	HYN_Open,
	HYN_Write,
	HYN_Read,
	HYN_Ioctl,
	HYN_Suspend,
	HYN_Resume,
	HYN_Close,
	HYN_Deinitial   
};


PUBLIC uint32 TpCst716se_test(void)
{
   SCI_TRACE_LOW("tpcst716se_test: tp start init!!");
   HYN_Initial();
   SCI_TRACE_LOW("tpcst716se_test:tp start init end!!");
   //HYN_I2C_Open();
   //SCI_TRACE_LOW("tpcst716se_test:tp i2c is open");
   //HYN_I2C_Close();
   //SCI_TRACE_LOW("tpcst716se_test:tp i2c is close");   
}

