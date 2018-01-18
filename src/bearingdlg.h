/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  Dirk Smits <email>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef BEARINGDLG_H
#define BEARINGDLG_H

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/datetime.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/timectrl.h>
#include <wx/gbsizer.h>
#include <wx/valnum.h>
#include "spa.h"
#include "SharedStuff.h"
#include "ocpn_plugin.h"

// class BearingDlg;
// BearingDlg* B_Dlg;

enum {
    devBEARING = 0,    
    devSUN_BEARING = 1,
    devSUN_SHADOW = 2,
    devBEARING_FROM_ROUTE = 3
};
enum{
    devCOMPASBEARING = 0,
    devRIGHTHANDBEARING = 1    
};

class BearingDlg: public wxDialog
{
	public:

		BearingDlg(wxWindow* parent, Meassurement* Mess, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize,   bool fullsize=true);
		virtual ~BearingDlg();

		//(*Declarations(BearingDlg)
		wxChoice* Choice;
        wxChoice* BearingChoice;
		wxTimePickerCtrl* TPickerCtrl;
		wxTextCtrl* CompassCourseCtrl;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxDatePickerCtrl* DPickerCtrl;
		wxStaticText* StaticText7;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText2;
		wxStaticText* StaticText6;
        wxStaticText* StaticText8;
        wxStaticText* StaticText9;
        wxStaticText* StaticText10;
        wxStaticText* StaticText11;
        wxStaticText* StaticText12;
		wxTextCtrl* TrueBearingCtrl;
		wxTextCtrl* CompassBearingCtrl;
        wxTextCtrl* VariationCtrl;
        wxTextCtrl* RemarksCtr;
        void SetDateTime(wxDateTime dt);
        wxDateTime GetDateTime();
        
        float TB_value;
        float CB_value;
        float CC_value;
        float VAR_value;
        float DEV_value;
        wxDateTime DT_value;
        float LAT_value;
        float LON_value;
        wxString Remarks_value;
        void SetPositionFix(PlugIn_Position_Fix_Ex &pfix);
        void SetNMEATimeFix(wxDateTime dt);
        void SetNMEAHeading(double hd);
        void SetSunBearing(wxDateTime t);
        void SetMessageVariation(wxString &message_id, wxString &message_body);
        void SetMessageRouteActiveLeg(wxString &message_id, wxString &message_body);
        
	protected:
		//(*Identifiers(BearingDlg)
		static const long ID_CHOICE;
		static const long ID_STATICTEXT1;
		static const long ID_DATEPICKERCTRL;
		static const long ID_TIMEPICKERCTRL;
		static const long ID_STATICTEXT2;
		static const long ID_TBCTRL;
		static const long ID_CBCTRL;
		static const long ID_CCCTRL;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT6;
		static const long ID_STATICTEXT4;
        static const long ID_STATICTEXT8;
        static const long ID_STATICTEXT9;
        static const long ID_STATICTEXT10;
        static const long ID_STATICTEXT11;
        static const long ID_VARCTRL;
        static const long ID_REMARKSCTRL;
        
		//*)

	private:
        void OnClose(wxCloseEvent& event);
        void OnChoiseSelect(wxCommandEvent& event);
        void OnTimeSelect(wxCommandEvent& event);
        void OnTextCtrlEnter(wxCommandEvent& event);
        //void OnChoiceCtrlEnter(wxCommandEvent& event);
        void OnOKBtnClick(wxCommandEvent& event);
        virtual bool Show(bool show = true);
        
        void CopyMessObjToDlg();    
        void CopyDlgToMessObj();
        
        Meassurement* localMesData;
        bool UpdateFlag;
        int GPS_UpdateTime;
        
        void GetMessageVariation(double lat, double lon);

		//(*Handlers(BearingDlg)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif // BEARINGDLG_H
