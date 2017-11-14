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

void DeviationLogMessage1(wxString s) { wxLogMessage(_T("Deviation: ") + s); }
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
//     AddLocaleCatalog( _T("opencpn-deviation_pi") );
// 
//     // Set some default private member parameters
//     m_deviation_dialog_x = 0;
//     m_deviation_dialog_y = 0;
// 
//     ::wxDisplaySize(&m_display_width, &m_display_height);
// 
//     //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
    m_parent_window = GetOCPNCanvasWindow();
    m_CompasDevListDlg = NULL;
//     //    Get a pointer to the opencpn configuration object
     m_pconfig = GetOCPNConfigObject();
// 
//     //    And load the configuration items
     LoadConfig();

     m_LastVal = wxEmptyString;
// 
//     pFontSmall = new wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
//     m_shareLocn =*GetpSharedDataLocation() +
//     _T("plugins") + wxFileName::GetPathSeparator() +
//     _T("deviation_pi") + wxFileName::GetPathSeparator();
//     
//     //    Deviation initialization
//     /* Memory allocation */
//     int NumTerms = ( ( Deviation_MAX_MODEL_DEGREES + 1 ) * ( Deviation_MAX_MODEL_DEGREES + 2 ) / 2 );    /* Deviation_MAX_MODEL_DEGREES is defined in Deviation_Header.h */
// 

    int ret_flag =  (
    WANTS_CURSOR_LATLON     |
    WANTS_TOOLBAR_CALLBACK  |
    WANTS_NMEA_EVENTS       |
    WANTS_PREFERENCES       |
    WANTS_CONFIG          |
    WANTS_PLUGIN_MESSAGING
 
    );
    m_bShowIcon = true;
    if(m_bShowIcon){
    //    This PlugIn needs a toolbar icon, so request its insertion
        m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_deviation, _img_deviation, wxITEM_NORMAL,
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
    RemovePlugInTool(m_leftclick_tool_id);

//     delete pFontSmall;
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
//     if (NULL == m_pWmmDialog)
//         return;
//     DimeWindow(m_pWmmDialog);
}

void deviation_pi::SetIconType()
{
    if(m_bShowLiveIcon){
        SetToolbarToolBitmaps(m_leftclick_tool_id, _img_deviation, _img_deviation);
        SetToolbarToolBitmapsSVG(m_leftclick_tool_id, _T(""), _T(""), _T(""));
        m_LastVal.Empty();
    }
    else{
        wxString normalIcon = m_shareLocn + _T("deviation_pi.svg");
        wxString toggledIcon = m_shareLocn + _T("deviation_pi.svg");
        wxString rolloverIcon = m_shareLocn + _T("deviation_pi.svg");
        
        SetToolbarToolBitmapsSVG(m_leftclick_tool_id, normalIcon, rolloverIcon, toggledIcon);
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
        wxFont *pFont = OCPNGetFont(_T("Dialog"), 0);
    }

     m_CompasDevListDlg->Show(!m_CompasDevListDlg->IsShown());
     
}


void deviation_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{
    //plugin_pfix_ex = pfix;
//     g_lat = pfix.Lat;
//     g_lon = pfix.Lon; 
//     g_fixtime = pfix.FixTime;
//     g_var = pfix.Var;
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


bool deviation_pi::LoadConfig(void)
{
    m_shareLocn =*GetpSharedDataLocation() +
    _T("plugins") + wxFileName::GetPathSeparator() +
    _T("deviation_pi");
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

