/******************************************************************************
 * $Id: deviation_pi.cpp,v 1.0 2011/02/26 01:54:37 nohal Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Deviation Plugin
 * Author:   Pavel Kalian
 *
 ***************************************************************************
 *   Copyright (C) 2011 by Pavel Kalian   *
 *   $EMAIL$   *
 *                                                 *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                         *
 *                                                 *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                  *
 *                                                 *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                           *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/stdpaths.h>
#ifndef __OCPN__ANDROID__
    #include <GL/gl.h>
    #include <GL/glu.h>
#else
    #include "qopengl.h"   // this gives us the qt runtime gles2.h
    #include "GL/gl_private.h"
#endif
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/imaglist.h>
#include <wx/renderer.h>
#include <wx/sstream.h>
#include <math.h> 
#include <stdio.h>      /* printf */
#include <vector>

#include "deviation_pi.h"
#include "bearingdlg.h"
#include "readwritexml.h"
#include "SharedStuff.h"
#include "spa.h"
#include "printing.h"
#include "icons.h"

using namespace std;

deviation_pi* Dev_PI;
extern BearingDlg* B_Dlg;
DevTableDialog* DT_Dlg;
CompasDev1Dialog* m_CompasDevListDlg;
// Global print data, to remember settings during the session
extern wxPrintData *g_printData;
extern wxPageSetupDialogData* g_pageSetupData;


//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

void DeviationLogMessage1(wxString s) { wxLogMessage(_T("Deviation_pi: ") + s); }
extern "C" void DeviationLogMessage(const char *s) { DeviationLogMessage1(wxString::FromAscii(s)); }



// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new deviation_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    Deviation PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//        PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

deviation_pi::deviation_pi(void *ppimgr)
    : opencpn_plugin_116(ppimgr)
//    : opencpn_plugin_114(ppimgr)
{
    // Create the PlugIn icons
    initialize_images();
//    Init();
}

int deviation_pi::Init(void)
{
    Dev_PI = this;
    B_Dlg = NULL; 
    m_CompasDevListDlg = NULL;
    //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
    m_parent_window = GetOCPNCanvasWindow();
    
//     //    Get a pointer to the opencpn configuration object
     m_pconfig = GetOCPNConfigObject();
// 
//     //    And load the configuration items
     LoadConfig();

     m_LastValue = wxEmptyString;
     mPriDateTime = 99;
     mPriHeadingM = 99;
 
     pFontSmall = new wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );

    int ret_flag =  (
    WANTS_CURSOR_LATLON     |
    WANTS_TOOLBAR_CALLBACK  |
    WANTS_NMEA_SENTENCES    |
    WANTS_NMEA_EVENTS       |
    WANTS_PREFERENCES       |
    WANTS_CONFIG          |
    WANTS_PLUGIN_MESSAGING 
    );
    b_ShowIcon = true;
    if( aCompass->data->ShowToolbarBtn ){
    //    This PlugIn needs a toolbar icon, so request its insertion
        i_leftclick_dev_tool_id  = InsertPlugInTool(_T(""), _img_deviation,
            _img_deviation, wxITEM_NORMAL,
            _("Deviation"), _T(""), NULL, Deviation_TOOL_POSITION, 0, this);
        
        SetIconType();          // SVGs allowed if not showing live icon
        ret_flag |= INSTALLS_TOOLBAR_TOOL;
    }

    return ret_flag;
}

bool deviation_pi::DeInit(void)
{
    SaveConfig();
    if( m_CompasDevListDlg != NULL) delete m_CompasDevListDlg;
    RemovePlugInTool(i_leftclick_dev_tool_id);
    delete pFontSmall;
    return true;
}

int deviation_pi::GetAPIVersionMajor() { return  OCPN_API_VERSION_MAJOR; }
int deviation_pi::GetAPIVersionMinor() { return OCPN_API_VERSION_MINOR; }
int deviation_pi::GetPlugInVersionMajor() { return PLUGIN_VERSION_MAJOR; }
int deviation_pi::GetPlugInVersionMinor() { return PLUGIN_VERSION_MINOR; }
wxBitmap *deviation_pi::GetPlugInBitmap() {  return _img_deviation_pi; }
wxString deviation_pi::GetCommonName() { return _T(PLUGIN_COMMON_NAME); }
wxString deviation_pi::GetShortDescription() { return _T( PLUGIN_SHORT_DESCRIPTION); }
wxString deviation_pi::GetLongDescription() { return _T(PLUGIN_LONG_DESCRIPTION); }

