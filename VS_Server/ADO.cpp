#include "StdAfx.h"
#include "ADO.h"
#include "DB_Def.h"

//static CRITICAL_SECTION _ADOLock = { 0 };
CADO::CADO(void)
{
	InitializeCriticalSection(&_ADOLock);
}

CADO::~CADO(void)
{
	DeleteCriticalSection(&_ADOLock);
}

int CADO::Lock()
{
	EnterCriticalSection(&_ADOLock);
	return 0;
}

int CADO::UnLock()
{
	LeaveCriticalSection(&_ADOLock);
	// TRACE(L"\t\t\t\t\t\t\t\t\t\tUnLockCount : %d\n", _ADOLock.LockCount);
	return 0;
}

void CADO::DumpStr( CString &str )
{

	CString strtxt;
	strtxt.Format(L"VS_SERVER::CADO : %s \n", str);
	strtxt.ReleaseBuffer();
	OutputDebugString( strtxt );

}

void CADO::CloseRS( _RecordsetPtr RS )
{
	if ( RS == NULL )
	{
		CString strMsg;
		strMsg.Format(L"Close Rs but Rs is NULL");
		DumpStr( strMsg );

		return;
	}

	if ( IsOpenRS( RS) == TRUE )
	{
		RS->Close();
		RS.Release();
		RS = NULL;
	}
}

void CADO::RefreshRS( _RecordsetPtr RS )
{
	if ( RS == NULL )
	{
		CString strMsg;
		strMsg.Format(L"RefreshRS but Rs is NULL");
		DumpStr( strMsg );

		return;
	}

	if ( IsOpenRS( RS) == TRUE )
	{
		RS->Close();
		
	}
}


void CADO::CloseCMD( _CommandPtr CMD )
{
	if ( CMD == NULL )
	{
		CString strMsg;
		strMsg.Format(L"Close CMD but CMD is NULL");
		DumpStr( strMsg );
		return;
	}
	CMD.Release();
	CMD = NULL;
}

BOOL CADO::IsOpenDB( _ConnectionPtr DBCon )
{
	if ( DBCon == NULL )
		return FALSE;

	return (( DBCon->GetState() == adStateOpen ) ? TRUE : FALSE);
}

BOOL CADO::IsOpenRS( _RecordsetPtr RS)
{
	if ( RS == NULL )
		return FALSE;

	return ((RS->GetState() == adStateOpen ) ? TRUE : FALSE);
}

BOOL CADO::IsEOF( _RecordsetPtr RS )
{
	if ( (RS == NULL) && (IsOpenRS(RS) == FALSE) )
		return TRUE;

	return RS->AdoEOF;
}

BOOL CADO::Next( _RecordsetPtr RS )
{
	if ( RS == NULL )
		return FALSE;

	return (FAILED(RS->MoveNext()) ? FALSE : TRUE);
}

BOOL CADO::First( _RecordsetPtr RS )
{
	if ( RS == NULL )
		return FALSE;

	return (FAILED(RS->MoveFirst()) ? FALSE : TRUE);
}

int CADO::GetRecCount( _RecordsetPtr RS )
{
	HRESULT hr;
	
	if ( RS == NULL )
		return -1;

	try
	{
		int count = (int)RS->GetRecordCount();
		if (count < 0)
		{
			hr = RS->MoveFirst();
			count = 0;
			while (!RS->AdoEOF) 
			{
				count++;
				RS->MoveNext();
			}
		}

		if ( count > 0 && RS->AdoEOF)
		{
			RS->MoveFirst();
		}

		return count;
	}
	catch (_com_error ex)
	{
		//TRACE(_T("Warning: GetRecordCount ErrorMessage: %s; File: %s; Line: %d\n"), ex.ErrorMessage(), __FILE__, __LINE__);	
	} 

	return -1;
}

int CADO::GetFieldCount( _RecordsetPtr RS )
{
	if ( RS == NULL )
		return -1;

	return (int)RS->Fields->GetCount();
}

void CADO::GetRS(_RecordsetPtr RS, _variant_t x, _bstr_t& ret)
{
	if ( RS == NULL )
		return;

	ret = RS->Fields->Item[x]->Value;
}

void CADO::GetRS(_RecordsetPtr RS, _variant_t x, _variant_t& ret)
{
	if ( RS == NULL )
		return;

	ret = RS->Fields->Item[x]->Value;
}

