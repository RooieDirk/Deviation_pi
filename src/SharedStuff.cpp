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

#include "SharedStuff.h"
#include "icons.h"

Meassurement::Meassurement ()
{
    compasscourse=0.0;
    compassbearing=0.0;
    truebearing=0.0;
    variation=0.0;
    deviation = 0.0;
    datetime=wxDateTime::Now();
    lat=91.0;
    lon=181.0;
    methode=0;
    enabled=true;
}

Meassurement::~Meassurement ()
{    
}

compass_data::compass_data(wxString sname, wxString cname)
{
    shipsname = sname;
    compassname = cname;
    A=0.0;
    B=0.0;
    C=0.0;
    D=0.0;
    E=0.0;
}

compass_data::~compass_data()
{}

ship_data::ship_data(wxString shipsname)
{}

ship_data::~ship_data()
{}

#include <stdio.h>
#include "spa.h"  //include the SPA header file

double SolarAzimuth( wxDateTime dt, double latitude, double longitude)
{
    spa_data spa;  //declare the SPA structure
    int result;
    float min, sec;

    //enter required input values into SPA structure

    spa.year          = dt.GetHour(0);
    spa.month         = dt.GetMonth(0);
    spa.day           = dt.GetDay(0);
    spa.hour          = dt.GetHour(0);
    spa.minute        = dt.GetMinute(0);
    spa.second        = dt.GetSecond(0);
    spa.timezone      = 0;
    spa.delta_ut1     = 0;
    spa.delta_t       = 67;
    spa.longitude     = longitude;
    spa.latitude      = latitude;
    spa.elevation     = 2.0;
    spa.pressure      = 1000;
    spa.temperature   = 20;
    spa.slope         = 30;
    spa.azm_rotation  = -10;
    spa.atmos_refract = 0.5667;
    spa.function      = SPA_ZA;  //calculate zenith and azimuth

    //call the SPA calculate function and pass the SPA structure

    result = spa_calculate(&spa);

    if (result == 0)  //check for SPA errors
    {
        return spa.azimuth;
        //display the results inside the SPA structure
    

    } else printf("SPA Error Code: %d\n", result);

    return 0;
}

void DrawTextCenterCenter(wxDC &dc, wxString text, wxPoint pos)
{
    int width, height;
    dc.GetTextExtent(text, &width, &height);
    dc.DrawText(text, pos.x- (int) width/2, pos.y - (int) height/2 );
}

double least(double a, double b)
{
    if ( a < b )
        return a;
    else 
        return b;
}

