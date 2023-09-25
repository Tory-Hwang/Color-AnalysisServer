#ifndef __TK_DIP_H__
#define __TK_DIP_H__



/* Must Modify at TK_DIP.def */
int /*CALLAGREEMENT*/ DIP_Initialize( void  );
int /*CALLAGREEMENT*/ DIP_Release( void  );

int /*CALLAGREEMENT*/ DIP_CopyImageAllocData( pIMAGE_DATA ptrImageDataDest , pIMAGE_DATA ptrImageDataSrc );
int /*CALLAGREEMENT*/ DIP_FreeAllocData( pIMAGE_DATA ptrImageData );
int /*CALLAGREEMENT*/ DIP_ImageProcess( pDIP_DATA ptrDIPData );
int /*CALLAGREEMENT*/ DIP_IOCTL( pIOCTL_DATA ptrIOCTLData );


#endif