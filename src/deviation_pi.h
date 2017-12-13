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
#define     MY_API_VERSION_MINOR    14

#include "ocpn_plugin.h"


#include "jsonreader.h"
#include "jsonwriter.h"
#include "preferencedlg.h"
#include "compass.h"

#include "nmea0183/nmea0183.h"
#include <wx/datetime.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/timectrl.h>
#include <wx/gbsizer.h>
#include <wx/valnum.h>

#include "readwritexml.h"
#include "SharedStuff.h"
#include "bearingdlg.h"
#include "spa.h"
#include "printing.h"
#include "preferencedlg.h"

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define Deviation_TOOL_POSITION    -1        // Request default positioning of toolbar tool

class deviation_pi;
class PreferenceDlg;
class Compass;
class Meassurement;
class DevDialog;
class DevTableDialog;
class BasicDrawPane;
class CompasDev1Dialog;

class deviation_pi : public opencpn_plugin_114
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
    void SendTrueCourse(double CompasCourse);
    void SendDeviation();
    void SendDeviationAt(double CompasCourse);
    void SetNMEASentence(wxString &sentence);

//    Other public methods
//     void SetWmmDialogX    (int x){ m_deviation_dialog_x = x;};
//     void SetWmmDialogY    (int x){ m_deviation_dialog_y = x;}

    void OnWmmDialogClose();
    void ShowPlotSettings();
    void DrawToolbarIconNumber( float dev );
    void SendNMEASentence(wxString sentence);
    void RequestPliginMessage(wxString MessageID, wxJSONValue message);
    wxString ComputeChecksum( wxString sentence );
//    Deviation Declarations
    
    wxString filename;
    Compass* aCompass;
    wxWindow       *m_parent_window;
    //CompasDev1Dialog *m_CompasDevListDlg;

private:
    wxFileConfig     *m_pconfig;
    wxString      m_shareLocation;
    wxString      ShipsName;
    wxString        CompassName;
    int          i_ShowLiveIcon;
    bool          b_ShowIcon;
    wxString        m_LastValue;
    double        g_var;
    double        g_dev;
    int           i_leftclick_dev_tool_id;
    NMEA0183      m_NMEA0183;                 // Used to parse NMEA Sentences
    short         mPriHeadingM, mPriHeadingT, mPriVar, mPriDateTime;
    wxDateTime    mUTCDateTime;
    double               mHdm;
    double        m_scale;
    wxFont        *pFontSmall;
};




//#include "tinyxml2/tinyxml2.h"
//#include "graphandprint.h"



class CompasDev1Dialog: public wxFrame
{
	public:
		CompasDev1Dialog(wxWindow *parent,
            const wxString &title,
            compass_data* Data=NULL );
		virtual ~CompasDev1Dialog();

		//(*Declarations(CompasDev1Dialog)
        wxFlexGridSizer* FlexGridSizer; 
		wxListCtrl* DevListCtrl;
        wxBoxSizer* BoxSizerButons;
        wxButton* AddBtn;
        wxButton* DeleteBtn;
        wxButton* EditBtn;
        wxButton* ShowBtn;
        wxButton* CancelBtn;
        wxButton* CloseBtn;
        
        void OnCloseWindow(wxCloseEvent &event);
        void OnAddBtnClick(wxCommandEvent& event);
        void OnDeleteBtnClick(wxCommandEvent& event);
        void OnEditBtnClick(wxCommandEvent& event);
        void OnShowBtnClick(wxCommandEvent& event);
        void OnCancelBtnClick(wxCommandEvent& event);
        void OnOKBtnClick(wxCommandEvent& event);
        void OnDevListCtrlItemSelect(wxListEvent& event);
        void OnDevListCtrlItemDeselect(wxListEvent& event);
		void OnDevListCtrlItemActivated(wxListEvent& event);
        void OnConnectionToggleEnableMouse(wxMouseEvent& event);
        void OnPreviewFrameModalityKind(wxCommandEvent& event);
        void OnRightDown(wxMouseEvent& event);
        void OnResize(wxSizeEvent& event);
        void EnableItem(const long index);
        //bool Show();
        void FillSourceList(void) ;
        void UpdateWindows(void);
        compass_data* data;
        DevTableDialog* DT_Dlg;
        //BearingDlg* B_Dlg;

	protected:

		//(*Identifiers(CompasDev1Dialog)
		static const long ID_LISTCTRLDEV;
		static const long ID_ADDBUTTON;
		static const long ID_DELBUTTON;
		static const long ID_EDITBUTTON;
		static const long ID_BUTTONSHOW;
		static const long ID_BUTTONCLOSE;
        static const long ID_BUTTONCANCEL;
		static const long ID_CBCTRL;
		//*)

	private:
        
       // wxPreviewFrameModalityKind m_previewModality;
        
		//(*Handlers(CompasDev1Dialog)
		//*)
        long listselindex;
		DECLARE_EVENT_TABLE()
};






class BasicDrawPane : public wxPanel
{
public:
    BasicDrawPane(DevTableDialog *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr,
                compass_data* Data=NULL);
    ~BasicDrawPane();
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void DrawTextCenterCenter(wxDC&  dc, wxString text, wxPoint pos);
    void render(wxDC& dc, compass_data* Data);
    wxBitmap* LogoBitmap;
    DevTableDialog *parentdlg;
    compass_data* data;
    DECLARE_EVENT_TABLE()
};

class DevTableDialog: public wxFrame
{
public:

    DevTableDialog(wxWindow* parent,wxWindowID id = wxID_ANY, compass_data* Data=NULL);
    virtual ~DevTableDialog();
    compass_data* data;
private:

    //(*Handlers(DevTableDialog)
    void OnQuit(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent &event);
    void OnPrintBtnClick(wxCommandEvent& event);
    void OnCloseBtnClick(wxCommandEvent& event);
    void OnPrintPreviewBtnClick(wxCommandEvent& event);
    //*)

    //(*Identifiers(DevTableDialog)
    static const long ID_PANEL1;
    static const long ID_PRINT;
    static const long ID_CLOSE;
    static const long ID_PRINTPREV;
    //*)

    //(*Declarations(DevTableDialog)
    BasicDrawPane* Panel1;
    wxButton* PrintBtn;
    wxButton* PrintPrevBtn;
    wxButton* CloseBtn;
    CompasDev1Dialog *parentdlg;
    //*)

    DECLARE_EVENT_TABLE()
};


#endif
