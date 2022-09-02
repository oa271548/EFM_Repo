#if !defined(AFX_SELECTOR_H__F27098F9_94D9_48A5_A610_6CCC588F9FFE__INCLUDED_)
#define AFX_SELECTOR_H__F27098F9_94D9_48A5_A610_6CCC588F9FFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


// Dispatch interfaces referenced by this interface
class CPicture;
class COleFont;

/////////////////////////////////////////////////////////////////////////////
// CSelector wrapper class

class CSelector : public CWnd
{
protected:
	DECLARE_DYNCREATE(CSelector)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xb9617403, 0x76ba, 0x11cf, { 0xbc, 0x8b, 0x20, 0x74, 0x2, 0xc1, 0x6, 0x27 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:
	CPicture GetBackPicture();
	void SetBackPicture(LPDISPATCH);
	CString GetCaption();
	void SetCaption(LPCTSTR);
	unsigned long GetCaptionColor();
	void SetCaptionColor(unsigned long);
	short GetCaptionFontID();
	void SetCaptionFontID(short);
	short GetCaptions();
	void SetCaptions(short);
	double GetCaptionX();
	void SetCaptionX(double);
	double GetCaptionY();
	void SetCaptionY(double);
	BOOL GetEnabled();
	void SetEnabled(BOOL);
	COleFont GetFont();
	void SetFont(LPDISPATCH);
	short GetFonts();
	void SetFonts(short);
	unsigned long GetKnobColor();
	void SetKnobColor(unsigned long);
	double GetKnobOriginX();
	void SetKnobOriginX(double);
	double GetKnobOriginY();
	void SetKnobOriginY(double);
	double GetKnobRadius();
	void SetKnobRadius(double);
	long GetKnobStyle();
	void SetKnobStyle(long);
	CString GetKnobUserDefined();
	void SetKnobUserDefined(LPCTSTR);
	unsigned long GetMarkColor();
	void SetMarkColor(unsigned long);
	double GetMarkInnerRadius();
	void SetMarkInnerRadius(double);
	double GetMarkOuterRadius();
	void SetMarkOuterRadius(double);
	long GetMarkStyle();
	void SetMarkStyle(long);
	CString GetMarkUserDefined();
	void SetMarkUserDefined(LPCTSTR);
	double GetMarkWidth();
	void SetMarkWidth(double);
	long GetSelectionAlign();
	void SetSelectionAlign(long);
	double GetSelectionAngle();
	void SetSelectionAngle(double);
	CString GetSelectionCaption();
	void SetSelectionCaption(LPCTSTR);
	unsigned long GetSelectionColor();
	void SetSelectionColor(unsigned long);
	short GetSelectionFontID();
	void SetSelectionFontID(short);
	double GetSelectionOffsetX();
	void SetSelectionOffsetX(double);
	double GetSelectionOffsetY();
	void SetSelectionOffsetY(double);
	double GetSelectionRadius();
	void SetSelectionRadius(double);
	short GetSelections();
	void SetSelections(short);
	short GetValue();
	void SetValue(short);
	short Get_Value();
	void Set_Value(short);
	CPicture GetKnobPicture();
	void SetKnobPicture(LPDISPATCH);
	long GetBevelInner();
	void SetBevelInner(long);
	long GetBevelOuter();
	void SetBevelOuter(long);
	short GetBevelWidth();
	void SetBevelWidth(short);
	short GetBorderWidth();
	void SetBorderWidth(short);
	double GetFrameBottom();
	void SetFrameBottom(double);
	unsigned long GetFrameColor();
	void SetFrameColor(unsigned long);
	double GetFrameLeft();
	void SetFrameLeft(double);
	CPicture GetFramePicture();
	void SetFramePicture(LPDISPATCH);
	CString GetFrameShape();
	void SetFrameShape(LPCTSTR);
	long GetFrameStyle();
	void SetFrameStyle(long);
	double GetFrameTop();
	void SetFrameTop(double);
	BOOL GetOutline();
	void SetOutline(BOOL);
	long GetOutlineAlign();
	void SetOutlineAlign(long);
	unsigned long GetOutlineColor();
	void SetOutlineColor(unsigned long);
	CString GetOutlineTitle();
	void SetOutlineTitle(LPCTSTR);
	short GetOutlineWidth();
	void SetOutlineWidth(short);
	double GetFrameRight();
	void SetFrameRight(double);
	double GetAutoOffsetDistance();
	void SetAutoOffsetDistance(double);
	long GetAutoOffsetStyle();
	void SetAutoOffsetStyle(long);
	double GetAutoRadius();
	void SetAutoRadius(double);
	double GetAutoStartAngle();
	void SetAutoStartAngle(double);
	double GetAutoStopAngle();
	void SetAutoStopAngle(double);
	OLE_COLOR GetBackColor();
	void SetBackColor(OLE_COLOR);
	short GetCaptionID();
	void SetCaptionID(short);
	short GetFontID();
	void SetFontID(short);
	unsigned long GetHighlightColor();
	void SetHighlightColor(unsigned long);
	BOOL GetLineDisplay();
	void SetLineDisplay(BOOL);
	double GetLineInnerRadius();
	void SetLineInnerRadius(double);
	short GetLineThickness();
	void SetLineThickness(short);
	short GetSelectionID();
	void SetSelectionID(short);
	BOOL GetAutoAlign();
	void SetAutoAlign(BOOL);
	BOOL GetAutoAngle();
	void SetAutoAngle(BOOL);
	BOOL GetAutoAngleConfine();
	void SetAutoAngleConfine(BOOL);
	BOOL GetAutoOffset();
	void SetAutoOffset(BOOL);
	BOOL GetHighlight();
	void SetHighlight(BOOL);
	CPicture GetMarkPicture();
	void SetMarkPicture(LPDISPATCH);
	long GetCaptionFlag();
	void SetCaptionFlag(long);
	CString GetFontName();
	void SetFontName(LPCTSTR);
	float GetFontSize();
	void SetFontSize(float);
	BOOL GetFontBold();
	void SetFontBold(BOOL);
	BOOL GetFontItalic();
	void SetFontItalic(BOOL);
	BOOL GetFontUnderline();
	void SetFontUnderline(BOOL);
	BOOL GetFontStrikethru();
	void SetFontStrikethru(BOOL);
	long GetMouseControl();
	void SetMouseControl(long);
	BOOL GetSingleBuffer();
	void SetSingleBuffer(BOOL);
	BOOL GetFocusOutline();
	void SetFocusOutline(BOOL);
	OLE_HANDLE GetHWnd();
	void SetHWnd(OLE_HANDLE);
	CString GetConfiguration();
	void SetConfiguration(LPCTSTR);

// Operations
public:
	void ShowPropertyPage();
	void AboutBox();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTOR_H__F27098F9_94D9_48A5_A610_6CCC588F9FFE__INCLUDED_)