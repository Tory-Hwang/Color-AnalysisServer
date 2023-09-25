#include "stdafx.h"
#include "Img_Proc.h"

#include "./DIP/DIP.h"
#include "./LPR/LPR.h"


CImg_Proc::CImg_Proc(void)
{
}

CImg_Proc::~CImg_Proc(void)
{
}

int CImg_Proc::Proc_Init( void )
{
	LIB_INIT_DATA_t lib_init_data;

	memset( &m_ImageData, 0, sizeof( IMAGE_DATA_t ));
	memset( &m_LPRChars, 0, sizeof( LPR_CHAR_t ));
	memset( m_tcFileName, 0, sizeof( m_tcFileName ));
	memset( &m_Rect, 0, sizeof( m_Rect ));
	memset( &m_Version, 0, sizeof( m_Version ));


	memset( &lib_init_data, 0, sizeof(LIB_INIT_DATA_t));
	memset( &m_Version, 0, sizeof( m_Version));

	m_Version.APP_VER[ 0 ] = 0;
	m_Version.APP_VER[ 1 ] = 0;

	m_WizardHelper.GetCurrentPathChar( lib_init_data.szwinpath );
	SetWinCurPath( lib_init_data.szwinpath );		
	
	DIP_Initialize( &lib_init_data );
	memcpy( m_Version.DIP_VER, lib_init_data.Version, VER_SIZE);

	LPR_Initialize( &lib_init_data );
	memcpy( m_Version.LPR_VER, lib_init_data.Version, VER_SIZE);
	

	return TRUE;
}


int CImg_Proc::Proc_LPR( IMAGE_DATA_t * ptrImageData, LPR_DATA_t *ptrLPRData, IMAGE_DATA_t *ptrOrgImageData, 
						 CAMERA_DATA_t *ptrCameraData, unsigned char  * ptrResult, int Area  )
{
	int nLineByte=0;
	int i, j;	
	IMAGE_DATA_t *ptrCharImage, * ptrTempImageData= NULL;	
	int ret = -1;
	int Cnt = 0;

	if ( ptrImageData == NULL || ptrLPRData == NULL || ptrOrgImageData == NULL || ptrCameraData == NULL )
	{
		AfxMessageBox(L"LPR 버퍼가 없습니다. ");
		return -1;
	}
	if ( ptrImageData->nBitCount != 24 )
	{
		AfxMessageBox(L"24bit 컬러 이미지만 그레이 스케일 처리 할 수 있습니다");
		return -1;
	}

	memset( ptrCameraData, 0, sizeof( CAMERA_DATA_t ));
	memset( ptrLPRData, 0, sizeof( LPR_DATA_t ));
	memset( ptrOrgImageData, 0, sizeof( IMAGE_DATA_t ));		

	ptrTempImageData = ( pIMAGE_DATA)&ptrLPRData->LPRResult[ 0 ].TempImdateData;

	/* 이미지 데이터 복사 */	
	m_WizardHelper.CopyImageData( ptrOrgImageData, ptrImageData , TRUE);
	m_WizardHelper.CopyImageData( ptrTempImageData, ptrImageData , FALSE);	

	/* First DOC 문자 데이터 메모리 해제 */
	for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		m_WizardHelper.FreeImageAllocData( &m_LPRChars.Chars[ i ].ImageData  );
	}	
	
	/* 번호판 개별 문자 Display buffer */
	for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		ptrCharImage = &ptrLPRData->LPRResult[ 0 ].LPRChars.Chars[ i ].ImageData;
		m_WizardHelper.CopyImageDataWithEmpty(ptrCharImage, 24, STRIP_H_SIZE, STRIP_W_SIZE );
	}

	
	ptrCameraData->nArea = Area;
	ptrCameraData->nBitCount = ptrImageData->nBitCount;
	ptrCameraData->nH = ptrImageData->nH;
	ptrCameraData->nW = ptrImageData->nW;
	ptrCameraData->nImageSize = ptrImageData->nImageSize;
	nLineByte = m_WizardHelper.GetCalBmpWidth(ptrImageData->nW, ptrImageData->nBitCount );
	ptrCameraData->ptrSrcImage1D = new BYTE[ ptrImageData->nH * nLineByte ];
	
	if ( ptrCameraData->ptrSrcImage1D == NULL )
	{
		return -1;
	}

	/*Camera Image Copy ( Matrix --> Array )*/
	for( i = 0 ; i < ptrImageData->nH ; i++ )
	{
		for( j = 0 ; j < nLineByte ; j++)
		{
			ptrCameraData->ptrSrcImage1D[ ( i * nLineByte ) + j ] = ptrImageData->ptrSrcImage2D[ i ][ j ];
		}
	}

	ptrLPRData->ptrCameraData = ptrCameraData;
	if ( LPR_LPRProcess( ptrLPRData ) == ERR_SUCCESS )
	{
		/* 영역을 찾지 못하면 nChartCnt가 0으로 넘어옴 */
		Cnt = ptrLPRData->LPRResult[ 0 ].LPRChars.nCharCnt;
		memset( ptrResult, 0,MAX_LPR_CHAR_CNT); 		
#if 1
//		Internal_NewImageDoc( ptrtcFileName, ptrTempImageData, &ptrLPRData->LPRResult[ 0 ].PlateRect[ 0 ], &ptrLPRData->LPRResult[ 0 ].LPRChars );
#else
		Internal_NewImageDoc( ptrtcFileName, ptrOrgImageData, &ptrLPRData->LPRResult[ 0 ].PlateRect[ 0 ] , &ptrLPRData->LPRResult[ 0 ].LPRChars );
#endif
	
	}
	else
	{
		return -1;
	}

	return TRUE;
}

int CImg_Proc::Proc_Release( void )
{

	/* Image Buffer Release */
	m_WizardHelper.FreeImageAllocData( &m_ImageData );
	for( int i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		m_WizardHelper.FreeImageAllocData( &m_LPRChars.Chars[ i ].ImageData );
	}

	DIP_Release();
	LPR_Release();

	return TRUE;
}
