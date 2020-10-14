#include "ActLib.h"
#include "App.h"
#include "ElectricalTree.h"
#include <stdlib.h>

ActLib* l;

XPath gXPath;
YPath gYPath;
int XResetSpeed = 6;
int YResetSpeed = 1500;

/***************************************************************************************************************
  * 函数名称： PlanXPath
  * 函数描述： 转动路径规划
  * 其它说明
****************************************************************************************************************/
void PlanXPath()
{
	gXPath.nDir = 1;
	if(gXPath.nTargetX>gXPath.CurrentX)
	{
		gXPath.nDir = 0;
	}
	
	gXPath.nDistance = abs(gXPath.CurrentX-gXPath.nTargetX);
	gXPath.SensorCnt = 0;
	
	if(gXPath.nDistance>12)
	{
		if(gXPath.nDir)
		{
			gXPath.nDir = 0;
		}
		else
		{
			gXPath.nDir = 1;
		}
		gXPath.nDistance = 24-gXPath.nDistance;
	}
}
/***************************************************************************************************************
  * 函数名称： ResetDevice
  * 函数描述： 设备整机复位
  * 其它说明		 动作编号 1
****************************************************************************************************************/
STEP_START(ResetDevice):
	l->StartAction(ACTION_RESET_X);
	l->StartAction(ACTION_RESET_SENDCARD);
	l->StartAction(ACTION_RESET_SENDBOOK);
	l->StartAction(ACTION_RESET_CIRCLE);
	l->StartAction(ACTION_RESET_SLOT);
	l->JustWaitAction(ACTION_RESET_SENDCARD,STEP2,STEP_ERROR);
lSTEP2:		
	l->JustWaitAction(ACTION_RESET_SENDBOOK,STEP3,STEP_ERROR);
lSTEP3:
	l->WaitAction(ACTION_RESET_Y,STEP4,STEP_ERROR);
lSTEP4:
	l->JustWaitAction(ACTION_RESET_X,STEP5,STEP_ERROR);
lSTEP5:
	l->JustWaitAction(ACTION_RESET_CIRCLE,STEP6,STEP_ERROR);
lSTEP6:
	l->JustWaitAction(ACTION_RESET_SLOT,STEP7,STEP_ERROR);
lSTEP7:
	l->SetActionOk();
	l->StartAction(ACTION_CLOSE_DOOR);
lSTEP_ERROR:
	l->SetSubError();
STEP_END
/***************************************************************************************************************
  * 函数名称： ResetX
  * 函数描述： X轴 转盘的复位
  * 其它说明  动作编号 2
****************************************************************************************************************/
STEP_START(ResetX):
		if(l->IsSensorOn(SN_X_ORG))
		{
			l->SetSMSpeed(SM_X,XResetSpeed);
			l->LeaveSensor(SM_X,GetXAngle(10),SN_X_ORG,STEP2,STEP_ERROR1);
		}
		else
		{
			l->FindSensor(SM_X,GetXAngle(400),SN_X_ORG,STEP4,STEP_ERROR);
		}
lSTEP2:
		l->WaitSMStop(SM_X,STEP8);
lSTEP3:
		l->SetSMSpeed(SM_X,XResetSpeed);
		l->FindSensor(SM_X,-GetXAngle(20),SN_X_POS,STEP7,STEP_ERROR);
lSTEP4:
		l->WaitSensorOff(SN_X_ORG,STEP5);
		l->WaitSMStop(SM_X,STEP_ERROR);
lSTEP5:	
		l->BrakeSM(SM_X);
		l->WaitSMStop(SM_X,STEP6);
lSTEP6:
		l->WaitTimeOut(500,STEP3);
lSTEP7:
		l->StopSM(SM_X);
		l->WaitTimeOut(300,STEP_OK);
lSTEP8:
		l->WaitTimeOut(500,STEP3);
lSTEP_OK:
		gXPath.Status = STATUS_1;
		gXPath.CurrentX = 1;
		l->SetActionOk();
lSTEP_ERROR1:
		gXPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("转筒挡片离开原点传感器失败!");
lSTEP_ERROR:
	gXPath.Status = STATUS_UNKNOWN;
	l->SetErrorInfo("转筒挡片找原点传感器失败!");
STEP_END
/***************************************************************************************************************
  * 函数名称： GoToX
  * 函数描述： X轴 转盘的定位
  * 其它说明   动作编号 3
****************************************************************************************************************/
STEP_START(GoToX):
		if(l->GetByteParam(0)>24)
		{
			l->SetErrorInfo("转筒定位参数超出范围!");
			break;
		}
		
		gXPath.nTargetX = l->GetByteParam(0);
		if(gXPath.Status==STATUS_UNKNOWN)
		{
			l->WaitAction(ACTION_RESET_X,STEP2,STEP_ERROR1);
			break;
		}
		
		if(!l->IsSensorOn(SN_X_POS))
		{
			l->WaitAction(ACTION_RESET_X,STEP2,STEP_ERROR1);
			break;
		}
		
		l->NextTo(STEP2);
lSTEP2:
		PlanXPath();						//计算转筒的路径:方向，挡片数量
		if(gXPath.nDistance==0)	//和当前的位置相同就不动
		{
			l->SetActionOk();
			break;
		}
		
		if(gXPath.nDir==0)
		{
			l->LeaveSensor(SM_X,(gXPath.nDistance*GetXAngle(15)+GetXAngle(10)),SN_X_POS,STEP3,STEP5);
		}
		else
		{
			l->LeaveSensor(SM_X,-(gXPath.nDistance*GetXAngle(15)-GetXAngle(8)),SN_X_POS,STEP9,STEP11);
		}
lSTEP3:
		l->WaitSensorOn(SN_X_POS,STEP4);
		l->WaitSMStop(SM_X,STEP5);
lSTEP4:
		l->WaitSensorOff(SN_X_POS,STEP3);
		gXPath.SensorCnt++;
		l->WaitSMStop(SM_X,STEP5);
lSTEP5:
		l->WaitTimeOut(500,STEP7);
lSTEP6:
		l->StopSM(SM_X);
		l->NextTo(STEP7);
lSTEP7:
		l->SetSMSpeed(SM_X,XResetSpeed);
		l->FindSensor(SM_X,-GetXAngle(18),SN_X_POS,STEP8,STEP_ERROR);
lSTEP8:
		l->StopSM(SM_X);
		gXPath.CurrentX = gXPath.nTargetX;
		l->SetActionOk();
