/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  MUI Control Bar
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2018 by David S. Register                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 *
 *
 */

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/statline.h>

#include "dychart.h"

#include "chcanv.h"
#include "MUIBar.h"
#include "OCPNPlatform.h"
#include "CanvasOptions.h"

#ifdef ocpnUSE_SVG
#include "wxsvg/include/wxSVG/svg.h"
#endif // ocpnUSE_SVG

#ifdef __OCPN__ANDROID__
#include "androidUTIL.h"
#endif



//------------------------------------------------------------------------------
//    External Static Storage
//------------------------------------------------------------------------------

extern OCPNPlatform              *g_Platform;
extern bool                      g_bEffects;

//  Helper utilities
static wxBitmap LoadSVG( const wxString filename, unsigned int width, unsigned int height )
{
    #ifdef ocpnUSE_SVG
    wxSVGDocument svgDoc;
    if( svgDoc.Load(filename) )
        return wxBitmap( svgDoc.Render( width, height, NULL, true, true ) );
    else
        return wxBitmap(width, height);
    #else
        return wxBitmap(width, height);
    #endif // ocpnUSE_SVG
}

double getValue(int animationType, double t);


//  Helper classes


//------------------------------------------------------------------------------
//    MUIButton Static Storage
//------------------------------------------------------------------------------

class MUIButton: public wxWindow
{    
    DECLARE_DYNAMIC_CLASS( MUIButton )
    DECLARE_EVENT_TABLE()
    
    wxSize DoGetBestSize() const;
public:
    MUIButton();
    MUIButton(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString & bitmap = wxEmptyString, const wxString & bitmapToggle = wxEmptyString,
              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER);
    
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString & bitmap = wxEmptyString, const wxString & bitmapToggle = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER);
    
    ~MUIButton();
    
    void Init();
    void CreateControls();
    
    void ToggleBitmap( bool bt );
    
    void OnSize( wxSizeEvent& event );
    void OnPaint( wxPaintEvent& event );
    void OnLeftDown( wxMouseEvent& event );
    void OnLeftUp( wxMouseEvent& event );
    
    
    wxBitmap GetBitmapResource( const wxString& name );
    
    wxIcon GetIconResource( const wxString& name );
    
    /// Should we show tooltips?
    static bool ShowToolTips();
    
private:
    wxString m_bitmapFile;
    wxString m_bitmapFileToggle;
    wxBitmap m_bitmap;
    wxBitmap m_bitmapNormal;
    wxBitmap m_bitmapToggle;

    bool mToggle;
};


IMPLEMENT_DYNAMIC_CLASS( MUIButton, wxWindow )



BEGIN_EVENT_TABLE( MUIButton, wxWindow )

EVT_SIZE( MUIButton::OnSize )
EVT_PAINT( MUIButton::OnPaint )
EVT_LEFT_DOWN( MUIButton::OnLeftDown )
EVT_LEFT_UP( MUIButton::OnLeftUp )

END_EVENT_TABLE()


MUIButton::MUIButton()
{
    Init();
}

MUIButton::MUIButton(wxWindow* parent, wxWindowID id, const wxString & bitmap, const wxString & bitmapToggle, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, bitmap, bitmapToggle, pos, size, style);
}


bool MUIButton::Create(wxWindow* parent, wxWindowID id, const wxString & bitmap, const wxString & bitmapToggle, const wxPoint& pos, const wxSize& size, long style)
{
    wxWindow::Create(parent, id, pos, size, style);
    m_bitmapFile = bitmap;
    m_bitmapFileToggle = bitmapToggle;
    
    CreateControls();
    return true;
}


MUIButton::~MUIButton()
{
}


void MUIButton::Init()
{
    bool mToggle = false;
}


