/* ******************************************************************** **
** @@ Shingles Analyzer
** @  Copyrt : (c) gazlan@yandex.ru
** @  Author :
** @  Modify :
** @  Update : 15 Sep 2010
** @  Notes  :
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include <afxrich.h>

#include "Shingles.h"
#include "vector.h"
#include "vector_sorted.h"
#include "map_tst.h"
#include "mmf.h"
#include "ShinglesDlg.h"
#include "hash_hsieh.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  MAX_TAG_SIZE               (8)         // ����. ������ ���������� ����
#define  HTML_EN_STOP_WORDS_CNT     (36 + 22)   // EN set + Non ASCII set
#define  HTML_RU_STOP_WORDS_CNT     (64)
#define  COMMON_WORDS_CNT           (29)
#define  STOP_SIGNS_CNT             (37)

#define  DEFAULT_LIST_SIZE          (2048)
#define  DEFAULT_LIST_DELTA         (256)

// ����� ����-��������, ������� ����� _�������_ ��� ������������ ������ (NB: �������� � �������� ����!)
const char STOP_SIGNS_SET[STOP_SIGNS_CNT] = {'\a','\b','\f','\n','\v','\r','!','\"','#','$','%','&','\'','(',')', '*','+',',','-','.','/',':',';','<','=','>','?','[','\\',']','^','`','{','|','}','~'};
                                    
// ����������� ����� ����-��������, ������� ����� _��������_ ��� ������������ ������
const char STOP_SIGNS_TABLE[256] =
{
   '\x00', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', 
   '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', 
   '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', 
   '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x20', '\x20', '\x20', '\x20', '\x20', '\x20', 
   '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4A', '\x4B', '\x4C', '\x4D', '\x4E', '\x4F', 
   '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5A', '\x20', '\x20', '\x20', '\x20', '\x20', 
   '\x20', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F', 
   '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x20', '\x20', '\x20', '\x20', '\x20', 
   '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F', 
   '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F', 
   '\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF', 
   '\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF', 
   '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF', 
   '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF', 
   '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF', 
   '\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF' 
};

// ����������� HTML-����. EN set 
// DON'T SORT !!
// �� ����������� ! 
// ������� � TST ������ ���� ��������������� ��� ���������� ������������������ ������ ������
const char* HTML_EN_STOP_WORDS[HTML_EN_STOP_WORDS_CNT] =
{
   "&quot;",
   "&amp;",
   "&lt;",
   "&gt;",
   "&nbsp;",
   "&iexcl;",
   "&cent;",
   "&pound;",
   "&curren;",
   "&yen;",
   "&brvbar;",
   "&sect;",
   "&uml;",
   "&copy;",
   "&ordf;",
   "&laquo;",
   "&not;",
   "&shy;",
   "&reg;",
   "&macr;",
   "&deg;",
   "&plusmn;",
   "&sup2;",
   "&sup3;",
   "&acute;",
   "&micro;",
   "&para;",
   "&middot;",
   "&cedil;",
   "&sup1;",
   "&ordm;",
   "&raquo;",
   "&frac14;",
   "&frac12;",
   "&frac34;",
   "&iquest;",
   // ��� ���� �� ������ � ������� ASCII, �� �������� ���� ��� ��������� ���������
   "&bull;",
   "&trade;",
   "&euro;",
   "&bdquo;",
   "&dagger;",
   "&Dagger;", 
   "&hellip;",
   "&ldquo;",
   "&lsaquo;",
   "&lsquo;",
   "&mdash;",
   "&ndash;",
   "&permil;",
   "&rsaquo;",
   "&rsquo;",
   "&sbquo;",
   "&rdquo;",
   "&tilde;",
   "&spades;",
   "&clubs;",
   "&hearts;",
   "&diams;"
};

// ����������� HTML-����. RU set (������������� ������� �������� � Win-1251)
// DON'T SORT !!
// �� ����������� ! 
// ������� � TST ������ ���� ��������������� ��� ���������� ������������������ ������ ������
const char* HTML_RU_STOP_WORDS[HTML_RU_STOP_WORDS_CNT] =
{
   "&Agrave;",
   "&Aacute;",
   "&Acirc;",
   "&Atilde;",
   "&Auml;",
   "&Aring;",
   "&AElig;",
   "&Ccedil;",
   "&Egrave;",
   "&Eacute;",
   "&Ecirc;",
   "&Euml;",
   "&Igrave;",
   "&Iacute;",
   "&Icirc;",
   "&Iuml;",
   "&ETH;",
   "&Ntilde;",
   "&Ograve;",
   "&Oacute;",
   "&Ocirc;",
   "&Otilde;",
   "&Ouml;",
   "&times;",
   "&Oslash;",
   "&Ugrave;",
   "&Uacute;",
   "&Ucirc;",
   "&Uuml;",
   "&Yacute;",
   "&THORN;",
   "&szlig;",
   "&agrave;",
   "&aacute;",
   "&acirc;",
   "&atilde;",
   "&auml;",
   "&aring;",
   "&aelig;",
   "&ccedil;",
   "&egrave;",
   "&eacute;",
   "&ecirc;",
   "&euml;",
   "&igrave;",
   "&iacute;",
   "&icirc;",
   "&iuml;",
   "&eth;",
   "&ntilde;",
   "&ograve;",
   "&oacute;",
   "&ocirc;",
   "&otilde;",
   "&ouml;",
   "&divide;",
   "&oslash;",
   "&ugrave;",
   "&uacute;",
   "&ucirc;",
   "&uuml;",
   "&yacute;",
   "&thorn;",
   "&yuml;"
};

// Common Words
// ��� ����� ����� ��������� �� ������
// DON'T SORT !!
// �� ����������� ! 
// ������� � TST ������ ���� ��������������� ��� ���������� ������������������ ������ ������
const char* COMMON_WORDS[COMMON_WORDS_CNT] =
{
   "���",
   "���",
   "���",
   "�",
   "�",
   "���",
   "�",
   "��",
   "��",
   "��",
   "��",
   "��",
   "��",
   "�",
   "��",
   "�",
   "��",
   "��",
   "��",
   "���",
   "�",
   "��",
   "��",
   "��",
   "��",
   "���",
   "���",
   "��",
   "���"
};

/* ******************************************************************** **
** @@ CMP_Hash()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  : Hash compare function
** ******************************************************************** */

inline int CMP_Hash(const void** const pKey1,const void** const pKey2)
{
   DWORD*  p1 = (DWORD*)pKey1;
   DWORD*  p2 = (DWORD*)pKey2;

   if ((*p1) > (*p2))
   {
      return 1;
   }
   else if ((*p1) < (*p2))
   {
      return -1;
   }
   else
   {
      return 0;
   }
}

