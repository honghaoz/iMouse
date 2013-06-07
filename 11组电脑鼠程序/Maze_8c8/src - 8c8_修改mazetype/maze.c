/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           maze.c
** Last modified Date:  2010/08/26
** Last Version:        V1.0
** Description:         ���ݵײ����ȡ�õ��Թ���Ϣ�������������㷨���Ƶ��������һ״̬���������ײ�������
**                      ��ִ�С�
** 
**--------------------------------------------------------------------------------------------------------
** Created By:          Liao Maogang
** Created date:        2007/09/08
** Version:             V1.0
** Descriptions: 
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         Zhang Honghao
** Modified date:       2011/05/12
** Version:             V2.0 beta
** Description:         NO
**
*********************************************************************************************************/


/*********************************************************************************************************
  ����ͷ�ļ�
*********************************************************************************************************/
#include "Maze.h"
#include "Mouse_Drive.h"
/*********************************************************************************************************
  ȫ�ֱ�������  
*********************************************************************************************************/
       uint8    GucXStart                           = 0;//  static             
       uint8    GucYStart                           = 0;//  static                /*  ���X��Y����                */

       uint8    GucXGoal                            = XDST1;
       uint8    GucYGoal                            = YDST1;                /*  �յ�X,Y����                 */
//static uint8    GucXGoal0                            = XDST0;
//static uint8    GucYGoal0                            = YDST0;                /*  �յ�X,Y����                 */
//static uint8    GucXGoal1                           = XDST1;           /*  �յ�Y���꣬������ֵ         */
//static uint8    GucYGoal1                           = YDST1;


       uint8    GucMouseTask                        = WAIT;//static             /*  ״̬������ʼ״̬Ϊ�ȴ�      */

static uint16   GucMapStep[MAZETYPE][MAZETYPE]      = {0xffff};         /*  ����ȸ�ֵ                  */

static MAZECOOR GmcStack[MAZETYPE * MAZETYPE]       = {0};              /*  �ڵȸ�ͼ����ջʹ��          */
static MAZECOOR GmcCrossway[MAZETYPE * MAZETYPE]    = {0};              /*  Main()���ݴ�δ�߹�֧·����  */

       uint8    Gucstep                             = 0 ;               //�����Ĳ���
       uint8    GucDirFlag                          = 0 ;               //�������
       uint8    spurt_time = 0;//��̵Ĵ���
       uint8    maze_time=0;//maze״ִ̬�д���
       uint8    temp_x=0;//��ʱ����X����
       uint8    temp_y=0;//��ʱ����y����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       uint8    GuiStep[MAZETYPE * MAZETYPE][2]     ={0};              //objectto�м�¼������Ϣ
                     //GuiStep[256][2]
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint8    GucMapLoad[MAZETYPE+1][MAZETYPE+1]  = {0};//[17][17]      = {0};        //0��up
      						                   //1��right
							           //2��down
    							           //3��left
static MAZECOOR GucAboveLoad[(MAZETYPE+1)*(MAZETYPE+1)]= {0};//[17*17]        //��㵽�յ�����
static uint8    loadcount                            =  0 ;        //��㵽�յ�������
static uint8    uStartFlag                           =  0 ;        //������X if��0��if��15�� 
static uint8    ushieldflag                          =  0 ;        //�Ƿ���·������
static uint8    Gucsave[(MAZETYPE+1)*(MAZETYPE+1)]   = {0};        //�����·�±�
static uint8    GucMapFlag[MAZETYPE+1][MAZETYPE+1]   = {0};        //����߹���·

static  int8    crosslenth                           =  0 ;
static MAZECOOR Gmcshield[MAZETYPE]                  = {0};


/*********************************************************************************************************
** Function name:       Delay
** Descriptions:        ��ʱ����
** input parameters:    uiD :��ʱ������ֵԽ����ʱԽ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void delay (uint32 uiD)
{
    for (; uiD; uiD--);
}

/*********************************************************************************************************
** Function name:       mapStepEdit
** Descriptions:        ������Ŀ���Ϊ���ĵȸ�ͼ
** input parameters:    cX:    Ŀ�ĵغ�����
**                      cY:    Ŀ�ĵ�������
** output parameters:   GucMapStep[][]:  �������ϵĵȸ�ֵ
** Returned value:      ��
*********************************************************************************************************/
void mapStepEdit (int8  cX, int8  cY)
{
    uint8  n= 0,m=0;                                   /*  GmcStack[]�±�              */
    uint16  ucStep = 1;                              /*  �ȸ�ֵ                      */
    uint8  ucStat = 0;                              /*  ͳ�ƿ�ǰ���ķ�����          */
    int8   i=0,j=0;
    int8   cNBlock =0, cDirTemp;
    uint8  cXcur=cX,cYcur=cY;
    
    uint8  Dircur=0;                                  //����
    uint16 ucsteptemp;				      //
    uint8  choice;
    
    GmcStack[n].cX  = cXcur;                             /*  ���Xֵ��ջ                 */
    GmcStack[n].cY  = cYcur;                             /*  ���Yֵ��ջ                 */
    Gucsave[n]=0;
    n++;
    
    for(m = 0;m < MAZETYPE*MAZETYPE-1; m++)// m < 255   //��ʼ��������Ϣ
    {
	GuiStep[m][0] = 0;
	GuiStep[m][1] = 0;
    }
    m = 0;

    
    for (i = 0;i< MAZETYPE; i++)// i<16     //��ʼ���ȸ�ֵ
        for (j = 0; j<MAZETYPE; j++)// j<16
            GucMapStep[i][j] = 0xffff;       

    while (n)                  //�����ȸ�ͼ
    {
        GucMapStep[cXcur][cYcur] = ucStep;                     
        ucStat = 0;
        if (GucMapBlock[cXcur][cYcur] & 0x08)            /*  ����·                    */
	{    
	    if(Dircur==0x08||Dircur==0)
		ucsteptemp=ucStep+STRWEIGHT;
	    else
		ucsteptemp=ucStep+CURVEWEIGHT;
	    if(GucMapStep[cXcur - 1][cYcur] > (ucsteptemp))
	    {
		ucStat++;
		choice=0x08;
	    }
        }

        if (GucMapBlock[cXcur][cYcur] & 0x04)            /*  �·���·                    */
	{  
            if(Dircur==0x04||Dircur==0)
                    ucsteptemp=ucStep+STRWEIGHT;
            else
                    ucsteptemp=ucStep+CURVEWEIGHT;
            if(GucMapStep[cXcur][cYcur - 1] > (ucsteptemp))
            {
                    ucStat++;
                    choice=0x04;
            }
         }

	if (GucMapBlock[cXcur][cYcur] & 0x02)          /*  �ҷ���·                    */
	{    
            if(Dircur==0x02||Dircur==0)
                    ucsteptemp=ucStep+STRWEIGHT;
            else
                    ucsteptemp=ucStep+CURVEWEIGHT;
            if(GucMapStep[cXcur + 1][cYcur] > (ucsteptemp))
            {
                    ucStat++;
                    choice=0x02;
            }
         }

	if (GucMapBlock[cXcur][cYcur] & 0x01)        /*  �Ϸ���·                    */      
	{     
            if(Dircur==0x01||Dircur==0)
                    ucsteptemp=ucStep+STRWEIGHT;
            else
                    ucsteptemp=ucStep+CURVEWEIGHT;
            if(GucMapStep[cXcur][cYcur + 1] > (ucsteptemp))
            {
                    ucStat++;  
                    choice=0x01;
            }
        }
       
        if (ucStat == 0) 
        {
            n--;
            cXcur = GmcStack[n].cX;
            cYcur = GmcStack[n].cY;
	    Dircur = Gucsave[n];
            ucStep = GucMapStep[cXcur][cYcur];
        } 
        else 
        {
	    if (ucStat > 1) 
            {                                   
                GmcStack[n].cX = cXcur;            
                GmcStack[n].cY = cYcur; 
		Gucsave[n]=Dircur;
                n++;
            }
	    switch(choice)
	    {
		case 0x01:
			if(Dircur==0x01||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x01;
			cYcur++;                                    
			break;
		case 0x02:
			if(Dircur==0x02||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x02;
			cXcur++;                                      
			break;
		case 0x04:
			if(Dircur==0x04||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x04;
			cYcur--;                                     
			break;
		case 0x08:
			if(Dircur==0x08||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x08;
			cXcur--;                                       
			break;
	    }
        }
    }
    //�ȸ�ͼ�������
    cXcur = GmcMouse.cX;
    cYcur = GmcMouse.cY;
    ucsteptemp=0xffff;
    ucStep = GucMapStep[cXcur][cYcur];
    if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
        (GucMapBlock[cXcur][cYcur+1]!=0) &&
        (GucMapStep[cXcur][cYcur + 1] < ucStep)) 
            ucsteptemp=GucMapStep[cXcur][cYcur + 1];

    if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
        (GucMapBlock[cXcur+1][cYcur]!=0) &&
        (GucMapStep[cXcur + 1][cYcur] < ucStep)) 
            if(GucMapStep[cXcur + 1][cYcur] < ucsteptemp)
                    ucsteptemp=GucMapStep[cXcur+1][cYcur];

    if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
        (GucMapBlock[cXcur][cYcur-1]!=0) &&
        (GucMapStep[cXcur][cYcur - 1] < ucStep)) 
            if(GucMapStep[cXcur][cYcur -1] < ucsteptemp)
                    ucsteptemp=GucMapStep[cXcur][cYcur -1];

    if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
        (GucMapBlock[cXcur-1][cYcur]!=0) &&
        (GucMapStep[cXcur - 1][cYcur] < ucStep))
            if(GucMapStep[cXcur -1][cYcur] < ucsteptemp)
                    ucsteptemp=GucMapStep[cXcur -1][cYcur];


    if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
        (GucMapBlock[cXcur][cYcur+1]!=0) &&
        (GucMapStep[cXcur][cYcur + 1]==ucsteptemp)) 
    {                      
        cDirTemp = UP;                                      
    }
    if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
        (GucMapBlock[cXcur+1][cYcur]!=0) &&
        (GucMapStep[cXcur + 1][cYcur]==ucsteptemp)) 
    {            
        cDirTemp = RIGHT;                                   
    }
    if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
        (GucMapBlock[cXcur][cYcur-1]!=0) &&
        (GucMapStep[cXcur][cYcur - 1]==ucsteptemp)) 
    {           
        cDirTemp = DOWN;                                   
    }
    if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
        (GucMapBlock[cXcur-1][cYcur]!=0) &&
        (GucMapStep[cXcur - 1][cYcur]==ucsteptemp))
    {               
        cDirTemp = LEFT;                                    
    }
   cDirTemp = (cDirTemp + 4 - GucMouseDir)%4;  
    switch (cDirTemp) 
    {
            case 1:
                GuiStep[0][1] = 0x1;   //��ת
                break;
	     case 2:
               GuiStep[0][1] = 0x3;    //��ת
                break;
            case 3:
                GuiStep[0][1] = 0x2;   //��ת
                break;
            default:
                break;
    }
    GucMouseDir = (GucMouseDir+cDirTemp)%4;
    m++; 
    
    while ((cXcur != cX) || (cYcur != cY)) 
    {
        ucsteptemp=0xffff;
        ucStep = GucMapStep[cXcur][cYcur];
	if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
            (GucMapBlock[cXcur][cYcur+1]!=0) &&
            (GucMapStep[cXcur][cYcur + 1] < ucStep)) 
		ucsteptemp=GucMapStep[cXcur][cYcur + 1];

        if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
            (GucMapBlock[cXcur+1][cYcur]!=0) &&
            (GucMapStep[cXcur + 1][cYcur] < ucStep)) 
		if(GucMapStep[cXcur + 1][cYcur] < ucsteptemp)
			ucsteptemp=GucMapStep[cXcur+1][cYcur];

	if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
            (GucMapBlock[cXcur][cYcur-1]!=0) &&
            (GucMapStep[cXcur][cYcur - 1] < ucStep)) 
		if(GucMapStep[cXcur][cYcur -1] < ucsteptemp)
			ucsteptemp=GucMapStep[cXcur][cYcur -1];

	if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
            (GucMapBlock[cXcur-1][cYcur]!=0) &&
            (GucMapStep[cXcur - 1][cYcur] < ucStep))
		if(GucMapStep[cXcur -1][cYcur] < ucsteptemp)
			ucsteptemp=GucMapStep[cXcur -1][cYcur];


        if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
            (GucMapBlock[cXcur][cYcur+1]!=0) &&
            (GucMapStep[cXcur][cYcur + 1]==ucsteptemp)) 
        {                      
            cDirTemp = UP;                                      
            if (cDirTemp == GucMouseDir) 
            { 
                cNBlock++;                                     
                cYcur++;
                continue;                                      
            }
        }
        if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
            (GucMapBlock[cXcur+1][cYcur]!=0) &&
            (GucMapStep[cXcur + 1][cYcur]==ucsteptemp)) 
	{            
            cDirTemp = RIGHT;                                   
            if (cDirTemp == GucMouseDir)
	    {         
                cNBlock++;                                      
                cXcur++;
                continue;                                       
            }
        }
        if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
            (GucMapBlock[cXcur][cYcur-1]!=0) &&
            (GucMapStep[cXcur][cYcur - 1]==ucsteptemp)) 
	{           
            cDirTemp = DOWN;                                   
            if (cDirTemp == GucMouseDir) 
	    {    
                cNBlock++;                                      
                cYcur--;
                continue;                                       
            }
        }
        if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
            (GucMapBlock[cXcur-1][cYcur]!=0) &&
            (GucMapStep[cXcur - 1][cYcur]==ucsteptemp))
	{               
            cDirTemp = LEFT;                                    
            if (cDirTemp == GucMouseDir) 
	    {      
                cNBlock++;                                      
                cXcur--;
                continue;                                       
            }
        }
       cDirTemp = (cDirTemp + 4 - GucMouseDir)%4;              /*  ���㷽��ƫ����              */
       GucMouseDir=(GucMouseDir+cDirTemp)%4;
       GuiStep[m][0]=cNBlock;                                  /*  ǰ��cNBlock��               */       
       cNBlock = 0;                                            /*  ��������                    */
       
       switch (cDirTemp) 
       {
            case 1:
                GuiStep[m][1] = 0x1;   //��ת
                break;
	     case 2:
               GuiStep[m][1] = 0x3;    //��ת
                break;
            case 3:
                GuiStep[m][1] = 0x2;   //��ת
                break;
            default:
                break;
       }
       m++;
    }
    GuiStep[m][0] = cNBlock;    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//////mapStepEdit_check/////