void MUIButton::CreateControls()
{    
    this->SetForegroundColour(wxColour(255, 255, 255));
    
    wxColour backColor = GetGlobalColor( _("GREY3"));
    SetBackgroundColour(backColor);
    
    this->SetFont(wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Tahoma")));
}


bool MUIButton::ShowToolTips()
{
    return true;
}

void MUIButton::ToggleBitmap(bool bt)
{
    mToggle = bt;
    if(mToggle)
        m_bitmap = m_bitmapToggle;
    else
        m_bitmap = m_bitmapNormal;
    Refresh();
}


void MUIButton::OnSize( wxSizeEvent& event )
{
    if(m_bitmap.IsOk()){
        if( event.GetSize() == m_bitmap.GetSize())
            return;
    }

    if(!m_bitmapFile.IsEmpty())
        m_bitmapNormal = LoadSVG( m_bitmapFile, event.GetSize().x, event.GetSize().y );

    if(!m_bitmapFileToggle.IsEmpty())
        m_bitmapToggle = LoadSVG( m_bitmapFileToggle, event.GetSize().x, event.GetSize().y );
    
    if(!m_bitmapToggle.IsOk() || m_bitmapFileToggle.IsEmpty())
        m_bitmapToggle = m_bitmapNormal;
    
    if(mToggle)
        m_bitmap = m_bitmapToggle;
    else
        m_bitmap = m_bitmapNormal;
        
    Refresh();
}

wxBitmap MUIButton::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon MUIButton::GetIconResource( const wxString& name )
{
    // Icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
}

wxSize MUIButton::DoGetBestSize() const
{
//     wxSize labelSize = wxDefaultSize;
//     GetTextExtent(m_Label, &labelSize.x, &labelSize.y);
//     return wxSize(wxMax(40, labelSize.x + 20), wxMax(20, labelSize.y + 10));
    return wxSize(40, 40);
}


void MUIButton::OnPaint( wxPaintEvent& event )
{
    wxPaintDC dc( this );
    
    if(m_bitmap.IsOk()){
        dc.DrawBitmap(m_bitmap, 0, 0, true);
    }
    
        
#if 0
    wxBufferedPaintDC dc(this);
    
    wxRect clientRect = GetClientRect();
    wxRect gradientRect = clientRect;
    gradientRect.SetHeight(gradientRect.GetHeight()/2 + ((GetCapture() == this) ? 1 : 0));
    if(GetCapture() != this)
    {
        dc.GradientFillLinear(gradientRect, 
                              m_GradientTopStartColour, m_GradientTopEndColour, wxSOUTH);
    }
    else
    {
        dc.SetPen(wxPen(m_PressedColourTop));
        dc.SetBrush(wxBrush(m_PressedColourTop));
        dc.DrawRectangle(gradientRect);
    }
    
    gradientRect.Offset(0, gradientRect.GetHeight());
    
    if(GetCapture() != this)
    {
        dc.GradientFillLinear(gradientRect, 
                              m_GradientBottomStartColour, m_GradientBottomEndColour, wxSOUTH);
    }
    else
    {
        dc.SetPen(wxPen(m_PressedColourBottom));
        dc.SetBrush(wxBrush(m_PressedColourBottom));
        dc.DrawRectangle(gradientRect);
    }
    dc.SetPen(wxPen(GetBackgroundColour()));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(0, 0, clientRect.GetWidth(), clientRect.GetHeight());
    dc.SetFont(GetFont());
    dc.SetTextForeground(GetForegroundColour());
    if(GetCapture() == this)
    {
        clientRect.Offset(1, 1);
    }
    dc.DrawLabel(m_Label, clientRect, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
#endif    
}


#if 1
void MUIButton::OnLeftDown( wxMouseEvent& event )
{
    event.Skip();
}


void MUIButton::OnLeftUp( wxMouseEvent& event )
{
    if(GetClientRect().Contains(event.GetPosition())){
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, GetId());
        GetParent()->GetEventHandler()->AddPendingEvent( evt );
    }
    event.Skip();
}
#endif






#define CANVAS_OPTIONS_ANIMATION_TIMER_1 800
#define CANVAS_OPTIONS_TIMER 801

//------------------------------------------------------------------------------
//          MUIBar Window Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MUIBar, wxDialog)
EVT_TIMER ( CANVAS_OPTIONS_ANIMATION_TIMER_1, MUIBar::onCanvasOptionsAnimationTimerEvent )
//EVT_PAINT ( MUIBar::OnPaint )
EVT_SIZE( MUIBar::OnSize )
EVT_MENU(-1, MUIBar::OnToolLeftClick)
EVT_TIMER(CANVAS_OPTIONS_TIMER, MUIBar::CaptureCanvasOptionsBitmapChain)

END_EVENT_TABLE()

// Define a constructor
MUIBar::MUIBar()
{
}

MUIBar::MUIBar(ChartCanvas* parent, int orientation, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_parentCanvas = parent;
    m_orientation = orientation;
    
    //SetBackgroundStyle( wxBG_STYLE_TRANSPARENT );
    //wxWindow::Create(parent, id, pos, size, style, name);
    //long mstyle = wxSIMPLE_BORDER;
    long mstyle = wxNO_BORDER | wxFRAME_NO_TASKBAR | wxFRAME_SHAPED;
    
    wxDialog::Create(parent, id, _T(""), pos, size, mstyle, name);
    Init();
    CreateControls();
    //Show();
}




MUIBar::~MUIBar()
{
    if(m_canvasOptions){
        m_canvasOptions->Destroy();
        m_canvasOptions = 0;
    }
}

void MUIBar::Init()
{
    m_zinButton = NULL;
    m_zoutButton = NULL;
    m_canvasOptions = NULL;
    m_canvasOptionsAnimationTimer.SetOwner(this, CANVAS_OPTIONS_ANIMATION_TIMER_1);
    m_backcolorString = _T("GREY3");
    m_scaleTextBox = NULL;
    m_capture_size_y = 0;
    
    m_COTopOffset = 60;  //  TODO should be below GPS/Compass
    
    CanvasOptionTimer.SetOwner( this, CANVAS_OPTIONS_TIMER );
    
    //CaptureCanvasOptionsBitmap();
}

void MUIBar::CreateControls()
{
    //SetBackgroundStyle( wxBG_STYLE_TRANSPARENT );
    
    wxColour backColor = GetGlobalColor( m_backcolorString );
    SetBackgroundColour(backColor);
    wxBoxSizer *topSizer;

    wxString iconDir = g_Platform->GetSharedDataDir() + _T("uidata/MUI_flat/");
    
    if(m_orientation == wxHORIZONTAL){
        topSizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(topSizer);

        wxBoxSizer *barSizer = new wxBoxSizer(wxHORIZONTAL);
        topSizer->Add(barSizer, 0, wxEXPAND );
        
        // Buttons
        
        m_zinButton = new MUIButton( this, ID_ZOOMIN, iconDir + _T("MUI_zoom-in.svg"));
        barSizer->Add(m_zinButton, 1);
    
        m_zoutButton = new MUIButton( this, ID_ZOOMOUT, iconDir + _T("MUI_zoom-out.svg"));
        barSizer->Add(m_zoutButton, 1);
    
        barSizer->AddSpacer(2);
        
        //  Scale 
        m_scaleTextBox = new wxStaticText(this, wxID_ANY, _("1:400000"));
        m_scaleTextBox->SetForegroundColour(wxColour(200,200,200));
        barSizer->Add(m_scaleTextBox, 1, wxALIGN_CENTER_VERTICAL );
        barSizer->AddSpacer(5);
        
        
//         wxStaticLine *pl1=new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
//         barSizer->Add(pl1, 1);
        
        m_followButton = new MUIButton( this, ID_FOLLOW, iconDir + _T("MUI_follow.svg"), iconDir + _T("MUI_follow_active.svg"));
        barSizer->Add(m_followButton, 1, wxSHAPED);
        
        barSizer->AddSpacer(2);
        
        m_menuButton = new MUIButton( this, ID_MUI_MENU, iconDir + _T("MUI_menu.svg"));
        barSizer->Add(m_menuButton, 1,  wxSHAPED);
    }
    else{
        topSizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(topSizer);
        
        wxBoxSizer *barSizer = new wxBoxSizer(wxVERTICAL);
        topSizer->Add(barSizer, 0, wxEXPAND );
        
        // Buttons
        
        m_zinButton = new MUIButton( this, ID_ZOOMIN, iconDir + _T("MUI_zoom-in.svg"));
        barSizer->Add(m_zinButton, 1, wxSHAPED);
        
        m_zoutButton = new MUIButton( this, ID_ZOOMOUT, iconDir + _T("MUI_zoom-out.svg"));
        barSizer->Add(m_zoutButton, 1, wxSHAPED);
        
        barSizer->AddSpacer(5);
#if 0        
        //  Scale 
        m_scaleTextBox = new wxStaticText(this, wxID_ANY, _("1:400000"));
        m_scaleTextBox->SetForegroundColour(wxColour(200,200,200));
        barSizer->Add(m_scaleTextBox, 1, wxALIGN_CENTER_VERTICAL );
        
#endif

//         wxStaticLine *pl1=new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
//         barSizer->Add(pl1, 1);
        
        m_followButton = new MUIButton( this, ID_FOLLOW, iconDir + _T("MUI_follow.svg"), iconDir + _T("MUI_follow_active.svg"));
        barSizer->Add(m_followButton, 1, wxSHAPED);
        
        barSizer->AddSpacer(5);
        
        m_menuButton = new MUIButton( this, ID_MUI_MENU, iconDir + _T("MUI_menu.svg"));
        barSizer->Add(m_menuButton, 1,  wxALIGN_RIGHT | wxSHAPED);
        
        
    }
    
    
    topSizer->SetSizeHints( this );
    topSizer->Fit( this );
     
    
}

void MUIBar::SetBestPosition( void )
{
#if 0 // for wxWindow    
    int x = (m_parent->GetClientSize().x - GetSize().x) / 2;
    if(x > 0){
        int bottomOffset = 0;
    
        ChartCanvas *pcc = wxDynamicCast(m_parent, ChartCanvas);
        bottomOffset += pcc->GetPianoHeight();
        
        int y = m_parent->GetClientSize().y - GetSize().y - bottomOffset;
        SetSize(x, y, -1, -1, wxSIZE_USE_EXISTING);
    }
    
#else   // for wxDialog
    int x = (m_parent->GetClientSize().x - (GetSize().x * 1.02));
    
    //if(x > 0)
    {
        int bottomOffset = 2;
    
        ChartCanvas *pcc = wxDynamicCast(m_parent, ChartCanvas);
//         bottomOffset += pcc->GetPianoHeight();
    
        int y = m_parent->GetClientSize().y - GetSize().y - bottomOffset;
        
        wxPoint m_position = wxPoint(x,y);
        wxPoint screenPos = pcc->ClientToScreen( m_position );
        
        //  GTK sometimes has trouble with ClientToScreen() if executed in the context of an event handler
        //  The position of the window is calculated incorrectly if a deferred Move() has not been processed yet.
        //  So work around this here...
        
#ifdef __WXGTK__
        wxPoint pp = m_parent->GetPosition();
        wxPoint ppg = m_parent->GetParent()->GetScreenPosition();
        wxPoint screen_pos_fix = ppg + pp + m_position;
        screenPos.x = screen_pos_fix.x;
#endif        
        
        Move( screenPos );
 
        if(m_canvasOptions){
            m_canvasOptions->Destroy();
            m_canvasOptions = 0;
        }
        Show();
        
    }
#endif
}

void MUIBar::OnSize( wxSizeEvent& event )
{
    //int buttonSize = event.GetSize().y / 2;
    Layout();

#if !defined(__WXMAC__) && !defined(__OCPN__ANDROID__)       
    if(1) {
        wxBitmap m_MaskBmp = wxBitmap( GetSize().x, GetSize().y );
        wxMemoryDC sdc( m_MaskBmp );
        sdc.SetBackground( *wxWHITE_BRUSH );
        sdc.Clear();
        sdc.SetBrush( *wxBLACK_BRUSH );
        sdc.SetPen( *wxBLACK_PEN );
        sdc.DrawRoundedRectangle( 0, 0, m_MaskBmp.GetWidth(), m_MaskBmp.GetHeight(), 5 );
        sdc.SelectObject( wxNullBitmap );
        SetShape( wxRegion( m_MaskBmp, *wxWHITE, 0 ) );
    }
#endif

}

void MUIBar::UpdateDynamicValues()
{
    if(!m_scaleTextBox)
        return;
    
    wxString scaleString;
    int scale = m_parentCanvas->GetScaleValue();
    if(scale < 1e6)
        scaleString.Printf(_T("1:%d"), scale);
    else
        scaleString.Printf(_T("1:%4.1f MM"), scale / 1e6);
    
    m_scaleTextBox->SetLabel(scaleString);
}

void MUIBar::SetFollowButton( bool bFollow )
{
    m_followButton->ToggleBitmap( bFollow );
}

void MUIBar::OnToolLeftClick(  wxCommandEvent& event )
{
    //  Handle the MUIButton clicks here
    
    switch( event.GetId() ){
        
        case ID_ZOOMIN:
        case ID_ZOOMOUT:
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, event.GetId());
            GetParent()->GetEventHandler()->AddPendingEvent( evt );
            break;
        }

        case ID_FOLLOW:
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, event.GetId());
            GetParent()->GetEventHandler()->AddPendingEvent( evt );
            break;
        }
        
        case ID_MUI_MENU:
        {
            if(!m_canvasOptions){
                m_canvasOptions = new CanvasOptions(m_parent);
                
                // calculate best size for CanvasOptions dialog
                
                wxPoint parentClientUpperRight = m_parent->ClientToScreen(wxPoint( m_parent->GetSize().x, 0));
                wxRect rmui = m_parentCanvas->GetMUIBarRect();
                int size_y = rmui.y - (parentClientUpperRight.y + m_COTopOffset);
                size_y -= GetCharHeight();
                size_y = wxMax(size_y, 100);            // ensure always big enough to see
                
                m_canvasOptions->SetSize(wxSize(-1, size_y));
                m_canvasOptionsFullSize = m_canvasOptions->GetSize();
                m_canvasOptionsFullSize.x += m_canvasOptions->GetCharWidth();  // Allow for scroll bar, since sizer won't do it.
                
                if(1)
                    m_currentCOPos = m_parent->ClientToScreen(wxPoint( m_parent->GetSize().x, m_COTopOffset));
                else
                    m_currentCOPos = wxPoint( m_parent->GetSize().x, 20);
                
                m_canvasOptions->Move(m_currentCOPos);
                m_canvasOptions->Hide();
            }

           
            if(m_canvasOptions->IsShown())
                PushCanvasOptions();            // hide it
            else{
                // Grab the backing bitmap, if available
                
                if(m_capture_size_y){
                    int overShoot_x = m_canvasOptions->GetSize().x * 2 / 10;    //20%
                    m_backingPoint = wxPoint(m_capturePoint.x - overShoot_x, m_capturePoint.y);
                
                    m_backingBitmap = wxBitmap(m_canvasOptionsFullSize.x  + overShoot_x, m_capture_size_y, -1);
                    wxMemoryDC mdcb;
                    mdcb.SelectObject(m_backingBitmap);
                    wxScreenDC sdc;
                    mdcb.Blit(0, 0, m_canvasOptionsFullSize.x  + overShoot_x, m_capture_size_y, &sdc, m_capturePoint.x - overShoot_x, m_capturePoint.y, wxCOPY); 
                    mdcb.SelectObject(wxNullBitmap);
                }
                PullCanvasOptions();
            }
            break;
        }
        
        default:
            break;
    }        
}

