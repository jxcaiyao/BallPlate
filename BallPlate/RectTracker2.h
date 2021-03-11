// RectTracker2.h: interface for the CRectTracker2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTTRACKER2_H__4BB8FB79_7D88_48D6_9747_B221DB0EC83B__INCLUDED_)
#define AFX_RECTTRACKER2_H__4BB8FB79_7D88_48D6_9747_B221DB0EC83B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRectTracker2 : public CRectTracker  
{
public:
    CRectTracker2();
    virtual ~CRectTracker2();

public:
    virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo,
                                CDC* pDC, CWnd* pWnd);
    void Draw2(CDC* pDC) const;

public:
    long			m_lStrokeWidth;
    COLORREF		m_Color;
    CString			m_strTitle;
};

#endif // !defined(AFX_RECTTRACKER2_H__4BB8FB79_7D88_48D6_9747_B221DB0EC83B__INCLUDED_)
