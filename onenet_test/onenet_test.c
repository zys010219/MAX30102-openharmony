
#include <stdio.h>
#include <unistd.h>
#include "MQTTClient.h"
#include "onenet.h"
#include "max30102_hello.h"


#include "iot_gpio.h"
#include "hi_gpio.h"
#include "hi_io.h"
#define ONENET_INFO_DEVID "963010225"
#define ONENET_INFO_AUTH "20220624"
#define ONENET_INFO_APIKEY "v7H3If9mvIoZattl4GWyqXm0Gkg="
#define ONENET_INFO_PROID "530853"
#define ONENET_MASTER_APIKEY "bxyUEfSHNvAwcKke9Dy=UaPU61I="

extern int rand(void);
uint8_t IR_channel_data, RED_channel_data;

void onenet_cmd_rsp_cb(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size)
{
    printf("recv data is %.*s\n", recv_size, recv_data);

    *resp_data = NULL;
    *resp_size = 0;
}

int onenet_test(void)
{
	
    device_info_init(ONENET_INFO_DEVID, ONENET_INFO_PROID, ONENET_INFO_AUTH, ONENET_INFO_APIKEY, ONENET_MASTER_APIKEY);
    onenet_mqtt_init();

    onenet_set_cmd_rsp_cb(onenet_cmd_rsp_cb);
    
    IoTGpioInit(MAX_SDA_IO0);
	IoTGpioInit(MAX_SCL_IO1);
	hi_io_set_func(MAX_SDA_IO0, HI_IO_FUNC_GPIO_0_I2C1_SDA);
    hi_io_set_func(MAX_SCL_IO1, HI_IO_FUNC_GPIO_1_I2C1_SCL);
    hi_i2c_init(MAX_I2C_IDX, MAX_I2C_BAUDRATE);
	IoTGpioInit(11);
    IoTGpioSetDir(11,IOT_GPIO_DIR_OUT);   //==led初始化

    //成功的话，可读取ID REG_PART_ID = %d \n
	max30102_init();  //看输出Failed 还是Successful



	while (1)
    {
		//int value = 0;
		
        //value = rand() % 100;
        max30102_FIFO_Read_Data( & RED_channel_data, & IR_channel_data);		
        IoTGpioSetOutputVal(11,1); 
        usleep(300000); 
		
		printf("HR=%i, \r\n", RED_channel_data); 

        int ht = RED_channel_data;
        int bo = IR_channel_data;
        
            onenet_mqtt_upload_digit("heartrate",ht);


            onenet_mqtt_upload_digit("bloodoxygen",bo);

        
        
        
        /*

        if (onenet_mqtt_upload_digit("bloodoxygen", bo) < 0)
        {
            printf("upload has an error, stop uploading");
            //break;
        }
        else
        {
            printf("buffer : {\"bloodoxygen\":%d} \r\n", bo);
        }
        */

       
        IoTGpioSetOutputVal(11,0); 
        usleep(300000);  
        sleep(1);
    }
	return 0;
}
