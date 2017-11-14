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

#include "preferencedlg.h"

//(*IdInit(PreferenceDlg)
const long PreferenceDlg::ID_STATICTEXT1 = wxNewId();
const long PreferenceDlg::ID_SHIPSNAMEB = wxNewId();
const long PreferenceDlg::ID_STATICTEXT2 = wxNewId();
const long PreferenceDlg::ID_COMPASSNAMEB = wxNewId();
const long PreferenceDlg::ID_SENDNMEACB = wxNewId();
const long PreferenceDlg::ID_CHECKBOX2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(PreferenceDlg,wxDialog)
	//(*EventTable(PreferenceDlg)
	//*)
END_EVENT_TABLE()

PreferenceDlg::PreferenceDlg(wxWindow* parent,wxWindowID id, compass_data* Data )
{
	
    data = Data;
    wxPuts( data->shipsname );
    GetListOfAvailableShips();
    GetListOfAvailableCompasses();
    //(*Initialize(PreferenceDlg)
	wxGridBagSizer* GridBagSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	GridBagSizer1 = new wxGridBagSizer(0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, wxT("Shipsname"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	GridBagSizer1->Add(StaticText1, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ShipsnameBox = new wxComboBox(this, ID_SHIPSNAMEB, wxEmptyString, wxDefaultPosition, wxDefaultSize, ShipsArrayStr,  wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_SHIPSNAMEB"));
        ShipsnameBox->SetValue( data->shipsname );
	GridBagSizer1->Add(ShipsnameBox, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, wxT("Compass name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	GridBagSizer1->Add(StaticText2, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	CompassnameBox = new wxComboBox(this, ID_COMPASSNAMEB, wxEmptyString, wxDefaultPosition, wxDefaultSize, CompasArrayStr, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_COMPASSNAMEB"));
        CompassnameBox->SetValue( data->compassname );
	GridBagSizer1->Add(CompassnameBox, wxGBPosition(1, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	GridBagSizer1->Add(StdDialogButtonSizer1, wxGBPosition(4, 0), wxGBSpan(1, 2), wxALL|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	SendNmeaCB = new wxCheckBox(this, ID_SENDNMEACB, wxT("Use to send NMEA true north"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SENDNMEACB"));
	SendNmeaCB->SetValue(false);
	GridBagSizer1->Add(SendNmeaCB, wxGBPosition(2, 0), wxGBSpan(1, 2), wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	ShowToolbarCB = new wxCheckBox(this, ID_CHECKBOX2, wxT("Show in toolbar"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	ShowToolbarCB->SetValue(true);
	GridBagSizer1->Add(ShowToolbarCB, wxGBPosition(3, 0), wxGBSpan(1, 2), wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(GridBagSizer1);
	GridBagSizer1->Fit(this);
	GridBagSizer1->SetSizeHints(this);

	Connect(ID_SHIPSNAMEB,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&PreferenceDlg::OnShipsnameBoxTextUpdated);
	//Connect(ID_SHIPSNAMEB,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&PreferenceDlg::OnShipsnameBoxTextUpdated);
	Connect(ID_SHIPSNAMEB,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&PreferenceDlg::OnShipsnameBoxTextUpdated);
	Connect(ID_COMPASSNAMEB,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&PreferenceDlg::OnCompassnameBoxTextUpdated);
	//Connect(ID_COMPASSNAMEB,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&PreferenceDlg::OnCompassnameBoxTextUpdated);
	Connect(ID_COMPASSNAMEB,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&PreferenceDlg::OnCompassnameBoxTextUpdated);
    Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&PreferenceDlg::OnOKBtnClick);
	//*)
}

PreferenceDlg::~PreferenceDlg()
{
	//(*Destroy(PreferenceDlg)
	//*)
}


void PreferenceDlg::OnShipsnameBoxTextUpdated(wxCommandEvent& event)
{
    wxPuts(_("OnShipsnameBoxTextUpdated"));
    data->shipsname = ShipsnameBox->GetValue();
    
    GetListOfAvailableCompasses();
    CompassnameBox->Clear();
    CompassnameBox->Append(CompasArrayStr);
    if (CompasArrayStr.GetCount() == 0)  
        CompassnameBox->SetValue(_("-----"));
    else
        CompassnameBox->SetSelection(0);
}
void PreferenceDlg::OnCompassnameBoxTextUpdated(wxCommandEvent& event)
{
    wxPuts(_("OnCompssnameBoxTextUpdated"));
    data->compassname = CompassnameBox->GetValue();
}

void PreferenceDlg::OnRadioButton1Select(wxCommandEvent& event)
{
}

void PreferenceDlg::GetListOfAvailableShips()
{
    ReadWriteXML* a = new ReadWriteXML( data);
    ShipsArrayStr = a->GetShipsList();
    delete a;
}
void PreferenceDlg::GetListOfAvailableCompasses()
{
    ReadWriteXML* a = new ReadWriteXML( data );
    CompasArrayStr.Clear();
    CompasArrayStr = a->GetCompassList();
    
    delete a;
}
void PreferenceDlg::OnOKBtnClick(wxCommandEvent& event)
{
    data->shipsname = ShipsnameBox->GetValue();
    data->compassname = CompassnameBox->GetValue();
    ReadWriteXML* a = new ReadWriteXML( data ); 
    
    delete a;
    EndModal(wxID_OK);
}