void deviation_pi::SetCursorLatLon(double lat, double lon)
{

}

int deviation_pi::GetToolbarToolCount(void)
{
    return 1;
}

void deviation_pi::SetColorScheme(PI_ColorScheme cs)
{
    if (NULL == m_CompasDevListDlg)
        return;
    DimeWindow(m_CompasDevListDlg);
}

void deviation_pi::SetIconType()
{
    if(i_ShowLiveIcon){
        SetToolbarToolBitmaps(i_leftclick_dev_tool_id, _img_deviation, _img_deviation);
        SetToolbarToolBitmapsSVG(i_leftclick_dev_tool_id, _T(""), _T(""), _T(""));
        m_LastValue.Empty();
    }
    else{
        wxString normalDevIcon = m_shareLocation + _T("deviation_pi.svg");
        wxString toggledDevIcon = m_shareLocation + _T("deviation_pi.svg");
        wxString rolloverDevIcon = m_shareLocation + _T("deviation_pi.svg");
        
        SetToolbarToolBitmapsSVG(i_leftclick_dev_tool_id, normalDevIcon, rolloverDevIcon, toggledDevIcon);
    }    
}


void deviation_pi::RearrangeWindow()
{
}

void deviation_pi::OnToolbarToolCallback(int id)
{
    if(NULL == m_CompasDevListDlg)
    {
        m_CompasDevListDlg = new CompasDev1Dialog(m_parent_window, 
            aCompass->data->shipsname + _T("  ") + aCompass->data->compassname,
            aCompass->data );
        m_CompasDevListDlg->Show();
    }
    else if ( m_CompasDevListDlg->IsShown() )
    {
        m_CompasDevListDlg->Show(false);
        m_CompasDevListDlg->Destroy();
        m_CompasDevListDlg = NULL;
    }     
}


void deviation_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{
    g_var = pfix.Var;
    if ( B_Dlg != NULL )
        B_Dlg->SetPositionFix(pfix);
    if (i_ShowLiveIcon)
        i_ShowLiveIcon --;
}

//Demo implementation of response mechanism
void deviation_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
    wxJSONReader r;
    wxJSONValue v;
    r.Parse(message_body, &v);
    if(message_id == _T("TRUEHEADING_REQUEST"))
    {
        
        double CompasCourse = v[_T("HdgC")].AsDouble();
        SendTrueCourse(CompasCourse);
    }
    else if(message_id == _T("CURRENT_DEVIATION_REQUEST"))
    {
        SendDeviation();
    }
    else if(message_id == _T("DEVIATION_AT_CC_REQUEST"))
    {
        SendDeviationAt(0.0);
    }
    else if(message_id == _T("WMM_VARIATION"))
    {
        //Forward to BearingDlg
        if( B_Dlg != NULL )
            B_Dlg->SetMessageVariation( message_id, message_body );
    }
    else if(message_id == _T("OCPN_ACTIVE_ROUTELEG_RESPONSE"))
    {
        //Forward to BearingDlg
        if( B_Dlg != NULL )
            B_Dlg->SetMessageRouteActiveLeg( message_id, message_body );


    }
    
}

void deviation_pi::DoRouteLegRequest()
{
    wxJSONValue v;
    v[_T("Test")] = _("test");
        RequestPluginMessage( _T("OCPN_ACTIVE_ROUTELEG_REQUEST"), v);  
}

void deviation_pi::SendTrueCourse(double CompasCourse)
{
    wxJSONValue v;
    double x = aCompass->data->getDeviation(CompasCourse);
    v[_T("Decl")] = aCompass->data->getDeviation(CompasCourse);
    //v[_T("Decldot")] = x - abs(x);    
//     v[_T("Decl")] = m_boatVariation.Decl;
//     v[_T("Decldot")] = m_boatVariation.Decldot;
//     v[_T("F")] = m_boatVariation.F;
//     v[_T("Fdot")] = m_boatVariation.Fdot;
//     v[_T("GV")] = m_boatVariation.GV;
//     v[_T("GVdot")] = m_boatVariation.GVdot;
//     v[_T("H")] = m_boatVariation.H;
//     v[_T("Hdot")] = m_boatVariation.Hdot;
//     v[_T("Incl")] = m_boatVariation.Incl;
//     v[_T("Incldot")] = m_boatVariation.Incldot;
//     v[_T("X")] = m_boatVariation.X;
//     v[_T("Xdot")] = m_boatVariation.Xdot;
//     v[_T("Y")] = m_boatVariation.Y;
//     v[_T("Ydot")] = m_boatVariation.Ydot;
//     v[_T("Z")] = m_boatVariation.Z;
//     v[_T("Zdot")] = m_boatVariation.Zdot;
    wxJSONWriter w;
    wxString out;
    w.Write(v, out);
    SendPluginMessage(wxString(_T("WMM_VARIATION_BOAT")), out);

}
void deviation_pi::RequestPluginMessage(wxString MessageID, wxJSONValue message)
{
    wxJSONWriter w;
    wxString out;
    w.Write(message, out);
    SendPluginMessage(wxString(MessageID), out);    
}
void deviation_pi::SendDeviation()
{}
void deviation_pi::SendDeviationAt(double CompasCourse)
{}

