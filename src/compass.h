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

#ifndef COMPASS_H
#define COMPASS_H

#include "SharedStuff.h"
#include "readwritexml.h"
class Compass: public wxObject
{
public:
    Compass(wxString FName, wxString SName, wxString CName);
    ~Compass();
//     void ReadObjectsFromXML();
//     void WriteObjectsToXML();
    void ShowDlg();
 //   double GetDeviation(double CompassCourse);
    double CalcSqueredDev();
    std::vector<Meassurement*> ListOfMs;
    //wxXmlDocument xmlDoc;
    //wxString FileName, ShipsName, CompassName;

    //double d_A, d_B, d_C, d_D, d_E;
    compass_data* data;
};


#endif // COMPASS_H
