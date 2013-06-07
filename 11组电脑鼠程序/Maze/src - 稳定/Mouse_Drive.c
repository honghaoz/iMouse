/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           Mouse_Drive.c
** Last modified Date: 
** Last Version: 
** Description:         ������ײ�����
** 
**--------------------------------------------------------------------------------------------------------
** Created By: 
** Created date: 
** Version: 
** Descriptions: 
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/


/*********************************************************************************************************
  ����ͷ�ļ�
*********************************************************************************************************/
#include "Mouse_Drive.h"
#include "Maze.h"
/*********************************************************************************************************
  ����ȫ�ֱ���
*********************************************************************************************************/
MAZECOOR          GmcMouse                        = {0,0};              /*  ���������ǰλ������      */

uint8             GucMouseDir                     = UP;                 /*  ���������ǰ����          */

uint8             GucMapBlock[MAZETYPE][MAZETYPE] = {0};                /*  GucMapBlock[x][y]           */
                                                                        /*  x,������;y,������;          */
                                                                        /*  bit3~bit0�ֱ������������   */
                                                                        /*  0:�÷�����·��1:�÷�����·  */

static __MOTOR  __GmLeft                          = {0, 0, 0, 0, 0};    /*  ���岢��ʼ������״̬      */
static __MOTOR  __GmRight                         = {0, 0, 0, 0, 0};    /*  ���岢��ʼ���ҵ��״̬      */

static uint8    __GucMouseState                   = __STOP;             /*  ���������ǰ����״̬      */
static uint32   __GuiAccelTable[400]              = {0};                /*  ����Ӽ��ٸ��׶ζ�ʱ��ֵ    */
static int32    __GiMaxSpeed                      = SEARCHSPEED;        /*  �����������е�����ٶ�      */
static uint8    __GucDistance[5]                  = {0};                /*  ��¼������״̬              */

uint8 		speedcontrol 			= 0 ;			/*������Ʒ�ʽ*/
int8		motormethod			= 0;			/*���ת����ʽ*/
int8		turnflag	     		= 0 ;
uint8		turnbackspeed			= 42;//45   //������ ���ֵ��ʲô�ã� ת��ʱ������

/*********************************************************************************************************
** Function name:       __delay
** Descriptions:        ��ʱ����
** input parameters:    uiD :��ʱ������ֵԽ����ʱԽ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __delay (uint32  uiD)
{
    for (; uiD; uiD--);
}

/*********************************************************************************************************
** Function name:       __rightMotorContr
** Descriptions:        �Ҳ����������ʱ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __rightMotorContr (void)
{
	static int8 cStep = 0;                                              /*  ��������ǰλ��            */  
	if(motormethod)
	{
		switch(__GmRight.cDir)
		{
			case __MOTORGOAHEAD:
				cStep = (cStep + 2) % 8;
			break;

			case __MOTORGOBACK:
				cStep = (cStep + 6) % 7;

			default:
			break;
		}
		if(cStep % 2)
			cStep --;
	}
	else
	{
		switch (__GmRight.cDir)
		{
			case __MOTORGOAHEAD:                                                /*  ��ǰ����                    */
				cStep = (cStep + 1) % 8;
			break;

			case __MOTORGOBACK:                                                 /*  ��󲽽�                    */
					cStep = (cStep + 7) % 8;
			break;

			default:
			break;
		}
	}
	switch (cStep)
	{
		case 0:                                                             /*  A2B2                        */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
			__PHRA1 | __PHRA2 | __PHRB1 | __PHRB2);
		break;

		case 1:                                                             /*  B2                          */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
			__PHRA1 | __PHRA2);
		break;

		case 2:                                                             /*  A1B2                        */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
			__PHRA1 | __PHRA2 | __PHRB2);
		break;

		case 3:                                                             /*  A1                          */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
			__PHRB2);
		break;

		case 4:                                                             /*  A1B1                        */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
			__PHRA2 | __PHRB2);
		break;

		case 5:                                                             /*  B1                          */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
			__PHRA2);
		break;

		case 6:                                                             /*  A2B1                        */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
			__PHRA2 | __PHRB1 | __PHRB2);
		break;

		case 7:                                                             /*  A2                          */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
			__PHRB1 | __PHRB2);
		break;

		default:
		break;
	}
}


/*********************************************************************************************************
** Function name:       __leftMotorContr
** Descriptions:        �󲽽��������ʱ��
** input parameters:    __GmLeft.cDir :������з���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __leftMotorContr (void)
{
	static int8 cStep = 0;                                              /*  ��������ǰλ��            */
	if(motormethod)
	{
		switch(__GmLeft.cDir)
		{
			case __MOTORGOAHEAD:
				cStep = (cStep + 2) % 8;
			break;

			case __MOTORGOBACK:
				cStep = (cStep + 6) % 7;

			default:
			break;
		}
		if(cStep % 2)
			cStep --;
	}
	else
	{
		switch (__GmLeft.cDir)
		{
			case __MOTORGOAHEAD:                                                /*  ��ǰ����                    */
				cStep = (cStep + 1) % 8;
			break;

			case __MOTORGOBACK:                                                 /*  ��󲽽�                    */
				cStep = (cStep + 7) % 8;
			break;

			default:
			break;
		}
	}
	switch (cStep)
	{
		case 0:                                                             /*  A2B2                        */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
			__PHLA1 | __PHLA2 | __PHLB1 | __PHLB2);
		break;

		case 1:                                                             /*  B2                          */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
			__PHLB1 | __PHLB2);
		break;

		case 2:                                                             /*  A1B2                        */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
			__PHLA2 | __PHLB1 | __PHLB2);
		break;

		case 3:                                                             /*  A1                          */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
			__PHLA2);
		break;

		case 4:                                                             /*  A1B1                        */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
			__PHLA2 | __PHLB2);
		break;

		case 5:                                                             /*  B1                          */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
			__PHLB2);
		break;

		case 6:                                                             /*  A2B1                        */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
			__PHLA1 | __PHLA2 | __PHLB2);
		break;

		case 7:                                                             /*  A2                          */
			GPIOPinWrite(GPIO_PORTD_BASE,
			__PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
			__PHLA1 | __PHLA2);
		break;

		default:
		break;
	}
}


/*********************************************************************************************************
** Function name:       __speedContrR
** Descriptions:        �ҵ���ٶȵ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __speedContrR (void)
{ 
	int32 iDPusle;
	switch (speedcontrol)
	{
		case 0:
			iDPusle = __GmRight.uiPulse - __GmRight.uiPulseCtr;           /*  ͳ�Ƶ����ʣ��Ĳ���        */
			if (iDPusle <= __GmRight.iSpeed)
				__GmRight.iSpeed--;
			else if(__GmRight.iSpeed < __GiMaxSpeed)                    /*  �Ǽ������䣬����ٵ����ֵ  */
					__GmRight.iSpeed++;
			if (__GmRight.iSpeed < 0)                                   /*  �����ٶ�����                */
				__GmRight.iSpeed = 0;
		break;

		case 1:
			if(__GmRight.iSpeed < __GiMaxSpeed)
				__GmRight.iSpeed ++;
			if(__GmRight.iSpeed > __GiMaxSpeed)
				__GmRight.iSpeed --;
		break;

		case 2:
			iDPusle = __GmRight.uiPulse - __GmRight.uiPulseCtr;          /*  ͳ�Ƶ����ʣ��Ĳ���        */
			if(__GmRight.iSpeed >= (iDPusle + 2 * turnbackspeed+10))
				__GmRight.iSpeed --;
			else  if (__GmRight.iSpeed < __GiMaxSpeed)
					__GmRight.iSpeed ++;
		break;

		case 3:
			if(__GmRight.iSpeed < 80)
				__GmRight.iSpeed ++;
			if(__GmRight.iSpeed > 80)
				__GmRight.iSpeed --;
		break;

		default:
		break;
	}
	TimerLoadSet(TIMER0_BASE,TIMER_A,__GuiAccelTable[__GmRight.iSpeed]);/*  ���ö�ʱʱ��                */
}



/*********************************************************************************************************
** Function name:       __speedContrL
** Descriptions:        �����ٶȵ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __speedContrL (void)
{
	int32 iDPusle;
	switch (speedcontrol)
	{
		case 0:
			iDPusle = __GmLeft.uiPulse - __GmLeft.uiPulseCtr;                   /*  ͳ�Ƶ����ʣ��Ĳ���        */
			if (iDPusle <= __GmLeft.iSpeed)
				__GmLeft.iSpeed--;
			else  if(__GmLeft.iSpeed < __GiMaxSpeed)                      /*  �Ǽ������䣬����ٵ����ֵ  */
					__GmLeft.iSpeed++;
			if (__GmLeft.iSpeed < 0)                                          /*  �����ٶ�����                */
				__GmLeft.iSpeed = 0;
		break;

		case 1:
			if(__GmLeft.iSpeed < __GiMaxSpeed)
				__GmLeft.iSpeed ++;
			if(__GmLeft.iSpeed > __GiMaxSpeed)
				__GmLeft.iSpeed --;
		break;

		case 2:
			iDPusle = __GmLeft.uiPulse - __GmLeft.uiPulseCtr;                   /*  ͳ�Ƶ����ʣ��Ĳ���        */
			if(__GmLeft.iSpeed >= (iDPusle + 2 * turnbackspeed+10))
				__GmLeft.iSpeed --;
			else if (__GmLeft.iSpeed < __GiMaxSpeed)
					__GmLeft.iSpeed ++;
		break;

		case 3:
			if(__GmLeft.iSpeed < 80)
				__GmLeft.iSpeed ++;
			if(__GmLeft.iSpeed > 80)
				__GmLeft.iSpeed --;
		break;

		default:
		break;
	}
	TimerLoadSet(TIMER1_BASE,TIMER_A,__GuiAccelTable[__GmLeft.iSpeed]); /*  ���ö�ʱʱ��                */
}


