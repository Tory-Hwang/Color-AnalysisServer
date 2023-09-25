#include "StdAfx.h"
#include "QueryProcessor.h"
#include "SVR_Doc.h"



#define DEFAULT_DATE	L"19700101235959"

CQueryProcessor * pQuery =NULL;
CQueryProcessor::CQueryProcessor(void):
m_DBCmdCon( NULL ),
m_DBReadCon( NULL ),
m_DBManager( NULL ),
m_Controller( NULL ),
m_ServerID( 0 ),
m_bNonblock( 0x00 ),
m_bDBUse( FALSE ),
m_nForceUpdateTime ( 0 )
{
	//pQuery = this;
	
}

CQueryProcessor::~CQueryProcessor(void)
{
	
}

/* =====================================================================
Internal Functions 
=======================================================================*/

void CQueryProcessor::DumpStr( CString &str )
{
#if DEBUG_FILEMANAGER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::QueryProcess : %s \n", str);
	strtxt.ReleaseBuffer();
	OutputDebugString( strtxt );
#endif
}


BOOL CQueryProcessor::ChkDBCon()
{
	
	if ( CheckDBUseState() == FALSE )
	{
		return TRUE;
	}

	if ( m_DBCmdCon == NULL || m_DBCmdCon->GetState() != adStateOpen  || m_DBReadCon == NULL || m_DBReadCon->GetState() != adStateOpen)
	{
		if ( m_DBManager != NULL )
		{
			m_DBManager->OnRecvSubModelMsg( NULL );
		}

		return FALSE;
	}

	return TRUE;
}


BOOL CQueryProcessor::CheckDBUseState()
{	
	return m_bDBUse;
}