void MUIBar::CaptureCanvasOptionsBitmap()
{
    m_coSequence = 0;
    CanvasOptionTimer.Start(100, wxTIMER_ONE_SHOT );
}
    
    

void MUIBar::CaptureCanvasOptionsBitmapChain( wxTimerEvent& event )
{
    if(m_coSequence == 0){
        
        
        if(!m_canvasOptions)
            m_canvasOptions = new CanvasOptions(m_parent);
        
        wxPoint parentClientUpperRight = m_parent->ClientToScreen(wxPoint( m_parent->GetSize().x, 0));
        wxRect rmui = m_parentCanvas->GetMUIBarRect();
        int size_y = rmui.y - (parentClientUpperRight.y + m_COTopOffset);
        size_y -= GetCharHeight();
        size_y = wxMax(size_y, 100);            // ensure always big enough to see
        m_capture_size_y = size_y;
        
        m_canvasOptions->SetSize(wxSize(-1, size_y));
        
        m_capturePoint = m_parent->ClientToScreen(wxPoint( m_parent->GetSize().x, m_COTopOffset));
        m_canvasOptions->Move(m_capturePoint);
        m_canvasOptions->Show();
        
        m_coSequence++;
        CanvasOptionTimer.Start(1, wxTIMER_ONE_SHOT );
    }
    
    else if(m_coSequence == 1){
        m_capturePoint = m_parent->ClientToScreen(wxPoint( m_parent->GetSize().x - m_canvasOptionsFullSize.x, m_COTopOffset));
        m_canvasOptions->Move(m_capturePoint);
        
        m_coSequence++;
        CanvasOptionTimer.Start(1, wxTIMER_ONE_SHOT );
    }
    
    else if(m_coSequence == 2){
        m_animateBitmap = wxBitmap(m_canvasOptions->GetSize().x, m_capture_size_y, -1);
        wxMemoryDC mdc(m_animateBitmap);
        
        wxScreenDC sdc;
        
        mdc.Blit(0, 0, m_canvasOptions->GetSize().x, m_capture_size_y, &sdc, m_capturePoint.x, m_capturePoint.y, wxCOPY); 
        mdc.SelectObject(wxNullBitmap);
        
        //delete m_canvasOptions;
        //m_canvasOptions = NULL;
    }
    
}
    
    
    

