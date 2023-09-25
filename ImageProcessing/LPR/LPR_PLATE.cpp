#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/TK_TYPE.h"
#include "../include/TK_LIB_Helper.h"

#include "LPR_COMMON.h"
#include "LPR_DIP.h"
#include "LPR_PLATE.h"

static float m_fNewTh = 0;

/* ===================================== Make Uniformity  =========================================================================================
===================================================================================================================================================
참조 논문 : 분할된 영역의 특성을 이용한 차량 번호판 포착( 한국 정보과학회논문지 )
디지털 신호 분석 기법을 이용한 다양한 번호판 추출 방법( 전자 공학회 논문지 )
U( m, G( maxpixel ) , s) = | m- G(maxpixel ) | + s
m : 평균 = ( 1/ K * L ) * sigma( G ( i, j )
s : 분산 = ( 1/ K * L ) * sigma( | G( i, j ) - m ) | )

T : 임계값 = ( 1/ N * M ) * sigma( U( m, G(maxpixel ), s )

K, L : 영역 크기
N, M : 영상 크기
G( i, j ) : 영상 밝기
G( maxpixel ) : 가장 많은 개수를 가진 밝기 값
==================================================================================================================================================*/

/*=========================================================================
유효하지 않는 불균등 영역은 삭제한다. 
0 : 불균등 영역 
x : 균등 영역 
1)        2)       3)       4)      5)
x x x     x x x    x 0 x    x x 0   0 x x
x 0 x     0 0 0    x 0 x    x 0 x   x 0 x 
x x x     x x x    x 0 x    0 x x   x x 0
==========================================================================*/



