
#ifndef _VIEW_SIGNAL_H_
#define _VIEW_SIGNAL_H_

struct SViewSignal
{
    enum{NoPen = -1L, NoBrush = -1L};
    enum{Ty_Line, Ty_Rect};
    BYTE	m_Type;
    CRect	m_Rect;
    long	m_clrPen; //if = -1, then No Pen
    long	m_clrFill;//if = -1, then No Brush

    SViewSignal()
    {
        m_Type = Ty_Rect;
        m_clrPen = -1;
        m_clrFill = -1;
    }
};

#include <vector>

template<class T>
class CViewSignalImpl 
{
    typedef std::vector<SViewSignal>	ViewSignalVect;
protected:
    CViewSignalImpl()
    {
        m_pThis = (T*)this;
        m_vtViewSignal.reserve(30);
    }
    ~CViewSignalImpl()
    {
    }

public:
    void VSI_Clear()
    {
        m_vtViewSignal.clear();
    }
    void VSI_AddSignal(const SViewSignal& signal)
    {
        m_vtViewSignal.push_back(signal);
    }

protected:
    void VSI_OnDraw(CDC &dc)
    {
        
        ViewSignalVect::const_iterator cit;
        for( cit = m_vtViewSignal.begin(); cit!=m_vtViewSignal.end(); ++cit )
        {
            CPen Pen, *pOldPen;
            if( cit->m_clrPen == SViewSignal::NoPen )
            {
                Pen.CreatePen(PS_NULL, 1, COLORREF(0));
            }
            else{
                Pen.CreatePen(PS_SOLID, 1, COLORREF(cit->m_clrPen));
            }
            pOldPen = dc.SelectObject(&Pen);

            //
            CBrush Brush, *pOldBrush;
            if( cit->m_clrFill == SViewSignal::NoBrush )
            {
                Brush.Attach(::GetStockObject(NULL_BRUSH));
            }
            else{
                Brush.CreateSolidBrush(COLORREF(cit->m_clrFill));
            }
            pOldBrush = dc.SelectObject(&Brush);

            //
            if( cit->m_Type == SViewSignal::Ty_Rect )
            {
                dc.Rectangle(cit->m_Rect );
            }
            else if(  cit->m_Type == SViewSignal::Ty_Line )
            {
                dc.MoveTo(cit->m_Rect.TopLeft());
                dc.LineTo(cit->m_Rect.BottomRight());
            }

            //
            dc.SelectObject(pOldBrush);
            dc.SelectObject(pOldPen);
        }
    }
protected:
    ViewSignalVect			m_vtViewSignal;

private:
    T *					m_pThis;
};
#endif //_VIEW_SIGNAL_H_