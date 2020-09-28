//
// CResFile.h
//

#ifndef __CRESFILE_H__
#define __CRESFILE_H__

enum CRESFILETYPE
{
	CRFILE		= 1,
	CRRESOURCE	= 2,
	CRCLOSED	= 3,
	CREITHER	= 4
};

class CResFile
{
public:
	// Constructor/Destructor
	CResFile();
	~CResFile();

	// File opening and closing
	BOOL Open(LPCSTR lpFile, // Name of the file or resource
			  enum CRESFILETYPE eType = CREITHER, // Flag specifying the type
			  BOOL bWrite = FALSE,  // True to open for writing
			  HMODULE hModule = NULL, // Module in which resource is located
			  LPCSTR lpType = NULL); // resource type
	BOOL Close();

	// File reading/writing
	BOOL Read(LPVOID lpBuffer, 
			  DWORD dwBytesToRead,
			  DWORD *lpdwBytesRead = NULL);
	BOOL Write(LPVOID lpBuffer, 
			   DWORD dwBytesToWrite,
			   DWORD *lpdwBytesWritten = NULL);


	// File pointer
	BOOL Seek(DWORD dwPos); // Seeks to the specified position, from the start
	BOOL Eof(); // True if you have reached the end of the file (or resource)

private:

	// Common data
	enum CRESFILETYPE	m_eType;
	DWORD	m_dwFileSize;
	DWORD	m_dwFilePtr;
	
	// Data associated with reading from a resource
	HRSRC	m_hResource;
	BYTE	*m_lpResData;

	// Data associated with reading from a file
	HANDLE	m_hFile;

public:
	DWORD dwFileSize() const { return m_dwFileSize + 1; }

};

#endif //__CRESFILE_H__