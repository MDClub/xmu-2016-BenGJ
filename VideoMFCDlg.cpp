
// VideoMFCDlg.cpp : implementation file
//
#include <cstdio>
#include "stdafx.h"
#include "VideoMFC.h"
#include "VideoMFCDlg.h"
#include "afxdialogex.h"
#include "Queue.h"

//#include "Beepmain.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "BEEPSHorizontalVertical.h"
#include "BEEPSVerticalHorizontal.h"
#include <opencv2\opencv.hpp>  

using namespace cv;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CvCapture* capture;
CRect rect;
CDC *pDC;
HDC hDC;
CWnd *pwnd;


CRect rectt;
CDC *pDCt;
HDC hDCt;
CWnd *pwndt;
LinkQueue MYQUEUE; 
LinkQueue MYINITQUEUE;

CvVideoWriter* writer = 0;
void CALLBACK TimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
// CAboutDlg dialog used for App About
static float g_photometricDec=20.0;  //值域系数
static float g_spatialDecay=0.8;  //空域系数 

bool InitQueue(LinkQueue *Q)  
{   
    Q->front=Q->rear = new QNode;  
    if(!Q->front)  
        return false;  
    Q->front->next=NULL;  
    return true;  
}  
  
//销毁队列  
bool DestoryQueue(LinkQueue * Q)  
{  
    while(Q->front)  
    {  
         Q->rear=Q->front->next;  
         delete Q->front;  
         Q->front=Q->rear;  
    }  
    return true;  
}  
  
  
/* 求队列的长度 */  
int QueueLength(LinkQueue * Q)  
{   
    int i=0;  
    QueuePtr p;  
    p= Q->front;  
    while(Q->rear!=p)  
    {  
         i++;  
         p=p->next;  
    }  
    return i;  
}  
  
  
//队列在队尾插入节点，入队操作  
//插入元素data为Q的新的队尾元素，插入成功返回true，否则返回false  
bool EnQueue(LinkQueue *Q, IplImage * data)  
{  
    QueuePtr s = new QNode;  
    if(!s)   //存储空间分配失败  
        return false;  
    s->img = data;  
    s->next = NULL;  
    Q->rear->next = s;  
    Q->rear = s;  
    return true;  
}  
  
  
//队列在队头删除节点，出队操作  
//若队列不为空，删除Q的队头元素，用data返回该图像的地址，并返回true表示操作成功，否则返回false  
bool DeQueue(LinkQueue *Q, IplImage ** data)  
{  
      
    QueuePtr p;  
    if(Q->front == Q->rear)  
    {  
        cout<<"The Queue is NULL!"<<endl;  
        return false;  
    }  
    p = Q->front->next;  
    *data = p->img;  
    Q->front->next = p->next;  
    if(Q->rear == p)  
        Q->rear = Q->front;  
    delete p;  
    return true;  
}  







//转换数据 
void Convert2Double(Mat& src,double* data)
{   
	int index;
	for(int y=0;y<src.rows;y++)
	{   
		uchar* p=src.ptr<uchar>(y);
		index=y*src.cols;
		for(int x=0;x<src.cols;x++)
		{
			data[index++]=(double)(*p++);
		}
	}
}

void Convert2Mat(Mat& dst,double* data)
{
	int index;
	for(int y=0;y<dst.rows;y++)
	{   
		uchar* p=dst.ptr<uchar>(y);
		index=y*dst.cols;
		for(int x=0;x<dst.cols;x++)
		{
			*p++=saturate_cast<uchar>(data[index++]);
		}
	}
}

