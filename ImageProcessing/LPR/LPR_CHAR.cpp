#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"

#include "LPR_DIP.h"
#include "LPR_COMMON.h"
#include "LPR_CHAR.h"

#define PI 3.14159265

STRIP_LEVEL_t m_strip_level[ MAX_LPR_CHAR_CNT  ];

/*=========================================================================
문자 초기화
==========================================================================*/
int CHAR_Initialize( void )
{
	int i = 0;
	memset(&m_strip_level, 0, sizeof( m_strip_level ));


	/* 잠혈 */
	m_strip_level[ i ].hsi[0].fh	= 47.73;	
	m_strip_level[ i ].hsi[0].fs	= 35.19;
	m_strip_level[ i ].hsi[0].fi	= 203.67;

	m_strip_level[ i ].hsi[1].fh	= 0.0;	
	m_strip_level[ i ].hsi[1].fs	= 0.0;
	m_strip_level[ i ].hsi[1].fi	= 0.0;

	m_strip_level[ i ].hsi[2].fh	= 82.73;	
	m_strip_level[ i ].hsi[2].fs	= 30.36;
	m_strip_level[ i ].hsi[2].fi	= 168;

	m_strip_level[ i ].hsi[3].fh	= 123.16;	
	m_strip_level[ i ].hsi[3].fs	= 27.84;
	m_strip_level[ i ].hsi[3].fi	= 12.33;

	m_strip_level[ i ].hsi[4].fh	= 164.39;	
	m_strip_level[ i ].hsi[4].fs	= 64.85;
	m_strip_level[ i ].hsi[4].fi	= 79.67;

	m_strip_level[ i ].hsi[5].fh	= 0.0;	
	m_strip_level[ i ].hsi[5].fs	= 0.0;
	m_strip_level[ i ].hsi[5].fi	= 0.0;

	
	/* 빌리루빈 */
	i +=1;
	m_strip_level[ i ].hsi[0].fh	= 104.92;	
	m_strip_level[ i ].hsi[0].fs	= 4.8;
	m_strip_level[ i ].hsi[0].fi	= 229;

	m_strip_level[ i ].hsi[1].fh	= 0.0;	
	m_strip_level[ i ].hsi[1].fs	= 0.0;
	m_strip_level[ i ].hsi[1].fi	= 0.0;

	m_strip_level[ i ].hsi[2].fh	= 24.01;	
	m_strip_level[ i ].hsi[2].fs	= 9;
	m_strip_level[ i ].hsi[2].fi	= 229.67;

	m_strip_level[ i ].hsi[3].fh	= 11.42;	
	m_strip_level[ i ].hsi[3].fs	= 13.17;
	m_strip_level[ i ].hsi[3].fi	= 205;

	m_strip_level[ i ].hsi[4].fh	= 357.8;	
	m_strip_level[ i ].hsi[4].fs	= 17.3;
	m_strip_level[ i ].hsi[4].fi	= 185;

	m_strip_level[ i ].hsi[5].fh	= 0.0;	
	m_strip_level[ i ].hsi[5].fs	= 0.0;
	m_strip_level[ i ].hsi[5].fi	= 0.0;
		
	/* 우로빌리노겐 */
	i +=1;
	m_strip_level[ i ].hsi[0].fh	= 8.95;	
	m_strip_level[ i ].hsi[0].fs	= 1.88;
	m_strip_level[ i ].hsi[0].fi	= 247.67;

	m_strip_level[ i ].hsi[1].fh	= 0.0;	
	m_strip_level[ i ].hsi[1].fs	= 0.0;
	m_strip_level[ i ].hsi[1].fi	= 0.0;

	m_strip_level[ i ].hsi[2].fh	= 24.4;	
	m_strip_level[ i ].hsi[2].fs	= 11.11;
	m_strip_level[ i ].hsi[2].fi	= 225;

	m_strip_level[ i ].hsi[3].fh	= 3.67;	
	m_strip_level[ i ].hsi[3].fs	= 12.32;
	m_strip_level[ i ].hsi[3].fi	= 203;

	m_strip_level[ i ].hsi[4].fh	= 2.86;	
	m_strip_level[ i ].hsi[4].fs	= 21.56;
	m_strip_level[ i ].hsi[4].fi	= 174.67;

	m_strip_level[ i ].hsi[5].fh	= 356.24;	
	m_strip_level[ i ].hsi[5].fs	= 29.14;
	m_strip_level[ i ].hsi[5].fi	= 151;


	/* 케톤체 */	
	i +=1;
	m_strip_level[ i ].hsi[0].fh	= 21.05;	
	m_strip_level[ i ].hsi[0].fs	= 2.01;
	m_strip_level[ i ].hsi[0].fi	= 249;

	m_strip_level[ i ].hsi[1].fh	= 9.11;	
	m_strip_level[ i ].hsi[1].fs	= 10.75;
	m_strip_level[ i ].hsi[1].fi	= 214;

	m_strip_level[ i ].hsi[2].fh	= 0;	
	m_strip_level[ i ].hsi[2].fs	= 10.05;
	m_strip_level[ i ].hsi[2].fi	= 202.33;

	m_strip_level[ i ].hsi[3].fh	= 334.19;	
	m_strip_level[ i ].hsi[3].fs	= 50.99;
	m_strip_level[ i ].hsi[3].fi	= 118.33;

	m_strip_level[ i ].hsi[4].fh	= 324.23;	
	m_strip_level[ i ].hsi[4].fs	= 40.19;
	m_strip_level[ i ].hsi[4].fi	= 105.33;

	m_strip_level[ i ].hsi[5].fh	= 0.0;	
	m_strip_level[ i ].hsi[5].fs	= 0.0;
	m_strip_level[ i ].hsi[5].fi	= 0.0;

	/* 단백질 */
	i +=1;
	m_strip_level[ i ].hsi[0].fh	= 60.0;	
	m_strip_level[ i ].hsi[0].fs	= 34.61;
	m_strip_level[ i ].hsi[0].fi	= 200.33;

	m_strip_level[ i ].hsi[1].fh	= 68.08;	
	m_strip_level[ i ].hsi[1].fs	= 32.62;
	m_strip_level[ i ].hsi[1].fi	= 187;

	m_strip_level[ i ].hsi[2].fh	= 77.48;	
	m_strip_level[ i ].hsi[2].fs	= 28.79;
	m_strip_level[ i ].hsi[2].fi	= 178.33;

	m_strip_level[ i ].hsi[3].fh	= 133.44;	
	m_strip_level[ i ].hsi[3].fs	= 16.73;
	m_strip_level[ i ].hsi[3].fi	= 163.33;

	m_strip_level[ i ].hsi[4].fh	= 149.17;	
	m_strip_level[ i ].hsi[4].fs	= 25.16;
	m_strip_level[ i ].hsi[4].fi	= 157.67;

	m_strip_level[ i ].hsi[5].fh	= 171.98;	
	m_strip_level[ i ].hsi[5].fs	= 39.45;
	m_strip_level[ i ].hsi[5].fi	= 145.33;

	/* 아질산염 */
	i +=1;
	m_strip_level[ i ].hsi[0].fh	= 0.0;	
	m_strip_level[ i ].hsi[0].fs	= 0.0;
	m_strip_level[ i ].hsi[0].fi	= 255;

	m_strip_level[ i ].hsi[1].fh	= 0.0;	
	m_strip_level[ i ].hsi[1].fs	= 0.0;
	m_strip_level[ i ].hsi[1].fi	= 0.0;

	m_strip_level[ i ].hsi[2].fh	= 355.81;	
	m_strip_level[ i ].hsi[2].fs	= 5.87;
	m_strip_level[ i ].hsi[2].fi	= 227.33;

	m_strip_level[ i ].hsi[3].fh	= 0.0;	
	m_strip_level[ i ].hsi[3].fs	= 0.0;
	m_strip_level[ i ].hsi[3].fi	= 0.0;

	m_strip_level[ i ].hsi[4].fh	= 0.0;	
	m_strip_level[ i ].hsi[4].fs	= 0.0;
	m_strip_level[ i ].hsi[4].fi	= 0.0;

	m_strip_level[ i ].hsi[5].fh	= 0.0;	
	m_strip_level[ i ].hsi[5].fs	= 0.0;
	m_strip_level[ i ].hsi[5].fi	= 0.0;

	/* 포도당 */
	i +=1;
	m_strip_level[ i ].hsi[0].fh	= 191.03;	
	m_strip_level[ i ].hsi[0].fs	= 29.57;
	m_strip_level[ i ].hsi[0].fi	= 180.33;

	m_strip_level[ i ].hsi[1].fh	= 145.28;	
	m_strip_level[ i ].hsi[1].fs	= 12.42;
	m_strip_level[ i ].hsi[1].fi	= 161;

	m_strip_level[ i ].hsi[2].fh	= 108.07;	
	m_strip_level[ i ].hsi[2].fs	= 11.48;
	m_strip_level[ i ].hsi[2].fi	= 162.67;

	m_strip_level[ i ].hsi[3].fh	= 54.29;	
	m_strip_level[ i ].hsi[3].fs	= 24.47;
	m_strip_level[ i ].hsi[3].fi	= 141.67;

	m_strip_level[ i ].hsi[4].fh	= 37.79;	
	m_strip_level[ i ].hsi[4].fs	= 33.61;
	m_strip_level[ i ].hsi[4].fi	= 122;

	m_strip_level[ i ].hsi[5].fh	= 16.9;	
	m_strip_level[ i ].hsi[5].fs	= 25.44;
	m_strip_level[ i ].hsi[5].fi	= 114;

	/* 산성 */
	i +=1;
	m_strip_level[ i ].hsi[0].fh	= 18.04;	
	m_strip_level[ i ].hsi[0].fs	= 36.31;
	m_strip_level[ i ].hsi[0].fi	= 168;

	m_strip_level[ i ].hsi[1].fh	= 37.1;	
	m_strip_level[ i ].hsi[1].fs	= 45.05;
	m_strip_level[ i ].hsi[1].fi	= 182;

	m_strip_level[ i ].hsi[2].fh	= 61.17;	
	m_strip_level[ i ].hsi[2].fs	= 34.62;
	m_strip_level[ i ].hsi[2].fi	= 163.67;

	m_strip_level[ i ].hsi[3].fh	= 141.68;	
	m_strip_level[ i ].hsi[3].fs	= 29.01;
	m_strip_level[ i ].hsi[3].fi	= 118.33;

	m_strip_level[ i ].hsi[4].fh	= 194.28;	
	m_strip_level[ i ].hsi[4].fs	= 69.41;
	m_strip_level[ i ].hsi[4].fi	= 101.33;

	m_strip_level[ i ].hsi[5].fh	= 0.0;	
	m_strip_level[ i ].hsi[5].fs	= 0.0;
	m_strip_level[ i ].hsi[5].fi	= 0.0;

	/* 비중 */
	i +=1;
	m_strip_level[ i ].hsi[0].fh	= 192.85;	
	m_strip_level[ i ].hsi[0].fs	= 73.21;
	m_strip_level[ i ].hsi[0].fi	= 93.33;

	m_strip_level[ i ].hsi[1].fh	= 166.9;	
	m_strip_level[ i ].hsi[1].fs	= 19.88;
	m_strip_level[ i ].hsi[1].fi	= 112.33;

	m_strip_level[ i ].hsi[2].fh	= 112.69;	
	m_strip_level[ i ].hsi[2].fs	= 8.03;
	m_strip_level[ i ].hsi[2].fi	= 137;

	m_strip_level[ i ].hsi[3].fh	= 72.65;	
	m_strip_level[ i ].hsi[3].fs	= 41.15;
	m_strip_level[ i ].hsi[3].fi	= 156.33;

	m_strip_level[ i ].hsi[4].fh	= 344.62;	
	m_strip_level[ i ].hsi[4].fs	= 83.07;
	m_strip_level[ i ].hsi[4].fi	= 106.33;

	m_strip_level[ i ].hsi[5].fh	= 42.33;	
	m_strip_level[ i ].hsi[5].fs	= 56.1;
	m_strip_level[ i ].hsi[5].fi	= 177.67;

	/* 백혈구 */
	i +=1;
	m_strip_level[ i ].hsi[0].fh	= 14.7;	
	m_strip_level[ i ].hsi[0].fs	= 3.28;
	m_strip_level[ i ].hsi[0].fi	= 244;

	m_strip_level[ i ].hsi[1].fh	= 0.0;	
	m_strip_level[ i ].hsi[1].fs	= 0.0;
	m_strip_level[ i ].hsi[1].fi	= 0.0;

	m_strip_level[ i ].hsi[2].fh	= 355.87;	
	m_strip_level[ i ].hsi[2].fs	= 3.88;
	m_strip_level[ i ].hsi[2].fi	= 232;

	m_strip_level[ i ].hsi[3].fh	= 340.89;	
	m_strip_level[ i ].hsi[3].fs	= 5.94;
	m_strip_level[ i ].hsi[3].fi	= 224.33;

	m_strip_level[ i ].hsi[4].fh	= 295.39;	
	m_strip_level[ i ].hsi[4].fs	= 18.9;
	m_strip_level[ i ].hsi[4].fi	= 151.67;

	m_strip_level[ i ].hsi[5].fh	= 0.0;	
	m_strip_level[ i ].hsi[5].fs	= 0.0;
	m_strip_level[ i ].hsi[5].fi	= 0.0;

	return ERR_SUCCESS;
}

