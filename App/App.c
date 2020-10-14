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
  * �������ƣ� PlanXPath
  * ���������� ת��·���滮
  * ����˵��
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
  * �������ƣ� ResetDevice
  * ���������� �豸������λ
  * ����˵��		 ������� 1
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
  * �������ƣ� ResetX
  * ���������� X�� ת�̵ĸ�λ
  * ����˵��  ������� 2
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
		l->SetErrorInfo("תͲ��Ƭ�뿪ԭ�㴫����ʧ��!");
lSTEP_ERROR:
	gXPath.Status = STATUS_UNKNOWN;
	l->SetErrorInfo("תͲ��Ƭ��ԭ�㴫����ʧ��!");
STEP_END
/***************************************************************************************************************
  * �������ƣ� GoToX
  * ���������� X�� ת�̵Ķ�λ
  * ����˵��   ������� 3
****************************************************************************************************************/
STEP_START(GoToX):
		if(l->GetByteParam(0)>24)
		{
			l->SetErrorInfo("תͲ��λ����������Χ!");
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
		PlanXPath();						//����תͲ��·��:���򣬵�Ƭ����
		if(gXPath.nDistance==0)	//�͵�ǰ��λ����ͬ�Ͳ���
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
		l->SetErrorInfo("תͲ��λ��Ƭ�Ҵ���������!");
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
			l->SetErrorInfo("������λ��Ƭ�������");
		}
		else
		{
			gYPath.nBookOutPos = gYPath.nCardOutPos-GetYLen(61);
			gYPath.nBatchOutPos = gYPath.nBookOutPos-GetYLen(113);
			l->SetActionOk();
		}
lSTEP_ERROR:
		
		l->SetErrorInfo("�ҷ�������������");
lSTEP_ERROR1:
		l->SetErrorInfo("�ҷ�������������");
lSTEP_ERROR2:
		l->SetErrorInfo("�ҳ�������������");
