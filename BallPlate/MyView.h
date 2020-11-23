#if !defined(AFX_MYVIEW_H__52E6707F_199C_42BF_8E7E_47CD12C775E2__INCLUDED_)
#define AFX_MYVIEW_H__52E6707F_199C_42BF_8E7E_47CD12C775E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyView.h : header file
//
#include "RectTracker2.h"
#include "ViewSigal.h"


/////////////////////////////////////////////////////////////////////////////
class CMyView : public CWnd,
				public CViewSignalImpl<CMyView>
{
// Construction
public:
	CMyView();           // protected constructor used by dynamic creation
	virtual ~CMyView();
	friend class CViewSignalImpl<CMyView>;
	//DECLARE_DYNCREATE(CMyView)

// Attributes
public:

// Operations
public:
	UINT nCalibrateStep;
	UINT nCalibrateCount;
	BOOL bCaliFlag;        //Image Calibration Flag
	BOOL bSetBoundary;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyView)
	//protected:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	CRectTracker2	m_RectTracker;

protected:	
	virtual void	OnDraw(CDC* pDC);
protected:
	int Cali_xpos,Cali_ypos;//标定时鼠标选取的位置（图像坐标系中）
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CMyView)

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CMyView *m_MyView;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYVIEW_H__52E6707F_199C_42BF_8E7E_47CD12C775E2__INCLUDED_)