void DoRender(wxDC &dc, compass_data* Data )
{
   compass_data* data = Data;
//     wxBitmap* LogoBitmap = new wxBitmap(100,100,8);
//     LogoBitmap->LoadFile(_("logoOpenCPN.bmp"));
    //some SetDefaults 
    
    int j;
    wxCoord w, h;
    dc.GetSize(&w, &h);
    double scaleX = (double)(w-10) / (double)(848.0 );
    double scaleY = (double)(h-10) / (double)(600.0 );

    dc.SetUserScale(least(scaleX, scaleY), least(scaleX, scaleY));

    int CenterY = (int) 848 / 2;
    int BorderLR = 10;
    int LeftLeftPage = BorderLR;
    int RightLeftPage = CenterY - BorderLR;
    int LeftPageWidth = RightLeftPage - LeftLeftPage;
    dc.SetFont(*wxNORMAL_FONT);
    wxFont stdFont= dc.GetFont();
    wxFont stdFontSm= stdFont;
    stdFontSm.MakeSmaller();
    wxFont ShipFont = stdFont;
    ShipFont.SetPointSize( stdFont.GetPointSize()*2 );

    dc.SetFont(ShipFont); 
    DrawTextCenterCenter(dc, data->shipsname, wxPoint(300 ,30));
   
    dc.SetFont(stdFont);
    DrawTextCenterCenter(dc, data->compassname, wxPoint(300,55));
    DrawTextCenterCenter(dc, _("Compass\nCourse"), wxPoint(52,20));
    DrawTextCenterCenter(dc, _("Deviation"), wxPoint(137,20));
    dc.SetFont(stdFontSm);

//draw table
    dc.DrawLine(wxPoint(20,35), wxPoint(170,35));
    double DevMin = 0;
    double DevMax = 0;
    double Dev;
    for (int i=0; i<37; i++)
    {
        if ( i % 3 == 0)
            dc.SetFont(stdFontSm.Italic());
       
        DrawTextCenterCenter(dc,        
            wxString::Format(wxT("%03i°"),i*10), wxPoint(52, 40+15*i));
        Dev =  GetDeviation(i*10.0, data);
        DrawTextCenterCenter( dc, wxString::Format(wxT("%0.1f°"), Dev), wxPoint(137, 40+15*i) );
        dc.SetFont(stdFontSm);
        if ( Dev < DevMin ) DevMin = Dev; //Keep min/max value for graph scaling
        if ( Dev > DevMax ) DevMax = Dev;
        j = i;
    }

    dc.DrawLine(wxPoint(95,35), wxPoint(95,50+15*j));    
//draw graph

    int pixel_zero;
    double  pixel_Deg;
    double ScaleSteps[] = {0.02, 0.05, 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100, 200};
    double bestScaleStep;
    if (( DevMax != DevMin )|| (Data->vec.size() < 2)) //is there some data available to draw
    {    
        int i=0;
        while (ScaleSteps[i] < (DevMax - DevMin)/10) // for 10 scalesteps
            i++; //todo prevent overrun
        bestScaleStep = ScaleSteps[i];    
        
        pixel_Deg = 200/(DevMax - DevMin);
        int ScaleStepPix = (int)(pixel_Deg / bestScaleStep);      
        
        pixel_zero = 200 - (int)( DevMin*pixel_Deg );    
        dc.DrawLine(wxPoint(200,95), wxPoint(400,95)); //Top;ine
        dc.DrawLine(wxPoint(200,95+360), wxPoint(400,95+360)); //Bottomline
        dc.DrawLine(wxPoint(pixel_zero,95), wxPoint(pixel_zero,95+360)); //zero axis
        //draw messure points
        for (size_t m = 0; m < Data->vec.size(); m++)
        {
        if (Data->vec[m]->enabled)
                dc.DrawCircle( (wxCoord) (Data->vec[m]->deviation*pixel_Deg +pixel_zero), (wxCoord) (Data->vec[m]->compasscourse + 95 ), (wxCoord) 2);
    
        }

        int scale_min = (int) DevMin / bestScaleStep;
        int scale_max = (int) DevMax / bestScaleStep;
        for(int c=scale_min;c<=scale_max; c++) //draw scale points
        {
            dc.DrawLine(wxPoint(c * bestScaleStep * pixel_Deg +pixel_zero,93), wxPoint(c * bestScaleStep * pixel_Deg +pixel_zero,95));
            if ( c % 2 == 0)
            {
                if ( bestScaleStep >= 1.0 )
                    DrawTextCenterCenter( dc, wxString::Format(wxT("%3.0f°"), c*bestScaleStep ), wxPoint(c * bestScaleStep * pixel_Deg +pixel_zero, 85) );
                else
                    DrawTextCenterCenter( dc, wxString::Format(wxT("%4.1f°"), c*bestScaleStep ), wxPoint(c * bestScaleStep * pixel_Deg +pixel_zero, 85) );
            }
            for ( int cc =30; cc < 360;cc=cc + 30 )
            {
                dc.DrawLine(wxPoint(c * bestScaleStep * pixel_Deg +pixel_zero, cc + 94), 
                            wxPoint(c * bestScaleStep * pixel_Deg +pixel_zero, cc + 96));
                dc.DrawLine(wxPoint(c * bestScaleStep * pixel_Deg +pixel_zero-1, cc+95), 
                            wxPoint(c * bestScaleStep * pixel_Deg +pixel_zero+1, cc+95));
                if (c == 0)
                    DrawTextCenterCenter( dc, wxString::Format(wxT("%03i°"), cc), wxPoint(pixel_zero+15, cc+95 ));
            }     
            
        }
    }

    wxPoint Prev, Curr;
    Prev = wxPoint((int) (GetDeviation(0.0, data)*pixel_Deg) + pixel_zero, 95);
    for (int ii=1; ii<361; ii++)
    {
        Curr = wxPoint( (int) (GetDeviation( ii*1.0, data )*pixel_Deg +pixel_zero), 95+ii );
        dc.DrawLine( Prev, Curr);
        Prev = Curr;
    }
    //draw signature
    dc.DrawBitmap(*_img_logoOpenCPN, wxPoint(300, 490));
    dc.DrawText(wxString::Format(wxT("Date: %02i-%02i-%04i "),wxDateTime::Now().GetDay(), wxDateTime::Now().GetMonth()+1, wxDateTime::Now().GetYear()), 200,500 );
    dc.DrawText(wxT("Signature:.............................."), 200,560 );
       
    //draw backside
    dc.DrawLine( 468, 20, 818, 20 );
    dc.DrawLine( 818, 20, 818, 580 );
    dc.DrawLine( 818, 580, 468, 580 );
    dc.DrawLine( 468, 580, 468, 20 );
    dc.DrawText(wxT("Deviation terms according formula:"), 480, 40 );
    dc.DrawText(wxT("Dev=A + B.sin(CC) + C.cos(CC) + D.sin(2.CC) + E.cos(2.CC)"), 480, 60 );
    dc.DrawText(wxT("where:"),480, 80 );
    dc.DrawText(wxT("CC = Compass Course"), 500, 100 );
    dc.DrawText(wxString::Format(wxT("A = %3f (fixed alignment error)"), Data->A), 500, 120);
    dc.DrawText(wxString::Format(wxT("B = %3f (abeam fixed magnetic error)"), Data->B),500, 140);
    dc.DrawText(wxString::Format(wxT("C = %3f (fore and aft fixed magnetic error)"), Data->C), 500, 160);
    dc.DrawText(wxString::Format(wxT("D = %3f (abeam induced magnetic error)"), Data->D), 500, 180);
    dc.DrawText(wxString::Format(wxT("E = %3f (fore and aft induced magnetic error)"), Data->E), 500, 200);
    
    dc.DrawText(wxT("©R.D.teE."), 470, 565);
    // draw some text


wxFont WarningFont;
WarningFont.SetPointSize( stdFont.GetPointSize()*3 );

if ( Data->vec.size() < 6 )
    {
        dc.SetFont(WarningFont);
        DrawTextCenterCenter(dc, wxT("You don't have enough\n bearings entered \nConsider this output as \nNOT valid"), wxPoint(300, 300) );
        dc.SetFont(stdFont);
    }
   
}

