#ifndef __KEY_H
#define __KEY_H

void Motor_Control_Init(void);
void ControlSwitch_EXTI_Init(void);
u16 Get_Motor_Dir(void);
u16 Get_Motor_Control(void);
u16 Get_Motor_Run(void);
#endif
