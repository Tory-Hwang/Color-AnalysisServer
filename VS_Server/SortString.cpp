#include "StdAfx.h"
#include "SortString.h"

CSortString::CSortString(void)
{
}

CSortString::~CSortString(void)
{
}

void CSortString::Sort()
{
	BOOL bNotDone = TRUE;

	while (bNotDone)
	{
		bNotDone = FALSE;
		for(int pos = 0;pos < GetUpperBound();pos++)
			bNotDone |= CompareAndSwap(pos);
	}
}

BOOL CSortString::CompareAndSwap(int pos)
{
	CString temp;
	int posFirst = pos;
	int posNext = pos + 1;

	if (GetAt(posFirst).CompareNoCase(GetAt(posNext)) > 0)
	{
		temp = GetAt(posFirst);
		SetAt(posFirst, GetAt(posNext));
		SetAt(posNext, temp);
		return TRUE;

	}
	return FALSE;
}