void deviation_pi::SetNMEASentence(wxString &sentence)
{
m_NMEA0183 << sentence;
    if( m_NMEA0183.PreParse() ) 
    {
        if( m_NMEA0183.LastSentenceIDReceived == _T("GGA") ) {
            if( m_NMEA0183.Parse() ) {
                if( m_NMEA0183.Gga.GPSQuality > 0 ) {
                    if( mPriDateTime >= 4 ) {
                        mPriDateTime = 4;                        
                        if ( B_Dlg != NULL ){
                            mUTCDateTime.ParseFormat( m_NMEA0183.Gga.UTCTime.c_str(), _T("%H%M%S") );
                            B_Dlg->SetNMEATimeFix( mUTCDateTime );
                        }
                    }
                }
            }
        }
        else if( m_NMEA0183.LastSentenceIDReceived == _T("GLL") ) {
            if( m_NMEA0183.Parse() ) {
                if( m_NMEA0183.Gll.IsDataValid == NTrue ) {
                    if( mPriDateTime >= 5 ) {
                        mPriDateTime = 5;
                        if ( B_Dlg != NULL ){
                            mUTCDateTime.ParseFormat( m_NMEA0183.Gll.UTCTime.c_str(), _T("%H%M%S") );
                            B_Dlg->SetNMEATimeFix( mUTCDateTime );
                        }
                    }
                }
            }
        }
        else if( m_NMEA0183.LastSentenceIDReceived == _T("HDG") ) {
            if( m_NMEA0183.Parse() ) {
                
                if( mPriHeadingM >= 1 ) {
                    mPriHeadingM = 1;
                    mHdm = m_NMEA0183.Hdg.MagneticSensorHeadingDegrees;
                    i_ShowLiveIcon = 3;
                    DrawToolbarBtnNumber(mHdm);
                    double mHdt = mHdm + g_var + aCompass->data->getDeviation( mHdm );
                    if( sentence.Left(6) != _T("$XXHDG")){
                        SendNMEASentence( _T("$XXHDT,") + wxString::Format( _T("%1.1f"), mHdt));                        
                        //SendNMEASentence( _T("$XXHDG,") + wxString::Format( _T("%1.1f,%1.1f,E,%1.1f,E"), mHdm, g_var, aCompass->data->getDeviation( mHdm )));
                        if ( B_Dlg != NULL ){
                            B_Dlg->SetNMEAHeading(mHdm);
                        }
                    }                    
                }
            }
        }
    }    
}

bool deviation_pi::LoadConfig(void)
{
    m_shareLocation =  *GetpPrivateApplicationDataLocation();
    // above function is not always give a path seperator on the end. So we have to check for this.
    if ( m_shareLocation.Right(1) != wxFileName::GetPathSeparator())
        m_shareLocation = m_shareLocation + wxFileName::GetPathSeparator();
     m_shareLocation = m_shareLocation + wxT("plugins") + wxFileName::GetPathSeparator() + 
            wxT("deviation_pi");
    // Check if directory exsist, and if not make it
    if ( !wxFileName::DirExists(m_shareLocation) )
        wxFileName::Mkdir( m_shareLocation, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    
    filename = m_shareLocation + wxFileName::GetPathSeparator() + _T("deviation_data.xml");

    
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;
    if(pConf)
    {       
         pConf->SetPath ( _T( "/Settings/Deviation" ) );
         pConf->Read ( _T("ShipsName"),  &ShipsName, wxEmptyString );
         pConf->Read ( _T("CompassName"),  &CompassName, wxEmptyString );

        aCompass = new Compass(filename, ShipsName, CompassName);

        return true;
    }
        return false;
}


bool deviation_pi::SaveConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if(pConf)
    {
        pConf->SetPath ( _T( "/Settings/Deviation" ) );
        pConf->Write ( _T("ShipsName"), aCompass->data-> shipsname );
        pConf->Write ( _T("CompassName"), aCompass->data->compassname );

        return true;
    }
    else
        return false;
}

