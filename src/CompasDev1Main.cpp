/***************************************************************
 * Name:      CompasDev1Main.cpp
 * Purpose:   Code for Application Frame
 * Author:    Dirk Smits ()
 * Created:   2017-05-16
 * Copyright: Dirk Smits ()
 * License:
 **************************************************************/

#include "CompasDev1Main.h"
//#include "DevDialog.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(CompasDev1Dialog)
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/imaglist.h>
#include <wx/renderer.h>
#include <wx/sstream.h>
#include <math.h> 
#include <stdio.h>      /* printf */
#include <vector>

#include "readwritexml.h"
#include "SharedStuff.h"
#include "spa.h"
#include "printing.h"
#include "icons.h"

using namespace std;

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

// Global print data, to remember settings during the session
//wxPrintData *g_printData = NULL;
extern wxPrintData *g_printData;
extern wxPageSetupDialogData* g_pageSetupData;
BearingDlg* B_Dlg;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//(*IdInit(CompasDev1Dialog)
const long CompasDev1Dialog::ID_LISTCTRLDEV = wxNewId();
const long CompasDev1Dialog::ID_ADDBUTTON = wxNewId();
const long CompasDev1Dialog::ID_DELBUTTON = wxNewId();
const long CompasDev1Dialog::ID_EDITBUTTON = wxNewId();
const long CompasDev1Dialog::ID_BUTTONSHOW = wxNewId();
const long CompasDev1Dialog::ID_BUTTONCANCEL = wxNewId();
const long CompasDev1Dialog::ID_BUTTONCLOSE = wxNewId();
//*)

BEGIN_EVENT_TABLE(CompasDev1Dialog,wxFrame)
    //(*EventTable(CompasDev1Dialog)
    //*)
END_EVENT_TABLE()