/*********************************************************************************************************
** Function name:       Timer0A_ISR
** Descriptions:        Timer0�жϷ�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void Timer0A_ISR(void)
{
	static int8 n = 0,m = 0;
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                     /*  �����ʱ��0�жϡ�           */
	switch (__GmRight.cState)
	{  
		case __MOTORSTOP:                                                   /*  ֹͣ��ͬʱ��������ֵ  */
			__GmRight.uiPulse    = 0;
			__GmRight.uiPulseCtr = 0;
		break;

		case __WAITONESTEP:                                                 /*  ��ͣһ��                    */
			__GmRight.cState     = __MOTORRUN;
		break;

		case __MOTORRUN:                                                    /*  �������                    */
			if (__GucMouseState == __GOAHEAD)			/*  ���ݴ�����״̬΢�����λ��  */
			{                            
				if (__GucDistance[__FRONTL] && (__GucDistance[__FRONTR] == 0))
				{
					if (n == 1)
						__GmRight.cState = __WAITONESTEP;
					n++;
					n %= 2;
				}
				else
					n = 0;

				if ((__GucDistance[__RIGHT] == 1) && (__GucDistance[__LEFT] == 0))
				{
					if(m == 3)
						__GmRight.cState = __WAITONESTEP;
					m++;
					m %= 6;
				}
				else
					m  = 0;
			}
			__rightMotorContr();                                            /*  �����������                */
		break;

		default:
		break;
	}
	/*
	*  �Ƿ���������ж�
	*/
	if (__GmRight.cState != __MOTORSTOP)
	{
		__GmRight.uiPulseCtr++;                                         /*  �����������                */
		__speedContrR();                                                /*  �ٶȵ���                    */
		if (__GmRight.uiPulseCtr >= __GmRight.uiPulse)
		{
			__GmRight.cState      = __MOTORSTOP;
			__GmRight.uiPulseCtr  = 0;
			__GmRight.uiPulse     = 0;
                     //�˴�ɾȥ���ٶ�ֵ���Ϊ0   __GmRight.iSpeed      = 0;

		}
	}
}


/*********************************************************************************************************
** Function name:       Timer1A_ISR
** Descriptions:        Timer1�жϷ�����
** input parameters:    __GmLeft.cState :�������������ʱ��״̬
**                      __GmLeft.cDir   :��������˶��ķ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void Timer1A_ISR(void)
{
    static int8 n = 0, m = 0;
    
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);                     /*  �����ʱ��1�жϡ�           */
    switch (__GmLeft.cState)
	{
		case __MOTORSTOP:                                                   /*  ֹͣ��ͬʱ��������ֵ  */
			__GmLeft.uiPulse    = 0;
                        //�˴�ɾȥ���ٶ�ֵ __GmLeft.iSpeed     = 0;
			__GmLeft.uiPulseCtr = 0;
        break;
        
		case __WAITONESTEP:                                                 /*  ��ͣһ��                    */
		  __GmLeft.cState     = __MOTORRUN;
        break;

		case __MOTORRUN:                                                    /*  �������                    */
			if (__GucMouseState == __GOAHEAD)                            /*  ���ݴ�����״̬΢�����λ��  */
			{ 
				if (__GucDistance[__FRONTR] &&(__GucDistance[__FRONTL]==0))
				{
					if (n == 1)
				      __GmLeft.cState = __WAITONESTEP;
					n++;
					 n %= 2;
				}
				else 
					n = 0;

				if ((__GucDistance[__LEFT] == 1) && (__GucDistance[__RIGHT] == 0))
				{
					if(m == 3)
						__GmLeft.cState = __WAITONESTEP;
					m++;
					m %= 6;
				}
				else
					m  = 0;
			}
		 __leftMotorContr();                                             /*  �����������                */
		break;

		default:
        break;
    }
    /*
     *  �Ƿ���������ж�
     */
    if (__GmLeft.cState != __MOTORSTOP)
	{
        __GmLeft.uiPulseCtr++;                                          /*  �����������                */
        __speedContrL();                                                /*  �ٶȵ���                    */
        if (__GmLeft.uiPulseCtr >= __GmLeft.uiPulse)
		{
            __GmLeft.cState      = __MOTORSTOP;
            __GmLeft.uiPulseCtr  = 0;
            __GmLeft.uiPulse     = 0;
            //__GmLeft.iSpeed     = 0;
        }
    }
}

void acc_change(int kai){
      uint16 n = 0;
//     kai=1581139;
    __GuiAccelTable[0] = kai;//2236068
    __GuiAccelTable[1] = 0.4142*kai;
    for(n = 2; n < 400; n++) {
        __GuiAccelTable[n] = __GuiAccelTable[n - 1] - (2 * __GuiAccelTable[n - 1] / (4 * n + 1));
    }
}
/****************************************************************
** Function name:   backTurnright 
** Descriptions:   �ڷ��ع�����̽���ұߵ���
		�⵽�������ݲ�ͬ���ȷ��Ӧǰ���Ĳ���
** output parameters:   ��
** Returned value:      ��
****************************************************************/
void backTurnright ()
{
	while (__GmLeft.cState  != __MOTORSTOP);
	while (__GmRight.cState != __MOTORSTOP);//__MOTORSTOP �Ѿ�û���ٶ�ֵΪ0
	/*
	 *  ��ʼ��ת
	 */
//       zlg7289Reset();
//       zlg7289Download(1, 0, 0, 0x0B);
//       zlg7289Download(0, 1, 0, 0x0E);
        //acc_change(2001139);
//      acc_change(2236068);

	__GucMouseState   = __TURNRIGHT;
	__GmLeft.uiPulse = 88;//86
	__GmLeft.iSpeed = 80;
	__GmLeft.cState   = __MOTORRUN;
        
	while ((__GmLeft.uiPulse - __GmLeft.uiPulseCtr) > 3);//ʣ�ಽ������3
	__GmRight.uiPulse  = (turnbackspeed - 40)/3 +7; //??? ������ɻ󣬲���
	__GmRight.cState   = __MOTORRUN;
	while (__GmRight.cState != __MOTORSTOP);
	__GmLeft.iSpeed = 40;
//        acc_change(1901139);
//      acc_change(2236068);
        

}

/****************************************************************
** Function name: backTurnleft
** Descriptions:  �ڷ��ع�����̽����ߵ���
		�⵽�������ݲ�ͬ���ȷ��Ӧǰ���Ĳ���
** output parameters:   ��
** Returned value:      ��
****************************************************************/
void backTurnleft ()
{
	while (__GmLeft.cState  != __MOTORSTOP);
	while (__GmRight.cState != __MOTORSTOP);
	/*
	 *  ��ʼ��ת
	 */
//        zlg7289Reset();
//        zlg7289Download(1, 0, 0, 0x0B);
//        zlg7289Download(0, 1, 0, 0x0D);
//        acc_change(2001139);
//      acc_change(2236068);
        
        
	__GucMouseState   = __TURNLEFT;
	__GmRight.uiPulse = 88;//86
	__GmRight.iSpeed = 80;
	__GmRight.cState  = __MOTORRUN;
	while ((__GmRight.uiPulse - __GmRight.uiPulseCtr) > 3);
	__GmLeft.uiPulse  = (turnbackspeed - 40)/3 +7;
	__GmLeft.cState   = __MOTORRUN;
	while (__GmLeft.cState  != __MOTORSTOP);
	__GmRight.iSpeed = 40;
//        acc_change(1901139);
//      acc_change(2236068);

}

/****************************************************************
** Function name:		  backturnback
** Descriptions:    		�ڷ��ع�����̽����ߵ���
					�⵽�������ݲ�ͬ���ȷ��Ӧǰ���Ĳ���
** output parameters:   ��
** Returned value:      ��
****************************************************************/

