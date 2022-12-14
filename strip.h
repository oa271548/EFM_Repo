#if !defined(AFX_STRIP_H__417401CA_594F_4EC4_890F_2DDF0A615771__INCLUDED_)
#define AFX_STRIP_H__417401CA_594F_4EC4_890F_2DDF0A615771__INCLUDED_

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
// CStrip wrapper class

class CStrip : public CWnd
{
protected:
	DECLARE_DYNCREATE(CStrip)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xdf75593, 0x1c7b, 0x11d0, { 0xa8, 0x9e, 0x0, 0xa0, 0x24, 0xc8, 0x5c, 0x82 } };
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
	long GetBevelInner();
	void SetBevelInner(long);
	long GetBevelOuter();
	void SetBevelOuter(long);
	short GetBevelWidth();
	void SetBevelWidth(short);
	short GetBorderWidth();
	void SetBorderWidth(short);
	double GetAreaBottom();
	void SetAreaBottom(double);
	double GetAreaTop();
	void SetAreaTop(double);
	double GetAreaLeft();
	void SetAreaLeft(double);
	double GetAreaRight();
	void SetAreaRight(double);
	OLE_COLOR GetBackColor();
	void SetBackColor(OLE_COLOR);
	CPicture GetBackPicture();
	void SetBackPicture(LPDISPATCH);
	long GetGrid();
	void SetGrid(long);
	long GetMaxBufferSize();
	void SetMaxBufferSize(long);
	short GetVariables();
	void SetVariables(short);
	short GetVariableID();
	void SetVariableID(short);
	unsigned long GetVariableColor();
	void SetVariableColor(unsigned long);
	CString GetVariableName();
	void SetVariableName(LPCTSTR);
	double GetVariableLastX();
	void SetVariableLastX(double);
	double GetVariableDeltaX();
	void SetVariableDeltaX(double);
	long GetVariableLineStyle();
	void SetVariableLineStyle(long);
	long GetVariableSymbol();
	void SetVariableSymbol(long);
	CString GetVariableSymbolUserDefined();
	void SetVariableSymbolUserDefined(LPCTSTR);
	long GetHandles();
	void SetHandles(long);
	double GetXSpan();
	void SetXSpan(double);
	short GetVariableLineWidth();
	void SetVariableLineWidth(short);
	double GetLastX();
	void SetLastX(double);
	unsigned long GetGridColor();
	void SetGridColor(unsigned long);
	short GetXMajorTics();
	void SetXMajorTics(short);
	short GetXMinorTics();
	void SetXMinorTics(short);
	unsigned long GetXTicColor();
	void SetXTicColor(unsigned long);
	short GetTracks();
	void SetTracks(short);
	short GetTrackID();
	void SetTrackID(short);
	unsigned long GetTrackBackColor();
	void SetTrackBackColor(unsigned long);
	double GetTrackDisplayMin();
	void SetTrackDisplayMin(double);
	double GetTrackDisplayMax();
	void SetTrackDisplayMax(double);
	unsigned long GetTrackTicColor();
	void SetTrackTicColor(unsigned long);
	short GetTrackMajorTics();
	void SetTrackMajorTics(short);
	short GetTrackMinorTics();
	void SetTrackMinorTics(short);
	short GetVariableTrackID();
	void SetVariableTrackID(short);
	short GetTrackTicFontID();
	void SetTrackTicFontID(short);
	unsigned long GetStampColor();
	void SetStampColor(unsigned long);
	short GetStampFontID();
	void SetStampFontID(short);
	BOOL GetTrackTicLabel();
	void SetTrackTicLabel(BOOL);
	BOOL GetXTicLabel();
	void SetXTicLabel(BOOL);
	short GetXTicFontID();
	void SetXTicFontID(short);
	short GetFonts();
	void SetFonts(short);
	short GetFontID();
	void SetFontID(short);
	COleFont GetFont();
	void SetFont(LPDISPATCH);
	CString GetFontName();
	void SetFontName(LPCTSTR);
	float GetFontSize();
	void SetFontSize(float);
	BOOL GetFontBold();
	void SetFontBold(BOOL);
	BOOL GetFontItalic();
	void SetFontItalic(BOOL);
	BOOL GetFontStrikethru();
	void SetFontStrikethru(BOOL);
	BOOL GetFontUnderline();
	void SetFontUnderline(BOOL);
	CPicture GetTrackBackPicture();
	void SetTrackBackPicture(LPDISPATCH);
	double GetTrackSeparation();
	void SetTrackSeparation(double);
	BOOL GetTrackOutline();
	void SetTrackOutline(BOOL);
	unsigned long GetTrackOutlineColor();
	void SetTrackOutlineColor(unsigned long);
	CString GetCaption();
	void SetCaption(LPCTSTR);
	unsigned long GetCaptionColor();
	void SetCaptionColor(unsigned long);
	short GetCaptionFontID();
	void SetCaptionFontID(short);
	short GetCaptionID();
	void SetCaptionID(short);
	short GetCaptions();
	void SetCaptions(short);
	double GetCaptionX();
	void SetCaptionX(double);
	double GetCaptionY();
	void SetCaptionY(double);
	long GetCaptionOrientation();
	void SetCaptionOrientation(long);
	BOOL GetPause();
	void SetPause(BOOL);
	double GetTrackMax();
	void SetTrackMax(double);
	double GetTrackMin();
	void SetTrackMin(double);
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
	long GetDisplayMode();
	void SetDisplayMode(long);
	double GetVariableSymbolSize();
	void SetVariableSymbolSize(double);
	short GetWrapCursorWidth();
	void SetWrapCursorWidth(short);
	unsigned long GetWrapCursorColor();
	void SetWrapCursorColor(unsigned long);
	long GetStampSymbol();
	void SetStampSymbol(long);
	long GetStampMode();
	void SetStampMode(long);
	short GetStampTrack();
	void SetStampTrack(short);
	double GetStampSymbolSize();
	void SetStampSymbolSize(double);
	CString GetStampSymbolUserDefined();
	void SetStampSymbolUserDefined(LPCTSTR);
	short GetXTicLabelOffset();
	void SetXTicLabelOffset(short);
	short GetTrackTicLabelOffset();
	void SetTrackTicLabelOffset(short);
	BOOL GetSingleBuffer();
	void SetSingleBuffer(BOOL);
	BOOL GetFocusOutline();
	void SetFocusOutline(BOOL);
	BOOL GetVariableVisible();
	void SetVariableVisible(BOOL);
	long GetCursorMode();
	void SetCursorMode(long);
	double GetCursorX();
	void SetCursorX(double);
	long GetVariableType();
	void SetVariableType(long);
	long GetXTicMode();
	void SetXTicMode(long);
	CString GetTimeSecondsFormat();
	void SetTimeSecondsFormat(LPCTSTR);
	CString GetTimeMinutesFormat();
	void SetTimeMinutesFormat(LPCTSTR);
	CString GetTimeHoursFormat();
	void SetTimeHoursFormat(LPCTSTR);
	CString GetTimeDaysFormat();
	void SetTimeDaysFormat(LPCTSTR);
	CString GetTimeYearsFormat();
	void SetTimeYearsFormat(LPCTSTR);
	CString GetTimeDateFormat();
	void SetTimeDateFormat(LPCTSTR);
	CString GetTimeMonthsFormat();
	void SetTimeMonthsFormat(LPCTSTR);
	BOOL GetEnablePanning();
	void SetEnablePanning(BOOL);
	BOOL GetEnableTrackSizing();
	void SetEnableTrackSizing(BOOL);
	double GetTrackStart();
	void SetTrackStart(double);
	double GetTrackStop();
	void SetTrackStop(double);
	BOOL GetAutoRedraw();
	void SetAutoRedraw(BOOL);
	double GetXDisplayMax();
	void SetXDisplayMax(double);
	double GetXDisplayMin();
	void SetXDisplayMin(double);
	OLE_HANDLE GetHWnd();
	void SetHWnd(OLE_HANDLE);
	unsigned long GetCursorColor();
	void SetCursorColor(unsigned long);
	long GetCaptionAlign();
	void SetCaptionAlign(long);
	long GetOrientation();
	void SetOrientation(long);
	long GetXTicLabelAlign();
	void SetXTicLabelAlign(long);
	long GetXTicLabelOrientation();
	void SetXTicLabelOrientation(long);
	long GetTrackXTics();
	void SetTrackXTics(long);
	long GetTrackTicLabelAlign();
	void SetTrackTicLabelAlign(long);
	long GetTrackTicLabelOrientation();
	void SetTrackTicLabelOrientation(long);
	BOOL GetVariableFill();
	void SetVariableFill(BOOL);
	BOOL GetTrackInvert();
	void SetTrackInvert(BOOL);
	long GetTrackYTics();
	void SetTrackYTics(long);
	short GetCursorTrackID();
	void SetCursorTrackID(short);
	double GetCursorY();
	void SetCursorY(double);
	CString GetConfiguration();
	void SetConfiguration(LPCTSTR);
	double GetY();
	void SetY(double);
	double Get_y();
	void Set_y(double);
	long GetStampOrientation();
	void SetStampOrientation(long);
	short GetTrackBands();
	void SetTrackBands(short);
	short GetTrackBandID();
	void SetTrackBandID(short);
	unsigned long GetTrackBandColor();
	void SetTrackBandColor(unsigned long);
	double GetTrackBandStart();
	void SetTrackBandStart(double);
	double GetTrackBandStop();
	void SetTrackBandStop(double);
	CPicture GetTrackBandPicture();
	void SetTrackBandPicture(LPDISPATCH);
	BOOL GetEnabled();
	void SetEnabled(BOOL);