/* ******************************************************************** **
** @@ CShinglesDlg::CShinglesDlg()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

CShinglesDlg::CShinglesDlg(CWnd* pParent /*=NULL*/)
:  CDialog(CShinglesDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CShinglesDlg)
   m_EdtLeft = _T("");
   m_EdtRight = _T("");
   m_PathLeft = _T("");
   m_PathRight = _T("");
   //}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   _bInitialized      = false;
   _bNormalizedLeft   = false;
   _bNormalizedRight  = false;
   _bNormalizeHiAscii = false;
   _bLoadedLeft       = false;
   _bLoadedRight      = false;
   _bUtf8             = false;

   _iGranularity = 0;
   _iThreshold   = 0;

   memset(_pszFileLeft, 0,sizeof(_pszFileLeft));
   memset(_pszFileRight,0,sizeof(_pszFileRight));

   memset(_pszFileLeftNrm, 0,sizeof(_pszFileLeft));
   memset(_pszFileRightNrm,0,sizeof(_pszFileRight));
}

/* ******************************************************************** **
** @@ CShinglesDlg::DoDataExchange()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CShinglesDlg)
   DDX_Control(pDX, IDC_SPN_THRESHOLD, m_Threshold);
   DDX_Control(pDX, IDC_SPN_GRANULARITY, m_Granularity);
   DDX_Control(pDX, IDC_CHK_UTF8, m_UTF);
   DDX_Control(pDX, IDC_CHK_HI_ASCII, m_HiAscii);
   DDX_Control(pDX, IDC_CHK_RIGHT, m_ChkRight);
   DDX_Control(pDX, IDC_CHK_LEFT, m_ChkLeft);
   DDX_Text(pDX, IDC_RICHEDT_LEFT, m_EdtLeft);
   DDX_Text(pDX, IDC_RICHEDT_RIGHT, m_EdtRight);
   DDX_Text(pDX, IDC_EDT_PATH_LEFT, m_PathLeft);
   DDX_Text(pDX, IDC_EDT_PATH_RIGHT, m_PathRight);
   //}}AFX_DATA_MAP
}

/* ******************************************************************** **
** @@ CShinglesDlg::MESSAGE_MAP()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CShinglesDlg, CDialog)
   //{{AFX_MSG_MAP(CShinglesDlg)
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_BTN_CLR_LEFT,OnBtnClrLeft)
   ON_BN_CLICKED(IDC_BTN_CLR_RIGHT,OnBtnClrRight)
   ON_BN_CLICKED(IDC_BTN_LOAD_LEFT,OnBtnLoadLeft)
   ON_BN_CLICKED(IDC_BTN_LOAD_RIGHT,OnBtnLoadRight)
   ON_BN_CLICKED(IDC_BTN_COMPARE,OnBtnCompare)
   ON_BN_CLICKED(IDC_CHK_LEFT,OnChkLeft)
   ON_BN_CLICKED(IDC_CHK_RIGHT,OnChkRight)
   ON_BN_CLICKED(IDC_BTN_NRM_LEFT, OnBtnNrmLeft)
   ON_BN_CLICKED(IDC_BTN_NRM_RIGHT, OnBtnNrmRight)
   ON_BN_CLICKED(IDC_CHK_UTF8, OnChkUtf8)
   ON_BN_CLICKED(IDC_CHK_HI_ASCII, OnChkHiAscii)
   ON_EN_CHANGE(IDC_EDT_GRANULARITY, OnChangeEdtGranularity)
   ON_EN_CHANGE(IDC_EDT_THRESHOLD, OnChangeEdtThreshold)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CShinglesDlg::OnInitDialog()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BOOL CShinglesDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon,TRUE);  // Set big icon
   SetIcon(m_hIcon,FALSE); // Set small icon
                           
   _bInitialized = true;

   _bNormalizeHiAscii = false;
   _bUtf8             = true;

   // ���������� �������
   m_Threshold.SetBase(10);
   m_Granularity.SetBase(10);

   // ���������� ��������
   m_Threshold.SetRange(1,10);
   m_Granularity.SetRange(1,20);

   // ���������� ��������� ��������
   m_Threshold.SetPos(4);
   m_Granularity.SetPos(10);

   m_HiAscii.SetCheck(BST_UNCHECKED);
   m_UTF.SetCheck(BST_CHECKED);

   GetDlgItem(IDC_CHK_LEFT)->    EnableWindow(FALSE);
   GetDlgItem(IDC_BTN_NRM_LEFT)->EnableWindow(FALSE);
   GetDlgItem(IDC_BTN_CLR_LEFT)->EnableWindow(FALSE);

   GetDlgItem(IDC_CHK_RIGHT)->    EnableWindow(FALSE);
   GetDlgItem(IDC_BTN_NRM_RIGHT)->EnableWindow(FALSE);
   GetDlgItem(IDC_BTN_CLR_RIGHT)->EnableWindow(FALSE);

   for (int ii = 0; ii < HTML_EN_STOP_WORDS_CNT; ++ii)  
   {
      _DixHtmlEn._pData = (char*)HTML_EN_STOP_WORDS[ii];

      if (!_DixHtmlEn.Insert((char*)HTML_EN_STOP_WORDS[ii]))
      {
         AfxMessageBox("OnInitDialog: ������ ������������� ������� HTML_EN_STOP_WORDS",MB_ICONSTOP);
         return FALSE;
      }
   } 

   #if _DEBUG  
   for (int kk = 0; kk < HTML_EN_STOP_WORDS_CNT; ++kk)  
   {
      if (!_DixHtmlEn.Search((char*)HTML_EN_STOP_WORDS[kk]))
      {
         AfxMessageBox("OnInitDialog: ������ ������� �� ������� HTML_EN_STOP_WORDS",MB_ICONSTOP);
         return FALSE;
      }
   } 
   #endif

   for (int jj = 0; jj < HTML_RU_STOP_WORDS_CNT; ++jj)  
   {
      _DixHtmlRu._pData = (char*)HTML_RU_STOP_WORDS[jj];

      if (!_DixHtmlRu.Insert((char*)HTML_RU_STOP_WORDS[jj]))
      {
         AfxMessageBox("OnInitDialog: ������ ������������� ������� HTML_RU_STOP_WORDS",MB_ICONSTOP);
         return FALSE;
      }
   } 

   #if _DEBUG  
   for (int ll = 0; ll < HTML_RU_STOP_WORDS_CNT; ++ll)  
   {
      if (!_DixHtmlRu.Search((char*)HTML_RU_STOP_WORDS[ll]))
      {
         AfxMessageBox("OnInitDialog: ������ ������� �� ������� HTML_RU_STOP_WORDS",MB_ICONSTOP);
         return FALSE;
      }
   } 
   #endif

   for (int mm = 0; mm < COMMON_WORDS_CNT; ++mm)  
   {
      _DixCommonWords._pData = (char*)COMMON_WORDS[mm];

      if (!_DixCommonWords.Insert((char*)COMMON_WORDS[mm]))
      {
         AfxMessageBox("OnInitDialog: ������ ������������� ������� COMMON_WORDS",MB_ICONSTOP);
         return FALSE;
      }
   } 

   #if _DEBUG  
   for (int nn = 0; nn < COMMON_WORDS_CNT; ++nn)  
   {
      if (!_DixCommonWords.Search((char*)COMMON_WORDS[nn]))
      {
         AfxMessageBox("OnInitDialog: ������ ������� �� ������� COMMON_WORDS",MB_ICONSTOP);
         return FALSE;
      }
   } 
   #endif

   _iGranularity = m_Granularity.GetPos();

   _ShingleListLeft.Resize(DEFAULT_LIST_SIZE);
   _ShingleListLeft.Delta(DEFAULT_LIST_DELTA);
   _ShingleListLeft.SetSorter(CMP_Hash);

   _ShingleListRight.Resize(DEFAULT_LIST_SIZE);
   _ShingleListRight.Delta(DEFAULT_LIST_DELTA);
   _ShingleListRight.SetSorter(CMP_Hash);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnPaint()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void CShinglesDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND,(WPARAM)dc.GetSafeHdc(),0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);

      CRect    rect;
      
      GetClientRect(&rect);
      
      int x = (rect.Width()  - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnQueryDragIcon()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CShinglesDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnChkUtf8()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnChkUtf8() 
{
   _bUtf8 = m_UTF.GetCheck() == BST_CHECKED  ?  true  :  false;
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnChkHiAscii()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnChkHiAscii() 
{
   _bNormalizeHiAscii = m_HiAscii.GetCheck() == BST_CHECKED  ?  true  :  false;
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnCancel()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnCancel() 
{
   Cleanup();

   CDialog::OnCancel();
}

/* ******************************************************************** **
** @@ CShinglesDlg::Cleanup()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::Cleanup()
{
   int   iCnt = 0;

   iCnt = _ShingleListLeft.Count();

   for (int ii = iCnt - 1; ii >= 0; --ii)
   {
      _ShingleListLeft.RemoveAt(ii);
   }

   iCnt = _ShingleListRight.Count();

   for (int jj = iCnt - 1; jj >= 0; --jj)
   {
      _ShingleListRight.RemoveAt(jj);
   }
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnBtnNrmLeft()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnBtnNrmLeft() 
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   m_ChkLeft.SetCheck(BST_CHECKED);
   GetDlgItem(IDC_BTN_NRM_LEFT)->EnableWindow(FALSE);
   
   char     pszNrmName[_MAX_PATH];
   char     pszDrive  [_MAX_DRIVE];
   char     pszDir    [_MAX_DIR];
   char     pszFName  [_MAX_FNAME];
   char     pszExt    [_MAX_EXT];

   _splitpath(_pszFileLeft,pszDrive,pszDir,pszFName,pszExt);
   _makepath (pszNrmName,  pszDrive,pszDir,pszFName,"nrml");

   memset(_pszFileLeftNrm,0,sizeof(_pszFileLeftNrm));    // Clear

   if (!CopyFile(_pszFileLeft,pszNrmName,FALSE))
   {
      AfxMessageBox("OnBtnNrmLeft: ������ ����������� �����.",MB_ICONSTOP);
      return;
   }

   strncpy(_pszFileLeftNrm,pszNrmName,MAX_PATH);
   _pszFileLeftNrm[MAX_PATH] = 0;   // Ensure ASCIIZ

   MMF      Victim;

   if (!Victim.OpenReadWrite(_pszFileLeftNrm))
   {
      AfxMessageBox("OnBtnNrmLeft: ������ �������� �����.",MB_ICONSTOP);
      return;
   }

   BYTE*    pBuf   = Victim.Buffer();
   DWORD    dwSize = Victim.Size();
   
   if (!pBuf || !dwSize)
   {
      AfxMessageBox("OnBtnNrmLeft: ������: ������ ����.",MB_ICONSTOP);
      return;
   }

   dwSize = Normalize(pBuf,dwSize);

   Victim.Close();

   // Resize
   if (dwSize)
   {
      HANDLE   hFile = CreateFile(_pszFileLeftNrm,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

      if (hFile == INVALID_HANDLE_VALUE)
      {
         AfxMessageBox("OnBtnNrmLeft: ������ �������� ���������������� �����.",MB_ICONSTOP);
         return;
      }

      SetFilePointer(hFile,dwSize,NULL,FILE_BEGIN);
      SetEndOfFile(hFile); 
      CloseHandle(hFile);
   }

   _bNormalizedLeft = true;
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnBtnNrmRight()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnBtnNrmRight() 
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   m_ChkRight.SetCheck(BST_CHECKED);
   GetDlgItem(IDC_BTN_NRM_RIGHT)->EnableWindow(FALSE);
   
   char     pszNrmName[_MAX_PATH];
   char     pszDrive  [_MAX_DRIVE];
   char     pszDir    [_MAX_DIR];
   char     pszFName  [_MAX_FNAME];
   char     pszExt    [_MAX_EXT];

   _splitpath(_pszFileRight,pszDrive,pszDir,pszFName,pszExt);
   _makepath (pszNrmName,  pszDrive,pszDir,pszFName,"nrml");

   memset(_pszFileRightNrm,0,sizeof(_pszFileRightNrm));    // Clear

   if (!CopyFile(_pszFileRight,pszNrmName,FALSE))
   {
      AfxMessageBox("OnBtnNrmRight: ������ ����������� �����.",MB_ICONSTOP);
      return;
   }

   strncpy(_pszFileRightNrm,pszNrmName,MAX_PATH);
   _pszFileRightNrm[MAX_PATH] = 0;   // Ensure ASCIIZ

   MMF      Victim;

   if (!Victim.OpenReadWrite(_pszFileRightNrm))
   {
      AfxMessageBox("OnBtnNrmRight: ������ �������� �����.",MB_ICONSTOP);
      return;
   }

   BYTE*    pBuf   = Victim.Buffer();
   DWORD    dwSize = Victim.Size();
   
   if (!pBuf || !dwSize)
   {
      AfxMessageBox("OnBtnNrmRight: ������: ������ ����.",MB_ICONSTOP);
      return;
   }

   dwSize = Normalize(pBuf,dwSize);

   Victim.Close();

   // Resize
   if (dwSize)
   {
      HANDLE   hFile = CreateFile(_pszFileRightNrm,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

      if (hFile == INVALID_HANDLE_VALUE)
      {
         AfxMessageBox("OnBtnNrmRight: ������ �������� ���������������� �����.",MB_ICONSTOP);
         return;
      }

      SetFilePointer(hFile,dwSize,NULL,FILE_BEGIN);
      SetEndOfFile(hFile); 
      CloseHandle(hFile);
   }

   _bNormalizedRight = true;
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnBtnClrLeft()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnBtnClrLeft() 
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   m_EdtLeft.Empty();
   
   memset(_pszFileLeft,   0,sizeof(_pszFileLeft));
   memset(_pszFileLeftNrm,0,sizeof(_pszFileLeftNrm));
   
   SetDlgItemText(IDC_EDT_PATH_LEFT,"");
   SetDlgItemText(IDC_RICHEDT_LEFT,"");
   
   m_ChkLeft.SetCheck(BST_UNCHECKED);
   
   GetDlgItem(IDC_CHK_LEFT)->    EnableWindow(FALSE);
   GetDlgItem(IDC_BTN_NRM_LEFT)->EnableWindow(FALSE);
   GetDlgItem(IDC_BTN_CLR_LEFT)->EnableWindow(FALSE);
   
   _bLoadedLeft = false;
   _bNormalizedLeft = false;
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnBtnClrRight()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnBtnClrRight() 
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   m_EdtRight.Empty();
   
   memset(_pszFileRight,   0,sizeof(_pszFileRight));
   memset(_pszFileRightNrm,0,sizeof(_pszFileRightNrm));
   
   SetDlgItemText(IDC_EDT_PATH_RIGHT,"");
   SetDlgItemText(IDC_RICHEDT_RIGHT, "");
   
   m_ChkRight.SetCheck(BST_UNCHECKED);

   GetDlgItem(IDC_CHK_RIGHT)->    EnableWindow(FALSE);
   GetDlgItem(IDC_BTN_NRM_RIGHT)->EnableWindow(FALSE);
   GetDlgItem(IDC_BTN_CLR_RIGHT)->EnableWindow(FALSE);

   _bLoadedRight = false;
   _bNormalizedRight = false;
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnBtnLoadLeft()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnBtnLoadLeft() 
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   DWORD    dwFlags =   OFN_ENABLESIZING     |
                        OFN_FILEMUSTEXIST    |
                        OFN_PATHMUSTEXIST;
                                    
   char     pszFilter[MAX_PATH] = "HTM* (*.htm*)|*.htm*|"
                                  "PHP* (*.php*)|*.php*|"
                                  "ASP* (*.asp)|*.asp|"
                                  "NRML (*.nrml)|*.nrml|"
                                  "TXT* (*.txt)|*.txt|"
                                  "ALL (*.*)|*.*||";

   CFileDialog    FileLeft(TRUE,NULL,NULL,dwFlags,pszFilter);

   if (FileLeft.DoModal() == IDOK)
   {
      strcpy(_pszFileLeft,(LPCTSTR)FileLeft.GetPathName());
      SetDlgItemText(IDC_EDT_PATH_LEFT,_pszFileLeft);

      if (_Left.IsOpen())
      {
         _Left.Close();
      }

      if (!_Left.OpenReadOnly(_pszFileLeft))
      {
         AfxMessageBox("OnBtnLoadLeft: ������ �������� �����.",MB_ICONSTOP);
      }

      m_EdtLeft = _Left.Buffer();

      SetDlgItemText(IDC_RICHEDT_LEFT,(LPCTSTR)m_EdtLeft);

      _Left.Close();
      _bLoadedLeft     = true;
      _bNormalizedLeft = false;

      GetDlgItem(IDC_CHK_LEFT)->    EnableWindow(TRUE);
      GetDlgItem(IDC_BTN_NRM_LEFT)->EnableWindow(TRUE);
      GetDlgItem(IDC_BTN_CLR_LEFT)->EnableWindow(TRUE);
   }

   m_ChkLeft.SetCheck(BST_UNCHECKED);
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnBtnLoadRight()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnBtnLoadRight() 
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   DWORD    dwFlags =   OFN_ENABLESIZING     |
                        OFN_FILEMUSTEXIST    |
                        OFN_PATHMUSTEXIST;
                                    
   char     pszFilter[MAX_PATH] = "HTM* (*.htm*)|*.htm*|"
                                  "PHP* (*.php*)|*.php*|"
                                  "ASP* (*.asp)|*.asp|"
                                  "NRML (*.nrml)|*.nrml|"
                                  "TXT* (*.txt)|*.txt|"
                                  "ALL (*.*)|*.*||";

   CFileDialog    FileRight(TRUE,NULL,NULL,dwFlags,pszFilter);

   if (FileRight.DoModal() == IDOK)
   {
      strcpy(_pszFileRight,(LPCTSTR)FileRight.GetPathName());
      SetDlgItemText(IDC_EDT_PATH_RIGHT,_pszFileRight);

      if (_Right.IsOpen())
      {
         _Right.Close();
      }

      if (!_Right.OpenReadOnly(_pszFileRight))
      {
         AfxMessageBox("OnBtnLoadRight: ������ �������� �����.",MB_ICONSTOP);
      }

      m_EdtRight = _Right.Buffer();

      SetDlgItemText(IDC_RICHEDT_RIGHT,(LPCTSTR)m_EdtRight);

      _Right.Close();
      _bLoadedRight     = true;
      _bNormalizedRight = false;

      GetDlgItem(IDC_CHK_RIGHT)->    EnableWindow(TRUE);
      GetDlgItem(IDC_BTN_NRM_RIGHT)->EnableWindow(TRUE);
      GetDlgItem(IDC_BTN_CLR_RIGHT)->EnableWindow(TRUE);
   }

   m_ChkRight.SetCheck(BST_UNCHECKED);
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnBtnCompare()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnBtnCompare() 
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   if (!_bLoadedLeft && !_bLoadedRight)
   {
      AfxMessageBox("OnBtnCompare: ����� �� ���������.",MB_ICONSTOP);
      return;
   }

   if (!_bLoadedLeft)
   {
      AfxMessageBox("OnBtnCompare: �� �������� ���� �� ����� ������.",MB_ICONSTOP);
      return;
   }

   if (!_bLoadedRight)
   {
      AfxMessageBox("OnBtnCompare: �� �������� ���� �� ������ ������.",MB_ICONSTOP);
      return;
   }

   if (!_bNormalizedLeft)
   {
      OnBtnNrmLeft();
   }

   if (!_bNormalizedRight)
   {
      OnBtnNrmRight();
   }

   Compare();
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnChangeEdtGranularity()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnChangeEdtGranularity() 
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   if (!_bInitialized)
   {
      return;
   }

   m_ChkLeft.SetCheck(BST_UNCHECKED);
   m_ChkRight.SetCheck(BST_UNCHECKED);

   OnChkLeft();
   OnChkRight();
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnChangeEdtThreshold()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnChangeEdtThreshold() 
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   if (!_bInitialized)
   {
      return;
   }

   m_ChkLeft.SetCheck(BST_UNCHECKED);
   m_ChkRight.SetCheck(BST_UNCHECKED);

   OnChkLeft();
   OnChkRight();
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnChkLeft()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnChkLeft() 
{
   if (m_ChkLeft.GetCheck() == BST_CHECKED)
   {
      _bNormalizedLeft = true;
      GetDlgItem(IDC_BTN_NRM_LEFT)->EnableWindow(FALSE);
   }
   else
   {
      _bNormalizedLeft = false;
      GetDlgItem(IDC_BTN_NRM_LEFT)->EnableWindow(TRUE);
   }
}

/* ******************************************************************** **
** @@ CShinglesDlg::OnChkRight()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::OnChkRight() 
{
   if (m_ChkRight.GetCheck() == BST_CHECKED)
   {
      _bNormalizedRight = true;
      GetDlgItem(IDC_BTN_NRM_RIGHT)->EnableWindow(FALSE);
   }
   else
   {
      _bNormalizedRight = false;
      GetDlgItem(IDC_BTN_NRM_RIGHT)->EnableWindow(TRUE);
   }
}

/* ******************************************************************** **
** @@ CShinglesDlg::Normalize()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

DWORD CShinglesDlg::Normalize(BYTE* pBuf,DWORD dwSize)
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   // 1. ������� ��� HTML ����
   //    ��� ������ �������� ������� ��������� ������ �����
   //    � �������� ����������� ���������

   // ���������� ASCIIZ ������
   RemoveHTMLtags(pBuf,dwSize);
   
   DWORD    dwNewSize = strlen((char*)pBuf);

   // ��� ��������� ����������� ���������, ����������
   // ��� ������������� ���������� ������� 

   char*    pTempBuf = new char[dwNewSize + 1];    // ASCIIZ

   if (!pTempBuf)
   {
      AfxMessageBox("Normalize: ������ ��������� ������ ��� ��������� �����.",MB_ICONSTOP);
      return 0;
   }

   // ���������� ASCIIZ ������
   Unspace((char*)pBuf,pTempBuf);

   dwNewSize = strlen(pTempBuf);

   memcpy(pBuf,pTempBuf,dwNewSize);

   delete[] pTempBuf;
   pTempBuf = NULL; 

   // �������� ������� ������
   memset(pBuf + dwNewSize,0,dwSize - dwNewSize);

   // ������������ UTF-8 � ANSI
   // ���� ����������� ��� ���� (���� ���� � UTF):
   // 1. ���������� ������� � ��������� �������� ���������
   // 2. ��������� ���������� ���������, �.�. Windows �� �����
   //    ���������� ��������� UTF � ��� ����������� �� �������������
   //    ����������� ����������, ��� ���������� ������� � ������ ������
   //    (�������� ������ ��������� �� ��������� �����)

   if (_bUtf8)
   {
      if (!UTF8_2_ANSI((char*)pBuf,dwNewSize))
      {
         AfxMessageBox("Normalize: ������ �������������� UTF-8 -> ANSI.",MB_ICONSTOP);
         return 0;
      }
   }

   // ������� HTML ����.���� (������ ������ �������� ASCII)
   RemoveHtmlSpecialEn((char*)pBuf);

   if (_bNormalizeHiAscii)
   {
      // ������� HTML ����.���� (������ ������� �������� ASCII)
      RemoveHtmlSpecialRu((char*)pBuf);
   }

   // ������ ���������� �������� ����-��������, �� ��� �������� ���������
   // �����, ��� ��� �������� � �������� ����. ������ ��������� (� �����
   // ����� ������ �� ��������������) ����� ������ �� �� ������.
   // ��� ���, ��������� ����� ��������� ������������������

   // ������������ ����� � ������ ������� � ������� ����-�������
   // Lower_n_Remove_StopSigns((char*)pBuf);

   // ������������ ����� � ������ ������� � �������� ����-�������
   // ��������, ������ �� ��������. ���������� �� ��������������.
   Lower_n_Replace_StopSigns((char*)pBuf);

   // Finishing. �������� �������������� ��� ��������� ��������.
   pTempBuf = new char[dwNewSize + 1];

   if (!pTempBuf)
   {
      AfxMessageBox("Normalize: ������ ��������� ������ ��� ��������� �����.",MB_ICONSTOP);
      return 0;
   }

   // ���������� ASCIIZ ������
   Unspace((char*)pBuf,pTempBuf);

   dwNewSize = strlen(pTempBuf);

   memcpy(pBuf,pTempBuf,dwNewSize);

   delete[] pTempBuf;
   pTempBuf = NULL; 

   // �������� ������� ������
   memset(pBuf + dwNewSize,0,dwSize - dwNewSize);

   // ���������� ASCIIZ ������
   RemoveCommonWords((char*)pBuf);

   // ������� ������ ���������������� ����� 
   return strlen((char*)pBuf);
}

/* ******************************************************************** **
** @@ CShinglesDlg::Compare()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CShinglesDlg::Compare()
{
   SetDlgItemText(IDC_EDT_RESULT,"");

   Cleanup();

   _iGranularity = m_Granularity.GetPos();

   CString     sTextLeft;
   CString     sTextRight;

   MMF      MF;

   if (!MF.OpenReadOnly(_pszFileLeftNrm))
   {
      AfxMessageBox("Compare: ������ �������� ���������������� �����.",MB_ICONSTOP);
      return;
   }

   sTextLeft = MF.Buffer();

   MF.Close();

   if (!MF.OpenReadOnly(_pszFileRightNrm))
   {
      AfxMessageBox("Compare: ������ �������� ���������������� �����.",MB_ICONSTOP);
      return;
   }

   sTextRight = MF.Buffer();

   MF.Close();

   Vector   BreakListLeft;
   Vector   BreakListRight;

   BreakListLeft.Resize(DEFAULT_LIST_SIZE);
   BreakListLeft.Delta (DEFAULT_LIST_DELTA);

   BreakListRight.Resize(DEFAULT_LIST_SIZE);
   BreakListRight.Delta (DEFAULT_LIST_DELTA);

   int      iStart = 0;

   // ��������� ������� ��� ����������� ����, ������� ��������
   while (true)
   {
      int   iPos = sTextLeft.Find(0x20,iStart);

      if (iPos == -1)
      {
         // ����� ������
         break;
      }

      // Append
      if (!BreakListLeft.InsertAt(BreakListLeft.Count(),(void*)iPos))
      {
         AfxMessageBox("Compare: ������ �������� ���������������� �����.",MB_ICONSTOP);
         return;
      }

      iStart = iPos + 1;
   }

   iStart = 0;

   // ��������� ������� ��� ����������� ����, ������� ��������
   while (true)
   {
      int   iPos = sTextRight.Find(0x20,iStart);

      if (iPos == -1)
      {
         // ����� ������
         break;
      }

      // Append
      if (!BreakListRight.InsertAt(BreakListRight.Count(),(void*)iPos))
      {
         AfxMessageBox("Compare: ������ �������� ���������������� �����.",MB_ICONSTOP);
         return;
      }

      iStart = iPos + 1;
   }

   iStart = 0;
   
   int   iCnt   = 0;

   // ������� ����� �������
   iCnt = BreakListLeft.Count();

   for (int ii = 0; ii < (iCnt - _iGranularity); ++ii)
   {
      int   iStartPos = (int)BreakListLeft.At(iStart + ii);;
      int   iStopPos  = (int)BreakListLeft.At(iStart + _iGranularity + ii);

      int   iLen = iStopPos - iStartPos + 1;

      DWORD    dwHash = SuperFastHash((BYTE*)(LPCTSTR)sTextLeft.Mid(iStartPos,iLen),iLen);

      _ShingleListLeft.Insert((void*)dwHash);
   }

   // ������� ������ �������
   iCnt = BreakListRight.Count();

   for (int jj = 0; jj < (iCnt - _iGranularity); ++jj)
   {
      int   iStartPos = (int)BreakListRight.At(iStart + jj);;
      int   iStopPos  = (int)BreakListRight.At(iStart + _iGranularity + jj);

      int   iLen = iStopPos - iStartPos + 1;

      DWORD    dwHash = SuperFastHash((BYTE*)(LPCTSTR)sTextRight.Mid(iStartPos,iLen),iLen);

      _ShingleListRight.Insert((void*)dwHash);
   }

   // ������� ����� ����� �������
   iCnt = BreakListLeft.Count();

   int   iIntersection = 0;

   for (int kk = 0; kk < iCnt; ++kk)
   {
      DWORD    dwHash = (DWORD)_ShingleListLeft.At(kk);

      if (_ShingleListRight.Search((void*)dwHash))
      {
         ++iIntersection;
      }
   }

   double   fLikeness = 100.0 * 2.0 * (double)iIntersection / (double)(_ShingleListLeft.Count() + _ShingleListRight.Count());

   char     pszTemp[MAX_PATH];

   sprintf(pszTemp,"%7.2f",fLikeness);

   SetDlgItemText(IDC_EDT_RESULT,pszTemp);

   sTextLeft.Empty();
   sTextRight.Empty();

   Cleanup();
}

/* ******************************************************************** **
** @@ CShinglesDlg::RemoveHTMLtags()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : ���������� ASCIIZ ������
** ******************************************************************** */