void backTurnback(void)
{
	/*
	 *  �ȴ�ֹͣ
	 */
	while (__GmLeft.cState  != __MOTORSTOP);
	while (__GmRight.cState != __MOTORSTOP);
	/*
	 *  ��ʼ��ת
	 */
        acc_change(1881139);//1381139
//        acc_change(2236068);
//        zlg7289Reset();
//        zlg7289Download(1, 0, 0, 0x0B);
//        zlg7289Download(1, 1, 0, 0x0B);
	 motormethod = 1;
	__GiMaxSpeed = 15;
	__GucMouseState   = __TURNBACK;
	__GmRight.cDir    = __MOTORGOBACK;
	__GmRight.uiPulse = 45;//44
	__GmLeft.uiPulse  = 45;//44
	 speedcontrol = 0;
	__GmLeft.cState   = __MOTORRUN;
	__GmRight.cState  = __MOTORRUN;
	while (__GmLeft.cState  != __MOTORSTOP);
	while (__GmRight.cState != __MOTORSTOP);
	motormethod = 0;
	__GiMaxSpeed = MAXSPEED;
	__GmRight.cDir = __MOTORGOAHEAD;
        
        acc_change(2236068);
}



/*********************************************************************************************************
** Function name:       objectGoTo
** Descriptions:        ʹ��������췽ʽ�˶���ָ������
** input parameters:    cXdst: Ŀ�ĵصĺ�����
**                      cYdst: Ŀ�ĵص�������
** output parameters:   ��
** Returned value:      ��
** GuiStep[][]:		��һ����������ǰӦ���ߵĸ���
			�ڶ���������0����ת�䣻1������ת��2������ת��3�����ת
*********************************************************************************************************/
void objectGoTo (int8  cXdst, int8  cYdst, uint8 mode)
{
	uint8   i = 1, cL = 0, cR = 0, offset = 0;
        
	static uint8 gapflag = 0; //??? �������û�п���
        
	mapStepEdit(cXdst,cYdst);	 /*  �����ȸ�ͼ��������������*/  
	if( GucDirFlag == 1)		   //GucDirFlag  �������      /*�Ƿ����³��*/
	{
		GuiStep[0][1]=0;
		GucDirFlag = 0;
	}
    switch (mode)		/*���ݲ�ͬ�ķ���ģʽ����ȡ��ͬ�ķ����ٶ�*/
    {
		case __BACK:
			turnbackspeed = 45;//45
		break;

		case __START:
                  if(gapflag == 1){
				turnbackspeed = 40;//40
                  }
                        
                  else{
				turnbackspeed = 45;//45     
                  }
		break;

		case __END:
			turnbackspeed = 50;//50
			gapflag = 1;
		break;

		default:
		break;
		
    }
    switch (GuiStep[0][1])		/*ת��ǰ��ת��*/
    {
                case 0:
			if(!mode)
			{
				__GmRight.uiPulse = __GmRight.uiPulseCtr + 50;
				__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 50 ;
				speedcontrol = 1;
				__GmLeft.cState = __MOTORRUN;
				__GmRight.cState = __MOTORRUN;
				while((__GmLeft.uiPulse - __GmLeft.uiPulseCtr ) > 5);
			}
			else
				GucDirFlag = 0;
		break;
		
		case 1:
			backTurnright();
		break;

		case 2:
			backTurnleft();
		break;

		case 3:
 			if((!(__GucDistance[__LEFT] && __GucDistance[__RIGHT])))	/*����������һ�ߴ���ȱ��,��ǰ��40��*/
 			{
				__GmRight.uiPulse = __GmRight.uiPulseCtr + 40;
				__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 40 ;
				speedcontrol = 0;
				__GmLeft.cState = __MOTORRUN;
				__GmRight.cState = __MOTORRUN;
				while (__GmLeft.cState  != __MOTORSTOP);
				while (__GmRight.cState != __MOTORSTOP);
				__GmLeft.iSpeed = 0;
				__GmRight.iSpeed = 0;
 			}
 			backTurnback();
			speedcontrol = 1;
		break;

		default:
		break;
	}
	while(GuiStep[i][0] != 0)	/*���ݷ������飬��ʼִ�з���*/
	{
		__GucMouseState   = __GOAHEAD;
		offset = 0;
		if(GuiStep[i][0] > 1)	/*������ǰ��GuiStep[i][0]-1�����ӣ�Ŀ��λ��Ϊ���һ�����ӵ�����*/
		{	
			__GiMaxSpeed  =  3 *  turnbackspeed;
			__GmRight.uiPulse =  (GuiStep[i][0] - 1) * ONEBLOCK - 20; 
			__GmLeft.uiPulse  =  (GuiStep[i][0] - 1) * ONEBLOCK - 20;
			if((i == 1) && ((GuiStep[0][1] == 3) || (GuiStep[0][1] == 0)))
			{
				__GmLeft.uiPulse += 20;
				__GmRight.uiPulse += 20;
			}
			speedcontrol = 2;
			__GmRight.cState  = __MOTORRUN;
			__GmLeft.cState   = __MOTORRUN;
			while((__GmLeft.uiPulse - __GmLeft.uiPulseCtr ) > 2);
		}
		__GiMaxSpeed      =  SEARCHSPEED;
		__GmRight.uiPulseCtr = 0;
		__GmLeft.uiPulseCtr = 0;
		switch (GuiStep[i][1])	/*׼��ת��*/
		{
			case 0:		/*����Ҫת�䣬˵���ѵ���Ŀ���*/
				switch (mode)
				{
					case __START:
	 					__GmRight.uiPulse =  ONEBLOCK;
						__GmLeft.uiPulse  =  ONEBLOCK;
						__GiMaxSpeed      =  turnbackspeed;
						speedcontrol = 0;
						__GmRight.cState  = __MOTORRUN;
						__GmLeft.cState   = __MOTORRUN;
						while(__GmLeft.cState != __MOTORSTOP)
						{
							if (__GucDistance[__FRONT] == 0x03)   /*����ǰ�������ж��Ƿ񵽴��Թ�������*/
							{ 
								__GmRight.uiPulse = __GmRight.uiPulseCtr + 60;
								__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 60;
								while (__GucDistance[ __FRONT] == 0x03)
								{
									if((__GmLeft.uiPulse - __GmLeft.uiPulseCtr) <5)
										return;
								}
							}
						}
					break;

					case __END:
					case __BACK:
						speedcontrol = 3;
						if((GuiStep[i][0] > 1) || (GuiStep[i-1][1] ==0))/*��ֱ�ߵ���Ŀ���,��ʱ�ٶȽϿ죬��������ȱ���ж��Ƿ񵽴�Ŀ���*/
						{
							__GmRight.uiPulse =  2 * ONEBLOCK;
							__GmLeft.uiPulse  = 2 * ONEBLOCK;
							__GmRight.cState  = __MOTORRUN;
							__GmLeft.cState   = __MOTORRUN;
							while(__GmLeft.cState != __MOTORSTOP)
							{
								if (cL) 	/* �Ƿ����������*/
								{                                                
									if (!__GucDistance[ __LEFT])   	/*�����֧·������������*/
									{         
										speedcontrol = 4;
										__GmLeft.iSpeed = 40;
										__GmRight.iSpeed = 40;
										__GmRight.uiPulse = __GmRight.uiPulseCtr + 42;//45
										__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 42;//45
										while (!__GucDistance[ __LEFT] )	/*������⣬��ֹ����*/
										{
											if ((__GmLeft.uiPulseCtr + 5) > __GmLeft.uiPulse)/*ȷ������Ŀ��㣬����*/
												return;
										}
										__GmRight.uiPulse =  ONEBLOCK;	/*���У��ָ�*/
										__GmLeft.uiPulse  = ONEBLOCK;
									}
								} 
								else if ( __GucDistance[ __LEFT] ) 	/*�����ǽʱ��ʼ���������*/
									cL = 1;
								if (cR)            /*�Ƿ��������ұ�*/
								{
									if (!__GucDistance[ __RIGHT] )/*�ұ���֧·������������*/
									{
										speedcontrol = 4;
										__GmLeft.iSpeed = 40;
										__GmRight.iSpeed = 40;
										__GmRight.uiPulse = __GmRight.uiPulseCtr + 41;//45
										__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 41;//45
										while (!__GucDistance[ __RIGHT])/*������⣬��ֹ����*/
										{
											if ((__GmLeft.uiPulse - __GmLeft.uiPulseCtr ) < 10)/*ȷ������Ŀ��㣬����*/
												return;
										}
										__GmRight.uiPulse = ONEBLOCK;	/*���У��ָ�*/
										__GmLeft.uiPulse  = ONEBLOCK;
									}
								} 
								else if ( __GucDistance[__RIGHT] )	/*�ұ���ǽʱ��ʼ�������ұ�*/               
									cR = 1;
							}
						}
						else if((GuiStep[i-1][1] == 1) || (GuiStep[i-1][1] == 2))/*ͨ����ת����ת����Ŀ��㣬��ʱ�ٶȽ������ҿ���ת��ͷ*/
						{
							__GmRight.uiPulse =  ONEBLOCK;
							__GmLeft.uiPulse  =  ONEBLOCK ;
							speedcontrol = 4;
							__GmLeft.iSpeed = 40;		/*�����ٶ�Ϊ40�����´�ת���ٶ�ƥ��*/
							__GmRight.iSpeed = 40;
							__GmRight.cState  = __MOTORRUN;
							__GmLeft.cState   = __MOTORRUN;
							while (__GmLeft.cState != __MOTORSTOP)
							{ 
								if(!__GucDistance[__LEFT])	/*�����֧·������������*/
								{
									__GmRight.uiPulse = __GmRight.uiPulseCtr + 48;
									__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 48;
									while(!__GucDistance[__LEFT])		/*������⣬��ֹ����*/
									{
										if((__GmLeft.uiPulse - __GmLeft.uiPulseCtr ) < 10)/*ȷ������Ŀ��㣬����*/
											return;
									} 
									__GmRight.uiPulse =  ONEBLOCK;	/*���У��ָ�*/
									__GmLeft.uiPulse  =  ONEBLOCK ;
								}
								if(!__GucDistance[__RIGHT])		/*�ұ���֧·������������*/
								{ 
									__GmRight.uiPulse = __GmRight.uiPulseCtr + 48;//48
									__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 48;//48
									while(!__GucDistance[__RIGHT] )		/*������⣬��ֹ����*/
									{
								 		if((__GmRight.uiPulse - __GmRight.uiPulseCtr ) < 10)	/*ȷ������Ŀ��㣬����*/
											return;
									} 
									__GmRight.uiPulse =  ONEBLOCK;		/*���У��ָ�*/
									__GmLeft.uiPulse  =  ONEBLOCK ;
								}
							}
						}
						else if(GuiStep[i-1][1] == 3)			/*���ת��󵽴�Ŀ���*/
						{
							__GmRight.uiPulse =  ONEBLOCK;
							__GmLeft.uiPulse  =  ONEBLOCK;
							speedcontrol = 1;
							__GmRight.cState  = __MOTORRUN;
							__GmLeft.cState   = __MOTORRUN;
							while (__GmLeft.cState != __MOTORSTOP)
							{ 
								if(cL)					/*�Ƿ����������*/
								{
									if(!__GucDistance[__LEFT])
									{
										__GmRight.uiPulse = __GmRight.uiPulseCtr + 45;
										 __GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 45;
										while(!__GucDistance[__LEFT])
										{
											if((__GmLeft.iSpeed > 40) && (__GmRight.iSpeed > 40))	/*�����ٶ�Ϊ������40���������´�ת���ٶ�ƥ��*/
											{
												speedcontrol = 4;
												__GmLeft.iSpeed = 40;
												__GmRight.iSpeed = 40;
											}
											if((__GmLeft.uiPulse - __GmLeft.uiPulseCtr ) < 10)	/*ȷ������Ŀ��㣬����*/
												return;
										} 
									}
								}
								else if ( __GucDistance[__LEFT] )		/*�����ǽʱ��ʼ���������*/               
									cL = 1;
								if(cR)					/*�Ƿ��������ұ�*/
								{
									if(!__GucDistance[__RIGHT])
									{ 
										__GmRight.uiPulse = __GmRight.uiPulseCtr + 44;//45
										__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 44;//45
										while(!__GucDistance[__RIGHT] )
										{
											if((__GmLeft.iSpeed > 40) && (__GmRight.iSpeed > 40))	/*�����ٶ�Ϊ������40���������´�ת���ٶ�ƥ��*/
											{
												speedcontrol = 4;
												__GmLeft.iSpeed = 40;
												__GmRight.iSpeed = 40;
											}
										 	if((__GmRight.uiPulse - __GmRight.uiPulseCtr ) < 10)	/*ȷ������Ŀ��㣬����*/
												return;
										} 
									}
								}
								else if ( __GucDistance[__RIGHT] )			/*�ұ���ǽʱ��ʼ�������ұ�*/               
									cR = 1;
							}
						}
					break;

					default:
					break;
				}
			break;
			
			case 1:								/*����ת*/
				if((GuiStep[i][0] > 1) || (GuiStep[i-1][1] == 0) || (GuiStep[i-1][1] == 3))  /*��ʱ����������λ�ò��ᳬ���ɼ�ⷶΧ������ͨ������У��*/
				{
					__GmRight.uiPulse = 2 *  ONEBLOCK;
					__GmLeft.uiPulse  =  2 * ONEBLOCK;
					__GmRight.cState  = __MOTORRUN;
					__GmLeft.cState   = __MOTORRUN;
					speedcontrol = 3;			/*ʹ�ٶ���80����*/
					if(!__GucDistance[__RIGHT])		/*�ұ���ȱ�ڣ�����Ŀ��ȱ�ڣ��ȴ���һ��ȱ�ڳ���*/
					{
						while(__GmRight.cState != __MOTORSTOP)
						{
							if(__GucDistance[__RIGHT])
								break;
						}
					}
					while(__GmRight.cState != __MOTORSTOP)
					{
						if(!__GucDistance[__RIGHT])	/*��⵽�ұ���ȱ�ڣ�׼��ת��*/
						{ 
							if((GuiStep[i-1][1] == 3) && (GuiStep[i][0] == 1))
							{
								__GmRight.uiPulse = __GmRight.uiPulseCtr + 45;
								__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 45;
							}
							else					/*���ֱ�߼��ٵ���õ㣬���ڹ��ԣ�ת��Ӧ��ǰ�ߵ���������*/
							{
								__GmRight.uiPulse = __GmRight.uiPulseCtr + 65 - turnbackspeed/2;
								__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 65 - turnbackspeed/2;
							}
							while(!__GucDistance[__RIGHT])		/*������⣬��ֹ����*/
							{
								if((__GmLeft.iSpeed > 40) && (__GmRight.iSpeed > 40))	/*��֤ת��ǰ�ٶȲ�����40*/
								{
									speedcontrol = 4;
									__GmLeft.iSpeed =  40;
									__GmRight.iSpeed =  40;
								}
					 			if((__GmRight.uiPulseCtr + 2) > __GmRight.uiPulse)
								{
									backTurnright();
									break;
								}
							} 
						}
					}
				}
				else if((GuiStep[i-1][1] == 1) || (GuiStep[i-1][1] == 2))/*ת��ǰҲ��ת�䣬˵��������ת�䣬��ʱ�п��ܳ���Ҳ��ⷶΧ���޷�У��*/
				{
					speedcontrol = 4;
					__GmLeft.iSpeed =  40;
					__GmRight.iSpeed =  40;
					__GmRight.uiPulse = ONEBLOCK;
					__GmLeft.uiPulse  =  ONEBLOCK;
					__GmRight.cState  = __MOTORRUN;
					__GmLeft.cState   = __MOTORRUN;
					while(__GmRight.cState != __MOTORSTOP)
					{
						if(!__GucDistance[__RIGHT])
						{ 
							__GmRight.uiPulse = __GmRight.uiPulseCtr + 62 + offset - turnbackspeed/2;
							__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 62 + offset - turnbackspeed/2;
							while(!__GucDistance[__RIGHT])
							{
						 		if((__GmRight.uiPulseCtr + 2) > __GmRight.uiPulse)
								{
									if(GuiStep[i-1][1] == 2)//��ת����ת
										__GmLeft.uiPulse += 5;
									backTurnright();
									break;
								}
							} 
							__GmRight.uiPulse = ONEBLOCK;
							__GmLeft.uiPulse  =  ONEBLOCK;
						}
						offset = 5;			/*������δ�����ɼ�ⷶΧ���ʵ�����Ӧ��ǰ�ߵĲ���*/
					}
				}
			break;
			
			case 2:						/*����ת*/
				if((GuiStep[i][0] > 1) || (GuiStep[i-1][1] == 0) || (GuiStep[i-1][1] == 3))  /*��ʱ����������λ�ò��ᳬ���ɼ�ⷶΧ������ͨ������У��*/
				{
					__GmRight.uiPulse = 2 *  ONEBLOCK;
					__GmLeft.uiPulse  =  2 * ONEBLOCK;
					__GmRight.cState  = __MOTORRUN;
					__GmLeft.cState   = __MOTORRUN;
					speedcontrol = 3;				/*ʹ�ٶ���80����*/
					if(!__GucDistance[__LEFT])			/*�����ȱ�ڣ�����Ŀ��ȱ�ڣ��ȴ���һ��ȱ�ڳ���*/
					{	
						while(__GmLeft.cState != __MOTORSTOP)
						{
							if(__GucDistance[__LEFT])
								break;
						}
					}
					while(__GmLeft.cState != __MOTORSTOP)
					{
						if(!__GucDistance[__LEFT])		/*��⵽�����ȱ�ڣ�׼��ת��*/
						{ 
							if((GuiStep[i-1][1] == 3) && (GuiStep[i][0] == 1))
							{
								__GmRight.uiPulse = __GmRight.uiPulseCtr + 43;//45
								__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 43;//45
							}
							else						/*���ֱ�߼��ٵ���õ㣬���ڹ��ԣ�ת��Ӧ��ǰ�ߵ���������*/
							{
								__GmRight.uiPulse = __GmRight.uiPulseCtr + 65 - turnbackspeed/2;
								__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 65 - turnbackspeed/2;
							}
							while(!__GucDistance[__LEFT])		/*������⣬��ֹ����*/
							{
								if((__GmLeft.iSpeed >  40) && (__GmRight.iSpeed > 40))	/*��֤ת��ǰ�ٶȲ�����40*/
								{
									speedcontrol = 4;
									__GmLeft.iSpeed =  40;
									__GmRight.iSpeed =  40;
								}
					 			if((__GmLeft.uiPulseCtr + 2) > __GmLeft.uiPulse)
								{
									backTurnleft();
									break;
								}
							} 
						}
					}
		             	}
				else if((GuiStep[i-1][1] == 1) || (GuiStep[i-1][1] == 2))	/*ת��ǰҲ��ת�䣬˵��������ת�䣬��ʱ�п��ܳ���Ҳ��ⷶΧ���޷�У��*/
				{
					speedcontrol = 4;
					__GmLeft.iSpeed =  40;
					__GmRight.iSpeed =  40;
					__GmRight.uiPulse = ONEBLOCK;
					__GmLeft.uiPulse  =  ONEBLOCK;
					__GmRight.cState  = __MOTORRUN;
					__GmLeft.cState   = __MOTORRUN;
					while(__GmLeft.cState != __MOTORSTOP)
					{
						if(!__GucDistance[__LEFT])
						{ 
							__GmRight.uiPulse = __GmRight.uiPulseCtr + 62 + offset - turnbackspeed/2;
							__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 62 + offset - turnbackspeed/2;
							while(!__GucDistance[__LEFT])
							{
						 		if((__GmLeft.uiPulseCtr + 2) > __GmLeft.uiPulse)
								{
									if(GuiStep[i-1][1] == 1)//��ת����ת
										__GmRight.uiPulse += 5;
									backTurnleft();
									break;
								}
							} 
							__GmRight.uiPulse = ONEBLOCK;
							__GmLeft.uiPulse  =  ONEBLOCK;
						}
						offset = 5;				/*������δ�����ɼ�ⷶΧ���ʵ�����Ӧ��ǰ�ߵĲ���*/
					}
				}
			break;

			default:
			break;
		}
		i++;
	}	
}



