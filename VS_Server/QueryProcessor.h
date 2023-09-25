#pragma once
#include "Com_Def.h"
#include "DB_Def.h"
#include "IModel.h"
#include "ADO.h"
#include "MsgController.h"
#include "Message.h"
#include "Util.h"
#include "IController.h"


class CQueryProcessor
{
public:
	CQueryProcessor(void);
	~CQueryProcessor(void);

	int InitQueryProcess( BYTE bNonblock );
	int SetDBCmdConnecter( _ConnectionPtr DBConnector );
	int SetDBReadConnecter( _ConnectionPtr DBConnector );

	int SetDBManager( IModel * ModelHandle );
	int SetControllerHandler( IController *  ControllerHandle );
	int OnRecvMsg( MsgInfo_t * MsgInfo );
	int Release( void );	
	
private:

	_ConnectionPtr m_DBCmdCon;
	_ConnectionPtr m_DBReadCon;

	//_CommandPtr m_CMD ;
	//_RecordsetPtr  m_RS;

	//_RecordsetPtr  m_FindVS_RS;
	//_RecordsetPtr  m_FindLocation_RS;
	//_RecordsetPtr  m_FindCar_RS;

	IModel * m_DBManager;
	IController * m_Controller;
	CADO m_ADO;
	CMsgController m_MsgController;
	CMessage m_Message;
	CUtil m_Util;
	int m_ServerID;
	BYTE m_bNonblock;
	BOOL m_bDBUse;
	int m_nForceUpdateTime;

	void DumpStr( CString &str );
	BOOL ChkDBCon();	
	BOOL IsSameErrData( QueryMsg_t * ptrQuery );
	int InsertNetworkTable(short Msg, TCHAR NetworkState,  DummyMsg_t * DummyMsg );
	int InsertErrTable( TCHAR * pErrcode, int Size, ErrMsg_t * ErrMsg);
	int DeleteErrTable( TCHAR * pErrcode, int Size, DummyMsg_t * DummyMsg );

	/* 차량 경로 로직 */
	BOOL GetPreLocationCode( TCHAR * CurLocation, PreLocation_t * PreLocation );
	BOOL  ParseVSImageMsg( ClientRecvMsg_t * ClientRecvMsg, VSImage_t *VSImage );
	BOOL FindVisionSensor( VSImage_t * VSImage, VST_VS * VSTable );
	BOOL ProcVSCarTable( PreLocation_t * PreLocation, VST_VS_CAR * VSCarTable , TCHAR * DeleteTime );
	int InsertVSCarTable( VST_VS_CAR * VSCarTable );
	int DeleteVSCarTable( VST_VS_CAR * VSCarTable , TCHAR * DeleteTime);
	int UpdateVSCarTable( VST_VS_CAR * VSCarTable);
	BOOL InternalVSCarTable( int LocationType, VST_VS_CAR * VSCarTable, TCHAR * DeleteTime );

	BOOL CheckDBUseState();
	BOOL IsOldTimeData( VST_VS_CAR * VSCarTable ) ;
	int InsertErrDataWithOldTime( VST_VS_CAR * VSCarTable, TCHAR * csLastDBSeeTime, BOOL IsSame );


	/* VST_COMPANY 테이블 관련 쿼리 */
	static int VST_COMPANY_INSERT( QueryMsg_t * ptrQuery /*VST_COMPANY*/);
	static int VST_COMPANY_DELETE( QueryMsg_t * ptrQuery /*VST_COMPANY*/);	
	static int VST_COMPANY_UPDATE( QueryMsg_t * ptrQuery /*VST_COMPANY*/);
	static int VST_COMPANY_SELECT( QueryMsg_t * ptrQuery /*VST_COMPANY*/);

	/* VST_SVR_CONF 테이블 관련 쿼리 */
	static int VST_SVR_CONF_INSERT( QueryMsg_t * ptrQuery /*VST_SVR_CONF */);
	static int VST_SVR_CONF_DELETE( QueryMsg_t * ptrQuery /*VST_SVR_CONF */);
	static int VST_SVR_CONF_UPDATE( QueryMsg_t * ptrQuery /*VST_SVR_CONF */);
	static int VST_SVR_CONF_SELECT( QueryMsg_t * ptrQuery /*VST_SVR_CONF */);

	/* VST_LOCATION 테이블 관련 쿼리 */
	static int VST_LOCATION_INSERT( QueryMsg_t * ptrQuery /*VST_LOCATION  */);
	static int VST_LOCATION_DELETE( QueryMsg_t * ptrQuery /*VST_LOCATION  */);
	static int VST_LOCATION_UPDATE( QueryMsg_t * ptrQuery /*VST_LOCATION  */);
	static int VST_LOCATION_UPDATE_INFO( QueryMsg_t * ptrQuery );
	static int VST_LOCATION_SELECT( QueryMsg_t * ptrQuery /*VST_LOCATION  */);