BOOL CQueryProcessor::IsSameErrData( QueryMsg_t * ptrQuery )
{
	VST_ERR * ErrTable = NULL;
	int Cnt =0;

	if ( VST_ERR_SELECT( ptrQuery) >= 0 )
	{
		ErrTable = (VST_ERR * )ptrQuery->ptrData;
		Cnt = m_ADO.GetRecCount( ErrTable->RS);

		if ( Cnt > 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}


int CQueryProcessor::InsertNetworkTable( short Msg, TCHAR NetworkState,  DummyMsg_t * DummyMsg )
{
	QueryMsg_t QueryMsg;
	VST_NETWORK_LOG VSNetTalbe;	
	int Rect = -1;

	QueryMsg.Msg = QUERY_NETWORK_LOG_INSERT;
	QueryMsg.DataSize = sizeof(VST_NETWORK_LOG );
	QueryMsg.ptrData = &VSNetTalbe;

	VSNetTalbe.cSYSCode = DummyMsg->SysCode;
	VSNetTalbe.cConnectState = NetworkState;
	VSNetTalbe.nID = DummyMsg->ClientID;
	VSNetTalbe.nServerID = m_ServerID;
	VSNetTalbe.btMessageType = Msg;

	Rect = VST_NETWORK_LOG_INSERT( &QueryMsg );

	
	return Rect;
}

int CQueryProcessor::InsertErrTable( TCHAR * pErrcode, int Size, ErrMsg_t * ErrMsg)
{
	QueryMsg_t QueryMsg;
	VST_ERR ErrTable;
	VST_ERR_LOG ErrLogTable;	
	( void )Size;

	memset( &QueryMsg, 0, sizeof( QueryMsg_t ));
	memset( &ErrTable, 0, sizeof( ErrTable ));

	QueryMsg.Msg = QUERY_ERR_INSERT;
	QueryMsg.DataSize = sizeof( VST_ERR );
	QueryMsg.ptrData = &ErrTable;

	//m_ADO.RefreshRS( m_RS );
	//ErrTable.RS = m_RS;
	ErrTable.RS.CreateInstance(__uuidof(Recordset));

	ErrTable.cSYSCode = ErrMsg->SysCode;
	ErrTable.nServerID = pQuery->m_ServerID;
	ErrTable.nID = ErrMsg->ClientID;	

	/* Err code & Size 대입 */
	wcscpy_s( ErrTable.csErrCode, pErrcode );	
	
	/* 에러 테이블은 동일한 대상에 동일한 에러 정보가 없을때만 등록 */
	if ( IsSameErrData( &QueryMsg ) == FALSE )	
	{		
		VST_ERR_INSERT( &QueryMsg  );		
	}		
	
	memset( &QueryMsg, 0, sizeof( QueryMsg_t ));
	memset( &ErrLogTable, 0, sizeof( ErrLogTable ));
	
	QueryMsg.Msg = QUERY_ERR_LOG_INSERT;
	QueryMsg.DataSize = sizeof( VST_ERR_LOG );
	QueryMsg.ptrData = &ErrLogTable;

	ErrLogTable.cSYSCode = ErrMsg->SysCode;
	ErrLogTable.nServerID = pQuery->m_ServerID;
	ErrLogTable.nID = ErrMsg->ClientID;
	/* Err code & Size 대입 */
	wcscpy_s( ErrLogTable.csErrCode, pErrcode ); 

	/* 에러 로그 테이블은 무조건 등록 */
	VST_ERR_LOG_INSERT( &QueryMsg );
	
	m_ADO.CloseRS( ErrTable.RS );
	return 0;
}

int CQueryProcessor::DeleteErrTable( TCHAR * pErrcode, int Size, DummyMsg_t * DummyMsg )
{
	QueryMsg_t QueryMsg;
	VST_ERR ErrTable;
	
	( void )Size;

	memset( &QueryMsg, 0, sizeof( QueryMsg_t ));
	memset( &ErrTable, 0, sizeof( ErrTable ));

	QueryMsg.Msg = QUERY_ERR_DELETE;
	QueryMsg.DataSize = sizeof( VST_ERR );
	QueryMsg.ptrData = &ErrTable;

	//m_ADO.RefreshRS( m_RS );
	ErrTable.RS.CreateInstance(__uuidof(Recordset));	

	ErrTable.cSYSCode = DummyMsg->SysCode;
	ErrTable.nServerID = pQuery->m_ServerID;
	ErrTable.nID = DummyMsg->ClientID;
	
	wcscpy_s( ErrTable.csErrCode, pErrcode ); 	

	/* 존재하는 에러 내용일 경우 삭제 */
	if ( IsSameErrData( &QueryMsg ) == TRUE )
	{		
		VST_ERR_DELETE( &QueryMsg );		
	}
	
	m_ADO.CloseRS(ErrTable.RS );

	return 0;
}


BOOL CQueryProcessor::ParseVSImageMsg( ClientRecvMsg_t * ClientRecvMsg, VSImage_t *VSImage )
{
	(void)ClientRecvMsg;
	(void)VSImage;

	return TRUE;
}

BOOL CQueryProcessor::GetPreLocationCode( TCHAR * CurLocation, PreLocation_t * PreLocation )
{
	QueryMsg_t QueryMsg;
	
	BOOL bResult = TRUE;
	VST_LOCATION VSLocationTable;
	int i, Cnt =0;
	CString strCol;
	_variant_t VAR_strCol;
	TCHAR strCode[ MAX_DB_CODE_SIZE ];

	
	memset( &QueryMsg, 0, sizeof( QueryMsg_t ));
	memset( &VSLocationTable, 0 , sizeof( VST_LOCATION ));
	
	//m_ADO.RefreshRS( m_FindLocation_RS );
	VSLocationTable.RS.CreateInstance(__uuidof(Recordset));

	//VSLocationTable.RS = m_FindLocation_RS;

	QueryMsg.Msg = QUERY_LOCATION_SELECT;
	QueryMsg.DataSize = sizeof(VST_LOCATION );
	QueryMsg.ptrData = &VSLocationTable;

	wcscpy_s( VSLocationTable.csCode, CurLocation );
	
	if ( pQuery->VST_LOCATION_SELECT( &QueryMsg) < 0  )
	{
		CString strMsg;
		strMsg.Format(L" Can not find VST LOCATION  Info ( %s )", CurLocation );
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );
		bResult = FALSE;
	}
	
	if ( bResult )
	{	
		if ( m_ADO.IsEOF( VSLocationTable.RS ) == FALSE )
		{		
			if ( VSLocationTable.RS->GetCollect( L"LOCATION_TYPE" ).vt != VT_NULL )
			{
				PreLocation->LocationType = (int)VSLocationTable.RS->GetCollect( L"LOCATION_TYPE" );
			}
			
			for( i = 0; i < MAX_PRE_LOCATION_CNT ; i++ )
			{
				strCol.Format(L"P%d", i+ 1 );
				VAR_strCol = strCol;

				
				if ( VSLocationTable.RS->GetCollect( VAR_strCol ).vt != VT_NULL )
				{
					memset( strCode, 0, sizeof( strCode ) );
					wcscpy_s(strCode, MAX_LOCATION_SIZE + 1 , (LPWSTR)(_bstr_t)VSLocationTable.RS->GetCollect( VAR_strCol));

					if ( wcslen( strCode ) >5 )
					{
						wcscpy_s( PreLocation->Pre[ Cnt].Code, MAX_LOCATION_SIZE + 1, strCode );
						Cnt++;
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			bResult = FALSE;
		}
	}


	PreLocation->PreCnt = Cnt;
	strCol.ReleaseBuffer();	
	
	m_ADO.CloseRS( VSLocationTable.RS );
	return bResult;	
}

BOOL CQueryProcessor::FindVisionSensor( VSImage_t * VSImage, VST_VS * VSTable )
{
	QueryMsg_t QueryMsg;	
	BOOL bResult = TRUE;
	
	memset( &QueryMsg, 0, sizeof( QueryMsg_t ));

	QueryMsg.Msg = QUERY_VS_SELECT;
	QueryMsg.DataSize = sizeof(VST_VS );
	QueryMsg.ptrData = VSTable;

	VSTable->nID = VSImage->ClientID;
	
	//m_ADO.RefreshRS(  m_FindVS_RS );
	//VSTable->RS = m_FindVS_RS;
	VSTable->RS.CreateInstance(__uuidof(Recordset) );

	if ( pQuery->VST_VS_SELECT( &QueryMsg) < 0  )
	{
		CString strMsg;
		strMsg.Format(L" Can not find VS ID ( %d )", VSImage->ClientID );
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );	
		bResult = FALSE;
	}
	
	if ( bResult )
	{
		if ( m_ADO.IsEOF( VSTable->RS ) == FALSE )
		{
			if ( VSTable->RS->GetCollect( L"LOCATION_CODE" ).vt != VT_NULL )
			{
				wcscpy_s( VSTable->csLocationCode, MAX_LOCATION_SIZE + 1 , (LPWSTR)(_bstr_t)VSTable->RS->GetCollect( L"LOCATION_CODE") );
			}
		}
		else
			bResult = FALSE;
	}


	m_ADO.CloseRS( VSTable->RS );
	return bResult;
}

BOOL CQueryProcessor::InternalVSCarTable( int LocationType, VST_VS_CAR * VSCarTable, TCHAR * DeleteTime )
{
	_variant_t vtDate;
	if ( m_ADO.First( VSCarTable->RS ) )
	{
		if ( m_ADO.IsEOF( VSCarTable->RS ) == FALSE )
		{
			vtDate = VSCarTable->RS->GetCollect( L"CAR_SEE_TIME" );
			if(vtDate.vt != NULL)
			{
				COleDateTime dt(vtDate);
				wsprintf( VSCarTable->csDBCar_SeeTime, L"%04d%02d%02d%02d%02d%02d", 
					dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond());
				/*
					select 시 datetime Formt 사용할 경우 select * 쿼리 한 번 더 호출함 사용 금지
					wcscpy_s( VSCarTable->csFirstInputTime, MAX_TIME_LEN, 
					COleDateTime(VSCarTable->RS->GetCollect( L"FIRST_INPUT_TIME" )).Format(L"%Y%m%d%H%M%S"));
				// wcscpy_s( VSCarTable->csFirstInputTime, MAX_TIME_LEN , (LPWSTR)(_bstr_t)VSCarTable->RS->GetCollect( L"FIRST_INPUT_TIME") );
				*/
			}

			/* 이전 가능 경로를 통과한 챠량 번호가 있을 시 업데이트, 단 현재 구역이 출구이면 삭제 */ 
			if ( LocationType == LOCATION_OUT )
			{
				DeleteVSCarTable( VSCarTable , DeleteTime );
				return TRUE;
			}
			else
			{
				UpdateVSCarTable( VSCarTable );
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CQueryProcessor::ProcVSCarTable( PreLocation_t * PreLocation, VST_VS_CAR * VSCarTable, TCHAR * DeleteTime )
{
	QueryMsg_t QueryMsg;
	
	BOOL bResult = FALSE;
	int LocationType = PreLocation->LocationType;
	int i;
	PreLocation_t PrePreLocation;
	
	
	memset( &QueryMsg, 0, sizeof( QueryMsg_t ));

	QueryMsg.Msg = QUERY_VS_CAR_SELECT;
	QueryMsg.DataSize = sizeof( VST_VS_CAR );
	QueryMsg.ptrData = VSCarTable;

	//m_ADO.RefreshRS(  m_FindCar_RS );
	//VSCarTable->RS = m_FindCar_RS;
	VSCarTable->RS.CreateInstance(__uuidof(Recordset));


	/* 현 구역이 최초 입구 이면 무조건 등록 */
	if ( LocationType == LOCATION_IN )
	{
		/* 현 구역의 이전 진입 가능 구역으로 조회 하여 */
		if ( VST_VS_CAR_INTERNAL_SELECT( &QueryMsg, PreLocation , 30,  0 ) < 0 )
		{	
			m_ADO.CloseRS( VSCarTable->RS );
			return FALSE;
		}
		
		if ( m_ADO.IsEOF( VSCarTable->RS ))
		{
			InsertVSCarTable( VSCarTable );		
		}
		else
		{
			/* 존재하면 삭제 또는 갱신 */
			InternalVSCarTable( LocationType, VSCarTable, DeleteTime );
		}
		m_ADO.CloseRS( VSCarTable->RS );
		return TRUE;
	}

	/* 현 구역의 이전 진입 가능 구역으로 조회 하여 */
	if ( VST_VS_CAR_INTERNAL_SELECT( &QueryMsg, PreLocation ,0, 0 ) < 0 )
	{	
		m_ADO.CloseRS( VSCarTable->RS );
		return FALSE;
	}

	/* 이전 진입 가능 구역에서 동일한 차량 번호를 찾지 못하면*/
	if ( m_ADO.IsEOF( VSCarTable->RS ))
	{		
		for ( i = 0 ; i < PreLocation->PreCnt ; i++ )
		{
			/* 새로운 레코드셋을 생성하고 */
			memset( &PrePreLocation, 0, sizeof( PreLocation_t));
			m_ADO.RefreshRS(  VSCarTable->RS );
			//VSCarTable->RS = m_FindCar_RS;			
			
			if ( GetPreLocationCode( PreLocation->Pre[ i ].Code, &PrePreLocation ) == TRUE )
			{	
				/* 이전의 이전 진입 가능 구역으로 다시 조회해서 */
				if ( VST_VS_CAR_INTERNAL_SELECT( &QueryMsg, &PrePreLocation ,0, 0 ) >= 0 )
				{	
					/* 존재하면 삭제 또는 갱신 */
					if ( m_ADO.IsEOF( VSCarTable->RS ) == FALSE)
					{
						if (InternalVSCarTable( LocationType, VSCarTable, DeleteTime ) == TRUE )
						{
							bResult = TRUE;
							break;
						}
					}
				}
			}
		}

	}
	else
	{
		/* 존재하면 삭제 또는 갱신 */
		if (InternalVSCarTable( LocationType, VSCarTable, DeleteTime ) == TRUE )
		{
			bResult = TRUE;
		}
	}
	
	/* 진입 가능 구간으로 검색이 되지 않았을 때 가능성 낮음*/
	if ( bResult != TRUE )
	{
		m_ADO.RefreshRS( VSCarTable->RS );
		//VSCarTable->RS = m_FindCar_RS;		

		/* VS_CAR_TIMEINTERVAL 시간 이전에 인식된 차량이 있는지 조회 */
		if ( pQuery->VST_VS_CAR_INTERNAL_SELECT( &QueryMsg, PreLocation , 0, pQuery->m_nForceUpdateTime ) < 0  )
		{
			m_ADO.CloseRS( VSCarTable->RS );
			return FALSE;
		}

		/* VS_CAR_TIMEINTERVAL분 이전에 동일한 차량 번호의 차량 정보가 없다면 */
		if ( m_ADO.IsEOF( VSCarTable->RS ))
		{
			if ( LocationType != LOCATION_OUT )
			{
				/* 처음이자 마지막 인식 구역이 출구가 아니라면 신규 등록 */
				InsertVSCarTable( VSCarTable );
			}
			m_ADO.CloseRS( VSCarTable->RS );
			return TRUE;
		}
		else
		{
			/* 존재하면 삭제 또는 갱신 */
			if (InternalVSCarTable( LocationType, VSCarTable, DeleteTime ) == TRUE )
			{
				bResult = TRUE;			
			}
		}
	}		


	m_ADO.CloseRS( VSCarTable->RS );
	return bResult;
}

BOOL CQueryProcessor::IsOldTimeData( VST_VS_CAR * VSCarTable ) 
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	_RecordsetPtr RS;	
	_variant_t vtDate;
	time_t RecvSeeTime =0;
	time_t LastDBSeeTime=0; 
	BOOL bResult = FALSE;
	BOOL IsSame = FALSE;

	TCHAR strDBTime[ MAX_DB_DATE_SIZE ];

	if ( pQuery->ChkDBCon() == FALSE )
		return -1;

	RS.CreateInstance(__uuidof(Recordset));
	CMD.CreateInstance(__uuidof(Command) );
	
	strCmdText.Format(L" SELECT CAR_SEE_TIME FROM VST_VS_CAR WHERE CAR_NUM = '%s' ORDER BY CAR_SEE_TIME DESC", VSCarTable->csCarNum );
	
	Rect = m_ADO.ExecuteAndReadRow( m_DBReadCon, CMD, strCmdText, adCmdText, RS );
	
	if ( Rect == 0 && m_ADO.IsEOF( RS ) == FALSE )
	{
		if ( m_ADO.First( RS ))
		{
			vtDate = RS->GetCollect( L"CAR_SEE_TIME" );
			if(vtDate.vt != NULL)
			{
				COleDateTime dt(vtDate);
				
				memset( strDBTime, 0, sizeof( strDBTime ));
				wsprintf( strDBTime, L"%04d%02d%02d%02d%02d%02d", dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond());
				bResult = TRUE;
			}			
		}
	}

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	pQuery->m_ADO.CloseRS( RS );

	if ( bResult == TRUE )
	{
		m_Util.ConvertWStringToTime( VSCarTable->csSeeTime , &RecvSeeTime );
		m_Util.ConvertWStringToTime( strDBTime, &LastDBSeeTime );

		/* 현재 받은 인식 시간이 DB에 등록된 차량의 최종 인식 시간 보다 작을 경우 갱신 하지 않는다 */
		if ( LastDBSeeTime != 0 && ( RecvSeeTime <= LastDBSeeTime ))
		{
			if ( RecvSeeTime == LastDBSeeTime )
				IsSame = TRUE;

			InsertErrDataWithOldTime( VSCarTable, strDBTime , IsSame );
			return TRUE;
		}
	}	

	return FALSE;
}

int CQueryProcessor::InsertErrDataWithOldTime( VST_VS_CAR * VSCarTable, TCHAR * csLastDBSeeTime , BOOL IsSame )
{
	QueryMsg_t QueryMsg;
	VST_ERR_LOG ErrLogTable;

	memset( &QueryMsg, 0, sizeof( QueryMsg_t));
	memset( &ErrLogTable, 0, sizeof( VST_ERR_LOG ));

	QueryMsg.Msg = QUERY_ERR_LOG_INSERT;
	QueryMsg.DataSize = sizeof( VST_ERR_LOG );
	QueryMsg.ptrData = &ErrLogTable;

	ErrLogTable.cSYSCode = 'V';
	ErrLogTable.nServerID = pQuery->m_ServerID;
	ErrLogTable.nID = VSCarTable->nID;

	if ( IsSame == TRUE )
	{
		wcscpy_s( ErrLogTable.csErrCode, L"ETD0" ); 
	}
	else
	{
		wcscpy_s( ErrLogTable.csErrCode, L"ETD1" ); 
	}

	wsprintf( ErrLogTable.csInfo, L"VSID(%d),CarNum(%s),RecvTime(%s),DBTime(%s)", VSCarTable->nID, VSCarTable->csCarNum, VSCarTable->csSeeTime, csLastDBSeeTime );
	VST_ERR_LOG_INSERT( &QueryMsg );

	return 0;
}

int CQueryProcessor::InsertVSCarTable( VST_VS_CAR * VSCarTable )
{
	QueryMsg_t QueryMsg;
	BOOL bResult = TRUE;

	memset( &QueryMsg, 0, sizeof( QueryMsg_t ));

	/*차량 인식 시간이 db에 등록된 차량 인식 시간 보다 작으면 예외 처리 함 
	  2010-08-31 
	*/
	if ( IsOldTimeData( VSCarTable ) == TRUE )
	{
		return 1;
	}

	QueryMsg.Msg = QUERY_VS_CAR_INSERT;
	QueryMsg.DataSize = sizeof( VST_VS_CAR );
	QueryMsg.ptrData = VSCarTable;
	
	if ( pQuery->VST_VS_CAR_INSERT( &QueryMsg) < 0  )
	{		
		bResult = FALSE;
	}

	return bResult;
}


int CQueryProcessor::UpdateVSCarTable( VST_VS_CAR * VSCarTable )
{
	QueryMsg_t QueryMsg;
	BOOL bResult = TRUE;
	BOOL IsSame = FALSE;

	time_t RecvSeeTime =0;
	time_t LastDBSeeTime=0; 

	memset( &QueryMsg, 0, sizeof( QueryMsg_t ));

	m_Util.ConvertWStringToTime( VSCarTable->csSeeTime , &RecvSeeTime );
	m_Util.ConvertWStringToTime( VSCarTable->csDBCar_SeeTime, &LastDBSeeTime );

	/* 현재 받은 인식 시간이 DB에 등록된 차량의 최종 인식 시간 보다 작을 경우 갱신 하지 않는다 
	   2010-08-31 
	*/
	if ( LastDBSeeTime != 0 && ( RecvSeeTime <= LastDBSeeTime ))
	{
		if ( RecvSeeTime == LastDBSeeTime )
			IsSame = TRUE;

		InsertErrDataWithOldTime( VSCarTable, VSCarTable->csDBCar_SeeTime, IsSame );
		return -1;
	}

	QueryMsg.Msg = QUERY_VS_CAR_UPDATE;
	QueryMsg.DataSize = sizeof( VST_VS_CAR );
	QueryMsg.ptrData = VSCarTable;	

	if ( pQuery->VST_VS_CAR_UPDATE( &QueryMsg) < 0  )
	{		
		bResult = FALSE;
	}	

	return bResult;
}

int CQueryProcessor::DeleteVSCarTable( VST_VS_CAR * VSCarTable, TCHAR * DelteTime )
{
	QueryMsg_t QueryMsg;
	BOOL bResult = TRUE;

	memset( &QueryMsg, 0, sizeof( QueryMsg_t ));

	QueryMsg.Msg = QUERY_VS_CAR_DELETE;
	QueryMsg.DataSize = sizeof( VST_VS_CAR );
	QueryMsg.ptrData = VSCarTable;

	if ( pQuery->VST_VS_CAR_DELETE( &QueryMsg) < 0  )
	{		
		bResult = FALSE;
	}
	else
	{
		wcscpy_s( DelteTime, 6, L"1900");
	}

	return bResult;

}

/*====================================================================================================*/
/* VST_COMPANY 테이블 관련 쿼리 */
int CQueryProcessor::VST_COMPANY_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;	
	CString strCmdText;	
	_CommandPtr CMD;

	VST_COMPANY * stTable = NULL;

	stTable = ( VST_COMPANY *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );

	strCmdText.Format(L"INSERT INTO VST_COMPANY ( ID, NAME, TEL ) VALUES (%d, '%s', '%s')", stTable->nID, stTable->csName, stTable->csTel);	
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;	
}

int CQueryProcessor::VST_COMPANY_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	VST_COMPANY * stTable = NULL;
	_CommandPtr CMD;

	stTable = ( VST_COMPANY *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_COMPANY  WHERE ID = %d", stTable->nID);		
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_COMPANY_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix;
	CString strWhere;
	VST_COMPANY * stTable = NULL;
	_CommandPtr CMD;

	stTable = ( VST_COMPANY *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"UPDATE VST_COMPANY SET NAME = '%s', TEL = '%s'", stTable->csName, stTable->csTel);
	strWhere.Format(L" WHERE ID = '%d'", stTable->nID);	
	strCmdText.Format(L"%s %s", strPrefix, strWhere);	
	
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	
	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_COMPANY_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix, strCol; 
	CString strWhere;
	VST_COMPANY * stTable = NULL;
	_CommandPtr CMD;

	stTable = ( VST_COMPANY *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_COMPANY" );	
	strWhere.Format(L" WHERE 1 = 1 " );

	if(stTable->nID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND ID = %d", stTable->nID);		
		strWhere += strCol;	
	}

	if(wcsnlen(stTable->csName, sizeof( stTable->csName )) > 0)
	{		
		strCol.Empty();
		strCol.Format(L" AND NAME = '%s'", stTable->csName);
		strWhere += strCol;
	}
	
	if( wcsnlen(stTable->csTel, sizeof( stTable->csTel )) > 0)
	{
		strCol.Empty();
		strCol.Format(L"AND TEL = '%s'", stTable->csTel );		
		strWhere += strCol;
	}
	
	strCmdText.Format(L"%s %s", strPrefix, strWhere);	
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );	
	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

/* VST_SVR_CONF 테이블 관련 쿼리 */
int CQueryProcessor::VST_SVR_CONF_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_SVR_CONF * stTable = NULL;

	stTable = ( VST_SVR_CONF *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_SVR_CONF( ID, VIEW_CNT, TCP_IP, TCP_PORT, RUN_TYPE, COMPANY_ID ) VALUES (%d, %d, '%s', %d, %d, %d )" , 
					       stTable->nID, stTable->nViewCnt, stTable->csTCPIP, stTable->nTCPPort, stTable->nRunType, stTable->nCompanyID);
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");

	
	
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_SVR_CONF_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;		
	_CommandPtr CMD;
	VST_SVR_CONF * stTable = NULL;

	stTable = ( VST_SVR_CONF *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_SVR_CONF  WHERE ID = %d ", stTable->nID);
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	
	
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_SVR_CONF_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix;
	CString strWhere;
	_CommandPtr CMD;
	VST_SVR_CONF * stTable = NULL;

	stTable = ( VST_SVR_CONF *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"UPDATE VST_SVR_CONF SET VIEW_CNT = %d, TCP_IP = '%s', TCP_PORT = %d, RUN_TYPE = %d, COMPANY_ID = %d" ,
		               stTable->nViewCnt, stTable->csTCPIP, stTable->nTCPPort, stTable->nRunType, stTable->nCompanyID );
	strWhere.Format(L" WHERE ID = %d", stTable->nID);
	strCmdText.Format(L"%s %s", strPrefix, strWhere );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();	
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_SVR_CONF_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strCol ;
	CString strWhere;
	VST_SVR_CONF * stTable = NULL;

	stTable = ( VST_SVR_CONF *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_SVR_CONF" );	
	strWhere.Format(L" WHERE 1 = 1 " );

	if( stTable->nID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND ID = %d", stTable->nID);		
		strWhere += strCol;	
	}

	if( stTable->nCompanyID > 0)
	{		
		strCol.Empty();
		strCol.Format(L" AND COMPANY_ID = %d", stTable->nCompanyID);
		strWhere += strCol;
	}
	
	if( wcsnlen(stTable->csTCPIP, sizeof( stTable->csTCPIP ) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND TCP_IP = '%s'", stTable->csTCPIP );
		strWhere += strCol;
	}

	if(stTable->nTCPPort > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND TCP_PORT = %d", stTable->nTCPPort);
		strWhere += strCol;
	}

	if(stTable->nRunType > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND RUN_TYPE = %d", stTable->nRunType );
		strWhere += strCol;
	}
	
	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );

	
	
	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

/* VST_LOCATION 테이블 관련 쿼리 */
int CQueryProcessor::VST_LOCATION_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_LOCATION * stTable = NULL;

	stTable = ( VST_LOCATION *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.GetBuffer( 1024 );
	strCmdText.Format(L"INSERT INTO VST_LOCATION( CODE, FLOOR, ZONE, SIDE, MAX_PARKING_CNT, CUR_PARKING_CNT, LOCATION_TYPE, P1, P2, P3, P4, P5, P6,P7,P8,P9,P10, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, INFO)  \
					   VALUES ('%s','%s','%s', '%s', %d, %d, %d, '%s','%s','%s','%s','%s','%s' )" , 
					 stTable->csCode, stTable->csFloor, stTable->csZone, stTable->csSide, stTable->nMaxParkingCnt, stTable->nCurParkingCnt, stTable->nLocationType, 
					 stTable->csPreCode1, stTable->csPreCode2, stTable->csPreCode3, stTable->csPreCode4, stTable->csPreCode5, stTable->csPreCode6,stTable->csPreCode7,stTable->csPreCode8,stTable->csPreCode9,stTable->csPreCode10,
					 stTable->csActive1, stTable->csActive2,stTable->csActive3,stTable->csActive4,stTable->csActive5,stTable->csActive6,stTable->csActive7,stTable->csActive8,stTable->csActive9,stTable->csActive10,stTable->csInfo );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_LOCATION_DELETE( QueryMsg_t * ptrQuery)
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_LOCATION * stTable = NULL;

	stTable = ( VST_LOCATION *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L" DELETE FROM VST_LOCATION WHERE CODE = '%s'" ,stTable->csCode );
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_LOCATION_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix;
	CString strWhere;
	_CommandPtr CMD;
	
	VST_LOCATION * stTable = NULL;

	stTable = ( VST_LOCATION *  )ptrQuery->ptrData;


	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"UPDATE VST_LOCATION SET FLOOR = '%s', ZONE = '%s', SIDE = '%s', \
						MAX_PARKING_CNT = %d, CUR_PARKING_CNT = %d, LOCATION_TYPE = %d, P1 = '%s', P2 = '%s', P3 = '%s', P4 = '%s', P5 = '%s',P6 = '%s',P7 = '%s',P8 = '%s',P9 = '%s',P10 = '%s', A1 = '%s',A2 = '%s',A3 = '%s',A4 = '%s',A5 = '%s',A6 = '%s',A7 = '%s',A8 = '%s',A9 = '%s',A10 = '%s',INFO = '%s'", 
						stTable->csFloor, stTable->csZone, stTable->csSide, stTable->nMaxParkingCnt, stTable->nCurParkingCnt, stTable->nLocationType, 
						stTable->csPreCode1, stTable->csPreCode2, stTable->csPreCode3, stTable->csPreCode4, stTable->csPreCode5, stTable->csPreCode6,stTable->csPreCode7,stTable->csPreCode8,stTable->csPreCode9,stTable->csPreCode10,
						stTable->csActive1, stTable->csActive2,stTable->csActive3,stTable->csActive4,stTable->csActive5,stTable->csActive6,stTable->csActive7,stTable->csActive8,stTable->csActive9,stTable->csActive10,stTable->csInfo );

	strWhere.Format(L"WHERE CODE = '%s'", stTable->csCode);
	strCmdText.Format(L"%s %s", strPrefix,strWhere );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");

	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();	
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_LOCATION_UPDATE_INFO( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix;
	CString strWhere;
	_CommandPtr CMD;
	
	VST_LOCATION * stTable = NULL;

	stTable = ( VST_LOCATION *  )ptrQuery->ptrData;


	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"UPDATE VST_LOCATION SET INFO = '%s'", stTable->csInfo);

	strWhere.Format(L"WHERE CODE = '%s'", stTable->csCode);
	strCmdText.Format(L"%s %s", strPrefix,strWhere );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();	
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_LOCATION_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strCol;
	CString strWhere;
	VST_LOCATION * stTable = NULL;

	stTable = ( VST_LOCATION *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	strCmdText.GetBuffer( 1000 );
	strWhere.GetBuffer( 1000 );

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_LOCATION ");
	strWhere.Format(L" WHERE 1=1 ") ;
	

	if(wcsnlen(stTable->csCode, sizeof( stTable->csCode) ) > 0)
	{
		
		strCol.Empty();
		strCol.Format(L" AND CODE = '%s'", stTable->csCode);		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csFloor, sizeof( stTable->csFloor) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND FLOOR = '%s'", stTable->csFloor);
		strWhere += strCol;
	}
	
	if(wcsnlen(stTable->csZone,sizeof( stTable->csZone) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND ZONE = '%s'", stTable->csZone);
		strWhere += strCol;
	}
	
	if(wcsnlen(stTable->csSide, sizeof( stTable->csSide )) > 0)
	{	
		strCol.Empty();
		strCol.Format(L" AND SIDE = '%s'", stTable->csSide);	
		strWhere += strCol;
	}
	
	if( stTable->nMaxParkingCnt > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND MAX_PARKING_CNT = %d", stTable->nMaxParkingCnt);		
		strWhere += strCol;
	}
	
	if(stTable->nCurParkingCnt > 0)
	{	
		strCol.Empty();
		strCol.Format(L" AND CUR_PARKING_CNT = %d ", stTable->nCurParkingCnt);	
		strWhere += strCol;
	}

	if(stTable->nLocationType > 0)
	{	
		strCol.Empty();
		strCol.Format(L" AND LOCATION_TYPE = %d ", stTable->nLocationType);	
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csPreCode1,sizeof( stTable->csPreCode1) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P1 = '%s'", stTable->csPreCode1);
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csPreCode2,sizeof( stTable->csPreCode2) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P2 = '%s'", stTable->csPreCode2);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csPreCode3,sizeof( stTable->csPreCode3) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P3 = '%s'", stTable->csPreCode3);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csPreCode4,sizeof( stTable->csPreCode4) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P4 = '%s'", stTable->csPreCode4);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csPreCode5,sizeof( stTable->csPreCode5) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P5 = '%s'", stTable->csPreCode5);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csPreCode6,sizeof( stTable->csPreCode6) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P6 = '%s'", stTable->csPreCode6);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csPreCode7,sizeof( stTable->csPreCode7) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P7 = '%s'", stTable->csPreCode7);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csPreCode8,sizeof( stTable->csPreCode8) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P8 = '%s'", stTable->csPreCode8);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csPreCode9,sizeof( stTable->csPreCode9) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P9 = '%s'", stTable->csPreCode9);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csPreCode10,sizeof( stTable->csPreCode10) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND P10 = '%s'", stTable->csPreCode10);
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csActive1,sizeof( stTable->csActive1) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A1 = '%s'", stTable->csActive1);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csActive2,sizeof( stTable->csActive2) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A2 = '%s'", stTable->csActive2);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csActive3,sizeof( stTable->csActive3) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A3 = '%s'", stTable->csActive3);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csActive4,sizeof( stTable->csActive4) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A4 = '%s'", stTable->csActive4);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csActive5,sizeof( stTable->csActive5) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A5 = '%s'", stTable->csActive5);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csActive6,sizeof( stTable->csActive6) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A6 = '%s'", stTable->csActive6);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csActive7,sizeof( stTable->csActive7) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A7 = '%s'", stTable->csActive7);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csActive8,sizeof( stTable->csActive8) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A8 = '%s'", stTable->csActive8);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csActive9,sizeof( stTable->csActive9) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A9 = '%s'", stTable->csActive9);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csActive10,sizeof( stTable->csActive10) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A10 = '%s'", stTable->csActive10);
		strWhere += strCol;
	}


	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

/* VST_USER 테이블 관련 쿼리 */
int CQueryProcessor::VST_USER_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_USER * stTable = NULL;

	stTable = ( VST_USER *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_USER( ID, PASSWORD, LEVEL, NAME, TEL, CREATER_ID ) VALUES ('%s', '%s', %d, '%s', '%s', '%s')", 
		stTable->csID, stTable->csPassword, stTable->nLevel, stTable->csName, stTable->csTel, stTable->csCreaterID);

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_USER_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_USER * stTable = NULL;

	stTable = ( VST_USER *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_USER WHERE ID = '%s'", stTable->csID );
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");

	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_USER_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix;
	CString strWhere;
	VST_USER * stTable = NULL;

	stTable = ( VST_USER *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"UPDATE VST_USER SET PASSWORD = '%s', LEVEL = %d, NAME = '%s', TEL = '%s', CREATER_ID = '%s'", 
		stTable->csPassword, stTable->nLevel, stTable->csName, stTable->csTel, stTable->csCreaterID);
	
	strWhere.Format(L"WHERE ID = '%s'", stTable->csID);
	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");

	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();	
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_USER_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	CString strCol;
	VST_USER * stTable = NULL;

	stTable = ( VST_USER *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_USER " );
	strWhere.Format(L" WHERE 1=1 ");
	
	if(wcsnlen(stTable->csID, sizeof( stTable->csID ) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND ID = '%s'", stTable->csID);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csPassword, sizeof( stTable->csPassword) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND PASSWORD = '%s'", stTable->csPassword);
		strWhere += strCol;
	}
	if(stTable->nLevel > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND LEVEL = %d", stTable->nLevel);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csName, sizeof( stTable->csName) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND NAME = '%s'", stTable->csName);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csTel, sizeof( stTable->csTel) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND TEL = '%s'", stTable->csTel);		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csCreaterID, sizeof( stTable->csCreaterID ) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND CREATER_ID = '%s'", stTable->csCreaterID);		
		strWhere += strCol;
	}
	
	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

/* VST_ERR_CODE 테이블 관련 쿼리 */
int CQueryProcessor::VST_ERR_CODE_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_ERR_CODE * stTable = NULL;

	stTable = ( VST_ERR_CODE *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_ERR_CODE( CODE, INFO )  VALUES ('%s', '%s')", 
		stTable->csCode, stTable->csInfo);

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_ERR_CODE_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_ERR_CODE * stTable = NULL;

	stTable = ( VST_ERR_CODE *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_ERR_CODE WHERE CODE ='%s'", stTable->csCode );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_ERR_CODE_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_ERR_CODE * stTable = NULL;

	stTable = ( VST_ERR_CODE *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"UPDATE VST_ERR_CODE SET INFO = '%s' WHERE CODE = '%s'", stTable->csInfo , stTable->csCode );
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_ERR_CODE_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	CString strCol;
	VST_ERR_CODE * stTable = NULL;

	stTable = ( VST_ERR_CODE *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_ERR_CODE"  );
	strWhere.Format(L" WHERE 1=1 ");

	if(wcsnlen( stTable->csCode, sizeof( stTable->csCode ) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND CODE = '%s'", stTable->csCode );		
		strWhere += strCol;
	}
	
	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

/* VST_SYS_CODE 테이블 관련 쿼리 */
int CQueryProcessor::VST_SYS_CODE_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_SYS_CODE * stTable = NULL;

	stTable = ( VST_SYS_CODE *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_SYS_CODE( CODE, INFO )  VALUES ('%c', '%s')", stTable->cCode, stTable->csInfo);
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");

	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_SYS_CODE_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_SYS_CODE * stTable = NULL;

	stTable = ( VST_SYS_CODE *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_SYS_CODE WHERE CODE = '%c'", stTable->cCode );
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");

	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_SYS_CODE_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_SYS_CODE * stTable = NULL;

	stTable = ( VST_SYS_CODE *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"UPDATE VST_SYS_CODE SET INFO = '%s' WHERE CODE = '%c'" , stTable->csInfo, stTable->cCode );
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");

	

	strCmdText.ReleaseBuffer();

	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_SYS_CODE_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere, strCol;
	VST_SYS_CODE * stTable = NULL;

	stTable = ( VST_SYS_CODE *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_SYS_CODE" );
	strWhere.Format(L" WHERE 1=1 ");

	if( stTable->cCode != '\x00' )
	{
		strCol.Empty();
		strCol.Format(L" AND CODE = '%c'", stTable->cCode );		
		strWhere += strCol;
	}

	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}

/* VST_VS 테이블 관련 쿼리 */
int CQueryProcessor::VST_VS_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_VS * stTable = NULL;

	stTable = ( VST_VS *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;
	
	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_VS ( ID, SERVER_ID, TYPE, LOCATION_CODE, IP, CREATER_ID ) VALUES( %d, %d, %d, '%s', '%s', '%s')", 
		stTable->nID, stTable->nServerID, stTable->nType,  stTable->csLocationCode, stTable->csIP, stTable->csCreaterID);

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_VS_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_VS * stTable = NULL;

	stTable = ( VST_VS *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_VS WHERE ID = %d ", stTable->nID );
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_VS_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_VS * stTable = NULL;

	stTable = ( VST_VS *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"UPDATE VST_VS SET SERVER_ID = %d , TYPE = %d, LOCATION_CODE = '%s', IP = '%s', CREATER_ID = '%s'", 
					stTable->nServerID, stTable->nType, stTable->csLocationCode, stTable->csIP, stTable->csCreaterID);	
	strWhere.Format(L" WHERE ID = %d", stTable->nID);
	strCmdText.Format(L"%s %s ", strPrefix, strWhere );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strPrefix.ReleaseBuffer();	
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_VS_SELECT( QueryMsg_t * ptrQuery )
{
 	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	CString strCol;
	VST_VS * stTable = NULL;

	stTable = ( VST_VS *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE/* || stTable == NULL */ )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_VS ");
	strWhere.Format(L" WHERE 1=1 ");

	if(  stTable->nID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND ID = %d", stTable->nID);
		strWhere += strCol;
	}
	if( stTable->nType > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND TYPE = %d ", stTable->nType );
		strWhere += strCol;
	}
	if( stTable->nServerID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND SERVER_ID = %d ", stTable->nServerID );
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csLocationCode, sizeof( stTable->csLocationCode) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND LOCATION_CODE = '%s'", stTable->csLocationCode);
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csIP, sizeof( stTable->csIP ) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND IP = '%s'", stTable->csIP);		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csCreaterID, sizeof( stTable->csCreaterID) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND CREATER_ID = '%s'", stTable->csCreaterID);		
		strWhere += strCol;
	}

	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	strPrefix.ReleaseBuffer();	
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}


/* VST_KIOSK 테이블 관련 쿼리 */
int CQueryProcessor::VST_KIOSK_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_KIOSK * stTable = NULL;

	stTable = ( VST_KIOSK *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_KIOSK( ID, SERVER_ID, LOCATION_CODE, IP,  CPU_RATE, RAM_RATE, CREATER_ID, ) VALUES(%d, %d, '%s', '%s', %d, %d, '%s' )", 
						stTable->nID, stTable->nServerID, stTable->csLocationCode, stTable->csIP, stTable->nCPURate, stTable->nRAMRate, stTable->csCreaterID);

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_KIOSK_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_KIOSK * stTable = NULL;

	stTable = ( VST_KIOSK *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_KIOSK WHERE ID = %d", stTable->nID);
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");

	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_KIOSK_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_KIOSK * stTable = NULL;

	stTable = ( VST_KIOSK *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"UPDATE VST_KIOSK SET SERVER_ID = %d, LOCATION_CODE = '%s', IP = '%s', CREATER_ID = '%s', CPU_RATE = %d, RAM_RATE = %d", 
					stTable->nServerID, stTable->csLocationCode, stTable->csIP, stTable->csCreaterID, stTable->nCPURate, stTable->nRAMRate);
	
	strWhere.Format(L" WHERE ID = %d", stTable->nID);	
	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strPrefix.ReleaseBuffer();		
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_KIOSK_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	CString strCol;
	VST_KIOSK * stTable = NULL;

	stTable = ( VST_KIOSK *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_KIOSK ");
	strWhere.Format(L" WHERE 1=1 ");

	if(  stTable->nID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND ID = %d", stTable->nID);
		strWhere += strCol;
	}
	if(  stTable->nServerID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND SERVER_ID = %d", stTable->nServerID);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csLocationCode, sizeof( stTable->csLocationCode )) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND LOCATION_CODE = '%s'", stTable->csLocationCode);
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csIP, sizeof( stTable->csIP) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND IP = '%s'", stTable->csIP);		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csCreaterID, sizeof( stTable->csCreaterID) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND CREATER_ID = '%s'", stTable->csCreaterID);		
		strWhere += strCol;
	}
	if( stTable->nCPURate > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND CPU_RATE = %d ", stTable->nCPURate );
		strWhere += strCol;
	}
	if( stTable->nRAMRate > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND RAM_RATE = %d ", stTable->nRAMRate );
		strWhere += strCol;
	}

	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );

	

	strPrefix.ReleaseBuffer();	
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

/* VST_VS_CAR 테이블 관련 쿼리 */

int CQueryProcessor::VST_VS_CAR_INSERT( QueryMsg_t * ptrQuery /*VST_VS_CAR */)
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_VS_CAR * stTable = NULL;
	CString strPath;

	stTable = ( VST_VS_CAR *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	if ( wcsnlen( stTable->csIMGPath, sizeof(stTable->csIMGPath)) > 0 )
	{		
		strPath.Format(L"%s", stTable->csIMGPath);
		strPath.Replace(L"\\", L"\\\\");		
	}

	strCmdText.GetBuffer( 1024 );
	strCmdText.Format(L"INSERT INTO VST_VS_CAR( ID, SERVER_ID, CAR_NUM, CAR_SEE_TIME,  CAR_IMG_PATH, VS_LOCATION, FIRST_INPUT_TIME ) \
					   VALUES (%d, %d, '%s', date_format('%s','%%Y%%m%%d%%H%%i%%s'),'%s','%s',date_format('%s','%%Y%%m%%d%%H%%i%%s') )", 
		stTable->nID, stTable->nServerID, stTable->csCarNum, stTable->csSeeTime, strPath, stTable->csVSLocation, stTable->csSeeTime );

	wcscpy_s( stTable->csFirstInputTime, stTable->csSeeTime);

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	strPath.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_VS_CAR_DELETE( QueryMsg_t * ptrQuery /*VST_VS_CAR */)
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_VS_CAR * stTable = NULL;

	stTable = ( VST_VS_CAR *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"DELETE FROM VST_VS_CAR ");		

	strWhere.Format(L" WHERE CAR_NUM = '%s' AND date_format( CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'", 
		stTable->csCarNum, stTable->csFirstInputTime );	

	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strPrefix.ReleaseBuffer();		
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_VS_CAR_UPDATE( QueryMsg_t * ptrQuery /*VST_VS_CAR */ )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_VS_CAR * stTable = NULL;
	CString strPath;

	stTable = ( VST_VS_CAR *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	if ( wcsnlen( stTable->csIMGPath, sizeof(stTable->csIMGPath)) > 0 )
	{		
		strPath.Format(L"%s", stTable->csIMGPath);
		strPath.Replace(L"\\", L"\\\\");		
	}
	
	strCmdText.GetBuffer( 1024 );
	strPrefix.Format(L"UPDATE VST_VS_CAR SET ID = %d, SERVER_ID = %d, CAR_SEE_TIME = date_format('%s', '%%Y%%m%%d%%H%%i%%s'), CAR_IMG_PATH = '%s', VS_LOCATION = '%s' ", 
		              stTable->nID, stTable->nServerID, stTable->csSeeTime, strPath, stTable->csVSLocation );		

	strWhere.Format(L" WHERE CAR_NUM = '%s' AND date_format( CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'", 
		stTable->csCarNum, stTable->csDBCar_SeeTime );	

	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strPath.ReleaseBuffer();
	strPrefix.ReleaseBuffer();		
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}


int CQueryProcessor::VST_VS_CAR_SELECT( QueryMsg_t * ptrQuery /*VST_VS_CAR_CONDITION */)	
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere, strCol;
	VST_VS_CAR_CONDITION * stTable = NULL;

	stTable = ( VST_VS_CAR_CONDITION *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	/*
	strPrefix.Format(L" SELECT A.ID, A.SERVER_ID, A.CAR_NUM, A.CAR_SEE_TIME, A.CAR_IMG_PATH, A.FIRST_INPUT_TIME, A.VS_LOCATION, B.INFO FROM VST_VS_CAR A, VST_LOCATION B WHERE A.VS_LOCATION = B.CODE" );
	if ( wcslen ( stTable->csCarNum ) > 0 && wcslen( stTable->csBeginTime ) == MAX_TIME_SIZE && wcslen( stTable->csEndTime ) == MAX_TIME_SIZE)
	{
		strCol.Format(L" AND A.CAR_NUM = '%s' AND date_format( A.CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' AND '%s' ", stTable->csCarNum, stTable->csBeginTime, stTable->csEndTime );
		strWhere += strCol;

		strCmdText.Format(L"%s %s", strPrefix, strWhere);
		Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );		
	}
	*/

	strPrefix.Format(L" SELECT A.ID, A.SERVER_ID, A.CAR_NUM, CAR_SEE_TIME, A.CAR_IMG_PATH, A.VS_LOCATION, B.INFO FROM VST_VS_CAR A, VST_LOCATION B WHERE A.VS_LOCATION = B.CODE" ); 
	if ( wcslen ( stTable->csCarNum ) > 0 )
	{
		strCol.Format(L" AND A.CAR_NUM = '%s' ORDER BY CAR_SEE_TIME DESC ", stTable->csCarNum );
		strWhere += strCol;

		strCmdText.Format(L"%s %s", strPrefix, strWhere);
		Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );    
	}
	
	strCol.ReleaseBuffer();
	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_VS_CAR_INTERNAL_SELECT( QueryMsg_t * ptrQuery, PreLocation_t * PreLocations, int LotionExistTime, int TimeInterval )
{
	int Rect = -1, Cnt, i ;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere, strCol, strIN, strTime;
	VST_VS_CAR * stTable = NULL;
	TCHAR * ptrLocation = NULL;
	TCHAR strBegin[ MAX_DB_DATE_SIZE ];	

	stTable = ( VST_VS_CAR *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	// strPrefix.Format(L" SELECT date_format( FIRST_INPUT_TIME,'%%Y%%m%%d%%H%%i%%s') AS FIRST_INPUT_TIME FROM VST_VS_CAR WHERE CAR_NUM = '%s' ", stTable->csCarNum );
	strPrefix.Format(L" SELECT CAR_SEE_TIME FROM VST_VS_CAR WHERE CAR_NUM = '%s' ", stTable->csCarNum );

	if ( TimeInterval == 0 )
	{		
		Cnt = PreLocations->PreCnt;

		if ( Cnt > 0 )
		{
			strIN.Format(L" AND VS_LOCATION IN ( ") ;

			for( i = 0 ; i < Cnt ; i++ )
			{
				ptrLocation = ( TCHAR *)&PreLocations->Pre[ i ].Code;
				if ( wcslen( ptrLocation ) > 5 )
				{
					strCol.Format(L" '%s'", ptrLocation );
					if ( i != ( Cnt -1 ) )
					{
						strCol += L", ";
					}
					else 
					{
						strCol += L" ) ";
					}				
				}
				strIN += strCol;
			}

		}

		strWhere += strIN;
		
		if ( LotionExistTime > 0 )
		{
			memset( strBegin, 0, sizeof( strBegin ));		
			m_Util.GetBeforTime( LotionExistTime, strBegin );
			strTime.Format(L" AND date_format( CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", strBegin , stTable->csSeeTime );
			strWhere += strTime;
		}

		strWhere += L" ORDER BY CAR_SEE_TIME DESC ";
	}
	else
	{
		
		memset( strBegin, 0, sizeof( strBegin ));		
		m_Util.GetBeforTime( TimeInterval, strBegin );

		strWhere.Format(L" AND date_format( CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ORDER BY CAR_SEE_TIME DESC", strBegin , stTable->csSeeTime );
	}

	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = m_ADO.ExecuteAndReadRow( m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );

	strIN.ReleaseBuffer();
	strTime.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}


/* VST_VS_EVT_LOG 테이블 관련 쿼리 */
int CQueryProcessor::VST_VS_EVT_LOG_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	CString strPath;
	_CommandPtr CMD;
	VST_VS_EVT_LOG * stTable = NULL;
	char szTime[ MAX_DB_DATE_SIZE  + 1 ];

	stTable = ( VST_VS_EVT_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	if ( wcsnlen( stTable->csSeeTime, sizeof(stTable->csSeeTime)) == 0 )
	{
		wcscpy_s( stTable->csSeeTime, DEFAULT_DATE);
	}

	if ( wcsnlen( stTable->csDeleteTime, sizeof(stTable->csDeleteTime)) == 0 )
	{
		wcscpy_s( stTable->csDeleteTime, DEFAULT_DATE);
	}
	else
	{
		memset( szTime, 0, sizeof( szTime ));
		pQuery->m_Util.GetCurPacketTime( szTime,sizeof( szTime) );
		pQuery->m_Util.McsToWcs( szTime, stTable->csDeleteTime, (int)strlen( szTime ));
	}

	if ( wcsnlen( stTable->csVSLocation, sizeof(stTable->csVSLocation)) == 0 )
	{
		wcscpy_s( stTable->csVSLocation, DEFAULT_DATE);
	}
	if ( wcsnlen( stTable->csFirstInputTime, sizeof(stTable->csFirstInputTime)) == 0 )
	{
		wcscpy_s( stTable->csFirstInputTime, DEFAULT_DATE);
	}

	if ( wcsnlen( stTable->csIMGPath, sizeof(stTable->csIMGPath)) > 0 )
	{		
		strPath.Format(L"%s", stTable->csIMGPath);
		strPath.Replace(L"\\", L"\\\\");
	}

	/* strCmdText Resize Buffer */
	strCmdText.GetBuffer( 1024 );	
	strCmdText.Format(L"INSERT INTO VST_VS_EVT_LOG(ID, SERVER_ID, CAR_NUM, CAR_SEE_TIME, CAR_IMG_PATH, CAR_IMG_LOCATION, VS_LOCATION, FIRST_INPUT_TIME, DELETE_TIME ,CREATE_TIME ) \
					   VALUES (%d, %d,'%s',date_format('%s','%%Y%%m%%d%%H%%i%%s'),'%s',%d, '%s', date_format('%s','%%Y%%m%%d%%H%%i%%s'), date_format('%s','%%Y%%m%%d%%H%%i%%s'), now() )",
						stTable->nID, stTable->nServerID, stTable->csCarNum, stTable->csSeeTime, strPath, stTable->nIMGLocation , stTable->csVSLocation, stTable->csFirstInputTime, stTable->csDeleteTime );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	strPath.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_VS_EVT_LOG_DATE_DELETE( QueryMsg_t * ptrQuery)
{
	int Rect = -1;
	CString strPrefix;
	CString strCmdText;	
	CString strWhere;
	_CommandPtr CMD;
	VST_DATE_CONDITION * stTable = NULL;

	stTable = ( VST_DATE_CONDITION *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"DELETE FROM VST_VS_EVT_LOG");
	if(wcsnlen( stTable->csBeginTime , sizeof(stTable->csBeginTime) ) ==MAX_TIME_SIZE && wcsnlen( stTable->csEndTime , sizeof(stTable->csEndTime) )==MAX_TIME_SIZE)
	{
		strWhere.Format(L" WHERE  SERVER_ID = %d  AND date_format( CREATE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", stTable->nServerID ,stTable->csBeginTime, stTable->csEndTime );
		strCmdText.Format(L"%s %s", strPrefix,strWhere );
		Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	}
	
	strPrefix.ReleaseBuffer();	
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}

#if 0
/* vs event log는 서버 프로그램에서는 갱신 할수 없음 */
int CQueryProcessor::VST_VS_EVT_LOG_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	


	

	return Rect;
}
#endif

int CQueryProcessor::VST_VS_EVT_LOG_SELECT( QueryMsg_t * ptrQuery)
{
	int Rect = -1;
	CString strCmdText;	
	CString strWhere;
	CString strPrefix;
	_CommandPtr CMD;
	VST_VS_EVT_LOG * stTable = NULL;
	CString strCol;

	stTable = ( VST_VS_EVT_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_VS_EVT_LOG WHERE 1=1");

	if(  stTable->nID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND ID = %d", stTable->nID);
		strWhere += strCol;
	}
	if(  stTable->nServerID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND SERVER_ID = %d", stTable->nServerID);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csCarNum, sizeof(stTable->csCarNum ) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND CAR_NUM = '%s'", stTable->csCarNum);		
		strWhere += strCol;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 2010.6.18 수정 백승범
   -> 기간 조건 추가
*/
/*
	if(wcsnlen(stTable->csSeeTime, sizeof(stTable->csSeeTime ) ) == MAX_TIME_SIZE )
	{
		strCol.Empty();
		strCol.Format(L" AND date_format( CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'" , stTable->csSeeTime );		
		strWhere += strCol;
	}
*/
	if(wcsnlen(stTable->csStartTime, sizeof(stTable->csStartTime)) == MAX_TIME_SIZE  &&
		wcsnlen(stTable->csEndTime, sizeof(stTable->csEndTime)) == MAX_TIME_SIZE)
	{
		CString strStartTime, strEndTime;
		strStartTime.Format(L" AND date_format( CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') > '%s'" , stTable->csStartTime );
		strStartTime.ReleaseBuffer();

		strWhere += strStartTime;

		strEndTime.Format(L" AND date_format( CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') < '%s'" , stTable->csEndTime );
		strEndTime.ReleaseBuffer();

		strWhere += strEndTime;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( stTable->nIMGLocation > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND CAR_IMG_LOCATION = %d ", stTable->nIMGLocation );
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csVSLocation, sizeof(stTable->csVSLocation ) ) > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND VS_LOCATION = '%s'" , stTable->csVSLocation );
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csFirstInputTime, sizeof(stTable->csFirstInputTime ) ) == MAX_TIME_SIZE )
	{
		strCol.Empty();
		strCol.Format(L" AND date_format( FIRST_INPUT_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'" , stTable->csFirstInputTime );		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csDeleteTime, sizeof(stTable->csDeleteTime ) ) == MAX_TIME_SIZE )
	{
		strCol.Empty();
		strCol.Format(L" AND date_format( DELETE_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'" , stTable->csDeleteTime );		
		strWhere += strCol;
	}
	
	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	strPrefix.ReleaseBuffer();	
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}


int CQueryProcessor::VST_VS_EVT_LOG_DATE_SELECT( QueryMsg_t * ptrQuery /*VST_DATE_CONDITION  */)
{
	int Rect = -1;
	CString strCmdText;	
	CString strWhere;
	CString strPrefix;
	_CommandPtr CMD;
	VST_DATE_CONDITION * stTable = NULL;

	stTable = ( VST_DATE_CONDITION *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	/* CString 255자 이상 버퍼에 저장 할 수 없음 */
	//strPrefix.Format(L"SELECT  INDEX, ID, CAR_NUM, CAR_SEE_TIME, CAR_IMG_PATH, CAR_IMG_LOCATION, MSG_TYPE, CREATE_TIME FROM VST_VS_EVT_LOG WHERE 1=1");
	strPrefix.Format(L"SELECT * FROM VST_VS_EVT_LOG WHERE 1=1");

	if(wcsnlen( stTable->csBeginTime , sizeof(stTable->csBeginTime ) ) == MAX_TIME_SIZE && wcsnlen( stTable->csEndTime , sizeof(stTable->csEndTime ) ) == MAX_TIME_SIZE)
	{
		strWhere.Format(L" AND SERVER_ID = %d AND  date_format( CREATE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", stTable->nServerID , stTable->csBeginTime, stTable->csEndTime );
	}

	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	strPrefix.ReleaseBuffer();	
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}
/* VST_KIOSK_EVT_LOG 테이블 관련 쿼리 */
int CQueryProcessor::VST_KIOSK_EVT_LOG_INSERT(QueryMsg_t * ptrQuery)
{
	int Rect = -1;
	CString strCmdText;
	_CommandPtr CMD;
	VST_KIOSK_EVT_LOG * stTable = NULL;

	stTable = ( VST_KIOSK_EVT_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_KIOSK_EVT_LOG ( ID, SERVER_ID, CPU_RATE, RAM_RATE, CREATE_TIME ) VALUES(%d, %d, %d, %d, now() )", 
		stTable->nID, stTable->nServerID , stTable->nCPURate, stTable->nRAMRate );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_KIOSK_EVT_LOG_DATE_DELETE(QueryMsg_t * ptrQuery)
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix;
	CString strWhere;
	_CommandPtr CMD;
	
	VST_DATE_CONDITION * stTable = NULL;

	stTable = ( VST_DATE_CONDITION *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"DELETE FROM VST_KIOSK_EVT_LOG" );
	if(wcsnlen( stTable->csBeginTime , sizeof(stTable->csBeginTime ) ) == MAX_TIME_SIZE && wcsnlen( stTable->csEndTime , sizeof(stTable->csEndTime ) ) == MAX_TIME_SIZE)
	{
		strWhere.Format(L" WHERE  SERVER_ID = %d AND date_format( CREATE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", stTable->nServerID , stTable->csBeginTime, stTable->csEndTime );
		strCmdText.Format(L"%s %s ", strPrefix, strWhere );
		Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	
	}	
	

	strPrefix.ReleaseBuffer();	
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}

#if 0
/* kiosk event log는 서버 프로그램에서는 갱신 할수 없음 */
int CQueryProcessor::VST_KIOSK_EVT_LOG_UPDATE(QueryMsg_t * ptrQuery)
{
	int Rect = -1;
	CString strCmdText;	
	

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	


	

	return Rect;
}
#endif

int CQueryProcessor::VST_KIOSK_EVT_LOG_SELECT( QueryMsg_t * ptrQuery)
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix;
	CString strWhere;
	CString strCol;
	_CommandPtr CMD;
	VST_KIOSK_EVT_LOG * stTable = NULL;

	stTable = ( VST_KIOSK_EVT_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_KIOSK_EVT_LOG WHERE 1 = 1" );

	if(  stTable->nID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND ID = %d", stTable->nID);
		strWhere += strCol;
	}
	if ( stTable->nServerID > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND SERVER_ID = %d", stTable->nServerID );
		strWhere += strCol;
	}
	if( stTable->nCPURate > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND CPU_RATE = %d", stTable->nCPURate);		
		strWhere += strCol;
	}
	if( stTable->nRAMRate > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND RAM_RATE = %d", stTable->nRAMRate);		
		strWhere += strCol;
	}
	
	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}


int CQueryProcessor::VST_KIOSK_EVT_LOG_DATE_SELECT(QueryMsg_t * ptrQuery)
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix;
	CString strWhere;
	_CommandPtr CMD;
	
	VST_DATE_CONDITION * stTable = NULL;

	stTable = ( VST_DATE_CONDITION *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_KIOSK_EVT_LOG WHERE 1 = 1" );

	if(wcsnlen( stTable->csBeginTime , sizeof(stTable->csBeginTime ) ) == MAX_TIME_SIZE && wcsnlen( stTable->csEndTime , sizeof(stTable->csEndTime ) ) == MAX_TIME_SIZE)
	{
		strWhere.Format(L" AND SERVER_ID = %d AND  date_format( CREATE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", stTable->nServerID, stTable->csBeginTime, stTable->csEndTime );
	}

	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );


	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}


/* VST_LOGIN_LOG 테이블 관련 쿼리 */
int CQueryProcessor::VST_LOGIN_LOG_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_LOGIN_LOG * stTable = NULL;

	stTable = ( VST_LOGIN_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;	

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_LOGIN_LOG ( USER_ID, SERVER_ID,  SESSION_KEY, LOGIN_TIME) VALUES('%s', %d, %d,  now() )", stTable->csUserID, stTable->nServerID, stTable->nSessionKey );
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_LOGIN_LOG_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix;
	CString strWhere ;
	_CommandPtr CMD;
	
	VST_LOGIN_LOG * stTable = NULL;

	stTable = ( VST_LOGIN_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"DELETE FROM VST_LOGIN_LOG ");

	if ( stTable->nSessionKey > 0 )
	{
		strWhere.Format(L" WHERE SESSION_KEY = %d AND USER_ID = '%s' AND SERVER_ID= %d ", stTable->nSessionKey, stTable->csUserID , stTable->nServerID );		
		strCmdText.Format(L" %s %s ", strPrefix, strWhere );
		Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	}
		
	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_LOGIN_LOG_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_LOGIN_LOG * stTable = NULL;

	stTable =( VST_LOGIN_LOG *  ) ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"UPDATE VST_LOGIN_LOG SET LOGOFF_TIME = now() ");

	if ( stTable->nSessionKey > 0 && wcsnlen( stTable->csUserID , sizeof( stTable->csUserID)) > 0)
	{
		strWhere.Format(L" WHERE SESSION_KEY = %d AND USER_ID = '%s' AND SERVER_ID = %d", stTable->nSessionKey , stTable->csUserID , stTable->nServerID );
		strCmdText.Format(L" %s %s ", strPrefix, strWhere );
		Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	}

	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_LOGIN_LOG_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix, strWhere, strCol;
	_CommandPtr CMD;
	
	VST_LOGIN_LOG * stTable = NULL;

	stTable = ( VST_LOGIN_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_LOGIN_LOG WHERE 1 = 1" );

	if ( stTable->nServerID > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND SERVER_ID = %d", stTable->nServerID );
		strWhere += strCol;
	}

	if ( stTable->nSessionKey > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND SESSION_KEY = %d", stTable->nSessionKey );
		strWhere += strCol;
	}

	if( wcsnlen( stTable->csUserID , sizeof( stTable->csUserID )) > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND USER_ID = '%s'", stTable->csUserID );
		strWhere += strCol;
	}

	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}


int CQueryProcessor::VST_LOGIN_LOG_DATE_SELECT(QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_DATE_CONDITION  * stTable = NULL;

	stTable = ( VST_DATE_CONDITION  *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_LOGIN_LOG " );

	if(wcsnlen( stTable->csBeginTime , sizeof(stTable->csBeginTime ) ) == MAX_TIME_SIZE && wcsnlen( stTable->csEndTime , sizeof(stTable->csEndTime ) ) == MAX_TIME_SIZE)
	{
		strWhere.Format(L" WHERE  SERVER_ID = %d AND date_format( LOGIN_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", stTable->nServerID, stTable->csBeginTime, stTable->csEndTime );		
	}

	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );	
	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}


/* VST_NETWORK_LOG 테이블 관련 쿼리 */ 
int CQueryProcessor::VST_NETWORK_LOG_INSERT(QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_NETWORK_LOG * stTable = NULL;

	stTable = ( VST_NETWORK_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_NETWORK_LOG ( ID, SERVER_ID, SYS_CODE, CON_STATE, MSG_TYPE, CREATE_TIME ) VALUES( %d, %d, '%c', '%c', %d, now() )", 
						 stTable->nID, stTable->nServerID, stTable->cSYSCode,  stTable->cConnectState, stTable->btMessageType );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_NETWORK_LOG_DATE_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix;
	CString strWhere;
	VST_DATE_CONDITION * stTable = NULL;

	stTable = ( VST_DATE_CONDITION *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"DELETE FROM VST_NETWORK_LOG" );
	if(wcsnlen( stTable->csBeginTime , sizeof(stTable->csBeginTime ) ) == MAX_TIME_SIZE && wcsnlen( stTable->csEndTime , sizeof(stTable->csEndTime ) ) == MAX_TIME_SIZE)
	{
		strWhere.Format(L" WHERE  SERVER_ID = %d AND date_format( CREATE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", stTable->nServerID, stTable->csBeginTime, stTable->csEndTime );
		strCmdText.Format(L"%s %s", strPrefix,strWhere );
		Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	
	}

	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
#if 0
int CQueryProcessor::VST_NETWORK_LOG_UPDATE(QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	


	

	return Rect;
}
#endif

int CQueryProcessor::VST_NETWORK_LOG_SELECT(QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere, strCol;
	VST_NETWORK_LOG * stTable = NULL;

	stTable = ( VST_NETWORK_LOG *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_NETWORK_LOG WHERE 1 = 1" );

	if ( stTable->cSYSCode != '\x00' )
	{
		strCol.Empty();
		strCol.Format(L" AND SYS_CODE =' %c'", stTable->cSYSCode );
		strWhere += strCol;
	}
	if ( stTable->nID >0 )
	{
		strCol.Empty();
		strCol.Format(L" AND ID = %d", stTable->nID );
		strWhere += strCol;
	}
	if ( stTable->nServerID > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND SERVER_ID = %d", stTable->nServerID );
		strWhere += strCol;
	}
	if ( stTable->cConnectState != '\x00' )
	{
		strCol.Empty();
		strCol.Format(L" AND CON_STATE = '%c'", stTable->cConnectState );
		strWhere += strCol;
	}
	if ( stTable->btMessageType != 0x00 )
	{
		strCol.Empty();
		strCol.Format(L" AND MSG_TYPE = %d", stTable->cConnectState );
		strWhere += strCol;
	}
	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );	
	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}


int CQueryProcessor::VST_NETWORK_LOG_DATE_SELECT(QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_DATE_CONDITION  * stTable = NULL;

	stTable = ( VST_DATE_CONDITION  *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_NETWORK_LOG " );

	if(wcsnlen( stTable->csBeginTime , sizeof(stTable->csBeginTime ) ) == MAX_TIME_SIZE && wcsnlen( stTable->csEndTime , sizeof(stTable->csEndTime ) ) == MAX_TIME_SIZE)
	{
		strWhere.Format(L" WHERE  SERVER_ID = %d AND date_format( CREATE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", stTable->nServerID, stTable->csBeginTime, stTable->csEndTime );		
	}

	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );	
	

	strPrefix.ReleaseBuffer();	
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}

/* VST_ERR 테이블 관련 쿼리 */
int CQueryProcessor::VST_ERR_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_ERR * stTable = NULL;

	stTable = ( VST_ERR *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_ERR ( ID , SERVER_ID , ERR_CODE, SYS_CODE ) VALUES(  %d, %d, '%s', '%c')", 
		stTable->nID , stTable->nServerID, stTable->csErrCode, stTable->cSYSCode );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}
int CQueryProcessor::VST_ERR_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	CString strPrefix, strWhere;
	_CommandPtr CMD;
	
	VST_ERR * stTable = NULL;

	stTable = ( VST_ERR *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"DELETE  FROM VST_ERR WHERE 1 = 1");
	
	if(wcsnlen( stTable->csErrCode , sizeof(stTable->csErrCode ) ) > 0 &&  stTable->nServerID > 0 && stTable->nID >0 && stTable->cSYSCode !='\x00')
	{
		strWhere.Format(L" AND SERVER_ID = %d AND SYS_CODE = '%c' AND ERR_CODE = '%s' AND ID = %d " , stTable->nServerID, stTable->cSYSCode, stTable->csErrCode , stTable->nID );
		strCmdText.Format(L"%s %s ", strPrefix, strWhere );
		Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	}	

	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
#if 0
int CQueryProcessor::VST_ERR_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	


	

	return Rect;
}
#endif

int CQueryProcessor::VST_ERR_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere, strCol;
	VST_ERR * stTable = NULL;

	stTable = ( VST_ERR *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT *  FROM VST_ERR WHERE 1 = 1");

	if ( stTable->cSYSCode != '\x00' )
	{
		strCol.Empty();
		strCol.Format(L" AND SYS_CODE ='%c'", stTable->cSYSCode );
		strWhere += strCol;
	}
	if ( stTable->nID >0 )
	{
		strCol.Empty();
		strCol.Format(L" AND ID = %d", stTable->nID );
		strWhere += strCol;
	}
	if ( stTable->nServerID > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND SERVER_ID = %d", stTable->nServerID );
		strWhere += strCol;
	}
	if ( wcsnlen( stTable->csErrCode, sizeof( stTable->csErrCode)  ) > 0  )
	{
		strCol.Empty();
		strCol.Format(L" AND ERR_CODE = '%s'", stTable->csErrCode );
		strWhere += strCol;
	}

	strCmdText.Format(L"%s %s ", strPrefix, strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon , CMD, strCmdText, adCmdText, stTable->RS );

	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

/* VST_ERR_LOG 테이블 관련 쿼리 */
int CQueryProcessor::VST_ERR_LOG_INSERT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_ERR_LOG * stTable = NULL;

	stTable = ( VST_ERR_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_ERR_LOG ( ID,  SERVER_ID, ERR_CODE,  SYS_CODE, INFO, CREATE_TIME) VALUES(%d,  %d, '%s','%c', '%s', now() )", 
						stTable->nID, stTable->nServerID , stTable->csErrCode,  stTable->cSYSCode , stTable->csInfo );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");
	

	strCmdText.ReleaseBuffer();

	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}
int CQueryProcessor::VST_ERR_LOG_DATE_DELETE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_DATE_CONDITION * stTable = NULL;

	stTable = ( VST_DATE_CONDITION *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"DELETE  FROM VST_ERR_LOG WHERE 1 = 1");

	if(wcsnlen( stTable->csBeginTime , sizeof(stTable->csBeginTime ) ) == MAX_TIME_SIZE && wcsnlen( stTable->csEndTime , sizeof(stTable->csEndTime ) ) == MAX_TIME_SIZE)
	{
		strWhere.Format(L" AND SERVER_ID = %d AND  date_format( CREATE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", stTable->nServerID, stTable->csBeginTime, stTable->csEndTime );
		strCmdText.Format(L"%s %s", strPrefix,strWhere );
		Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	
	}

	

	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}
#if 0
int CQueryProcessor::VST_ERR_LOG_UPDATE( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	


	

	return Rect;
}
#endif

int CQueryProcessor::VST_ERR_LOG_SELECT( QueryMsg_t * ptrQuery )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_ERR_LOG * stTable = NULL;
	CString strCol;

	stTable = ( VST_ERR_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_ERR_LOG WHERE 1 = 1 ");

	if ( stTable->cSYSCode != '\x00' )
	{
		strCol.Empty();
		strCol.Format(L" AND SYS_CODE ='%c'", stTable->cSYSCode );
		strWhere += strCol;
	}
	if ( stTable->nID >0 )
	{
		strCol.Empty();
		strCol.Format(L" AND ID = %d", stTable->nID );
		strWhere += strCol;
	}
	if ( stTable->nServerID > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND SERVER_ID = %d", stTable->nServerID );
		strWhere += strCol;
	}
	if ( wcsnlen( stTable->csErrCode, sizeof(stTable->csErrCode )) > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND ERR_CODE = '%s'", stTable->csErrCode );
		strWhere += strCol;
	}

	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );	
	

	strPrefix.ReleaseBuffer();
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}

int CQueryProcessor::VST_ERR_LOG_DATE_SELECT( QueryMsg_t * ptrQuery /*VST_DATE_CONDITION */)
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	VST_DATE_CONDITION * stTable = NULL;

	stTable = ( VST_DATE_CONDITION *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT * FROM VST_ERR_LOG ");

	if(wcsnlen( stTable->csBeginTime , sizeof(stTable->csBeginTime ) ) == MAX_TIME_SIZE && wcsnlen( stTable->csEndTime , sizeof(stTable->csEndTime ) ) == MAX_TIME_SIZE)
	{
		strWhere.Format(L" WHERE  SERVER_ID = %d AND date_format( CREATE_TIME, '%%Y%%m%%d%%H%%i%%s' ) BETWEEN '%s' and '%s' ", stTable->nServerID, stTable->csBeginTime, stTable->csEndTime );		
		strCmdText.Format(L" %s %s ", strPrefix, strWhere );
		Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon , CMD, strCmdText, adCmdText, stTable->RS);	
	}

	
	


	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}

int CQueryProcessor::VST_MONITORING_INSERT( QueryMsg_t * ptrQuery /*VST_MONITORING */)
{

	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	VST_MONITORING * stTable = NULL;

	stTable = ( VST_MONITORING *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"INSERT INTO VST_MONITORING_LOG ( SERVER_ID , SYS_CODE, WORK_DESC , CREATE_TIME) VALUES(  %d, '%c', '%s', now() )", 
		stTable->nServerID , stTable->cSYSCode, stTable->csWorkDesc );

	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	
	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}

/* All Delete Function */
int CQueryProcessor::VST_VS_EVT_LOG_DELETE_ALL( QueryMsg_t * ptrQuery  )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	
	( void )ptrQuery;

	if ( pQuery->ChkDBCon() == FALSE  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_VS_EVT_LOG ");
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	
	
	


	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;

}

int CQueryProcessor::VST_KIOSK_EVT_LOG_DELETE_ALL( QueryMsg_t * ptrQuery  )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;

	( void )ptrQuery;

	if ( pQuery->ChkDBCon() == FALSE  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_KIOSK_EVT_LOG ");
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	

	


	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );


	return Rect;
}
int CQueryProcessor::VST_LOGIN_LOG_DELETE_ALL( QueryMsg_t * ptrQuery  )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;

	( void )ptrQuery;

	if ( pQuery->ChkDBCon() == FALSE  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_LOGIN_LOG ");
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	


	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}
int CQueryProcessor::VST_NETWORK_LOG_DELETE_ALL( QueryMsg_t * ptrQuery  )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;

	( void )ptrQuery;
	if ( pQuery->ChkDBCon() == FALSE  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_NETWORK_LOG ");
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	

	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );


	return Rect;
}
int CQueryProcessor::VST_ERR_LOG_DELETE_ALL( QueryMsg_t * ptrQuery  )
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;

	( void )ptrQuery;
	if ( pQuery->ChkDBCon() == FALSE  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strCmdText.Format(L"DELETE FROM VST_ERR_LOG ");
	Rect = pQuery->m_ADO.ExecuteCommand( pQuery->m_DBCmdCon , CMD, strCmdText, adCmdText, L"");	

	

	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;

}