void CADO::GetRS(_RecordsetPtr RS, _variant_t x, float& ret)
{
	if ( RS == NULL )
		return;

	ret = RS->Fields->Item[x]->Value;
}

void CADO::GetRS(_RecordsetPtr RS, _variant_t x, long& ret)
{
	if ( RS == NULL )
		return;

	ret = RS->Fields->Item[x]->Value;
}

void CADO::GetRS(_RecordsetPtr RS, _variant_t x, double& ret)
{
	if ( RS == NULL )
		return;

	ret = RS->Fields->Item[x]->Value;
}

long CADO::GetParamValue(_CommandPtr CMD, _bstr_t _ParameterName)
{
	return long(CMD->Parameters->Item[_ParameterName]->Value); 
}

void CADO::Add_Parameters(_CommandPtr CMD, _bstr_t _ParameterName,
							DataTypeEnum _SqlDbType,
							int _Size,
							ParameterDirectionEnum _Direction,
							_variant_t _User_Value)
{

	_ParameterPtr AddParameter;
	AddParameter.CreateInstance("ADODB.Parameter");
	AddParameter->Name = _ParameterName;
	AddParameter->Type = _SqlDbType;
	AddParameter->Size = _Size;
	AddParameter->Direction = _Direction;
	CMD->Parameters->Append(AddParameter);
	CMD->Parameters->Item[_ParameterName]->Value = _User_Value;

}

int CADO::ExecuteCommand( _ConnectionPtr DBCon, _CommandPtr CMD, CString & strCmdText, int CmdType, _bstr_t _ParameterName )
{
	int Rect = -1;	
	
	if ( DBCon == NULL || CMD == NULL  || IsOpenDB( DBCon) == FALSE )
	{
		CString strMsg;		
		strMsg.Format(L"ERR ExecuteCommand but CMD is NULL" );
		DumpStr( strMsg );
		return Rect;
	}
	
	CMD->CommandText = _bstr_t( strCmdText );	
	CMD->CommandType = (CommandTypeEnum)CmdType;
	CMD->ActiveConnection = DBCon;	
	
	Lock();
	try
	{
#if 0
		CMD->Execute( NULL, NULL, CMD->CommandType );
#else
		DBCon->Execute( _bstr_t( strCmdText ), NULL, adOptionUnspecified );
#endif
		Rect = 0;
	}
	catch (_com_error ex)
	{
		CString db_Error_Message;
		CString strErr;

		db_Error_Message = ex.ErrorMessage();
		strErr.Format(L"%s...%s", strCmdText, db_Error_Message );
		DumpStr( strErr );
	}
	
	UnLock();
	//Sleep( 1 );
	return Rect;
}


int CADO::ExecuteAndReadRow( _ConnectionPtr DBCon, _CommandPtr CMD, CString & strCmdText, int CmdType, _RecordsetPtr  RS )
{
	int Rect = -1;	

	if ( DBCon == NULL || CMD == NULL || RS == NULL )
	{
		CString strMsg;		
		strMsg.Format(L"ERR ExecuteAndReadRow but RS is NULL" );
		DumpStr( strMsg );
		return Rect;
	}	
	
	Lock();
	try
	{
#if 1
		CMD->CommandText = _bstr_t( strCmdText );	
		CMD->CommandType = (CommandTypeEnum)CmdType;
		CMD->ActiveConnection = DBCon;
		RS->PutRefSource( CMD );

		_variant_t vNull;
		vNull.vt = VT_ERROR;
		vNull.scode = DISP_E_PARAMNOTFOUND;
		RS->CursorLocation = adUseClient;
		RS->Open(vNull, vNull, adOpenStatic, adLockOptimistic, CMD->CommandType);
		Rect = 0;;
#else
		RS->CursorLocation = adUseClient;
		RS= DBCon->Execute( _bstr_t( strCmdText ), NULL, adOptionUnspecified );		
#endif
		Rect = 0;
	}
	catch (_com_error ex)
	{
		CString db_Error_Message;
		CString strErr;


		db_Error_Message = ex.ErrorMessage();
		strErr.Format(L"%s...%s", strCmdText, db_Error_Message );
		DumpStr( strErr );

		if( RS != NULL)
		{
			CloseRS( RS );
		}		
	}
	UnLock();

	return Rect;
}


