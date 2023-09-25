#pragma once

#include "Com_Def.h"

#include "IController.h"
#include "IModel.h"
#include "IViewer.h"
#include "ITransfer.h"

typedef struct ModelInfo_
{
	BYTE Kind;
	IModel * ptrModel;
}ModelInfo_t;

typedef struct TransferInfo
{
	BYTE Kind;
	ITransfer * ptrTransfer;
}TransferInfo_t;

typedef struct ViewerInfo_
{
	BYTE Kind;
	IViewer * ptrViewer;
}ViewerInfo_t;

typedef struct ObjectManager_
{
	int ModelCnt;
	int ViewerCnt;
	int TransferCnt;

	ModelInfo_t ModelObjects[ MAX_MODEL_CNT ];
	TransferInfo_t TransferObjects[ MAX_TRANSFER_CNT ];
	ViewerInfo_t ViewerObjects[ MAX_VIEWER_CNT ];	
}ObjectManager_t;