int CQueryProcessor::VST_VS_EVT_LOG_SELECT_WITH_LOCATION( QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */)
{
	int Rect = -1;
	CString strCmdText;	
	CString strWhere;
	CString strPrefix;
	_CommandPtr CMD;
	VST_VS_EVT_LOG * stTable = NULL;
	CString strCol;

	stTable = ( VST_VS_EVT_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;

	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT A.ID, A.SERVER_ID, A.CAR_NUM, A.CAR_SEE_TIME, A.CAR_IMG_PATH, A.CAR_IMG_LOCATION, A.VS_LOCATION, A.FIRST_INPUT_TIME, A.DELETE_TIME, A.CREATE_TIME, B.INFO FROM VST_VS_EVT_LOG A, VST_LOCATION B WHERE 1=1 AND A.VS_LOCATION = B.CODE");

	if(  stTable->nID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.ID = %d", stTable->nID);
		strWhere += strCol;
	}
	if(  stTable->nServerID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.SERVER_ID = %d", stTable->nServerID);
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csCarNum, sizeof(stTable->csCarNum ) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.CAR_NUM = '%s'", stTable->csCarNum);		
		strWhere += strCol;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* 2010.6.18 수정 백승범
	-> 기간 조건 추가
	*/
	/*
	if(wcsnlen(stTable->csSeeTime, sizeof(stTable->csSeeTime ) ) == MAX_TIME_SIZE )
	{
	strCol.Empty();
	strCol.Format(L" AND date_format( CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'" , stTable->csSeeTime );		
	strWhere += strCol;
	}
	*/
	if(wcsnlen(stTable->csStartTime, sizeof(stTable->csStartTime)) == MAX_TIME_SIZE  &&
		wcsnlen(stTable->csEndTime, sizeof(stTable->csEndTime)) == MAX_TIME_SIZE)
	{
		CString strStartTime, strEndTime;
		strStartTime.Format(L" AND date_format( A.CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') > '%s'" , stTable->csStartTime );
		strStartTime.ReleaseBuffer();

		strWhere += strStartTime;

		strEndTime.Format(L" AND date_format( A.CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') < '%s'" , stTable->csEndTime );
		strEndTime.ReleaseBuffer();

		strWhere += strEndTime;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( stTable->nIMGLocation > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.CAR_IMG_LOCATION = %d ", stTable->nIMGLocation );
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csVSLocation, sizeof(stTable->csVSLocation ) ) > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND A.VS_LOCATION = '%s'" , stTable->csVSLocation );
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csFirstInputTime, sizeof(stTable->csFirstInputTime ) ) == MAX_TIME_SIZE )
	{
		strCol.Empty();
		strCol.Format(L" AND date_format( A.FIRST_INPUT_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'" , stTable->csFirstInputTime );		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csDeleteTime, sizeof(stTable->csDeleteTime ) ) == MAX_TIME_SIZE )
	{
		strCol.Empty();
		strCol.Format(L" AND date_format( A.DELETE_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'" , stTable->csDeleteTime );		
		strWhere += strCol;
	}

	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	

	strPrefix.ReleaseBuffer();	
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_VS_SELECT_WITH_LOCATION( QueryMsg_t * ptrQuery /*VST_VS */)
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere;
	CString strCol;
	VST_VS * stTable = NULL;

	stTable = ( VST_VS *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE/* || stTable == NULL */ )
		return -1;

	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.Format(L"SELECT A.ID, A.SERVER_ID, A.TYPE, A.LOCATION_CODE, A.IP, A.CREATER_ID, B.INFO FROM VST_VS A , VST_LOCATION B");
	strWhere.Format(L" WHERE 1=1 AND A.LOCATION_CODE = B.CODE ");

	if(  stTable->nID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.ID = %d", stTable->nID);
		strWhere += strCol;
	}
	if( stTable->nType > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.TYPE = %d ", stTable->nType );
		strWhere += strCol;
	}
	if( stTable->nServerID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.SERVER_ID = %d ", stTable->nServerID );
		strWhere += strCol;
	}
	if(wcsnlen(stTable->csLocationCode, sizeof( stTable->csLocationCode) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.LOCATION_CODE = '%s'", stTable->csLocationCode);
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csIP, sizeof( stTable->csIP ) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.IP = '%s'", stTable->csIP);		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csCreaterID, sizeof( stTable->csCreaterID) ) > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.CREATER_ID = '%s'", stTable->csCreaterID);		
		strWhere += strCol;
	}

	strCmdText.Format(L"%s %s", strPrefix, strWhere);
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );
	
	CMD.CreateInstance(__uuidof(Command) );
	strPrefix.ReleaseBuffer();	
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;

}