STEP_END
void IsYSafety()
{
		if(l->IsSensorOff(SN_SENDCARD_ORG))
		{
			l->SetErrorInfo("����ԭ��û�и�Ӧ");
			return ;
		}
		
		if(l->IsSensorOff(SN_SENDBOOK_ORG))
		{
			l->SetErrorInfo("����ԭ��û�и�Ӧ");
			return ;
		}
}
/***************************************************************************************************************
  * �������ƣ� ResetY
  * ���������� Y�� ������λ
  * ����˵��   ������� 4
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
			l->RunABS(SM_Y,-150);							  //���¶���10mm
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
		l->RunSM(SM_Y,GetYLen(1100));	//�����Ҵ�����
		l->WaitRunLength(SM_Y,3000,STEP4);						//������һ����
lSTEP4:
		l->WaitSensorOff(SN_Y_UNIT,STEP5);
		
		l->WaitSMStop(SM_Y,STEP_ERROR);
lSTEP5:
		gYPath.YToothTopPos[nToothIndex] = l->GetAbsPos(SM_Y);		//ת�����ٶ�Ϊ0��λ��
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
			l->SetErrorInfo("������¼����������");
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
		l->SetErrorInfo("�ŷ�����!");
lSTEP_ERROR:
		gYPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("������λ���������� ������!");
STEP_END
/***************************************************************************************************************
  * �������ƣ� CreateYCordinate
  * ���������� ��������Y�������ϵ
  * ����˵�������ͱ�����һ����׼��(�Կ�Ϊ��׼)����ΪY������ԭ��,���ȿ���4mm ÿ��֮��ľ���14mm��֮��ľ���13mm ÿ�鿨9�� ��8��,
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
	else if(PosType==1)	//ȡ����λ��
	{
		nGroupNum = Pos/8;
		nCellNum = Pos%8;
		return gYPath.YToothBottomPos[nGroupNum]+GetYLen(14)*(nCellNum)+GetYLen(4.5);
	}
	else								//�汾��λ��
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
if(l->GetDir(SM_Y)==1)	//��������,�ҵײ�����
{
	for(int i=0;i<7;i++)
	{
		if(abs(gYPath.YToothBottomPos[i]-gYPath.LastToothPos)<300)
		{
			UpdateAllPos(gYPath.LastToothPos-gYPath.ToothSpeed/1000-gYPath.YToothBottomPos[i]);
			//gYPath.YToothBottomPos[i] = gYPath.LastToothPos-gYPath.ToothSpeed/1000; //�����ٶ�3000����ô����Ҫ0.12mm��ƫ�����Ӧ�����ȥ3��Ϊ��׼��Ȼ����ƫ�Ƽ���
		}
	}
}
else
{
	for(int i=0;i<7;i++)
	{
		if(abs(gYPath.YToothTopPos[i]-gYPath.LastToothPos)<300)
		{
			gYPath.LastToothPos += GetYLen(0.9);							//������߿��ƫ��
			gYPath.LastToothPos += 2*gYPath.ToothSpeed/1000;	//���������ٶȲ�һ����ɵ�ƫ��
			gYPath.YToothTopPos[i] = gYPath.LastToothPos;
			UpdateAllPos(gYPath.LastToothPos-gYPath.YToothTopPos[i]);
			break;
		}
	}
}
l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * �������ƣ� GoToBookY
  * ���������� ����Ԫ��λ-��Ԫ��ļ��14mm ��֮��ĵ�Ԫ���13mm,һ��8���洢��Ԫ,���ȿ���9mm
  * ����˵��   ������� 6
****************************************************************************************************************/
char ActionType;
STEP_START(GoToY):
		IsYSafety();
		
		if(l->IsActionRun(ACTION_GOTO_CARDY))
		{
			if(l->GetByteParam(0)>63)
			{
				l->SetErrorInfo("������λ����������Χ!");
				break;
			}
			ActionType = 0;
			gYPath.nTargetPos = GetYPos(0,l->GetByteParam(0));
		}
		else if(l->IsActionRun(ACTION_YTO_SAVEBOOKY))
		{
			if(l->GetByteParam(0)>56)
			{
				l->SetErrorInfo("������λ����������Χ!");
				break;
			}
			ActionType = 1;
			gYPath.nTargetPos = GetYPos(1,l->GetByteParam(0));
		}
		else
		{
			if(l->GetByteParam(0)>56)
			{
				l->SetErrorInfo("������λ����������Χ!");
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
		l->SetErrorInfo("����ײ����λ!");
lSTEP5:
		l->StopSM(SM_Y);
		gYPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("�ŷ�����!");
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
  * �������ƣ�YGoToGetCard
  * ����������Y��������������λ��
  * ����˵��  ������� 7
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
		l->RunABS(SM_Y,(*gYSensor.nSensorPos)+GetYLen(3));	//���϶���3mm
		l->WaitSMStop(SM_Y,STEP3);
lSTEP3:
		if(l->IsSensorOn(gYSensor.SensorNum))
		{
			l->NextTo(STEP6);	//��Ҫ�뿪������
		}
		else
		{
			l->NextTo(STEP4);	//���˴�������Ҫ�����Ҵ�����
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
		l->SetErrorInfo("�����뿪λ�ô���������!");	//�뿪ȡ��������λ�ó���
lSTEP_ERROR:
		gYPath.Status = STATUS_UNKNOWN;
		l->SetErrorInfo("����������λ�ô���������!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�YGoToGetBook
  * ����������Y��������������λ��
  * ����˵��  ������� 8
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
  * �������ƣ�YGoToBatch
  * ����������Y�������������ڵ�λ��
  * ����˵���� ������� 11
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
  * �������ƣ�ResetSendCard
  * ������������λ����
  * ����˵��
****************************************************************************************************************/
STEP_START(ResetSendCard):
		l->FindSensor(SM_SENDCARD,-GetLeadLen(150),SN_SENDCARD_ORG,STEP2,STEP3);
lSTEP2:
		l->StopSM(SM_SENDCARD);
	  l->SetActionOk();
lSTEP3:
		l->FindSensor(SM_SENDCARD,-GetLeadLen(150),SN_SENDCARD_ORG,STEP2,STEP_ERROR);
lSTEP_ERROR:
			l->SetErrorInfo("���临λ�Ҵ�����ʧ��!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�SendCardToLimt
  * ������������������λ
  * ����˵��
****************************************************************************************************************/
STEP_START(SendCardToLimt):
		l->FindSensor(SM_SENDCARD,GetLeadLen(150),SN_SENDCARD_LIMT,STEP2,STEP3);
lSTEP2:
		l->StopSM(SM_SENDCARD);
		l->SetActionOk();
lSTEP3:
		l->FindSensor(SM_SENDCARD,GetLeadLen(150),SN_SENDCARD_LIMT,STEP2,STEP_ERROR);
lSTEP_ERROR:
			l->SetErrorInfo("���䷢���Ҵ�����ʧ��!");		
STEP_END
/***************************************************************************************************************
  * �������ƣ�SendCard
  * ��������������
  * ����˵��
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
  * �������ƣ�ResetSendBook
  * ����������������λ
  * ����˵��
****************************************************************************************************************/
STEP_START(ResetSendBook):
			l->FindSensor(SM_SENDBOOK,-GetLeadLen(150),SN_SENDBOOK_ORG,STEP2,STEP3);
lSTEP2:
		l->StopSM(SM_SENDBOOK);
		l->SetActionOk();
lSTEP3:
	  l->FindSensor(SM_SENDBOOK,-GetLeadLen(150),SN_SENDBOOK_ORG,STEP2,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("��λ��������!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�SendBookToLimt
  * ������������������λ
  * ����˵��
****************************************************************************************************************/
STEP_START(SendBookToLimt):
		l->FindSensor(SM_SENDBOOK,GetLeadLen(160),SN_SENDBOOK_LIMT,STEP2,STEP3);
lSTEP2:
	l->StopSM(SM_SENDBOOK);
	l->SetActionOk();
lSTEP3:
	l->FindSensor(SM_SENDBOOK,GetLeadLen(160),SN_SENDBOOK_LIMT,STEP2,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("���䷢���Ҵ�����ʧ��!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�SendBook
  * ��������������
  * ����˵��
****************************************************************************************************************/
char IsFatBook;
STEP_START(SendBook):
		if(l->IsSensorOn(SN_SENDBOOK_PRESS))
		{
			l->SetErrorInfo("ѹ��û��ѹ��!");
			break;
		}
//		
//		if(l->IsSensorOn(SN_BOOK_DOOR))
//		{
//			l->SetErrorInfo("������û�й�!");
//			break;
//		}
				
		if(l->IsSensorOn(SN_SENDBOOK_CHECK))
		{
			l->WaitAction(ACTION_RESET_SENDBOOK,STEP2,STEP_ERROR);
		}
		else
		{
			l->SetActionError(ERROR_BOOKBOX_EMPTY);//�����
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
		l->SetActionError(ACTION_FAT_BOOK);	//��
lSTEP6:
		l->SetActionError(ACTION_THIN_BOOK); //����
lSTEP_ERROR:
		l->SetErrorInfo("������λʧ��!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�ReadyGetCardFromCar
  * ������������С���ӿ�
  * ����˵��
****************************************************************************************************************/
STEP_START(ReadyGetCardFromCar):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetErrorInfo("������֤��!");
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
		l->SetErrorInfo("�ӿ���ʱ!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�ReadyGetBookFromCar
  * ������������С���ӱ�
  * ����˵��
****************************************************************************************************************/
STEP_START(ReadyGetBookFromCar):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetErrorInfo("������֤��!");
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
		l->SetErrorInfo("�����뿪����������!");
lSTEP_ERROR1:
		l->StopSM(SM_PUSH_BOOK);
		l->SetActionOk();
lSTEP_ERROR:
		l->StopSM(SM_PUSH_BOOK);
		l->SetErrorInfo("�ӱ���ʱ!!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�PushOutCard
  * �����������¿�
  * ����˵��
****************************************************************************************************************/
STEP_START(PushOutCard):
		l->RunSM(SM_PUSH_CARD,-30000);
		l->WaitTimeOut(1000,STEP2);
lSTEP2:
			l->StopSM(SM_PUSH_CARD);
			if(l->IsSensorOn(SN_OUT_CHECK))
			{
				l->SetErrorInfo("�¿�����������֤��!");
			}
			else
			{
				l->SetActionOk();
			}
STEP_END
/***************************************************************************************************************
  * �������ƣ�PushOutBook
  * �����������±�
  * ����˵��
****************************************************************************************************************/
STEP_START(PushOutBook):
		l->RunSM(SM_PUSH_BOOK,-30000);
		l->WaitTimeOut(1000,STEP2);
lSTEP2:
		l->StopSM(SM_PUSH_BOOK);
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetErrorInfo("������֤��!");
		}
		else
		{
			l->SetActionOk();
		}
STEP_END
/***************************************************************************************************************
  * �������ƣ�BatchOutCardBook
  * ������������������ ����
  * ����˵��
****************************************************************************************************************/
STEP_START(BatchOutCardBook):
		l->RunSM(SM_PUSH_BOOK,-30000);
		l->WaitTimeOut(6000,STEP_OK);
lSTEP_OK:
		l->StopSM(SM_PUSH_BOOK);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * �������ƣ�RecyleCardToCar
  * �������������տ���С��
  * ����˵��
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
		l->SetErrorInfo("û��֤����������֤�ڹر�!!");
		l->StartAction(ACTION_CLOSE_DOOR);
lSTEP_ERROR:
		l->StopSM(SM_PUSH_CARD);
		l->SetErrorInfo("�������뿪����������!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�RecyleBookToCar
  * �������������ձ���С��
  * ����˵��
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
		l->SetErrorInfo("û��֤����������֤�ڹر�!");
		l->StartAction(ACTION_CLOSE_DOOR);
lSTEP_ERROR:
		l->StopSM(SM_PUSH_BOOK);
		l->SetErrorInfo("�������뿪����������!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�CloseDoor
  * �����������ص�����
  * ����˵��
****************************************************************************************************************/
STEP_START(CloseDoor):
		if(l->IsSensorOn(SN_OUT_CHECK))
		{
			l->SetErrorInfo("������֤!");
			break;
		}
		
		l->FindSensor(SM_DOOR,4800,SN_DOOR_CLOSE,STEP_OK,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("�ҹ��Ŵ���������!");
lSTEP_OK:
		l->StopSM(SM_DOOR);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * �������ƣ�OpenBookDoor
  * �����������򿪱�������
  * ����˵��
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
		l->SetErrorInfo("�Ҵ򿪱������Ŵ���������!");
lSTEP_OK:
		l->StopSM(SM_DOOR);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * �������ƣ�OpenDoor
  * ���������������忪��
  * ����˵��
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
		l->SetErrorInfo("�Ҵ򿪿������Ŵ���������!");
lSTEP_OK:
		l->StopSM(SM_DOOR);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * �������ƣ�ResetCircle
  * ������������λת��
  * ����˵��
****************************************************************************************************************/
STEP_START(ResetCircle):
		l->FindSensor(SM_CIRCLE,GetCircleAngle(105),SN_CIRCLE_ORG,STEP2,STEP3);
lSTEP2:
		l->DisableSM(SM_CIRCLE);
		l->WaitTimeOut(200,STEP_OK);
lSTEP3:
		l->FindSensor(SM_CIRCLE,GetCircleAngle(18),SN_CIRCLE_ORG,STEP2,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("��λ��תС������!");
lSTEP_OK:
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * �������ƣ�CircleFromOrgToCheck
  * ����������С��ת�̴�ԭ�㵽���˼��λ
  * ����˵��
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
		l->SetErrorInfo("��תС����λ���λ����!");
lSTEP_OK:
		l->StopSM(SM_CIRCLE);
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * �������ƣ�CircleFromOrgToOut
  * ����������С��ת�̴�ԭ�㵽����λ��
  * ����˵��
****************************************************************************************************************/
STEP_START(CircleFromOrgToOut):
		l->FindSensor(SM_CIRCLE,-GetCircleAngle(100),SN_CIRCLE_OUT,STEP2,STEP3);
lSTEP2:
		l->DisableSM(SM_CIRCLE);
		l->WaitTimeOut(200,STEP_OK);
lSTEP3:
		l->FindSensor(SM_CIRCLE,-GetCircleAngle(15),SN_CIRCLE_OUT,STEP2,STEP_ERROR);
lSTEP_ERROR:
		l->SetErrorInfo("��תС����λ���ڳ���!");
lSTEP_OK:
		l->SetActionOk();
STEP_END
/***************************************************************************************************************
  * �������ƣ�DetectServoError 
  * �����������ŷ�������(������������λ���)
  * ����˵��
****************************************************************************************************************/
STEP_START(DetectServoError):
		l->WaitTimeOut(3000,STEP2);
lSTEP2:
		l->WriteIO(DM_SEVRO_BRAKE,DM_CW);	//�ͷ�
		l->WaitSensorOn(SN_SEVRO_ERROR,STEP3);
lSTEP3:
			l->WriteIO(DM_SEVRO_BRAKE,DM_STOP);	//ɲ����ס���
			l->WaitSensorOff(SN_SEVRO_ERROR,STEP2);
STEP_END
/***************************************************************************************************************
  * �������ƣ�DetectServoError 
  * ����������ʵʱ����ŷ�ƫ��
  * ����˵��
****************************************************************************************************************/
STEP_START(DetectServeroOffset):
		if(l->IsSensorOn(SN_Y_UNIT))
		{
			l->WaitSensorOff(SN_Y_UNIT,STEP3);	//�˶�����Ԫ��Ķ���
		}
		else
		{
			l->WaitSensorOn(SN_Y_UNIT,STEP2);	//�˶�����Ԫ��ĵײ�
		}
							
		l->WaitSMStop(SM_Y,STEP4);
lSTEP2:
		l->WaitSensorOff(SN_Y_UNIT,STEP3);	//�˶�����Ԫ��Ķ���
		l->WaitSMStop(SM_Y,STEP4);
lSTEP3:
		l->WaitSensorOn(SN_Y_UNIT,STEP2);	//�˶�����Ԫ��ĵײ�
		l->WaitSMStop(SM_Y,STEP4);
lSTEP4:
		l->SetActionOk();
lSTEP_ERROR:
		l->SetErrorInfo("����ƫ�������!");
STEP_END
signed long testPos;
char OptialLen;
/***************************************************************************************************************
  * �������ƣ�DetectServoError 
  * ����������ʵʱ����ŷ�ƫ��
		* ����˵�� �ٶ�3000->7:33 32; 6:34 34;5:34 33;4->35 35 ;3->35 34;2->36 34;�ٶȣ�5000->3A,40�ٶ�Խ��ƫ��Խ��,�����һ���׵Ĺ�߿��
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
		l->SetErrorInfo("����ƫ�������!");
STEP_END
/***************************************************************************************************************
  * �������ƣ�FlashCardLed 
  * ������������ʽ֤������˸
  * ����˵��
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
  * �������ƣ�ResetSlot 
  * ����������֤���۵ĸ�λ
  * ����˵��
****************************************************************************************************************/
STEP_START(ResetSlot):
		if(l->IsSensorOn(SN_OUT_CHECK))	//����Ϊ�����߱�
		{
			l->SetErrorInfo("������֤!");
		}
		else
		{
			l->FindSensor(SM_PUSH_CARD,-15000,SN_OUT_CHECK,STEP2,STEP4);
		}
lSTEP2:
		l->WaitTimeOut(250,STEP3);
lSTEP3:
		l->DisableSM(SM_PUSH_CARD);
		l->SetErrorInfo("������֤��");
lSTEP4:	//�ұ�
		l->DisableSM(SM_PUSH_CARD);
		l->FindSensor(SM_PUSH_BOOK,-15000,SN_OUT_CHECK,STEP5,STEP7);
lSTEP5:
		l->WaitTimeOut(300,STEP6);
lSTEP6:
		l->DisableSM(SM_PUSH_BOOK);
		l->SetErrorInfo("������֤��");
lSTEP7:
		l->SetActionOk();
		l->DisableSM(SM_PUSH_BOOK);	
STEP_END
/***************************************************************************************************************
  * �������ƣ�RestartReader
  * ������������д����λ
  * ����˵��
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
	l->AddAction(ACTION_RESET_DEVICE,ResetDevice,"�豸��λ");
	
	l->AddAction(ACTION_RESET_X,ResetX,"תͲ��λ");  
	l->AddActionParam(ACTION_GOTO_X,0,TYPE_CHAR,"��λ����");
	l->AddAction(ACTION_GOTO_X,GoToX,"תͲ��λ");
	
	l->AddAction(ACTION_RESET_Y,ResetY1,"������λ");
	l->AddActionParam(ACTION_GOTO_CARDY,0,TYPE_CHAR,"��λ����");
	l->AddAction(ACTION_GOTO_CARDY,GoToY,"��������λ");
	l->AddActionParam(ACTION_GOTO_BOOKY,0,TYPE_CHAR,"��λ����");
	l->AddAction(ACTION_GOTO_BOOKY,GoToY,"��������λ");
	l->AddAction(ACTION_YTO_GETCARD,YGoToSensor,"������λ��������");
	l->AddAction(ACTION_YTO_GETBOOK,YGoToGetBook,"������λ��������");
	l->AddAction(ACTION_YTO_SENDCARD,YGoToSensor,"������λ����λ��");
	l->AddAction(ACTION_YTO_SENDBOOK,YGoToSensor,"������λ����λ��");
	l->AddAction(ACTION_YTO_BATCH,YGoToBatch,"������������λ��");
	
	
	l->AddAction(ACTION_RESET_SENDCARD,ResetSendCard,"���临λ");
	l->AddAction(ACTION_SENDCARDTO_LIMT,SendCardToLimt,"���俨������λ");
	l->AddAction(ACTION_SENDCARD,SendCard,"����");
	
	l->AddAction(ACTION_RESET_SENDBOOK,ResetSendBook,"��λ����");
	l->AddAction(ACTION_SENDBOOKTO_LIMT,SendBookToLimt,"���乳�ӵ���λ");
	l->AddAction(ACTION_SENDBOOK,SendBook,"����");
	
	l->AddAction(ACTION_READY_GETCARD_FROM_CAR,ReadyGetCardFromCar,"׼����С���ӿ�");
	l->AddAction(ACTION_READY_GETBOOK_FROM_CAR,ReadyGetBookFromCar,"׼����С���ӱ�");
	
	l->AddAction(ACTION_PUSHOUT_CARD,PushOutCard,"�¿�");
	l->AddAction(ACTION_PUSHOUT_BOOK,PushOutBook,"�±�");
	
	l->AddAction(ACTION_BATCH_PUSH_CARDBOOK,BatchOutCardBook,"�������֤");
	l->AddAction(ACTION_RECYLDCARD_TO_CAR,RecyleCardToCar,"���տ���С��");
	l->AddAction(ACTION_RECYLEBOOK_TO_CAR,RecyleBookToCar,"���ձ���С��");


	l->AddAction(ACTION_OPEN_CARD_DOOR,OpenCardDoor,"�򿪿�����");
	l->AddAction(ACTION_OPEN_BOOK_DOOR,OpenBookDoor,"�򿪱�����");
	l->AddAction(ACTION_CLOSE_DOOR,CloseDoor,"�رյ���"); 
	
	l->AddAction(ACTION_RESET_CIRCLE,ResetCircle,"С����ת��λ"); 
	l->AddAction(ACTION_CIRCLE_ORG_TO_CHECK,CircleFromOrgToCheck,"С����ת�����λ"); 
	l->AddAction(ACTION_CIRCLE_ORG_TO_OUT,CircleFromOrgToOut,"С����ת������"); 
	
	l->AddAction(ACTION_CHECK_RACKPOS,CheckRackOffset,"������ƫ��"); 
	l->AddAction(ACTION_DETECT_SERVER_ERROR,DetectServoError,"�ŷ�������"); 
	
	l->AddAction(ACTION_FLASH_CARD_LED,FlashLed,"�����ڵ���˸"); 
	l->AddAction(ACTION_FLASH_BOOK_LED,FlashLed,"�����ڵ���˸"); 
	l->AddAction(ACTION_FLASH_BATCH_LED,FlashLed,"�����ڵ���˸"); 
	l->AddAction(ACTION_RESET_SLOT,ResetSlot,"���ڲ۸�λ"); 

	l->AddAction(ACTION_DETECT_SERVERO_OFFSET,DetectServeroOffset,"�ŷ�ƫ����"); 
	l->AddAction(ACTION_RECORD_YSENSOR,RecordYAllSensorPos,"��¼������λ��");
	l->AddActionParam(ACTION_YTO_SAVEBOOKY,0,TYPE_CHAR,"��λ����");
	l->AddAction(ACTION_YTO_SAVEBOOKY,GoToY,"������λ�汾λ��");
	l->AddAction(ACTION_RESTART_READER,RestartReader,"������д��");
	l->AddAction(ACTION_TEST,GetOptionLen,"��߿�ȼ��");
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