void CShinglesDlg::RemoveHTMLtags(BYTE* pBuf,DWORD dwSize)
{
   CMemFile    TempFile; 

   DWORD    dwNewSize  = 0;
   int      iPos       = 0;
   int      iChunkSize = 0;

   while ((DWORD)iPos < dwSize)
   {
      // ������������� ������ ������ �����
      int   iStart = iPos;
            
      // ���������� ������ HTML ����
      while ((pBuf[iPos++] != '<') && ((DWORD)iPos < dwSize))
      {
         ++iChunkSize;
      }

      // �������� ���, ��� ���� �� ���� �� ��������� ����
      if (iChunkSize && (pBuf[iPos - 1] == '<'))
      {
         // HTML open tag marker found
         TempFile.Write(&pBuf[iStart],iChunkSize);
         iChunkSize = 0;
      }

      // ���������� ����� HTML ����
      while ((pBuf[iPos++] != '>') && ((DWORD)iPos < dwSize))
      {
         // � ������� ���� ���
         ;  // ������ ����
      }

      // ��������� ������ ������ ���������� ����, ����� ����� �� ��������
      TempFile.Write(" ",1);

      // ������ iPos ��������� �� ��������� �� ������ HTML ���� ������
      // ��� �� ����� ������
      if ((DWORD)iPos >= dwSize)
      {
         // ��� HTML ���� �������
         // ��������� ����������� ����, ����� ���� ����������, ��� �������� ASCIIZ ������
         TempFile.Write("\0",1);

         // �������� ��������� ���� ����� � ����� � ���������� ����� ������
         TempFile.Flush();
         
         dwNewSize = TempFile.GetLength();

         BYTE*    pTemp = TempFile.Detach();

         memcpy(pBuf,pTemp,dwNewSize);

         // CMemFile ��� ������, ������� ������������ ����� �������������� ������ ������� �� ���
         delete[] pTemp;
         pTemp = NULL;
      }
   }
}