int CQueryProcessor::ClientRecvEtcMsg( ClientRecvMsg_t  * ClientRecvMsg )
{	
	DummyMsg_t DummyMsg;

	memset( &DummyMsg, 0, sizeof( DummyMsg ));

	DummyMsg.ClientID = ClientRecvMsg->SIPID;
#if 0
	if ( ClientRecvMsg->Msg == MSG_VS_HEARTBIT )
	{
		pQuery->DeleteErrTable( ERR_RECV_TIMEOUT, ( sizeof( TCHAR ) * MAX_DB_ERR_CODE_SIZE ), &DummyMsg );
	}
#endif
	//pQuery->InsertNetworkTable( ClientRecvMsg->Msg, NET_CONNECTED, &DummyMsg );
	return 0;
}

int CQueryProcessor::ClientRecvCurrentImageMsg( ClientRecvMsg_t  * ClientRecvMsg )
{
	QueryMsg_t QueryMsg;
	VST_NETWORK_LOG VSNetTalbe;	
	int Rect = -1;

	QueryMsg.Msg = QUERY_NETWORK_LOG_INSERT;
	QueryMsg.DataSize = sizeof(VST_NETWORK_LOG );
	QueryMsg.ptrData = &VSNetTalbe;

	VSNetTalbe.cConnectState = NET_CONNECTED;
	VSNetTalbe.nID = ClientRecvMsg->SIPID;
	VSNetTalbe.nServerID = pQuery->m_ServerID;
	VSNetTalbe.btMessageType = ClientRecvMsg->Msg;
#if 0
	Rect = VST_NETWORK_LOG_INSERT( &QueryMsg );
#else
	Rect = 0;
#endif

	return Rect;
}