	/* VST_USER 테이블 관련 쿼리 */
	static int VST_USER_INSERT( QueryMsg_t * ptrQuery /*VST_USER   */);
	static int VST_USER_DELETE( QueryMsg_t * ptrQuery /*VST_USER   */);
	static int VST_USER_UPDATE( QueryMsg_t * ptrQuery /*VST_USER   */);
	static int VST_USER_SELECT( QueryMsg_t * ptrQuery /*VST_USER   */);

	/* VST_ERR_CODE 테이블 관련 쿼리 */
	static int VST_ERR_CODE_INSERT( QueryMsg_t * ptrQuery /*VST_ERR_CODE*/);
	static int VST_ERR_CODE_DELETE( QueryMsg_t * ptrQuery /*VST_ERR_CODE*/);
	static int VST_ERR_CODE_UPDATE( QueryMsg_t * ptrQuery /*VST_ERR_CODE*/);
	static int VST_ERR_CODE_SELECT( QueryMsg_t * ptrQuery /*VST_ERR_CODE*/);

	/* VST_SYS_CODE 테이블 관련 쿼리 */
	static int VST_SYS_CODE_INSERT( QueryMsg_t * ptrQuery /*VST_SYS_CODE */);
	static int VST_SYS_CODE_DELETE( QueryMsg_t * ptrQuery /*VST_SYS_CODE */);
	static int VST_SYS_CODE_UPDATE( QueryMsg_t * ptrQuery /*VST_SYS_CODE */);
	static int VST_SYS_CODE_SELECT( QueryMsg_t * ptrQuery /*VST_SYS_CODE */);

	/* VST_VS 테이블 관련 쿼리 */
	static int VST_VS_INSERT( QueryMsg_t * ptrQuery /*VST_VS */);
	static int VST_VS_DELETE( QueryMsg_t * ptrQuery /*VST_VS */);
	static int VST_VS_UPDATE( QueryMsg_t * ptrQuery /*VST_VS */);
	static int VST_VS_SELECT( QueryMsg_t * ptrQuery /*VST_VS */);

	/* VST_KIOSK 테이블 관련 쿼리 */
	static int VST_KIOSK_INSERT( QueryMsg_t * ptrQuery /*VST_KIOSK  */);
	static int VST_KIOSK_DELETE( QueryMsg_t * ptrQuery /*VST_KIOSK  */);
	static int VST_KIOSK_UPDATE( QueryMsg_t * ptrQuery /*VST_KIOSK  */);
	static int VST_KIOSK_SELECT( QueryMsg_t * ptrQuery /*VST_KIOSK  */);

	/* VST_VS_CAR 테이블 관련 쿼리 */
	static int VST_VS_CAR_INSERT( QueryMsg_t * ptrQuery /*VST_VS_CAR */);	
	static int VST_VS_CAR_DELETE( QueryMsg_t * ptrQuery /*VST_VS_CAR */);
	static int VST_VS_CAR_UPDATE( QueryMsg_t * ptrQuery /*VST_VS_CAR */ );	
	static int VST_VS_CAR_SELECT( QueryMsg_t * ptrQuery /*VST_VS_CAR_CONDITION */);
	int VST_VS_CAR_INTERNAL_SELECT( QueryMsg_t * ptrQuery /*VST_VS_CAR */, PreLocation_t * PreLocations, int LotionExistTime, int TimeInterval );	


	/* VST_VS_EVT_LOG 테이블 관련 쿼리 */
	static int VST_VS_EVT_LOG_INSERT( QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */);
	static int VST_VS_EVT_LOG_DATE_DELETE( QueryMsg_t * ptrQuery /*VST_DATE_CONDITION  */);	
	static int VST_VS_EVT_LOG_DELETE_ALL( QueryMsg_t * ptrQuery );
	static int VST_VS_EVT_LOG_SELECT( QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */);
	static int VST_VS_EVT_LOG_DATE_SELECT( QueryMsg_t * ptrQuery /*VST_DATE_CONDITION  */);
	
	/* VST_KIOSK_EVT_LOG 테이블 관련 쿼리 */
	static int VST_KIOSK_EVT_LOG_INSERT( QueryMsg_t * ptrQuery /*(VST_KIOSK_EVT_LOG   */);
	static int VST_KIOSK_EVT_LOG_DATE_DELETE( QueryMsg_t * ptrQuery /*VST_DATE_CONDITION  */);	
	static int VST_KIOSK_EVT_LOG_DELETE_ALL( QueryMsg_t * ptrQuery );
	static int VST_KIOSK_EVT_LOG_SELECT( QueryMsg_t * ptrQuery /*VST_KIOSK_EVT_LOG  */);
	static int VST_KIOSK_EVT_LOG_DATE_SELECT( QueryMsg_t * ptrQuery /*VST_DATE_CONDITION  */);
	