/* ******************************************************************** **
** @@ CShinglesDlg::Unspace()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : ���������� ASCIIZ ������
** ******************************************************************** */

void CShinglesDlg::Unspace(const char* const pszIn,char* pszOut)
{      
   char*    pTemp = (char*)pszIn;
       
   for (int ii = 0, iSize = 0; *pTemp; ++ii)
   {
      if (*pTemp != ' ' && *pTemp != '\t' && *pTemp != '\n' && *pTemp != '\r')
      {
         // �������� ������������ ������ � �������� �����
         pszOut[ii] = *pTemp;
         ++pTemp;
         ++iSize;
      }
      else
      {
         // �������� �� ������������ ������
         pszOut[ii] = 0x20;
         ++pTemp;
         ++iSize;

         // � ������� ��� ��������� �������� � ���������
         while (*pTemp == ' ' || *pTemp == '\t' || *pTemp == '\n' || *pTemp == '\r')
         {
            ++pTemp;
         }
      }
   }

   pszOut[iSize] = 0;   // Ensure ASCIIZ

   // ������� ��� ������� � ��������� �� "������" ������
   if (iSize)
   {
      char*    pEnd = pszOut + (iSize - 1);

      while (pEnd && (pEnd > pszOut) && (*pEnd == ' ' || *pEnd == '\t' || *pTemp == '\n' || *pTemp == '\r'))
      {
         *pEnd = 0;
         --pEnd;
      }
   }
}

