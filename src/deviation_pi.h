/******************************************************************************
 * $Id: deviation_pi.h,v 1.0 2011/02/26 01:54:37 nohal Exp $
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

#ifndef _DeviationPI_H_
#define _DeviationPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/fileconf.h>

#include "version.h"

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    8

#include "ocpn_plugin.h"


#include "jsonreader.h"
#include "jsonwriter.h"
#include "preferencedlg.h"
#include "compass.h"
#include "CompasDev1Main.h"

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define Deviation_TOOL_POSITION    -1        // Request default positioning of toolbar tool

class deviation_pi;
class PreferenceDlg;


class deviation_pi : public opencpn_plugin_18
{
public:
    deviation_pi(void *ppimgr);

//    The required PlugIn Methods
    int Init(void);
    bool DeInit(void);

    int GetAPIVersionMajor();
    int GetAPIVersionMinor();
    int GetPlugInVersionMajor();
    int GetPlugInVersionMinor();
    wxBitmap *GetPlugInBitmap();
    wxString GetCommonName();
    wxString GetShortDescription();
    wxString GetLongDescription();

//    The required override PlugIn Methods
    void SetCursorLatLon(double lat, double lon);
    void SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix);

    int GetToolbarToolCount(void);
    bool SaveConfig(void);
    bool LoadConfig(void);
    void ShowPreferencesDialog( wxWindow* parent );
    void RearrangeWindow();
    void OnToolbarToolCallback(int id);
    void SetIconType();

//    Optional plugin overrides
    void SetColorScheme(PI_ColorScheme cs);
    void SetPluginMessage(wxString &message_id, wxString &message_body);

    

//    Other public methods
//     void SetWmmDialogX    (int x){ m_deviation_dialog_x = x;};
//     void SetWmmDialogY    (int x){ m_deviation_dialog_y = x;}

    void OnWmmDialogClose();
    void ShowPlotSettings();

//    Deviation Declarations
    
    wxString filename;
    Compass* aCompass;
    wxWindow       *m_parent_window;
    CompasDev1Dialog *m_CompasDevListDlg;

private:
    wxFileConfig     *m_pconfig;
    wxString      m_shareLocn;
    wxString      ShipsName;
    wxString        CompassName;
    bool          m_bShowLiveIcon;
    bool          m_bShowIcon;
    wxString        m_LastVal;

    int           m_leftclick_tool_id;
};


#endif