/*********************************************************************************************************
** Function name:       mazeSearch
** Descriptions:        ǰ��N��
** input parameters:    iNblock: ǰ���ĸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mazeSearch(void)
{
   int8  cL = 0, cR = 0, sL = 0, sR = 0;
   __GucMouseState   = __GOAHEAD;
  if((turnflag == 1) || (turnflag == 2))	/*��ת���䣬����ֱ���ֿ�����*/
  {
		speedcontrol = 4;
		__GmLeft.iSpeed = 40;
		__GmRight.iSpeed = 40;
		__GmRight.uiPulse = 75;
		__GmLeft.uiPulse = 75;
		__GmRight.cState  = __MOTORRUN;
		__GmLeft.cState  = __MOTORRUN;
		while((__GmLeft.uiPulse - __GmLeft.uiPulseCtr) > 5)
		{ 
			if(!__GucDistance[__LEFT] )   		/*�����֧· */
			{
				__GmRight.uiPulse = __GmRight.uiPulseCtr + 40;//ǰһ��ת�䣬������ת
				__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 40;
				while(!__GucDistance[__LEFT])	/*������⣬��ֹ����*/
				{
					if((__GmRight.uiPulseCtr + 10) > __GmRight.uiPulse)	/*ȷ����֧·������*/
					{
						__mouseCoorUpdate();
						if(turnflag == 1)//֮ǰ��ת	/*��ת�亯�����õ����ж��Ƿ�Ϊ��ͬ���������ת��*/
							turnflag = 3;//ת�����ת
						else//֮ǰ��ת
							turnflag = 4;//turnflag==2(��ת) 
						return;
					}
				} 
				__GmRight.uiPulse = 75;			/*���У��ָ�*/
				__GmLeft.uiPulse  = 75;
			}
			if(!__GucDistance[__RIGHT])			/*�ұ���֧· */
			{ 
				__GmRight.uiPulse = __GmRight.uiPulseCtr + 38;//45 ǰһ��ת�䣬������ת
				__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 38;//45
				while(!__GucDistance[__RIGHT])		/*������⣬��ֹ����*/
				{
					if((__GmLeft.uiPulseCtr + 10) > __GmLeft.uiPulse)	/*ȷ����֧·������*/
					{
						__mouseCoorUpdate();
						if(turnflag == 1)		/*��ת�亯�����õ����ж��Ƿ�Ϊ��ͬ���������ת��*/
							turnflag = 3;
						else
							turnflag = 4;
						return;
					}
				} 
				__GmRight.uiPulse = 75;			 /*���У��ָ�*/
				__GmLeft.uiPulse  = 75;
			}
			if(__GucDistance[__FRONT] == 0x03)		/*ǰ����ǽ������������*/
			{
				speedcontrol = 0;

				__GmRight.uiPulse = __GmRight.uiPulseCtr + 60;//60;
				__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 60;//60;
                                
				while(__GucDistance[__FRONT] == 0x03)		/*������⣬��ֹ����*/
				{
					if((__GmLeft.uiPulseCtr + 10) > __GmLeft.uiPulse)	/*ȷ����·���ߣ�����*/
					{
						__mouseCoorUpdate();
						turnflag = 0;
						return;
					}
				} 
				speedcontrol = 4;					/*���У��ָ�֮ǰ״̬*/
				__GmLeft.iSpeed = 40;
				__GmRight.iSpeed = 40;
    			__GiMaxSpeed = SEARCHSPEED;
				__GmRight.uiPulse = 75;
				__GmLeft.uiPulse  = 75;
			}
		}
		__mouseCoorUpdate();						/*��������*/
	} 
  	else if (__GmLeft.cState)
  	{
  		speedcontrol = 1;
		__GmRight.uiPulse = __GmRight.uiPulseCtr + 30;
		__GmLeft.uiPulse = __GmLeft.uiPulseCtr + 30;
		while((__GmLeft.uiPulse - __GmLeft.uiPulseCtr) > 2);
	}
	turnflag = 0;
	__GmLeft.uiPulseCtr = 0;
	__GmRight.uiPulseCtr = 0;
	__GmRight.uiPulse =   MAZETYPE * ONEBLOCK;
	__GmLeft.uiPulse  =   MAZETYPE * ONEBLOCK;
	speedcontrol = 1;
        __GiMaxSpeed = SEARCHSPEED;
	__GmRight.cState  = __MOTORRUN;
	__GmLeft.cState   = __MOTORRUN; 
	while (__GmLeft.cState != __MOTORSTOP)
	{
		if (__GmLeft.uiPulseCtr >= ONEBLOCK)    	/*�ж��Ƿ�����һ��*/
		{                    
			__GmLeft.uiPulse    -= ONEBLOCK;
			__GmLeft.uiPulseCtr -= ONEBLOCK;
			__mouseCoorUpdate();			/*��������*/
		}
		if (__GmRight.uiPulseCtr >= ONEBLOCK)		/*�ж��Ƿ�����һ��*/
		{                      
			__GmRight.uiPulse    -= ONEBLOCK;
			__GmRight.uiPulseCtr -= ONEBLOCK;
		}
		if (cL) 					/*�Ƿ����������*/
		{                                                
			if (!__GucDistance[ __LEFT])   		/*�����֧·������������*/
			{         
				sL = __GmLeft.iSpeed;
				sR = __GmRight.iSpeed;
				__GmRight.uiPulse = __GmRight.uiPulseCtr + 38;//42
				__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 38;//42
				while (!__GucDistance[ __LEFT] )	/*������⣬��ֹ����*/
				{
					if((__GmLeft.iSpeed > 40) && (__GmRight.iSpeed > 40))	/*����Ϊת���ٶ�*/
					{
						speedcontrol = 4;
						__GmLeft.iSpeed = 40;
						__GmRight.iSpeed = 40;
					}
					if ((__GmLeft.uiPulseCtr + 10) > __GmLeft.uiPulse)	/*ȷ����֧·������*/
					{
						__mouseCoorUpdate();
						return;
					}
				}
				if((__GmLeft.iSpeed == 40) && (__GmRight.iSpeed == 40)&&(sL != 0)&&(sR != 0)) /*���У��ָ�֮ǰ״̬*/
				{
					__GmLeft.iSpeed = sL;
					__GmRight.iSpeed = sR;
					speedcontrol = 1;
					__GmRight.uiPulse = MAZETYPE * ONEBLOCK;
					__GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
				}
			}
		} 
		else if ( __GucDistance[ __LEFT] ) 			/*�����ǽʱ��ʼ���������  */
			cL = 1;
		if (cR)							 /*�Ƿ��������ұ�*/
		{
			if (!__GucDistance[ __RIGHT] )			/*�ұ���֧·������������*/
			{
				sL = __GmLeft.iSpeed;
				sR = __GmRight.iSpeed;

				__GmRight.uiPulse = __GmRight.uiPulseCtr +34;//42
				__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  +34;//42
				while (!__GucDistance[ __RIGHT])		/*������⣬��ֹ����*/
				{
					if((__GmLeft.iSpeed > 40) && (__GmRight.iSpeed > 40))/*����Ϊת���ٶ�*/
					{
						speedcontrol = 4;
						__GmLeft.iSpeed = 40;
						__GmRight.iSpeed = 40;
					}
					if ((__GmLeft.uiPulseCtr + 10) > __GmLeft.uiPulse)	/*ȷ����֧·������*/
					{
						__mouseCoorUpdate();
						return;
					}
				}
				if((__GmLeft.iSpeed == 40) && (__GmRight.iSpeed == 40)&&(sL != 0)&&(sR != 0)) /*���У��ָ�֮ǰ״̬*/
				{
					__GmLeft.iSpeed = sL;
					__GmRight.iSpeed = sR;
					speedcontrol = 1;
					__GmRight.uiPulse = MAZETYPE * ONEBLOCK;
					__GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
				}
			}
		} 
		else if ( __GucDistance[__RIGHT] )			/*  �ұ���ǽʱ��ʼ�������ұ�*/               
			cR = 1;
		if (__GucDistance[__FRONT] == 0x03)			/*ǰ����ǽ������������*/
		{ 
			speedcontrol = 0;
			sL = __GmLeft.iSpeed;
			sR = __GmRight.iSpeed;
			__GmRight.uiPulse = __GmRight.uiPulseCtr + 60;//60
			__GmLeft.uiPulse  = __GmLeft.uiPulseCtr  + 60;//60
			while (__GucDistance[ __FRONT] == 0x03)			/*������⣬��ֹ����*/
			{
				if((__GmLeft.iSpeed > 55) && (__GmRight.iSpeed > 55))/*����Ϊת���ٶ�*/
				{
					__GmLeft.iSpeed = 55;
					__GmRight.iSpeed = 55;
				}
				if ((__GmLeft.uiPulseCtr + 10) > __GmLeft.uiPulse)	/*ȷ����·���ߣ�����*/
				{
					__mouseCoorUpdate();
					return;
				}
			}
			if((__GmLeft.iSpeed == 55) && (__GmRight.iSpeed == 55)&&(sL != 0)&&(sR != 0)) /*���У��ָ�֮ǰ״̬*/
			{
				__GmLeft.iSpeed = sL;
				__GmRight.iSpeed = sR;
				speedcontrol = 1;
				__GmRight.uiPulse = MAZETYPE * ONEBLOCK;
				__GmLeft.uiPulse  = MAZETYPE * ONEBLOCK;
			}
		}
     }
}



