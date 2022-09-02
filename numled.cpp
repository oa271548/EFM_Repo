// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "numled.h"

// Dispatch interfaces referenced by this interface
#include "picture.h"

/////////////////////////////////////////////////////////////////////////////
// CNumLED

IMPLEMENT_DYNCREATE(CNumLED, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CNumLED properties

short CNumLED::GetDigits()
{
	short result;
	GetProperty(0x1, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetDigits(short propVal)
{
	SetProperty(0x1, VT_I2, propVal);
}

short CNumLED::GetDecimals()
{
	short result;
	GetProperty(0x2, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetDecimals(short propVal)
{
	SetProperty(0x2, VT_I2, propVal);
}

OLE_COLOR CNumLED::GetBackColor()
{
	OLE_COLOR result;
	GetProperty(DISPID_BACKCOLOR, VT_I4, (void*)&result);
	return result;
}

void CNumLED::SetBackColor(OLE_COLOR propVal)
{
	SetProperty(DISPID_BACKCOLOR, VT_I4, propVal);
}

CPicture CNumLED::GetBackPicture()
{
	LPDISPATCH pDispatch;
	GetProperty(0x3, VT_DISPATCH, (void*)&pDispatch);
	return CPicture(pDispatch);
}

void CNumLED::SetBackPicture(LPDISPATCH propVal)
{
	SetProperty(0x3, VT_DISPATCH, propVal);
}

long CNumLED::GetBevelInner()
{
	long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

void CNumLED::SetBevelInner(long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}

long CNumLED::GetBevelOuter()
{
	long result;
	GetProperty(0x5, VT_I4, (void*)&result);
	return result;
}

void CNumLED::SetBevelOuter(long propVal)
{
	SetProperty(0x5, VT_I4, propVal);
}

unsigned long CNumLED::GetOnColor()
{
	unsigned long result;
	GetProperty(0x6, VT_I4, (void*)&result);
	return result;
}

void CNumLED::SetOnColor(unsigned long propVal)
{
	SetProperty(0x6, VT_I4, propVal);
}

unsigned long CNumLED::GetOffColor()
{
	unsigned long result;
	GetProperty(0x7, VT_I4, (void*)&result);
	return result;
}

void CNumLED::SetOffColor(unsigned long propVal)
{
	SetProperty(0x7, VT_I4, propVal);
}

double CNumLED::GetValue()
{
	double result;
	GetProperty(0x8, VT_R8, (void*)&result);
	return result;
}

void CNumLED::SetValue(double propVal)
{
	SetProperty(0x8, VT_R8, propVal);
}

double CNumLED::Get_Value()
{
	double result;
	GetProperty(0x0, VT_R8, (void*)&result);
	return result;
}

void CNumLED::Set_Value(double propVal)
{
	SetProperty(0x0, VT_R8, propVal);
}

short CNumLED::GetBevelWidth()
{
	short result;
	GetProperty(0x9, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetBevelWidth(short propVal)
{
	SetProperty(0x9, VT_I2, propVal);
}

short CNumLED::GetBorderWidth()
{
	short result;
	GetProperty(0xa, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetBorderWidth(short propVal)
{
	SetProperty(0xa, VT_I2, propVal);
}

BOOL CNumLED::GetLeadingZeros()
{
	BOOL result;
	GetProperty(0xb, VT_BOOL, (void*)&result);
	return result;
}

void CNumLED::SetLeadingZeros(BOOL propVal)
{
	SetProperty(0xb, VT_BOOL, propVal);
}

short CNumLED::GetSpacingHorizontal()
{
	short result;
	GetProperty(0xc, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetSpacingHorizontal(short propVal)
{
	SetProperty(0xc, VT_I2, propVal);
}

short CNumLED::GetSpacingVertical()
{
	short result;
	GetProperty(0xd, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetSpacingVertical(short propVal)
{
	SetProperty(0xd, VT_I2, propVal);
}

BOOL CNumLED::GetFixedDecimal()
{
	BOOL result;
	GetProperty(0xe, VT_BOOL, (void*)&result);
	return result;
}

void CNumLED::SetFixedDecimal(BOOL propVal)
{
	SetProperty(0xe, VT_BOOL, propVal);
}

short CNumLED::GetItalicsOffset()
{
	short result;
	GetProperty(0xf, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetItalicsOffset(short propVal)
{
	SetProperty(0xf, VT_I2, propVal);
}

short CNumLED::GetSegmentWidth()
{
	short result;
	GetProperty(0x10, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetSegmentWidth(short propVal)
{
	SetProperty(0x10, VT_I2, propVal);
}

short CNumLED::GetSegmentSeparation()
{
	short result;
	GetProperty(0x11, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetSegmentSeparation(short propVal)
{
	SetProperty(0x11, VT_I2, propVal);
}

BOOL CNumLED::GetLeadingOneDigit()
{
	BOOL result;
	GetProperty(0x12, VT_BOOL, (void*)&result);
	return result;
}

void CNumLED::SetLeadingOneDigit(BOOL propVal)
{
	SetProperty(0x12, VT_BOOL, propVal);
}

long CNumLED::GetDisplayMode()
{
	long result;
	GetProperty(0x13, VT_I4, (void*)&result);
	return result;
}

void CNumLED::SetDisplayMode(long propVal)
{
	SetProperty(0x13, VT_I4, propVal);
}

short CNumLED::GetDecimalSize()
{
	short result;
	GetProperty(0x14, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetDecimalSize(short propVal)
{
	SetProperty(0x14, VT_I2, propVal);
}

short CNumLED::GetDecimalPosition()
{
	short result;
	GetProperty(0x15, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetDecimalPosition(short propVal)
{
	SetProperty(0x15, VT_I2, propVal);
}

BOOL CNumLED::GetLeadingPlusMinus()
{
	BOOL result;
	GetProperty(0x16, VT_BOOL, (void*)&result);
	return result;
}

void CNumLED::SetLeadingPlusMinus(BOOL propVal)
{
	SetProperty(0x16, VT_BOOL, propVal);
}

CString CNumLED::GetAlphaNumeric()
{
	CString result;
	GetProperty(0x17, VT_BSTR, (void*)&result);
	return result;
}

void CNumLED::SetAlphaNumeric(LPCTSTR propVal)
{
	SetProperty(0x17, VT_BSTR, propVal);
}

BOOL CNumLED::GetOutline()
{
	BOOL result;
	GetProperty(0x18, VT_BOOL, (void*)&result);
	return result;
}

void CNumLED::SetOutline(BOOL propVal)
{
	SetProperty(0x18, VT_BOOL, propVal);
}

long CNumLED::GetOutlineAlign()
{
	long result;
	GetProperty(0x19, VT_I4, (void*)&result);
	return result;
}

void CNumLED::SetOutlineAlign(long propVal)
{
	SetProperty(0x19, VT_I4, propVal);
}

unsigned long CNumLED::GetOutlineColor()
{
	unsigned long result;
	GetProperty(0x1a, VT_I4, (void*)&result);
	return result;
}

void CNumLED::SetOutlineColor(unsigned long propVal)
{
	SetProperty(0x1a, VT_I4, propVal);
}

CString CNumLED::GetOutlineTitle()
{
	CString result;
	GetProperty(0x1b, VT_BSTR, (void*)&result);
	return result;
}

void CNumLED::SetOutlineTitle(LPCTSTR propVal)
{
	SetProperty(0x1b, VT_BSTR, propVal);
}

short CNumLED::GetOutlineWidth()
{
	short result;
	GetProperty(0x1c, VT_I2, (void*)&result);
	return result;
}

void CNumLED::SetOutlineWidth(short propVal)
{
	SetProperty(0x1c, VT_I2, propVal);
}

BOOL CNumLED::GetSingleBuffer()
{
	BOOL result;
	GetProperty(0x1d, VT_BOOL, (void*)&result);
	return result;
}

void CNumLED::SetSingleBuffer(BOOL propVal)
{
	SetProperty(0x1d, VT_BOOL, propVal);
}

BOOL CNumLED::GetFocusOutline()
{
	BOOL result;
	GetProperty(0x1e, VT_BOOL, (void*)&result);
	return result;
}

void CNumLED::SetFocusOutline(BOOL propVal)
{
	SetProperty(0x1e, VT_BOOL, propVal);
}

OLE_HANDLE CNumLED::GetHWnd()
{
	OLE_HANDLE result;
	GetProperty(DISPID_HWND, VT_I4, (void*)&result);
	return result;
}

void CNumLED::SetHWnd(OLE_HANDLE propVal)
{
	SetProperty(DISPID_HWND, VT_I4, propVal);
}

BOOL CNumLED::GetEnabled()
{
	BOOL result;
	GetProperty(DISPID_ENABLED, VT_BOOL, (void*)&result);
	return result;
}

void CNumLED::SetEnabled(BOOL propVal)
{
	SetProperty(DISPID_ENABLED, VT_BOOL, propVal);
}

CString CNumLED::GetConfiguration()
{
	CString result;
	GetProperty(0x1f, VT_BSTR, (void*)&result);
	return result;
}

void CNumLED::SetConfiguration(LPCTSTR propVal)
{
	SetProperty(0x1f, VT_BSTR, propVal);
}

BOOL CNumLED::GetAutoRedraw()
{
	BOOL result;
	GetProperty(0x20, VT_BOOL, (void*)&result);
	return result;
}

void CNumLED::SetAutoRedraw(BOOL propVal)
{
	SetProperty(0x20, VT_BOOL, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// CNumLED operations

void CNumLED::ShowPropertyPage()
{
	InvokeHelper(0x21, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CNumLED::Redraw()
{
	InvokeHelper(0x22, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CNumLED::RedrawStatic()
{
	InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CNumLED::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}