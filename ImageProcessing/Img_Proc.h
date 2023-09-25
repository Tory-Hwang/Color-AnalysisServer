#pragma once


#include "./include/TK_TYPE.h"
#include "./include/TK_CONF.h"
#include "./UTIL/WizardHelper.h"


class CImg_Proc
{
public:
	CImg_Proc(void);
	~CImg_Proc(void);

	int Proc_Init( void );
	int Proc_Release( void );

	int Proc_LPR(IMAGE_DATA_t * ptrImageData, LPR_DATA_t *ptrLPRData, IMAGE_DATA_t *ptrOrgImageData, CAMERA_DATA_t *ptrCameraData, unsigned char * ptrResult, int Area );	
	int Cal_Strip_Level( pLPRCHAR ptrLPRChars, int cnt, unsigned char * ptrResult );

	IMAGE_DATA_t m_ImageData;
	CVRECT_t m_Rect[ MAX_LPR_AREA_CNT ];
	LPR_CHAR_t m_LPRChars;
	TCHAR m_tcFileName[ MAX_PATH ];
	HDC m_hDC;	
	VERSION_t m_Version;

private:
	CWizardHelper m_WizardHelper;	
	pLPR_CONF m_ptrLPRConf;
	pOCR_CONF m_ptrOCRConf;
	pDIP_CONF m_ptrDIPConf;
	pAPP_CONF m_ptrAPPConf;
	

};