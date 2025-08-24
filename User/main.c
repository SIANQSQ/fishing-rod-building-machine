#include "stm32f10x.h"    
#include "Delay.h"
#include "sys.h"
#include "usart.h"
#include "OLED.h"
#include "AD.h"
#include "pul.h"
#include "MotorSetting.h"
#include "MotorControl.h"

#define delay_ms Delay_ms
#define delay_us Delay_us

//电机全局变量
u8 Start_Flag=1; //启动标志位
u16 Pluse_High=1; //高电平次数
u16 Pluse_Period=1;//脉冲周期中断次数

u32 TimeCount=0;//计数器
extern uint16_t AD_Value[2];
int Mode=0;//默认0：手动
int SpeedMap(int spd);
void mtor(int speed)
{
	delay_us(2);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	delay_us(speed);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

int SPD=50;  //全局速度变量 0-100



/* ADC消抖处理 */
#define ADC_SAMPLES 8 //采样次数
#define HYSTERESIS 15 // 滞回区间值

uint16_t adc_buffer[ADC_SAMPLES];
uint16_t filtered_adc = 0;

#define NUM_LEVELS 85//32   //挡位数
uint8_t current_level = 0;
u8 last_reported_level;
uint8_t current;
//移动平均滤波
uint16_t filter_adc(uint16_t new_sample) {
    static uint8_t index = 0;
    static uint32_t sum = 0;
    
    sum -= adc_buffer[index];
    sum += new_sample;
    adc_buffer[index] = new_sample;
    index = (index + 1) % ADC_SAMPLES;
    
    return sum / ADC_SAMPLES;
}



uint8_t calculate_level(uint16_t adc_val) {
    static uint8_t last_level = 0;
    uint16_t step = 4095 / (NUM_LEVELS - 1);
    
    // ??????
    uint8_t new_level = adc_val / step;
    if(new_level >= NUM_LEVELS) new_level = NUM_LEVELS-1;

    // ????
    uint16_t lower_bound = new_level * step - HYSTERESIS;
    uint16_t upper_bound = new_level * step + HYSTERESIS;
    
    if(adc_val < lower_bound && new_level > 0) 
        new_level--;
    else if(adc_val > upper_bound && new_level < NUM_LEVELS-1)
        new_level++;

    // ????
    if(new_level >= NUM_LEVELS) new_level = NUM_LEVELS-1;
    
    // ??????????????
    if(abs(new_level - last_level) > 0) {
        last_level = new_level;
    }
    
    return last_level;
}

#define DEBOUNCE_COUNT 3 // ????

uint16_t raw_adc;
uint8_t get_stable_level(void) {
    static uint8_t stable_level = 0;
    static uint8_t counter = 0;
    static uint8_t last_raw_level = 0;
    
    
	
	if(Mode == 1) raw_adc=AD_Value[0];
	else raw_adc=AD_Value[1];
	
    uint16_t filtered = filter_adc(raw_adc);
    uint8_t new_level = calculate_level(filtered);
    
    if(new_level != last_raw_level) {
        counter = 0;
        last_raw_level = new_level;
    } else {
        if(counter < DEBOUNCE_COUNT) {
            counter++;
        } else {
            stable_level = new_level;
        }
    }
    
    return stable_level;
}

//int Pul[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,18,22,27,35,40,45,50,55,60,65,70,72,74,76,78,80};
int Pul[81];
void InitPul()
{
	for(int r=1;r<85;r++) 
	{
		if(r<10)Pul[r]=1;
		else if(r<15)Pul[r]=2;
		else if(r<20)Pul[r]=3;
		else if(r<25)Pul[r]=4;
		else if(r<30)Pul[r]=5;
		else Pul[r]=r-25;
	}
}
void eDelay(int k)
{
	while(k)
		k--;
}

int CURSPD=0;
int DelayMap(int t)
{
	if(t>60) return 1000000;
	else if(t>24) return 300000;
	else if(t>20) return 250000;
	else if(t>15) return 230000;
	else if(t>10) return 230000;
	else if(t>5) return 230000;
	else return 230000;
}
void ChangeSpeed(u8 OSpd,u8 NSpd)
{
	
	if(OSpd>NSpd)
	{
		last_reported_level=current;
		Motor_ON();
		for(int t=Pul[OSpd];t>=Pul[NSpd];t--)
		{            
			if(t<=Pul[NSpd])t=Pul[NSpd];
			Set_Speed(t);
		    OLED_ShowNum(2,14,t,3);
			int TurningNowStateFOOT=AD_Value[1];
			int TurningNowStateHAND=AD_Value[0];
			if(TurningNowStateFOOT<250){Set_Speed(0);Motor_OFF();break;}
			if(TurningNowStateHAND<250){Set_Speed(0);Motor_OFF();break;}
		}
		
	}
	else if(OSpd<NSpd)
	{
		last_reported_level=current;
		Motor_ON();
		for(int t=Pul[OSpd];t<=Pul[NSpd];t+=1)
		{
            if(t>=Pul[NSpd])t=Pul[NSpd];
			Set_Speed(t);
			OLED_ShowNum(2,14,t,3);
			eDelay(150000);
			//if(t%10==0) {for(int o=0;o<5;o++) {eDelay(1000000);}}
			int TurningNowStateFOOT=AD_Value[1];
			int TurningNowStateHAND=AD_Value[0];
			OLED_ShowNum(4,10,get_stable_level(),4);
			if(get_stable_level()!=NSpd){break;}
			if(TurningNowStateFOOT<250){Set_Speed(0);Motor_OFF();break;}
			if(TurningNowStateHAND<250){Set_Speed(0);Motor_OFF();break;}
		}
		
	}
	
}

int main(void)
{	
	SystemInit();
	InitPul();
	OLED_Init(); 
    AD_Init();
	MotorSetting_Init();
	Pul_Init();
	Set_Speed(0);
	
	ControlSwitch_EXTI_Init();
	//Motor_Control_Init();
    ///Pul_Timer_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	//串口1 9600波特率
	printf("usart init successfully\r\n");

	
	OLED_Clear();
	OLED_ShowString(4,6," CW");
	OLED_ShowString(4,1,"OFF");
	OLED_ShowString(2,1,"CurrentSpeed:");
	OLED_ShowString(1,1,"Target Speed:");
	OLED_ShowString(3,1,"ControlMode:");
	Motor_Zheng();
	Motor_ON();
	last_reported_level=get_stable_level();
	Mode = Get_Motor_Control();
	if(Mode==1)OLED_ShowString(3,13,"HAND");
	else OLED_ShowString(3,13,"FOOT");
    current = get_stable_level();
	Set_Speed(Pul[current]);
	
	
	
    while(1)
	{
		current = get_stable_level();
		OLED_ShowNum(1,14,Pul[current],3);
		ChangeSpeed(last_reported_level,current);
        eDelay(20000);
	}
}

void EXTI1_IRQHandler(void) {/*
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        //PB1触发中断 ----顺时针/逆时针  
		Delay_ms(20);
        uint8_t pin_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
        if (pin_state == Bit_SET) {
            OLED_ShowString(4,6," CW");
			//Motor_Zheng();
        } else {
            OLED_ShowString(4,6,"CCW");
			//Motor_Fan();
        }
        // 清除标志位
        EXTI_ClearITPendingBit(EXTI_Line1);
    }*/
}

void EXTI15_10_IRQHandler(void) {
	/*PB10触发中断 ----启动/停止  
    if (EXTI_GetITStatus(EXTI_Line10) != RESET) {
		Delay_ms(20);
        uint8_t pin_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
        if (pin_state == Bit_SET) {
            OLED_ShowString(4,1," ON");
			//Motor_ON();
        } else {
            OLED_ShowString(4,1,"OFF");
			//Motor_OFF();
        }
        EXTI_ClearITPendingBit(EXTI_Line10);
    }*/
	/*PB11触发中断 ----手动/脚动  */
    if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		Delay_ms(20);
        uint8_t pin_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
        if (pin_state == Bit_SET) {
            OLED_ShowString(3,13,"HAND");
			Mode = 1;
        } else {
            OLED_ShowString(3,13,"FOOT");
			Mode = 2;
        }
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
}




