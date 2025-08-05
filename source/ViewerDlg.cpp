// ---------------------------------------------------------------------------------------------------------------------------------
//  _     _ _                         _____  _                            
// | |   | (_)                       |  __ \| |                           
// | |   | |_  _____      __ ___ _ __| |  | | | __ _      ___ _ __  _ __  
//  \ \ / /| |/ _ \ \ /\ / // _ \ '__| |  | | |/ _` |    / __| '_ \| '_ \ 
//   \ V / | |  __/\ V  V /|  __/ |  | |__| | | (_| | _ | (__| |_) | |_) |
//    \_/  |_|\___| \_/\_/  \___|_|  |_____/|_|\__, |(_) \___| .__/| .__/ 
//                                              __/ |        | |   | |    
//                                             |___/         |_|   |_|    
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

#include "stdafx.h"
#include "Common.h"
#include "ViewerDlg.h"
#include "Render.h"
#include "Logger.h"
#include "OctMap.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CViewerDlg, CDialog)
	//{{AFX_MSG_MAP(CViewerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ---------------------------------------------------------------------------------------------------------------------------------

	CViewerDlg::CViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewerDlg::IDD, pParent), render(NULL)
{
	//{{AFX_DATA_INIT(CViewerDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// ---------------------------------------------------------------------------------------------------------------------------------

	CViewerDlg::~CViewerDlg()
{
	delete render;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewerDlg)
	//}}AFX_DATA_MAP
}

// ---------------------------------------------------------------------------------------------------------------------------------

BOOL	CViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// Clear the log

	unlink(logger.logFile().c_str());

	// Set the window size

	enum	{WIDTH  = 640};
	enum	{HEIGHT = 480};

	// Current window position & size

	CRect	clientRect;
	GetClientRect(clientRect);
	CRect	windowRect;
	GetWindowRect(windowRect);

	// New window position & size

	windowRect.right  += WIDTH  - (clientRect.right  - clientRect.left);
	windowRect.bottom += HEIGHT - (clientRect.bottom - clientRect.top); 
	MoveWindow(windowRect);

	// Create a renderer with an empty polygon list (and a default camera position)

	vector<primitive<> >	polygonList;
	point3	cameraPos(0,0,0);
	render = new Render(*GetDC(), *this, polygonList, cameraPos);

	// Read the OCT file

	readOct("steps.oct");

	// Setup a timer for rendering

	SetTimer(0, 0, NULL);

	// Hide the cursor

	ShowCursor(FALSE);

	// Done

	return TRUE;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		if (render) render->flip();
		CDialog::OnPaint();
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

HCURSOR	CViewerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (render) render->updateWindowPosition();
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::OnTimer(UINT nIDEvent) 
{
	// Draw a frame

	render->renderFrame();
	
	// Keeeep going

	CDialog::OnTimer(nIDEvent);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect	r;
	GetClientRect(r);
	POINT	topLeft; topLeft.x = r.left; topLeft.y = r.top;
	ClientToScreen(&topLeft);
	POINT	botRight; botRight.x = r.right; botRight.y = r.bottom;
	ClientToScreen(&botRight);

	point.x += topLeft.x;
	point.y += topLeft.y;
	int	w = r.Width();
	int	h = r.Height();
	int	cx = topLeft.x + r.Width() / 2;
	int	cy = topLeft.y + r.Height() / 2;
	int	mouseX = point.x - cx;
	int	mouseY = point.y - cy;

	static	bool	beenHere;

	if (beenHere)	render->mouseMoved(mouseX, mouseY);
	else		beenHere = true;

	if (mouseX || mouseY)
		SetCursorPos(cx, cy);

	CDialog::OnMouseMove(nFlags, point);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	render->mousePressed(true, render->rButton());
	CDialog::OnLButtonDown(nFlags, point);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	render->mousePressed(true, render->rButton());
	CDialog::OnLButtonDblClk(nFlags, point);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	render->mousePressed(false, render->rButton());
	CDialog::OnLButtonUp(nFlags, point);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ShowCursor(TRUE);
	CMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu(MF_STRING, MENU_LOAD_OCT, "Load OCT file");
	POINT	loc;
	GetCursorPos(&loc);
	menu.TrackPopupMenu(TPM_CENTERALIGN, loc.x, loc.y, this, NULL);
	ShowCursor(FALSE);
}