/* ******************************************************************** **
** @@ CShinglesDlg::UTF8_2_ANSI()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Modify : 
** @  Notes  :
** ******************************************************************** */

bool CShinglesDlg::UTF8_2_ANSI(char* pBuf,DWORD dwSize)
{           
   if (!pBuf || !dwSize)
   {
      return false;
   }

   int      iUSize = 0;
   int      iASize = 0;

   DWORD    dwOutSize = dwSize + 2;

   WORD*    pBufOut = new WORD[dwOutSize];

   if (!pBufOut)
   {
      // Error
      AfxMessageBox("UTF8_2_ANSI: ������ ��������� ������ ��� ��������� �����.",MB_ICONSTOP);
      return false;
   }

   WORD*    pBufOut2 = new WORD[dwOutSize];

   if (!pBufOut2)
   {
      // Error
      delete[] pBufOut;
      pBufOut = NULL; 
      AfxMessageBox("UTF8_2_ANSI: ������ ��������� ������ ��� ��������� �����.",MB_ICONSTOP);
      return false;
   }

   iUSize = MultiByteToWideChar
   (
      CP_UTF8,    // Translate using UTF-8. When this is set, dwFlags must be zero
      0,
      pBuf,
      dwSize,
      pBufOut,
      dwOutSize
   );

   if (!iUSize)
   {
      // Error
      switch (GetLastError())
      {
         case ERROR_INSUFFICIENT_BUFFER:
         {
            AfxMessageBox("UTF8_2_ANSI: MultiByteToWideChar - ������ ERROR_INSUFFICIENT_BUFFER.",MB_ICONSTOP);
            goto CLEANUP;
         }
         case ERROR_INVALID_FLAGS:
         {
            AfxMessageBox("UTF8_2_ANSI: MultiByteToWideChar - ������ ERROR_INVALID_FLAGS.",MB_ICONSTOP);
            goto CLEANUP;
         }
         case ERROR_INVALID_PARAMETER:
         {
            AfxMessageBox("UTF8_2_ANSI: MultiByteToWideChar - ������ ERROR_INVALID_PARAMETER.",MB_ICONSTOP);
            goto CLEANUP;
         }
         case ERROR_NO_UNICODE_TRANSLATION:
         {
            AfxMessageBox("UTF8_2_ANSI: MultiByteToWideChar - ������ ERROR_NO_UNICODE_TRANSLATION.",MB_ICONSTOP);
            goto CLEANUP;
         }
         default:
         {
            AfxMessageBox("UTF8_2_ANSI: MultiByteToWideChar - ������ ���������������.",MB_ICONSTOP);
            goto CLEANUP;
         }
      }
   }

   iASize = WideCharToMultiByte
   (
      CP_ACP,
      0,
      pBufOut,
      iUSize,
      (char*)pBufOut2,
      dwOutSize,
      NULL,
      NULL
   );

   delete[] pBufOut;
   pBufOut = NULL; 

   if (!iASize)
   {
      // Error
      switch (GetLastError())
      {
         case ERROR_INSUFFICIENT_BUFFER:
         {
            AfxMessageBox("UTF8_2_ANSI: WideCharToMultiByte - ������ ERROR_INSUFFICIENT_BUFFER.",MB_ICONSTOP);
            goto CLEANUP;
         }
         case ERROR_INVALID_FLAGS:
         {
            AfxMessageBox("UTF8_2_ANSI: WideCharToMultiByte - ������ ERROR_INVALID_FLAGS.",MB_ICONSTOP);
            goto CLEANUP;
         }
         case ERROR_INVALID_PARAMETER:
         {
            AfxMessageBox("UTF8_2_ANSI: WideCharToMultiByte - ������ ERROR_INVALID_PARAMETER.",MB_ICONSTOP);
            goto CLEANUP;
         }
         default:
         {
            AfxMessageBox("UTF8_2_ANSI: WideCharToMultiByte - ������ ���������������.",MB_ICONSTOP);
            goto CLEANUP;
         }
      }
   }

CLEANUP:

   // ������� �������� �����
   memset(pBuf,0,dwSize);

   if (iASize)
   {
      memcpy(pBuf,pBufOut2,iASize);
   }

   if (pBufOut)
   {
      delete[] pBufOut;
      pBufOut = NULL; 
   }

   if (pBufOut2)
   {
      delete[] pBufOut2;
      pBufOut2 = NULL; 
   }

   return true;
}

