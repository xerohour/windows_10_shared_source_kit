#include "ModeSet.h"

void clsModeDescList::Append(ModeDescription NewModeDetails)
{
	ListElement *pNewElement = NULL;
	
	if (pNewElement = new ListElement)
	{
		pNewElement -> ModeDetails = NewModeDetails;
		pNewElement -> pNextElement = NULL;

		if (pFirst)
			pLast -> pNextElement = pNewElement;
		else
			pFirst = pNewElement;

		pLast = pNewElement;

		++uiElementCount;
	}
}

void clsModeDescList::DeleteAll()
{
	ListElement *pCurrent = NULL, *pNext = NULL;

	if (pCurrent = pFirst)
		do
		{
			pNext = pCurrent -> pNextElement;
			delete pCurrent;
		}
		while (pCurrent = pNext);

	pFirst = (pLast = NULL); uiElementCount = 0;
}

void clsModeDescSet::DeleteAll()
{
	if (pModeDescArray)
	{
		delete [] pModeDescArray;
		pModeDescArray = NULL;
	}

	uiElementCount = 0;
}

void clsModeDescSet::InitializeFromList(clsModeDescList *pSourceList)
{
	UINT uiNoOfElements;

	//DebugOut.Write("Entering clsModeDescSet::InitializeFromList.\n");

	DeleteAll();

	if (uiNoOfElements = pSourceList -> uiElementCount)
		if (pModeDescArray = new ModeDescription[uiNoOfElements])
		{
			UINT uiCurrentIndex = 0;
			ListElement *pCurrent = pSourceList -> pFirst;

			do
			{
				pModeDescArray[uiCurrentIndex++] = (pCurrent -> ModeDetails);
			}
			while (pCurrent = pCurrent -> pNextElement);

			uiElementCount = uiNoOfElements;
		}
}

const ModeDescription* clsModeDescSet::GetModeDesc(UINT uiIndex)
{
	if (pModeDescArray)
		if (uiIndex < uiElementCount)
			return (const ModeDescription*)(&(pModeDescArray[uiIndex]));

	return NULL;
}