void deviation_pi::ShowPreferencesDialog( wxWindow* parent )
{
    PreferenceDlg *PrefDlg = new PreferenceDlg(parent, wxID_ANY, aCompass->data);
    if(PrefDlg->ShowModal() == wxID_OK)
    {
        SaveConfig();
    }
    delete PrefDlg;
}

void deviation_pi::DrawToolbarBtnNumber( float dev )
{
    wxString NewVal = wxString::Format(_T("%.1f"), aCompass->data->getDeviation( dev ));
    double scale = GetOCPNGUIToolScaleFactor_PlugIn();
    scale = wxRound(scale * 4.0) / 4.0;
    scale = wxMax(1.0, scale);          // Let the upstream processing handle minification.
    
    if( b_ShowIcon && i_ShowLiveIcon && ((m_LastValue != NewVal) || (scale != m_scale)) )
    {
        m_scale = scale;
        m_LastValue = NewVal;
        int w = _img_deviation_live->GetWidth() * scale;
        int h = _img_deviation_live->GetHeight() * scale;
        wxMemoryDC dc;
        wxBitmap icon;
        
        //  Is SVG available?
        wxBitmap live = GetBitmapFromSVGFile(m_shareLocation + _T("deviation_live.svg"), w, h);
        if( ! live.IsOk() ){
            icon = wxBitmap(_img_deviation_live->GetWidth(), _img_deviation_live->GetHeight());
            dc.SelectObject(icon);
            dc.DrawBitmap(*_img_deviation_live, 0, 0, true);
        }
        else{
            icon = wxBitmap(w, h);
            dc.SelectObject(icon);
            wxColour col;
            dc.SetBackground( *wxTRANSPARENT_BRUSH );
            dc.Clear();
        
            dc.DrawBitmap(live, 0, 0, true);
        }
        wxColour cf;
        GetGlobalColor(_T("CHBLK"), &cf);
        dc.SetTextForeground(cf);
        if(pFontSmall->IsOk()){
            if(live.IsOk()){
                int point_size = wxMax(10, (int)10 * scale);
                pFontSmall->SetPointSize(point_size);              
                //  Validate and adjust the font size...
                //   No smaller than 8 pt.
                int w;
                wxScreenDC sdc;
                sdc.SetFont(*pFontSmall);
                sdc.GetTextExtent(NewVal, &w, NULL);
                while( (w > (icon.GetWidth() * 8 / 10) ) && (point_size >= 8) ){
                    point_size--;
                    pFontSmall->SetPointSize(point_size);
                    sdc.SetFont(*pFontSmall);
                    sdc.GetTextExtent(NewVal, &w, NULL);
                }
            }
            dc.SetFont(*pFontSmall);           
        }
        wxSize s = dc.GetTextExtent(NewVal);
        dc.DrawText(NewVal, (icon.GetWidth() - s.GetWidth()) / 2, (icon.GetHeight() - s.GetHeight()) / 2);
        dc.SelectObject(wxNullBitmap);
        if(live.IsOk()){
            //  By using a DC to modify the bitmap, we have lost the original bitmap's alpha channel
            //  Recover it by copying from the original to the target, bit by bit
            wxImage imo = live.ConvertToImage();
            wxImage im = icon.ConvertToImage();
            
            if(!imo.HasAlpha())
                imo.InitAlpha();
            if(!im.HasAlpha())
                im.InitAlpha();
            
            unsigned char *alive = imo.GetAlpha();
            unsigned char *target = im.GetAlpha();
                
            for(int i=0 ; i < h ; i ++){
                for(int j=0 ; j < w ; j++){
                    int index = (i * w) + j;
                    target[index] = alive[index];
                }
            }
            icon = wxBitmap(im);
        }        
        SetToolbarToolBitmaps(i_leftclick_dev_tool_id, &icon, &icon);
    }
}

void deviation_pi::SendNMEASentence(wxString sentence)
{
    wxString Checksum = ComputeChecksum(sentence);
    sentence = sentence.Append(wxT("*"));
    sentence = sentence.Append(Checksum);
    sentence = sentence.Append(wxT("\r\n"));
    PushNMEABuffer(sentence);
}

