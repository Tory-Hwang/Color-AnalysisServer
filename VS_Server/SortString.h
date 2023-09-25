#pragma once

class CSortString : public CStringArray
{
public:
	CSortString(void);
	~CSortString(void);

	void Sort();
private:
	BOOL CompareAndSwap(int pos);

};
