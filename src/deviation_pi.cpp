#include "bearingdlg.h"
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
#include "qopengl.h"                  // this gives us the qt runtime gles2.h
#include "GL/gl_private.h"
#endif
#include <wx/filename.h>
#include "bearingdlg.h"
#include "SharedStuff.h"

extern BearingDlg* B_Dlg;
//wxFont *pFont;

void DeviationLogMessage1(wxString s) { wxLogMessage(_T("Deviation_pi: ") + s); }
extern "C" void DeviationLogMessage(const char *s) { DeviationLogMessage1(wxString::FromAscii(s)); }

#include "deviation_pi.h"

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
    : opencpn_plugin_18(ppimgr)
{
    // Create the PlugIn icons
    initialize_images();
//    Init();
}

int deviation_pi::Init(void)
{
    B_Dlg = NULL; 
    m_CompasDevListDlg = NULL;
    //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
    m_parent_window = GetOCPNCanvasWindow();
    
//     //    Get a pointer to the opencpn configuration object
     m_pconfig = GetOCPNConfigObject();
// 
//     //    And load the configuration items
     LoadConfig();

     m_LastVal = wxEmptyString;
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
        i_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_deviation, _img_deviation, wxITEM_NORMAL,
                                            _("Deviation"), _T(""), NULL, Deviation_TOOL_POSITION, 0, this);
        
        SetIconType();          // SVGs allowed if not showing live icon
        ret_flag |= INSTALLS_TOOLBAR_TOOL;
    }
// 
     m_CompasDevListDlg = NULL;

    return ret_flag;
}

bool deviation_pi::DeInit(void)
{
    SaveConfig();
    if( m_CompasDevListDlg != NULL) delete m_CompasDevListDlg;
    RemovePlugInTool(i_leftclick_tool_id);
    delete pFontSmall;
    return true;
}

int deviation_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int deviation_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int deviation_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int deviation_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *deviation_pi::GetPlugInBitmap()
{
    return _img_deviation_pi;
}

wxString deviation_pi::GetCommonName()
{
    return _("Deviation");
}


wxString deviation_pi::GetShortDescription()
{
    return _("Compass deviation PlugIn for OpenCPN");
}

wxString deviation_pi::GetLongDescription()
{
    return _("Compass deviation PlugIn for OpenCPN\n\
    With this plugin you can make a deviation table,\n\
    and use it to correct incoming nmea compass course\n\
    sentences to true course sentences. To get this working\n\
    you will also need the wmm plugin activated.");
}

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
    if(b_ShowLiveIcon){
        SetToolbarToolBitmaps(i_leftclick_tool_id, _img_deviation, _img_deviation);
        SetToolbarToolBitmapsSVG(i_leftclick_tool_id, _T(""), _T(""), _T(""));
        m_LastVal.Empty();
    }
    else{
        wxString normalIcon = m_shareLocn + _T("deviation_pi.svg");
        wxString toggledIcon = m_shareLocn + _T("deviation_pi.svg");
        wxString rolloverIcon = m_shareLocn + _T("deviation_pi.svg");
        
        SetToolbarToolBitmapsSVG(i_leftclick_tool_id, normalIcon, rolloverIcon, toggledIcon);
    }    
}


void deviation_pi::RearrangeWindow()
{
}

void deviation_pi::OnToolbarToolCallback(int id)
{
//     if( !m_buseable )
//         return;
    if(NULL == m_CompasDevListDlg)
    {
        m_CompasDevListDlg = new CompasDev1Dialog(m_parent_window, 
            aCompass->data->shipsname + _("  ") + aCompass->data->compassname,
            aCompass->data );
        //wxFont *pFont = OCPNGetFont(_T("Dialog"), 0);
    }

     m_CompasDevListDlg->Show(!m_CompasDevListDlg->IsShown());
     
}


void deviation_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{
    //plugin_pfix_ex = pfix;
//     g_lat = pfix.Lat;
//     g_lon = pfix.Lon; 
//     g_fixtime = pfix.FixTime;
     g_var = pfix.Var;
//     g_hdm = pfix.Hdm;
    if ( B_Dlg != NULL )
        B_Dlg->SetPositionFix(pfix);
}

//Demo implementation of response mechanism
void deviation_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
//     if(message_id == _T("Deviation_VARIATION_REQUEST"))
//     {
//         wxJSONReader r;
//         wxJSONValue v;
//         r.Parse(message_body, &v);
//         double lat = v[_T("Lat")].AsDouble();
//         double lon = v[_T("Lon")].AsDouble();
//         int year = v[_T("Year")].AsInt();
//         int month = v[_T("Month")].AsInt();
//         int day = v[_T("Day")].AsInt();
//         SendVariationAt(lat, lon, year, month, day);
//     }
//     else if(message_id == _T("Deviation_VARIATION_BOAT_REQUEST"))
//     {
//         SendBoatVariation();
//     }
//     else if(message_id == _T("Deviation_VARIATION_CURSOR_REQUEST"))
//     {
//         SendCursorVariation();
//     }
}

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
                    DrawToolbarIconNumber(mHdm);
                    double mHdt = mHdm + g_var + aCompass->data->getDeviation( mHdm );
                    if( sentence.Left(6) != _("$XXHDG")){
                        SendNMEASentence( _("$XXHDT,") + wxString::Format( _("%1.1f"), mHdt));                        
                        SendNMEASentence( _("$XXHDG,") + wxString::Format( _("%1.1f,%1.1f,E,%1.1f,E"), mHdm, g_var, aCompass->data->getDeviation( mHdm )));
                    }
                    if ( B_Dlg != NULL ){
                        B_Dlg->SetNMEAHeading(mHdm);
                    }
                }
            }
        }

    }
    
}

bool deviation_pi::LoadConfig(void)
{
    m_shareLocn =  *GetpPrivateApplicationDataLocation();
    // above function is not always give a path seperator on the end. So we have to check for this.
    if ( m_shareLocn.Right(1) != wxFileName::GetPathSeparator())
        m_shareLocn = m_shareLocn + wxFileName::GetPathSeparator();
     m_shareLocn = m_shareLocn + wxT("plugins") + wxFileName::GetPathSeparator() + 
            wxT("deviation_pi");
    // Check if directory exsist, and if not make it
    if ( !wxFileName::DirExists(m_shareLocn) )
        wxFileName::Mkdir( m_shareLocn, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    
    filename = m_shareLocn + wxFileName::GetPathSeparator() + _T("deviation_data.xml");

    
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

void deviation_pi::DrawToolbarIconNumber( float dev )
{
    wxString NewVal = wxString::Format(_T("%.1f"), aCompass->data->getDeviation( dev ));
    double scale = GetOCPNGUIToolScaleFactor_PlugIn();
    scale = wxRound(scale * 4.0) / 4.0;
    scale = wxMax(1.0, scale);          // Let the upstream processing handle minification.
    
    if( b_ShowIcon && b_ShowLiveIcon && ((m_LastVal != NewVal) || (scale != m_scale)) )
    {
        m_scale = scale;
        m_LastVal = NewVal;
        int w = _img_deviation_live->GetWidth() * scale;
        int h = _img_deviation_live->GetHeight() * scale;
        wxMemoryDC dc;
        wxBitmap icon;
        
        //  Is SVG available?
        wxBitmap live = GetBitmapFromSVGFile(m_shareLocn + _T("deviation_live.svg"), w, h);
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
        
        SetToolbarToolBitmaps(i_leftclick_tool_id, &icon, &icon);
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

   return( wxString::Format(_("%02X"), calculated_checksum) );
}
