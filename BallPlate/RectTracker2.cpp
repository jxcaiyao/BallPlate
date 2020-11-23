// RectTracker2.cpp: implementation of the CRectTracker2 class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "RectTracker2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRectTracker2::CRectTracker2()
:m_Color(0)
{

}

CRectTracker2::~CRectTracker2()
{

}

//////////////////////////////////////////////////////////////////////////
void CRectTracker2::DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo,
								CDC* pDC, CWnd* pWnd)
{
	CPen Pen, *pOldPen;
	Pen.CreatePen(PS_SOLID, 1, m_Color);
	pOldPen = pDC->SelectObject(&Pen);

	CRectTracker::DrawTrackerRect(lpRect, pWndClipTo, pDC, pWnd);

	pDC->SelectObject(pOldPen);
}

//
// various GDI objects we need to draw
AFX_STATIC_DATA HCURSOR _afxCursors[10] = { 0, };
AFX_STATIC_DATA HBRUSH _afxHatchBrush = 0;
AFX_STATIC_DATA HPEN _afxBlackDottedPen = 0;
AFX_STATIC_DATA int _afxHandleSize = 0;

void CRectTracker2::Draw2(CDC* pDC) const
{
	if( m_rect.IsRectEmpty() )
		return ;
	// set initial DC state
	VERIFY(pDC->SaveDC() != 0);
	pDC->SetMapMode(MM_TEXT);
	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0, 0);

	// get normalized rectangle
	CRect rect = m_rect;
	rect.NormalizeRect();

	CPen* pOldPen = NULL;
	CPen Pen;//added by espring.
	CBrush* pOldBrush = NULL;
	CGdiObject* pTemp;
	int nOldROP;
	
	// draw lines
	if ((m_nStyle & (dottedLine|solidLine)) != 0)
	{
		if (m_nStyle & dottedLine)
			pOldPen = pDC->SelectObject(CPen::FromHandle(_afxBlackDottedPen));
		else
//			pOldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);//here changed!
		{
			Pen.CreatePen(PS_SOLID, 1, m_Color);
			pOldPen = (CPen *)pDC->SelectObject(&Pen);
		}
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		nOldROP = pDC->SetROP2(R2_COPYPEN);
		rect.InflateRect(+1, +1);   // borders are one pixel outside
		pDC->Ellipse(rect.left, rect.top, rect.right, rect.bottom);
		pDC->SetROP2(nOldROP);
	}

	// if hatchBrush is going to be used, need to unrealize it
	if ((m_nStyle & (hatchInside|hatchedBorder)) != 0)
		UnrealizeObject(_afxHatchBrush);

	// hatch inside
	if ((m_nStyle & hatchInside) != 0)
	{
		pTemp = pDC->SelectStockObject(NULL_PEN);
		if (pOldPen == NULL)
			pOldPen = (CPen*)pTemp;
		pTemp = pDC->SelectObject(CBrush::FromHandle(_afxHatchBrush));
		if (pOldBrush == NULL)
			pOldBrush = (CBrush*)pTemp;
		pDC->SetBkMode(TRANSPARENT);
		nOldROP = pDC->SetROP2(R2_MASKNOTPEN);
		pDC->Ellipse(rect.left+1, rect.top+1, rect.right, rect.bottom);
		pDC->SetROP2(nOldROP);
	}

	// draw hatched border
	if ((m_nStyle & hatchedBorder) != 0)
	{
		pTemp = pDC->SelectObject(CBrush::FromHandle(_afxHatchBrush));
		if (pOldBrush == NULL)
			pOldBrush = (CBrush*)pTemp;
		pDC->SetBkMode(OPAQUE);
		CRect rectTrue;
		GetTrueRect(&rectTrue);
		pDC->PatBlt(rectTrue.left, rectTrue.top, rectTrue.Width(),
			rect.top-rectTrue.top, 0x000F0001 /* Pn */);
		pDC->PatBlt(rectTrue.left, rect.bottom,
			rectTrue.Width(), rectTrue.bottom-rect.bottom, 0x000F0001 /* Pn */);
		pDC->PatBlt(rectTrue.left, rect.top, rect.left-rectTrue.left,
			rect.Height(), 0x000F0001 /* Pn */);
		pDC->PatBlt(rect.right, rect.top, rectTrue.right-rect.right,
			rect.Height(), 0x000F0001 /* Pn */);
	}

	// draw resize handles
/*
	if ((m_nStyle & (resizeInside|resizeOutside)) != 0)
	{
		UINT mask = GetHandleMask();
		for (int i = 0; i < 8; ++i)
		{
			if (mask & (1<<i))
			{
				GetHandleRect((TrackerHit)i, &rect);
				pDC->FillSolidRect(rect, RGB(255, 255, 255));
			}
		}
	}
*/

	// cleanup pDC state
	if (pOldPen != NULL)
		pDC->SelectObject(pOldPen);
	if (pOldBrush != NULL)
		pDC->SelectObject(pOldBrush);
	VERIFY(pDC->RestoreDC(-1));
	//pOldPen->DeleteObject();

	if( !m_strTitle.IsEmpty() )
	{
		pDC->SetBkMode(TRANSPARENT);

		COLORREF clrText = 0xff00;
		pDC->SetTextColor(clrText);
		pDC->TextOut(m_rect.right, m_rect.bottom, m_strTitle);
	}
}