lSTEP9:
		l->WaitSensorOn(SN_X_POS,STEP10);
		l->WaitSMStop(SM_X,STEP11);
lSTEP10:
		l->WaitSensorOff(SN_X_POS,STEP9);
		gXPath.SensorCnt++;
		l->WaitSMStop(SM_X,STEP11);
lSTEP11:
		if(l->IsSensorOn(SN_X_POS))
		{
				l->WaitTimeOut(500,STEP13);
		}
		else 
		{
				l->WaitTimeOut(500,STEP7);
		}
lSTEP13:
		l->RunSM(SM_X,GetXAngle(5));
		l->WaitSMStop(SM_X,STEP14);
lSTEP14:
		l->WaitTimeOut(500,STEP7);
lSTEP_ERROR1:
		l->SetSubError();
lSTEP_ERROR:
		gXPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("转筒定位挡片找传感器出错!");
STEP_END
long BlockStart1,BlockStart2;
STEP_START(RecordYAllSensorPos):
		l->WaitSensorOn(SN_Y_SENDBOOK,STEP2);
		l->WaitSMStop(SM_Y,STEP_ERROR);
lSTEP2:
		l->WaitSensorOff(SN_Y_SENDBOOK,STEP3);
		l->WaitSMStop(SM_Y,STEP_ERROR);
lSTEP3:
		gYPath.nSendBookPos = l->GetAbsPos(SM_Y);
		l->WaitSensorOn(SN_Y_SENDCARD,STEP4);
		l->WaitSMStop(SM_Y,STEP_ERROR1);
lSTEP4:
			l->WaitSensorOff(SN_Y_SENDCARD,STEP5);
			BlockStart1 = l->GetAbsPos(SM_Y);
			l->WaitSMStop(SM_Y,STEP_ERROR1);
lSTEP5:
			gYPath.nSendCardPos = l->GetAbsPos(SM_Y);
			l->WaitSensorOn(SN_Y_GETCARD,STEP6);
			l->WaitSMStop(SM_Y,STEP_ERROR2);
lSTEP6:
			BlockStart2 = l->GetAbsPos(SM_Y);
			l->WaitSensorOff(SN_Y_GETCARD,STEP7);
			l->WaitSMStop(SM_Y,STEP_ERROR2);
lSTEP7:
		gYPath.nCardOutPos = l->GetAbsPos(SM_Y);
		BlockStart1 = abs(BlockStart1-gYPath.nSendCardPos);
		BlockStart2 = abs(BlockStart2-gYPath.nCardOutPos);
		if(abs(BlockStart1-BlockStart2)>100)
		{
			l->SetErrorInfo("升降定位挡片存在误检");
		}
		else
		{
			gYPath.nBookOutPos = gYPath.nCardOutPos-GetYLen(61);
			gYPath.nBatchOutPos = gYPath.nBookOutPos-GetYLen(113);
			l->SetActionOk();
		}
lSTEP_ERROR:
		
		l->SetErrorInfo("找发本传感器出错");
lSTEP_ERROR1:
		l->SetErrorInfo("找发卡传感器出错");
lSTEP_ERROR2:
		l->SetErrorInfo("找出卡传感器出错");
STEP_END
void IsYSafety()
{
		if(l->IsSensorOff(SN_SENDCARD_ORG))
		{
			l->SetErrorInfo("发卡原点没有感应");
			return ;
		}
		
		if(l->IsSensorOff(SN_SENDBOOK_ORG))
		{
			l->SetErrorInfo("发卡原点没有感应");
			return ;
		}
}
/***************************************************************************************************************
  * 函数名称： ResetY
  * 函数描述： Y轴 升降复位
  * 其它说明   动作编号 4
****************************************************************************************************************/
char nToothIndex;
int SpeedOffset;
STEP_START(ResetY1): 
		IsYSafety();
		if(gYPath.Status==STATUS_UNKNOWN)
		{	
			l->SetSMSpeed(SM_Y,YResetSpeed);
			l->FindSensor(SM_Y,-GetYLen(1100),SN_SEVRO_ERROR,STEP_ERROR1,STEP_ERROR);
		}
		else
		{
			l->RunABS(SM_Y,-150);							  //向下多走10mm
			l->WaitSMStop(SM_Y,STEP2);
		}
		gYPath.Status=STATUS_UNKNOWN;
		l->WaitSensorOn(SN_Y_SENDBOOK,STEP2);	
		l->WaitSensorOn(SN_Y_DOWNLIMT,STEP2);
lSTEP2:
		l->WaitRunLength(SM_Y,30,STEP8);
		l->WaitSMStop(SM_Y,STEP8);
lSTEP3:
		l->StartAction(ACTION_RECORD_YSENSOR);
		gYPath.Status = STATUS_1;
		gYPath.NoCheckCnt=0;
		l->SetAbsOrg(SM_Y);	
		SpeedOffset = l->GetSMSpeed(SM_Y)/1000;
		nToothIndex = 0;	
		l->RunSM(SM_Y,GetYLen(1100));	//向上找传感器
		l->WaitRunLength(SM_Y,3000,STEP4);						//跳过第一格锯齿
lSTEP4:
		l->WaitSensorOff(SN_Y_UNIT,STEP5);
		
		l->WaitSMStop(SM_Y,STEP_ERROR);
lSTEP5:
		gYPath.YToothTopPos[nToothIndex] = l->GetAbsPos(SM_Y);		//转换到速度为0的位置
		nToothIndex++;
		l->WaitSensorOn(SN_Y_UNIT,STEP6);
lSTEP6:
		gYPath.YToothBottomPos[nToothIndex] = l->GetAbsPos(SM_Y);
		if(nToothIndex>=6)
		{
			l->WaitSensorOn(SN_Y_GETCARD,STEP9);
			l->WaitSensorOn(SN_Y_UPLIMT,STEP_ERROR);
			l->WaitSMStop(SM_Y,STEP_ERROR);
										
		}
		else
		{
			l->WaitSensorOff(SN_Y_UNIT,STEP5);
		}
lSTEP7:
		if(l->IsActionOk(ACTION_RECORD_YSENSOR)!=0xff)
		{
			l->SetErrorInfo("升降记录传感器出错");
		}
		else
		{			
			gYPath.RealOffset = 0;
			
			for(char i=1;i<7;i++)
			{
				gYPath.ToothLen[i] = gYPath.YToothTopPos[i]-gYPath.YToothBottomPos[i];
				gYPath.YToothBottomPos[i] -= SpeedOffset;
				gYPath.YToothTopPos[i] -= SpeedOffset;
			}
			gYPath.ToothLen[0] = gYPath.ToothLen[1];
			gYPath.YToothBottomPos[0] = gYPath.YToothTopPos[0]-gYPath.ToothLen[0];
			l->SetActionOk();
		}
