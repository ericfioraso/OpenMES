#include <wx/wx.h>
#include "wx/grid.h"
#include "occi/occi.h"
#include <iostream>
#include <iomanip>


#include "DA.h"

#include "Table.h"
#include "Files.h"



using namespace std;

class MyApp: public wxApp 
{
public:
    virtual bool OnInit();
};
class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
    void OnMigrate(wxCommandEvent& event);
    void OnReport(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTables(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
};

Files *pFiles = new Files();
wxGrid *m_Grid_Hold;
wxPanel *Panel1;
wxSizer *sizer;

int row_count;
int col_count;

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU_RANGE(0, pFiles->GetTablesCount(), MyFrame::OnTables)
    EVT_MENU(pFiles->GetTablesCount() + 1,   MyFrame::OnMigrate)
    EVT_MENU(pFiles->GetTablesCount() + 2,   MyFrame::OnReport)
    EVT_MENU(wxID_ABOUT,   MyFrame::OnAbout)
    
    wxEND_EVENT_TABLE()
    wxIMPLEMENT_APP(MyApp);


bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( "Integrator", wxPoint(50, 50), wxSize(450, 340) );
    frame->Show( true );
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(pFiles->GetTablesCount() + 1, "&Migrate\tCtrl-M",
                     "Start migration process");
    menuFile->Append(pFiles->GetTablesCount() + 2, "&Report\tCtrl-R",
                     "Start migration process");

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );

    wxMenu *menuTables = new wxMenu;
    vector<Table> TableList = pFiles->GetTables();

    int id = 0;
    for (auto& table : TableList){
        menuTables->Append(table.GetId(), table.GetDescription(), table.GetDescription());
        id++;
    }

    menuBar->Append( menuTables, "&Tables" );

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    menuBar->Append( menuHelp, "&Help" );

    SetMenuBar( menuBar );

    CreateStatusBar();
    
    SetStatusText( "Welcome to Integrator!" );

    //wxPanel *Panel1 = new wxPanel(this, 1, wxDefaultPosition, wxDefaultSize);
    //Panel1->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Tahoma")));

    //wxButton *Hello = new wxButton(Panel1, BUTTON_Hello, _T("Hello")); 

    //wxButton *World = new wxButton(Panel1, BUTTON_World, _T("World")); 

    //wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    //Panel1->SetSizer(sizer);

    //sizer->Add(Hello, 1, wxALIGN_TOP);
    //sizer->Add(World, 1, wxALIGN_TOP);

    //Panel1->SetSizer(sizer);
    //sizer->SetSizeHints(this);


    //wxButton *Hello = new wxButton(this, BUTTON_Hello, _T("Hello"),
      // shows a button on this window
    //wxDefaultPosition, wxDefaultSize, 0); 

    //wxButton *World = new wxButton(this, BUTTON_World, _T("World"),
    //  // shows a button on this window
    //wxDefaultPosition, wxDefaultSize, 0); 

    row_count = 0;
    col_count = 0;
    
}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}


void MyFrame::OnMigrate(wxCommandEvent& event)
{
    wxLogMessage("Migration about to start!");

    bool success = false;
    success = pFiles->MigrateTables();

    if (success){
            wxLogMessage("Migration finished successfully");
    }else
    {
        wxLogMessage("Migration failed");
    }

    

}

void MyFrame::OnReport(wxCommandEvent& event)
{
    wxLogMessage("Migration Report!");
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxLogMessage("Lucas Santos created this program");
}

void MyFrame::OnTables(wxCommandEvent& event)
{
    
    DA *pDA = new DA();

    vector<Table> TableList = pFiles->GetTables();
    Table table = TableList.at(event.GetId() - 1);



    if (col_count > 0){
        m_Grid_Hold->DeleteCols(0, col_count, false);
    }
    else
    {
        Panel1 = new wxPanel(this, 1, wxDefaultPosition, wxDefaultSize);

        sizer = new wxBoxSizer(wxVERTICAL);

        m_Grid_Hold= new wxGrid( Panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        m_Grid_Hold->CreateGrid(0, 1);

        sizer->Add(m_Grid_Hold, 1, wxALIGN_TOP);
    }

    if (row_count > 0)
        m_Grid_Hold->DeleteRows(0, row_count, false);

    m_Grid_Hold = pDA->QueryToGrid(m_Grid_Hold, table.GetDescription(), table.GetColumnsList());

    m_Grid_Hold->AutoSize();

    Panel1->SetSizer(sizer);
    sizer->SetSizeHints(this);



    row_count = m_Grid_Hold->GetNumberRows();
    col_count = m_Grid_Hold->GetNumberCols();
    
    delete pDA; 
}