/*=========================================================================
문자 인식
==========================================================================*/
static int rgb_to_hsi( BYTE R, BYTE G, BYTE B, float * ptrH, float * ptrS, float *ptrI )
{
	float r, g, b, h, s, in;
	int min_val = 0;

	r = g = b = h = s = in = 0.0;

	r = R;
	g = G;
	b = B;

	in = (b + g + r) / 3;
	min_val = min(r, min(b,g));
	s = 1 - 3*(min_val/(b + g + r));

    if(s < 0.00001)
    {
		s = 0;
    }else if(s > 0.99999){
        s = 1;
    }

    if(s != 0)
    {
        h = 0.5 * ((r - g) + (r - b)) / sqrt(((r - g)*(r - g)) + ((r - b)*(g - b)));
        h = acos(h);

        if(b <= g)
        {
            h = h;
        } else{
            h = ((360 * 3.14159265) / 180.0) - h;
        }
    }

    *ptrH = (h * 180) / 3.14159265;
    *ptrS = s*100;
    *ptrI = in;   

	return ERR_SUCCESS;
}



int CHAR_GetRecognition( pLPR_CONF ptrLPRConf, pOCR_CONF ptrOCRConf, pDIP_DATA ptrDIPData, pIMAGE_DATA ptrDestImageData,
						 pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrOrgPlateRect, pLPRCHAR ptrLPRChars )
{
	pCVRECT ptrStripRec = NULL;
	pLPRCHAR ptrChar = NULL;

	BYTE * pData = NULL;
	BYTE Blue, Green, Red;
	int nCnt, nHCent , nWCent, k, i, j, nNewW;
	float H, S, I;
	float Sum_H, Sum_S, Sum_I;
	float Mean_H, Mean_S, Mean_I;
	int nCharCnt = 0;

	int Sum_R, Sum_G, Sum_B;
	int Mean_R, Mean_G, Mean_B;
	
	/* 
	영역의 중심 픽셀을 기준으로 3X3 매트리스 색상의 평균 
	0 : 전체 영역이므로 제외 
	*/
	
	for( k = 0 ; k < MAX_STRIP_CNT ; k++ )
	{
		ptrStripRec = (pCVRECT)&ptrOrgPlateRect[ k ];		
		nCnt = 0;

		if( ptrStripRec != NULL )
		{
			H = S = I = Sum_B = Sum_G = Sum_R = Mean_B = Mean_G = Mean_R = 0;
			
			for (i= ptrStripRec->nTop ; i< ptrStripRec->nBottom ; i++)
			{
				/* 24bit 원본 이미지이므로 W의 위치를 RGB위치로 계산 */
				nNewW = ( ptrStripRec->nLeft ) * 3;
				pData = (BYTE*)&ptrImageDataSrc->ptrSrcImage2D[ i ][ nNewW ];				
				
				/* W center 기준으로 -5에서 +5까지의 영역을 조사한다 */
				for (j= 0 ; j< ( HSI_SIZE *2 ) ; j++)
				{
					Blue = Green = Red = 0x00;

					if ( pData != NULL )
					{
						Blue = (BYTE)(*pData++);
						Green = (BYTE)(*pData++);
						Red = (BYTE)(*pData++);

						Sum_B += Blue;
						Sum_G += Green;
						Sum_R += Red;
						nCnt +=1;

					}
				}
			}			

			if ( nCnt > 0 )
			{
				Mean_B = Sum_B / nCnt;
				Mean_G = Sum_G / nCnt;
				Mean_R = Sum_R / nCnt;
				H = S = I = 0.0;

				if ( rgb_to_hsi( Mean_R, Mean_G, Mean_B , &H, &S, &I ) == ERR_SUCCESS )
				{			
					ptrStripRec->HSI.H = H;
					ptrStripRec->HSI.S = S;
					ptrStripRec->HSI.I = I;
				}

				if( ptrLPRChars != NULL && ptrStripRec != NULL )
				{	
					ptrLPRChars->Chars[ k ].ChRect.nTop		= ptrStripRec->nTop;
					ptrLPRChars->Chars[ k ].ChRect.nBottom	= ptrStripRec->nBottom;
					ptrLPRChars->Chars[ k ].ChRect.nLeft	= ptrStripRec->nLeft;
					ptrLPRChars->Chars[ k ].ChRect.nRight	= ptrStripRec->nRight;
					ptrLPRChars->Chars[ k ].ChRect.HSI.H	= ptrStripRec->HSI.H;
					ptrLPRChars->Chars[ k ].ChRect.HSI.S	= ptrStripRec->HSI.S;
					ptrLPRChars->Chars[ k ].ChRect.HSI.I	= ptrStripRec->HSI.I;				
					HLP_CopyImageDataWithRect24( &ptrLPRChars->Chars[ k ].ImageData, ptrImageDataSrc, 
													&ptrLPRChars->Chars[ k ].ChRect );
						
					ptrLPRChars->nCharCnt = MAX_STRIP_CNT;
				}
			}
		}
	}
	return ERR_SUCCESS;
}