/*********************************************************************************************************
** Function name:       mapStepEdit_check
** Descriptions:        ������Ŀ���Ϊ���ĵȸ�ͼ
** input parameters:    cX:    Ŀ�ĵغ�����
**                      cY:    Ŀ�ĵ�������
** output parameters:   GucMapStep[][]:  �������ϵĵȸ�ֵ
** Returned value:      ��
*********************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////
void mapStepEdit_check (int8  cX, int8  cY)
{
    uint8  n=0;//,m=0;                                   /*  GmcStack[]�±�              */
    uint16 ucStep = 1;                              /*  �ȸ�ֵ                      */
    uint8  ucStat = 0;                              /*  ͳ�ƿ�ǰ���ķ�����          */
    int8   i=0,j=0;
//    int8   cNBlock =0, cDirTemp;
    uint8  cXcur=cX,cYcur=cY;//
//    uint8  tempX=GmcMouse.cX,tempY=GmcMouse.cY;

//    GmcMouse.cX = GucXStart;
//    GmcMouse.cY = GucYStart;

    
    uint8  Dircur=0;                                  //����
    uint16 ucsteptemp;				      //
    uint8  choice;
    
    GmcStack[n].cX  = cXcur;                             /*  ���Xֵ��ջ                 */
    GmcStack[n].cY  = cYcur;                             /*  ���Yֵ��ջ                 */
    Gucsave[n]=0;
    n++;
       
