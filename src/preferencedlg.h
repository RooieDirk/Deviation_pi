
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

#ifndef PREFERENCEDLG_H
#define PREFERENCEDLG_H

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/gbsizer.h>
#include "SharedStuff.h"
#include "readwritexml.h"

class PreferenceDlg: public wxDialog
{
	public:

		PreferenceDlg(wxWindow* parent,wxWindowID id=wxID_ANY, compass_data* Data=NULL);
		virtual ~PreferenceDlg();

		//(*Declarations(PreferenceDlg)
		wxCheckBox* ShowToolbarCB;
		wxStaticText* StaticText1;
		wxCheckBox* SendNmeaCB;
		wxComboBox* CompassnameBox;
		wxComboBox* ShipsnameBox;
		wxStaticText* StaticText2;
        wxArrayString ShipsArrayStr;
        wxArrayString CompasArrayStr;
        compass_data* data;
       
		//*)

	protected:

		//(*Identifiers(PreferenceDlg)
		static const long ID_STATICTEXT1;
		static const long ID_SHIPSNAMEB;
		static const long ID_STATICTEXT2;
		static const long ID_COMPASSNAMEB;
		static const long ID_SENDNMEACB;
		static const long ID_CHECKBOX2;
		//*)

	private:

		//(*Handlers(PreferenceDlg)
		void OnShipsnameBoxTextUpdated(wxCommandEvent& event);
        void OnCompassnameBoxTextUpdated(wxCommandEvent& event);
		void OnRadioButton1Select(wxCommandEvent& event);
        void GetListOfAvailableShips();
        void GetListOfAvailableCompasses();
        void OnOKBtnClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif // PREFERENCEDLG_H
