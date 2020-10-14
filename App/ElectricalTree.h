typedef struct _ElectNode
{
	char ID;
	unsigned char Name[40];
}ENode;

typedef struct _ElectTree
{
	ENode* SensorList;
	ENode* SMList;
	ENode* DMList;
	char SensorCnt;
	char SMCnt;
	char DMCnt;
}ETree;

enum
{
	SM_SENDBOOK=0,		//发本步进电机
	SM_SENDCARD,			//发卡步进电机
	SM_Y,							//升降电梯伺服电机
	SM_PUSH_BOOK,			//出本步进电机(和批量出口公用)
	SM_PUSH_CARD,			//出卡步进电机	
	SM_DOOR,					//挡板门电机
	SM_X,							//转筒步进电机
	SM_CIRCLE,				//圆盘电机	
};

enum
{
	SN_X_ORG,//2
	SN_X_POS, //1
	SN_SENDBOOK_ORG,//3
	SN_SENDBOOK_LIMT,//4
	SN_SENDBOOK_CHECK,//5
	SN_SENDBOOK_FATBOOK,//6
	SN_SENDBOOK_PRESS, //7
	SN_SENDCARD_ORG,//8
	SN_SENDCARD_LIMT,//9
	SN_SENDCARD_CHECK,//10
	SN_Y_SENDBOOK,//11
	SN_Y_SENDCARD,//12
	SN_Y_BATCHBOOK,//13
	SN_Y_GETBOOK,//14
	SN_Y_GETCARD,//15
	SN_Y_LIMTP,  //16
	SN_DOOR_BOOK_OPEN=19, //17
	SN_DOOR_CARD_OPEN=28, //17
	SN_DOOR_CLOSE=18,//18
	SN_OUT_CHECK=20,//19
	SN_Y_UNIT=21, //20
	
	SN_Y_DOWNLIMT=16,	//下限位
	SN_Y_UPLIMT=17,		//上限位
	
	SN_CIRCLE_ORG=22,
	SN_CIRCLE_CHECK=23,		
	SN_CIRCLE_OUT=24,
	SN_SEVRO_ERROR=25,//伺服报警
	SN_OUT_DOOR=26,//
	SN_BOOK_DOOR=30,
};


ETree gETree;
ENode gSensorList[]=
{
	{SN_X_ORG,"转盘原点"},
};



ENode gStepMotorList[]=
{
	{SM_SENDBOOK,"SM101升降"},
};

enum
{
	DM_SEVRO_ALARM,
	DM_SEVRO_BRAKE,
	DM_CARD_LED,			//卡式证件指示灯
	DM_BOOK_LED,			//本式证件指示灯
	DM_BATCH_LED,			//批量证件指示灯
};

ETree* InitETree()
{
	gETree.SMList = &gStepMotorList[0];
	gETree.SensorList = &gSensorList[0];
	gETree.SensorCnt = sizeof(gSensorList)/sizeof(ENode);
	gETree.SMCnt = sizeof(gStepMotorList)/sizeof(ENode);
	return &gETree;
}