lSTEP8:
		l->StopSM(SM_Y);
		l->WaitTimeOut(400,STEP3);
lSTEP9:
		l->BrakeSM(SM_Y);
		l->WaitSMStop(SM_Y,STEP7);
lSTEP_ERROR1:
		gYPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("伺服出错!");
lSTEP_ERROR:
		gYPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("升降复位出错，请检查电机 传感器!");
STEP_END
/***************************************************************************************************************
  * 函数名称： CreateYCordinate
  * 函数描述： 建立升降Y轴的坐标系
  * 其它说明：卡和本公用一个基准面(以卡为基准)，做为Y轴坐标原点,本比卡高4mm 每本之间的距离14mm组之间的距离13mm 每组卡9个 本8个,
****************************************************************************************************************/
signed long GetYPos(char PosType,char Pos)
{
	char nGroupNum,nCellNum;
	Pos -= 1;
	if(PosType==0)
	{
		nGroupNum = Pos/9;
		nCellNum = Pos%9;
		return gYPath.YToothBottomPos[nGroupNum]+GetYLen(14)*(nCellNum);
	}
	else if(PosType==1)	//取本的位置
	{
		nGroupNum = Pos/8;
		nCellNum = Pos%8;
		return gYPath.YToothBottomPos[nGroupNum]+GetYLen(14)*(nCellNum)+GetYLen(4.5);
	}
	else								//存本的位置
	{
			nGroupNum = Pos/8;
		  nCellNum = Pos%8;
			return gYPath.YToothBottomPos[nGroupNum]+GetYLen(14)*(nCellNum)+GetYLen(2.5);
	}
}

void UpdateAllPos(signed long offset)
{
	for(char i=0;i<7;i++)
	{
		gYPath.YToothBottomPos[i] += offset;
		gYPath.YToothTopPos[i] += offset;
	}
}

STEP_START(CheckRackOffset):
gYPath.ToothSensorCnt = 0;
l->WaitSensorOff(SN_Y_UNIT,STEP2);
l->WaitSMStop(SM_Y,STEP_OK);
lSTEP2:
	l->WaitSensorOn(SN_Y_UNIT,STEP3);
	l->WaitSMStop(SM_Y,STEP_OK);
lSTEP3:
	gYPath.ToothSensorCnt++;
	gYPath.ToothSpeed = l->GetSMSpeed(SM_Y);
	gYPath.LastToothPos = l->GetAbsPos(SM_Y);
	l->WaitSensorOff(SN_Y_UNIT,STEP2);
	l->WaitSMStop(SM_Y,STEP_OK);
lSTEP_OK:
if(gYPath.ToothSensorCnt == 0)
{
	l->SetActionOk();
	break;
}
if(l->GetDir(SM_Y)==1)	//方向向上,找底部坐标
{
	for(int i=0;i<7;i++)
	{
		if(abs(gYPath.YToothBottomPos[i]-gYPath.LastToothPos)<300)
		{
			UpdateAllPos(gYPath.LastToothPos-gYPath.ToothSpeed/1000-gYPath.YToothBottomPos[i]);
			//gYPath.YToothBottomPos[i] = gYPath.LastToothPos-gYPath.ToothSpeed/1000; //假设速度3000，那么就需要0.12mm的偏差，即感应坐标减去3作为基准，然后做偏移计算
		}
	}
}
else
{
	for(int i=0;i<7;i++)
	{
		if(abs(gYPath.YToothTopPos[i]-gYPath.LastToothPos)<300)
		{
			gYPath.LastToothPos += GetYLen(0.9);							//补偿光斑宽度偏差
			gYPath.LastToothPos += 2*gYPath.ToothSpeed/1000;	//补偿由于速度不一致造成的偏差
			gYPath.YToothTopPos[i] = gYPath.LastToothPos;
			UpdateAllPos(gYPath.LastToothPos-gYPath.YToothTopPos[i]);
			break;
		}
	}
}
l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * 函数名称： GoToBookY
  * 函数描述： 本单元定位-单元格的间距14mm 组之间的单元间距13mm,一组8个存储单元,本比卡高9mm
  * 其它说明   动作编号 6
****************************************************************************************************************/
char ActionType;
STEP_START(GoToY):
		IsYSafety();
		
		if(l->IsActionRun(ACTION_GOTO_CARDY))
		{
			if(l->GetByteParam(0)>63)
			{
				l->SetErrorInfo("升降定位参数超出范围!");
				break;
			}
			ActionType = 0;
			gYPath.nTargetPos = GetYPos(0,l->GetByteParam(0));
		}
		else if(l->IsActionRun(ACTION_YTO_SAVEBOOKY))
		{
			if(l->GetByteParam(0)>56)
			{
				l->SetErrorInfo("升降定位参数超出范围!");
				break;
			}
			ActionType = 1;
			gYPath.nTargetPos = GetYPos(1,l->GetByteParam(0));
		}
		else
		{
			if(l->GetByteParam(0)>56)
			{
				l->SetErrorInfo("升降定位参数超出范围!");
				break;
			}
			ActionType = 2; 
			gYPath.nTargetPos = GetYPos(2,l->GetByteParam(0));
		}
		if(gYPath.Status != STATUS_1)
		{
			l->WaitAction(ACTION_RESET_Y,STEP2,STEP_ERROR);
			break;
		}
		
		if(gYPath.NoCheckCnt>45)
		{
			l->WaitAction(ACTION_RESET_Y,STEP2,STEP_ERROR);
			break;
		}

		l->NextTo(STEP2);
lSTEP2:
		l->StartAction(ACTION_CHECK_RACKPOS);
		l->RunABS(SM_Y,gYPath.nTargetPos);
		l->WaitSensorOn(SN_Y_UPLIMT,STEP4);
		l->WaitSensorOn(SN_SEVRO_ERROR,STEP5);
		l->WaitSMStop(SM_Y,STEP3);
lSTEP3:
		l->WaitTimeOut(100,STEP6);
lSTEP4:
		l->StopSM(SM_Y);
		gYPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("升降撞上限位!");
lSTEP5:
		l->StopSM(SM_Y);
		gYPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("伺服报错!");