/* ******************************************************************** **
** @@ CShinglesDlg::Lower_n_Remove_StopSigns()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Modify : 
** @  Notes  : ������������ ������ � ������ ������� � ������� ����-�������
** ******************************************************************** */

void CShinglesDlg::Lower_n_Remove_StopSigns(char* pBuf)
{
   CString     sText = pBuf;

   sText.MakeLower();

   DWORD    dwSize = sText.GetLength();

   for (int ii = 0; ii < STOP_SIGNS_CNT; ++ii)
   {
      sText.Remove(STOP_SIGNS_SET[ii]);
   }

   DWORD    dwNewSize = sText.GetLength();

   memcpy(pBuf,(LPCTSTR)sText,dwNewSize);

   // �������� ������� ������
   memset(pBuf + dwNewSize,0,dwSize - dwNewSize);
}

/* ******************************************************************** **
** @@ CShinglesDlg::Lower_n_Replace_StopSigns()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Modify : 
** @  Notes  : ������������ ������ � ������ ������� � �������� ����-�������
** @  Notes  : �� �������, ��� ������������� �������� ����
** ******************************************************************** */

void CShinglesDlg::Lower_n_Replace_StopSigns(char* pBuf)
{
   int   iSize = strlen(pBuf);

   // XLAT
   for (int ii = 0; ii < iSize; ++ii)
   {
      pBuf[ii] = STOP_SIGNS_TABLE[(BYTE)pBuf[ii]];
   }

   CString     sText = pBuf;

   sText.MakeLower();

   strcpy(pBuf,(LPCTSTR)sText);
}