/*********************************************************************************************************
** Function name:       SysTick_ISR
** Descriptions:        ��ʱ�ж�ɨ�衣
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SysTick_ISR(void)
{
    static int32 iL = 0, iR = 0;
    if (__GmLeft.cState == __MOTORSTOP) 
    {
        iL++;
    } 
    else 
    {
        iL = 0;
    }
    if (iL >= 500) 
    {
        GPIOPinWrite(GPIO_PORTD_BASE,
                     __PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
                     0x00);
    }
    if (__GmRight.cState == __MOTORSTOP)
    {
        iR++;
    } 
    else 
    {
        iR = 0;
    }
    if (iR >= 500) 
    {
        GPIOPinWrite(GPIO_PORTD_BASE,
                     __PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
                     0x00);
    }
    __irCheck();
}


/*********************************************************************************************************
** Function name:       __irSendFreq
** Descriptions:        ���ͺ����ߡ�
** input parameters:    __uiFreq:  �����ߵ���Ƶ��
**                      __cNumber: ѡ����Ҫ���õ�PWMģ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __irSendFreq (uint32  __uiFreq, int8  __cNumber)
{
    __uiFreq = SysCtlClockGet() / __uiFreq;
    switch (__cNumber) {

    case 1:
        PWMGenPeriodSet(PWM_BASE, PWM_GEN_1, __uiFreq);                 /*  ����PWM������1������        */
        PWMPulseWidthSet(PWM_BASE, PWM_OUT_2, __uiFreq / 2);            /*  ����PWM2�����������      */
        PWMGenEnable(PWM_BASE, PWM_GEN_1);                              /*  ʹ��PWM������1              */
        break;

    case 2:
        PWMGenPeriodSet(PWM_BASE, PWM_GEN_2, __uiFreq);                 /*  ����PWM������2������        */
        PWMPulseWidthSet(PWM_BASE, PWM_OUT_4, __uiFreq / 2);            /*  ����PWM4�����������      */
        PWMGenEnable(PWM_BASE, PWM_GEN_2);                              /*  ʹ��PWM������2              */
        break;

    default:
        break;
    }
}




