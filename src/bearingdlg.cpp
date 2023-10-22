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

#include "bearingdlg.h"
#include "deviation_pi.h"
#include "wx/jsonreader.h"
#include "wx/jsonwriter.h"

BearingDlg* B_Dlg;
extern deviation_pi* Dev_PI;

BEGIN_EVENT_TABLE(BearingDlg,wxDialog)
	//(*EventTable(BearingDlg)
	//*)
END_EVENT_TABLE()
//(*IdInit(BearingDlg)
const long BearingDlg::ID_CHOICE = wxNewId();
const long BearingDlg::ID_STATICTEXT1 = wxNewId();
const long BearingDlg::ID_DATEPICKERCTRL = wxNewId();
const long BearingDlg::ID_TIMEPICKERCTRL = wxNewId();
const long BearingDlg::ID_STATICTEXT2 = wxNewId();
const long BearingDlg::ID_TBCTRL = wxNewId();
const long BearingDlg::ID_CBCTRL = wxNewId();
const long BearingDlg::ID_CCCTRL = wxNewId();
const long BearingDlg::ID_STATICTEXT7 = wxNewId();
const long BearingDlg::ID_STATICTEXT5 = wxNewId();
const long BearingDlg::ID_STATICTEXT3 = wxNewId();
const long BearingDlg::ID_STATICTEXT6 = wxNewId();
const long BearingDlg::ID_STATICTEXT4 = wxNewId();
const long BearingDlg::ID_STATICTEXT8 = wxNewId();
const long BearingDlg::ID_STATICTEXT9 = wxNewId();
const long BearingDlg::ID_STATICTEXT10 = wxNewId();
const long BearingDlg::ID_STATICTEXT11 = wxNewId();
const long BearingDlg::ID_VARCTRL =  wxNewId();
const long BearingDlg::ID_REMARKSCTRL =  wxNewId();
//*)