void MUIBar::OnEraseBackground( wxEraseEvent& event )
{
}


void MUIBar::OnPaint( wxPaintEvent& event )
{
    return;
    
    int width, height;
    GetClientSize( &width, &height );
    wxPaintDC dc( this );
   
//    dc.SetBackgroundMode(wxTRANSPARENT);
//    dc.SetBackground(*wxTRANSPARENT_BRUSH);
    
//     dc.SetPen( *wxTRANSPARENT_PEN );
//     dc.SetBrush( *wxTRANSPARENT_BRUSH );
//     dc.DrawRectangle( 0, 0, width, height);
    
    wxColour backColor = GetGlobalColor( m_backcolorString );
    
    
    dc.SetBrush( wxBrush( backColor/*wxColour(200, 200, 200)*/) );
    dc.SetPen( wxPen( backColor) );
    dc.DrawRoundedRectangle( 0, 0, width - 10, height - 10, 8 );
}

void MUIBar::ResetCanvasOptions()
{
    delete m_canvasOptions;
    m_canvasOptions = NULL;
}
    
void MUIBar::PullCanvasOptions()
{
    //  Target position
    int cox = m_parent->GetSize().x - m_canvasOptionsFullSize.x;
    int coy = m_COTopOffset;
        m_targetCOPos = m_parent->ClientToScreen(wxPoint(cox, coy));

    if(!g_bEffects){
        m_canvasOptions->Move(m_targetCOPos);
        m_canvasOptions->Show();
        return;
    }
    
    //  Capture the animation bitmap, if required..
    
    if(!m_animateBitmap.IsOk()){
        m_canvasOptions->Move(m_targetCOPos);
        m_canvasOptions->Show();
        CaptureCanvasOptionsBitmap();
        return;
    }
        
        
    
    //  Setup animation parameters
    
    
    //  Start Position
    m_startCOPos = m_canvasOptions->GetPosition();

    //  Present Position
    m_currentCOPos = m_startCOPos;
    
    //  Animation type
//    m_animationType = CO_ANIMATION_CUBIC_BOUNCE_IN;
//    m_animateSteps = 50; 
//    m_animationTotalTime = 1000;  // msec
    
    m_animationType = CO_ANIMATION_CUBIC_BACK_IN;
    m_animateSteps = 50; 
    m_animationTotalTime = 200;  // msec
    
    m_pushPull = CO_PULL;
    ChartCanvas *pcc = wxDynamicCast(m_parent, ChartCanvas);
    pcc->m_b_paint_enable = false;
    
    // Start the animation....
    m_animateStep = 0;
    m_canvasOptionsAnimationTimer.Start(10, true);
    //m_canvasOptions->Show();
    m_canvasOptions->Move(m_targetCOPos);
    m_canvasOptions->Hide();
    
}