// ---------------------------------------------------------------------------------------------------------------------------------

BOOL	CViewerDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam == MENU_LOAD_OCT)
	{
		ShowCursor(TRUE);
		char		fname[256] = "";
		OPENFILENAME	of;
		memset(&of, 0, sizeof(OPENFILENAME));

		of.lStructSize  = sizeof(OPENFILENAME);
		of.hwndOwner    = GetSafeHwnd();
		of.lpstrFilter  = "OCT (*.oct)\0*.oct\0All files (*.*)\0*.*\0\0";
		of.nFilterIndex = 1;
		of.lpstrFile    = fname;
		of.nMaxFile     = 256;
		of.lpstrTitle = "Open";
		of.Flags        = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_EXPLORER;
		of.lpstrDefExt  = "oct";

		if (GetOpenFileName(&of))
		{
			readOct(fname);
		}
		ShowCursor(FALSE);
	}

	return CDialog::OnCommand(wParam, lParam);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	CViewerDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
		case 'A': render->antialias()          = !render->antialias();                   break;
		case 'B': render->blinn()              = !render->blinn();                       break;
		case 'C': render->cull()               = !render->cull();                        break;
		case 'D': render->polygons()           = !render->polygons();                    break;
		case 'J': render->jumble()             = !render->jumble();
				// INTENTIONAL FALL-THROUGH
		case 'E': 
			{
				for (int i = 0; i < render->polygonList.size(); i++)
				{
					render->polygonList[i][0].world().x() += (frand() - 0.5f) * 5.0f;
					render->polygonList[i][0].world().y() += (frand() - 0.5f) * 5.0f;
					render->polygonList[i][0].world().z() += (frand() - 0.5f) * 5.0f;
					render->polygonList[i][1].world().x() += (frand() - 0.5f) * 5.0f;
					render->polygonList[i][1].world().y() += (frand() - 0.5f) * 5.0f;
					render->polygonList[i][1].world().z() += (frand() - 0.5f) * 5.0f;
					render->polygonList[i][2].world().x() += (frand() - 0.5f) * 5.0f;
					render->polygonList[i][2].world().y() += (frand() - 0.5f) * 5.0f;
					render->polygonList[i][2].world().z() += (frand() - 0.5f) * 5.0f;
					render->polygonList[i].calcPlane(false);
				}
				break;
			}
		case 'F': render->fly()                = !render->fly();                         break;
		case 'G': render->gravity()            = !render->gravity();                     break;
		case 'H': render->help()               = !render->help();                        break;
		case 'I': break;
		case 'K': break;
		case 'L':
			{
				FILE	*fp = fopen("campos.dat", "rb");
				if (fp)
				{
					fread(&render->cameraPosition(), sizeof(render->cameraPosition()), 1, fp);
					fread(&render->velocityVector(), sizeof(render->velocityVector()), 1, fp);
					fread(&render->radiusVector(), sizeof(render->radiusVector()), 1, fp);
					fread(&render->xrot(), sizeof(render->xrot()), 1, fp);
					fread(&render->yrot(), sizeof(render->yrot()), 1, fp);
					fclose(fp);
				}
				break;
			}
		case 'M': render->manual()             = !render->manual();                      break;
		case 'N': render->filterPulseJumps()   = !render->filterPulseJumps();            break;
		case 'O': render->wireframe()          = !render->wireframe();                   break;
		case 'P': render->perspectiveCorrect() = (render->perspectiveCorrect() + 1) % 3; break;
		case 'Q': break;
		case 'R': render->gravityVector()      = vector3(frand() - 0.5f, frand() - 0.5f, frand() - 0.5f); render->gravityVector().normalize(); break;
		case 'S':
			{
				FILE	*fp = fopen("campos.dat", "wb");
				if (fp)
				{
					fwrite(&render->cameraPosition(), sizeof(render->cameraPosition()), 1, fp);
					fwrite(&render->velocityVector(), sizeof(render->velocityVector()), 1, fp);
					fwrite(&render->radiusVector(), sizeof(render->radiusVector()), 1, fp);
					fwrite(&render->xrot(), sizeof(render->xrot()), 1, fp);
					fwrite(&render->yrot(), sizeof(render->yrot()), 1, fp);
					fclose(fp);
				}
				break;
			}
		case 'T': logger.logMask()             = ~logger.logMask();		         break;
		case 'U': break;
		case 'V': render->collisionType()      = (render->collisionType() + 1) % 3;      break;
		case 'W': render->widescreen()         = !render->widescreen();                  break;
		case 'X': render->jumbleScale() += (FLT) 0.1;                                    break;
		case 'Y': break;
		case 'Z': render->jumbleScale() -= (FLT) 0.1;                                    break;
		case VK_F2: if (render->radiusVector().x() > 1) render->radiusVector().x() -= (FLT) 1;break;
		case VK_F3: render->radiusVector().x() += (FLT) 1;                               break;
		case VK_F4: if (render->radiusVector().y() > 1) render->radiusVector().y() -= (FLT) 1;break;
		case VK_F5: render->radiusVector().y() += (FLT) 1;                               break;
		case VK_F6: if (render->radiusVector().z() > 1) render->radiusVector().z() -= (FLT) 1;break;
		case VK_F7: render->radiusVector().z() += (FLT) 1;                               break;
	}

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	CViewerDlg::readOct(char *filename)
{
	OctMap	om;
	if (!om.Load(filename))
	{
		char	dsp[256];
		sprintf(dsp, "Unable to load data file \"%s\"", filename);
		AfxMessageBox(dsp);
		return false;
	}

	// Convert the data into polygons

	octVert	*ov = om.GetVerts();
	octFace *of = om.GetFaces();
	vector<primitive<> >	polygonList;

	// For each face...

	for (int i = 0; i < om.GetNumFaces(); i++)
	{
		octFace		curFace = of[i];

		// For each vertex in the current face...

		vert<>	v0;
		v0.world().x() = ov[curFace.start+0].pos[0];
		v0.world().y() = ov[curFace.start+0].pos[1];
		v0.world().z() = ov[curFace.start+0].pos[2];

		for (int j = 1; j < curFace.num - 1; j++)
		{
			primitive<>	curPrim;

			vert<>	v1;
			v1.world().x() = ov[curFace.start+j+0].pos[0];
			v1.world().y() = ov[curFace.start+j+0].pos[1];
			v1.world().z() = ov[curFace.start+j+0].pos[2];

			vert<>	v2;
			v2.world().x() = ov[curFace.start+j+1].pos[0];
			v2.world().y() = ov[curFace.start+j+1].pos[1];
			v2.world().z() = ov[curFace.start+j+1].pos[2];

			curPrim += v0;
			curPrim += v1;
			curPrim += v2;

			curPrim.calcPlane(false);
			curPrim.setWorldTexture(0.4f, 0.4f);
			polygonList.push_back(curPrim);
		}
	}

	octVect3	*cp = om.GetPlayerStartPos();
	point3	cameraPos(cp[0][0], cp[0][1], cp[0][2]);
	render->cameraPosition() = cameraPos;

	render->polygonList.clear();
	render->polygonList = polygonList;
	render->polygonListBackup = polygonList;
	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// ViewerDlg.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