int Cal_Strip_Level( pLPRCHAR ptrLPRChars )
{
	#if 0
	int i,j, ncnt;
	CVRECT_t * ptrTempRect = NULL;
	float dH, dRecH, dMinRecH, dMaxRecH;
	float nLevel;

	/* 잠혈 */
	ncnt = ptrLPRChars->nCharCnt;

	for( i = 0 ; i < ncnt ; i++ )
	{	
		ptrTempRect = ( CVRECT_t*)&ptrLPRChars->Chars[ i ].ChRect;
		
		if ( ptrTempRect != NULL )
		{
			dH = 0.0;
			dH = ptrTempRect->HSI.H;

			/* 등록된 모든 level을 찾는다 */
			for( j = 0 ; j < MAX_STRIP_LEVEL_CNT ; j++ )
			{
				dRecH = dMinRecH = dMaxRecH = 0;
				dRecH = m_strip_level[ j ].dval[ i ];

				//if ( dRecH != 0 )
				{
					dMinRecH = dRecH - 3.0f;
					dMaxRecH = dRecH + 3.0f + m_strip_level[ j+ 1 ].dval[ i ];
				}
				/* 임계 범위내에 존재 할때 해당 Level을 등록한다 */
				if ( dMinRecH <= dH &&  dMaxRecH >= dH )
				{
					ptrTempRect->HSI.L = j;
					break;
				}

			}
		}
	}
#else
	int i,j, ncnt, nmin;
	CVRECT_t * ptrTempRect = NULL;
	float q_fs, q_fh, q_fi;
	float d_fs, d_fh, d_fi;
	
	float q_fx, q_fy, q_fz;
	float d_fx, d_fy, d_fz;
	float dx, dy, dz;

	float fdistance ;
	float min_fdistance ;
	pSTRIP_LEVEL ptrStripLevel =NULL;
	
	ncnt = ptrLPRChars->nCharCnt;
	if( ncnt == MAX_STRIP_CNT) 
		ncnt = MAX_STRIP_CNT-1;

	/* 10개 병명 */
	for( i = 0 ; i < ncnt ; i++ )
	{	
		ptrTempRect = ( CVRECT_t*)&ptrLPRChars->Chars[ i ].ChRect;
		
		
		if ( ptrTempRect != NULL )
		{
			q_fs = q_fh = q_fi = 0.0;
			q_fx = q_fy = q_fz = 0.0;

			/* query image h,s,i*/
			q_fh = ptrTempRect->HSI.H;
			q_fs = ptrTempRect->HSI.S;
			q_fi = ptrTempRect->HSI.I;
#if 0
			q_fh = 68.73;
			q_fs = 29.48;
			q_fi = 178.67;
#endif

			/* query image 의 공간좌표 */
			q_fy = q_fs * sin( q_fh * PI / 180);
			q_fx = q_fs * cos( q_fh * PI / 180);			
			q_fz = q_fi;

			/*db image */
			ptrStripLevel= (pSTRIP_LEVEL)&m_strip_level[i];
			nmin = 0;
			if ( ptrStripLevel != NULL )
			{	
				min_fdistance = 1000.0;
				fdistance  = 0.0;

				/*병명 별 레벨 처리 */
				for( j = 0 ; j < MAX_STRIP_LEVEL_CNT ; j++ )
				{
					d_fh = d_fs = d_fi = 0.0;
					d_fx = d_fy = d_fz = 0.0;
					dx = dy = dz= 0.0;

					/* db image h,s,i */
					d_fh = ptrStripLevel->hsi[ j ].fh;
					d_fs = ptrStripLevel->hsi[ j ].fs;
					d_fi = ptrStripLevel->hsi[ j ].fi;

					if( ! (d_fh == 0.0 && d_fs == 0.0 && d_fi == 0.0) )					
					{
						/* db image의 공간좌표 */	
						d_fy = d_fs * sin( d_fh * PI / 180);
						d_fx = d_fs * cos( d_fh * PI / 180);						
						d_fz = d_fi;
						
						/* 색거리 */
						fdistance = sqrt( ((q_fx - d_fx )*(q_fx - d_fx )) + ((q_fy- d_fy)*(q_fy- d_fy)) + ((q_fz -d_fz)*(q_fz -d_fz)) );
					
						if ( min_fdistance > fdistance )
						{
							ptrTempRect->HSI.L = j;
							min_fdistance = fdistance;
						}
					}
				}
			}
		}
	}
#endif
	return ncnt;

}

int	CHAR_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	return ERR_SUCCESS;
}

int CHAR_Release( void )
{
	return ERR_SUCCESS;
}