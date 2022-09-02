// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "lightningalarm.h"

/////////////////////////////////////////////////////////////////////////////
// CLightningAlarm

IMPLEMENT_DYNCREATE(CLightningAlarm, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CLightningAlarm properties

/////////////////////////////////////////////////////////////////////////////
// CLightningAlarm operations

void CLightningAlarm::SetColourControlBackground(unsigned long newColour)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 newColour);
}

void CLightningAlarm::SetColourRingBorder(unsigned long newColour)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 newColour);
}

void CLightningAlarm::SetHighFieldAlarm(long bActive)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bActive);
}

void CLightningAlarm::SetVeryHighFieldAlarm(long bActive)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bActive);
}

void CLightningAlarm::SetLightningAlarm(long bActive, double dDistance)
{
	static BYTE parms[] =
		VTS_I4 VTS_R8;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bActive, dDistance);
}

void CLightningAlarm::SetDistanceRings(double* pdRingDef, short sRingCount)
{
	static BYTE parms[] =
		VTS_PR8 VTS_I2;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pdRingDef, sRingCount);
}

void CLightningAlarm::SetEnabled(long bEnabled)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 bEnabled);
}

void CLightningAlarm::SetUnitText(LPCTSTR szUnitText)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 szUnitText);
}