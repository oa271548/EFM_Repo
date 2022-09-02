// HTML.h: interface for the CHTML class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTML_H__C00B22C6_BCE6_47C9_A929_8BFF9706C510__INCLUDED_)
#define AFX_HTML_H__C00B22C6_BCE6_47C9_A929_8BFF9706C510__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHTML  
{
public:
	CHTML();
	virtual ~CHTML();
	char * readUrl2(CString szUrl, long &bytesReturnedOut, char **headerOut);

protected:
	int getHeaderLength(char *content);
	SOCKET connectToServer(CString szServerName, WORD portNum);
	void ParseUrl(CString mUrl, std::string &serverName, std::string &filepath, std::string &filename);
};

#endif // !defined(AFX_HTML_H__C00B22C6_BCE6_47C9_A929_8BFF9706C510__INCLUDED_)
