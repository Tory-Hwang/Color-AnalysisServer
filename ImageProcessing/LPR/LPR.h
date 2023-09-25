#ifndef _LPR_H__
#define _LPR_H__


int LPR_Initialize( pLIB_INIT_DATA  ptrInitData );
int LPR_Release( void  );

int LPR_DIFFProcess( pDIFF_DATA ptrDiffData );
int LPR_LPRProcess( pLPR_DATA ptrLPRData );
int LPR_IOCTL( pIOCTL_DATA ptrIOCTLData );

#endif