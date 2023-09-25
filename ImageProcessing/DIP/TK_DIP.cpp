

#include "stdafx.h"
#include "../INC/TK_TYPE.h"

#include "DIP.h"
#include "TK_DIP.h"



int DIP_Initialize( pLIB_INIT_DATA ptrInitData  )
{
	return Initialize( ptrInitData );
}

 int DIP_Release( void  )
{
	return Release();
}

int DIP_CopyImageAllocData( pIMAGE_DATA ptrImageDataDest , pIMAGE_DATA ptrImageDataSrc )
{
	if ( ptrImageDataSrc == NULL )
		return ERR_NO_PARAM;

	return CopyImageAllocData( ptrImageDataDest, ptrImageDataSrc);
}

int DIP_FreeAllocData( pIMAGE_DATA ptrImageData )
{
	if ( ptrImageData == NULL )
		return ERR_NO_PARAM;

	return FreeImageAllocData( ptrImageData);
}

int DIP_ImageProcess( pDIP_DATA ptrDIPData )
{
	if ( ptrDIPData == NULL )
		return ERR_NO_PARAM;

	return ImageProcess( ptrDIPData );
}

int DIP_IOCTL( pIOCTL_DATA ptrIOCTLData )
{
	return IOCTL( ptrIOCTLData );
}