/*********************************************************************************************************
** Function name:       __irCheck
** Descriptions:        �����ߴ�������⡣
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __irCheck (void)
{
	static uint8 ucState = 0;
	static uint8 ucIRCheck;

	switch (ucState)
	{
		case 0:
			__irSendFreq(32200, 2);                                         /*  ̽�������������            */
			__irSendFreq(35000, 1);                                         /*  ����б���ϵĴ��������      */
		break;

		case 1:
			ucIRCheck = GPIOPinRead(GPIO_PORTB_BASE, 0x3e);                 /*  ��ȡ������״̬              */
			PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  ��ֹPWM������2              */
			PWMGenDisable(PWM_BASE, PWM_GEN_1);                             /*  ��ֹPWM������1              */
			if (ucIRCheck & __RIGHTSIDE)
				__GucDistance[__RIGHT]  &= 0xfd;
			else 
				__GucDistance[__RIGHT]  |= 0x02;
			if (ucIRCheck & __LEFTSIDE)
				__GucDistance[__LEFT]   &= 0xfd;
			else 
				__GucDistance[__LEFT]   |= 0x02;
			if (ucIRCheck & __FRONTSIDE_R)
				__GucDistance[__FRONTR]  = 0x00;
			else
				__GucDistance[__FRONTR]  = 0x01;
			if (ucIRCheck & __FRONTSIDE_L)
				__GucDistance[__FRONTL]  = 0x00;
			else
				__GucDistance[__FRONTL]  = 0x01;
		break;

		case 2:
			__irSendFreq(35000, 2);   
		break;

		case 3:
			ucIRCheck = GPIOPinRead(GPIO_PORTB_BASE, 0x3e);
			PWMGenDisable(PWM_BASE, PWM_GEN_2);  
			if (ucIRCheck & __FRONTSIDE)
				__GucDistance[__FRONT] &= 0xfd;
			else
				__GucDistance[__FRONT]  |= 0x02;
		break;

		case 4:
			__irSendFreq(36000, 2);                                         /*  ���������ǰ����������Զ��  */
		break;

		case 5:
			ucIRCheck = GPIOPinRead(GPIO_PORTB_BASE, 0x2a);                 /*  ��ȡ������״̬              */
			PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  ��ֹPWM������2              */
		break;

		case 6:
			__irSendFreq(36000, 2);                                         /*  �ظ������ǰ����������Զ��  */
		break;

		case 7:
			ucIRCheck &= GPIOPinRead(GPIO_PORTB_BASE, 0x2a);                /*  ��ȡ������״̬              */
			PWMGenDisable(PWM_BASE, PWM_GEN_2);                             /*  ��ֹPWM������2              */
			if (ucIRCheck & __RIGHTSIDE)
				__GucDistance[__RIGHT] &= 0xfe;
			else
				__GucDistance[__RIGHT] |= 0x01;
			if (ucIRCheck & __LEFTSIDE)
				__GucDistance[__LEFT]  &= 0xfe;
			else 
				__GucDistance[__LEFT]  |= 0x01;
			if (ucIRCheck & __FRONTSIDE)
				__GucDistance[__FRONT] &= 0xfe;
			else
				__GucDistance[__FRONT] |= 0x01;
		break;

		default:
		break;
	}
	ucState = (ucState + 1) % 8;//8                                        /*  ѭ�����    */
}



/*********************************************************************************************************
** Function name:       mouseTurnright
** Descriptions:        ��ת
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseTurnright(void)
{
	while (__GmLeft.cState  != __MOTORSTOP);
	while (__GmRight.cState != __MOTORSTOP);
	/*
	 *  ��ʼ��ת
	 */
//        zlg7289Reset();
//        zlg7289Download(0, 1, 0, 0x0E);
        
//        acc_change(2101139);
//      acc_change(2236068);        
	__GucMouseState   = __TURNRIGHT;
	__GmLeft.uiPulse = 87;//86
	if(turnflag == 4)//֮ǰ��ת�˾���Ϊ��4
		__GmLeft.uiPulse += 2;
	__GmLeft.iSpeed = 80;
	__GmLeft.cState  = __MOTORRUN;
	GucMouseDir  = (GucMouseDir + 1) % 4;                            /*  ������                    */
	while ((__GmLeft.uiPulse - __GmLeft.uiPulseCtr) > 3);//3
	__GmRight.uiPulse  = 7;
	__GmRight.cState  = __MOTORRUN;
	while (__GmRight.cState  != __MOTORSTOP);
	__GmLeft.iSpeed = 40;
	turnflag = 1;//turnflag = 1 ��ת
//      acc_change(1581139);
//        acc_change(2236068);
}


/*********************************************************************************************************
** Function name:       mouseTurnleft
** Descriptions:        ��ת
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseTurnleft(void)
{
	while (__GmLeft.cState  != __MOTORSTOP);
	while (__GmRight.cState != __MOTORSTOP);
	/*
	 *  ��ʼ��ת
	 */
