
#define GetXAngle(angle) (angle*800/9) //(4*10/360=1/9*800)
#define GetYLen(Len)  (50*Len)		//10*X/5*30=X/15 电子齿轮比 750/15=50
#define GetLeadLen(Len) (80*Len)	//导程20 细分800  800/20=20
#define GetCircleAngle(angle) (160*angle/3)	//4800 1:4的同步带轮比例:angle*(4*4800)/360=480/9=160/3
enum
{
	ACTION_RESET_DEVICE=1,					//1
	ACTION_RESET_X,									//2
	ACTION_GOTO_X,									//3
	ACTION_RESET_Y,									//4
	ACTION_GOTO_CARDY,							//5
	ACTION_GOTO_BOOKY,							//6
	ACTION_YTO_GETCARD,							//7
	ACTION_YTO_GETBOOK,							//8
	ACTION_YTO_SENDCARD,						//9
	ACTION_YTO_SENDBOOK,						//10
	ACTION_YTO_BATCH,								//11
	ACTION_RESET_SENDCARD,					//12
	ACTION_SENDCARDTO_LIMT, 				//13
	ACTION_SENDCARD,								//14
	ACTION_RESET_SENDBOOK,					//15
	ACTION_SENDBOOKTO_LIMT, 				//16
	ACTION_SENDBOOK,								//17
	ACTION_READY_GETCARD_FROM_CAR, 	//18
	ACTION_READY_GETBOOK_FROM_CAR, 	//19
	ACTION_PUSHOUT_CARD,					 	//20
	ACTION_PUSHOUT_BOOK,						//21
	ACTION_BATCH_PUSH_CARDBOOK,   	//22
	ACTION_RECYLDCARD_TO_CAR,				//23
	ACTION_RECYLEBOOK_TO_CAR,				//24
	ACTION_OPEN_CARD_DOOR,					//25
	ACTION_CLOSE_DOOR,							//26
	ACTION_RESET_CIRCLE,						//27
	ACTION_CIRCLE_ORG_TO_CHECK,			//28
	ACTION_CIRCLE_ORG_TO_OUT,				//29
	ACTION_CIRCLE_CHECK_TO_ORG,			//30
	ACTION_CIRCLE_CHECK_TO_OUT,			//31
	ACTION_TEST,			//32
	ACTION_CHECK_RACKPOS,				//33
	ACTION_DETECT_SERVER_ERROR,			//34
	ACTION_FLASH_CARD_LED,					//35
	ACTION_FLASH_BOOK_LED,					//36
	ACTION_FLASH_BATCH_LED,					//37
	ACTION_RESET_SLOT,							//38
	ACTION_DETECT_LIFT_ERROR,				//39
	ACTION_DETECT_SERVERO_OFFSET,		//40			实时检测伺服偏差
	ACTION_OPEN_BOOK_DOOR,					//41
	ACTION_STACTION_RECYLE_ACTION,			//42
	ACTION_DETECTTING_DOOR,					//43			门检测
	ACTION_RECORD_YSENSOR,
	ACTION_YTO_SAVEBOOKY,
	ACTION_RESTART_READER,
};

enum
{
	ERROR_CARDBOX_EMPTY=1004,
	ERROR_BOOKBOX_EMPTY=1005,
};

/// <summary>
/// 消息的wParam数值表示动作执行结果
/// </summary>
enum ActionResult
{
	ACTION_THIN_BOOK=0xfb,	//薄本,APP_SENDBOOK_BOX_RW
	ACTION_FAT_BOOK=0xfa,	//厚本,APP_SENDBOOK_BOX_RW
};

typedef struct _XPath
{
	unsigned char nDir;
	unsigned int  nDistance;
	unsigned char nTargetX;
	unsigned char CurrentX;
	unsigned char Status;
	unsigned char SensorCnt;
}XPath;


typedef struct _YPath
{
	int ToothSpeed;
	signed long nTargetPos;
	signed long nSendCardPos;
	signed long nSendBookPos;
	signed long nCardOutPos;
	signed long nBookOutPos;
	signed long nBatchOutPos;
	
	unsigned char Status;
	signed long YToothBottomPos[11];
	signed long YToothTopPos[11];
	signed long ToothLen[7];
	signed long RealOffset;
	signed long LastToothPos;
	char ToothSensorCnt;
	unsigned char CurrentTooth;
	unsigned char NoCheckCnt;
}YPath;

typedef struct _YSensor
{
	char SensorNum;
	signed long *nSensorPos;
}YSensor;
