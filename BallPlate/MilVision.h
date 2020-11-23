#if !defined(AFX_DISPVISION_H__2AEC5223_77CA_43DE_BE96_6C6FF87C69BC__INCLUDED_)
#define AFX_DISPVISION_H__2AEC5223_77CA_43DE_BE96_6C6FF87C69BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MilVision.h : header file

#include "Mil.h"
#include <math.h>

typedef struct {
	double x;
	double y;
	double z;
}wPoint;

typedef struct {
	double x;
	double y;
}pPoint;

typedef struct {
	float m11,m12,m13,m14;
	float m21,m22,m23,m24;
	float m31,m32,m33,m34;
}Matrix;

/////////////////////////////////////////////////////////////////////////////
// CDispVision window

class CMilVision : public CWnd
{
// Construction
public:
	CMilVision();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMilVision)
	//}}AFX_VIRTUAL

// Implementation
public:	
	BYTE *m_pDataArray;          // the buffer of the image array
	BYTE *m_pProcDataArray;      // the buffer of the image array having been processed
	void InitialVision();
	void ImageFrameGrab();
	void GrabContinuous(HWND m_MyView);
	void MiLDestroy();		
	
	BOOL BivalueCentroid(int *xPos, int *yPos,CRect rect);//ellispe
	void DisplayImage(byte *m_pProcDataArray,CClientDC *pDC);
	BOOL GrayCentroid(CRect rect,int *xPos,int *yPos);//retangle
	void Gray2Bivalue(CRect rect);

	void RobertEdge();
	void GaussSmooth();//smooth an image using Gauss Template
	void MediumFilter();
	unsigned char GetMediumValue(unsigned char *bArray,int iFilterLen);
		
	virtual ~CMilVision();

private:

	MIL_ID		MilApplication,  /* MIL Application identifier.  */
				MilSystem,       /* MIL System identifier.       */
				MilDisplay,      /* MIL Display identifier.      */
				MilDigitizer,    /* MIL Digitizer identifier.    */
				MilImage;        /* MIL Image buffer identifier. */
////////////////////////////////////////////////////////////////////////////
	// Generated message map functions
protected:
	//{{AFX_MSG(CMilVision)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPVISION_H__2AEC5223_77CA_43DE_BE96_6C6FF87C69BC__INCLUDED_)
