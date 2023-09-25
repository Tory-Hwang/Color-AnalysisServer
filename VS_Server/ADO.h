#pragma once

class CADO
{
public:
	CADO(void);
	~CADO(void);
	
	void DumpStr( CString &str );
	void CloseRS( _RecordsetPtr RS );
	void CloseCMD( _CommandPtr CMD );
	void RefreshRS( _RecordsetPtr RS );

	BOOL IsOpenDB( _ConnectionPtr DBCon );
	BOOL IsOpenRS( _RecordsetPtr RS);

	BOOL IsEOF( _RecordsetPtr RS);
	BOOL Next( _RecordsetPtr RS);
	BOOL First( _RecordsetPtr RS );

	int GetRecCount(_RecordsetPtr RS );
	int GetFieldCount(_RecordsetPtr RS );
	
	void GetRS(_RecordsetPtr RS, _variant_t x, _bstr_t& ret);
	void GetRS(_RecordsetPtr RS, _variant_t x, _variant_t& ret);
	void GetRS(_RecordsetPtr RS, _variant_t x, float& ret);
	void GetRS(_RecordsetPtr RS, _variant_t x, long& ret);
	void GetRS(_RecordsetPtr RS, _variant_t x, double& ret);

	long GetParamValue(_CommandPtr CMD, _bstr_t _ParameterName);
	void Add_Parameters(_CommandPtr CMD, _bstr_t _ParameterName, DataTypeEnum _SqlDbType, int _Size,ParameterDirectionEnum _Direction,_variant_t _User_Value);

	int ExecuteCommand( _ConnectionPtr DBCon, _CommandPtr CMD, CString & strCmdText, int CmdType,  _bstr_t _ParameterName );	
	int ExecuteAndReadRow(_ConnectionPtr DBCon, _CommandPtr CMD, CString & strCmdText, int CmdType, _RecordsetPtr  RS );


	int Lock();
	int UnLock();
private:
	CRITICAL_SECTION _ADOLock;

};