lSTEP6:
gYPath.nTargetPos = GetYPos(ActionType,l->GetByteParam(0));
l->RunABS(SM_Y,gYPath.nTargetPos);
l->WaitSensorOn(SN_Y_UPLIMT,STEP4);
l->WaitSensorOn(SN_SEVRO_ERROR,STEP5);
l->WaitSMStop(SM_Y,STEP7);	
lSTEP7:
l->StopSM(SM_Y);
l->SetActionOk();
lSTEP_ERROR:
	gYPath.Status = STATUS_UNKNOWN;
	l->SetSubError();
STEP_END
YSensor gYSensor;
/***************************************************************************************************************
  * 函数名称：YGoToGetCard
  * 函数描述：Y轴升降到出卡的位置
  * 其它说明  动作编号 7
****************************************************************************************************************/
STEP_START(YGoToSensor):
		IsYSafety();
		
		if(gYPath.Status==STATUS_UNKNOWN)
		{
			l->WaitAction(ACTION_RESET_Y,STEP1,STEP_ERROR1);
		}
		else
		{
			if(l->IsActionRun(ACTION_YTO_SENDBOOK))
			{
				gYSensor.SensorNum = SN_Y_SENDBOOK;
				gYSensor.nSensorPos = &gYPath.nSendBookPos;
			}
			
			if(l->IsActionRun(ACTION_YTO_SENDCARD))
			{
				gYSensor.SensorNum = SN_Y_SENDCARD;
				gYSensor.nSensorPos = &gYPath.nSendCardPos;
			}
			
			if(l->IsActionRun(ACTION_YTO_GETCARD))
			{
				gYSensor.SensorNum = SN_Y_GETCARD;
				gYSensor.nSensorPos = &gYPath.nCardOutPos;
			}
			
			l->NextTo(STEP2);
		}
lSTEP2:
		l->RunABS(SM_Y,(*gYSensor.nSensorPos)+GetYLen(3));	//向上多走3mm
		l->WaitSMStop(SM_Y,STEP3);
lSTEP3:
		if(l->IsSensorOn(gYSensor.SensorNum))
		{
			l->NextTo(STEP6);	//需要离开传感器
		}
		else
		{
			l->NextTo(STEP4);	//过了传感器需要向下找传感器
		}
lSTEP4:
		l->FindSensor(SM_Y,-GetYLen(50),gYSensor.SensorNum,STEP5,STEP_ERROR);
lSTEP5:
		l->StopSM(SM_Y);;
		*gYSensor.nSensorPos = l->GetAbsPos(SM_Y);		
		l->SetActionOk();
lSTEP6:
		l->LeaveSensor(SM_Y,GetYLen(20),gYSensor.SensorNum,STEP7,STEP_ERROR2);
lSTEP7:
		l->StopSM(SM_Y);
		l->WaitTimeOut(100,STEP4);
lSTEP_ERROR1:
		gYPath.Status = STATUS_UNKNOWN;
		l->SetSubError();
lSTEP_ERROR2:
		gYPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("升降离开位置传感器出错!");	//离开取卡传感器位置出错
lSTEP_ERROR:
		gYPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("升降向下找位置传感器出错!");
STEP_END
/***************************************************************************************************************
  * 函数名称：YGoToGetBook
  * 函数描述：Y轴升降到出本的位置
  * 其它说明  动作编号 8
****************************************************************************************************************/
STEP_START(YGoToGetBook):
		IsYSafety();
		if(gYPath.Status==STATUS_UNKNOWN)
		{
			l->WaitAction(ACTION_RESET_Y,STEP2,STEP_ERROR1);
		}
		else
		{
			l->RunABS(SM_Y,gYPath.nBookOutPos);
		l->WaitSMStop(SM_Y,STEP2);
		}		
lSTEP2:
		l->SetActionOk();
lSTEP_ERROR1:
		gYPath.Status = STATUS_UNKNOWN;
		l->SetSubError();
STEP_END
/***************************************************************************************************************
  * 函数名称：YGoToBatch
  * 函数描述：Y轴升降到批量口的位置
  * 其它说明： 动作编号 11
****************************************************************************************************************/
STEP_START(YGoToBatch):
		IsYSafety();
		if(gYPath.Status==STATUS_UNKNOWN)
		{
			l->WaitAction(ACTION_RESET_Y,STEP2,STEP_ERROR1);
		}
		else
		{
			l->RunABS(SM_Y,gYPath.nBatchOutPos);
			l->WaitSMStop(SM_Y,STEP2);
		}
lSTEP2:
		l->SetActionOk();
lSTEP_ERROR1:
		gYPath.Status = STATUS_UNKNOWN;
		l->SetSubError();
STEP_END
/***************************************************************************************************************
  * 函数名称：ResetSendCard
  * 函数描述：复位发卡
  * 其它说明
****************************************************************************************************************/
STEP_START(ResetSendCard):
		l->FindSensor(SM_SENDCARD,-GetLeadLen(150),SN_SENDCARD_ORG,STEP2,STEP3);
lSTEP2:
		l->StopSM(SM_SENDCARD);
	  l->SetActionOk();
lSTEP3:
		l->FindSensor(SM_SENDCARD,-GetLeadLen(150),SN_SENDCARD_ORG,STEP2,STEP_ERROR);
lSTEP_ERROR:
			l->SetErrorInfo("卡箱复位找传感器失败!");
STEP_END
/***************************************************************************************************************
  * 函数名称：SendCardToLimt
  * 函数描述：发卡到限位
  * 其它说明
****************************************************************************************************************/
STEP_START(SendCardToLimt):
		l->FindSensor(SM_SENDCARD,GetLeadLen(150),SN_SENDCARD_LIMT,STEP2,STEP3);
lSTEP2:
		l->StopSM(SM_SENDCARD);
		l->SetActionOk();
lSTEP3:
		l->FindSensor(SM_SENDCARD,GetLeadLen(150),SN_SENDCARD_LIMT,STEP2,STEP_ERROR);
lSTEP_ERROR:
			l->SetErrorInfo("卡箱发卡找传感器失败!");		
STEP_END
/***************************************************************************************************************
  * 函数名称：SendCard
  * 函数描述：发卡
  * 其它说明
****************************************************************************************************************/
STEP_START(SendCard):
		if(l->IsSensorOn(SN_SENDCARD_CHECK))
		{
			l->WaitAction(ACTION_RESET_SENDCARD,STEP2,STEP_ERROR);
		}
		else
		{
			l->SetActionError(ERROR_CARDBOX_EMPTY);
		}
