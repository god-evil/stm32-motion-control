enum
{
	STATUS_UNKNOWN,
	STATUS_1,
	STATUS_2,
	STATUS_3,
	STATUS_ERROR,
};

enum
{
	DM_STOP,
	DM_CW,
	DM_CCW,
	DM_BRAKE,
};

enum
{
	TYPE_CHAR,
	TYPE_UCHAR,
	TYPE_SHORT,
	TYPE_USHRT,
};
enum _opration
{
	STEP1=0,
	STEP2,
	STEP3,
	STEP4,
	STEP5,
	STEP6,
	STEP7,
	STEP8,
	STEP9,
	STEP10,
	STEP11,
	STEP12,
	STEP13,
	STEP14,
	STEP15,
	STEP16,
	STEP17,
	STEP18,
	STEP19,
	STEP20,
	STEP21,
	STEP22,
	STEP23,
	STEP24,
	STEP25,
	STEP26,
	STEP27,
	STEP28,
	STEP29,
	STEP30,
	STEP31,
	STEP32,
	STEP33,
	STEP34,
	STEP35,
	STEP36,
	STEP37,
	STEP38,
	STEP39,
	STEPEX01,   
	STEPEX02,
	STEPEX03,
	STEPEX04,
	STEPEX05,
	STEPEX06,
	STEPEX07,
	STEPEX08,
	
	STEP_ERROR6 = 248,
	STEP_ERROR5 = 249,
	STEP_ERROR4 = 250,
	STEP_ERROR3 = 251,
	STEP_ERROR2 = 252,
	STEP_ERROR1 = 253,
	STEP_ERROR = 254,
	STEP_OK = 255,
};



#define STEP_START(x) void x() {switch(l->GetThisStep()){case STEP1
#define lSTEP1 break;case STEP1
#define lSTEP2 break;case STEP2
#define lSTEP3 break;case STEP3
#define lSTEP4 break;case STEP4
	
#define lSTEP5 break;case STEP5
#define lSTEP6 break;case STEP6
#define lSTEP7 break;case STEP7
#define lSTEP8 break;case STEP8
	
#define lSTEP9 break;case STEP9
#define lSTEP10 break;case STEP10
#define lSTEP11 break;case STEP11
#define lSTEP12 break;case STEP12
	
#define lSTEP13 break;case STEP13
#define lSTEP14 break;case STEP14
#define lSTEP15 break;case STEP15
#define lSTEP16 break;case STEP16
	
#define lSTEP17 break;case STEP17
#define lSTEP18 break;case STEP18
#define lSTEP19 break;case STEP19
#define lSTEP20 break;case STEP20

#define lSTEP21 break;case STEP21
#define lSTEP22 break;case STEP22
#define lSTEP23 break;case STEP23
#define lSTEP24 break;case STEP24

#define lSTEP_OK break;case STEP_OK
#define lSTEP_ERROR break;case STEP_ERROR
#define lSTEP_ERROR1 break;case STEP_ERROR1
#define lSTEP_ERROR2 break;case STEP_ERROR2
#define lSTEP_ERROR3 break;case STEP_ERROR3
#define lSTEP_ERROR4 break;case STEP_ERROR4
#define lSTEP_ERROR5 break;case STEP_ERROR5
#define lSTEP_ERROR6 break;case STEP_ERROR6

#define STEP_END break;}}

typedef struct _ActLib
{
	
		//步进
	void (*RunSM)(char sm_id,int nLen);
	void (*BrakeSM)(char sm_id);
	void (*RunABS)(char sm_id,int step);
	int (*GetAbsPos)(unsigned char sm_id);
	void (*SetAbsOrg)(unsigned char sm_id);
	void (*DisableSM)(unsigned char sm_id);
	char (*IsSMStop)(char sm_id);
	void (*StopSM)(unsigned char sm_id);
	char (*GetDir)(char sm_id);	
	void (*SetSMSpeed)(char ID,int nSpeed);
  void (*SetSoftNLimt)(char sm_id,char sensor);
	void (*SetSoftPLimt)(char sm_id,char sensor);
	
	//输入传感器,输出
	char (*IsSensorOn)(char SensorNum);
	char (*IsSensorOff)(char SensorNum);
	void (*SetSensorLogic)(char Id,char Logic);
	void (*WriteIO)(char dm_id,char status);
	
	void (*NextTo)(unsigned char NextStep);
	void (*SetActionError)(unsigned int nErrorCode);
	unsigned short (*IsActionOk)(unsigned char ActionID);	
	unsigned short (*IsActionRun)(unsigned char ActionID);
	void (*SetActionOk)(void);
	void (*SetResult)(char id,unsigned short result);
	
	unsigned char (*StartAction)(char ActionId);
	void (*AddAction)(int nActionNum,void (*OpApp)(),const char* pActionName);
	void (*AddSlaveAction)(int nActionNum,char SlaveActionNum,char BoardID,const char* pActionName);
	void (*AddActionParam)(unsigned char ID,char ParamPos,char paramtype,const char* pParamName);
	void (*SetActionParam)(char ActionId,char* ActionParam,char Len);
	
	void (*ExitAction)(char ActionId);
	unsigned char (*GetByteParam)(int pos);
	
	//事件
	void (*WaitSMStop)(unsigned char SmId,unsigned char StopStep);
	void (*WaitSensorOn)(unsigned char SensorId,unsigned char OnStep);
	void (*WaitSensorOff)(unsigned char SensorId,unsigned char OffStep);
	void (*WaitAction)(unsigned char ActionId,unsigned char OkStep,unsigned char ErrorStep);
	void (*JustWaitAction)(unsigned char ActionId,unsigned char OkStep,unsigned char ErrorStep);
	void (*WaitTimeOut)(unsigned int Time,unsigned char OutStep);
	void (*WaitRunLength)(unsigned char SmId,signed long nLen,unsigned char StopStep);
	void (*SetErrorInfo)(const char* error_text);
	void (*SetSubError)(void);
	
	void (*FindSensor)(unsigned char MotorId,signed int nLen,unsigned char SnId,unsigned char OkStep,unsigned char ErrorStep);
	void (*LeaveSensor)(unsigned char MotorId,signed int nLen,unsigned char SnId,unsigned char OkStep,unsigned char ErrorStep);
	unsigned char (*GetThisStep)();

	//通讯
	void (*SendUartData)(char nPort,unsigned char* pData,int len);
	void (*SetUartRxCallBack)(char nPort,void* pCallback);
	
	
	void (*SetPrjName)(char* name);
	void (*SetVersion)(const char* name);
	
	void (*StartTimer)(unsigned int nTime);
  	char (*IsTimeOut)();	
	void (*CopyDataFromUser)(char* pData,int Len);
	void (*CopyDataToUser)(char* pData,int nLen);
	void (*RunDelayTask)(void (*OpApp)(),unsigned short TimeOut);	
	int (*GetSMSpeed)(char id);
}ActLib;