void BEEPSFilter(IplImage* src)
{
	Mat dst;
	Mat srct(src,true);
	//Mat dstt(dst,true);
	vector<Mat> channels;
	split(srct,channels);   //分离通道   
	int width=srct.cols,height=srct.rows;
    double* value1=new double[width*height];
	double* value2=new double[width*height];
	int i,j;
	for(i=0;i<channels.size();i++)
	{
		Convert2Double(channels.at(i),value1);
		memcpy(value2,value1,width*height*sizeof(double));

		//分别执行  源程序使用多线程并发执行   
		BEEPSHorizontalVertical beephv(value1,width,height,g_photometricDec,g_spatialDecay);
		beephv.Run();
		BEEPSVerticalHorizontal beepvh(value2,width,height,g_photometricDec,g_spatialDecay);
		beepvh.Run();

		for(j=0;j<width*height;j++)
		{
			value1[j]=(value1[j]+value2[j])/2.0;
		}

		Convert2Mat(channels.at(i),value1);
	}
	merge(channels,dst);  //合并通道
	//*dst=IplImage(dstt);
	 //src = cvCreateImage(cvGetSize(&dst),IPL_DEPTH_8U,3);
	//cvGetImage(&dst,src);  
	//imshow("1",dst); 
   // *src=IplImage(dst);
   // waitKey(1);
	//cv::Mat img2;
   //resize(dst, dst, Size(520, 520));
   IplImage imgTmp = dst;
   IplImage* src1 = cvCloneImage(&imgTmp);
   
   EnQueue(&MYQUEUE,src1);
   //cvReleaseImage(&src1); 
   
   /*
   if(QueueLength(&MYQUEUE)>15)    
       {         
          IplImage * data = NULL;        
          DeQueue(&MYQUEUE, &data);         
          cvReleaseImage(&data);     
       }    */
	/*
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(src1,1);	
	if (true)
	{
		m_CvvImage.DrawToHDC(hDCt, &rectt);
		//KillTimer(2);
		//cvWriteFrame(writer,src);
		//cvWaitKey(10);
	}*/
	//SetTimer(2,15,(TIMERPROC)TimerProc);
	//CDialogEx::OnTimer(nIDEvent);

	delete[] value1;
	delete[] value2;
}


DWORD WINAPI ThreadProc(LPVOID pParam)
{  
	//Mat src=imread("Skin3.jpg");
	//resize(src,src,Size(520, 520));
	IplImage* src=(IplImage* )pParam;
	//HWND hwnd=(HWND)pParam;
	//double _time=static_cast<double>(getTickCount());
	
	while(1){
		//SetTimer(hwnd,10,NULL);
		
		//AfxMessageBox("这是一个最简单的消息框！");
	   BEEPSFilter(src);
	   Sleep(1);
	}

	//SetTimer(hwnd,2,15,(TIMERPROC)TimerProc);
	//}
	//_time=1000*(static_cast<double>(getTickCount())-_time)/getTickFrequency();  
	//printf("The total time cost: %f\n",_time);

	//imshow("处理前",src);
	//imshow("处理后",dst);
	//waitKey();

	//destroyAllWindows();
	//src.release();
	//dst.release();
	return 0;
}



void CALLBACK TimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	//IplImage* m_Frame;
	//Mat dst;
	//dst=NULL;
	IplImage* m_Frame;
	CvvImage m_CvvImage;
	//m_Frame=cvQueryFrame(capture);
	
              
	//for(;;){
	//if(QueueLength(&MYQUEUE)>1){
	//		break;
	switch(1){
	case 1:
		if(QueueLength(&MYQUEUE)<1)
			break;
	    
		DeQueue(&MYQUEUE, &m_Frame);
	//cvShowImage("IPCamera",m_Frame);  
      //  cvWaitKey(1);  
	//IplImage* m_Frame=(IplImage* )hwnd;
	//m_Frame=cvQueryFrame(capture);
	//BEEPSFilter(src);
	//AfxMessageBox("这是一个最简单的消息框！");	
	
	
	
	m_CvvImage.CopyOf(m_Frame,1);	
	if (true)
	{
		m_CvvImage.DrawToHDC(hDCt, &rectt);
		//cvWriteFrame(writer,m_Frame);
		//cvWaitKey(10);
	}

	break;
	}
	//}
	//AfxBeginThread((AFX_THREADPROC)ThreadProc,m_Frame,0);
	//ttt_main(m_Frame,dst);
	//}
}







class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVideoMFCDlg dialog




CVideoMFCDlg::CVideoMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideoMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVideoMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CVideoMFCDlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, &CVideoMFCDlg::OnBnClickedButton2)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON3, &CVideoMFCDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CVideoMFCDlg message handlers