static int Del_InvalidUniformity( pLPR_CONF ptrLPRConf, UNIFORMITY_t **ptrUniformitys, int nUniforH, int nUniforW )
{
	int nRepeat = 1;
	int i, j;
	int nLoopCnt = 0;
	int nUnCnt = 0;
	int nMidFlag = 0;
	/* 전체 W에서 스트립 항목 개수에서 3개를 뺀 영역을 최소 개수로 본다 */
	int nMinCnt = ( MAX_IMG_W / STRIP_W_SIZE ) - 5;

	( void )ptrLPRConf;

	while( nRepeat > 0 )
	{
		nRepeat = 0;

		if( nLoopCnt++ > 50 )
		{
			HLP_print_dbg(DBG_INFO, "1Del_InvalidUniformity LOOPCNT :%d", nLoopCnt );
			break;
		}		
		
		for( i = 1; i < nUniforH -1 ; i++ )
		{
			for( j = 1 ; j < nUniforW -1; j++ )
			{
				/* 1 Mask */
				if (( ptrUniformitys[ i ][ j ].nUniform == 1  ) &&

					( ptrUniformitys[ i-1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i   ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j-1 ].nUniform == 0 ) && 

					( ptrUniformitys[ i-1 ][ j ].nUniform == 0 ) && 					 
					( ptrUniformitys[ i+1 ][ j ].nUniform == 0 ) && 

					( ptrUniformitys[ i-1 ][ j+1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i   ][ j+1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i ][ j ].nUniform = 0;
					nRepeat = 1;
				}

				/* 2 Mask */
				if ( nRepeat != 1 && ( ptrUniformitys[ i-1 ][ j ].nUniform == 1  ) &&
					( ptrUniformitys[ i  ][ j ].nUniform == 1  ) &&
					( ptrUniformitys[ i+1 ][ j ].nUniform == 1  ) &&

					( ptrUniformitys[ i-1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i   ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j-1 ].nUniform == 0 ) && 
					
					( ptrUniformitys[ i-1 ][ j+1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i   ][ j+1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i-1 ][ j ].nUniform = 0;
					ptrUniformitys[ i   ][ j ].nUniform = 0;
					ptrUniformitys[ i+1 ][ j ].nUniform = 0;

					nRepeat = 1;
				}

				/* 3 Mask */
				if (nRepeat != 1 &&  ( ptrUniformitys[ i ][j-1 ].nUniform == 1  ) &&
					( ptrUniformitys[ i ][j ].nUniform == 1  ) &&
					( ptrUniformitys[ i ][j +1].nUniform == 1  ) &&

					( ptrUniformitys[ i-1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i-1 ][ j ].nUniform == 0 ) && 
					( ptrUniformitys[ i-1 ][ j+1 ].nUniform == 0 ) && 

					( ptrUniformitys[ i+1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i ][j-1].nUniform = 0;
					ptrUniformitys[ i ][ j ].nUniform = 0;
					ptrUniformitys[ i ][j+1].nUniform = 0;

					nRepeat = 1;
				}

				/* 4 Mask */
				if (nRepeat != 1 &&  ( ptrUniformitys[ i+1][j-1 ].nUniform == 1  ) &&
					( ptrUniformitys[ i ][ j ].nUniform == 1  ) &&
					( ptrUniformitys[ i-1 ][j +1].nUniform == 1  ) &&

					( ptrUniformitys[ i-1 ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i  ][ j -1].nUniform == 0 ) && 
					
					( ptrUniformitys[ i-1 ][ j ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j ].nUniform == 0 ) && 

					( ptrUniformitys[ i ][ j +1].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i+1][j-1 ].nUniform = 0;
					ptrUniformitys[ i ][ j ].nUniform = 0;
					ptrUniformitys[ i-1 ][j +1].nUniform = 0;

					nRepeat = 1;
				}

				/* 5 Mask */
				if ( nRepeat != 1 &&  ( ptrUniformitys[ i-1][j-1 ].nUniform == 1  ) &&
					( ptrUniformitys[ i ][ j ].nUniform == 1  ) &&
					( ptrUniformitys[ i+1 ][j +1].nUniform == 1  ) &&

					( ptrUniformitys[ i ][ j-1 ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j -1].nUniform == 0 ) && 

					( ptrUniformitys[ i-1 ][ j ].nUniform == 0 ) && 
					( ptrUniformitys[ i+1 ][ j ].nUniform == 0 ) && 

					( ptrUniformitys[ i-1][ j +1].nUniform == 0 ) && 
					( ptrUniformitys[ i ][ j+1 ].nUniform == 0 ) )
				{					
					ptrUniformitys[ i-1][j-1 ].nUniform = 0;
					ptrUniformitys[ i ][ j ].nUniform = 0;
					ptrUniformitys[ i+1 ][j +1].nUniform = 0;

					nRepeat = 1;
				}

			}

		}

	}
	/* 수평기준으로 불균등 개수가 최소 개수보다 작을 경우 제거 */
	for( i = 1; i < nUniforH -1 ; i++ )
	{		
		nUnCnt = 0;

		for( j = 1 ; j < nUniforW -1; j++ )
		{
			if ( ptrUniformitys[ i ][ j ].nUniform == 1  )
			{					
				nUnCnt++;
			}	
		}

		if ( nUnCnt < nMinCnt )
		{
			for( j = 0 ; j < nUniforW ; j++ )
			{
				ptrUniformitys[ i ][ j ].nUniform = 0 ;
			}
		}
	}

	return ERR_SUCCESS;
}


/*=========================================================================
불균등 영역 설정 
==========================================================================*/
static int MakeUniformityArea( pLPR_CONF ptrLPRConf, pDIP_DATA ptrDIPData, UNIFORMITY_t **ptrUniformitys, 
							   int nUniforH, int nUniforW , pIMAGE_DATA ptrImageDataSrc )
{
	int i, j, k, q, z,x;
	int nVal, nSum1;
	float fSum2;
	float fMean, fDif ,fVar, fU, fTh,fTotalUni, fSig;		
	
	int nValCnt;
	int nHSize = 0;
	int nWSize = 0;
	
	( void )ptrDIPData;

	for(i = 0 ; i < nUniforH ; i++)
	{
		memset( ptrUniformitys[ i ] , 0, sizeof( UNIFORMITY_t) * nUniforW );
	}

	z =0;
	fTotalUni = 0;
	x =0;	

	nHSize = UF_H_SIZE;
	nWSize = UF_W_SIZE;

	/*===========================================================================
	[ 이미지의 모든 가로를 처리하는것이 아니라 일정 간격으로 두고 처리한다. 
	  차량 영상의 이웃은 서로 비슷한 속성을 가지고 있으며 이를 통해 처리속도에 이점이 있다. ]	
	============================================================================*/
	for( i = 0 ; i < ptrImageDataSrc->nH -1 ; i += (nHSize) )
	{
		fMean= 0;
		fDif = 0;
		fVar = 0;
		fSig = 0;

		q =0;
		x =0;		

		for( j = 1 ; j < ptrImageDataSrc->nW -1; j += nWSize)
		{	
			if ( j >= ptrImageDataSrc->nW -1 )
			{
				continue;;
			}
			nSum1 = 0;
			fSum2 = 0;
			
			/*===========================================================================
			 [ Uniform 단위 평균  ]
			============================================================================*/
			nValCnt = 0;
			q = i;
			//for( q = i ; q < ( i + nHSize -1) ; q++  )
			{			
				for( k = j; k < ( j + nWSize )  ; k++  )
				{	
					if( q < ptrImageDataSrc->nH -1 && k < ptrImageDataSrc->nW -1 )
					{
						nVal = ptrImageDataSrc->ptrSrcImage2D[ i][ k ] ;

						nSum1 += nVal;
						nValCnt++;
					}
				}
			}
			if ( nValCnt > 0 )
			{
				fMean = (float)( nSum1 / ( nValCnt   ) );	/* 평균 */
			}
			
			/*===========================================================================
			 [ Uniform 단위 표준 편차  ]
			============================================================================*/
			nValCnt = 0;
			q = i;
			//for( q = i ; q < ( i + nHSize -1) ; q++  )
			{			
				for( k = j; k < ( j + nWSize )  ; k++  )
				{
					if( k < ptrImageDataSrc->nW -1 )
					{
						nVal = ptrImageDataSrc->ptrSrcImage2D[ i ][ k ] ;

						fDif = abs( nVal - fMean ) ;	
						fSum2 += ( fDif * fDif ) ;				
						//fSum2 += fDif;
						nValCnt++;
					}				
				}
			}
			if ( nValCnt > 0 )
			{
				fVar = (float)( fSum2 / nValCnt );			/* 분산		*/
				fSig = sqrt( fVar )	;						/* 표준편차 */
				//fU = ( abs( fMean - nMaxVal ) + fVar ) ;	/* 균일성	*/
				//fU = ( abs( fMean - nMaxVal ) + fSig ) ;	/* 균일성	*/
				fU =0 ;	/* 균일성	*/
				/*===========================================================================
				 [ Uniform 단위 정보  ]
				============================================================================*/
				fTotalUni += fSig;				
				ptrUniformitys[ z ][ x ].fU = fU;				/* 균일성	*/
				ptrUniformitys[ z ][ x ].nSum = nSum1;		/* 합		*/
				ptrUniformitys[ z ][ x ].nCnt = nValCnt;		/* 합		*/				
				
				ptrUniformitys[ z ][ x ].fMean = fMean;		/* 평균		*/
				ptrUniformitys[ z ][ x ].fVar = fVar;			/* 분산		*/
				ptrUniformitys[ z ][ x ].fSig = fSig;			/* 표준편차 */
				ptrUniformitys[ z ][ x ].nX = j;
				ptrUniformitys[ z ][ x ].nY = i;
				x++;
				if ( fSig > 0 )
				{
					int kkk = 0;
				}
			}
		}
		z++;
	}

	/*===========================================================================
	 [ 균일성 설정 ]
	 fTotalUni가 높을수록 영상 전체가 불규칙하게 이루어져 있음  

	 ※전체 영상의 불균등이 높을수록 번호판 영역의 불균등은 높음 
	 Uniformy Width에 따라 nA 값을 변경해준다. 

	 평균 fTotalUni은 약 15 ~ 30 사이의 값이 나오나 영상에 따라 다르게 나올수 있음. 
	============================================================================*/
	fTh = ( fTotalUni / ( x * z ) );
		
	float nA ;
	
	if ( fTh < 19 )			/* 불균등이 낮을 경우 */
		nA = 2.0;
	else if ( fTh < 23 )	/* 불균등이 중간 경우 */
		nA = 2.5;
	else					/* 불균등이 높을 경우 */
		nA = 3.0;
	
	m_fNewTh = fTh * nA;
	m_fNewTh = fTh ;

	/* UF Width가 클 경우 혼잡도 계산 */
	if ( ptrLPRConf->LPR_UF_W >= ptrLPRConf->LPR_MIN_CHAR_W * 10  )
	{
		if ( fTh >= 25.0 )
		{
			m_fNewTh = m_fNewTh * 0.8;
		}		
	}

	//if( ptrLPRConf->LPR_DBG_SHOW_LOG )
	{
		//HLP_print_dbg(DBG_INFO, "fTh : %f, nA : %f, Threshold : %f  =============", fTh ,nA,m_fNewTh );
	}

	for( i = 0; i < nUniforH ; i++ )
	{
		for( j = 0 ; j < nUniforW ;j++ )
		{			
			if ( ptrUniformitys[ i][ j ].fSig >= m_fNewTh  )
			{
				ptrUniformitys[ i][ j ].nUniform = 1;
			}
		}
	}

	return ERR_SUCCESS;
}


/*===========================================================================
	 [ 영역의 유효성 체크 ]
	 [ 유효한 영역에서 수직 엣지 카운트를 계산  ]
	============================================================================*/
static int GetPlateRectWithUniformify( pLPR_CONF ptrLPRConf, UNIFORMITY_t ** ptrUniformitys, int nUniforH, int nUniforW , 
									  pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrPlateRect )
{	
	int i, j;
	UNIFORMITY_t TempUnifor;
	UNIFORMITY_t TempUnifor2;
	
	int nTop, nBottom, nLeft, nRight, nCenter;
	BYTE nInterVal[ MAX_STRIP_CNT ];
	BYTE nYInterVal[ MAX_STRIP_CNT ];
	int x,y;
	int nHCnt , nWCnt;
	int nRecCnt = 0;
	int nBottomY ;
	int nLeftX, nRightX;
	int nStip_Right = 0;
	/* 스트립영역의 Right_Bottim 영역을 찾았을 경우 */
	pCVRECT ptrStripRec = NULL;
	int interval = 0;
	int Yinterval = 0;

	int nTopUH  = 0;
	int OverR = 0;

	nBottomY = 0;
	nHCnt = nWCnt = 0;
	memset( nInterVal, 0, sizeof( nInterVal ));
	memset( nYInterVal, 0, sizeof( nYInterVal ));

	/*===========================================================================
	 볼록 렌즈의 특성상 가운데 영역이 조금 크게보여 스트립항목 사이의 간격을 각각 처리한다. 
	 실제 간격은 17 pixel
	============================================================================*/	
	nInterVal[ 0 ] = 16;
	nInterVal[ 1 ] = 13;
	nInterVal[ 2 ] = 17;
	nInterVal[ 3 ] = 16;
	nInterVal[ 4 ] = 16;
	nInterVal[ 5 ] = 19;
	nInterVal[ 6 ] = 19;
	nInterVal[ 7 ] = 19;
	nInterVal[ 8 ] = 20;
	nInterVal[ 9 ] = 17;

	nYInterVal[ 0 ] = 0;
	nYInterVal[ 1 ] = 0;
	nYInterVal[ 2 ] = 1;
	nYInterVal[ 3 ] = 1;
	nYInterVal[ 4 ] = 1;
	nYInterVal[ 5 ] = 2;
	nYInterVal[ 6 ] = 2;
	nYInterVal[ 7 ] = 2;
	nYInterVal[ 8 ] = 3;
	nYInterVal[ 9 ] = 3;

	/* 불균등 영역의 오른쪽 Bottom 영역을 찾는다 */
	for( i =  (nUniforH / 3); i < (nUniforH - STRIP_H_SIZE ); i++ )
	{
		nWCnt = 0;
		for( j = i+1 ; j < ( i + STRIP_H_SIZE ) ; j++ )
		{
			TempUnifor	= ptrUniformitys[ i ][ nUniforW-2 ];	
			TempUnifor2	= ptrUniformitys[ j ][ nUniforW-2 ];

			if (  TempUnifor.nUniform == 1 && TempUnifor2.nUniform == 1 )
			{				
				nWCnt +=1;
			}
			else
			{
				break;
			}
		}

		if ( nWCnt > STRIP_H_SIZE / 4 )
		{
			TempUnifor	= ptrUniformitys[ i ][ 0 ];
			if ( nBottomY == 0 )
			{
				nBottomY = TempUnifor.nY + STRIP_H_SIZE;
				nRightX  =  MAX_IMG_W - STRIP_RIGHT_POS;
			}
			nTopUH =i;
			break;
		}
	}

	/* Bottom 영역에서 스트립지 영역의 불균등 영역을 다시 확인하여 
	   스트립지의 불균등이 맞는지 확인 */
	nHCnt = 0;
	interval = nTopUH + ( STRIP_H_SIZE / UF_H_SIZE );
	if ( interval > 0 )
	{
		for( i = nTopUH ; i <= interval ; i++ )
		{
			nWCnt = 0;
			for( j = nUniforW -1 ; j>=0 ; j-- )
			{
				TempUnifor	= ptrUniformitys[ i ][ j ];		

				if (  TempUnifor.nUniform == 1 )
				{				
					nWCnt +=1;
				}
			}
			/* 스트립 중간위치의 불균등이 1/2이상이면 스트립영역이라 판단 */
			if ( nWCnt >= ( nUniforW/ 2 ) )
			{
				nHCnt +=1;
			}
		}
		if ( nHCnt < (STRIP_H_SIZE/4) )
		{
			nBottomY=0;
		}
	}
		
	/* 오른쪽 불균등영역에서 스트립 오른쪽 첫번째 수직 라인을 찾는다.  */
	nHCnt = 0;
	if ( nBottomY > 0 )
	{
		nLeftX  = nRightX - STRIP_W_SIZE;

		for( x = nRightX ; x >= nLeftX ; x-- )
		{	
			nHCnt = 0;
			for(y = nBottomY ; y >= (nBottomY - STRIP_H_SIZE) ; y-- )
			{			
				if( ptrImageDataSrc->ptrSrcImage2D[ y ][ x ] == 255  ) 
				{	
					nHCnt +=1;
				}					
			}
			/* 불균등 영역의 개수가 이미지 사이즈의 1/5이상이 되면 첫번째 스트립 오른쪽 라인으로 인지 */
			if ( nHCnt >= ( STRIP_H_SIZE /3 ) )
			{
				nStip_Right = x;
				OverR = ( MAX_IMG_W - ( STRIP_RIGHT_POS + 5));
				/*첫번째 스트립의 왼쪽을 찾을 경우 */
				if( nStip_Right <= ( OverR ))
				{
					nStip_Right += STRIP_W_SIZE;
				}
				break;
			}
		}
	}

	/* 오른쪽 수직 라인 엣지를 찾았을 경우 */
	if ( nStip_Right > 0 )
	{
		nLeft	= nStip_Right- STRIP_W_SIZE;
		nTop	=  nBottomY - STRIP_H_SIZE;
		nRight	= nStip_Right;
		nBottom = nBottomY;

		for( i = 0 ; i < MAX_STRIP_CNT ; i++ )
		{
			/* 시트지 경계영역을 제거하기위해 가로,세로 FIX_CELL_SIZE만큼 줄임 */
			Yinterval = nYInterVal[ i ];
			ptrStripRec = (pCVRECT)&ptrPlateRect[ i ];
			ptrStripRec->nSeqNum = i+1 ;

			ptrStripRec->nLeft	= nLeft +  FIX_CELL_SIZE;
			ptrStripRec->nTop	=  nTop + FIX_CELL_SIZE + Yinterval;
			ptrStripRec->nRight = nRight - FIX_CELL_SIZE;
			ptrStripRec->nBottom = ptrStripRec->nTop + STRIP_H_SIZE  - FIX_CELL_SIZE  ;

			/* 볼록 렌즈 특성상 가운데가 조금 넓게 보이기 때문에 항목별 간격 데이터로 처리한다 */	
			interval = nInterVal[ i ];
			
			nRight		= ptrStripRec->nLeft - interval ;  
			nLeft		= ( nRight - STRIP_W_SIZE );
		
			nRecCnt++;
		}
		int nWCent, nHCent;

		for( i = 0 ; i < MAX_STRIP_CNT -1; i++ )
		{
			/* 시트지 경계영역을 제거하기위해 가로,세로 FIX_CELL_SIZE만큼 줄임 */			
			nWCent = nHCent = 0;

			ptrStripRec = (pCVRECT)&ptrPlateRect[ i ];			
			nWCent = (ptrStripRec->nRight + ptrStripRec->nLeft ) /2 ;
			nHCent = (ptrStripRec->nBottom + ptrStripRec->nTop ) /2 ;

			ptrStripRec->nLeft	= nWCent - HSI_SIZE;
			ptrStripRec->nTop	=  nHCent -HSI_SIZE;
			ptrStripRec->nRight =  nWCent + HSI_SIZE;
			ptrStripRec->nBottom = nHCent + HSI_SIZE;
		}
	}

	return nRecCnt;
}

/*================================================================================
External Functions 
=================================================================================*/
int PLATE_Initialize( void )
{
	return ERR_SUCCESS;
}

/*======================================================================
번호판 영역 추출

pDIP_DATA ptrDIPData			: DIP 처리용 Image 메모리
pIMAGE_DATA ptrDestImageData	: 변환 처리 후 저자용 Image 메모리
pIMAGE_DATA ptrImageDataSrc		: 원본 데이터 Image 메모리 
pCVRECT ptrPlateRect			: 번호판 데이터 메모리 

========================================================================*/
int PLATE_GetRecognition( pLPR_CONF ptrLPRConf, pOCR_CONF ptrOCRConf, pDIP_DATA ptrDIPData, pIMAGE_DATA ptrDestImageData, 
						  pIMAGE_DATA ptrChgImageData, pIMAGE_DATA ptrImageDataSrc, pCVRECT ptrPlateRect)
{
	int nUniforH , nUniforW, nCandidateCnt;	
	pCOMMONINFO ptrComInfo = NULL;
	UNIFORMITY_t ** ptrComUniformitys = NULL;	
	int nValidCnt =0;
	int nLabelCnt = 0;

	( void )ptrDestImageData;

	/*===========================================================================
	[ 공용 메모리 요청 ]
	============================================================================*/
	ptrComInfo = COM_GetInfo();
	ptrComUniformitys = COM_GetUniformityBuf();
	
	/*===========================================================================
	[ 유효 번호판 이진화 영상 저장용 버퍼 초기화 ]
	============================================================================*/
	COM_Init_RectImageBuf();

	if ( ptrComUniformitys == NULL || ptrComInfo == NULL || ptrPlateRect == NULL || ptrLPRConf == NULL ||  ptrOCRConf == NULL )
	{
		return ERR_ALLOC_BUF;
	}	

	/*===========================================================================
	[ 이미지 스무딩 필터링 단계 ]
	강한 잡음을 먼저 제거하여 불균형 영역 인식률을 높인다. 
	============================================================================*/	
	
	if ( ptrLPRConf->LPR_DBG_RUN_FIND_UF && ptrLPRConf->LPR_DBG_RUN_UF_PREDIP )
	{
		//Contrast( ptrDIPData, ptrChgImageData, 80 );
		//Sobel( ptrDIPData, ptrChgImageData, H_LINE_EDGE );
		//Binarization_Otsu( ptrDIPData, ptrChgImageData, NULL);
		//Mean( ptrDIPData, ptrChgImageData );
		Mean( ptrDIPData, ptrChgImageData );
		Unsharp(ptrDIPData, ptrChgImageData,1);
		Canny(ptrDIPData, ptrChgImageData );
	}
	

	nUniforH = ptrComInfo->nUniformityH;
	nUniforW = ptrComInfo->nUniformityW;

	/*===========================================================================
	[ 라인별 불균일 영역 검출 ]	
	============================================================================*/	
	if ( MakeUniformityArea( ptrLPRConf, ptrDIPData, ptrComUniformitys, nUniforH, nUniforW, ptrChgImageData ) != ERR_SUCCESS )
	{
		return ERR_INVALID_DATA;
	}
	
	/*===========================================================================
	[ 잡음 불균일 영역 제외 ]	
	============================================================================*/		
	Del_InvalidUniformity( ptrLPRConf, ptrComUniformitys , nUniforH, nUniforW);		

	/*===========================================================================
	[ 유효 영역 검출 ]	
	============================================================================*/	
	nValidCnt= GetPlateRectWithUniformify( ptrLPRConf, ptrComUniformitys, nUniforH, nUniforW , ptrChgImageData, ptrPlateRect );	
			
	/*===========================================================================
	[ 불균등 영역 이미지 복사 ]
	============================================================================*/
	if( ptrLPRConf->LPR_DBG_SHOW_UF_LINE )
	{	
		int i,j;
		for( i = 0; i < nUniforH ; i++ )
		{
			for( j = 0 ; j < nUniforW ; j++ )
			{
				if ( ptrComUniformitys[ i][ j ].nUniform == 1 )
				{	
					COM_DrawCandidateLine( ptrComUniformitys[ i][ j ].nY, ptrComUniformitys[ i][ j ].nX, ptrComUniformitys[ i][ j ].nX + ptrLPRConf->LPR_UF_W, ptrChgImageData );
				}
			}		
		}
	}

	return nValidCnt;
}

int PLATE_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	( void )ptrIOCTLData;

	return ERR_SUCCESS;
}

int PLATE_Release( void )
{
	return ERR_SUCCESS;
}