lSTEP2:
		l->WaitAction(ACTION_SENDCARDTO_LIMT,STEP3,STEP_ERROR);
lSTEP3:
		l->WaitAction(ACTION_RESET_SENDCARD,STEP4,STEP_ERROR);		
lSTEP4:
		l->SetActionError(ACTION_THIN_BOOK);
lSTEP_ERROR:
		l->SetSubError();
STEP_END
/***************************************************************************************************************
  * 函数名称：ResetSendBook
  * 函数描述：发本复位
  * 其它说明
****************************************************************************************************************/
STEP_START(ResetSendBook):
			l->FindSensor(SM_SENDBOOK,-GetLeadLen(150),SN_SENDBOOK_ORG,STEP2,STEP3);
lSTEP2:
		l->StopSM(SM_SENDBOOK);
		l->SetActionOk();
lSTEP3:
	  l->FindSensor(SM_SENDBOOK,-GetLeadLen(150),SN_SENDBOOK_ORG,STEP2,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("复位发本出错!");
STEP_END
/***************************************************************************************************************
  * 函数名称：SendBookToLimt
  * 函数描述：发本到限位
  * 其它说明
****************************************************************************************************************/
STEP_START(SendBookToLimt):
		l->FindSensor(SM_SENDBOOK,GetLeadLen(160),SN_SENDBOOK_LIMT,STEP2,STEP3);
lSTEP2:
	l->StopSM(SM_SENDBOOK);
	l->SetActionOk();
lSTEP3:
	l->FindSensor(SM_SENDBOOK,GetLeadLen(160),SN_SENDBOOK_LIMT,STEP2,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("本箱发本找传感器失败!");
STEP_END
/***************************************************************************************************************
  * 函数名称：SendBook
  * 函数描述：发本
  * 其它说明
****************************************************************************************************************/
char IsFatBook;
STEP_START(SendBook):
		if(l->IsSensorOn(SN_SENDBOOK_PRESS))
		{
			l->SetErrorInfo("压块没有压下!");
			break;
		}
//		
//		if(l->IsSensorOn(SN_BOOK_DOOR))
//		{
//			l->SetErrorInfo("本箱门没有关!");
//			break;
//		}
				
		if(l->IsSensorOn(SN_SENDBOOK_CHECK))
		{
			l->WaitAction(ACTION_RESET_SENDBOOK,STEP2,STEP_ERROR);
		}
		else
		{
			l->SetActionError(ERROR_BOOKBOX_EMPTY);//本箱空
		}
lSTEP2:
		l->WaitAction(ACTION_SENDBOOKTO_LIMT,STEP3,STEP_ERROR);
		l->WaitSensorOn(SN_SENDBOOK_FATBOOK,STEP4);
		IsFatBook = 0;
lSTEP3:
		if(IsFatBook)
		{		
			l->WaitAction(ACTION_RESET_SENDBOOK,STEP5,STEP_ERROR);
		}
		else
		{
			l->WaitAction(ACTION_RESET_SENDBOOK,STEP6,STEP_ERROR);
		}
lSTEP4:
		IsFatBook = 1;
		l->JustWaitAction(ACTION_SENDBOOKTO_LIMT,STEP3,STEP_ERROR);
lSTEP5:
		l->SetActionError(ACTION_FAT_BOOK);	//厚本
lSTEP6:
		l->SetActionError(ACTION_THIN_BOOK); //薄本
lSTEP_ERROR:
		l->SetErrorInfo("发本复位失败!");
STEP_END
/***************************************************************************************************************
  * 函数名称：ReadyGetCardFromCar
  * 函数描述：从小车接卡
  * 其它说明
****************************************************************************************************************/
STEP_START(ReadyGetCardFromCar):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetErrorInfo("出口有证件!");
		}
		else
		{
			l->FindSensor(SM_PUSH_CARD,-48000,SN_OUT_CHECK,STEP2,STEP_ERROR);
		}
lSTEP2:
		l->WaitTimeOut(4000,STEP_ERROR);
		l->WaitSensorOff(SN_OUT_CHECK,STEP3);
lSTEP3:
		l->SetActionOk();
		l->DisableSM(SM_PUSH_CARD);
lSTEP_ERROR1:
		l->StopSM(SM_PUSH_CARD);
		l->SetActionOk();
lSTEP_ERROR:
		l->StopSM(SM_PUSH_CARD);
		l->SetErrorInfo("接卡超时!");
STEP_END
/***************************************************************************************************************
  * 函数名称：ReadyGetBookFromCar
  * 函数描述：从小车接本
  * 其它说明
****************************************************************************************************************/
STEP_START(ReadyGetBookFromCar):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetErrorInfo("出口有证件!");
		}
		else
		{
			l->FindSensor(SM_PUSH_BOOK,-48000,SN_OUT_CHECK,STEP2,STEP_ERROR);
		}
lSTEP2:
		l->WaitTimeOut(4000,STEP_ERROR2);
		l->WaitSensorOff(SN_OUT_CHECK,STEP3);
lSTEP3:
		l->StopSM(SM_PUSH_BOOK);
		l->SetActionOk();
lSTEP_ERROR2:
		l->StopSM(SM_PUSH_BOOK);
		l->SetErrorInfo("出本离开传感器出错!");
lSTEP_ERROR1:
		l->StopSM(SM_PUSH_BOOK);
		l->SetActionOk();
lSTEP_ERROR:
		l->StopSM(SM_PUSH_BOOK);
		l->SetErrorInfo("接本超时!!");
STEP_END
/***************************************************************************************************************
  * 函数名称：PushOutCard
  * 函数描述：吐卡
  * 其它说明
****************************************************************************************************************/
STEP_START(PushOutCard):
		l->RunSM(SM_PUSH_CARD,-30000);
		l->WaitTimeOut(1000,STEP2);
lSTEP2:
			l->StopSM(SM_PUSH_CARD);
			if(l->IsSensorOn(SN_OUT_CHECK))
			{
				l->SetErrorInfo("吐卡出错：出口有证件!");
			}
			else
			{
				l->SetActionOk();
			}
STEP_END
/***************************************************************************************************************
  * 函数名称：PushOutBook
  * 函数描述：吐本
  * 其它说明
****************************************************************************************************************/
STEP_START(PushOutBook):
		l->RunSM(SM_PUSH_BOOK,-30000);
		l->WaitTimeOut(1000,STEP2);