//    for(m = 0;m < MAZETYPE*MAZETYPE-1; m++)// m < 255   //��ʼ��������Ϣ
//    {
//	GuiStep[m][0] = 0;
//	GuiStep[m][1] = 0;
//    }
//    m = 0;

    
    for (i = 0;i< MAZETYPE; i++)// i<16     //��ʼ���ȸ�ֵ
        for (j = 0; j<MAZETYPE; j++)// j<16
            GucMapStep[i][j] = 0xffff;       

    while (n)                  //�����ȸ�ͼ
    {
        GucMapStep[cXcur][cYcur] = ucStep;                     
        ucStat = 0;
        if (GucMapBlock[cXcur][cYcur] & 0x08)            /*  ����·                    */
	{    
	    if(Dircur==0x08||Dircur==0)
		ucsteptemp=ucStep+STRWEIGHT;
	    else
		ucsteptemp=ucStep+CURVEWEIGHT;
	    if(GucMapStep[cXcur - 1][cYcur] > (ucsteptemp))
	    {
		ucStat++;
		choice=0x08;
	    }
        }

        if (GucMapBlock[cXcur][cYcur] & 0x04)            /*  �·���·                    */
	{  
            if(Dircur==0x04||Dircur==0)
                    ucsteptemp=ucStep+STRWEIGHT;
            else
                    ucsteptemp=ucStep+CURVEWEIGHT;
            if(GucMapStep[cXcur][cYcur - 1] > (ucsteptemp))
            {
                    ucStat++;
                    choice=0x04;
            }
         }

	if (GucMapBlock[cXcur][cYcur] & 0x02)          /*  �ҷ���·                    */
	{    
            if(Dircur==0x02||Dircur==0)
                    ucsteptemp=ucStep+STRWEIGHT;
            else
                    ucsteptemp=ucStep+CURVEWEIGHT;
            if(GucMapStep[cXcur + 1][cYcur] > (ucsteptemp))
            {
                    ucStat++;
                    choice=0x02;
            }
         }

	if (GucMapBlock[cXcur][cYcur] & 0x01)        /*  �Ϸ���·                    */      
	{     
            if(Dircur==0x01||Dircur==0)
                    ucsteptemp=ucStep+STRWEIGHT;
            else
                    ucsteptemp=ucStep+CURVEWEIGHT;
            if(GucMapStep[cXcur][cYcur + 1] > (ucsteptemp))
            {
                    ucStat++;  
                    choice=0x01;
            }
        }
       
        if (ucStat == 0) 
        {
            n--;
            cXcur = GmcStack[n].cX;
            cYcur = GmcStack[n].cY;
	    Dircur = Gucsave[n];
            ucStep = GucMapStep[cXcur][cYcur];
        } 
        else 
        {
	    if (ucStat > 1) 
            {                                   
                GmcStack[n].cX = cXcur;            
                GmcStack[n].cY = cYcur; 
		Gucsave[n]=Dircur;
                n++;
            }
	    switch(choice)
	    {
		case 0x01:
			if(Dircur==0x01||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x01;
			cYcur++;                                    
			break;
		case 0x02:
			if(Dircur==0x02||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x02;
			cXcur++;                                      
			break;
		case 0x04:
			if(Dircur==0x04||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x04;
			cYcur--;                                     
			break;
		case 0x08:
			if(Dircur==0x08||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x08;
			cXcur--;                                       
			break;
	    }
        }
    }
    //�ȸ�ͼ�������
//    cXcur = GmcMouse.cX;
//    cYcur = GmcMouse.cY;
//    ucsteptemp=0xffff;
//    ucStep = GucMapStep[cXcur][cYcur];
//    if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
//        (GucMapBlock[cXcur][cYcur+1]!=0) &&
//        (GucMapStep[cXcur][cYcur + 1] < ucStep)) 
//            ucsteptemp=GucMapStep[cXcur][cYcur + 1];
//
//    if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
//        (GucMapBlock[cXcur+1][cYcur]!=0) &&
//        (GucMapStep[cXcur + 1][cYcur] < ucStep)) 
//            if(GucMapStep[cXcur + 1][cYcur] < ucsteptemp)
//                    ucsteptemp=GucMapStep[cXcur+1][cYcur];
//
//    if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
//        (GucMapBlock[cXcur][cYcur-1]!=0) &&
//        (GucMapStep[cXcur][cYcur - 1] < ucStep)) 
//            if(GucMapStep[cXcur][cYcur -1] < ucsteptemp)
//                    ucsteptemp=GucMapStep[cXcur][cYcur -1];
//
//    if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
//        (GucMapBlock[cXcur-1][cYcur]!=0) &&
//        (GucMapStep[cXcur - 1][cYcur] < ucStep))
//            if(GucMapStep[cXcur -1][cYcur] < ucsteptemp)
//                    ucsteptemp=GucMapStep[cXcur -1][cYcur];
//
//
//    if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
//        (GucMapBlock[cXcur][cYcur+1]!=0) &&
//        (GucMapStep[cXcur][cYcur + 1]==ucsteptemp)) 
//    {                      
//        cDirTemp = UP;                                      
//    }
//    if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
//        (GucMapBlock[cXcur+1][cYcur]!=0) &&
//        (GucMapStep[cXcur + 1][cYcur]==ucsteptemp)) 
//    {            
//        cDirTemp = RIGHT;                                   
//    }
//    if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
//        (GucMapBlock[cXcur][cYcur-1]!=0) &&
//        (GucMapStep[cXcur][cYcur - 1]==ucsteptemp)) 
//    {           
//        cDirTemp = DOWN;                                   
//    }
//    if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
//        (GucMapBlock[cXcur-1][cYcur]!=0) &&
//        (GucMapStep[cXcur - 1][cYcur]==ucsteptemp))
//    {               
//        cDirTemp = LEFT;                                    
//    }
//   cDirTemp = (cDirTemp + 4 - GucMouseDir)%4;  
//    switch (cDirTemp) 
//    {
//            case 1:
//                GuiStep[0][1] = 0x1;   //��ת
//                break;
//	     case 2:
//               GuiStep[0][1] = 0x3;    //��ת
//                break;
//            case 3:
//                GuiStep[0][1] = 0x2;   //��ת
//                break;
//            default:
//                break;
//    }
//    GucMouseDir = (GucMouseDir+cDirTemp)%4;
//    m++; 
//    
//    while ((cXcur != cX) || (cYcur != cY)) 
//    {
//        ucsteptemp=0xffff;
//        ucStep = GucMapStep[cXcur][cYcur];
//	if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
//            (GucMapBlock[cXcur][cYcur+1]!=0) &&
//            (GucMapStep[cXcur][cYcur + 1] < ucStep)) 
//		ucsteptemp=GucMapStep[cXcur][cYcur + 1];
//
//        if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
//            (GucMapBlock[cXcur+1][cYcur]!=0) &&
//            (GucMapStep[cXcur + 1][cYcur] < ucStep)) 
//		if(GucMapStep[cXcur + 1][cYcur] < ucsteptemp)
//			ucsteptemp=GucMapStep[cXcur+1][cYcur];
//
//	if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
//            (GucMapBlock[cXcur][cYcur-1]!=0) &&
//            (GucMapStep[cXcur][cYcur - 1] < ucStep)) 
//		if(GucMapStep[cXcur][cYcur -1] < ucsteptemp)
//			ucsteptemp=GucMapStep[cXcur][cYcur -1];
//
//	if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
//            (GucMapBlock[cXcur-1][cYcur]!=0) &&
//            (GucMapStep[cXcur - 1][cYcur] < ucStep))
//		if(GucMapStep[cXcur -1][cYcur] < ucsteptemp)
//			ucsteptemp=GucMapStep[cXcur -1][cYcur];
//
//
//        if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
//            (GucMapBlock[cXcur][cYcur+1]!=0) &&
//            (GucMapStep[cXcur][cYcur + 1]==ucsteptemp)) 
//        {                      
//            cDirTemp = UP;                                      
//            if (cDirTemp == GucMouseDir) 
//            { 
//                cNBlock++;                                     
//                cYcur++;
//                continue;                                      
//            }
//        }
//        if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
//            (GucMapBlock[cXcur+1][cYcur]!=0) &&
//            (GucMapStep[cXcur + 1][cYcur]==ucsteptemp)) 
//	{            
//            cDirTemp = RIGHT;                                   
//            if (cDirTemp == GucMouseDir)
//	    {         
//                cNBlock++;                                      
//                cXcur++;
//                continue;                                       
//            }
//        }
//        if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
//            (GucMapBlock[cXcur][cYcur-1]!=0) &&
//            (GucMapStep[cXcur][cYcur - 1]==ucsteptemp)) 
//	{           
//            cDirTemp = DOWN;                                   
//            if (cDirTemp == GucMouseDir) 
//	    {    
//                cNBlock++;                                      
//                cYcur--;
//                continue;                                       
//            }
//        }
//        if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
//            (GucMapBlock[cXcur-1][cYcur]!=0) &&
//            (GucMapStep[cXcur - 1][cYcur]==ucsteptemp))
//	{               
//            cDirTemp = LEFT;                                    
//            if (cDirTemp == GucMouseDir) 
//	    {      
//                cNBlock++;                                      
//                cXcur--;
//                continue;                                       
//            }
//        }
//       cDirTemp = (cDirTemp + 4 - GucMouseDir)%4;              /*  ���㷽��ƫ����              */
//       GucMouseDir=(GucMouseDir+cDirTemp)%4;
//       GuiStep[m][0]=cNBlock;                                  /*  ǰ��cNBlock��               */       
//       cNBlock = 0;                                            /*  ��������                    */
//       
//       switch (cDirTemp) 
//       {
//            case 1:
//                GuiStep[m][1] = 0x1;   //��ת
//                break;
//	     case 2:
//               GuiStep[m][1] = 0x3;    //��ת
//                break;
//            case 3:
//                GuiStep[m][1] = 0x2;   //��ת
//                break;
//            default:
//                break;
//       }
//       m++;
//    }
//    GuiStep[m][0] = cNBlock; 
    
    
//    GmcMouse.cX = tempX;
//    GmcMouse.cY = tempY;

}


/*********************************************************************************************************
** Function name:       mazeBlockDataGet
** Descriptions:        ������Է���ȡ����һ�����Թ����ǽ������
** input parameters:    ucDir: ���������Է���
** output parameters:   ��
** Returned value:      GucMapBlock[cX][cY] : ǽ������
*********************************************************************************************************/
uint8 mazeBlockDataGet (uint8  ucDirTemp)
{
    int8 cX = 0,cY = 0;
    switch (ucDirTemp) 
    {
        case MOUSEFRONT:
            ucDirTemp = GucMouseDir;
            break;
        case MOUSELEFT:
            ucDirTemp = (GucMouseDir + 3) % 4;
            break;
        case MOUSERIGHT:
            ucDirTemp = (GucMouseDir + 1) % 4;
            break;
        default:
            break;
    }
    switch (ucDirTemp) 
    {
        case 0:
            cX = GmcMouse.cX;
            cY = GmcMouse.cY + 1;
            break;   
        case 1:
            cX = GmcMouse.cX + 1;
            cY = GmcMouse.cY;
            break;   
        case 2:
            cX = GmcMouse.cX;
            cY = GmcMouse.cY - 1;
            break;    
        case 3:
            cX = GmcMouse.cX - 1;
            cY = GmcMouse.cY;
            break;   
        default:
            break;
    } 
    return(GucMapBlock[cX][cY]);                                    
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
uint8 process1(uint8 cXsur,uint8 cYsur,int8 cXYflag,uint8 k)
{
	int8 count;
	uint8 i;
	if( cXsur==GmcStack[0].cX && (cXYflag==1||cXYflag==3) ||
	    cYsur==GmcStack[0].cY && (cXYflag==0||cXYflag==2) )
	{
		if((cXsur-GmcStack[0].cX)<0)
			return 0;
		if(cXYflag==0)                 //���Է���Ϊ�ϵ�·��      
		{
			crosslenth=cXsur-GmcStack[0].cX;
			count=crosslenth-1;
			while(count)
			{
				if(!(GucMapBlock[GmcMouse.cX+count][GmcMouse.cY]&0x08))
						return 0;
				count--;
			}
			if(crosslenth!=1)
				for(i=0;i<k;i++)
				{
					if(GucAboveLoad[i].cY<GmcStack[0].cY)
							return 0;
				}
			for(count=0;count<crosslenth;count++)
			{
				Gmcshield[count].cX=GmcMouse.cX+count;
				Gmcshield[count].cY=GmcMouse.cY;
			}
		}
		if(cXYflag==2)                 //���Է���Ϊ�µ�·��      
		{
			crosslenth=cXsur-GmcStack[0].cX;
			count=crosslenth-1;
			while(count)
			{
				if(!(GucMapBlock[GmcMouse.cX+count][GmcMouse.cY]&0x08))
						return 0;
				count--;
			}
			if(crosslenth!=1)
				for(i=0;i<k;i++)
				{
					if(GucAboveLoad[i].cY>GmcStack[0].cY)
							return 0;
				}
			for(count=0;count<crosslenth;count++)
			{
				Gmcshield[count].cX=GmcMouse.cX+count;
				Gmcshield[count].cY=GmcMouse.cY;
			}
		}


		if((cYsur-GmcStack[0].cY)<0)
			return 0;
		if(cXYflag==1)               //���Է���Ϊ�ҵ�·��
		{
			crosslenth=cYsur-GmcStack[0].cY;
			count=crosslenth-1;
			while(count)
			{
				if(!(GucMapBlock[GmcMouse.cX][GmcMouse.cY+count]&0x04))
						return 0;
				count--;
			}
			if(crosslenth!=1)
				for(i=0;i<k;i++)
				{
					if(GucAboveLoad[i].cX<GmcStack[0].cX)
							return 0;
				}
			for(count=0;count<crosslenth;count++)
			{
				Gmcshield[count].cX=GmcMouse.cX;
				Gmcshield[count].cY=GmcMouse.cY+count;
			}
		}
		if(cXYflag==3)               //���Է���Ϊ���·��
		{
			crosslenth=cYsur-GmcStack[0].cY;
			count=crosslenth-1;
			while(count)
			{
				if(!(GucMapBlock[GmcMouse.cX][GmcMouse.cY+count]&0x04))
						return 0;
				count--;
			}
			if(crosslenth!=1)
				for(i=0;i<k;i++)
				{
					if(GucAboveLoad[i].cX>GmcStack[0].cX)
							return 0;
				}
			for(count=0;count<crosslenth;count++)
			{
				Gmcshield[count].cX=GmcMouse.cX;
				Gmcshield[count].cY=GmcMouse.cY+count;
			}
		}
		for(i=0;i<k;i++)
		{
			 if( (GmcStack[0].cY<=8&&GmcStack[0].cX<=8 && GucAboveLoad[i].cX>=8&&GucAboveLoad[i].cY>=8) ||   \
                             (GmcStack[0].cY<=8&&GmcStack[0].cX>=8 && GucAboveLoad[i].cX<=8&&GucAboveLoad[i].cY>=8) ||   \
                             (GmcStack[0].cY>=8&&GmcStack[0].cX<=8 && GucAboveLoad[i].cX>=8&&GucAboveLoad[i].cY<=8) ||   \
                             (GmcStack[0].cY>=8&&GmcStack[0].cX>=8 && GucAboveLoad[i].cX<=8&&GucAboveLoad[i].cY<=8)  )
                                                    return 0;
		}
		return 1;
	}
	return 0;
 }



uint8 process2(uint8 cXsur,uint8 cYsur,int8 cXYflag,uint8 k)
{
	int8 count;
	uint8 i;
	if( cXsur==GmcStack[0].cX && (cXYflag==1||cXYflag==3) ||
	    cYsur==GmcStack[0].cY && (cXYflag==0||cXYflag==2) )
	{
		if(GmcStack[0].cX-cXsur<0)
			return 0;
		if(cXYflag==0)                 //���Է���Ϊ�ϵ�·��      
		{
		    crosslenth=GmcStack[0].cX-cXsur;
			count=crosslenth-1;
			while(count)
			{
				if(!(GucMapBlock[GmcMouse.cX-count][GmcMouse.cY]&0x02))
						return 0;
				count--;
			}
			if(crosslenth!=1)
				for(i=0;i<k;i++)
				{
					if(GucAboveLoad[i].cY<GmcStack[0].cY)
							return 0;
				}
			for(count=0;count<crosslenth;count++)
			{
				Gmcshield[count].cX=GmcMouse.cX-count;
				Gmcshield[count].cY=GmcMouse.cY;
			}
		}
		if(cXYflag==2)                 //���Է���Ϊ�µ�·��      
		{
			crosslenth=GmcStack[0].cX-cXsur;
			count=crosslenth-1;
			while(count)
			{
				if(!(GucMapBlock[GmcMouse.cX-count][GmcMouse.cY]&0x02))
						return 0;
				count--;
			}
			if(crosslenth!=1)
				for(i=0;i<k;i++)
				{
					if(GucAboveLoad[i].cY>GmcStack[0].cY)
							return 0;
				}
			for(count=0;count<crosslenth;count++)
			{
				Gmcshield[count].cX=GmcMouse.cX-count;
				Gmcshield[count].cY=GmcMouse.cY;
			}
		}

		if(GmcStack[0].cY-cYsur<0)
			return 0;
		if(cXYflag==1)               //���Է���Ϊ�ҵ�·��
		{
			crosslenth=GmcStack[0].cY-cYsur;
			count=crosslenth-1;
			while(count)
			{
				if(!(GucMapBlock[GmcMouse.cX][GmcMouse.cY-count]&0x01))
						return 0;
				count--;
			}
			if(crosslenth!=1)
				for(i=0;i<k;i++)
				{
					if(GucAboveLoad[i].cX<GmcStack[0].cX)
							return 0;
				}
			for(count=0;count<crosslenth;count++)
			{
				Gmcshield[count].cX=GmcMouse.cX;
				Gmcshield[count].cY=GmcMouse.cY-count;
			}
		}
		if(cXYflag==3)               //���Է���Ϊ���·��
		{
			crosslenth=GmcStack[0].cY-cYsur;
			count=crosslenth-1;
			while(count)
			{
				if(!(GucMapBlock[GmcMouse.cX][GmcMouse.cY-count]&0x01))
						return 0;
				count--;
			}
			if(crosslenth!=1)
				for(i=0;i<k;i++)
				{
					if(GucAboveLoad[i].cX>GmcStack[0].cX)
							return 0;
				}
			for(count=0;count<crosslenth;count++)
			{
				Gmcshield[count].cX=GmcMouse.cX;
				Gmcshield[count].cY=GmcMouse.cY-count;
			}
		}
		for(i=0;i<k;i++)
		{
			 if( (GmcStack[0].cY<=8&&GmcStack[0].cX<=8 && GucAboveLoad[i].cX>8&&GucAboveLoad[i].cY>8) ||   \
                             (GmcStack[0].cY<=8&&GmcStack[0].cX>=8 && GucAboveLoad[i].cX<8&&GucAboveLoad[i].cY>8) ||   \
                             (GmcStack[0].cY>=8&&GmcStack[0].cX<=8 && GucAboveLoad[i].cX>8&&GucAboveLoad[i].cY<8) ||   \
                             (GmcStack[0].cY>=8&&GmcStack[0].cX>=8 && GucAboveLoad[i].cX<8&&GucAboveLoad[i].cY<8)  )
							return 0;
		}
		return 1;
	}
	return 0;
 }

/********************************************************************************************************
**Function name:        ShieldLoad
**Descriptions:         ����û��Ҫ��·
**input parameters��    ��Է���
**output parameters��   �߼�����
*********************************************************************************************************/
uint8 ShieldLoad(uint8  ucDir)                              //��ת�亯����
{
    int16 i,j;
    int16 n=0;
    uint8 k=0;
    uint8 ucDirTemp=ucDir;
    uint8 ucStat = 0;                                   //ͳ�ƿ�ǰ���ķ�����   
    uint8 cXsur,cYsur;
    uint8 cXdst,cYdst;
    int8  cXYflag=0;               
        
    for(i=0;i<MAZETYPE;i++)//16
      for(j=0;j<MAZETYPE;j++)//16
      {
          if(GucMapBlock[i][j]==0)
             GucMapFlag[i][j]=0xff;
          else
             GucMapFlag[i][j]=GucMapBlock[i][j];
      }

    if(!uStartFlag)					   //�Ľǳ�ʼ��
    {
       GucMapLoad[0][0] = 0x00 | 0x00; 
       GucMapLoad[MAZETYPE][0] = 0x01 | 0x08;//GucMapLoad[16][0]
    }
    else
    {
       GucMapLoad[0][0] = 0x01 | 0x02; 
       GucMapLoad[MAZETYPE][0] = 0x00 | 0x00;//GucMapLoad[16][0]
    }
    GucMapLoad[0][MAZETYPE] = 0x02 | 0x04;//GucMapLoad[0][16]
    GucMapLoad[MAZETYPE][MAZETYPE] = 0x04 | 0x08;//GucMapLoad[16][16]
    
    
    for(j=1;j<MAZETYPE;j++) //16                                //�ı߳�ʼ��
    {
            GucMapLoad[0][j] = 0x01 | 0x04 |  ((~GucMapFlag[0][j-1])&0x01)<<1 | ((~GucMapFlag[0][j])&0x04) >>1 ;  
            //              ��   ��                             ��
//            GucMapLoad[16][j] = 0x01 | 0x04 | ((~GucMapFlag[15][j-1])&0x01)<<3 | ((~GucMapFlag[15][j])&0x04)<<1 ;
            GucMapLoad[MAZETYPE][j] = 0x01 | 0x04 | ((~GucMapFlag[MAZETYPE-1][j-1])&0x01)<<3 | ((~GucMapFlag[MAZETYPE-1][j])&0x04)<<1 ;
            //               ��     ��                          ��
    }
    for(i=1;i<MAZETYPE;i++)//16
    {
            GucMapLoad[i][0] = 0x02 | 0x08 | ((~GucMapFlag[i-1][0])&0x02)>>1 | ((~GucMapFlag[i][0])&0x08)>>3 ;
            //              ��    ��                           ��
//            GucMapLoad[i][16] = 0x02 | 0x08 | ((~GucMapFlag[i-1][15])&0x02)<<1 | ((~GucMapFlag[i][15])&0x08)>>1 ;
            GucMapLoad[i][MAZETYPE] = 0x02 | 0x08 | ((~GucMapFlag[i-1][MAZETYPE-1])&0x02)<<1 | ((~GucMapFlag[i][MAZETYPE-1])&0x08)>>1 ;
            //               ��    ��                          ��
    }
    for(j=1;j<MAZETYPE;j++)//16                                                  
      for(i=1;i<MAZETYPE;i++)//16
      {
          GucMapLoad[i][j] = ((~GucMapFlag[i-1][j])&0x02)>>1 | ((~GucMapFlag[i][j])&0x08)>>3      |  \
     //                                    ��
                             ((~GucMapFlag[i][j-1])&0x01)<<1 | ((~GucMapFlag[i][j])&0x04) >>1     |  \
             //                                    ��                
                             ((~GucMapFlag[i-1][j-1])&0x02)<<1 | ((~GucMapFlag[i][j-1])&0x08)>>1  |  \
             //                                    ��
                             ((~GucMapFlag[i-1][j-1])&0x01)<<3 | ((~GucMapFlag[i-1][j])&0x04)<<1  ;
             //                                    ��
      }
//    if( GucXStart   == MAZETYPE - 1��                       

    GucMapLoad[GucXGoal][GucXGoal]=0;//[8][8]???????????????????GucMapLoad�Ǹ�ʲô�õ�
    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ȷ��Դ��Ŀ������
    switch (ucDirTemp) 
    {
		case MOUSEFRONT:
			ucDirTemp = GucMouseDir;
			break;
		case MOUSELEFT:
			ucDirTemp = (GucMouseDir + 3) % 4;
			break;
		case MOUSERIGHT:
			ucDirTemp = (GucMouseDir + 1) % 4;
			break;
		default:
			break;
    }
    switch (ucDirTemp) 
    {
		case 0:
			cXsur = GmcMouse.cX;
			cYsur = GmcMouse.cY + 1;
			cXdst = GmcMouse.cX + 1;
			cYdst = GmcMouse.cY + 1;
			cXYflag= 0;
			break; 
		case 1:
			cXsur = GmcMouse.cX+1;
			cYsur = GmcMouse.cY;
			cXdst = GmcMouse.cX + 1;
			cYdst = GmcMouse.cY + 1;
			cXYflag=1; 
			break;       
		case 2:
			cXsur = GmcMouse.cX;
			cYsur = GmcMouse.cY;
			cXdst = GmcMouse.cX + 1;
			cYdst = GmcMouse.cY;
			cXYflag=2;
			break;    
		case 3:
			cXsur = GmcMouse.cX;
			cYsur = GmcMouse.cY;
			cXdst = GmcMouse.cX;
			cYdst = GmcMouse.cY + 1;
			cXYflag=3;
			break;     
		default:
			break;
     }
/////////////////////////////////////////////////////////////////////////////////////////////////
	//ȷ���Ƿ��γɻ�·
        GmcStack[n].cX  = cXsur;                             /*  ���Xֵ��ջ                 */
        GmcStack[n].cY  = cYsur;                             /*  ���Yֵ��ջ                 */
        n++;
        
        GucAboveLoad[k].cX=cXsur;                            /*  ��㵽�յ�·��              */
        GucAboveLoad[k].cY=cYsur;
        k++;
        
        for(i=0;i<(MAZETYPE+1);i++)//17
          for(j=0;j<(MAZETYPE+1);j++)//17
            GucMapFlag[i][j]=0;
        GucMapFlag[cXsur][cYsur]=1;
        
        while (n) 
        {
            ucStat = 0;
            if ((GucMapLoad[cXsur][cYsur] & 0x01) &&     
                (GucMapFlag[cXsur][cYsur+ 1]==0))              
					  ucStat++;                           
            if ((GucMapLoad[cXsur][cYsur] & 0x02) &&     
                (GucMapFlag[cXsur + 1][cYsur]==0))                                 
				      ucStat++;                            
            if ((GucMapLoad[cXsur][cYsur] & 0x04) &&
                (GucMapFlag[cXsur][cYsur - 1]==0))
			            ucStat++;                            
            if ((GucMapLoad[cXsur][cYsur] & 0x08) &&
                (GucMapFlag[cXsur - 1][cYsur]==0))
				         ucStat++;                            
            if (ucStat == 0) 
            {
                n--;
                cXsur = GmcStack[n].cX;
                cYsur = GmcStack[n].cY;
                k=Gucsave[n];
            } 
            else 
            {
                loadcount=k+1;
                if (ucStat > 1) 
                {                                       
                    GmcStack[n].cX = cXsur;             
                    GmcStack[n].cY = cYsur;             
                    Gucsave[n]=k;
                    n++;
                }
                if ((GucMapLoad[cXsur][cYsur] & 0x01) &&            /*  �Ϸ���·                    */
                    (GucMapFlag[cXsur][cYsur+ 1]==0)) 
                {   
                    cYsur++;                                      
                    GucAboveLoad[k].cX=cXsur;
                    GucAboveLoad[k].cY=cYsur;
                    k++;
                    GucMapFlag[cXsur][cYsur]=1;
                    if(process1(cXsur,cYsur,cXYflag,k))
                            return 1;
                    continue;
                }
                if ((GucMapLoad[cXsur][cYsur] & 0x02) &&            /*  �ҷ���·                    */
                    (GucMapFlag[cXsur + 1][cYsur]==0)) 
                {   
                    cXsur++;                                    
                    GucAboveLoad[k].cX=cXsur;
                    GucAboveLoad[k].cY=cYsur;
                    k++;
                    GucMapFlag[cXsur][cYsur]=1;
					if(process1(cXsur,cYsur,cXYflag,k))
						return 1;
					continue;
                }
                if ((GucMapLoad[cXsur][cYsur] & 0x04) &&            /*  �·���·                    */
                    (GucMapFlag[cXsur][cYsur - 1]==0))
                {   
                    cYsur--;                                     
                    GucAboveLoad[k].cX=cXsur;
                    GucAboveLoad[k].cY=cYsur;
                    k++;
                    GucMapFlag[cXsur][cYsur]=1;
					if(process1(cXsur,cYsur,cXYflag,k))
						return 1;
					continue; 
                }
                if ((GucMapLoad[cXsur][cYsur] & 0x08) &&             /*  ����·                    */
                    (GucMapFlag[cXsur - 1][cYsur]==0))
                {      
                    cXsur--;                                         
                    GucAboveLoad[k].cX=cXsur;
                    GucAboveLoad[k].cY=cYsur;
                    k++;
                    GucMapFlag[cXsur][cYsur]=1;
					if(process1(cXsur,cYsur,cXYflag,k))
						return 1;
					continue;
                }
            }
        }
/////////////////////////////////////////////////////////////////////////////////////////
        //�ı������յ�
		ucStat=cXsur;
		cXsur=cXdst;
		cXdst=ucStat;
		ucStat=cYsur;
		cYsur=cYdst;
		cYdst=ucStat;

		n=0;
		GmcStack[n].cX  = cXsur;                             /*  ���Xֵ��ջ                 */
		GmcStack[n].cY  = cYsur;                             /*  ���Yֵ��ջ                 */
		n++;
		k=0;
		GucAboveLoad[k].cX=cXsur;                       
		GucAboveLoad[k].cY=cYsur;
		k++;

		for(i=0;i<MAZETYPE+1;i++)//17                                   
		  for(j=0;j<MAZETYPE+1;j++)//17
			GucMapFlag[i][j]=0;
		GucMapFlag[cXsur][cYsur]=1;  

		while (n) 
		{
			ucStat = 0;
			if ((GucMapLoad[cXsur][cYsur] & 0x01) &&     
				(GucMapFlag[cXsur][cYsur+ 1]==0))              
					  ucStat++;                           
			if ((GucMapLoad[cXsur][cYsur] & 0x02) &&     
				(GucMapFlag[cXsur + 1][cYsur]==0))                                 
					  ucStat++;                            
			if ((GucMapLoad[cXsur][cYsur] & 0x04) &&
				(GucMapFlag[cXsur][cYsur - 1]==0))
						ucStat++;                            
			if ((GucMapLoad[cXsur][cYsur] & 0x08) &&
				(GucMapFlag[cXsur - 1][cYsur]==0))
						 ucStat++; 
			if (ucStat == 0) 
			{
				n--;
				cXsur = GmcStack[n].cX;
				cYsur = GmcStack[n].cY;
				k=Gucsave[n];
			} 
			else 
			{
                                loadcount=k+1;
				if (ucStat > 1) 
				{                                       
					GmcStack[n].cX = cXsur;            
					GmcStack[n].cY = cYsur;            
					Gucsave[n]=k;                        //�����±�
					n++;
				}
				if ((GucMapLoad[cXsur][cYsur] & 0x01) &&            /*  �Ϸ���· */
					(GucMapFlag[cXsur][cYsur+ 1]==0)) 
				{    
					cYsur++;                                     
					GucAboveLoad[k].cX=cXsur;
					GucAboveLoad[k].cY=cYsur;
					k++;
					GucMapFlag[cXsur][cYsur]=1;
					if(process2(cXsur,cYsur,cXYflag,k))
						return 1;
					continue;
				}
				if ((GucMapLoad[cXsur][cYsur] & 0x02) &&            /*  �ҷ���·                    */
					(GucMapFlag[cXsur + 1][cYsur]==0)) 
					{   
						cXsur++;                                 
						GucAboveLoad[k].cX=cXsur;
						GucAboveLoad[k].cY=cYsur;
						k++;
						GucMapFlag[cXsur][cYsur]=1;
						if(process2(cXsur,cYsur,cXYflag,k))
							return 1;
						continue;
					}
				if ((GucMapLoad[cXsur][cYsur] & 0x04) &&            /*  �·���·                    */
					(GucMapFlag[cXsur][cYsur - 1]==0))
				{   
					cYsur--;                                    
					GucAboveLoad[k].cX=cXsur;
					GucAboveLoad[k].cY=cYsur;
					k++;
					GucMapFlag[cXsur][cYsur]=1;
					if(process2(cXsur,cYsur,cXYflag,k))
						return 1;
					continue;
				}
				if ((GucMapLoad[cXsur][cYsur] & 0x08) &&             /*  ����·                    */
					(GucMapFlag[cXsur - 1][cYsur]==0))
				{      
					cXsur--;                                      
					GucAboveLoad[k].cX=cXsur;
	         		GucAboveLoad[k].cY=cYsur;
	      			k++;
					GucMapFlag[cXsur][cYsur]=1;
					if(process2(cXsur,cYsur,cXYflag,k))
						return 1;
					continue;
				}
			}
		}
	return 0;    //û���γɻ�·
}
/*********************************************************************************************************
** Function name:       rightMethod
** Descriptions:        ���ַ�����������ǰ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void rightMethod (void)
{
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_R) &&         /*  ��������ұ���·            */
        (mazeBlockDataGet(MOUSERIGHT) == 0x00)) 
    {                       
        if(ShieldLoad(MOUSERIGHT))
        {
           ushieldflag = 1; 
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_R));
                --crosslenth;
           }
           return;
        }
        mouseTurnright();                                             
        return;
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_F) &&         /*  �������ǰ����·            */
        (mazeBlockDataGet(MOUSEFRONT) == 0x00)) 
    {                      
       if(ShieldLoad(MOUSEFRONT))
        {
           ushieldflag = 1 ;
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_F));
                --crosslenth;
           }
           return;
        }
        return;                                                        
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_L) &&         /*  ������������·            */
        (mazeBlockDataGet(MOUSELEFT ) == 0x00)) 
    {                     
       if(ShieldLoad(MOUSELEFT))
        {
           ushieldflag = 1 ;
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_L));
                --crosslenth;
           }
           return;
        }
        mouseTurnleft();                                            
        return;
    }
}
/*********************************************************************************************************
** Function name:       leftMethod
** Descriptions:        ���ַ������������˶�
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void leftMethod (void)
{
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_L) &&         /*  ������������·            */
        (mazeBlockDataGet(MOUSELEFT ) == 0x00)) 
    {                       
       if(ShieldLoad(MOUSELEFT))
        {
           ushieldflag = 1 ;
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_L));
                --crosslenth;
           }
           return;
        }
        mouseTurnleft();                                           
        return;
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_F) &&         /*  �������ǰ����·            */
        (mazeBlockDataGet(MOUSEFRONT) == 0x00)) 
    {                      
       if(ShieldLoad(MOUSEFRONT))
        {
           ushieldflag = 1 ;
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_F));
                --crosslenth;
           }
           return;
        }
        return;                                                        
    }
     if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_R) &&         /*  ��������ұ���·            */
        (mazeBlockDataGet(MOUSERIGHT) == 0x00)) 
    {                       
        if(ShieldLoad(MOUSERIGHT))
        {
           ushieldflag = 1; 
           while(crosslenth)
           {
             GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_R));
                --crosslenth;
           }
           return;
        }
        mouseTurnright();                                              
        return;
    }
}
/*********************************************************************************************************
** Function name:       frontRightMethod
** Descriptions:        ���ҷ���������ǰ���У��������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void frontRightMethod (void)
{
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_F) &&         /*  �������ǰ����·            */
        (mazeBlockDataGet(MOUSEFRONT) == 0x00)) 
    {                     
       if(ShieldLoad(MOUSEFRONT))
        {
           ushieldflag = 1 ;
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_F));
                --crosslenth;
           }
           return;
        }
        return;                                                      
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_R) &&         /*  ��������ұ���·            */
        (mazeBlockDataGet(MOUSERIGHT) == 0x00)) 
    {                      
        if(ShieldLoad(MOUSERIGHT))
        {
           ushieldflag = 1; 
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_R));
                --crosslenth;
           }
           return;
        }
        mouseTurnright();                                               
        return;
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_L) &&         /*  ������������·            */
        (mazeBlockDataGet(MOUSELEFT ) == 0x00)) 
    {                      
       if(ShieldLoad(MOUSELEFT))
        {
           ushieldflag = 1 ;
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_L));
                --crosslenth;
           }
           return;
        }
        mouseTurnleft();                                                
        return;
    }
}
/*********************************************************************************************************
** Function name:       frontLeftMethod
** Descriptions:        ������������ǰ���У��������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void frontLeftMethod (void)
{
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_F) &&         /*  �������ǰ����·            */
        (mazeBlockDataGet(MOUSEFRONT) == 0x00)) 
    {                      
       if(ShieldLoad(MOUSEFRONT))
        {
           ushieldflag = 1 ;
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_F));
                --crosslenth;
           }
           return;
        }
        return;                                                        
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_L) &&         /*  ������������·            */
        (mazeBlockDataGet(MOUSELEFT ) == 0x00)) 
    {                    
       if(ShieldLoad(MOUSELEFT))
        {
           ushieldflag = 1 ;
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_L));
                --crosslenth;
           }
           return;
        }
        mouseTurnleft();                                               
        return;
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_R) &&         /*  ��������ұ���·            */
        (mazeBlockDataGet(MOUSERIGHT) == 0x00)) 
    {                      
        if(ShieldLoad(MOUSERIGHT))
        {
           ushieldflag = 1; 
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_R));
                --crosslenth;
           }
           return;
        }
        mouseTurnright();                                             
        return;
    }
}
///*********************************************************************************************************
//** Function name:       centralMethod
//** Descriptions:        ���ķ��򣬸��ݵ�����Ŀǰ���Թ���������λ�þ���ʹ�ú�����������
//** input parameters:    ��
//** output parameters:   ��
//** Returned value:      ��
//*********************************************************************************************************/
//void centralMethod (void)
//{
//    if (GmcMouse.cX & 0x08) {
//        if (GmcMouse.cY & 0x08) {
//
//            /*
//             *  ��ʱ���������Թ������Ͻ�
//             */ 
//            switch (GucMouseDir) {
//                
//            case UP:                                                    /*  ��ǰ����������              */
//                leftMethod();                                           /*  ���ַ���                    */
//                break;
//
//            case RIGHT:                                                 /*  ��ǰ����������              */
//                rightMethod();                                          /*  ���ַ���                    */
//                break;
//
//            case DOWN:                                                  /*  ��ǰ����������              */
//                frontRightMethod();                                     /*  ���ҷ���                    */
//                break;
//
//            case LEFT:                                                  /*  ��ǰ����������              */
//                frontLeftMethod();                                      /*  ������                    */
//                break;
//
//            default:
//                break;
//            }
//        } else {
//
//            /*
//             *  ��ʱ���������Թ������½�
//             */    
//            switch (GucMouseDir) {
//                
//            case UP:                                                    /*  ��ǰ����������              */
//                frontLeftMethod();                                      /*  ������                    */
//                break;
//
//            case RIGHT:                                                 /*  ��ǰ����������              */
//                leftMethod();                                           /*  ���ַ���                    */
//                break;
//
//            case DOWN:                                                  /*  ��ǰ����������              */
//                rightMethod();                                          /*  ���ַ���                    */
//                break;
//
//            case LEFT:                                                  /*  ��ǰ����������              */
//                frontRightMethod();                                     /*  ���ҷ���                    */
//                break;
//
//            default:
//                break;
//            }
//        }
//    } else {
//        if (GmcMouse.cY & 0x08) {
//
//            /*
//             *  ��ʱ���������Թ������Ͻ�
//             */    
//            switch (GucMouseDir) {
//                
//            case UP:                                                    /*  ��ǰ����������              */
//                rightMethod();                                          /*  ���ַ���                    */
//                break;
//
//            case RIGHT:                                                 /*  ��ǰ����������              */
//                frontRightMethod();                                     /*  ���ҷ���                    */
//                break;
//
//            case DOWN:                                                  /*  ��ǰ����������              */
//                frontLeftMethod();                                      /*  ������                    */
//                break;
//
//            case LEFT:                                                  /*  ��ǰ����������              */
//                leftMethod();                                           /*  ���ַ���                    */
//                break;
//
//            default:
//                break;
//            }
//        } else {
//
//            /*
//             *  ��ʱ���������Թ������½�
//             */    
//            switch (GucMouseDir) {
//                
//            case UP:                                                    /*  ��ǰ����������              */
//                frontRightMethod();                                     /*  ���ҷ���                    */
//                break;
//
//            case RIGHT:                                                 /*  ��ǰ����������              */
//                frontLeftMethod();                                      /*  ������                    */
//                break;
//
//            case DOWN:                                                  /*  ��ǰ����������              */
//                leftMethod();                                           /*  ���ַ���                    */
//                break;
//
//            case LEFT:                                                  /*  ��ǰ����������              */
//                rightMethod();                                          /*  ���ַ���                    */
//                break;
//
//            default:
//                break;
//            }
//        }
//    }
//}

