// ---------------------------------------------------------------------------------------------------------------------------------
//  _     _ _                         _____  _           _     
// | |   | (_)                       |  __ \| |         | |    
// | |   | |_  _____      __ ___ _ __| |  | | | __ _    | |__  
//  \ \ / /| |/ _ \ \ /\ / // _ \ '__| |  | | |/ _` |   | '_ \ 
//   \ V / | |  __/\ V  V /|  __/ |  | |__| | | (_| | _ | | | |
//    \_/  |_|\___| \_/\_/  \___|_|  |_____/|_|\__, |(_)|_| |_|
//                                              __/ |          
//                                             |___/           
//
// Collision detection tutorial
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Originally released under a custom license.
// This historical re-release is provided under the MIT License.
// See the LICENSE file in the repo root for details.
//
// https://github.com/nettlep
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/06/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_H_VIEWERDLG
#define _H_VIEWERDLG

// ---------------------------------------------------------------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------------------------------------------------------------

class	Render;

// ---------------------------------------------------------------------------------------------------------------------------------
// Required includes
// ---------------------------------------------------------------------------------------------------------------------------------

#include "resource.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	CViewerDlg : public CDialog
{
public:
			CViewerDlg(CWnd* pParent = NULL);
			~CViewerDlg();

	//{{AFX_DATA(CViewerDlg)
	enum { IDD = IDD_VIEWER_DIALOG };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	bool		readOct(char *filename);

	HICON		m_hIcon;
	Render		*render;

	//{{AFX_MSG(CViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // _H_VIEWERDLG
// ---------------------------------------------------------------------------------------------------------------------------------
// ViewerDlg.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

