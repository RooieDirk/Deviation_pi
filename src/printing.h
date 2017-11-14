#include "SharedStuff.h"
#include "SharedStuff.h"
/////////////////////////////////////////////////////////////////////////////
// Name:        samples/printing.h
// Purpose:     Printing demo for wxWidgets
// Author:      Julian Smart
// Modified by:
// Created:     1995
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Define a new application
// class MyApp: public wxApp
// {
// public:
//     MyApp() {}
// 
//bool PrintOnInit() ;
//     virtual int OnExit() wxOVERRIDE;
// 
//     void Draw(wxDC& dc);
// 
//     void IncrementAngle()
//         { m_angle += 5; }
//     void DecrementAngle()
//         { m_angle -= 5; }
// 
//     wxFont& GetTestFont()
//         { return m_testFont; }
// 
// private:
//     int       m_angle;
//     wxBitmap  m_bitmap;
//     wxFont    m_testFont;
// };
// 
// wxDECLARE_APP(MyApp);

#ifndef PRINTING_H
#define PRINTING_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

#if !wxUSE_PRINTING_ARCHITECTURE
    #error "You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h, and recompile the library."
#endif

#include <ctype.h>
#include <wx/metafile.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/image.h>
#include <wx/accel.h>
#include <wx/defs.h> 

#if wxUSE_POSTSCRIPT
    #include "wx/generic/printps.h"
    #include "wx/generic/prntdlgg.h"
#endif

#if wxUSE_GRAPHICS_CONTEXT
    #include "wx/graphics.h"
#endif

#ifdef __WXMAC__
    #include "wx/osx/printdlg.h"
#endif

#include "SharedStuff.h"

bool PrintOnInit() ;
int PrintOnExit();

class MyCanvas;

// Define a new canvas and frame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnAngleUp(wxCommandEvent& event);
    void OnAngleDown(wxCommandEvent& event);

    void OnPrint(wxCommandEvent& event);
    void OnPrintPreview(wxCommandEvent& event);
    void OnPageSetup(wxCommandEvent& event);
#if wxUSE_POSTSCRIPT
    void OnPrintPS(wxCommandEvent& event);
    void OnPrintPreviewPS(wxCommandEvent& event);
    void OnPageSetupPS(wxCommandEvent& event);
#endif
#ifdef __WXMAC__
    void OnPageMargins(wxCommandEvent& event);
#endif

    void OnPreviewFrameModalityKind(wxCommandEvent& event);

    void OnExit(wxCommandEvent& event);
    void OnPrintAbout(wxCommandEvent& event);

private:
    MyCanvas* m_canvas;
    wxPreviewFrameModalityKind m_previewModality;

    wxDECLARE_EVENT_TABLE();
};

// Define a new white canvas
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxFrame *frame, const wxPoint& pos, const wxSize& size, long style = wxRETAINED);

    //void OnPaint(wxPaintEvent& evt);
    virtual void OnDraw(wxDC& dc);// wxOVERRIDE;

private:
    wxDECLARE_EVENT_TABLE();
};

// Defines a new printout class to print our document
class MyPrintout: public wxPrintout
{
public:
    MyPrintout( compass_data *Data, const wxString &title = wxT("My printout") )
        : wxPrintout(title) { data = Data; }

    virtual bool OnPrintPage(int page);// wxOVERRIDE;
    virtual bool HasPage(int page);// wxOVERRIDE;
    virtual bool OnBeginDocument(int startPage, int endPage);// wxOVERRIDE;
    virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);// wxOVERRIDE;

    void DrawPageOne();
    void DrawPageTwo();

    // Writes a header on a page. Margin units are in millimetres.
    bool WritePageHeader(wxPrintout *printout, wxDC *dc, const wxString& text, float mmToLogical);

private:
    compass_data* data;
};


// constants:
enum
{
    WXPRINT_PAGE_SETUP = 103,

    WXPRINT_PRINT_PS,
    WXPRINT_PAGE_SETUP_PS,
    WXPRINT_PREVIEW_PS,

    WXPRINT_ANGLEUP,
    WXPRINT_ANGLEDOWN,

#ifdef __WXMAC__
    WXPRINT_PAGE_MARGINS,
#endif

    WXPRINT_FRAME_MODAL_APP,
    WXPRINT_FRAME_MODAL_WIN,
    WXPRINT_FRAME_MODAL_NON
};
#endif //PRINTING_H
