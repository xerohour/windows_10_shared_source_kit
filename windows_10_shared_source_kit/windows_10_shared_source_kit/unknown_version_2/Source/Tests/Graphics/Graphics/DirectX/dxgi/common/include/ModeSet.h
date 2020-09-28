#ifndef MODE_SET_INCLUDED
#define MODE_SET_INCLUDED

#include <dxgi.h>

typedef
struct _ModeDescription
{
	UINT uiAdapterIndex;
	UINT uiOutputIndex;
	DXGI_MODE_DESC DXGIModeDesc;
}
ModeDescription;

typedef struct _ListElement ListElement;

struct _ListElement
{
	ModeDescription ModeDetails;
	ListElement *pNextElement;
};

class clsModeDescList
{
private:

	friend class clsModeDescSet;

	ListElement *pFirst, *pLast;
	UINT uiElementCount;

public:
	void Append(ModeDescription NewModeDetails);
	void DeleteAll();
	clsModeDescList() {pFirst = (pLast = NULL); uiElementCount = 0;}
	~clsModeDescList() {DeleteAll();}
};

class clsModeDescSet
{
private:
	ModeDescription *pModeDescArray;
	UINT uiElementCount;
public:
	void DeleteAll();
	clsModeDescSet() {pModeDescArray = NULL; uiElementCount = 0;}
	~clsModeDescSet() {DeleteAll();}
	void InitializeFromList(clsModeDescList *pSourceList);
	UINT GetElementCount() {return uiElementCount;}
	const ModeDescription *GetModeDesc(UINT uiIndex);
};

#endif