/* ******************************************************************** **
** @@ CShinglesDlg::RemoveHtmlSpecialEn()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Modify : 
** @  Notes  : ���������� RemoveHtmlSpecialRu(), �������� ��������� 
** @  Notes  : �������� HTML �����, ����������� ���������� � ������ �����
** @  Notes  : ���������� ������� ��� ������ �������� ASCII
** ******************************************************************** */

void CShinglesDlg::RemoveHtmlSpecialEn(char* pBuf)
{
   CMemFile    TempFile; 

   DWORD    dwSize = strlen(pBuf);

   int      iPos       = 0;
   int      iChunkSize = 0;

   while (true)
   {
      // ������������� ������ ������ �����
      int   iStart = iPos;
            
      // ���������� ������ HTML ����
      while ((pBuf[iPos++] != '&') && ((DWORD)iPos < dwSize))
      {
         ++iChunkSize;
      }

      // �������� ���, ��� ���� �� ���� �� ��������� ����
      if (iChunkSize && (pBuf[iPos - 1] == '&'))
      {
         // HTML open tag marker found
         TempFile.Write(&pBuf[iStart],iChunkSize);
         iChunkSize = 0;
      }

      // ��������� ������� ������ ���������� ����
      int   iTagStartPos = iPos - 1;

      // ���������� ����� HTML ����
      int   iLen = 0;

      while ((iLen < MAX_TAG_SIZE) && (pBuf[iPos] != ';') && ((DWORD)iPos < dwSize))
      {
         ++iLen;
         ++iPos;
      }

      // ��������� ����������. 
      // ����� ������������ ��� ������, ����������� �� ��, 
      // ��� ��� ������ ������ ������ ';' 
      char  pszTemp[MAX_TAG_SIZE + 1]; 

      memset(pszTemp,0,sizeof(pszTemp));  // Clear

      if (pBuf[iPos] == ';')
      {
         // ��������� �� �������� HTML-���
         if (pBuf[iTagStartPos + 1] == '#')
         {
            // ������ �������� HTML-���
            // ��������� ������ ������ ���������� ����, ����� ����� �� ��������
            TempFile.Write(" ",1);
            *pszTemp = '*';  // Special flag
         }
         else
         {
            // �������� ������� �� ��������� ����������
            int   iMinSize = min((BYTE)(iLen + 2),MAX_TAG_SIZE);

            strncpy(pszTemp,&pBuf[iTagStartPos],iMinSize);
            pszTemp[iMinSize] = 0; // Ensure ASCIIZ
         }
      }  

      // ��������� ��������� ������� �� ���������� � HTML-����� �� ������
      TST_Node*   pFound = NULL;

      // ���� ������ ';' �� ��� ������, �� � ��������� ������
      // ���� ������ ';' ��� ������, �� ��� ������ �������� HTML-���
      // (������������ ������� ����.�����), �� ��������� ������
      if (*pszTemp && *pszTemp != '*')
      {
         // ��������� ��������� ������� �� ���������� � HTML-����� �� ������
         pFound = (TST_Node*)_DixHtmlEn.Search(pszTemp);
      }

      // ���� ������ �� ������� ��� �������� �� �������������, � ��� ��� 
      // �� �������� HTML-���, �� ������ �������� ����������� �������
      if (pFound)
      {
         // Stopword found !
         // ��������� ������ ������ ���������� ����, ����� ����� �� ��������
         TempFile.Write(" ",1);
      }
      else
      {
         // ����������� �� ���������, ���� ��� ������ � ������ �������� HTML-���
         if (*pszTemp && *pszTemp != '*')
         {
            // Smth else
            // ������� �� ������ � ������� - �������� � �����
            TempFile.Write(&pBuf[iTagStartPos],iLen + 2);
         }
      }

      // ��������� � ���������� �������
      ++iPos;

      // ������ iPos ��������� �� ��������� �� ������ HTML ���� ������
      // ��� �� ����� ������
      if ((DWORD)iPos >= dwSize)
      {
         // ��� HTML ���� �������
         // �������� ������� �����
         if ((DWORD)iStart < dwSize)
         {
            TempFile.Write(&pBuf[iStart],dwSize - iStart);
         }

         // ��������� ����������� ����, ����� ���� ����������, ��� �������� ASCIIZ ������
         TempFile.Write("",1);

         // �������� ��������� ���� ����� � �����
         TempFile.Flush();

         DWORD    dwNewSize = TempFile.GetLength();

         BYTE*    pTemp = TempFile.Detach();

         memcpy(pBuf,pTemp,dwNewSize);

         // CMemFile ��� ������, ������� ������������ 
         // ����� �������������� ������ ������� �� ���
         delete[] pTemp;
         pTemp = NULL;

         return;
      }
   }
}