lSTEP2:
		l->StopSM(SM_PUSH_BOOK);
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetErrorInfo("出口有证件!");
		}
		else
		{
			l->SetActionOk();
		}
STEP_END
/***************************************************************************************************************
  * 函数名称：BatchOutCardBook
  * 函数描述：批量出卡 出本
  * 其它说明
****************************************************************************************************************/
STEP_START(BatchOutCardBook):
		l->RunSM(SM_PUSH_BOOK,-30000);
		l->WaitTimeOut(6000,STEP_OK);
lSTEP_OK:
		l->StopSM(SM_PUSH_BOOK);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * 函数名称：RecyleCardToCar
  * 函数描述：回收卡到小车
  * 其它说明
****************************************************************************************************************/
STEP_START(RecyleCardToCar):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->LeaveSensor(SM_PUSH_CARD,48000,SN_OUT_CHECK,STEP2,STEP_ERROR);
		}
		else
		{
			l->FindSensor(SM_PUSH_CARD,48000,SN_OUT_CHECK,STEP4,STEP5);
		}
lSTEP2:
		l->WaitTimeOut(1000,STEP3);
lSTEP3:
		l->DisableSM(SM_PUSH_CARD);
		l->SetActionOk();
lSTEP4:
		l->WaitSensorOff(SN_OUT_CHECK,STEP2);
		l->WaitTimeOut(5000,STEP_ERROR);
lSTEP5:
		l->StopSM(SM_PUSH_CARD);
		l->SetErrorInfo("没有证件，单本存证口关闭!!");
		l->StartAction(ACTION_CLOSE_DOOR);
lSTEP_ERROR:
		l->StopSM(SM_PUSH_CARD);
		l->SetErrorInfo("卡回收离开传感器出错!");
STEP_END
/***************************************************************************************************************
  * 函数名称：RecyleBookToCar
  * 函数描述：回收本到小车
  * 其它说明
****************************************************************************************************************/
STEP_START(RecyleBookToCar):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->LeaveSensor(SM_PUSH_BOOK,58000,SN_OUT_CHECK,STEP2,STEP_ERROR);
		}
		else
		{
			l->FindSensor(SM_PUSH_BOOK,58000,SN_OUT_CHECK,STEP4,STEP5);
		}
lSTEP2:
		l->WaitTimeOut(1000,STEP3);
lSTEP3:
		l->DisableSM(SM_PUSH_BOOK);
		l->SetActionOk();
lSTEP4:
		l->WaitSensorOff(SN_OUT_CHECK,STEP2);
		l->WaitTimeOut(5000,STEP_ERROR);
lSTEP5:
		l->StopSM(SM_PUSH_BOOK);
		l->SetErrorInfo("没有证件，单本存证口关闭!");
		l->StartAction(ACTION_CLOSE_DOOR);
lSTEP_ERROR:
		l->StopSM(SM_PUSH_BOOK);
		l->SetErrorInfo("本回收离开传感器出错!");
STEP_END
/***************************************************************************************************************
  * 函数名称：CloseDoor
  * 函数描述：关挡板门
  * 其它说明
****************************************************************************************************************/
STEP_START(CloseDoor):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetErrorInfo("出口有证!");
			break;
		}
		
		l->FindSensor(SM_DOOR,4800,SN_DOOR_CLOSE,STEP_OK,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("找关门传感器出错!");
lSTEP_OK:
		l->StopSM(SM_DOOR);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * 函数名称：OpenBookDoor
  * 函数描述：打开本挡板门
  * 其它说明
****************************************************************************************************************/
STEP_START(OpenBookDoor):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetActionOk();
			break;
		}

		l->FindSensor(SM_DOOR,-4800,SN_DOOR_BOOK_OPEN,STEP_OK,STEP2);
lSTEP2:
		l->FindSensor(SM_DOOR,-4800,SN_DOOR_BOOK_OPEN,STEP_OK,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("找打开本挡板门传感器出错!");
lSTEP_OK:
		l->StopSM(SM_DOOR);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * 函数名称：OpenDoor
  * 函数描述：卡挡板开门
  * 其它说明
****************************************************************************************************************/
STEP_START(OpenCardDoor):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetActionOk();
			break;
		}
		
		if(l->IsSensorOn(SN_DOOR_CLOSE))
		{
			l->FindSensor(SM_DOOR,-4800,SN_DOOR_CARD_OPEN,STEP_OK,STEP2);
		}
		else 
		{
			l->FindSensor(SM_DOOR,5800,SN_DOOR_CARD_OPEN,STEP_OK,STEP2);
			l->WaitSensorOn(SN_DOOR_CLOSE,STEP3);

		}
lSTEP2:
		l->FindSensor(SM_DOOR,-4800,SN_DOOR_CARD_OPEN,STEP_OK,STEP_ERROR);
lSTEP3:
		l->StopSM(SM_DOOR);
		l->WaitTimeOut(100,STEP1);
lSTEP_ERROR:
		l->SetErrorInfo("找打开卡挡板门传感器出错!");
lSTEP_OK:
		l->StopSM(SM_DOOR);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * 函数名称：ResetCircle
  * 函数描述：复位转盘
  * 其它说明
****************************************************************************************************************/
STEP_START(ResetCircle):
		l->FindSensor(SM_CIRCLE,GetCircleAngle(105),SN_CIRCLE_ORG,STEP2,STEP3);
lSTEP2:
		l->DisableSM(SM_CIRCLE);
		l->WaitTimeOut(200,STEP_OK);
