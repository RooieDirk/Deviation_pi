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

#ifndef  SHAREDSTUFF_H
#define  SHAREDSTUFF_H

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <vector>
#include "spa.h"
#include "ocpn_plugin.h"
class compass_data;
class ship_data;

//PlugIn_Position_Fix_Ex plugin_pfix_ex;

class Meassurement : public wxObject
{
public:
    Meassurement ();
    ~Meassurement();
    double compasscourse;
    double compassbearing;
    double truebearing;
    double variation;
    double deviation;
    wxDateTime datetime;
    double lat;
    double lon;
    int methode;
    bool enabled;
    compass_data* c_data;
    wxString Choise1 = _("Relative bearing (Righthand bearing)");
	wxString Choise2 = _("Bearing using \'Navigate to\'");
	wxString Choise3 = _("Steaming into leading line");
	wxString Choise4 = _("Sun bearing");
	wxString Choise5 = _("Sun bearing shadowline");    
};

class compass_data : public wxObject //typedef struct
{
public:
    compass_data(wxString sname, wxString cname);
    ~compass_data();
    wxString filename;
    wxString shipsname;
    wxString compassname;
    double A;
    double B;
    double C;
    double D;
    double E;
    std::vector<Meassurement*> vec;
    bool needsaving;
};// compass_data;

class ship_data : public wxObject
{
public:
    ship_data(wxString shipsname);
    ~ship_data();
    std::vector<compass_data*> cdata;   
    
};

// class BaseCompassDlg : public wxDialog
// {
// public:
//     BaseCompassDlg(compass_data* Data=NULL);
//     BaseCompassDlg(wxWindow* parent, compass_data* Data=NULL);
//     BaseCompassDlg(wxWindow* parent, wxWindowID id, compass_data* Data=NULL);
//     BaseCompassDlg(wxWindow* parent, wxWindowID id, const wxString &title, const wxPoint &pos = wxDefaultPosition,  long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxDialogNameStr ,compass_data* Data=NULL);
//     ~BaseCompassDlg();
// private:
//     compass_data* data;
//    // wxDECLARE_DYNAMIC_CLASS(BaseCompassDlg);
//     //wxDECLARE_EVENT_TABLE();
// };
//-----------------------------------------------------------------------------
// BaseCompassDlg
//-----------------------------------------------------------------------------




void DoRender(wxDC &dc, compass_data* Data );
double CalcSqueredDev( compass_data* Data);
double GetDeviation(double CompassCourse, compass_data* Data);
double SolarAzimuth( wxDateTime dt, double latitude, double longitude);


#endif //  SHAREDSTUFF_H