//        zlg7289Reset();
//        zlg7289Download(0, 1, 0, 0x0D);
//        acc_change(2101139);
//      acc_change(2236068);
	__GucMouseState   = __TURNLEFT;
	__GmRight.uiPulse = 87;//86
	if(turnflag == 3)//֮ǰ��ת����Ϊ3
	  __GmRight.uiPulse += 3;
	__GmRight.iSpeed = 80;
	__GmRight.cState  = __MOTORRUN;
	GucMouseDir     = (GucMouseDir + 3) % 4;                            /*  ������                    */
	while ((__GmRight.uiPulse - __GmRight.uiPulseCtr) > 3);
	__GmLeft.uiPulse  = 7;
	__GmLeft.cState   = __MOTORRUN;
	while (__GmLeft.cState != __MOTORSTOP);
	__GmRight.iSpeed = 40;
	turnflag = 2;
//    acc_change(1581139);
//      acc_change(2236068);
}


/*********************************************************************************************************
** Function name:       mouseTurnback
** Descriptions:        ��ת
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseTurnback(void)
{
	/*
	 *  �ȴ�ֹͣ
	 */
	 speedcontrol = 0;
	while (__GmLeft.cState  != __MOTORSTOP);
	while (__GmRight.cState != __MOTORSTOP);
	/*
	 *  ��ʼ��ת
	 */
        __GmLeft.iSpeed=0;
        __GmRight.iSpeed=0;

        acc_change(1551139);

//        zlg7289Reset();
//        zlg7289Download(1, 1, 0, 0x0B);
	__GucMouseState   = __TURNBACK;
	__GmRight.cDir    = __MOTORGOBACK;
	__GmRight.uiPulse = 87;//86
	__GmLeft.uiPulse  = 88;//86
	__GmLeft.cState   = __MOTORRUN;
	__GmRight.cState  = __MOTORRUN;
	GucMouseDir = (GucMouseDir + 2) % 4;                                /*  ������                    */
	while (__GmLeft.cState  != __MOTORSTOP);
	while (__GmRight.cState != __MOTORSTOP);
	__GmRight.cDir = __MOTORGOAHEAD;
        acc_change(2236068);
//        delay(20000000);        
}

/*********************************************************************************************************
** Function name:       __mouseCoorUpdate
** Descriptions:        ���ݵ�ǰ�����������ֵ
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __mouseCoorUpdate (void)
{
    switch (GucMouseDir)
	{
		case 0:
			GmcMouse.cY++;                        
//    zlg7289Download(1,2,1,GmcMouse.cX);
//    zlg7289Download(1,3,0,GmcMouse.cY);

		break;

		case 1:
			GmcMouse.cX++;
//    zlg7289Download(1,2,1,GmcMouse.cX);
//    zlg7289Download(1,3,0,GmcMouse.cY);
		break;

		case 2:
			GmcMouse.cY--;
//    zlg7289Download(1,2,1,GmcMouse.cX);
//    zlg7289Download(1,3,0,GmcMouse.cY);
		break;

		case 3:
			GmcMouse.cX--;
//    zlg7289Download(1,2,1,GmcMouse.cX);
//    zlg7289Download(1,3,0,GmcMouse.cY);
		break;

		default:
		break;
    }
/*    
    if(  (spurt_time==0) &&
         (GmcMouse.cX==XDST0 || GmcMouse.cX==XDST1) &&
         (GmcMouse.cY==YDST0 || GmcMouse.cY==YDST1)
      )
       {
         GucXGoal=GmcMouse.cX;
         GucYGoal=GmcMouse.cY;
         zlg7289Reset();
         zlg7289Download(0, 4, 1, GucXGoal);
         zlg7289Download(0, 5, 0, GucYGoal);
                               
         objectGoTo(GucXStart,GucYStart,__START);                         
 	 GmcMouse.cX = GucXStart;
	 GmcMouse.cY = GucYStart; 
         //zlg7289Reset();
         zlg7289Download(0, 6, 1, GucXStart);
         zlg7289Download(0, 7, 0, GucYStart);
         mouseTurnback();
         GucMouseTask = SPURT;                               //  �����󽫿�ʼ���״̬  

       }
*/
//    __mazeInfDebug();
    __wallCheck();
}


/*********************************************************************************************************
** Function name:       __wallCheck
** Descriptions:        ���ݴ�����������ж��Ƿ����ǽ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      cValue: ����λ������һ�δ�����ǰ�ҡ�1Ϊ��ǽ��0Ϊûǽ��
*********************************************************************************************************/
void __wallCheck (void)
{
    uint8 ucMap = 0;
    ucMap |= MOUSEWAY_B;
    
    if (__GucDistance[__LEFT]  & 0x01)
        ucMap &= ~MOUSEWAY_L;
    else
        ucMap |=  MOUSEWAY_L;

    if (__GucDistance[__FRONT] & 0x01)
        ucMap &= ~MOUSEWAY_F;
    else
        ucMap |=  MOUSEWAY_F;

    if (__GucDistance[__RIGHT] & 0x01)
        ucMap &= ~MOUSEWAY_R;
    else
        ucMap |=  MOUSEWAY_R;

//    zlg7289Reset();
//    zlg7289Download(1,4,0,GucMapBlock[GmcMouse.cX][GmcMouse.cY]);
//    zlg7289Download(1,5,0,ucMap);
    
    if (GucMapBlock[GmcMouse.cX][GmcMouse.cY] == 0x00)  //??? ������
    {
        GucMapBlock[GmcMouse.cX][GmcMouse.cY] = ucMap;
//        Gucstep++;//�����Ĳ���
/*        if(GmcMouse.cX >=7 && GmcMouse.cX <=8 && GmcMouse.cY >=7  && GmcMouse.cY<=8)
        {
           GucMapBlock[GmcMouse.cX][GmcMouse.cY] =  MOUSEWAY_B ;       
           GucXGoal = GmcMouse.cX;
           GucYGoal = GmcMouse.cY;
          zlg7289Reset();
//        zlg7289Download(0, 0, 0, 0x0E);
//        zlg7289Download(0, 1, 0, 0x0E);
//        zlg7289Download(0, 2, 0, 0x0E);
//        zlg7289Download(0, 3, 0, 0x0E);
          zlg7289Download(0, 4, 1, GucXGoal);
          zlg7289Download(0, 5, 0, GucYGoal);
//        zlg7289Download(0, 6, 0, 0x0E);
//        zlg7289Download(0, 7, 0, 0x0E);

        }
*/       
    }

  //ע��������ʾ����ʾ����ʡCPU��Դ  
/*    else {
        if (GucMapBlock[GmcMouse.cX][GmcMouse.cY] != ucMap) {
            zlg7289Download(1,4,0,GucMapBlock[GmcMouse.cX][GmcMouse.cY]);
            zlg7289Download(1,5,0,ucMap);
            while(keyCheck() == false);
        }
    }
*/    
}


/*********************************************************************************************************
** Function name:       SensorDebug
** Descriptions:        ���������ʾ��������״̬���������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sensorDebug (void)
{
    zlg7289Download(2, 0, 0, __GucDistance[__LEFT  ]);
    zlg7289Download(2, 1, 0, __GucDistance[__FRONTL]);
    zlg7289Download(2, 2, 0, __GucDistance[__FRONT ]);
    zlg7289Download(2, 3, 0, __GucDistance[__FRONTR]);    
    zlg7289Download(2, 4, 0, __GucDistance[__RIGHT ]);    
}


/*********************************************************************************************************
** Function name:       __mazeInfDebug
** Descriptions:        ���������ʾ����ǰ������ǰ�����������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
//void __mazeInfDebug (void)
//{
    /*
     *  ��ʾ����
     */
   // switch (GucMouseDir) {
        
  //  case 0:
   //     zlg7289Download(2, 3, 0, 0x47);                                /*  ��ǰ����F��ʾ               */
   //     break;
        
   // case 1:
   //     zlg7289Download(2, 3, 0, 0x77);                                /*  ���ң���R��ʾ               */
   //     break;
        
   // case 2:
   //     zlg7289Download(2, 3, 0, 0x1f);                                /*  �����b��ʾ               */
   //     break;
        
   // case 3:
   //     zlg7289Download(2, 3, 0, 0x0e);                                /*  ������L��ʾ               */
   //     break;
        
  //  default :
  //      zlg7289Download(2, 3, 0, 0x4f);                                /*  ������E��ʾ               */
     //   break;
    //}
    /*
     *  ��ʾ����
     */
   // zlg7289Download(1, 0, 0, GmcMouse.cX / 10);
  //  zlg7289Download(1, 1, 0, GmcMouse.cX % 10);
  //  zlg7289Download(1, 6, 0, GmcMouse.cY / 10);
  //  zlg7289Download(1, 7, 0, GmcMouse.cY % 10);
//}