double CalcSqueredDev(compass_data* Data)
{//a simple brute force curve fitting as we don't have to many data points (least squares)
    double val=100;
    double A=0, B=0, C=0, D=0, E=0;
    double minA=0, minB=0, minC=0, minD=0, minE=0;
    double Answ, minAnsw;
    double RadCC, dRadCC , Dev;
    minAnsw = 1.7e308;

    while (val > 0.001)
    {
        for (int m_A=-1; m_A<2; m_A++)
        {
            for (int m_B=-1; m_B<2; m_B++)
            {
                for (int m_C=-1; m_C<2; m_C++)
                {
                    for (int m_D=-1; m_D<2; m_D++)
                    {
                        for (int m_E=-1; m_E<2; m_E++)
                        {
                            for(int i=0; i < Data->vec.size(); i++)
                            {                        
                                if (Data->vec[i]->enabled)
                                {
                                    RadCC = Data->vec[i]->compasscourse / 57.2957795131;
                                    dRadCC = 2*RadCC;
                                    Dev= Data->vec[i]->deviation;
                                    Answ = Answ + pow(Dev - ((m_A*val+A) + (m_B*val+B)*sin(RadCC) + (m_C*val+C)*cos(RadCC) + (m_D*val+D)*sin(dRadCC) + (m_E*val+E)*cos(dRadCC)), 2);                                
                                }
                            }
                            
                            if (Answ < minAnsw)
                            {
                                minAnsw = Answ;
                                minA=m_A*val+A; minB=m_B*val+B; minC=m_C*val+C; minD=m_D*val+D; minE=m_E*val+E;
                            }
                            Answ=0;
                        }
                    }
                }
            }
        }

        A=minA, B=minB, C=minC, D=minD, E=minE;
        val=val * 0.7;
        minAnsw = 1.7e308;
        Answ=0;
    }
    Data->A=A;
    Data->B=B;
    Data->C=C;
    Data->D=D;
    Data->E=E;
}

double GetDeviation(double CompassCourse,compass_data* Data)
{
    double RadCC, dRadCC , Dev;
    RadCC =  CompassCourse / 57.2957795131;
    dRadCC = 2*RadCC;
    Dev = Data->A + Data->B*sin(RadCC) + Data->C*cos(RadCC) + Data->D*sin(dRadCC) +  Data->E*cos(dRadCC);
    return Dev;
}

