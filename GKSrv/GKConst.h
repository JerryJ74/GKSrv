#pragma once

#include "cppAsnHeader.h"
#include "asn1v4.h"

//#include "GKConfig.h"
//#include "SockDebug.h"
//#include "MessageThread.h"

#include "..\\CommonFiles\\Config.h"


#include <string>
#include <iostream>


// Complie method
//#define GK_FOR_LOCAL
//#define GK_FOR_HANARO
#define BUILDSTR					"[20050601]"


#define WM_RAS_REGRQST				WM_USER + 81
#define WM_RAS_REGRQST_RESULT		WM_USER + 82
#define WM_RAS_ADMRQST				WM_USER + 83
#define WM_RAS_ADMRQST_RESULT		WM_USER + 84
#define WM_RAS_INFRQST				WM_USER + 85
#define WM_RAS_URGRQST				WM_USER + 86
#define WM_RAS_NEEDLRQ				WM_USER + 87
#define WM_RAS_LOCRQST				WM_USER + 88
#define WM_RAS_LOCRQST_RESULT		WM_USER + 93
#define WM_RAS_LOCCFRM				WM_USER + 89
#define WM_RAS_LCCCFRM_RESULT		WM_USER + 90
#define WM_RAS_LOCRJCT				WM_USER + 91
#define WM_RAS_LOCRJCT_RESULT		WM_USER + 92

#define WM_CALL_LOG					WM_USER + 99

#define WM_Q931_SETUP				WM_USER + 100
#define WM_Q931_SETUP_RESULT		WM_USER + 101
#define WM_Q931_CALLPROCEEDING		WM_USER + 102
#define WM_Q931_CONNECT				WM_USER + 103
#define WM_Q931_CONNECT_RESULT		WM_USER + 104
#define WM_Q931_RELEASECOMPLETE		WM_USER + 105
#define WM_Q931_ALERTING			WM_USER + 106

#define WM_CDR						WM_USER + 107
//#define WM_CHECK_TIMEOUT			WM_USER + 108
#define WM_TIMEOUT					WM_USER + 109
#define WM_MSG_APPSERVER			WM_USER + 110

#define WM_CALLINFO					WM_USER + 101
#define WM_CALLINFO_NOT_FOUND		WM_USER + 102


#define WM_DB_TEST					WM_USER + 300
#define WM_MSG_TEST					WM_USER + 500


#define WM_CONFIG_RELOAD			WM_USER + 600



#define MAX_RAS_BUFSIZE				512
#define MIN_RAS_BUFSIZE				3
#define MAX_Q931_BUFSIZE			1024

// Time server sync interval
#define NTP_INTERVAL				1000 * 1200 // 20min

#define GK_ID						Config::Instance()->GET_GKID()
#define CONST_DBIP					Config::Instance()->GET_DBIP()
#define CONST_DBNAME				Config::Instance()->GET_DBNAME()
#define CONST_DBID					Config::Instance()->GET_DBUSER()
#define CONST_DBPASS				Config::Instance()->GET_DBPASS()
#define CONST_DBPORT				Config::Instance()->GET_DBPORT()	

// 2004. 11. 23. GetIRRInterval() 함수의 잦은 호출로 인한 부하 증가를 확인. 이후 60으로 고정 함.
#define IRR_INTERVAL		60
#define MESSAGE_PORT		7186

#define LWRRQ_INTERVAL				Config::Instance()->GET_RRQINTERVAL()
#define MESSAGE_ADDR				Config::Instance()->GET_APPADDR()
#define LOCAL_IPADDRESS				Config::Instance()->GET_LOCALADDR()

//CDR관련 정의
#define CDR_ADDRESS					Config::Instance()->GET_CDRADDR()
#define DUAL_CDR					Config::Instance()->GET_ENABLEDUALCDR()
#define DUAL_CDR_ADDR				Config::Instance()->GET_DUAL_CDRADDR()


#define RAS_PORT					Config::Instance()->GET_RASPORT()

//only signal server
#define SIGNAL_PORT					Config::Instance()->GET_SIGNALPORT()


#define CALL_SIGNAL_PORT_1720		1720
#define CALL_SIGNAL_PORT_1721		1721
#define CALL_SIGNAL_PORT_1722		1722
#define CALL_SIGNAL_PORT_1723		1723
#define CALL_SIGNAL_LRQ_PROCESS_PORT_1730	1730
#define CALL_SIGNAL_LRQ_PROCESS_PORT_1731	1731


#define AUTH_H323ID					1
#define AUTH_E164					2
#define AUTH_IP_H323ID				3
#define AUTH_E164_H323ID			4
#define AUTH_E164_H323ID_IP			5

// last value 0x03 = H323V3 , 0x04 = H323V4
const OCTET PROTOCOL_ID[6] = {0x00, 0x08, 0x91, 0x4A, 0x00, 0x03};

#define Q931PACKETHEADERLENGTH		9
#define Q931MAXPACKETLENGTH			1024