// Operations
public:
	void AddXY(short Index, double X, double Y);
	void AddY(short Index, double Y);
	void ClearAll();
	void AddYArray(short Index, VARIANT* Array);
	void AddXYArray(short Index, VARIANT* Array);
	void ClearVariable(short VariableID);
	void ClearTrack(short TrackID);
	void ClearStamps();
	void AddStamp(double Y, LPCTSTR Text);
	void AddSymbol(double Y);
	void ShowPropertyPage();
	double GetCursorValue(short VariableID);
	void SetCursorValue(short VariableID, double newValue);
	double GetVariableVisibleMax(short VariableID);
	void SetVariableVisibleMax(short VariableID, double newValue);
	double GetVariableVisibleMin(short VariableID);
	void SetVariableVisibleMin(short VariableID, double newValue);
	double GetVariableVisibleAvg(short VariableID);
	void SetVariableVisibleAvg(short VariableID, double newValue);
	double GetVariableMax(short VariableID);
	void SetVariableMax(short VariableID, double newValue);
	double GetVariableMin(short VariableID);
	void SetVariableMin(short VariableID, double newValue);
	double GetVariableAvg(short VariableID);
	void SetVariableAvg(short VariableID, double newValue);
	void Redraw();
	void RedrawStatic();
	void AddStampXY(double X, double Y, LPCTSTR Text);
	void AddSymbolXY(double X, double Y);
	void PrintArea(long hDC, double OffsetX, double OffsetY, double Width, double Height);
	void ClearBefore(double X);
	void ClearAfter(double X);
	void SetDisplay(double DisplayMax, double DisplayMin);
	double GetVariableVisibleStdDev(short VariableID);
	void SetVariableVisibleStdDev(short VariableID, double newValue);
	double GetVariableStdDev(short VariableID);
	void SetVariableStdDev(short VariableID, double newValue);
	void AddXYNoSort(short Index, double X, double Y);
	void AboutBox();
	void SetConfigurationEx(LPCTSTR ConfigName, LPCTSTR ConfigFile);
	void SetStampBufferMax(long bufferSize);
	void AddYArrayV(short Index, const VARIANT& Array);
	void AddXYArrayV(short Index, const VARIANT& Array);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRIP_H__417401CA_594F_4EC4_890F_2DDF0A615771__INCLUDED_)
