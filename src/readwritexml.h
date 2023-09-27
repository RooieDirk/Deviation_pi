#include <wx/string.h>
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

#ifndef READWRITEXML_H
#define READWRITEXML_H

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "tinyxml.h"
#include "SharedStuff.h"
#include <vector> 

//using namespace tinyxml2;
using namespace std;

class ReadWriteXML : public wxObject
{
public:
    ReadWriteXML(compass_data* Data);
    ~ReadWriteXML();
    void WriteBearings(std::vector<Meassurement*> Ms);
    void WriteBearing(Meassurement* M);
    void ReadBearings(std::vector<Meassurement*> &Ms);
    void ReadBearing(Meassurement* &M, TiXmlElement* pNode);

    void DeleteAllBearingNodes();
    
    wxArrayString GetShipsList();
    wxArrayString GetCompassList();
    bool NeedsSaving;
   //wxXmlDocument xmlDoc;
   TiXmlDocument doc;
   wxString FileName;
   wxString ShipsName;
   wxString CompassName;
   TiXmlElement* p_rootnode;
   TiXmlNode* p_shipnode;
   TiXmlNode* p_compassnode;
    bool WriteNeeded;
    compass_data* data;
};

void ReadObjectsFromXML(compass_data* Data);
void WriteObjectsToXML(compass_data* Data );
#endif // READWRITEXML_H