typedef enum CALL_STATS
{
	e_RAS_ARQ_ACF,
	e_RAS_ARQ_ARJ,
	e_RAS_LRQ_LCF,
	e_RAS_LRQ_LRJ,
	e_RAS_IRR,
	e_RAS_Unknown,
	e_Q931_Setup_ToCalled,
	e_Q931_Setup_ErrorSendSetup,
	e_Q931_CallProceeding,
	e_Q931_Alerting,
	e_Q931_Connect,
	e_Q931_ReleaseComplete_From_Called,
	e_Q931_ReleaseComplete_From_Calling,
	e_Q931_Unknown,
	e_ERR_Socket,
	e_TimeOut
} _CALL_STATS;


typedef struct _RAS_RRQRCF
{
	char h323id[16];
	char e164[128];
	char endpoint_id[19];
	WORD callsignalport;
	char callsignaladdr[16];
	WORD rasport;
	char rasaddr[16];
	char terminaltype[16];
	WORD request_number; 
	BYTE e164_count;
	bool Confirm;
	char rxaddr[16];
	UINT rxport;
} RAS_RRQRCF, *LPRAS_RRQRCF;


typedef struct _RAS_ARQACF
{
	char rxaddr[16];
	char call_id[33];
	WORD crv;
	char calling[16];
	char called[16];
	char calling_ip[16];
	char called_ip[16];
	char dest_num[20];
	char src_num[20];
	bool Confirm;
	BYTE reject_reason;
	WORD called_port;
	bool answer_call;
	WORD request_number;
	WORD process_id;
	UINT rxport;
	WORD routing_group;
} RAS_ARQACF, *LPRAS_ARQACF;


typedef struct _RAS_URQUCF
{
	char h323id[16];
	char endpoint_id[19];
	WORD request_number;
} RAS_URQUCF, *LPRAS_URQUCF;


typedef struct _CALL_INFO
{
	DWORD connect_t;
	CALL_STATS call_stat;
	char arq_time[32];
	char cnt_time[32];
	bool confirm;
	char calling[16];
	char calling_ip[16];
	char src_num[20];
	char called[16];
	char called_ip[16];
	char dest_num[20];
	LONG crv;
	bool connect;
	WORD release_complete;
	BYTE release_cause;
	char addprefix[6];
	char delprefix[6];
	char call_id[33];
	WORD called_port;
	DBTIMESTAMP connect_time;
} CALL_INFO, *LPCALL_INFO;


typedef struct _CDR
{
	char call_id[32];
	LONG crv;
	WORD release_complete;
	char calling[16];
	char calling_ip[16];
	char src_num[20];
	char called[16];
	char called_ip[16];
	char dest_num[20];
	char stime[20];
	char etime[20];
} CDR, *LPCDR;


typedef struct _RAS_IRR
{
	char call_id[33];
	char calling[16];
	LONG crv;
} RAS_IRR, *LPRAS_IRR;


typedef struct _RAS_LRJ
{
	WORD request_number;
	WORD arq_request_number;
	char rx_ip[16];
	WORD rx_port;
	char calling[16];
	char called[16];
	char dest_num[20];
	LONG rx_arq_port;
} RAS_LRJ, *LPRAS_LRJ;


typedef struct _APP_MESSAGE
{
	int type;
	char message[256];
	char gk_name[15];
} APP_MESSAGE, *LPAPP_MESSAGE;



#define CDR_PORT			7788
#define BACKUP_CDR_ADDR		"211.189.4.234"

#define APPMSG_ARQ_SEND		1//CGKConfig::Instance()->GetAppMsgARQ()
#define APPMSG_IRR_SEND		0//CGKConfig::Instance()->GetAppMsgIRR()
#define APPMSG_RRQ_SEND		1//CGKConfig::Instance()->GetAppMsgRRQ()
#define APPMSG_SETUP_SEND	0//CGKConfig::Instance()->GetAppMsgSETUP()
#define APPMSG_CNT_SEND		0//CGKConfig::Instance()->GetAppMsgCONNECT()

#define GK_FOR_BIT_BUILD	"780@calltime"

#define APPMSG_ARQ			0
#define APPMSG_LRQ			1
#define APPMSG_URQ			2
#define APPMSG_IRR			3
#define APPMSG_RRQ			4
#define APPMSG_SETUP		5
#define APPMSG_CONNECT		6
#define APPMSG_RELECOMP		7
#define APPMSG_ENDCALL		8


// CDBWorkThread
// 2004. 06. 08.
// 휴대폰 무료통화 서비스를 위한 트렁크 h323id
const char CONST_TID[] = "trvd";
const char CONST_TID2[] = "trve";


/*
// inline functions
inline void __DEBUG(char * x, ...) 
{ 
	char buff[256] = "";
	va_list va; 

	va_start(va, x);
	vsprintf(buff, x, (va_list)va);
	va_end(va);
	
	CSockDebug::Instance()->RDEBUG((const char *)buff);
}
*/