int  CQueryProcessor::ClientRecvImageMsg( ClientRecvMsg_t  * ClientRecvMsg )
{	
	QueryMsg_t QueryMsg;
	VST_VS_EVT_LOG VSEvtTable;
	VST_VS VSTable;
	VST_VS_CAR VSCarTable;
	VSImage_t VSImage;
	PreLocation_t PreLocation;

	TCHAR strCarNum[ MAX_CAR_NUM_SIZE ];
	TCHAR strSeeTime[ MAX_TIME_LEN ];
	TCHAR strImgPath[ MAX_PATH ];
	TCHAR strFirstInputTime[ MAX_DB_DATE_SIZE ];
	TCHAR strDeleteTime[ MAX_DB_DATE_SIZE ];

	int ImgLocation;	
	int Rect = -1;
	
	memset( &QueryMsg , 0, sizeof( QueryMsg_t ));	
	memset( &VSTable, 0, sizeof( VST_VS));
	memset( &VSCarTable, 0, sizeof( VST_VS_CAR));
	memset( &VSEvtTable, 0, sizeof( VST_VS_EVT_LOG));
	memset( &VSImage, 0, sizeof( VSImage_t));
	memset( &PreLocation, 0, sizeof( PreLocation_t));	

	memset( strCarNum,0, sizeof( strCarNum) );
	memset( strSeeTime,0, sizeof( strSeeTime) );
	memset( strImgPath,0, sizeof( strImgPath) );
	memset( strFirstInputTime,0, sizeof( strFirstInputTime) );
	memset( strDeleteTime,0, sizeof( strDeleteTime) );

	
	/* 차량 인식 이미지 패킷 파서 */
	if ( pQuery->ParseVSImageMsg( ClientRecvMsg, &VSImage ) == FALSE )
	{		
		return -1;
	}
	
	pQuery->m_Util.McsToWcs( VSImage.CarNum, strCarNum , (int)strlen( VSImage.CarNum ) );
	pQuery->m_Util.McsToWcs( VSImage.ImageTime, strSeeTime , (int)strlen( VSImage.ImageTime ) );
	pQuery->m_Util.McsToWcs( VSImage.ImagePath, strImgPath , (int)strlen( VSImage.ImagePath ) );
	ImgLocation = VSImage.ImageLocation;

	/* VS의 정보를 읽어 오고 */	
	VSTable.nID = VSImage.ClientID;	
	if ( pQuery->FindVisionSensor( &VSImage, &VSTable) == TRUE )
	{
		/* VS가 설치된 Location의 이전 진입 가능 경로 검색 */
		if ( pQuery->GetPreLocationCode( VSTable.csLocationCode, &PreLocation ) == TRUE )
		{
			VSCarTable.nServerID = pQuery->m_ServerID;
			VSCarTable.nID = VSImage.ClientID;
			wcscpy_s( VSCarTable.csIMGPath , strImgPath );
			wcscpy_s( VSCarTable.csSeeTime , strSeeTime );
			wcscpy_s( VSCarTable.csVSLocation, VSTable.csLocationCode );
			wcscpy_s( VSCarTable.csCarNum, strCarNum );
			
			/* VS_Car 테이블의 추가 , 업데이트 , 삭제 처리 */
			pQuery->ProcVSCarTable( &PreLocation, &VSCarTable, strDeleteTime);	
		}		
	}
	
	QueryMsg.Msg = QUERY_VS_EVT_LOG_INSERT;
	QueryMsg.DataSize = sizeof(VST_VS_EVT_LOG );
	QueryMsg.ptrData = &VSEvtTable;
	
	VSEvtTable.nID = VSImage.ClientID;
	VSEvtTable.nServerID = pQuery->m_ServerID;
	wcscpy_s( VSEvtTable.csCarNum, strCarNum );
	wcscpy_s( VSEvtTable.csSeeTime, strSeeTime );
	VSEvtTable.nIMGLocation = ImgLocation;
	wcscpy_s( VSEvtTable.csIMGPath, strImgPath );
	wcscpy_s( VSEvtTable.csVSLocation, VSTable.csLocationCode );
	// wcscpy_s( VSEvtTable.csFirstInputTime, VSCarTable.csFirstInputTime );		
	wcscpy_s( VSEvtTable.csFirstInputTime, DEFAULT_DATE);		
	wcscpy_s( VSEvtTable.csDeleteTime, strDeleteTime );

	/* VS_EVT_LOG 적재 */
	if ( wcslen( VSTable.csLocationCode ) >0  )
	{
		Rect= VST_VS_EVT_LOG_INSERT( &QueryMsg );
	}
	
	
	return 0;
}