wxString deviation_pi::ComputeChecksum( wxString sentence )
{
    unsigned char calculated_checksum = 0;
    for(wxString::const_iterator i = sentence.begin()+1; i != sentence.end() && *i != '*'; ++i)
        calculated_checksum ^= static_cast<unsigned char> (*i);

   return( wxString::Format(_T("%02X"), calculated_checksum) );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//(*IdInit(CompasDev1Dialog)
const long CompasDev1Dialog::ID_LISTCTRLDEV = wxNewId();
const long CompasDev1Dialog::ID_ADDBUTTON = wxNewId();
const long CompasDev1Dialog::ID_DELBUTTON = wxNewId();
const long CompasDev1Dialog::ID_EDITBUTTON = wxNewId();
const long CompasDev1Dialog::ID_BUTTONSHOW = wxNewId();
const long CompasDev1Dialog::ID_BUTTONCANCEL = wxNewId();
const long CompasDev1Dialog::ID_BUTTONCLOSE = wxNewId();
//*)

BEGIN_EVENT_TABLE(CompasDev1Dialog,wxFrame)
    //(*EventTable(CompasDev1Dialog)
    EVT_CLOSE(CompasDev1Dialog::OnCloseWindow)
    //*)
END_EVENT_TABLE()

CompasDev1Dialog::CompasDev1Dialog( wxWindow *parent,
            const wxString &title,
            compass_data* Data )
{
    data = Data;
    DT_Dlg = NULL;
//     if ( B_Dlg != NULL)
//     {
//         delete B_Dlg;
//         B_Dlg = NULL;
//     }
    //(*Initialize(CompasDev1Dialog)
    wxFlexGridSizer* FlexGridSizer;
    wxBoxSizer* BoxSizerButons;

    Create(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxSTAY_ON_TOP|wxCAPTION|wxFRAME_TOOL_WINDOW, _T("id"));
    
    FlexGridSizer = new wxFlexGridSizer(0, 1, 0, 0);
    DevListCtrl = new wxListCtrl(this, ID_LISTCTRLDEV, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRLDEV"));
    //DevListCtrl->SetMinSize(wxSize(0,150));
    FlexGridSizer->Add(DevListCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizerButons = new wxBoxSizer(wxHORIZONTAL);
    AddBtn = new wxButton(this, ID_ADDBUTTON, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_ADDBUTTON"));
    BoxSizerButons->Add(AddBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DeleteBtn = new wxButton(this, ID_DELBUTTON, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DELBUTTON"));
    DeleteBtn->Disable();
    BoxSizerButons->Add(DeleteBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    EditBtn = new wxButton(this, ID_EDITBUTTON, _("Edit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_EDITBUTTON"));
    EditBtn->Disable();
    BoxSizerButons->Add(EditBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ShowBtn = new wxButton(this, ID_BUTTONSHOW, _("Show"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONSHOW"));    
    BoxSizerButons->Add(ShowBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    CancelBtn = new wxButton(this, ID_BUTTONCANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONCANCEL"));
    BoxSizerButons->Add(CancelBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    CloseBtn = new wxButton(this, ID_BUTTONCLOSE, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONCLOSE"));
    BoxSizerButons->Add(CloseBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer->Add(BoxSizerButons, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer);
    FlexGridSizer->Fit(this);
    FlexGridSizer->SetSizeHints(this);
    
    DevListCtrl->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(CompasDev1Dialog::OnConnectionToggleEnableMouse), NULL, this);
    DevListCtrl->Connect(ID_LISTCTRLDEV, wxEVT_RIGHT_UP, wxMouseEventHandler(CompasDev1Dialog::OnRightDown), NULL ,this );
    Connect(ID_LISTCTRLDEV,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&CompasDev1Dialog::OnDevListCtrlItemSelect);
    Connect(ID_LISTCTRLDEV,wxEVT_COMMAND_LIST_ITEM_DESELECTED,(wxObjectEventFunction)&CompasDev1Dialog::OnDevListCtrlItemDeselect);
    Connect(ID_LISTCTRLDEV,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,(wxObjectEventFunction)&CompasDev1Dialog::OnDevListCtrlItemActivated);
    Connect(ID_ADDBUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnAddBtnClick);
    Connect(ID_DELBUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnDeleteBtnClick);
    Connect(ID_EDITBUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnEditBtnClick);
    Connect(ID_BUTTONSHOW,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnShowBtnClick);
    Connect(ID_BUTTONCANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnCancelBtnClick);
    Connect(ID_BUTTONCLOSE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnOKBtnClick);
    Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&CompasDev1Dialog::OnRightDown);
    Connect(wxEVT_SIZE,(wxObjectEventFunction)&CompasDev1Dialog::OnResize);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&CompasDev1Dialog::OnClose);
    
     wxString columns[] = {_("Do\nuse"),   _("Date/Time"), _("Compass\nCourse"),   _("Compass\nBearing"), _("True\nBearing"), _("Var."), _("Dev."), _T("")};
    for (int i = 0; i < 8; ++i) {
        wxListItem col;
        col.SetId(i);
        col.SetText(columns[i]);
        DevListCtrl->InsertColumn(i, col);
    }

    //  Build the image list
    wxImageList* imglist = new wxImageList(16, 16, TRUE, 1);
    wxBitmap unchecked_bmp(16, 16), checked_bmp(16, 16);
    wxMemoryDC renderer_dc;

    // Unchecked
    renderer_dc.SelectObject(unchecked_bmp);
    renderer_dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(
        GetBackgroundColour(), wxBRUSHSTYLE_SOLID));
    renderer_dc.Clear();
    wxRendererNative::Get().DrawCheckBox(this, renderer_dc, wxRect(0, 0, 16, 16), 0);

    // Checked
    renderer_dc.SelectObject(checked_bmp);
    renderer_dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(
        GetBackgroundColour(), wxBRUSHSTYLE_SOLID));
    renderer_dc.Clear();
    wxRendererNative::Get().DrawCheckBox(this, renderer_dc, wxRect(0, 0, 16, 16), wxCONTROL_CHECKED);

    // Deselect the renderer Object
    renderer_dc.SelectObject(wxNullBitmap);

    imglist->Add(unchecked_bmp);
    imglist->Add(checked_bmp);
    DevListCtrl->AssignImageList(imglist, wxIMAGE_LIST_SMALL);

    DevListCtrl->Refresh();
    
    FillSourceList();
}

CompasDev1Dialog::~CompasDev1Dialog()
{
    
}

void CompasDev1Dialog::OnClose(wxCloseEvent &event)
{
    Show(false);
    if ( DT_Dlg != NULL ){
        DT_Dlg->Show(false);
        DT_Dlg->Destroy();
        DT_Dlg = NULL;        
    }
    m_CompasDevListDlg = NULL;
    Destroy();
}

void CompasDev1Dialog::OnAddBtnClick(wxCommandEvent& event)
{    
    Meassurement* NewMess = new Meassurement();
    if ( B_Dlg == NULL ){
        B_Dlg =  new BearingDlg(this, NewMess, wxID_ANY, wxDefaultPosition, wxDefaultSize, false);
    }
    if ( B_Dlg->ShowModal() == wxID_OK )
    {        
        data->needsaving = true;
        data->vec.push_back( NewMess );
        CalcSqueredDev(data);
        FillSourceList();
        UpdateWindows();
        delete B_Dlg; 
        B_Dlg = NULL;
    }
    else if ( B_Dlg != NULL )
    {
        B_Dlg->Destroy(); 
        B_Dlg = NULL;
    }
}

void CompasDev1Dialog::OnDeleteBtnClick(wxCommandEvent& event)
{
    delete data->vec[listselindex]; //delete object
    data->vec.erase( data->vec.begin() + listselindex ); // and remove pointer to object from vector list
    CalcSqueredDev(data);
    FillSourceList();
    UpdateWindows();
}

void CompasDev1Dialog::OnEditBtnClick(wxCommandEvent& event)
{
    if ( B_Dlg == NULL )
        B_Dlg =  new BearingDlg(this, data->vec[listselindex], wxID_ANY,  wxDefaultPosition, wxDefaultSize, false);
    //CopyMessObjToDlg(data->vec[listselindex], B_Dlg);
    if ( B_Dlg->ShowModal() == wxID_OK )
    {        
        data->needsaving = true;
        CalcSqueredDev(data);
        FillSourceList();
        UpdateWindows();
    } 
    else if ( B_Dlg != NULL )
    {
        delete B_Dlg; 
        B_Dlg = NULL;
    }   
}

void CompasDev1Dialog::OnShowBtnClick(wxCommandEvent& event)
{
    if ( DT_Dlg == NULL ){
        DT_Dlg = new DevTableDialog(this, wxNewId(), data ) ;
        DT_Dlg->Show(true);
    }
    else
    {
        DT_Dlg->Close();             
    }

}

void CompasDev1Dialog::OnCancelBtnClick(wxCommandEvent& event)
{
    if ( DT_Dlg != NULL ){
        DT_Dlg->Show(false);
        DT_Dlg->Destroy();
        DT_Dlg = NULL;        
    }
    Close();
}

void CompasDev1Dialog::OnOKBtnClick(wxCommandEvent& event)
{
    data->needsaving = true;
    WriteObjectsToXML(data);
    
    if ( DT_Dlg != NULL ){
        DT_Dlg->Close();
        /*delete DT_Dlg;
        DT_Dlg = NULL;    */    
    }
    Close();
}

void CompasDev1Dialog::OnDevListCtrlItemSelect(wxListEvent& event)
{
    EditBtn->Enable();
    DeleteBtn->Enable();
}
void CompasDev1Dialog::OnDevListCtrlItemDeselect(wxListEvent& event)
{
    EditBtn->Disable();
    DeleteBtn->Disable();
}

void CompasDev1Dialog::OnDevListCtrlItemActivated(wxListEvent& event)
{
}

void CompasDev1Dialog::OnConnectionToggleEnableMouse(wxMouseEvent& event) {
  int flags;
  long index = DevListCtrl->HitTest(event.GetPosition(), flags);
  if (index == wxNOT_FOUND || event.GetX() < DevListCtrl->GetColumnWidth(0))
    EnableItem(index);
  listselindex = index;
  // Allow wx to process...
  event.Skip();
}

void CompasDev1Dialog::OnRightDown(wxMouseEvent& event)
{
    PreferenceDlg* pref = new PreferenceDlg(NULL, wxID_ANY, data );
    if ( pref->ShowModal() )
    {
        data->shipsname = pref->ShipsnameBox->GetValue();
        data->compassname = pref->CompassnameBox->GetValue();
        ReadObjectsFromXML(data);
        FillSourceList();
        CalcSqueredDev(data);
    }
    
}
void CompasDev1Dialog::OnResize(wxSizeEvent& event)
{
    //Layout();
    //FlexGridSizer->SetSizeHints(this);
}


void CompasDev1Dialog::EnableItem(const long index) {
    if (index != wxNOT_FOUND) {
        Meassurement* Mess = data->vec[index];
        if (!Mess) return;
        Mess->enabled = !Mess->enabled;

        DevListCtrl->SetItemImage(index, Mess->enabled);
        UpdateWindows();
    }
}


void CompasDev1Dialog::FillSourceList(void) 
{
    DevListCtrl->DeleteAllItems();

    for (size_t i = 0; i < data->vec.size(); i++) {
        wxListItem li;
        li.SetId(i);
        li.SetImage( data->vec[i]->enabled);
        li.SetData(i);
        li.SetText(wxEmptyString);
        long itemIndex = DevListCtrl->InsertItem(li);

        DevListCtrl->SetItem(itemIndex, 1,  data->vec[i]->datetime.Format(_T("%x %H:%M") ));//, wxDateTime::UTC).c_str());
        wxString tmp =  wxString::Format(_T( "%05.1f" ), data->vec[i]->compasscourse );
        DevListCtrl->SetItem(itemIndex, 2, tmp);
        tmp =  wxString::Format(_T( "%05.1f" ), data->vec[i]->compassbearing );
        DevListCtrl->SetItem(itemIndex, 3, tmp);
        tmp =  wxString::Format(_T( "%05.1f" ), data->vec[i]->truebearing );
        DevListCtrl->SetItem(itemIndex, 4, tmp);
        tmp =  wxString::Format(_T( "%0.1f" ), data->vec[i]->variation );
        DevListCtrl->SetItem(itemIndex, 5, tmp);
        tmp =  wxString::Format(_T( "%0.1f" ), data->vec[i]->deviation );
        DevListCtrl->SetItem(itemIndex, 6, tmp);

        DevListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
        DevListCtrl->SetColumnWidth(1, wxLIST_AUTOSIZE);
        DevListCtrl->SetColumnWidth(2, wxLIST_AUTOSIZE);
        DevListCtrl->SetColumnWidth(3, wxLIST_AUTOSIZE);
        DevListCtrl->SetColumnWidth(4, wxLIST_AUTOSIZE);
        DevListCtrl->SetColumnWidth(5, wxLIST_AUTOSIZE);
        DevListCtrl->SetColumnWidth(6, wxLIST_AUTOSIZE);
    }
}

void CompasDev1Dialog::UpdateWindows(void)
{
    CalcSqueredDev(data);
    if ( DT_Dlg != NULL )
        if ( DT_Dlg->IsShown() ){
            DT_Dlg->Refresh();
        }
        
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////


//(*IdInit(DevTableDialog)
const long DevTableDialog::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)

EVT_PAINT(BasicDrawPane::paintEvent)
 
END_EVENT_TABLE()

BasicDrawPane::BasicDrawPane(DevTableDialog* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size,
                long style, const wxString& name, compass_data* Data) : wxPanel(parent)
{
    parentdlg = parent;
    
    data = Data;
}
BasicDrawPane::~BasicDrawPane()
{
}
  
void BasicDrawPane::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    DoRender(dc, data);
}
 
void BasicDrawPane::paintNow()
{
    wxClientDC dc(this);
    DoRender(dc, data);
}
void BasicDrawPane::DrawTextCenterCenter(wxDC&  dc, wxString text, wxPoint pos)
{
//     int width, height;
//     dc.GetTextExtent(text, &width, &height);
//     dc.DrawText(text, pos.x- (int) width/2, pos.y - (int) height/2 );
}


BEGIN_EVENT_TABLE(DevTableDialog,wxFrame)
    //(*EventTable(DevTableDialog)
    EVT_CLOSE(DevTableDialog::OnCloseWindow)
    //*)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////////////////////////////////

const long DevTableDialog::ID_PRINT = wxNewId();
const long DevTableDialog::ID_PRINTPREV = wxNewId();
const long DevTableDialog::ID_CLOSE = wxNewId();

DevTableDialog::DevTableDialog(wxWindow* parent,wxWindowID id, compass_data* Data)
{
    data = Data;
    parentdlg = (CompasDev1Dialog*)parent;
    //(*Initialize(DevTableDialog)
    wxFlexGridSizer* FlexGridSizer1;
    
    wxBoxSizer* BoxSizerButons;
    Create(parent, wxID_ANY, _("Deviation Table"), wxDefaultPosition, wxSize(848, 600), wxSTAY_ON_TOP|wxCAPTION|wxFRAME_TOOL_WINDOW, _T("wxID_ANY"));
  
    FlexGridSizer1 = new wxFlexGridSizer(2, 1, 0, 0);
    Panel1 = new BasicDrawPane(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"), data);
    
    Panel1->SetMinSize(wxSize(848, 600));
    FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);

    BoxSizerButons = new wxBoxSizer(wxHORIZONTAL);
    CloseBtn = new wxButton(this, ID_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CLOSE"));
    PrintBtn = new wxButton(this, ID_PRINT, _("Print"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_PRINT"));
    PrintPrevBtn = new wxButton(this, ID_PRINTPREV, _("Print Preview"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_PRINTPREV"));
    BoxSizerButons->Add(PrintPrevBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizerButons->Add(PrintBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);    
    BoxSizerButons->Add(CloseBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    FlexGridSizer1->Add(BoxSizerButons, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);
    Connect(ID_PRINTPREV,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DevTableDialog::OnPrintPreviewBtnClick);
    Connect(ID_PRINT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DevTableDialog::OnPrintBtnClick);
    Connect(ID_CLOSE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DevTableDialog::OnCloseBtnClick); 
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&DevTableDialog::OnClose);
}

DevTableDialog::~DevTableDialog()
{
    //(*Destroy(DevTableDialog)
    //*)
}

void DevTableDialog::OnPrintBtnClick(wxCommandEvent& event)
{
    PrintOnInit();
    
     wxPrintDialogData printDialogData(* g_printData);

    wxPrinter printer(&printDialogData);
    MyPrintout printout(data);//this, _("My printout"));
    if (!printer.Print(this, &printout, true /*prompt*/))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
        {
            wxLogError(_("There was a problem printing. Perhaps your current printer is not set correctly?"));
        }
        else
        {
            wxLogMessage(_("You canceled printing"));
        }
    }
    else
    {
        (*g_printData) = printer.GetPrintDialogData().GetPrintData();
    }    
}
void DevTableDialog::OnCloseBtnClick(wxCommandEvent& event)
{
    Close();    
}

void DevTableDialog::OnPrintPreviewBtnClick(wxCommandEvent& event)
{
    PrintOnInit();
        // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview = new wxPrintPreview(new MyPrintout(data), new MyPrintout(data), &printDialogData);
    wxPreviewFrame *frame =
        new wxPreviewFrame(preview, this, _("Compass Deviation Table Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
}
void DevTableDialog::OnClose(wxCloseEvent &event)
{
    parentdlg->DT_Dlg = NULL;
    Destroy();
}


