// MyView.cpp : implementation file
//

#include "pch.h"
#include "BallPlate.h"
#include "MyView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyView
BYTE *SPtrBYTE; 
//IMPLEMENT_DYNCREATE(CMyView, CView)

CMyView::CMyView()
{
	nCalibrateCount=0;
	nCalibrateStep=0;
	bCaliFlag=FALSE;

	bSetBoundary=FALSE;
	m_RectTracker.m_rect = CRect(0, 0, 0, 0);
	m_RectTracker.m_nStyle = CRectTracker::solidLine | CRectTracker::resizeOutside;
	m_RectTracker.m_Color = RGB(255,0,255);
}

CMyView::~CMyView()
{
}


BEGIN_MESSAGE_MAP(CMyView, CWnd)
	//{{AFX_MSG_MAP(CMyView)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyView message handlers
void CMyView::OnDraw(CDC *pDC)
{
   if( pDC == NULL )
		return ;
	if (!m_RectTracker.m_rect.IsRectEmpty())
	{
		m_RectTracker.Draw2(pDC);

	}
	//VSI_OnDraw(*pDC);
}

BOOL CMyView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