/* ******************************************************************** **
** @@ CShinglesDlg::RemoveHtmlSpecialRu()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Modify : 
** @  Notes  : ���������� RemoveHtmlSpecialEn(), �������� ��������� 
** @  Notes  : �������� HTML �����, ����������� ���������� � ������ ���
** @  Notes  : ���������� ������� ��� ������� �������� ASCII
** ******************************************************************** */

void CShinglesDlg::RemoveHtmlSpecialRu(char* pBuf)
{
   CMemFile    TempFile; 

   DWORD    dwSize = strlen(pBuf);

   int      iPos       = 0;
   int      iChunkSize = 0;

   while (true)
   {
      // ������������� ������ ������ �����
      int   iStart = iPos;
            
      // ���������� ������ HTML ����
      while ((pBuf[iPos++] != '&') && ((DWORD)iPos < dwSize))
      {
         ++iChunkSize;
      }

      // �������� ���, ��� ���� �� ���� �� ��������� ����
      if (iChunkSize && (pBuf[iPos - 1] == '&'))
      {
         // HTML open tag marker found
         TempFile.Write(&pBuf[iStart],iChunkSize);
         iChunkSize = 0;
      }

      // ��������� ������� ������ ���������� ����
      int   iTagStartPos = iPos - 1;

      // ���������� ����� HTML ����
      int   iLen = 0;

      while ((iLen < MAX_TAG_SIZE) && (pBuf[iPos] != ';') && ((DWORD)iPos < dwSize))
      {
         ++iLen;
         ++iPos;
      }

      // ��������� ����������. 
      // ����� ������������ ��� ������, ����������� �� ��, 
      // ��� ��� ������ ������ ������ ';' 
      char  pszTemp[MAX_TAG_SIZE + 1]; 

      memset(pszTemp,0,sizeof(pszTemp));  // Clear

      if (pBuf[iPos] == ';')
      {
         int   iMinSize = min((BYTE)(iLen + 2),MAX_TAG_SIZE);

         strncpy(pszTemp,&pBuf[iTagStartPos],iMinSize);
         pszTemp[iMinSize] = 0; // Ensure ASCIIZ
      }

      // ��������� ��������� ������� �� ���������� � HTML-����� �� ������
      TST_Node*   pFound = NULL;

      // ���� ������ ';' �� ��� ������, �� � ��������� ������
      if (*pszTemp)
      {
         // ���� HTML-��� � �������
         pFound = (TST_Node*)_DixHtmlRu.Search(pszTemp);
      }

      // ���� ������ �� ������� ��� �������� �� �������������,
      // �� ������ �������� ����������� �������
      if (pFound)
      {
         // Stopword found !
         // ��������� ������ ������ ���������� ����, ����� ����� �� ��������
         TempFile.Write(" ",1);
      }
      else
      {
         // Smth else
         // ������� �� ������ � ������� - �������� � �����
         TempFile.Write(&pBuf[iTagStartPos],iLen + 2);
      }

      // ��������� � ���������� �������
      ++iPos;

      // ������ iPos ��������� �� ��������� �� ������ HTML ���� ������
      // ��� �� ����� ������
      if ((DWORD)iPos >= dwSize)
      {
         // ��� HTML ���� �������
         // ��������� ����������� ����, 
         // ����� ���� ����������, ��� �������� ASCIIZ ������
         TempFile.Write("",1);

         // �������� ��������� ���� ����� � �����
         TempFile.Flush();
         
         DWORD    dwNewSize = TempFile.GetLength();

         BYTE*    pTemp = TempFile.Detach();

         memcpy(pBuf,pTemp,dwNewSize);

         // CMemFile ��� ������, ������� ������������ 
         // ����� �������������� ������ ������� �� ���
         delete[] pTemp;
         pTemp = NULL;

         return;
      }
   }
}

/* ******************************************************************** **
** @@ CShinglesDlg::RemoveCommonWords()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Modify : 
** @  Notes  : ������� �� ����� ����� �� ������ COMMON_WORDS
** ******************************************************************** */

void CShinglesDlg::RemoveCommonWords(char* pBuf)
{  
   _iThreshold = m_Threshold.GetPos();

   CMemFile    TempFile; 

   char*    pStart = pBuf;

   // ������� ���� �� �����
   while (true)
   {
      while (*pStart && (*pStart == 0x20))
      {  
         ++pStart;  // ���������� �������
      }

      if (!*pStart)
      {
         // ��������� ����� �����
         return;
      }

      int   iLen = 0;   // �������

      // ��������� �� ������ ����� �����
      char*    pWord = pStart;

      while (*pStart && (*pStart != 0x20) && (iLen < MAX_PATH))
      {
         ++pStart;   // Next
         ++iLen;     // ������������ �����
      }

      // ��������� ���� ����� �����, ���� ����� �����, ���� � �� � ������
      // ����� ������ ���� �� ������ ����������� �����
      if (iLen && (iLen >= _iThreshold))
      {
         char     pszTemp[MAX_PATH + 1];  // ASCIIZ

         memset(pszTemp,0,sizeof(pszTemp));  // Clear

         strncpy(pszTemp,pWord,iLen);
         pszTemp[iLen] = 0;

         // ��������� ��������� ������� �� ���������� �� ������� �� ������ COMMON_WORDS
         if (*pszTemp)
         {
            // ���� ����� � �������
            TST_Node*   pFound = (TST_Node*)_DixCommonWords.Search(pszTemp);
         
            // ���� ��� �� common word � ��� ����� ������ �����������
            if (!pFound)
            {
               // �������� ��������� ����� � �������� �����
               TempFile.Write(pszTemp,iLen);
               // ��������� ������
               TempFile.Write(" ",1);
            }
         }
      }  

      if (!*pStart)
      {
         // ��������� ����� �����
         // ��������� ����������� ����, ����� ���� ����������, ��� �������� ASCIIZ ������
         TempFile.Write("",1);

         // �������� ��������� ���� ����� � �����
         TempFile.Flush();
         
         DWORD    dwNewSize = TempFile.GetLength();

         BYTE*    pTemp = TempFile.Detach();

         memcpy(pBuf,pTemp,dwNewSize);

         // CMemFile ��� ������, ������� ������������ 
         // ����� �������������� ������ ������� �� ���
         delete[] pTemp;
         pTemp = NULL;

         return;
      }
   }
}

/* ******************************************************************** **
** @@                   End of File
** ******************************************************************** */