/*********************************************************************************************************
** Function name:       calculatemap
** Descriptions:        ����ĳЩδ�߹�����ĵ������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void calculmap(void)
{
        uint8 i,j;
//        if(GucMapBlock[0][0]==0 && GucMapBlock[0][1]!=0 && GucMapBlock[1][0]!=0)                              //[0,0]
//            GucMapBlock[0][0]=(GucMapBlock[0][1]&0x04)>>2 | (GucMapBlock[1][0]&0x08)>>2;
//        if(GucMapBlock[0][15]==0 && GucMapBlock[0][14]!=0 && GucMapBlock[1][15]!=0)                           //[0,15]
//            GucMapBlock[0][15]=(GucMapBlock[0][14]&0x01)<<2 | (GucMapBlock[1][15]&0x08)>>2;
//        if(GucMapBlock[15][15]==0 && GucMapBlock[15][14]!=0 && GucMapBlock[14][15]!=0)                        //[15,15]
//            GucMapBlock[15][15]=(GucMapBlock[15][14]&0x01)<<2 | (GucMapBlock[14][15]&0x02)<<2;
//        if(GucMapBlock[15][0]==0 && GucMapBlock[15][1]!=0 && GucMapBlock[14][0]!=0)                           //[15,0]
//            GucMapBlock[15][0]=(GucMapBlock[15][1]&0x04)>>2 | (GucMapBlock[14][0]&0x02)<<2;
        
        if(GucMapBlock[0][0]==0 && GucMapBlock[0][1]!=0 && GucMapBlock[1][0]!=0)                              //[0,0]
            GucMapBlock[0][0]=(GucMapBlock[0][1]&0x04)>>2 | (GucMapBlock[1][0]&0x08)>>2;
        if(GucMapBlock[0][MAZETYPE-1]==0 && GucMapBlock[0][MAZETYPE-2]!=0 && GucMapBlock[1][MAZETYPE-1]!=0)                           //[0,15]
            GucMapBlock[0][MAZETYPE-1]=(GucMapBlock[0][MAZETYPE-2]&0x01)<<2 | (GucMapBlock[1][MAZETYPE-1]&0x08)>>2;
        if(GucMapBlock[MAZETYPE-1][MAZETYPE-1]==0 && GucMapBlock[MAZETYPE-1][MAZETYPE-2]!=0 && GucMapBlock[MAZETYPE-2][MAZETYPE-1]!=0)                        //[15,15]
            GucMapBlock[MAZETYPE-1][MAZETYPE-1]=(GucMapBlock[MAZETYPE-1][MAZETYPE-2]&0x01)<<2 | (GucMapBlock[MAZETYPE-2][MAZETYPE-1]&0x02)<<2;
        if(GucMapBlock[MAZETYPE-1][0]==0 && GucMapBlock[MAZETYPE-1][1]!=0 && GucMapBlock[MAZETYPE-2][0]!=0)                           //[15,0]
            GucMapBlock[MAZETYPE-1][0]=(GucMapBlock[MAZETYPE-1][1]&0x04)>>2 | (GucMapBlock[MAZETYPE-2][0]&0x02)<<2;
        
        
//	for(i=1;i<15;i++)                                 //�ı߳�ʼ��
	for(i=1;i<MAZETYPE-1;i++)                                 //�ı߳�ʼ��
	{
            if(GucMapBlock[i][0]==0 && GucMapBlock[i-1][0]!=0 && GucMapBlock[i+1][0]!=0 && GucMapBlock[i][1]!=0)
                GucMapBlock[i][0] = (GucMapBlock[i-1][0]&0x02)<<2 | (GucMapBlock[i][1]&0x04)>>2 | (GucMapBlock[i+1][0]&0x08)>>2 ;  
		//                        ��                              ��                           ��
//            if(GucMapBlock[i][15]==0 && GucMapBlock[i-1][15]!=0 && GucMapBlock[i+1][15]!=0 && GucMapBlock[i][14]!=0)
//                GucMapBlock[i][15] = (GucMapBlock[i-1][15]&0x02)<<2 | (GucMapBlock[i][14]&0x01)<<2 | (GucMapBlock[i+1][15]&0x08)>>2 ; 
            if(GucMapBlock[i][MAZETYPE-1]==0 && GucMapBlock[i-1][MAZETYPE-1]!=0 && GucMapBlock[i+1][MAZETYPE-1]!=0 && GucMapBlock[i][MAZETYPE-2]!=0)
                GucMapBlock[i][MAZETYPE-1] = (GucMapBlock[i-1][MAZETYPE-1]&0x02)<<2 | (GucMapBlock[i][MAZETYPE-2]&0x01)<<2 | (GucMapBlock[i+1][MAZETYPE-1]&0x08)>>2 ; 
		//                        ��                              ��                           ��
        }
	for(j=1;j<MAZETYPE-1;j++)//15
	{
            if(GucMapBlock[0][j]==0 && GucMapBlock[0][j-1]!=0 && GucMapBlock[1][j]!=0 && GucMapBlock[0][j+1]!=0 )
                GucMapBlock[0][j] = (GucMapBlock[0][j-1]&0x01)<<2 | (GucMapBlock[1][j]&0x08)>>2 | (GucMapBlock[0][j+1]&0x04)>>2;  
		//                        ��                              ��                           ��
//            if(GucMapBlock[15][j]==0 && GucMapBlock[15][j-1]!=0 && GucMapBlock[14][j]!=0 && GucMapBlock[15][j+1]!=0)
//                GucMapBlock[15][j] = (GucMapBlock[15][j-1]&0x01)<<2 | (GucMapBlock[14][j]&0x02)<<2 | (GucMapBlock[0][j+1]&0x04)>>2 ; 
            if(GucMapBlock[MAZETYPE-1][j]==0 && GucMapBlock[MAZETYPE-1][j-1]!=0 && GucMapBlock[MAZETYPE-2][j]!=0 && GucMapBlock[MAZETYPE-1][j+1]!=0)
                GucMapBlock[MAZETYPE-1][j] = (GucMapBlock[MAZETYPE-1][j-1]&0x01)<<2 | (GucMapBlock[MAZETYPE-2][j]&0x02)<<2 | (GucMapBlock[0][j+1]&0x04)>>2 ; 
		//                        ��                              ��                           ��
        }
	
        
      for(i=1;i<MAZETYPE-1;i++)//15                                                  
	 for(j=1;j<MAZETYPE-1;j++)//15
	 {
              if(GucMapBlock[i][j]==0 && GucMapBlock[i][j+1]!=0 && GucMapBlock[i+1][j]!=0 && GucMapBlock[i][j-1]!=0 && GucMapBlock[i-1][j]!=0)
                    GucMapBlock[i][j] = (GucMapBlock[i][j+1]&0x04)>>2 | (GucMapBlock[i+1][j]&0x08)>>2 | (GucMapBlock[i][j-1]&0x01)<<2 | (GucMapBlock[i-1][j]&0x02)<<2;
		 //                        ��                              ��                           ��                                ��
	 }
}

/*********************************************************************************************************
** Function name:       crosswayCheck
** Descriptions:        ͳ��ĳ������ڻ�δ�߹���֧·��
** input parameters:    ucX����Ҫ����ĺ�����
**                      ucY����Ҫ�����������
** output parameters:   ��
** Returned value:      ucCt��δ�߹���֧·��
*********************************************************************************************************/
uint8 crosswayCheck (int8  cX, int8  cY)
{
    uint8 ucCt = 0;
    calculmap();        //���㵲����Ϣ
    if ((GucMapBlock[cX][cY] & 0x01) &&                                
        (GucMapBlock[cX][cY + 1]) == 0x00) 
    {                            
        ucCt++;                                                      
    }
    if ((GucMapBlock[cX][cY] & 0x02) &&                               
        (GucMapBlock[cX + 1][cY]) == 0x00) 
    {                          
        ucCt++;                                                       
    }
    if ((GucMapBlock[cX][cY] & 0x04) &&                               
        (GucMapBlock[cX][cY - 1]) == 0x00) 
    {                         
        ucCt++;                                                     
    }
    if ((GucMapBlock[cX][cY] & 0x08) &&                               
        (GucMapBlock[cX - 1][cY]) == 0x00) 
    {                           
        ucCt++;                                                       
    }
    return ucCt;
}
/*********************************************************************************************************
** Function name:       crosswayChoice
** Descriptions:        ѡ��һ��֧·��Ϊǰ������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
/*
void crosswayChoice (void)
{
    switch (SEARCHMETHOD) {
        
    case RIGHTMETHOD:
        rightMethod();
        break;
    
    case LEFTMETHOD:
        leftMethod();
        break;
    
    case CENTRALMETHOD:
        centralMethod();
        break;

    case FRONTRIGHTMETHOD:
        frontRightMethod();
        break;

    case FRONTLEFTMETHOD:
        frontLeftMethod();
        break;

    default:
        break;
    }
}
*/
void crosswayChoice (void)
{
  int sm=0;//����
  
  if(GucXStart==MAZETYPE-1)//15
     if( GmcMouse.cY+GmcMouse.cX>=MAZETYPE-1)//15
  {
    if(GucMouseDir==UP) sm=3;  //��
    if(GucMouseDir==DOWN) sm=0;//��
    if(GucMouseDir==RIGHT) sm=3;//��
    if(GucMouseDir==LEFT) sm=2;// ǰ��
  }  
   if( GmcMouse.cY+GmcMouse.cX<MAZETYPE-1)//15
  {
    if(GucMouseDir==UP) sm=3; //  ��
    if(GucMouseDir==DOWN) sm=0;// ��
    if(GucMouseDir==RIGHT) sm=3;// ��
    if(GucMouseDir==LEFT) sm=0;//  ��
  }
    if(GucXStart == 0)
   {
     if( GmcMouse.cY>=GmcMouse.cX)
     {
       if(GucMouseDir==UP) sm=0;  //��
       if(GucMouseDir==DOWN) sm=3;//��
       if(GucMouseDir==RIGHT) sm=1;//ǰ��
       if(GucMouseDir==LEFT) sm=0;// ��
     }  
    if( GmcMouse.cY<GmcMouse.cX)
    {
    if(GucMouseDir==UP) sm=0; //  ��
    if(GucMouseDir==DOWN) sm=3;// ��
    if(GucMouseDir==RIGHT) sm=3;// ��
    if(GucMouseDir==LEFT) sm=0;//  ��
    }
   }
  switch(sm){
  case 0: rightMethod(); break;
  case 1: frontLeftMethod();break;
  case 2: frontRightMethod();break;
  case 3: leftMethod();break;
  }
}
/////////////////////////////////////////////////////////////////////////////////////
//////delect no effect statck node
/////////////////////////////////////////////////////////////////////////////////////
uint8 delnode(uint8 n)
{
    int i=0,j=0,k=0;
    uint8 flag[4]={0};
    uint8 count=n;
    for(j=0;j<count;)
    {
        flag[0]=0;
        flag[1]=0;
        flag[2]=0;
        flag[3]=0;
        for(i=0;i<loadcount;i++)
        {
           if(GmcCrossway[j].cX>=GucAboveLoad[i].cX && GmcCrossway[j].cY>=GucAboveLoad[i].cY)
             flag[0]=1;
           if(GmcCrossway[j].cX>=GucAboveLoad[i].cX && GmcCrossway[j].cY<GucAboveLoad[i].cY)
             flag[1]=1;
           if(GmcCrossway[j].cX<GucAboveLoad[i].cX && GmcCrossway[j].cY<GucAboveLoad[i].cY)
             flag[2]=1;
           if(GmcCrossway[j].cX<GucAboveLoad[i].cX && GmcCrossway[j].cY>=GucAboveLoad[i].cY)
             flag[3]=1;
        }
        if(flag[0]&&flag[1]&&flag[2]&&flag[3])
        {
           k=j;
           while(k<count)
           {
             GmcCrossway[k].cX = GmcCrossway[k+1].cX;
             GmcCrossway[k].cY = GmcCrossway[k+1].cY;
             k++;
           }
           count--;
           continue;
        }
        j++;
    }
    return count;
}
/*********************************************************************************************************
** Function name:       objecttest
** Descriptions:        �����Ƿ��Ҫ���غ���
** input parameters:    Ŀ������
** output parameters:   ��
** Returned value:      ��//�з���ֵ
*********************************************************************************************************/
uint8 objecttest (int8  cX, int8  cY, uint8 count)
{
    uint8 n=0;                                        /*  GmcStack[]�±�              */
    uint16 ucStep    = 1;                              /*  �ȸ�ֵ                      */
    uint8 ucStat    = 0;                              /*  ͳ�ƿ�ǰ���ķ�����          */
    int8  i=0,j=0;
    int8  cDirTemp;  
    uint8 cXcur=cX,cYcur=cY;
    uint8 MouseDirTemp  = GucMouseDir;
    uint8 cXtemp = GmcMouse.cX,cYtemp = GmcMouse.cY;
    
    uint8  counttemp=count;
    uint8  choice; 
    uint8  Dircur=0;                                  //����
    uint16 ucsteptemp;				      //
    
    
    
    GmcStack[n].cX  = cXcur;                             /*  ���Xֵ��ջ                 */
    GmcStack[n].cY  = cYcur;                             /*  ���Yֵ��ջ                 */
    Gucsave[n]=0;
    n++;
//////////////////////////////////////////////////////////////////////////////////////////////
    //�����ȸ�ͼĿ����Ϊ�ҳ����غ�ķ���  �Ľ�����objectto���õ����ļ����ɽ����ں�������
    //�Ľ�����Ҫ���� 
    for (i = 0; i<MAZETYPE; i++)//16     //��ʼ���ȸ�ֵ
        for (j = 0; j<MAZETYPE; j++)//16
            GucMapStep[i][j] = 0xffff;       

    while (n)                  //�����ȸ�ͼ
    {
        GucMapStep[cXcur][cYcur] = ucStep;                     
        ucStat = 0;
        if (GucMapBlock[cXcur][cYcur] & 0x08)            /*  ����·                    */
	{    
	    if(Dircur==0x08||Dircur==0)
		ucsteptemp=ucStep+STRWEIGHT;
	    else
		ucsteptemp=ucStep+CURVEWEIGHT;
	    if(GucMapStep[cXcur - 1][cYcur] > (ucsteptemp))
	    {
		ucStat++;
		choice=0x08;
	    }
        }

        if (GucMapBlock[cXcur][cYcur] & 0x04)            /*  �·���·                    */
	{  
            if(Dircur==0x04||Dircur==0)
                    ucsteptemp=ucStep+STRWEIGHT;
            else
                    ucsteptemp=ucStep+CURVEWEIGHT;
            if(GucMapStep[cXcur][cYcur - 1] > (ucsteptemp))
            {
                    ucStat++;
                    choice=0x04;
            }
         }

	if (GucMapBlock[cXcur][cYcur] & 0x02)          /*  �ҷ���·                    */
	{    
            if(Dircur==0x02||Dircur==0)
                    ucsteptemp=ucStep+STRWEIGHT;
            else
                    ucsteptemp=ucStep+CURVEWEIGHT;
            if(GucMapStep[cXcur + 1][cYcur] > (ucsteptemp))
            {
                    ucStat++;
                    choice=0x02;
            }
         }

	if (GucMapBlock[cXcur][cYcur] & 0x01)        /*  �Ϸ���·                    */      
	{     
            if(Dircur==0x01||Dircur==0)
                    ucsteptemp=ucStep+STRWEIGHT;
            else
                    ucsteptemp=ucStep+CURVEWEIGHT;
            if(GucMapStep[cXcur][cYcur + 1] > (ucsteptemp))
            {
                    ucStat++;  
                    choice=0x01;
            }
        }
       
        if (ucStat == 0) 
        {
            n--;
            cXcur = GmcStack[n].cX;
            cYcur = GmcStack[n].cY;
			Dircur = Gucsave[n];
            ucStep = GucMapStep[cXcur][cYcur];
        } 
        else 
        {
	    if (ucStat > 1) 
            {                                   
                GmcStack[n].cX = cXcur;            
                GmcStack[n].cY = cYcur; 
		Gucsave[n]=Dircur;
                n++;
            }
	    switch(choice)
	    {
		case 0x01:
			if(Dircur==0x01||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x01;
			cYcur++;                                    
			break;
		case 0x02:
			if(Dircur==0x02||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x02;
			cXcur++;                                      
			break;
		case 0x04:
			if(Dircur==0x04||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x04;
			cYcur--;                                     
			break;
		case 0x08:
			if(Dircur==0x08||Dircur==0)
			    ucStep+=STRWEIGHT;
			else
			    ucStep+=CURVEWEIGHT;
			Dircur=0x08;
			cXcur--;                                       
			break;
	    }
        }
    }
    //�ȸ�ͼ�������
    cXcur = GmcMouse.cX;
    cYcur = GmcMouse.cY;
    ucsteptemp=0xffff;
    ucStep = GucMapStep[cXcur][cYcur];
    if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
        (GucMapBlock[cXcur][cYcur+1]!=0) &&
        (GucMapStep[cXcur][cYcur + 1] < ucStep)) 
            ucsteptemp=GucMapStep[cXcur][cYcur + 1];

    if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
        (GucMapBlock[cXcur+1][cYcur]!=0) &&
        (GucMapStep[cXcur + 1][cYcur] < ucStep)) 
            if(GucMapStep[cXcur + 1][cYcur] < ucsteptemp)
                    ucsteptemp=GucMapStep[cXcur+1][cYcur];

    if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
        (GucMapBlock[cXcur][cYcur-1]!=0) &&
        (GucMapStep[cXcur][cYcur - 1] < ucStep)) 
            if(GucMapStep[cXcur][cYcur -1] < ucsteptemp)
                    ucsteptemp=GucMapStep[cXcur][cYcur -1];

    if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
        (GucMapBlock[cXcur-1][cYcur]!=0) &&
        (GucMapStep[cXcur - 1][cYcur] < ucStep))
            if(GucMapStep[cXcur -1][cYcur] < ucsteptemp)
                    ucsteptemp=GucMapStep[cXcur -1][cYcur];


    if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
        (GucMapBlock[cXcur][cYcur+1]!=0) &&
        (GucMapStep[cXcur][cYcur + 1]==ucsteptemp)) 
    {                      
        cDirTemp = UP;                                      
    }
    if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
        (GucMapBlock[cXcur+1][cYcur]!=0) &&
        (GucMapStep[cXcur + 1][cYcur]==ucsteptemp)) 
    {            
        cDirTemp = RIGHT;                                   
    }
    if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
        (GucMapBlock[cXcur][cYcur-1]!=0) &&
        (GucMapStep[cXcur][cYcur - 1]==ucsteptemp)) 
    {           
        cDirTemp = DOWN;                                   
    }
    if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
        (GucMapBlock[cXcur-1][cYcur]!=0) &&
        (GucMapStep[cXcur - 1][cYcur]==ucsteptemp))
    {               
        cDirTemp = LEFT;                                    
    }
    cDirTemp = (cDirTemp + 4 - GucMouseDir)%4;              /*  ���㷽��ƫ����              */
    GucMouseDir=(GucMouseDir+cDirTemp)%4; 
    while ((cXcur != cX) || (cYcur != cY)) 
    {
        ucsteptemp=0xffff;
        ucStep = GucMapStep[cXcur][cYcur];
	if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
            (GucMapBlock[cXcur][cYcur+1]!=0) &&
            (GucMapStep[cXcur][cYcur + 1] < ucStep)) 
		ucsteptemp=GucMapStep[cXcur][cYcur + 1];

        if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
            (GucMapBlock[cXcur+1][cYcur]!=0) &&
            (GucMapStep[cXcur + 1][cYcur] < ucStep)) 
		if(GucMapStep[cXcur + 1][cYcur] < ucsteptemp)
			ucsteptemp=GucMapStep[cXcur+1][cYcur];

	if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
            (GucMapBlock[cXcur][cYcur-1]!=0) &&
            (GucMapStep[cXcur][cYcur - 1] < ucStep)) 
		if(GucMapStep[cXcur][cYcur -1] < ucsteptemp)
			ucsteptemp=GucMapStep[cXcur][cYcur -1];

	if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
            (GucMapBlock[cXcur-1][cYcur]!=0) &&
            (GucMapStep[cXcur - 1][cYcur] < ucStep))
		if(GucMapStep[cXcur -1][cYcur] < ucsteptemp)
			ucsteptemp=GucMapStep[cXcur -1][cYcur];


        if ((GucMapBlock[cXcur][cYcur] & 0x01) &&               /*  �Ϸ���·                    */
            (GucMapBlock[cXcur][cYcur+1]!=0) &&
            (GucMapStep[cXcur][cYcur + 1]==ucsteptemp)) 
        {                      
            cDirTemp = UP;                                      
            if (cDirTemp == GucMouseDir) 
            {                                     
                cYcur++;
                continue;                                      
            }
        }
        if ((GucMapBlock[cXcur][cYcur] & 0x02) &&               /*  �ҷ���·                    */
            (GucMapBlock[cXcur+1][cYcur]!=0) &&
            (GucMapStep[cXcur + 1][cYcur]==ucsteptemp)) 
	{            
            cDirTemp = RIGHT;                                   
            if (cDirTemp == GucMouseDir)
	    {                                              
                cXcur++;
                continue;                                       
            }
        }
        if ((GucMapBlock[cXcur][cYcur] & 0x04) &&               /*  �·���·                    */
            (GucMapBlock[cXcur][cYcur-1]!=0) &&
            (GucMapStep[cXcur][cYcur - 1]==ucsteptemp)) 
	{           
            cDirTemp = DOWN;                                   
            if (cDirTemp == GucMouseDir) 
	    {                                       
                cYcur--;
                continue;                                       
            }
        }
        if ((GucMapBlock[cXcur][cYcur] & 0x08) &&               /*  ����·                    */
            (GucMapBlock[cXcur-1][cYcur]!=0) &&
            (GucMapStep[cXcur - 1][cYcur]==ucsteptemp))
	{               
            cDirTemp = LEFT;                                    
            if (cDirTemp == GucMouseDir) 
	    {                                          
                cXcur--;
                continue;                                       
            }
        }
       cDirTemp = (cDirTemp + 4 - GucMouseDir)%4;              /*  ���㷽��ƫ����              */
       GucMouseDir=(GucMouseDir+cDirTemp)%4; 
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////  
    GmcMouse.cX = cX;
    GmcMouse.cY = cY;
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_R) &&         /*  ��������ұ���· */
        (mazeBlockDataGet(MOUSERIGHT) == 0x00)) 
    {                   
        if(ShieldLoad(MOUSERIGHT))
        {
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY]&= (~(MOUSEWAY_R));
                --crosslenth;
           }
           counttemp=delnode(counttemp);
        }
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_F) &&         /*  �������ǰ����·            */
        (mazeBlockDataGet(MOUSEFRONT) == 0x00)) 
    {                   
       if(ShieldLoad(MOUSEFRONT))
        {
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &=(~(MOUSEWAY_F));
                --crosslenth;
           }
           counttemp=delnode(counttemp);
        }                                                          
    }
    if ((GucMapBlock[GmcMouse.cX][GmcMouse.cY] & MOUSEWAY_L) &&         /*  ������������·            */
        (mazeBlockDataGet(MOUSELEFT ) == 0x00)) 
    {                     
       if(ShieldLoad(MOUSELEFT))
        {
           while(crosslenth)
           {
                GucMapBlock[Gmcshield[crosslenth-1].cX][Gmcshield[crosslenth-1].cY] &= (~(MOUSEWAY_L));
                --crosslenth;
           }
        }
       counttemp=delnode(counttemp);
    }
    GmcMouse.cX = cXtemp;
    GmcMouse.cY = cYtemp;
    GucMouseDir = MouseDirTemp;
    return counttemp;
}


