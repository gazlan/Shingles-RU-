/* ******************************************************************** **
** @@ ShinglesDlg
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Dscr   :
** ******************************************************************** */

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#if !defined(AFX_SHINGLESDLG_H__99CF9EC5_1FD8_43B2_BE07_6F48FFD582E5__INCLUDED_)
#define AFX_SHINGLESDLG_H__99CF9EC5_1FD8_43B2_BE07_6F48FFD582E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   Classes
** ******************************************************************** */

class CShinglesDlg : public CDialog
{
   private:

      SortedVector   _ShingleListLeft;
      SortedVector   _ShingleListRight;

      MMF      _Left;
      MMF      _Right;

      TST      _DixHtmlEn;
      TST      _DixHtmlRu;
      TST      _DixStopWords;
      TST      _DixCommonWords;

      int      _iGranularity;
      int      _iThreshold;

      bool     _bInitialized;
      bool     _bNormalizedLeft;
      bool     _bNormalizedRight;
      bool     _bNormalizeHiAscii;
      bool     _bLoadedLeft;
      bool     _bLoadedRight;
      bool     _bUtf8;

      char     _pszFileLeft    [MAX_PATH + 1];
      char     _pszFileRight   [MAX_PATH + 1];
      char     _pszFileLeftNrm [MAX_PATH + 1];
      char     _pszFileRightNrm[MAX_PATH + 1];

   private:

      void     RemoveCommonWords(char* pBuf);
      void     RemoveHtmlSpecialEn(char* pBuf);
      void     RemoveHtmlSpecialRu(char* pBuf);
      void     Lower_n_Remove_StopSigns(char* pBuf);
      void     Lower_n_Replace_StopSigns(char* pBuf);
      bool     UTF8_2_ANSI(char* pBuf,DWORD dwSize);
      void     Unspace(const char* const pszIn,char* pszOut);
      DWORD    Normalize(BYTE* pBuf,DWORD dwSize);
      void     RemoveHTMLtags(BYTE* pBuf,DWORD dwSize);
      void     Compare();
      void     Cleanup();

// Construction
public:
	CShinglesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CShinglesDlg)
	enum { IDD = IDD_SHINGLES_DIALOG };
	CSpinButtonCtrl	m_Threshold;
	CSpinButtonCtrl	m_Granularity;
	CButton	m_UTF;
	CButton	m_HiAscii;
	CButton	m_ChkRight;
	CButton	m_ChkLeft;
	CString	m_EdtLeft;
	CString	m_EdtRight;
	CString	m_PathLeft;
	CString	m_PathRight;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShinglesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CShinglesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	afx_msg void OnBtnClrLeft();
	afx_msg void OnBtnClrRight();
	afx_msg void OnBtnLoadLeft();
	afx_msg void OnBtnLoadRight();
	afx_msg void OnBtnCompare();
	afx_msg void OnChkLeft();
	afx_msg void OnChkRight();
	afx_msg void OnBtnNrmLeft();
	afx_msg void OnBtnNrmRight();
	afx_msg void OnChkUtf8();
	afx_msg void OnChkHiAscii();
	afx_msg void OnChangeEdtGranularity();
	afx_msg void OnChangeEdtThreshold();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHINGLESDLG_H__99CF9EC5_1FD8_43B2_BE07_6F48FFD582E5__INCLUDED_)

/* ******************************************************************** **
**                End of File
** ******************************************************************** */