CompasDev1Dialog::CompasDev1Dialog( wxWindow *parent,
            const wxString &title,
            compass_data* Data )
{
    data = Data;
    DT_Dlg = NULL;
    wxPuts(_("CompasDev1Dialog") + data->shipsname);
    //(*Initialize(CompasDev1Dialog)
    wxFlexGridSizer* FlexGridSizer;
    wxBoxSizer* BoxSizerButons;

    Create(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxCLOSE_BOX, _T("id"));
    FlexGridSizer = new wxFlexGridSizer(0, 1, 0, 0);
    DevListCtrl = new wxListCtrl(this, ID_LISTCTRLDEV, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRLDEV"));
    //DevListCtrl->SetMinSize(wxSize(0,150));
    FlexGridSizer->Add(DevListCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizerButons = new wxBoxSizer(wxHORIZONTAL);
    AddBtn = new wxButton(this, ID_ADDBUTTON, wxT("&Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_ADDBUTTON"));
    BoxSizerButons->Add(AddBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    DeleteBtn = new wxButton(this, ID_DELBUTTON, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DELBUTTON"));
    DeleteBtn->Disable();
    BoxSizerButons->Add(DeleteBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    EditBtn = new wxButton(this, ID_EDITBUTTON, wxT("&Edit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_EDITBUTTON"));
    EditBtn->Disable();
    BoxSizerButons->Add(EditBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ShowBtn = new wxButton(this, ID_BUTTONSHOW, wxT("Show"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONSHOW"));    
    BoxSizerButons->Add(ShowBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    CancelBtn = new wxButton(this, ID_BUTTONCANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONCANCEL"));
    BoxSizerButons->Add(CancelBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    CloseBtn = new wxButton(this, ID_BUTTONCLOSE, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONCLOSE"));
    BoxSizerButons->Add(CloseBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer->Add(BoxSizerButons, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer);
    FlexGridSizer->Fit(this);
    FlexGridSizer->SetSizeHints(this);
    
    DevListCtrl->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(CompasDev1Dialog::OnConnectionToggleEnableMouse), NULL, this);
    DevListCtrl->Connect(ID_LISTCTRLDEV, wxEVT_RIGHT_UP, wxMouseEventHandler(CompasDev1Dialog::OnRightDown), NULL ,this );
    Connect(ID_LISTCTRLDEV,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&CompasDev1Dialog::OnDevListCtrlItemSelect);
    Connect(ID_LISTCTRLDEV,wxEVT_COMMAND_LIST_ITEM_DESELECTED,(wxObjectEventFunction)&CompasDev1Dialog::OnDevListCtrlItemDeselect);
    Connect(ID_LISTCTRLDEV,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,(wxObjectEventFunction)&CompasDev1Dialog::OnDevListCtrlItemActivated);
    Connect(ID_ADDBUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnAddBtnClick);
    Connect(ID_DELBUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnDeleteBtnClick);
    Connect(ID_EDITBUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnEditBtnClick);
    Connect(ID_BUTTONSHOW,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnShowBtnClick);
    Connect(ID_BUTTONCANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnCancelBtnClick);
    Connect(ID_BUTTONCLOSE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompasDev1Dialog::OnOKBtnClick);
    Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&CompasDev1Dialog::OnRightDown);
    Connect(wxEVT_SIZE,(wxObjectEventFunction)&CompasDev1Dialog::OnResize);
    
     wxString columns[] = {wxT("Do\nuse"),   wxT("Date/Time"), wxT("Compass\nCourse"),   wxT("Compass\nBearing"), wxT("True\nBearing"), wxT("Var."), wxT("Dev."), wxT("")};
    for (int i = 0; i < 8; ++i) {
        wxListItem col;
        col.SetId(i);
        col.SetText(columns[i]);
        DevListCtrl->InsertColumn(i, col);
    }

    //  Build the image list
    wxImageList* imglist = new wxImageList(16, 16, TRUE, 1);
    wxBitmap unchecked_bmp(16, 16), checked_bmp(16, 16);
    wxMemoryDC renderer_dc;

    // Unchecked
    renderer_dc.SelectObject(unchecked_bmp);
    renderer_dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(
        GetBackgroundColour(), wxBRUSHSTYLE_SOLID));
    renderer_dc.Clear();
    wxRendererNative::Get().DrawCheckBox(this, renderer_dc, wxRect(0, 0, 16, 16), 0);

    // Checked
    renderer_dc.SelectObject(checked_bmp);
    renderer_dc.SetBackground(*wxTheBrushList->FindOrCreateBrush(
        GetBackgroundColour(), wxBRUSHSTYLE_SOLID));
    renderer_dc.Clear();
    wxRendererNative::Get().DrawCheckBox(this, renderer_dc, wxRect(0, 0, 16, 16), wxCONTROL_CHECKED);

    // Deselect the renderer Object
    renderer_dc.SelectObject(wxNullBitmap);

    imglist->Add(unchecked_bmp);
    imglist->Add(checked_bmp);
    DevListCtrl->AssignImageList(imglist, wxIMAGE_LIST_SMALL);

    DevListCtrl->Refresh();
    
    FillSourceList();
}

CompasDev1Dialog::~CompasDev1Dialog()
{
    //(*Destroy(CompasDev1Dialog)
    //*)
}
// void CompasDev1Dialog::OnPreviewFrameModalityKind(wxCommandEvent& event)
// {
//     m_previewModality = static_cast<wxPreviewFrameModalityKind>(
//                             wxPreviewFrame_AppModal +
//                                 (event.GetId() - WXPRINT_FRAME_MODAL_APP));
// }

void CompasDev1Dialog::OnAddBtnClick(wxCommandEvent& event)
{    
    Meassurement* NewMess = new Meassurement();
    B_Dlg =  new BearingDlg(this, NewMess, wxID_ANY, wxDefaultPosition, wxDefaultSize, false);
    if ( B_Dlg->ShowModal() == wxID_OK )
    {        
        data->vec.push_back( NewMess );
        CalcSqueredDev(data);
        FillSourceList();
        UpdateWindows();
    };
    delete B_Dlg; 
    B_Dlg = NULL;
}

void CompasDev1Dialog::OnDeleteBtnClick(wxCommandEvent& event)
{
    delete data->vec[listselindex];
    data->vec.erase( data->vec.begin() + listselindex );
    FillSourceList();
}

void CompasDev1Dialog::OnEditBtnClick(wxCommandEvent& event)
{
    B_Dlg =  new BearingDlg(this, data->vec[listselindex], wxID_ANY,  wxDefaultPosition, wxDefaultSize, false);
    //CopyMessObjToDlg(data->vec[listselindex], B_Dlg);
    if ( B_Dlg->ShowModal() == wxID_OK )
    {
        

        CalcSqueredDev(data);
        FillSourceList();
        UpdateWindows();
    }; 
    delete B_Dlg;
    B_Dlg = NULL;
}

void CompasDev1Dialog::OnShowBtnClick(wxCommandEvent& event)
{
    if ( DT_Dlg == NULL ){
        DT_Dlg = new DevTableDialog(this, wxNewId(), data ) ;
        DT_Dlg->Show(true);
    }
    else
         DT_Dlg->Show( !DT_Dlg->IsShown() );

}

void CompasDev1Dialog::OnCancelBtnClick(wxCommandEvent& event)
{
    Show(false);
}

void CompasDev1Dialog::OnOKBtnClick(wxCommandEvent& event)
{
    WriteObjectsToXML(data);
    Show(false);
}

void CompasDev1Dialog::OnDevListCtrlItemSelect(wxListEvent& event)
{
    EditBtn->Enable();
    DeleteBtn->Enable();
}
void CompasDev1Dialog::OnDevListCtrlItemDeselect(wxListEvent& event)
{
    EditBtn->Disable();
    DeleteBtn->Disable();
}

void CompasDev1Dialog::OnDevListCtrlItemActivated(wxListEvent& event)
{
}

void CompasDev1Dialog::OnConnectionToggleEnableMouse(wxMouseEvent& event) {
  int flags;
  long index = DevListCtrl->HitTest(event.GetPosition(), flags);
  if (index == wxNOT_FOUND || event.GetX() < DevListCtrl->GetColumnWidth(0))
    EnableItem(index);
  listselindex = index;
  // Allow wx to process...
  event.Skip();
}

void CompasDev1Dialog::OnRightDown(wxMouseEvent& event)
{
    PreferenceDlg* pref = new PreferenceDlg(NULL, wxID_ANY, data );
    if ( pref->ShowModal() )
    {
        data->shipsname = pref->ShipsnameBox->GetValue();
        data->compassname = pref->CompassnameBox->GetValue();
        ReadObjectsFromXML(data);
        FillSourceList();
        CalcSqueredDev(data);
    }
    
}
void CompasDev1Dialog::OnResize(wxSizeEvent& event)
{
    Layout();
    //FlexGridSizer->SetSizeHints(this);
}


void CompasDev1Dialog::EnableItem(const long index) {
  if (index == wxNOT_FOUND) {
//     ClearNMEAForm();
//     m_buttonRemove->Disable();
  } else {
    Meassurement* Mess = data->vec[index];
    if (!Mess) return;
    Mess->enabled = !Mess->enabled;
    
    //m_connection_enabled = conn->bEnabled;
    // Mark as changed
    //conn->b_IsSetup = FALSE;
    DevListCtrl->SetItemImage(index, Mess->enabled);
    UpdateWindows();
  }
}


void CompasDev1Dialog::FillSourceList(void) {
//  m_buttonRemove->Enable(FALSE);
  DevListCtrl->DeleteAllItems();

  for (size_t i = 0; i < data->vec.size(); i++) {
    wxListItem li;
    li.SetId(i);
    li.SetImage( data->vec[i]->enabled);
    li.SetData(i);
    li.SetText(wxEmptyString);
    long itemIndex = DevListCtrl->InsertItem(li);

    DevListCtrl->SetItem(itemIndex, 1,  data->vec[i]->datetime.Format(wxT("%x %H:%M") ));//, wxDateTime::UTC).c_str());
    wxString tmp =  wxString::Format(_T( "%05.1f" ), data->vec[i]->compasscourse );
    DevListCtrl->SetItem(itemIndex, 2, tmp);
    tmp =  wxString::Format(_T( "%05.1f" ), data->vec[i]->compassbearing );
    DevListCtrl->SetItem(itemIndex, 3, tmp);
    tmp =  wxString::Format(_T( "%05.1f" ), data->vec[i]->truebearing );
    DevListCtrl->SetItem(itemIndex, 4, tmp);
    tmp =  wxString::Format(_T( "%05.1f" ), data->vec[i]->variation );
    DevListCtrl->SetItem(itemIndex, 5, tmp);
    tmp =  wxString::Format(_T( "%05.1f" ), data->vec[i]->deviation );
    DevListCtrl->SetItem(itemIndex, 6, tmp);

  DevListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
  DevListCtrl->SetColumnWidth(1, wxLIST_AUTOSIZE);
  DevListCtrl->SetColumnWidth(2, wxLIST_AUTOSIZE);
  DevListCtrl->SetColumnWidth(3, wxLIST_AUTOSIZE);
  DevListCtrl->SetColumnWidth(4, wxLIST_AUTOSIZE);
  DevListCtrl->SetColumnWidth(5, wxLIST_AUTOSIZE);
  DevListCtrl->SetColumnWidth(6, wxLIST_AUTOSIZE);
  }
}
void CompasDev1Dialog::UpdateWindows(void)
{
    CalcSqueredDev(data);
    if ( DT_Dlg != NULL )
        if ( DT_Dlg->IsShown() ){
            DT_Dlg->Refresh();
        }
        
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////


//(*IdInit(DevTableDialog)
const long DevTableDialog::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)

EVT_PAINT(BasicDrawPane::paintEvent)
 
END_EVENT_TABLE()

BasicDrawPane::BasicDrawPane(DevTableDialog* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size,
                long style, const wxString& name, compass_data* Data) : wxPanel(parent)
{
    parentdlg = parent;
    
    data = Data;
    wxPuts(_("BasicDrawPane::BasicDrawPane") );
    wxPuts(data->shipsname );
}
BasicDrawPane::~BasicDrawPane()
{
}
  
void BasicDrawPane::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    DoRender(dc, data);
}
 
void BasicDrawPane::paintNow()
{
    wxClientDC dc(this);
    DoRender(dc, data);
}
void BasicDrawPane::DrawTextCenterCenter(wxDC&  dc, wxString text, wxPoint pos)
{
    int width, height;
    dc.GetTextExtent(text, &width, &height);
    dc.DrawText(text, pos.x- (int) width/2, pos.y - (int) height/2 );
}


BEGIN_EVENT_TABLE(DevTableDialog,wxFrame)
    //(*EventTable(DevTableDialog)
    //*)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////////////////////////////////

const long DevTableDialog::ID_PRINT = wxNewId();
const long DevTableDialog::ID_PRINTPREV = wxNewId();
const long DevTableDialog::ID_CLOSE = wxNewId();

DevTableDialog::DevTableDialog(wxWindow* parent,wxWindowID id, compass_data* Data)
{
    data = Data;
   
    //(*Initialize(DevTableDialog)
    wxFlexGridSizer* FlexGridSizer1;
    
    wxBoxSizer* BoxSizerButons;
    Create(parent, wxID_ANY, _("Deviation Table"), wxDefaultPosition, wxSize(848, 600), wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    FlexGridSizer1 = new wxFlexGridSizer(2, 1, 0, 0);
    Panel1 = new BasicDrawPane(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"), data);
    
    Panel1->SetMinSize(wxSize(848, 600));
    FlexGridSizer1->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);

    BoxSizerButons = new wxBoxSizer(wxHORIZONTAL);
    CloseBtn = new wxButton(this, ID_CLOSE, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CLOSE"));
    PrintBtn = new wxButton(this, ID_PRINT, wxT("Print"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_PRINT"));
    PrintPrevBtn = new wxButton(this, ID_PRINTPREV, wxT("Print Preview"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_PRINTPREV"));
    BoxSizerButons->Add(PrintPrevBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizerButons->Add(PrintBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);    
    BoxSizerButons->Add(CloseBtn, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    FlexGridSizer1->Add(BoxSizerButons, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);
    Connect(ID_PRINTPREV,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DevTableDialog::OnPrintPreviewBtnClick);
    Connect(ID_PRINT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DevTableDialog::OnPrintBtnClick);
    Connect(ID_CLOSE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DevTableDialog::OnCloseBtnClick);
    
wxPuts(_("end init DevTableDialog"));
}

DevTableDialog::~DevTableDialog()
{
    //(*Destroy(DevTableDialog)
    //*)
}

void DevTableDialog::OnPrintBtnClick(wxCommandEvent& event)
{
    PrintOnInit();
    
     wxPrintDialogData printDialogData(* g_printData);

    wxPrinter printer(&printDialogData);
    MyPrintout printout(data);//this, wxT("My printout"));
    if (!printer.Print(this, &printout, true /*prompt*/))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
        {
            wxLogError(wxT("There was a problem printing. Perhaps your current printer is not set correctly?"));
        }
        else
        {
            wxLogMessage(wxT("You canceled printing"));
        }
    }
    else
    {
        (*g_printData) = printer.GetPrintDialogData().GetPrintData();
    }    
}
void DevTableDialog::OnCloseBtnClick(wxCommandEvent& event)
{
    Close();    
}

void DevTableDialog::OnPrintPreviewBtnClick(wxCommandEvent& event)
{
    PrintOnInit();
        // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview = new wxPrintPreview(new MyPrintout(data), new MyPrintout(data), &printDialogData);
    wxPreviewFrame *frame =
        new wxPreviewFrame(preview, this, wxT("Compass Deviation Table Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
}