lSTEP3:
		l->FindSensor(SM_CIRCLE,GetCircleAngle(18),SN_CIRCLE_ORG,STEP2,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("复位旋转小车出错!");
lSTEP_OK:
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * 函数名称：CircleFromOrgToCheck
  * 函数描述：小车转盘从原点到光纤检测位
  * 其它说明
****************************************************************************************************************/
STEP_START(CircleFromOrgToCheck):
		l->WaitTimeOut(100,STEP2);
lSTEP2:		
		if(l->IsSensorOn(SN_CIRCLE_OUT))
		{
			l->FindSensor(SM_CIRCLE,GetCircleAngle(55),SN_CIRCLE_CHECK,STEP_OK,STEP_ERROR);
		}
		else
		{
			l->FindSensor(SM_CIRCLE,-GetCircleAngle(55),SN_CIRCLE_CHECK,STEP_OK,STEP_ERROR);
			l->WaitSensorOn(SN_CIRCLE_OUT,STEP3);
		}
lSTEP3:
		l->StopSM(SM_CIRCLE);
		l->FindSensor(SM_CIRCLE,GetCircleAngle(55),SN_CIRCLE_CHECK,STEP_OK,STEP_ERROR);
lSTEP4:
		l->FindSensor(SM_CIRCLE,-GetCircleAngle(15),SN_CIRCLE_CHECK,STEP_OK,STEP_ERROR);
		l->WaitSensorOn(SN_CIRCLE_OUT,STEP3);
lSTEP_ERROR:
		l->SetErrorInfo("旋转小车定位检测位出错!");
lSTEP_OK:
		l->StopSM(SM_CIRCLE);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * 函数名称：CircleFromOrgToOut
  * 函数描述：小车转盘从原点到出口位置
  * 其它说明
****************************************************************************************************************/
STEP_START(CircleFromOrgToOut):
		l->FindSensor(SM_CIRCLE,-GetCircleAngle(100),SN_CIRCLE_OUT,STEP2,STEP3);
lSTEP2:
		l->DisableSM(SM_CIRCLE);
		l->WaitTimeOut(200,STEP_OK);
lSTEP3:
		l->FindSensor(SM_CIRCLE,-GetCircleAngle(15),SN_CIRCLE_OUT,STEP2,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("旋转小车定位出口出错!");
lSTEP_OK:
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * 函数名称：DetectServoError 
  * 函数描述：伺服出错检测(报警，上下限位检测)
  * 其它说明
****************************************************************************************************************/
STEP_START(DetectServoError):
		l->WaitTimeOut(3000,STEP2);
lSTEP2:
		l->WriteIO(DM_SEVRO_BRAKE,DM_CW);	//释放
		l->WaitSensorOn(SN_SEVRO_ERROR,STEP3);
lSTEP3:
			l->WriteIO(DM_SEVRO_BRAKE,DM_STOP);	//刹车锁住电机
			l->WaitSensorOff(SN_SEVRO_ERROR,STEP2);
STEP_END
/***************************************************************************************************************
  * 函数名称：DetectServoError 
  * 函数描述：实时检测伺服偏差
  * 其它说明
****************************************************************************************************************/
STEP_START(DetectServeroOffset):
		if(l->IsSensorOn(SN_Y_UNIT))
		{
			l->WaitSensorOff(SN_Y_UNIT,STEP3);	//运动到单元格的顶部
		}
		else
		{
			l->WaitSensorOn(SN_Y_UNIT,STEP2);	//运动到单元格的底部
		}
							
		l->WaitSMStop(SM_Y,STEP4);
lSTEP2:
		l->WaitSensorOff(SN_Y_UNIT,STEP3);	//运动到单元格的顶部
		l->WaitSMStop(SM_Y,STEP4);
lSTEP3:
		l->WaitSensorOn(SN_Y_UNIT,STEP2);	//运动到单元格的底部
		l->WaitSMStop(SM_Y,STEP4);
lSTEP4:
		l->SetActionOk();
lSTEP_ERROR:
		l->SetErrorInfo("齿条偏差检测过大!");
STEP_END
signed long testPos;
char OptialLen;
/***************************************************************************************************************
  * 函数名称：DetectServoError 
  * 函数描述：实时检测伺服偏差
		* 其它说明 速度3000->7:33 32; 6:34 34;5:34 33;4->35 35 ;3->35 34;2->36 34;速度：5000->3A,40速度越大偏差越大,大概是一毫米的光斑宽度
****************************************************************************************************************/
STEP_START(GetOptionLen):
		l->RunSM(SM_Y,GetYLen(120));
		l->WaitSensorOff(SN_Y_UNIT,STEP2);
lSTEP2:
		testPos = l->GetAbsPos(SM_Y);
		l->WaitSMStop(SM_Y,STEP3);
lSTEP3:
		l->WaitTimeOut(100,STEP4);
lSTEP4:
		l->RunSM(SM_Y,-GetYLen(120));
		l->WaitSensorOff(SN_Y_UNIT,STEP5);
lSTEP5:
		l->WaitSensorOn(SN_Y_UNIT,STEP6);
lSTEP6:
		if(testPos>l->GetAbsPos(SM_Y))
		{
			l->NextTo(STEP7);
		}
		else
		{
			l->NextTo(STEP8);
		}
		OptialLen = abs(testPos-l->GetAbsPos(SM_Y));
		lSTEP7:
		l->SetActionError(OptialLen);
		lSTEP8:
		l->SetActionError(OptialLen);
lSTEP_ERROR:
		l->SetErrorInfo("齿条偏差检测过大!");
STEP_END
/***************************************************************************************************************
  * 函数名称：FlashCardLed 
  * 函数描述：卡式证件的闪烁
  * 其它说明
****************************************************************************************************************/
char LedPort,nLedCnt;
STEP_START(FlashLed):
			nLedCnt = 0;
			if(l->IsActionRun(ACTION_FLASH_CARD_LED))
			{
				LedPort = DM_CARD_LED;
			}
			
			if(l->IsActionRun(ACTION_FLASH_BOOK_LED))
			{
				LedPort = DM_BOOK_LED;
			}
			
			if(l->IsActionRun(ACTION_FLASH_BATCH_LED))
			{
				LedPort = DM_BATCH_LED;
			}
			
			l->WriteIO(LedPort,DM_CW);
			l->WaitTimeOut(800,STEP2);
lSTEP2:
			l->WriteIO(LedPort,DM_STOP);
			if(nLedCnt<15)
			{
				l->WaitTimeOut(800,STEP3);
			}
			else
			{
				l->SetActionOk();
			}
			
			nLedCnt++;
lSTEP3:
			l->WriteIO(LedPort,DM_CW);
			l->WaitTimeOut(800,STEP2);
STEP_END
/***************************************************************************************************************
  * 函数名称：ResetSlot 
  * 函数描述：证件槽的复位
  * 其它说明
****************************************************************************************************************/
STEP_START(ResetSlot):
		if(l->IsSensorOn(SN_OUT_CHECK))	//可能为卡或者本
		{
			l->SetErrorInfo("出口有证!");
		}
		else
		{
			l->FindSensor(SM_PUSH_CARD,-15000,SN_OUT_CHECK,STEP2,STEP4);
		}
lSTEP2:
		l->WaitTimeOut(250,STEP3);
lSTEP3:
		l->DisableSM(SM_PUSH_CARD);
		l->SetErrorInfo("出口有证件");
lSTEP4:	//找本
		l->DisableSM(SM_PUSH_CARD);
		l->FindSensor(SM_PUSH_BOOK,-15000,SN_OUT_CHECK,STEP5,STEP7);
lSTEP5:
		l->WaitTimeOut(300,STEP6);
lSTEP6:
		l->DisableSM(SM_PUSH_BOOK);
		l->SetErrorInfo("出口有证件");
lSTEP7:
		l->SetActionOk();
		l->DisableSM(SM_PUSH_BOOK);	
STEP_END
/***************************************************************************************************************
  * 函数名称：RestartReader
  * 函数描述：读写器复位
  * 其它说明
****************************************************************************************************************/
STEP_START(RestartReader):
		l->WriteIO(5,DM_CW);
		l->WaitTimeOut(5000,STEP2);
lSTEP2:
		l->WriteIO(5,DM_STOP);
		l->WaitTimeOut(4000,STEP3);
lSTEP3:
		l->SetActionOk();
STEP_END

		
ETree* InitApp(ActLib* pLib)
{	
	l = pLib;
	l->AddAction(ACTION_RESET_DEVICE,ResetDevice,"设备复位");
	
	l->AddAction(ACTION_RESET_X,ResetX,"转筒复位");  
	l->AddActionParam(ACTION_GOTO_X,0,TYPE_CHAR,"定位坐标");
	l->AddAction(ACTION_GOTO_X,GoToX,"转筒定位");
	
	l->AddAction(ACTION_RESET_Y,ResetY1,"升降复位");
	l->AddActionParam(ACTION_GOTO_CARDY,0,TYPE_CHAR,"定位坐标");
	l->AddAction(ACTION_GOTO_CARDY,GoToY,"升降卡定位");
	l->AddActionParam(ACTION_GOTO_BOOKY,0,TYPE_CHAR,"定位坐标");
	l->AddAction(ACTION_GOTO_BOOKY,GoToY,"升降本定位");
	l->AddAction(ACTION_YTO_GETCARD,YGoToSensor,"升降定位到出卡口");
	l->AddAction(ACTION_YTO_GETBOOK,YGoToGetBook,"升降定位到出本口");
	l->AddAction(ACTION_YTO_SENDCARD,YGoToSensor,"升降定位发卡位置");
	l->AddAction(ACTION_YTO_SENDBOOK,YGoToSensor,"升降定位发本位置");
	l->AddAction(ACTION_YTO_BATCH,YGoToBatch,"升降定批量箱位置");
	
	
	l->AddAction(ACTION_RESET_SENDCARD,ResetSendCard,"卡箱复位");
	l->AddAction(ACTION_SENDCARDTO_LIMT,SendCardToLimt,"卡箱卡钩到限位");
	l->AddAction(ACTION_SENDCARD,SendCard,"发卡");
	
	l->AddAction(ACTION_RESET_SENDBOOK,ResetSendBook,"复位发本");
	l->AddAction(ACTION_SENDBOOKTO_LIMT,SendBookToLimt,"本箱钩子到限位");
	l->AddAction(ACTION_SENDBOOK,SendBook,"发本");
	
	l->AddAction(ACTION_READY_GETCARD_FROM_CAR,ReadyGetCardFromCar,"准备从小车接卡");
	l->AddAction(ACTION_READY_GETBOOK_FROM_CAR,ReadyGetBookFromCar,"准备从小车接本");
	
	l->AddAction(ACTION_PUSHOUT_CARD,PushOutCard,"吐卡");
	l->AddAction(ACTION_PUSHOUT_BOOK,PushOutBook,"吐本");
	
	l->AddAction(ACTION_BATCH_PUSH_CARDBOOK,BatchOutCardBook,"批量箱出证");
	l->AddAction(ACTION_RECYLDCARD_TO_CAR,RecyleCardToCar,"回收卡到小车");
	l->AddAction(ACTION_RECYLEBOOK_TO_CAR,RecyleBookToCar,"回收本到小车");


	l->AddAction(ACTION_OPEN_CARD_DOOR,OpenCardDoor,"打开卡挡板");
	l->AddAction(ACTION_OPEN_BOOK_DOOR,OpenBookDoor,"打开本挡板");
	l->AddAction(ACTION_CLOSE_DOOR,CloseDoor,"关闭挡板"); 
	
	l->AddAction(ACTION_RESET_CIRCLE,ResetCircle,"小车旋转复位"); 
	l->AddAction(ACTION_CIRCLE_ORG_TO_CHECK,CircleFromOrgToCheck,"小车旋转到检测位"); 
	l->AddAction(ACTION_CIRCLE_ORG_TO_OUT,CircleFromOrgToOut,"小车旋转到出口"); 
	
	l->AddAction(ACTION_CHECK_RACKPOS,CheckRackOffset,"检测齿条偏差"); 
	l->AddAction(ACTION_DETECT_SERVER_ERROR,DetectServoError,"伺服错误检测"); 
	
	l->AddAction(ACTION_FLASH_CARD_LED,FlashLed,"出卡口灯闪烁"); 
	l->AddAction(ACTION_FLASH_BOOK_LED,FlashLed,"出本口灯闪烁"); 
	l->AddAction(ACTION_FLASH_BATCH_LED,FlashLed,"批量口灯闪烁"); 
	l->AddAction(ACTION_RESET_SLOT,ResetSlot,"出口槽复位"); 

	l->AddAction(ACTION_DETECT_SERVERO_OFFSET,DetectServeroOffset,"伺服偏差检测"); 
	l->AddAction(ACTION_RECORD_YSENSOR,RecordYAllSensorPos,"记录传感器位置");
	l->AddActionParam(ACTION_YTO_SAVEBOOKY,0,TYPE_CHAR,"定位坐标");
	l->AddAction(ACTION_YTO_SAVEBOOKY,GoToY,"升降定位存本位置");
	l->AddAction(ACTION_RESTART_READER,RestartReader,"重启读写器");
	l->AddAction(ACTION_TEST,GetOptionLen,"光斑宽度检测");
	l->StartAction(ACTION_DETECT_SERVER_ERROR);
	l->SetVersion("V1.20201009.1");
	l->SetPrjName("BGG1000");
	l->DisableSM(SM_CIRCLE);
	l->SetSensorLogic(SN_SENDBOOK_CHECK,0);
	l->SetSensorLogic(SN_SENDCARD_CHECK,0);
	gYPath.Status = STATUS_UNKNOWN;
	gXPath.Status = STATUS_UNKNOWN;
	return InitETree();
}


