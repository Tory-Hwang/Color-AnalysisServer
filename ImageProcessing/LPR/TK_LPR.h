#ifndef __TK_LPR_H__
#define __TK_LPR_H__



	/* Must Modify at TK_LPR.def */
 int /*CALLAGREEMENT*/ LPR_Initialize( pLIB_INIT_DATA  ptrInitData );
 int /*CALLAGREEMENT*/ LPR_Release( void  );
	
 int /*CALLAGREEMENT*/ LPR_DIFFProcess( pDIFF_DATA ptrDiffData );
 int /*CALLAGREEMENT*/ LPR_LPRProcess( pLPR_DATA ptrLPRData );
 int /*CALLAGREEMENT*/ LPR_IOCTL( pIOCTL_DATA ptrIOCTLData );



#endif