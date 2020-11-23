// DispVision.cpp : implementation file
//

#include "pch.h"
#include "BallPlate.h"
#include "MilVision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMilVision

CMilVision::CMilVision()
{
}

CMilVision::~CMilVision()
{
}


BEGIN_MESSAGE_MAP(CMilVision, CWnd)
	//{{AFX_MSG_MAP(CMilVision)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMilVision message handlers

void CMilVision::InitialVision()
{	/*initialization of image feedback system*/     
    //metero II��
	m_pDataArray = NULL; 
	m_pProcDataArray = (byte *)malloc(768*576*sizeof(byte));

	MappAlloc(M_DEFAULT, &MilApplication); //ָ��MILӦ��
	MsysAlloc(M_DEF_SYSTEM_TYPE, M_DEF_SYSTEM_NUM, M_DEFAULT, &MilSystem);  //ָ��Ӳ��ϵͳ
	MdigAlloc(MilSystem, M_DEFAULT, TEXT("M_PAL"), M_DEFAULT, &MilDigitizer); 
	MdispAlloc(MilSystem, M_DEF_DISPLAY_NUM, M_DEF_DISPLAY_FORMAT, M_WINDOWED, &MilDisplay);//
	MbufAllocColor(MilSystem, 1, 768, 576, 8+M_UNSIGNED,
			 (MilDigitizer? M_IMAGE+M_DISP+M_GRAB : M_IMAGE+M_DISP), &MilImage);
	MdigGrab(MilDigitizer, MilImage);
	if(MbufInquire(MilImage,M_SIZE_BIT,M_NULL)==8)
		MbufInquire(MilImage,M_HOST_ADDRESS,&m_pDataArray);	
	
/*
 //MORPHIS��	
	m_pDataArray = NULL; 
	m_pProcDataArray = (byte *)malloc(768*576*sizeof(byte));

	MappAlloc(M_DEFAULT, &MilApplication); //ָ��MILӦ��
	MsysAlloc(M_SYSTEM_MORPHIS, M_DEF_SYSTEM_NUM, M_DEFAULT, &MilSystem);  //ָ��Ӳ��ϵͳ
	MdispAlloc(MilSystem,M_DEF_DISPLAY_NUM,"M_PAL",M_WINDOWED,&MilDisplay);
	MdigAlloc(MilSystem, M_DEV0, M_DEF_DIGITIZER_FORMAT, M_DEFAULT, &MilDigitizer); 
	MdigChannel(MilDigitizer,M_CH0);//ѡ��ͨ��
	MbufAllocColor(MilSystem, 1, 768, 576, 8+M_UNSIGNED,(MilDigitizer? M_IMAGE+M_DISP+M_GRAB : M_IMAGE+M_DISP), &MilImage);
	MdigGrab(MilDigitizer,MilImage);
	
	if(MbufInquire(MilImage,M_SIZE_BIT,M_NULL)==8)
		MbufInquire(MilImage,M_HOST_ADDRESS,&m_pDataArray);	
	*/
}

void CMilVision::GrabContinuous(HWND m_MyView) 
{
	MdigHalt(MilDigitizer);
	MdigGrabContinuous(MilDigitizer, MilImage); //����ץȡ��ʾ
	MdispSelectWindow(MilDisplay, MilImage,m_MyView);//������ʾ����
}

void CMilVision::ImageFrameGrab() 
{//��̬ץͼ
	MdigHalt(MilDigitizer);
	MdigGrab(MilDigitizer, MilImage);
	if(MbufInquire(MilImage,M_SIZE_BIT,M_NULL)==8)
		MbufInquire(MilImage,M_HOST_ADDRESS,&m_pDataArray);
}

BOOL CMilVision::BivalueCentroid(int *xPos, int *yPos,CRect rect)
{
	unsigned int xSum, ySum,potNum;
	int startX,endX,startY,endY;
	float centerx,centery;

	//ɨ�跶ΧΪ��Բ
	startY = rect.top;
	endY = rect.bottom;
	startX = rect.left;
	endX = rect.right;

	centery=(float)(endY+startY)/2;
	centerx=(float)(endX+startX)/2;
	
	xSum = ySum = potNum = 0; 
	for(int i = startY; i < endY; i++)
	{
		startX =(int)(centerx-sqrt((rect.Width()*rect.Width()/4)*(1-(((i-centery)*(i-centery))/(rect.Height()*rect.Height()/4)))));
	    endX =(int)(centerx+sqrt((rect.Width()*rect.Width()/4)*(1-(((i-centery)*(i-centery))/(rect.Height()*rect.Height()/4)))));

		for(int j = startX; j < endX; j++)
		{
			if(m_pProcDataArray[i * 768 + j] == 255)//1024*768
			{
				ySum += i;
				xSum += j;
				potNum++;
			}			
			else
			{
				m_pProcDataArray[i * 768 + j]=0;
			}
		}
	}
	if(potNum == 0)
		return FALSE;
	else
	{
		*xPos = xSum/potNum;
		*yPos = ySum/potNum;
	}
	return TRUE;	
}

void CMilVision::MiLDestroy()
{
//	free(m_pDataArray);
//	free(m_pProcDataArray);
	MdigHalt(MilDigitizer);
	MappFreeDefault(MilApplication, MilSystem, MilDisplay,
								  MilDigitizer, MilImage);
}