/*====================== %, Page 처리를 위한 조회 ========================================== */
int CQueryProcessor::INTERNAL_VS_EVT_PAGE_SELECT( BOOL bOnlyCnt , QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */ )
{
	int Rect = -1;
	CString strCmdText;	
	CString strWhere;
	CString strPrefix;
	_CommandPtr CMD;
	VST_VS_EVT_LOG * stTable = NULL;
	CString strCol;
	BOOL bLike = FALSE;
	int i, nLen;

	stTable = ( VST_VS_EVT_LOG *  )ptrQuery->ptrData;
	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;


	CMD.CreateInstance(__uuidof(Command) );

	if ( bOnlyCnt == TRUE )
	{
		strPrefix.Format(L"SELECT COUNT( * ) AS CNT FROM VST_VS_EVT_LOG A, VST_LOCATION B WHERE 1=1 AND A.VS_LOCATION = B.CODE");
	}
	else
	{
		strPrefix.Format(L"SELECT A.ID, A.SERVER_ID, A.CAR_NUM, A.CAR_SEE_TIME, A.CAR_IMG_PATH, A.CAR_IMG_LOCATION, A.VS_LOCATION, A.FIRST_INPUT_TIME, A.DELETE_TIME, A.CREATE_TIME, B.INFO FROM VST_VS_EVT_LOG A, VST_LOCATION B WHERE 1=1 AND A.VS_LOCATION = B.CODE");
	}
	
	if(  stTable->nID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.ID = %d", stTable->nID);
		strWhere += strCol;
	}
	if(  stTable->nServerID > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.SERVER_ID = %d", stTable->nServerID);
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csCarNum, sizeof(stTable->csCarNum ) ) > 0)
	{
		strCol.Empty();
		nLen = wcslen( stTable->csCarNum );

		for( i  = 0 ; i < nLen ; i++ )
		{
			if ( wcsncmp( (wchar_t *)&stTable->csCarNum[ i ],LIKE, 1 ) == 0 )
			{
				bLike = TRUE;
				break;
			}
		}

		if ( bLike == FALSE )
		{
			strCol.Format(L" AND A.CAR_NUM = '%s'", stTable->csCarNum);		
		}
		else
		{
			strCol.Format(L" AND A.CAR_NUM LIKE '%s'", stTable->csCarNum);		
		}		
		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csStartTime, sizeof(stTable->csStartTime)) == MAX_TIME_SIZE  &&
		wcsnlen(stTable->csEndTime, sizeof(stTable->csEndTime)) == MAX_TIME_SIZE)
	{
		CString strStartTime, strEndTime;
		strStartTime.Format(L" AND date_format( A.CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') > '%s'" , stTable->csStartTime );
		strStartTime.ReleaseBuffer();

		strWhere += strStartTime;

		strEndTime.Format(L" AND date_format( A.CAR_SEE_TIME, '%%Y%%m%%d%%H%%i%%s') < '%s'" , stTable->csEndTime );
		strEndTime.ReleaseBuffer();

		strWhere += strEndTime;
	}
	
	if( stTable->nIMGLocation > 0)
	{
		strCol.Empty();
		strCol.Format(L" AND A.CAR_IMG_LOCATION = %d ", stTable->nIMGLocation );
		strWhere += strCol;
	}
	
	if( wcsnlen(stTable->csVSLocation, sizeof(stTable->csVSLocation ) ) > 0 )
	{
		strCol.Empty();
		strCol.Format(L" AND A.VS_LOCATION = '%s'" , stTable->csVSLocation );
		strWhere += strCol;
	}

	if( wcsnlen(stTable->csZone, sizeof(stTable->csZone ) ) > 0  && wcsnlen(stTable->csFloor, sizeof(stTable->csFloor ) ) > 0)
	{
		strCol.Empty();
		if ( wcsncmp( stTable->csZone, NOT_ZONE , 2 ) == 0 )
		{
			strCol.Format(L" AND A.VS_LOCATION LIKE '%s%%'" , stTable->csFloor );
		}
		else
		{
			strCol.Format(L" AND A.VS_LOCATION LIKE '%s-%s%%'" , stTable->csFloor, stTable->csZone );
		}		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csFirstInputTime, sizeof(stTable->csFirstInputTime ) ) == MAX_TIME_SIZE )
	{
		strCol.Empty();
		strCol.Format(L" AND date_format( A.FIRST_INPUT_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'" , stTable->csFirstInputTime );		
		strWhere += strCol;
	}

	if(wcsnlen(stTable->csDeleteTime, sizeof(stTable->csDeleteTime ) ) == MAX_TIME_SIZE )
	{
		strCol.Empty();
		strCol.Format(L" AND date_format( A.DELETE_TIME, '%%Y%%m%%d%%H%%i%%s') = '%s'" , stTable->csDeleteTime );		
		strWhere += strCol;
	}

	if( stTable->nPageStart >= 0 && stTable->nPageCount >0 )
	{
		strCol.Empty();
		strCol.Format(L" LIMIT %d, %d" , stTable->nPageStart, stTable->nPageCount );		
		strWhere += strCol;
	}

	strCmdText.Format(L"%s %s", strPrefix,strWhere );
	Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );


	strPrefix.ReleaseBuffer();	
	strCol.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );

	return Rect;
}