/*********************************************************************************************************
** Function name:       main
** Descriptions:        ������
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
main (void)
{
    uint8 n          = 0;                                               /*  GmcCrossway[]�±�           */
    uint8 ucRoadStat = 0;                                               /*  ͳ��ĳһ�����ǰ����֧·��  */
    uint8 ucTemp     = 0;                                               /*  ����START״̬������ת��     */
    mouseInit();                                                        /*  �ײ������ĳ�ʼ��            */
    zlg7289Init();                                                      /*  ��ʾģ���ʼ��              */
    uint8 maze_time_ctr=0;//��¼mazesearch״̬����Ĵ���
//    uint8 denggaozhi = 0xff;
//    uint8 denggaozhi_temp = 0xff;

    while (1) 
    {
        switch (GucMouseTask) 
        {                                           
            case WAIT:
                sensorDebug(); //ѭ�����Ժ�����
                voltageDetect();
                zlg7289Download(1, 5, 1, 0x0A);
                delay(100000);
                if (keyCheck() == true) 
                {                              
                    zlg7289Reset();                                       
                    GucMouseTask = START;
                }
                break;
                
            case START:                                                     /*  �жϵ��������ĺ�����      */
                mazeSearch();                                               /*  ��ǰ����                    */
//                mouseTurnright();
//                mazeSearch();                                               /*  ��ǰ����                    */
//                mouseTurnleft();
//                mouseTurnback();
//                backTurnleft();
//                while (1) 
//                {
//                    if (keyCheck() == true) 
//                    {
//                        break;
//                    }
//                    sensorDebug();
//                    delay(20000);
//                }
                
                if (GucMapBlock[GmcMouse.cX][GmcMouse.cY] & 0x08) 
                {         
                    uStartFlag = 1;                      //��־�����ڶϿ����
                    GucXStart   = MAZETYPE - 1;                       
                    GmcMouse.cX = MAZETYPE - 1;                       
                    ucTemp = GmcMouse.cY;
                    GucXGoal=0;//8;
                    //GucYGoal= MAZETYPE -1;//7;
                    
                    do 
                    {
                        GucMapBlock[MAZETYPE - 1][ucTemp] = GucMapBlock[0][ucTemp];
                        GucMapBlock[0 ][ucTemp] = 0;
                    }while (ucTemp--);
                    GmcCrossway[n].cX = MAZETYPE - 1;
                    GmcCrossway[n].cY = 0;
                    n++;
                    GucMouseTask = MAZESEARCH;                           
                }
                if (GucMapBlock[GmcMouse.cX][GmcMouse.cY] & 0x02) 
                {        
                    GmcCrossway[n].cX = 0;
                    GmcCrossway[n].cY = 0;
                    n++;
                    GucMouseTask = MAZESEARCH;                            
                }
                break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////             
            case MAZESEARCH:
//              if(spurt_time!=0){
//                mapStepEdit_check(GucXGoal,GucYGoal);
//                denggaozhi_temp=GucMapStep[GucXStart][GucYStart];
//                 zlg7289Download(1, 2, 0, denggaozhi_temp / 10);
//                 zlg7289Download(1, 3, 0, denggaozhi_temp % 10);
//               
//              }
//              maze_time_ctr++;
//                zlg7289Download(1, 4, 0, maze_time_ctr / 10);
//                zlg7289Download(1, 5, 0, maze_time_ctr % 10);
              

              if(spurt_time!=0) maze_time++;
              
                zlg7289Download(1, 6, 0, maze_time / 10);
                zlg7289Download(1, 7, 0, maze_time % 10);
                
                ucRoadStat = crosswayCheck(GmcMouse.cX,GmcMouse.cY);        /*  ͳ�ƿ�ǰ����֧·��          */
                
    //           if( GmcMouse.cX == 4 && GmcMouse.cY == 0) //�����õ�
    //           ucTemp++;
                if (ucRoadStat) 
                {                                                           /*  �п�ǰ������                */
                    crosswayChoice(); //crosswayChoice�Ƿ���ѡ�����������ķ���    /*  �����ַ�������ѡ��ǰ������  */
                    if(ushieldflag == 1)
                    {
                       ushieldflag = 0;
                       n=delnode(n);
                       break;
                    }
                    if (ucRoadStat > 1) 
                    {                                                       /*  �ж�����ǰ�����򣬱�������  */
                        GmcCrossway[n].cX = GmcMouse.cX;
                        GmcCrossway[n].cY = GmcMouse.cY;
                        n++;
                    }
                    mazeSearch();                                           /*  ǰ��һ��                    */
                } 
                else 
                {                                                           /*  û�п�ǰ�����򣬻ص����֧·*/
                    while (--n) 
                    {
;                        
                       // if(Gucstep>=STEPNUM)
                       // {
                       //       if( GucMapBlock[8][8]!=0 || GucMapBlock[8][7]!=0 || GucMapBlock[7][8]!=0 || GucMapBlock[7][7]!=0 )
                       //       {
                       //              Gucstep = 0 ;
                       //              n=1;
                       //              continue;
                       //       }
                       //  }
;
                        ucRoadStat = crosswayCheck(GmcCrossway[n].cX,GmcCrossway[n].cY);
                        if (ucRoadStat) 
                        {  
                            n=objecttest(GmcCrossway[n].cX,GmcCrossway[n].cY,n);
                            ucRoadStat = crosswayCheck(GmcCrossway[n].cX,GmcCrossway[n].cY);
                            if(ucRoadStat==0)
                            {       
                               continue;
                            }         
                            objectGoTo(GmcCrossway[n].cX,GmcCrossway[n].cY,__BACK);
		 	    GmcMouse.cX = GmcCrossway[n].cX;
		            GmcMouse.cY = GmcCrossway[n].cY;
;                                        
//                          crosswayChoice();            //��ʱ·����objecttest�м��㣬���ushieldflag ��= 1 ��ת�䣡
//                          if (ucRoadStat > 1) 
//                          {
//                            n++;
//                          }
//                          mazeSearch();
//;
                            break;
                        }
                    }
                }
                 if (n == 0){                                                     //  ���������е�֧·���ص����  
                   //Gucstep = 0 ; 
                   objectGoTo(GucXStart,GucYStart,__START);//GmcCrossway[0].cX, GmcCrossway[0].cY ,__START
                   GmcMouse.cX = GucXStart;//GmcCrossway[0].cX;
                   GmcMouse.cY = GucYStart;//GmcCrossway[0].cY;
                   //delay(2000000);
                     mouseTurnback();
		   // delay(1000000);
                   GucMouseTask = SPURT;                               //  �����󽫿�ʼ���״̬        
                 }              
                if((spurt_time==0) && 
                   ((GmcMouse.cX==GucXGoal) && (GmcMouse.cY==GucYGoal)) 
                       ){
                   //      GucXGoal=GmcMouse.cX;
                   //      GucYGoal=GmcMouse.cY;
                         
//                         zlg7289Reset();
//                         zlg7289Download(0, 4, 1, GucXGoal);
//                         zlg7289Download(0, 5, 0, GucYGoal);
//                mapStepEdit_check(GucXGoal,GucYGoal);
//                denggaozhi=GucMapStep[GucXStart][GucYStart];
//                zlg7289Download(1, 0, 0, denggaozhi / 10);
//                zlg7289Download(1, 1, 0, denggaozhi % 10);
                         
                         objectGoTo(GucXStart,GucYStart,__START);                          /*  �ص����                    */
                         GmcMouse.cX = GucXStart;
                         GmcMouse.cY = GucYStart; 
                         mouseTurnback();
                         GucMouseTask = SPURT;                               //  �����󽫿�ʼ���״̬  
                       }
                if(n!=0 &&  maze_time==15){//(denggaozhi_temp>denggaozhi || maze_time==37)){
//                  temp_x=GmcMouse.cX;
//                  temp_y=GmcMouse.cY;
//                  objectGoTo(GucXStart,GucYStart,__START);                          /*  �ص����                    */
//                  GmcMouse.cX = GucXStart;
//                  GmcMouse.cY = GucYStart; 
//                  mouseTurnback();
//                  GucMouseTask = SPURT;                               //  �����󽫿�ʼ���״̬  
                  
                  if(!sanmianqiang(GmcMouse.cX,GmcMouse.cY)){
                      temp_x=GmcMouse.cX;
                      temp_y=GmcMouse.cY;
                      objectGoTo(GucXStart,GucYStart,__START);                          /*  �ص����                    */
                      GmcMouse.cX = GucXStart;
                      GmcMouse.cY = GucYStart; 
                      mouseTurnback();
                      GucMouseTask = SPURT;                               //  �����󽫿�ʼ���״̬  
                  }
                  else maze_time-=7;

                }
                break;
   
        case SPURT:             
 //       zlg7289Reset();
   //     zlg7289Download(0, 0, 0, 0x0E);
     //   zlg7289Download(0, 1, 0, 0x0E);
       // zlg7289Download(0, 2, 0, 0x0E);
      //  zlg7289Download(0, 3, 0, 0x0E);
 //       zlg7289Download(0, 4, 0, 0x0E);
   //     zlg7289Download(0, 5, 0, 0x0E);
     //   zlg7289Download(0, 6, 0, 0x0E);
       // zlg7289Download(0, 7, 0, 0x0E);
          spurt_time++;
          switch(spurt_time)
          {
          case 1:
            objectGoTo(GucXGoal,GucYGoal,__START);//objectGoTo(GucXGoal,GucYGoal,__END);                                            /*  ���е�ָ��Ŀ���            */
            GmcMouse.cX = GucXGoal;
            GmcMouse.cY = GucYGoal;
            mouseTurnback();
          //mouseTurnback();
          //mouseSpurt();                                               /*  ������·�������յ�          */
            GucMouseTask = MAZESEARCH;
            maze_time=0;
            break;
            
          default:
            objectGoTo(GucXGoal,GucYGoal,__START);//objectGoTo(GucXGoal,GucYGoal,__END);                                            /*  ���е�ָ��Ŀ���            */
            GmcMouse.cX = GucXGoal;
            GmcMouse.cY = GucYGoal;
            mouseTurnback();
        //  mouseTurnback();
        //  mouseSpurt();
            objectGoTo(temp_x,temp_y,__BACK);                          /*  �ص����                    */
            GmcMouse.cX = temp_x;
            GmcMouse.cY = temp_y;
            GucMouseTask = MAZESEARCH;
            maze_time=0;
            break;               
          }
              
//          objectGoTo(GucXGoal,GucYGoal,__END); //GucXGoal,GucYGoal,__END                           /*  ������·�������յ�          */
//          GmcMouse.cX = GucXGoal;
//          GmcMouse.cY = GucYGoal;
//          mouseTurnback();  

                  
//          delay(500000);
//                
//          objectGoTo(GucXStart,GucYStart,__START);                          /*  �ص����                    */
//          GmcMouse.cX = GucXStart;
//          GmcMouse.cY = GucYStart;
//          delay(1000000);
//          mouseTurnback();                                            /*  ���ת���ָ���������        */
//          GucMouseDir = (GucMouseDir + 2) % 4; 
//          GucDirFlag = 1;
/*                while (1) 
                {
                    if (keyCheck() == true) 
                    {
                        break;
                    }
                    sensorDebug();
                    delay(20000);
                }
                break;
 */
        default:
          break;
        }
    }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