BearingDlg::BearingDlg(wxWindow* parent, Meassurement* Mess, wxWindowID id,const wxPoint& pos,const wxSize& size, bool fullsize )
{
    //(*Initialize(BearingDlg)
    localMesData = Mess;
    Remarks_value = wxEmptyString;
	wxFlexGridSizer* FlexGridSizer1;
	wxGridBagSizer* GridBagSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(199,215));
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Methode"));
    wxArrayString aS;
    aS.Insert(_("Bearing"), devBEARING);
    aS.Insert(_("Sun bearing"), devSUN_BEARING);
    aS.Insert(_("Sun bearing shadowline"), devSUN_SHADOW);
    //aS.Insert(_("Bearing from Route"), devBEARING_FROM_ROUTE);
    
    Choice = new wxChoice (this, ID_CHOICE, wxDefaultPosition, wxDefaultSize, aS, 0, wxDefaultValidator, _T("ID_CHOICE"));
 	Choice->SetSelection( devBEARING );
	StaticBoxSizer1->Add(Choice, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridBagSizer1 = new wxGridBagSizer(0, 0);
	GridBagSizer1->AddGrowableCol(0);
	GridBagSizer1->AddGrowableRow(5);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Date/Time"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	GridBagSizer1->Add(StaticText1, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	DPickerCtrl = new wxDatePickerCtrl(this, ID_DATEPICKERCTRL, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT|wxDP_SHOWCENTURY, wxDefaultValidator, _T("ID_DATEPICKERCTRL"));
	GridBagSizer1->Add(DPickerCtrl, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TPickerCtrl = new wxTimePickerCtrl(this, ID_TIMEPICKERCTRL, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxTP_DEFAULT, wxDefaultValidator, _T("ID_TIMEPICKERCTRL"));
	GridBagSizer1->Add(TPickerCtrl, wxGBPosition(0, 2), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	wxArrayString aSb;
    aSb.Insert(_("Compass bearing"), devCOMPASBEARING);
    aSb.Insert(_("Righthand bearing"), devRIGHTHANDBEARING);
    BearingChoice = new wxChoice (this, ID_CHOICE, wxDefaultPosition, wxDefaultSize, aSb, 0, wxDefaultValidator, _T("ID_CHOICE"));
    BearingChoice->SetSelection( 0);    
	GridBagSizer1->Add(BearingChoice, wxGBPosition(2, 0), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Compass Course"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	GridBagSizer1->Add(StaticText2, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
        // Allow floating point numbers from 0 to 360 with 1 decimal
        // digits only and handle empty string as 0 by default.
    wxFloatingPointValidator<float>  val_TB(1, &TB_value );
        val_TB.SetRange(0, 360);
    wxFloatingPointValidator<float>  val_CC(1, &CC_value );
        val_CC.SetRange(0, 360);
    CompassCourseCtrl = new wxTextCtrl(this, ID_CCCTRL, _T("0"), wxDefaultPosition, wxDefaultSize, 0, val_CC, _T("ID_CCCTRL"));
    GridBagSizer1->Add(CompassCourseCtrl, wxGBPosition(1, 1), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	TrueBearingCtrl = new wxTextCtrl(this, ID_TBCTRL, _T("0"), wxDefaultPosition, wxDefaultSize, 0, val_TB, _T("ID_TBCTRL"));
	GridBagSizer1->Add(TrueBearingCtrl, wxGBPosition(3, 1), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    wxFloatingPointValidator<float>  val_CB(1, &CB_value );
        val_CB.SetRange(0, 360);
	CompassBearingCtrl = new wxTextCtrl(this, ID_CBCTRL, _T("0"), wxDefaultPosition, wxDefaultSize, 0, val_CB, _T("ID_CBCTRL"));
	GridBagSizer1->Add(CompassBearingCtrl, wxGBPosition(2, 1), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
	
    
    StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Variation"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	GridBagSizer1->Add(StaticText9, wxGBPosition(4, 0), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    wxFloatingPointValidator<float>  val_VAR(1, &VAR_value );
        val_VAR.SetRange(-180, 180);
	VariationCtrl = new wxTextCtrl(this, ID_VARCTRL, _T("0"), wxDefaultPosition, wxDefaultSize, 0, val_VAR, _T("ID_VARCTRL"));
    GridBagSizer1->Add(VariationCtrl, wxGBPosition(4, 1), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    StaticText9 = new wxStaticText(this, ID_STATICTEXT8, _T("\u00B0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	GridBagSizer1->Add(StaticText9, wxGBPosition(4, 2), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText10= new wxStaticText(this, ID_STATICTEXT10, _("Deviation"), wxDefaultPosition,wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	GridBagSizer1->Add(StaticText10, wxGBPosition(5, 0), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText11= new wxStaticText(this, ID_STATICTEXT11, _T("0.0"), wxDefaultPosition,wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	GridBagSizer1->Add(StaticText11, wxGBPosition(5, 1), wxGBSpan(1, 2), wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    wxFont StaticText11Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    if ( !StaticText11Font.Ok() ) StaticText11Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    StaticText11Font.SetPointSize(15);
    StaticText11Font.SetWeight(wxFONTWEIGHT_BOLD );
    StaticText11->SetFont(StaticText11Font);
    
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _T("\u00B0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	GridBagSizer1->Add(StaticText7, wxGBPosition(3, 2), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _T("\u00B0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	GridBagSizer1->Add(StaticText5, wxGBPosition(2, 2), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _T("\u00B0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	GridBagSizer1->Add(StaticText3, wxGBPosition(1, 2), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("True Bearing"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	GridBagSizer1->Add(StaticText6, wxGBPosition(3, 0), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    StaticText12 = new wxStaticText(this, ID_STATICTEXT6, _("Remarks"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	GridBagSizer1->Add(StaticText12, wxGBPosition(6, 0), wxDefaultSpan, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    RemarksCtr = new wxTextCtrl(this, ID_REMARKSCTRL, _("Text"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxTextValidator(wxFILTER_ALPHA, &Remarks_value), _T("ID_REMARKSCTRL"));
    GridBagSizer1->Add(RemarksCtr, wxGBPosition(6, 1),  wxGBSpan(1, 2), wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
    
        GridBagSizer1->Add(StdDialogButtonSizer1, wxGBPosition(7, 1), wxGBSpan(1, 2), wxALL|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
	FlexGridSizer1->Add(GridBagSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);
	Layout();
    CopyMessObjToDlg();
    GPS_UpdateTime = 0;

    Connect(ID_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&BearingDlg::OnChoiseSelect);
    //Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&BearingDlg::OnClose);
    Connect(wxID_ANY,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&BearingDlg::OnTextCtrlEnter);
    Connect(wxID_ANY,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&BearingDlg::OnTextCtrlEnter);
    Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&BearingDlg::OnOKBtnClick);
    Connect(ID_TIMEPICKERCTRL,wxEVT_TIME_CHANGED,(wxObjectEventFunction)&BearingDlg::OnTimeSelect);
	//*)
}

BearingDlg::~BearingDlg()
{
	
    //(*Destroy(BearingDlg)
	//*)
}

bool BearingDlg::Show(bool show)
{
    bool temp = wxDialog::Show(show);
    UpdateFlag = true; //Set updateflag after Textctr's are updated
    Dev_PI->DoRouteLegRequest();
    return temp;
}

void BearingDlg::OnChoiseSelect(wxCommandEvent& event)
{
    SetSunBearing(GetDateTime());
}
void BearingDlg::OnTimeSelect(wxCommandEvent& event)
{
    SetSunBearing(GetDateTime());
}
void BearingDlg::OnTextCtrlEnter(wxCommandEvent& event)
{
    bool OK = false;
    double tvalue, bvalue, vvalue;

    if( TrueBearingCtrl->GetValue().ToDouble(&tvalue)) 
        if( CompassBearingCtrl->GetValue().ToDouble(&bvalue))
            if( VariationCtrl->GetValue().ToDouble(&vvalue))
                {
                    DEV_value = tvalue - bvalue - vvalue;
                    while ( DEV_value < -180 ) DEV_value += 360;
                    while ( DEV_value > 180 ) DEV_value -= 360;
                    StaticText11->SetLabel(  wxString::Format(_T("%0.1f"), DEV_value));
                    OK = true;
                }
     if ( !OK )  StaticText11->SetLabel( _T("nan"));
     UpdateFlag = false; //Stop auto updating values from nmea after first edit 
}
void BearingDlg::OnClose(wxCloseEvent& event)
{
    
}

void BearingDlg::OnOKBtnClick(wxCommandEvent& event)
{
    if ( Validate() && TransferDataFromWindow() )
    {
        CopyDlgToMessObj();
        if ( IsModal() )
            EndModal(wxID_OK);
    }
    else
        wxMessageBox(_("Error Values entered are not valid"));
}

void BearingDlg::SetDateTime(wxDateTime dt)
{
    DT_value = dt;
    DPickerCtrl->SetValue(DT_value);
    TPickerCtrl->SetValue(DT_value);
        if (Choice->GetSelection()==devSUN_BEARING) //SunBearing
        TrueBearingCtrl->ChangeValue( (wxString::Format(_T("%05.1f"), SolarAzimuth( GetDateTime(), localMesData->lat, localMesData->lon) ) ) );
        
    if (Choice->GetSelection()==devSUN_SHADOW) //SunBearing shadow   {
        TrueBearingCtrl->ChangeValue( (wxString::Format(_T("%05.1f"),  limit_degrees(SolarAzimuth( GetDateTime(), localMesData->lat, localMesData->lon )+180) ) ) );
}

wxDateTime BearingDlg::GetDateTime()
{
    wxDateTime returnval;
    returnval = DPickerCtrl->GetValue();
    returnval.SetHour( TPickerCtrl->GetValue().GetHour() );
    returnval.SetMinute( TPickerCtrl->GetValue().GetMinute() );
    returnval.SetSecond( TPickerCtrl->GetValue().GetSecond() );
    return returnval;
}
void BearingDlg::CopyDlgToMessObj () 
{
    localMesData->compasscourse = CC_value;
    localMesData->compassbearing = CB_value;
    localMesData->truebearing = TB_value;
    localMesData->variation = VAR_value;
    localMesData->deviation = DEV_value;
    localMesData->datetime = GetDateTime();
    localMesData->enabled = true;
    localMesData->methode = Choice->GetSelection();
    localMesData->MeassurementRemarks = Remarks_value;
}
void BearingDlg::CopyMessObjToDlg() 
{
    CC_value = localMesData->compasscourse ;
    CB_value = localMesData->compassbearing;
    TB_value = localMesData->truebearing;
    VAR_value =localMesData->variation;
    DEV_value =localMesData->deviation;
    SetDateTime( localMesData->datetime );
    Choice->SetSelection(localMesData->methode);
    Remarks_value = localMesData->MeassurementRemarks;
}
void BearingDlg::SetPositionFix(PlugIn_Position_Fix_Ex &pfix)
{
    bool TempFlag = UpdateFlag;
    if (localMesData->lat > 90.0) //not valid so it must be a new entry. Set values from GPS 
    {
        localMesData->lat = pfix.Lat;
        localMesData->lon = pfix.Lon;
    }
   
    if ( UpdateFlag)        
    {
        GetMessageVariation(localMesData->lat, localMesData->lon);
        Dev_PI->DoRouteLegRequest();
        
        if (GPS_UpdateTime <= 0) 
        {
            wxWindow *w = FindFocus(); // Remember wich control has focus
            
            SetDateTime(wxDateTime::Now());
            GPS_UpdateTime = 0;
            if (w)
              w->SetFocus(); // restore focus
        }
        else
            GPS_UpdateTime -= 1;
        
       
        UpdateFlag = TempFlag;    
    }    
}
void BearingDlg::SetNMEATimeFix(wxDateTime dt)
{
    bool TempFlag = UpdateFlag;
    if ( UpdateFlag){
        wxWindow *w = FindFocus(); // Remember wich control has focus
        wxTimeSpan timezone = dt - dt.ToUTC();
        TPickerCtrl->SetValue(dt.Add( timezone) );
        GPS_UpdateTime = 10;
        SetSunBearing(GetDateTime());
            
        w->SetFocus(); // restore focus
    }
    UpdateFlag = TempFlag;
    
}
void BearingDlg::SetSunBearing(wxDateTime t)
{
    bool TempFlag = UpdateFlag;
    wxWindow *w = FindFocus();
    if (Choice->GetSelection()==devSUN_BEARING) //SunBearing
            TrueBearingCtrl->ChangeValue( (wxString::Format(_T("%05.1f"), SolarAzimuth( GetDateTime(), localMesData->lat, localMesData->lon) ) ) );        
    if (Choice->GetSelection()==devSUN_SHADOW) //SunBearing shadow   
        TrueBearingCtrl->ChangeValue( (wxString::Format(_T("%05.1f"),  limit_degrees(SolarAzimuth( GetDateTime(), localMesData->lat, localMesData->lon )+180) ) ) );
    w->SetFocus(); // restore focus
    UpdateFlag = TempFlag;
}

void BearingDlg::SetNMEAHeading(double hd)
{
    bool TempFlag = UpdateFlag;
    
    if ( UpdateFlag){        
        wxWindow *w = FindFocus();
        CompassCourseCtrl->ChangeValue(wxString::Format(_T("%f"), hd));
        if((Choice->GetSelection() == 2)){ //if heading into ... compassbearing is same as compass course
            CompassBearingCtrl->ChangeValue(wxString::Format(_T("%f"), hd));
        }
        w->SetFocus(); // restore focus
    }
    UpdateFlag = TempFlag;
}

void BearingDlg::SetMessageVariation(wxString &message_id, wxString &message_body)
{
    if ( UpdateFlag){
        wxJSONReader r;
        wxJSONValue v;
        r.Parse(message_body, &v);
        
        //TODO check lat/lon to make sure it is our requested message and not  an otherone.
        // get the DECL value from the JSON message
        VariationCtrl->ChangeValue(  wxString::Format( _T("%05.1f"), v[_T("Decl")].AsDouble()) );
    }
}
void BearingDlg::SetMessageRouteActiveLeg(wxString &message_id, wxString &message_body)
{
    if ( UpdateFlag){
        wxJSONReader r;
        wxJSONValue v;
        r.Parse(message_body, &v);
        
        if ( v[_("error")].AsBool() )  // error means no active route avalable
        {
            if (Choice != NULL)          // then we remove the option from the choice control
                if(Choice->GetCount() > devBEARING_FROM_ROUTE )
                    Choice->Delete(devBEARING_FROM_ROUTE);
        }
        else // no error so there is an active route
        {
            if (Choice != NULL)
            {
                //If there was no active route choice available make it and select it.
                if (Choice->GetCount() <= devBEARING_FROM_ROUTE )
                {
                    Choice->Insert(_("Bearing from Route"), devBEARING_FROM_ROUTE);
                    Choice->SetSelection(devBEARING_FROM_ROUTE);
                }

                if ( Choice->GetSelection() == devBEARING_FROM_ROUTE )
                {
                    TrueBearingCtrl->ChangeValue( wxString::Format( _T("%05.1f"), v[_T("bearing")].AsDouble() ) );
                }
            }   
        }
    }   
}
void BearingDlg::GetMessageVariation(double lat, double lon)
{
    if ( (abs(lat) < 90.0) && (abs(lon) <= 180.0)  ) //valid position
    {        
        wxJSONValue v;
        v[_T("Lat")] = lat;
        v[_T("Lon")] = lon;
        v[_T("Year")] = GetDateTime().GetYear();
        v[_T("Month")] = GetDateTime().GetMonth();
        v[_T("Day")] = GetDateTime().GetDay();
        Dev_PI->RequestPluginMessage( _T("WMM_VARIATION_REQUEST"), v);   
    }
}