int CQueryProcessor::INTERNAL_VS_CAR_PAGE_SELECT( BOOL bOnlyCnt ,QueryMsg_t * ptrQuery /*VST_VS_CAR_CONDITION */)
{
	int Rect = -1;
	CString strCmdText;	
	_CommandPtr CMD;
	CString strPrefix, strWhere, strCol;
	VST_VS_CAR_CONDITION * stTable = NULL;
	int i, nLen;
	BOOL bLike = FALSE;

	stTable = ( VST_VS_CAR_CONDITION *  )ptrQuery->ptrData;

	if ( pQuery->ChkDBCon() == FALSE || stTable == NULL  )
		return -1;


	CMD.CreateInstance(__uuidof(Command) );
	if ( bOnlyCnt == TRUE )
	{
		strPrefix.Format(L" SELECT COUNT( * ) AS CNT FROM VST_VS_CAR A, VST_LOCATION B WHERE A.VS_LOCATION = B.CODE" ); 
	}
	else
	{
		strPrefix.Format(L" SELECT A.ID, A.SERVER_ID, A.CAR_NUM, CAR_SEE_TIME, A.CAR_IMG_PATH, A.VS_LOCATION, B.INFO FROM VST_VS_CAR A, VST_LOCATION B WHERE A.VS_LOCATION = B.CODE" ); 
	}
	
	if ( wcslen ( stTable->csCarNum ) > 0 )
	{
		strCol.Empty();
		nLen = wcslen( stTable->csCarNum );

		for( i  = 0 ; i < nLen ; i++ )
		{
			if ( wcsncmp( (wchar_t *)&stTable->csCarNum[ i ],LIKE, 1 ) == 0 )
			{
				bLike = TRUE;
				break;
			}
		}

		if ( bLike == FALSE )
		{
			strCol.Format(L" AND A.CAR_NUM = '%s' ORDER BY CAR_SEE_TIME DESC", stTable->csCarNum);		
		}
		else
		{
			strCol.Format(L" AND A.CAR_NUM LIKE '%s' ORDER BY CAR_SEE_TIME DESC ", stTable->csCarNum);		
		}		

		strWhere += strCol;

		if( stTable->nPageStart >= 0 && stTable->nPageCount >0 )
		{
			strCol.Empty();
			strCol.Format(L" LIMIT %d, %d" , stTable->nPageStart, stTable->nPageCount );		
			strWhere += strCol;
		}
		
		strCmdText.Format(L"%s %s", strPrefix, strWhere);
		Rect = pQuery->m_ADO.ExecuteAndReadRow( pQuery->m_DBReadCon, CMD, strCmdText, adCmdText, stTable->RS );    
	}

	strCol.ReleaseBuffer();
	strPrefix.ReleaseBuffer();
	strWhere.ReleaseBuffer();
	strCmdText.ReleaseBuffer();
	pQuery->m_ADO.CloseCMD( CMD );
	return Rect;
}