BOOL CVideoMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	pwnd = GetDlgItem(IDC_ShowImage);
	pwndt = GetDlgItem(IDC_MYIMAGE);
	//pwnd->MoveWindow(35,30,352,288);
    pDC =pwnd->GetDC();
	pDCt =pwndt->GetDC();
	//pDC =GetDC();
    hDC= pDC->GetSafeHdc();
	hDCt= pDCt->GetSafeHdc();

	pwnd->GetClientRect(&rect);
	pwndt->GetClientRect(&rectt);



	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVideoMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVideoMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVideoMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVideoMFCDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	//AfxMessageBox("OK");
	if(!capture)
	{
		capture = cvCaptureFromCAM(0);
		//AfxMessageBox("OK");
	}

	if (!capture)
	{
		AfxMessageBox("无法打开摄像头");
		return;
	}

	InitQueue(&MYINITQUEUE);
	EnQueue(&MYINITQUEUE,cvQueryFrame(capture));
	//writer=cvCreateVideoWriter("MyVideo.avi",CV_FOURCC('x','v','I','D'),25,cvSize(640,480));
	// 测试
	/*
	IplImage* m_Frame;
	m_Frame=cvQueryFrame(capture);
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame,1);	
	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
		//cvWaitKey(10);
	}
	*/
	// 设置计时器,每10ms触发一次事件
	SetTimer(1,100,NULL);
	
}


//void CVideoMFCDlg::OnTimer(UINT nIDEvent)
//{
//	
//}


void CVideoMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/************************************************************************/
	/* 显示摄像头                                                           */
	/************************************************************************/
	IplImage* m_Frame;
	//Mat dst;
	//dst=NULL;
	m_Frame=cvQueryFrame(capture);
	CvvImage m_CvvImage;
	switch(nIDEvent){
	case 1:
	
	m_CvvImage.CopyOf(m_Frame,1);	
	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
		//cvWriteFrame(writer,m_Frame);
		//cvWaitKey(10);
	}

	CDialogEx::OnTimer(nIDEvent);
	break;

	}
/*
	case 3:
	
	   EnQueue(&MYINITQUEUE,m_Frame);
	   break;
	}*/
	//imshow("处理后",dst);
	//waitKey(1);
	//*m_Frame = IplImage(dst);
	/*
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame,1);	
	if (true)
	{
		m_CvvImage.DrawToHDC(hDC, &rect);
		cvWriteFrame(writer,m_Frame);
		//cvWaitKey(10);
	}

	CDialogEx::OnTimer(nIDEvent);*/
}



void CVideoMFCDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	DestoryQueue(&MYINITQUEUE);
	DestoryQueue(&MYQUEUE);
	cvReleaseVideoWriter(&writer);
	cvReleaseCapture(&capture);
	KillTimer(1);
	KillTimer(2);
	//KillTimer(3);
	CDC MemDC,MemDCt; 
	CBitmap m_Bitmap1,m_Bitmap1t;
	m_Bitmap1.LoadBitmap(IDB_BITMAP1); 
	m_Bitmap1t.LoadBitmap(IDB_BITMAP1);

	MemDC.CreateCompatibleDC(NULL);
	MemDC.SelectObject(&m_Bitmap1);
	MemDCt.CreateCompatibleDC(NULL);
	MemDCt.SelectObject(&m_Bitmap1t);

	pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&MemDC,0,0,48,48,SRCCOPY); 
	pDCt->StretchBlt(rectt.left,rectt.top,rectt.Width(),rectt.Height(),&MemDCt,0,0,48,48,SRCCOPY);
}


void CVideoMFCDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	cvReleaseCapture(&capture);
	CDialogEx::OnClose();
}


void CVideoMFCDlg::OnBnClickedButton3()
{
	IplImage* m_Frame=NULL;
	InitQueue(&MYQUEUE);
	
   if(QueueLength(&MYINITQUEUE)>=1)
	   DeQueue(&MYINITQUEUE, &m_Frame);
	
	
	/*
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame,1);	
	if (true)
	{
		m_CvvImage.DrawToHDC(hDCt, &rectt);
		//cvWaitKey(10);
	}*/
	
	AfxBeginThread((AFX_THREADPROC)ThreadProc,m_Frame,THREAD_PRIORITY_IDLE);
	
	// 设置计时器,每10ms触发一次事件
	SetTimer(2,1,(TIMERPROC)TimerProc);
	// TODO: Add your control notification handler code here
}
