/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2017  Dirk Smits <email>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#include "compass.h"
#include "deviation_pi.h"
Compass::Compass(wxString FName, wxString SName, wxString CName)
{
    data = new compass_data(SName, CName );
    
    data->filename = FName;
    ReadObjectsFromXML(data);
}
Compass::~Compass()
{}

void Compass::ShowDlg()
{
    wxString name = data->shipsname + _(" ") + data->compassname;
    CompasDev1Dialog* Dlg = new CompasDev1Dialog( (wxWindow*)0, name, data);
    Dlg->Show();
}