int CQueryProcessor::VST_VS_EVT_LOG_COUNT_SELECT( QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */)
{
	return pQuery->INTERNAL_VS_EVT_PAGE_SELECT( TRUE, ptrQuery );
}

int CQueryProcessor::VST_VS_EVT_LOG_MULTI_SELECT( QueryMsg_t * ptrQuery /*VST_VS_EVT_LOG */)
{
	return pQuery->INTERNAL_VS_EVT_PAGE_SELECT( FALSE, ptrQuery );
}

int CQueryProcessor::VST_VS_CAR_COUNT_SELECT( QueryMsg_t * ptrQuery /*VST_VS_CAR_CONDITION */)
{
	return pQuery->INTERNAL_VS_CAR_PAGE_SELECT( TRUE, ptrQuery );
}

int CQueryProcessor::VST_VS_CAR_MULTI_SELECT( QueryMsg_t * ptrQuery /*VST_VS_CAR_CONDITION */)
{
	return pQuery->INTERNAL_VS_CAR_PAGE_SELECT( FALSE, ptrQuery );;
}

int CQueryProcessor::ClientSendEtcMsg( ClientSendMsg_t  * ClientSendMsg )
{	
	DummyMsg_t DummyMsg;

	memset( &DummyMsg, 0, sizeof( DummyMsg ));
	DummyMsg.ClientID = ClientSendMsg->SIPID;

	//pQuery->InsertNetworkTable( ClientSendMsg->Msg, NET_CONNECTED, &DummyMsg );
	return 0;
}


int CQueryProcessor::DummyTCPConnect( DummyMsg_t * DummyMsg )
{
	return pQuery->InsertNetworkTable( 0x00, NET_CONNECTED, DummyMsg );
}

int CQueryProcessor::DummyTCPClose( DummyMsg_t * DummyMsg )
{
	ErrMsg_t ErrMsg;

	memset( &ErrMsg, 0, sizeof( ErrMsg_t ));

	ErrMsg.ClientID = DummyMsg->ClientID;
	ErrMsg.SysCode = DummyMsg->SysCode;
	ErrMsg.Msg = ERR_MSG_RECV_TIMEOUT;

	pQuery->InsertErrTable( ERR_RECV_TIMEOUT, ( sizeof( TCHAR ) * MAX_DB_ERR_CODE_SIZE ), &ErrMsg );
	pQuery->InsertNetworkTable( 0x00, NET_DISCONNECTED, DummyMsg );
	return 0;
}

int CQueryProcessor::DummyClearTimeOut( DummyMsg_t * DummyMsg)
{
	return pQuery->DeleteErrTable( ERR_RECV_TIMEOUT, ( sizeof( TCHAR ) * MAX_DB_ERR_CODE_SIZE ), DummyMsg );
}

int CQueryProcessor::ErrRecvTimeOut( ErrMsg_t * ErrMsg )
{
	return pQuery->InsertErrTable( ERR_RECV_TIMEOUT, ( sizeof( TCHAR ) * MAX_DB_ERR_CODE_SIZE ), ErrMsg );
}


/* =====================================================================
Export Functions 
=======================================================================*/
int CQueryProcessor::InitQueryProcess( BYTE bNonblock )
{
	
	/* Query Process 등록 */
	m_MsgController.AddQueryFunctions( QUERY_COMPANY_INSERT , VST_COMPANY_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_COMPANY_DELETE, VST_COMPANY_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_COMPANY_UPDATE, VST_COMPANY_UPDATE );
	m_MsgController.AddQueryFunctions( QUERY_COMPANY_SELECT, VST_COMPANY_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_SVR_CONF_INSERT, VST_SVR_CONF_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_SVR_CONF_DELETE, VST_SVR_CONF_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_SVR_CONF_UPDATE, VST_SVR_CONF_UPDATE );
	m_MsgController.AddQueryFunctions( QUERY_SVR_CONF_SELECT, VST_SVR_CONF_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_LOCATION_INSERT, VST_LOCATION_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_LOCATION_DELETE, VST_LOCATION_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_LOCATION_UPDATE, VST_LOCATION_UPDATE );
	m_MsgController.AddQueryFunctions( QUERY_LOCATION_SELECT, VST_LOCATION_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_USER_INSERT, VST_USER_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_USER_DELETE, VST_USER_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_USER_UPDATE, VST_USER_UPDATE );
	m_MsgController.AddQueryFunctions( QUERY_USER_SELECT, VST_USER_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_ERR_CODE_INSERT, VST_ERR_CODE_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_ERR_CODE_DELETE, VST_ERR_CODE_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_ERR_CODE_UPDATE, VST_ERR_CODE_UPDATE );
	m_MsgController.AddQueryFunctions( QUERY_ERR_CODE_SELECT, VST_ERR_CODE_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_SYS_CODE_INSERT, VST_SYS_CODE_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_SYS_CODE_DELETE, VST_SYS_CODE_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_SYS_CODE_UPDATE, VST_SYS_CODE_UPDATE );
	m_MsgController.AddQueryFunctions( QUERY_SYS_CODE_SELECT, VST_SYS_CODE_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_VS_INSERT, VST_VS_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_VS_DELETE, VST_VS_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_VS_UPDATE, VST_VS_UPDATE );
	m_MsgController.AddQueryFunctions( QUERY_VS_SELECT, VST_VS_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_KIOSK_INSERT, VST_KIOSK_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_KIOSK_DELETE, VST_KIOSK_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_KIOSK_UPDATE, VST_KIOSK_UPDATE );
	m_MsgController.AddQueryFunctions( QUERY_KIOSK_SELECT, VST_KIOSK_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_VS_CAR_INSERT, VST_VS_CAR_INSERT );	
	m_MsgController.AddQueryFunctions( QUERY_VS_CAR_DELETE, VST_VS_CAR_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_VS_CAR_UPDATE, VST_VS_CAR_UPDATE );	
	m_MsgController.AddQueryFunctions( QUERY_VS_CAR_SELECT, VST_VS_CAR_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_VS_EVT_LOG_INSERT, VST_VS_EVT_LOG_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_VS_EVT_LOG_DELETE, VST_VS_EVT_LOG_DATE_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_VS_EVT_LOG_ALL_DEL, VST_VS_EVT_LOG_DELETE_ALL );
	m_MsgController.AddQueryFunctions( QUERY_VS_EVT_LOG_SELECT, VST_VS_EVT_LOG_SELECT );
	m_MsgController.AddQueryFunctions( QUERY_VS_EVT_LOG_DATE_SELECT, VST_VS_EVT_LOG_DATE_SELECT);

	m_MsgController.AddQueryFunctions( QUERY_KIOSK_EVT_LOG_INSERT, VST_KIOSK_EVT_LOG_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_KIOSK_EVT_LOG_DELETE, VST_KIOSK_EVT_LOG_DATE_DELETE );	
	m_MsgController.AddQueryFunctions( QUERY_KIOSK_EVT_LOG_ALL_DEL, VST_KIOSK_EVT_LOG_DELETE_ALL);
	m_MsgController.AddQueryFunctions( QUERY_KIOSK_EVT_LOG_SELECT, VST_KIOSK_EVT_LOG_SELECT );
	m_MsgController.AddQueryFunctions( QUERY_KIOSK_EVT_LOG_DATE_SELECT, VST_KIOSK_EVT_LOG_DATE_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_LOGIN_LOG_INSERT, VST_LOGIN_LOG_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_LOGIN_LOG_DELETE, VST_LOGIN_LOG_DELETE );
	m_MsgController.AddQueryFunctions( QUERY_LOGIN_LOG_ALL_DEL, VST_LOGIN_LOG_DELETE_ALL);
	m_MsgController.AddQueryFunctions( QUERY_LOGIN_LOG_UPDATE, VST_LOGIN_LOG_UPDATE );
	m_MsgController.AddQueryFunctions( QUERY_LOGIN_LOG_SELECT, VST_LOGIN_LOG_SELECT );
	m_MsgController.AddQueryFunctions( QUERY_LOGIN_LOG_DATE_SELECT, VST_LOGIN_LOG_DATE_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_NETWORK_LOG_INSERT, VST_NETWORK_LOG_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_NETWORK_LOG_DELETE, VST_NETWORK_LOG_DATE_DELETE );	
	m_MsgController.AddQueryFunctions( QUERY_NETWORK_LOG_ALL_DEL, VST_NETWORK_LOG_DELETE_ALL );
	m_MsgController.AddQueryFunctions( QUERY_NETWORK_LOG_SELECT, VST_NETWORK_LOG_SELECT );
	m_MsgController.AddQueryFunctions( QUERY_NETWORK_LOG_DATE_SELECT, VST_NETWORK_LOG_DATE_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_ERR_INSERT, VST_ERR_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_ERR_DELETE, VST_ERR_DELETE );	
	m_MsgController.AddQueryFunctions( QUERY_ERR_SELECT, VST_ERR_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_ERR_LOG_INSERT, VST_ERR_LOG_INSERT );
	m_MsgController.AddQueryFunctions( QUERY_ERR_LOG_DELETE, VST_ERR_LOG_DATE_DELETE );	
	m_MsgController.AddQueryFunctions( QUERY_ERR_LOG_ALL_DEL, VST_ERR_LOG_DELETE_ALL );
	m_MsgController.AddQueryFunctions( QUERY_ERR_LOG_SELECT, VST_ERR_LOG_SELECT );
	m_MsgController.AddQueryFunctions( QUERY_ERR_LOG_DATE_SELECT, VST_ERR_LOG_DATE_SELECT );

	m_MsgController.AddQueryFunctions( QUERY_MONITORING_INSERT, VST_MONITORING_INSERT );

	m_MsgController.AddQueryFunctions( QUERY_VS_SELECT_WITH_LOCATION, VST_VS_SELECT_WITH_LOCATION );
	m_MsgController.AddQueryFunctions( QUERY_VS_EVT_LOG_SELECT_WITH_LOCATION, VST_VS_EVT_LOG_SELECT_WITH_LOCATION );
	m_MsgController.AddQueryFunctions( QUERY_LOCATION_UPDATE_INFO, VST_LOCATION_UPDATE_INFO);
	
	/* Page Query */
	m_MsgController.AddQueryFunctions( QUERY_VS_EVT_LOG_COUNT_SELECT, VST_VS_EVT_LOG_COUNT_SELECT);
	m_MsgController.AddQueryFunctions( QUERY_VS_EVT_LOG_MULTI_SELECT, VST_VS_EVT_LOG_MULTI_SELECT);
	
	m_MsgController.AddQueryFunctions( QUERY_VS_CAR_COUNT_SELECT, VST_VS_CAR_COUNT_SELECT);
	m_MsgController.AddQueryFunctions( QUERY_VS_CAR_MULTI_SELECT, VST_VS_CAR_MULTI_SELECT);

	/* Client Recv Message Process */
	m_MsgController.AddClientRecvFunctions( MSG_VS_RESP_STATUS, ClientRecvEtcMsg );	
	m_MsgController.AddClientRecvFunctions( MSG_VS_RESP_BASIC, ClientRecvEtcMsg );	

	m_MsgController.AddClientSendFunctions( MSG_NETWORK_ALL_BYPASS, ClientSendEtcMsg );
	m_MsgController.AddDummyFunctions( DUMMY_MSG_CLOSE_TCP , DummyTCPClose);
	m_MsgController.AddDummyFunctions( DUMMY_MSG_CONNECT_TCP , DummyTCPConnect);
	m_MsgController.AddDummyFunctions( DUMMY_MSG_CLEAR_TIMEOUT , DummyClearTimeOut);
	m_MsgController.AddErrFunctions( ERR_MSG_RECV_TIMEOUT , ErrRecvTimeOut );
	
	m_bNonblock = bNonblock;
	return 0;
}

int CQueryProcessor::SetDBCmdConnecter( _ConnectionPtr DBConnector )
{
	m_DBCmdCon = DBConnector;
	return 0;
}

int CQueryProcessor::SetDBReadConnecter( _ConnectionPtr DBConnector )
{
	m_DBReadCon = DBConnector;
	return 0;
}


int CQueryProcessor::SetDBManager( IModel * ModelHandle )
{
	m_DBManager = ModelHandle;
	return 0;
}

int CQueryProcessor::SetControllerHandler( IController * ControllerHandle  )
{
	m_Controller = ControllerHandle;
	CSVR_Doc * ptrDoc = NULL;
	VSSERVERINFO_t * ptrServerInfo = NULL;

	if ( m_Controller != NULL)
	{		
		ptrDoc = (CSVR_Doc * )m_Controller->GetSVR_DocHandler();
		if ( ptrDoc != NULL )
		{
			ptrServerInfo = ptrDoc->GetServerInfo();
			if ( ptrServerInfo != NULL )
			{
				m_ServerID = ptrServerInfo->nID;
				m_bDBUse = ptrServerInfo->nDBUse;
				m_nForceUpdateTime = ptrServerInfo->nForceUpdateTime * 60;
				return TRUE;
			}
		}
	}

	return 0;
}

int CQueryProcessor::OnRecvMsg( MsgInfo_t * MsgInfo )
{
	int nResult;

	pQuery = ( CQueryProcessor *)m_DBManager->GetSubModelObject( );
	if ( CheckDBUseState() == FALSE )
	{
		return 0;
	}

	nResult = m_MsgController.OnRecvMsg( MsgInfo );	
	return nResult;
}

int CQueryProcessor::Release( void )
{	
	m_Controller = NULL;
	m_MsgController.ReleaseFuncstions();
	return 0;
}
