/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           Maze.h
** Last modified Date: 
** Last Version: 
** Description:         �����󶥲���Ƴ���ͷ�ļ�
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


#ifndef __Maze_h
#define __Maze_h


/*********************************************************************************************************
  ����ͷ�ļ�
*********************************************************************************************************/
#include "Zlg7289.h"
#include "Type.h"
#include "Micromouse.h"
#include "Mouse_Config.h"


/*********************************************************************************************************
  �����궨�� -- ��������������״̬
*********************************************************************************************************/
#define  WAIT           0                                               /*  �ȴ�״̬                    */
#define  START          1                                               /*  ����״̬                    */
#define  MAZESEARCH     2                                               /*  ��Ѱ״̬                    */
#define  SPURT          3                                               /*  ���״̬                    */


#define  STEPNUM        255                                             //���߲�������
#define  STRWEIGHT      4
#define  CURVEWEIGHT    7

/*********************************************************************************************************
  ������Ҫʹ�õ��ⲿ����
*********************************************************************************************************/
extern void  mouseInit(void);                                           /*  �ײ����������ʼ��          */
extern void  mazeSearch(void);                                          /*  �Թ�����                    */
extern void  mouseTurnleft(void);                                       /*  ����ת90��                  */
extern void  mouseTurnright(void);                                      /*  ����ת90��                  */
extern void  mouseTurnback(void);                                       /*  ���ת                      */
extern uint8 keyCheck(void);                                            /*  ��ⰴ��                    */
extern void  sensorDebug(void);                                         /*  ����������                  */
extern void  voltageDetect(void);/*  ��ѹ���                    */
extern void  objectGoTo(int8  cXdst, int8  cYdst, uint8 flag);
/*********************************************************************************************************
  ������Ҫʹ�õ��ⲿ����
*********************************************************************************************************/
extern MAZECOOR GmcMouse;                                               /*  GmcMouse.x :�����������    */
                                                                        /*  GmcMouse.y :������������    */
                                                                        
extern uint8    GucMouseDir;                                            /*  �������ǰ������            */
extern uint8    GucMapBlock[MAZETYPE][MAZETYPE];                        /*  GucMapBlock[x][y]           */
                                                                        /*  x,������;y,������;          */
                                                                        /*  bit3~bit0�ֱ������������   */
                                                                        /*  0:�÷�����·��1:�÷�����·  */

       void  mapStepEdit(int8  cX, int8  cY);
static uint8 mazeBlockDataGet(uint8  ucDirTemp);
static void  rightMethod(void);
static void  leftMethod(void);
static void  frontRightMethod(void);
static void  frontLeftMethod(void);
static void  centralMethod(void);
static void  spurTrackChoice(void);


#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
