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

#include "readwritexml.h"
#include <wx/filename.h>
#include "SharedStuff.h"

#ifndef XMLCheckResult
    #define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); }
#endif


ReadWriteXML::ReadWriteXML(compass_data* Data)
{
    //Will initialize and read the XML file. If the XML file is not available or broken it will make a new file.
    data = Data; //save ptr to struct
    
    //doc;
	if (!doc.LoadFile(data->filename.mb_str() ) )
    {
        //Something going wrong with opening the file. We make a backup copy and make a fresh file without any info.
        //wxMessageBox(wxString::Format("Error loading file.... %s",  data->filename ));
        wxCopyFile(data->filename, data->filename + wxT(".bck"));
        wxRemoveFile(data->filename);
        doc.Clear();
        TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF8", "" );
        doc.LinkEndChild( decl );
        data->needsaving = true; //doc.SaveFile( data->filename.mb_str() ); 
    }
    
    p_rootnode = doc.FirstChildElement();
    if(p_rootnode == NULL)
    {
        //No root node available, so we do make one
        //wxMessageBox(wxT("Failed to load file: No root element."));        
        p_rootnode = new TiXmlElement( "root" );  
        doc.LinkEndChild( p_rootnode );
        TiXmlComment * comment = new TiXmlComment();
        comment->SetValue(" Settings for the OpenCPN Compass Deviation Plugin " );  
        p_rootnode->LinkEndChild( comment );
        data->needsaving = true; //doc.SaveFile( data->filename.mb_str() ); 
    }
    
    if (data->shipsname == _(""))
    {
        p_shipnode = NULL;
        p_compassnode = NULL;
        data->needsaving = true; 
        return;
    }
    
    p_shipnode = p_rootnode->FirstChildElement( "aship" );
    while ( p_shipnode )
    {
        TiXmlElement* temp = p_shipnode->ToElement();
        if ( strcmp (temp->Attribute("shipsname"),data->shipsname.mb_str()) == 0)
            break;
        p_shipnode = p_shipnode->NextSibling("aship");
    }
    if( p_shipnode == NULL )
    {
        //No child with given name found so we have to make one.
        TiXmlElement* temp = new TiXmlElement( "aship" );
        temp->SetAttribute( "shipsname", data->shipsname.mb_str() );
        p_shipnode = temp->Clone();
        p_rootnode->LinkEndChild( p_shipnode );
        data->needsaving = true;
    }
    
    if (data->compassname == _(""))
    {
        p_compassnode = NULL;
        data->needsaving = true; 
        return;
    }
 
    p_compassnode = p_shipnode->FirstChildElement( "acompass" );
    while ( p_compassnode )
    {
        TiXmlElement* temp = p_compassnode->ToElement();
        if ( strcmp (temp->Attribute("compassname"),data->compassname.mb_str()) == 0)
            break;
        p_compassnode = p_compassnode->NextSibling("acompass");
    }
    if(p_compassnode == NULL)
    {
        //No child with given name found so we have to make one.
        TiXmlElement* temp = new TiXmlElement( "acompass" );
        temp->SetAttribute( "compassname", data->compassname.mb_str() );
        p_compassnode = temp->Clone();
        p_shipnode->LinkEndChild( p_compassnode );
        data->needsaving = true;
    }      
}

ReadWriteXML::~ReadWriteXML()
{
    
    //if (data->needsaving) 	doc.SaveFile( data->filename.mb_str() );
}

void ReadWriteXML::WriteBearings(std::vector<Meassurement*> Ms)
{
    DeleteAllBearingNodes();
    
    TiXmlElement* envS = new TiXmlElement( "EnvSettings" );
    p_compassnode->LinkEndChild(envS);
    envS->SetDoubleAttribute("SendNMEA", data->SendNMEA);
    envS->SetDoubleAttribute("ShowTool", data->ShowToolbarBtn);
    
    TiXmlElement* abcde = new TiXmlElement( "ABCDEvalues" );
    p_compassnode->LinkEndChild(abcde);
    abcde->SetDoubleAttribute("factorA", data->A);
    abcde->SetDoubleAttribute("factorB", data->B);
    abcde->SetDoubleAttribute("factorC", data->C);
    abcde->SetDoubleAttribute("factorD", data->D);
    abcde->SetDoubleAttribute("factorE", data->E);
    
    for (unsigned i=0; i < Ms.size(); i++) 
    {
        WriteBearing( Ms[i]);
    }
    
    doc.SaveFile( data->filename.mb_str() );
    data->needsaving = false;    
}

void ReadWriteXML::WriteBearing(Meassurement* M)
{
    TiXmlElement* temp = new TiXmlElement( "abearing" );
    TiXmlElement * pBearings = new TiXmlElement("aBearing");   
    p_compassnode->LinkEndChild(pBearings);
    
    pBearings->SetDoubleAttribute("CompassBearing", M->compassbearing);
    pBearings->SetDoubleAttribute("CompassCourse", M->compasscourse);
    pBearings->SetDoubleAttribute("TrueBearing", M->truebearing);
    pBearings->SetDoubleAttribute("Variation", M->variation);
    pBearings->SetDoubleAttribute("Deviation", M->deviation);
    pBearings->SetDoubleAttribute("Latitude", M->lat);
    pBearings->SetDoubleAttribute("Longitude", M->lon);
    //wxString s = M->datetime.FormatISOCombined();
    pBearings->SetAttribute("DateTime", M->datetime.FormatISOCombined().mb_str() );
    pBearings->SetAttribute("Methode", M->methode);
    pBearings->SetAttribute("Enabled", (int)M->enabled);
    pBearings->SetAttribute("Remarks", M->MeassurementRemarks.mb_str());
}