/*********************************************************************************************************
** Function name:       keyCheck
** Descriptions:        ��ȡ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      true:  �����Ѱ���
**                      false: ����δ����
*********************************************************************************************************/
uint8 keyCheck (void)
{
    if (GPIOPinRead(GPIO_PORTC_BASE, __KEY) == 0)
	{
		__delay(50);
        while(GPIOPinRead(GPIO_PORTC_BASE, __KEY) == 0);
        return(true);
    }
	else
        return(false);
}


/*********************************************************************************************************
** Function name:       voltageDetect
** Descriptions:        ��ѹ��⣬�������7289 EX BOARD ����ʾ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void voltageDetect (void)
{
    unsigned long ulVoltage;
    
    ADCProcessorTrigger(ADC_BASE, 0);                                   /*  ����������һ��A/Dת��       */
    while (!ADCIntStatus(ADC_BASE, 0, false));                          /*  �ȴ�ת������                */
    ADCIntClear(ADC_BASE, 0);                                           /*  ����жϱ�׼λ              */
    ADCSequenceDataGet(ADC_BASE, 0, &ulVoltage);                        /*  ��ȡת�����                */
    
    ulVoltage = ulVoltage * 3000 / 1023;                                /*  ����ʵ�ʼ�⵽�ĵ�ѹֵ(mV)  */
    ulVoltage = ulVoltage * 3 + 350;                                    /*  �����ص�ѹֵ(mV)          */
    
    zlg7289Download(0,6,1,(ulVoltage % 10000) / 1000);                  /*  ��ʾ��ѹֵ�������֣���λV   */
    zlg7289Download(0,7,0,(ulVoltage % 1000 ) / 100 );                  /*  ��ʾ��ѹֵС�����֣���λV   */
}


/*********************************************************************************************************
** Function name:       mouseInit
** Descriptions:        ��LM3S615���������г�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void mouseInit (void)
{
    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_6MHZ );                                 /*  ʹ��PLL��50M                */

    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );                      /*  ʹ��GPIO B������            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC );                      /*  ʹ��GPIO C������            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );                      /*  ʹ��GPIO D������            */
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );                      /*  ʹ��GPIO E������            */
    __keyInit();                                                        /*  ������ʼ��                  */
    __sensorInit();                                                     /*  ��������ʼ��                */
    __stepMotorIint();                                                  /*  ����������Ƴ�ʼ��          */
    __sysTickInit();                                                    /*  ϵͳʱ�ӳ�ʼ��              */
    __ADCInit();
    GucMapBlock[0][0] = 0x01;
    Gucstep++;
}


/*********************************************************************************************************
** Function name:       __sensorInit
** Descriptions:        ���������Ƴ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __sensorInit (void)
{
    /*
     *  �������ӵ��������ź�����ŵ�I/O��Ϊ����ģʽ
     */
    GPIODirModeSet(GPIO_PORTB_BASE,
                   __LEFTSIDE    |
                   __FRONTSIDE_L |
                   __FRONTSIDE   |
                   __FRONTSIDE_R |
                   __RIGHTSIDE,  
                   GPIO_DIR_MODE_IN);
    /*
     *  ��PWM���������߷���ͷ�������Ƶĺ������ź�
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);                          /*  ʹ��PWMģ��                 */
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);                                 /*  PWMʱ�����ã�����Ƶ         */
    /*
     *  ��ʼ��PWM2����PWM����б�Ǻ��ⷢ��ͷ
     */
    GPIOPinTypePWM(GPIO_PORTB_BASE, __IRSEND_BEVEL);                    /*  PB0����ΪPWM����            */
    PWMGenConfigure(PWM_BASE, PWM_GEN_1,                                /*  ����PWM������1              */
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);       /*  �Ӽ�������������            */

    PWMOutputState(PWM_BASE, PWM_OUT_2_BIT, true);                      /*  ʹ��PWM2���                */
    PWMGenDisable(PWM_BASE, PWM_GEN_1);                                 /*  ��ֹPWM������1              */
    /*
     *  ��ʼ��PWM4����PWM������ǰ����������ⷢ��ͷ
     */
    GPIOPinTypePWM(GPIO_PORTE_BASE, __IRSEND_SIDE);                     /*  PE0����ΪPWM����            */
    PWMGenConfigure(PWM_BASE, PWM_GEN_2,                                /*  ����PWM������2              */
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);       /*  �Ӽ�������������            */

    PWMOutputState(PWM_BASE, PWM_OUT_4_BIT, true);                      /*  ʹ��PWM4���                */
    PWMGenDisable(PWM_BASE, PWM_GEN_2);                                 /*  ��ֹPWM������2              */
}


/*********************************************************************************************************
** Function name:       __stepMotorIint
** Descriptions:        ����������Ƴ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __stepMotorIint (void)
{
    uint16 n = 0;
    /*
     *  ����������������İ˸�I/O��Ϊ���ģʽ
     */
    GPIODirModeSet(GPIO_PORTD_BASE,
                   __PHRA1 |
                   __PHRA2 |
                   __PHRB1 |
                   __PHRB2 |
                   __PHLA1 |
                   __PHLA2 |
                   __PHLB1 |
                   __PHLB2,
                   GPIO_DIR_MODE_OUT);
    /*
     *  �����ҵ��ת����λ�ó�ʼ��
     */
    GPIOPinWrite(GPIO_PORTD_BASE,
                 __PHRA1 | __PHRA2 | __PHRB1 | __PHRB2,
                 __PHRA1 | __PHRA2 | __PHRB1 | __PHRB2);
    
    GPIOPinWrite(GPIO_PORTD_BASE,
                 __PHLA1 | __PHLA2 | __PHLB1 | __PHLB2,
                 __PHLA1 | __PHLA2 | __PHLB1 | __PHLB2);
    /*
     *  ��ʼ������/����ʱ��ʱ������ֵ�����ݱ�
     */
 //   int kaishi=2236068;
    __GuiAccelTable[0] = 2136068;//2236068
    __GuiAccelTable[1] = 2136068*0.4142;//906949
    for(n = 2; n < 400; n++) {
        __GuiAccelTable[n] = __GuiAccelTable[n - 1] - (2 * __GuiAccelTable[n - 1] / (4 * n + 1));
    }
    /*
     *  ��ʼ����ʱ��0�����������ҵ����ת��
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);                       /*  ʹ�ܶ�ʱ��0ģ��             */
    TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);                  /*  ����Ϊ32λ���ڼ���ģʽ      */
    TimerLoadSet(TIMER0_BASE, TIMER_A, __GuiAccelTable[0]);             /*  ���ö�ʱʱ��                */
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                    /*  ����Ϊ����ж�              */

    IntEnable(INT_TIMER0A);                                             /*  ʹ�ܶ�ʱ��0�ж�             */
    TimerEnable(TIMER0_BASE, TIMER_A);                                  /*  ʹ�ܶ�ʱ��0                 */
    
    /*
     *  ��ʼ����ʱ��1���������Ƶ����ת��
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);                       /*  ʹ�ܶ�ʱ��1ģ��             */
    TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);                  /*  ����Ϊ32λ���ڼ���ģʽ      */
    TimerLoadSet(TIMER1_BASE, TIMER_A, __GuiAccelTable[0]);             /*  ���ö�ʱʱ��                */
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);                    /*  ����Ϊ����ж�              */

    IntEnable(INT_TIMER1A);                                             /*  ʹ�ܶ�ʱ��1�ж�             */
    TimerEnable(TIMER1_BASE, TIMER_A);                                  /*  ʹ�ܶ�ʱ��1                 */
}


/*********************************************************************************************************
** Function name:       __keyInit
** Descriptions:        �����Ӱ�����GPIO�ڳ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __keyInit (void)
{
    GPIODirModeSet(GPIO_PORTC_BASE, __KEY, GPIO_DIR_MODE_IN);           /*  ���ð�����Ϊ����            */
}


/*********************************************************************************************************
** Function name:       __sysTickInit
** Descriptions:        ϵͳ���Ķ�ʱ����ʼ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __sysTickInit (void)
{
    SysTickPeriodSet(SysCtlClockGet() / 1600);                          /*  ���ö�ʱʱ��Ϊ1ms           */
    SysTickEnable();                                                    /*  ʹ��ϵͳʱ��                */
    SysTickIntEnable();                                                 /*  ʹ��ϵͳʱ���ж�            */
}


/*********************************************************************************************************
** Function name:       __ADCInit
** Descriptions:        �����Ӱ�����GPIO�ڳ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void __ADCInit (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);                          /*  ʹ��ADCģ��                 */
    SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);                         /*  125KSps������               */

    ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);        /*  ����0Ϊ���������������ȼ�Ϊ0*/
    ADCSequenceStepConfigure(ADC_BASE, 0, 0,
                             ADC_CTL_CH0  | 
                             ADC_CTL_IE   | 
                             ADC_CTL_END);                              /*  ���ò������з������Ĳ���    */
    
    ADCHardwareOversampleConfigure(ADC_BASE, 16);                       /*  ����ADC����ƽ�����ƼĴ���   */
    ADCSequenceEnable(ADC_BASE, 0);                                     /*  ʹ�ܲ�������0               */
}

uint8 sanmianqiang(uint8 x,uint8 y){
  if(__GucDistance[__FRONT] == 0x03) return 1;
  else return 0;
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