void MUIBar::PushCanvasOptions()
{
    if(!g_bEffects){
        m_canvasOptions->Hide();
        return;
    }
    
    //  Setup animation parameters
    
    //  Target position
    int cox = m_parent->GetSize().x;
    int coy = 20;

    if(1)
        m_targetCOPos = m_parent->ClientToScreen(wxPoint(cox, coy));
    else
        m_targetCOPos = wxPoint(cox, coy);
    
    //  Start Position
    m_startCOPos = m_canvasOptions->GetPosition();
    
    //  Present Position
    m_currentCOPos = m_startCOPos;
    
    //  Animation type
    m_animationType = CO_ANIMATION_LINEAR;
    m_animateSteps = 20; 
    m_animationTotalTime = 100;  // msec
    m_pushPull = CO_PUSH;
    ChartCanvas *pcc = wxDynamicCast(m_parent, ChartCanvas);
    //pcc->m_b_paint_enable = false;
    
    // Start the animation....
    m_animateStep = 0;
    m_canvasOptionsAnimationTimer.Start(10, true);
    m_canvasOptions->Show();
    
}

void MUIBar::onCanvasOptionsAnimationTimerEvent( wxTimerEvent &event )
{
    double progress = m_animateStep / (double) m_animateSteps;
    double valueX = getValue(m_animationType, progress);
    
    double dx = (m_targetCOPos.x - m_startCOPos.x) * valueX;
    
    wxPoint newPos = wxPoint(m_startCOPos.x + dx, m_currentCOPos.y);

    int size_x;
    if(m_pushPull == CO_PULL)
        size_x =  abs(dx);
    else
        size_x = (m_targetCOPos.x - m_startCOPos.x) - abs(dx);
    
    if(0){
        m_canvasOptions->SetSize(newPos.x, newPos.y, size_x, wxDefaultCoord, wxSIZE_USE_EXISTING);
        m_canvasOptions->GetSizer()->Layout();
    }
    else{
        m_canvasOptions->Hide();
        wxScreenDC sdc;
        
         if(1/*m_pushPull == CO_PULL*/){
            //  restore Backing bitmap, to cover any overshoot
            if(m_backingBitmap.IsOk()){
                wxMemoryDC mdc_back(m_backingBitmap);
                sdc.Blit(m_backingPoint.x, m_backingPoint.y, m_backingBitmap.GetWidth() - size_x, m_backingBitmap.GetHeight(), &mdc_back, 0, 0, wxCOPY); 
            }
        }
        
         wxMemoryDC mdc(m_animateBitmap);
         sdc.Blit(newPos.x, newPos.y, size_x, m_animateBitmap.GetHeight(), &mdc, 0, 0, wxCOPY); 
         mdc.SelectObject(wxNullBitmap);
        
    }
    
    m_currentCOPos = newPos;
//    m_canvasOptions->Show();
    
    double dt = m_animationTotalTime / m_animateSteps;
    
    if(m_animateStep++ < m_animateSteps){
        m_canvasOptionsAnimationTimer.Start(dt, true);
    }
    else{
        //m_canvasOptions->Move(m_targetCOPos);
        m_currentCOPos = m_targetCOPos;
        m_canvasOptions->Show(m_pushPull == CO_PULL);
        
        ChartCanvas *pcc = wxDynamicCast(m_parent, ChartCanvas);
        pcc->m_b_paint_enable = true;

        if(m_pushPull == CO_PUSH){
            delete m_canvasOptions;
            m_canvasOptions = NULL;
        }
        pcc->Refresh();
        
    }
    
}

//   Animation support 

double bounceMaker(double t, double c, double a)
{
    if (t == 1.0) return c;
    if (t < (4/11.0)) {
        return c*(7.5625*t*t);
    } else if (t < (8/11.0)) {
        t -= (6/11.0);
        return -a * (1. - (7.5625*t*t + .75)) + c;
    } else if (t < (10/11.0)) {
        t -= (9/11.0);
        return -a * (1. - (7.5625*t*t + .9375)) + c;
    } else {
        t -= (21/22.0);
        return -a * (1. - (7.5625*t*t + .984375)) + c;
    }
}

double getValue(int animationType, double t)
{
    double value = 0;
    double s = 1;
    
    switch (animationType){
        case CO_ANIMATION_LINEAR:
        default:
            value = t;
            break;
        case CO_ANIMATION_CUBIC:
            value = t*t*t;
            break;
        case CO_ANIMATION_CUBIC_BOUNCE_IN:
            value = bounceMaker(t, 1, s);
            break;
    
        case CO_ANIMATION_CUBIC_BACK_IN:
            double tp = t - 1.0;
            value = tp*tp*((s+1)*tp+ s) + 1;
            break;
            
    }
    
    return value;
}
