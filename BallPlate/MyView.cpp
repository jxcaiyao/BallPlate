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

}

BOOL CMyView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
    return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
