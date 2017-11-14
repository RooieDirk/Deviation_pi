#ifndef COMPASDEV1_H
#define COMPASDEV1_H

//(*Headers(DevDialog)
// #include <wx/dialog.h>
// #include <wx/sizer.h>
#include <wx/listctrl.h>
// #include <wx/button.h>
//*)
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/datetime.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/timectrl.h>
#include <wx/gbsizer.h>
#include <wx/valnum.h>

#include "readwritexml.h"
#include "SharedStuff.h"
#include "bearingdlg.h"
#include "spa.h"
#include "printing.h"
#include "preferencedlg.h"


//#include "tinyxml2/tinyxml2.h"
//#include "graphandprint.h"

class Compass;
class Meassurement;
class DevDialog;
class DevTableDialog;
class BasicDrawPane;

class CompasDev1Dialog: public wxFrame
{
	public:
		CompasDev1Dialog(wxWindow *parent,
            const wxString &title,
            compass_data* Data=NULL );
		virtual ~CompasDev1Dialog();

		//(*Declarations(CompasDev1Dialog)
        wxFlexGridSizer* FlexGridSizer; 
		wxListCtrl* DevListCtrl;
        wxBoxSizer* BoxSizerButons;
        wxButton* AddBtn;
        wxButton* DeleteBtn;
        wxButton* EditBtn;
        wxButton* ShowBtn;
        wxButton* CancelBtn;
        wxButton* CloseBtn;
        
        void OnAddBtnClick(wxCommandEvent& event);
        void OnDeleteBtnClick(wxCommandEvent& event);
        void OnEditBtnClick(wxCommandEvent& event);
        void OnShowBtnClick(wxCommandEvent& event);
        void OnCancelBtnClick(wxCommandEvent& event);
        void OnOKBtnClick(wxCommandEvent& event);
        void OnDevListCtrlItemSelect(wxListEvent& event);
        void OnDevListCtrlItemDeselect(wxListEvent& event);
		void OnDevListCtrlItemActivated(wxListEvent& event);
        void OnConnectionToggleEnableMouse(wxMouseEvent& event);
        void OnPreviewFrameModalityKind(wxCommandEvent& event);
        void OnRightDown(wxMouseEvent& event);
        void OnResize(wxSizeEvent& event);
        void EnableItem(const long index);
        //bool Show();
        void FillSourceList(void) ;
        void UpdateWindows(void);
        compass_data* data;
        DevTableDialog* DT_Dlg;

	protected:

		//(*Identifiers(CompasDev1Dialog)
		static const long ID_LISTCTRLDEV;
		static const long ID_ADDBUTTON;
		static const long ID_DELBUTTON;
		static const long ID_EDITBUTTON;
		static const long ID_BUTTONSHOW;
		static const long ID_BUTTONCLOSE;
        static const long ID_BUTTONCANCEL;
		static const long ID_CBCTRL;
		//*)

	private:
        
       // wxPreviewFrameModalityKind m_previewModality;
        
		//(*Handlers(CompasDev1Dialog)
		//*)
        long listselindex;
		DECLARE_EVENT_TABLE()
};






class BasicDrawPane : public wxPanel
{
public:
    BasicDrawPane(DevTableDialog *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxPanelNameStr,
                compass_data* Data=NULL);
    ~BasicDrawPane();
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void DrawTextCenterCenter(wxDC&  dc, wxString text, wxPoint pos);
    void render(wxDC& dc, compass_data* Data);
    wxBitmap* LogoBitmap;
    DevTableDialog *parentdlg;
    compass_data* data;
    DECLARE_EVENT_TABLE()
};

class DevTableDialog: public wxFrame
{
public:

    DevTableDialog(wxWindow* parent,wxWindowID id = wxID_ANY, compass_data* Data=NULL);
    virtual ~DevTableDialog();
    compass_data* data;
private:

    //(*Handlers(DevTableDialog)
    void OnQuit(wxCommandEvent& event);

    void OnPrintBtnClick(wxCommandEvent& event);
    void OnCloseBtnClick(wxCommandEvent& event);
    void OnPrintPreviewBtnClick(wxCommandEvent& event);
    //*)

    //(*Identifiers(DevTableDialog)
    static const long ID_PANEL1;
    static const long ID_PRINT;
    static const long ID_CLOSE;
    static const long ID_PRINTPREV;
    //*)

    //(*Declarations(DevTableDialog)
    BasicDrawPane* Panel1;
    wxButton* PrintBtn;
    wxButton* PrintPrevBtn;
    wxButton* CloseBtn;
    //*)

    DECLARE_EVENT_TABLE()
};

#endif
