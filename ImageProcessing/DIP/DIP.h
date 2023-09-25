#ifndef _DIP_H__
#define _DIP_H__

#include "../include/TK_TYPE.h"

int DIP_Initialize( pLIB_INIT_DATA ptrInitData  );
int DIP_Release( void  );

int DIP_CopyImageAllocData( pIMAGE_DATA ptrImageDataDest, pIMAGE_DATA ptrImageDataSrc );
int DIP_FreeImageAllocData( pIMAGE_DATA ptrImageData );
int DIP_ImageProcess( pDIP_DATA ptrDIPData );
int DIP_IOCTL( pIOCTL_DATA ptrIOCTLData );

#endif