void ReadWriteXML::ReadBearings(std::vector<Meassurement*> &Ms)
{    
    if ( p_compassnode == NULL ) return;
    
    TiXmlElement* N = p_compassnode->FirstChildElement("EnvSettings");
    if ( N != NULL ){
        N->QueryBoolAttribute("SendNMEA", &data->SendNMEA);
        N->QueryBoolAttribute("ShowTool", &data->ShowToolbarBtn);
    }
    
    N = p_compassnode->FirstChildElement("ABCDEvalues");
    if ( N != NULL ){
        N->QueryDoubleAttribute("factorA", &data->A);
        N->QueryDoubleAttribute("factorB", &data->B);
        N->QueryDoubleAttribute("factorC", &data->C);
        N->QueryDoubleAttribute("factorD", &data->D);
        N->QueryDoubleAttribute("factorE", &data->E);
    }
    
    for (unsigned i=0; i < Ms.size(); i++) //Empty the vector with bearings
    {
        delete Ms[i];
    }
    Ms.clear();    
    N = p_compassnode->FirstChildElement("aBearing");
    while ( N != NULL)
    {
        Meassurement* me = new Meassurement();
        ReadBearing( me, N );
        Ms.push_back( me );
        N = N->NextSiblingElement();
    }
}
 void ReadWriteXML::ReadBearing(Meassurement* &M, TiXmlElement* pNode)
{
    pNode->QueryDoubleAttribute("CompassBearing", &M->compassbearing);
    pNode->QueryDoubleAttribute("CompassCourse", &M->compasscourse);
    pNode->QueryDoubleAttribute("TrueBearing", &M->truebearing);
    pNode->QueryDoubleAttribute("Variation", &M->variation);
    pNode->QueryDoubleAttribute("Deviation", &M->deviation);
    pNode->QueryDoubleAttribute("Latitude", &M->lat);
    pNode->QueryDoubleAttribute("Longitude", &M->lon);
    const char* s =  pNode->Attribute("DateTime");
    wxString mystring = wxString::FromUTF8(s);
    M->datetime.ParseISOCombined(mystring);
    pNode->QueryIntAttribute("Methode", &M->methode);
    int i;
    pNode->QueryIntAttribute("Enabled", &i);
    M->enabled =  (bool)i;
    std::string _value;
    pNode->QueryStringAttribute("Remarks", &_value);
        wxString temp(_value);
        M->MeassurementRemarks= temp;

    //(_value);//.c_str(), wxConvUTF8);
    //wxString mystring(stlstring.c_str(), wxConvUTF8);
}

void ReadWriteXML::DeleteAllBearingNodes()
{
    if ( p_compassnode != NULL)
    {
        TiXmlNode* temp = p_compassnode->LastChild();
        while ( temp != NULL )
        {
            p_compassnode->RemoveChild( temp );
            temp = p_compassnode->LastChild();
        }
        data->needsaving = true;
    }
}



wxArrayString ReadWriteXML::GetShipsList()
{
    wxArrayString Ships;
    if ( p_rootnode == NULL) return Ships;
    
    TiXmlNode* p_Ship = p_rootnode->FirstChildElement( "aship" ); //Get the first ship node, if not exist return null
    while ( p_Ship )  //iterate over 
    {
        TiXmlElement* temp = p_Ship->ToElement();
        if ( temp != NULL )
            Ships.Add(  wxString::FromUTF8( temp->Attribute("shipsname")) );
        p_Ship = p_Ship->NextSibling();
    }
    return Ships;
}

wxArrayString ReadWriteXML::GetCompassList()
{
    wxArrayString CompassL;
    if ( p_shipnode == NULL) return CompassL;
    TiXmlNode* p_Compas = p_shipnode->FirstChildElement( "acompass" ); //Get the first ship node, if not exist return null
    while ( p_Compas )  //iterate over 
    {
        TiXmlElement* temp = p_Compas->ToElement();
        if ( temp != NULL )
            CompassL.Add(  wxString::FromUTF8( temp->Attribute("compassname")) );
        p_Compas = p_Compas->NextSibling();
    }
    return CompassL;
}

void ReadObjectsFromXML(compass_data* Data)
{
    if (( Data->shipsname != wxEmptyString ) && ( Data->compassname != wxEmptyString ))        
    {
        ReadWriteXML* a = new ReadWriteXML( Data);
        a->ReadBearings(Data->vec);
        delete a;         
    } 
}

void WriteObjectsToXML(compass_data* Data )
{
    if (( Data->shipsname != wxEmptyString ) && ( Data->compassname != wxEmptyString ) && ( Data->needsaving ))        
    {
        ReadWriteXML* a = new ReadWriteXML(Data);
        a->WriteBearings(Data->vec);
        Data->needsaving = false;
        delete a;
        //wxPuts(_("WriteObjectsToXML"));
    }    
}