/* obtain centroid directly from gray image */
BOOL CMilVision::GrayCentroid(CRect rect,int  *xPos,int  *yPos)
{
	int i,j;
	unsigned int xSum, ySum,potNum;
	int startY,endY,startX,endX;

	startY = rect.top;
	endY = rect.bottom;
	startX = rect.left;
	endX = rect.right;

	xSum = ySum = potNum = 0; 
    for(i = startY;i < endY;i ++)
	{
		for(j = startX;j < endX;j ++)
		{
			if(m_pProcDataArray[i * 768 + j] == 255/*0*/)
			{
				ySum += i;
				xSum += j;
				potNum++;
			}			
			else
			{
				m_pProcDataArray[i * 768 + j]=0/*255*/;
			}
		}
	}
	if(potNum == 0)
	{		
		return FALSE;
	}
	else
	{
		*xPos = xSum/potNum;
		*yPos = ySum/potNum;
	}

	return TRUE;
}

void CMilVision::DisplayImage(byte *m_pProcDataArray,CClientDC *pDC)
{
	if(m_pProcDataArray!=NULL)
	{ 				
		pDC->SetBkColor(RGB(0,0,0));
		int drawWidth=768;//768
		int drawHeight=576;	//576	
		for(int i=0;i<drawHeight;i++)
		{	  
			for(int j=0;j<drawWidth;j++)
			{
				pDC->SetPixelV(j,i,RGB( m_pProcDataArray[i*drawWidth+j],
										m_pProcDataArray[i*drawWidth+j],
										m_pProcDataArray[i*drawWidth+j] ));
			}
		}
	}
}

/* convert gray image to bivalue image */
void CMilVision::Gray2Bivalue(CRect rect)
{
	int startY,endY,startX,endX;
	startY = rect.top;
	endY = rect.bottom;
	startX = rect.left;
	endX = rect.right;
	for(int i =startY ; i < endY ; i++)
	{
	    for(int j = startX; j < endX ; j++) 
		{
			if (m_pDataArray[i * 768 + j] > 150)
			{
	            m_pProcDataArray[i * 768 + j] = 255;
			}
	        else 
			{
				m_pProcDataArray[i * 768 + j] = 0;
			}
		}
	}
}

void CMilVision::RobertEdge()
{
	int i,j;
	unsigned char pixel[4];

	for(j = 576; j > 0; j--)
	{
		for(i = 0; i <= 768; i++)
		{
			pixel[0] = m_pDataArray[768 * j + i];
			pixel[1] = m_pDataArray[768 * j + i + 1];
			pixel[2] = m_pDataArray[768 * j + i - 768];
			pixel[3] = m_pDataArray[768 * j + i - 768 + 1];

			m_pProcDataArray[768 * j + i] = (unsigned char)sqrt(( pixel[0] - pixel[3] )*( pixel[0] - pixel[3] ) 
				                          + ( pixel[1] - pixel[2] )*( pixel[1] - pixel[2] ));
		}
	}
}

void CMilVision::GaussSmooth()
{
	int   iTempH = 3;                //the height of the template
	int   iTempW = 3;                //the width of the template
	int   iTempMX = 1;               //the center x of the template
	int   iTempMY = 1;               //the center y of the template
	float fResult;
	float coef = 0.0625;
	float fpArray[9] = { 1.0 , 2.0 , 1.0 ,
	                     2.0 , 4.0 , 2.0 ,
						 1.0 , 2.0 , 1.0 };

	for(int i = iTempMY; i < 576 - iTempH + iTempMY + 1; i++)
	{
		for(int j = iTempMX; j < 768 - iTempW + iTempMX + 1; j++)
		{
			fResult = 0;
			for(int k = 0; k < iTempH; k ++)
			{
				for(int l = 0; l < iTempW; l++)
				{
					fResult += m_pDataArray[(i - iTempMY + k) * 768 + j - iTempMX + l]
						       * fpArray[k * iTempW + l];
				}
			}
			
			fResult = coef * fResult;
			if(fResult > 255)
			{
				m_pProcDataArray[i * 768 + j] = 255;
			}
			else
			{
				m_pProcDataArray[i * 768 + j] = (unsigned char)fResult;
			}			
		}
	}
}

void CMilVision::MediumFilter()
{
	int   iTempH = 3;                //the height of the template
	int   iTempW = 3;                //the width of the template
	int   iTempMX = 1;               //the center x of the template
	int   iTempMY = 1;               //the center y of the template
	
	unsigned char *aTemp = new unsigned char [9];
	for(int i = iTempMY; i < 576 - iTempH + iTempMY + 1; i++)
	{
		for(int j = iTempMX; j < 768 - iTempH + iTempMX + 1; j++)
		{
			for(int k = 0; k < iTempH; k ++)
			{
				for(int l = 0; l < iTempW; l++)
				{
					aTemp[k * iTempW + l] = m_pDataArray[(i - iTempMY + k) * 768 + j - iTempMX + l];
				}
			}
			
			m_pProcDataArray[i * 768 + j] = GetMediumValue(aTemp,iTempH * iTempW);
		}
	}
	delete[] aTemp;
}

unsigned char CMilVision::GetMediumValue(unsigned char *bArray,int iFilterLen)
{
	// ѭ������
	int		i;
	int		j;
	
	// �м����
	unsigned char bTemp;
	
	// ��ð�ݷ��������������

	for (j = 0; j < iFilterLen - 1; j ++)
	{
		for (i = 0; i < iFilterLen - j - 1; i ++)
		{
			if (bArray[i] > bArray[i + 1])
			{
				// ����
				bTemp = bArray[i];
				bArray[i] = bArray[i + 1];
				bArray[i + 1] = bTemp;
			}
		}
	}

	
	// ������ֵ
	
	if ((iFilterLen & 1) > 0)
	{
		// ������������Ԫ�أ������м�һ��Ԫ��
		bTemp = bArray[(iFilterLen + 1) / 2];
	}
	else
	{
		// ������ż����Ԫ�أ������м�����Ԫ��ƽ��ֵ
		bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
	}
	// ������ֵ
	return bTemp;
}