	/* VST_LOGIN_LOG 테이블 관련 쿼리 */
	static int VST_LOGIN_LOG_INSERT( QueryMsg_t * ptrQuery /*VST_LOGIN_LOG   */);
	static int VST_LOGIN_LOG_DELETE( QueryMsg_t * ptrQuery /*VST_LOGIN_LOG   */);
	static int VST_LOGIN_LOG_DELETE_ALL( QueryMsg_t * ptrQuery  );
	static int VST_LOGIN_LOG_UPDATE( QueryMsg_t * ptrQuery /*VST_LOGIN_LOG   */);
	static int VST_LOGIN_LOG_SELECT( QueryMsg_t * ptrQuery /*VST_LOGIN_LOG   */);
	static int VST_LOGIN_LOG_DATE_SELECT( QueryMsg_t * ptrQuery  /*(VST_DATE_CONDITION */);
	

	/* VST_NETWORK_LOG 테이블 관련 쿼리 */
	static int VST_NETWORK_LOG_INSERT( QueryMsg_t * ptrQuery /*(VST_NETWORK_LOG */);
	static int VST_NETWORK_LOG_DATE_DELETE( QueryMsg_t * ptrQuery /*VST_DATE_CONDITION  */);	
	static int VST_NETWORK_LOG_SELECT( QueryMsg_t * ptrQuery  /*(VST_NETWORK_LOG */);
	static int VST_NETWORK_LOG_DATE_SELECT( QueryMsg_t * ptrQuery  /*(VST_DATE_CONDITION */);
	static int VST_NETWORK_LOG_DELETE_ALL( QueryMsg_t * ptrQuery  );

	/* VST_ERR 테이블 관련 쿼리 */
	static int VST_ERR_INSERT( QueryMsg_t * ptrQuery /*VST_ERR  */);
	static int VST_ERR_DELETE( QueryMsg_t * ptrQuery  /*VST_ERR  */);	
	static int VST_ERR_SELECT( QueryMsg_t * ptrQuery /*VST_ERR  */);

	/* VST_ERR_LOG 테이블 관련 쿼리 */
	static int VST_ERR_LOG_INSERT( QueryMsg_t * ptrQuery /*VST_ERR_LOG */);
	static int VST_ERR_LOG_DATE_DELETE( QueryMsg_t * ptrQuery /*VST_DATE_CONDITION */);	
	static int VST_ERR_LOG_SELECT( QueryMsg_t * ptrQuery /*VST_ERR_LOG */ );
	static int VST_ERR_LOG_DATE_SELECT( QueryMsg_t * ptrQuery /*VST_DATE_CONDITION */);
	static int VST_ERR_LOG_DELETE_ALL( QueryMsg_t * ptrQuery  );
		

	static int VST_MONITORING_INSERT( QueryMsg_t * ptrQuery /*VST_MONITORING */);


	static int VST_VS_EVT_LOG_SELECT_WITH_LOCATION( QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */);
	static int VST_VS_SELECT_WITH_LOCATION( QueryMsg_t * ptrQuery /*VST_VS */);


	/* %, Page 처리를 위한 조회 */
	int INTERNAL_VS_EVT_PAGE_SELECT( BOOL bOnlyCnt , QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */ );
	int INTERNAL_VS_CAR_PAGE_SELECT( BOOL bOnlyCnt , QueryMsg_t * ptrQuery /*VST_VS_CAR_CONDITION */);

	static int VST_VS_EVT_LOG_COUNT_SELECT( QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */);
	static int VST_VS_EVT_LOG_MULTI_SELECT( QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */);

	static int VST_VS_CAR_COUNT_SELECT( QueryMsg_t * ptrQuery /*VST_VS_CAR_CONDITION */);
	static int VST_VS_CAR_MULTI_SELECT( QueryMsg_t * ptrQuery /*VST_VS_CAR_CONDITION */);

	/* MessageController용 등록 함수 */
	static int ClientRecvEtcMsg( ClientRecvMsg_t  * ClientRecvMsg );
	static int ClientRecvImageMsg( ClientRecvMsg_t  * ClientRecvMsg );
	static int ClientRecvCurrentImageMsg( ClientRecvMsg_t  * ClientRecvMsg );

	static int ClientSendEtcMsg( ClientSendMsg_t * ClientSendMsg );

	static int DummyTCPConnect( DummyMsg_t * DummyMsg );
	static int DummyTCPClose( DummyMsg_t * DummyMsg );
	static int DummyClearTimeOut( DummyMsg_t * DummyMsg);

	static int ErrRecvTimeOut( ErrMsg_t * ErrMsg );
	
};

