#if !defined(AFX_MYVIEW_H__52E6707F_199C_42BF_8E7E_47CD12C775E2__INCLUDED_)
#define AFX_MYVIEW_H__52E6707F_199C_42BF_8E7E_47CD12C775E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
class CMyView : public CWnd
{
// Construction
public:
	CMyView();           // protected constructor used by dynamic creation
	virtual ~CMyView();
	//DECLARE_DYNCREATE(CMyView)

// Attributes
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyView)
	//protected:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

protected:	
	virtual void	OnDraw(CDC* pDC);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYVIEW_H__52E6707F_199C_42BF_8E7E_47CD12C775E2__INCLUDED_)
