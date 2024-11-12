#include <wx/dir.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#include <string>
#include <vector>
#include <sstream>

using namespace std;

class Files {
public:
  Files();
  virtual ~Files();

  int GetTablesCount();
  vector<Table> GetTables();
  bool IsMigrated();
  bool MigrateTables();

private:
	vector<Table> Tables {};
	int TablesCount;
  bool isMigrated;
};

Files::Files()
{
    isMigrated = false;
    wxDir dir; 
    wxString myParentFolder = _T("./Tables/"); 
    wxString filename;
    wxString Filespec = _T("*");
    wxArrayString childFolders;
    if ( dir.Open( myParentFolder ) )
    {
        bool cont = dir.GetFirst(&filename, Filespec, wxDIR_DIRS);
        TablesCount = 0;
        while ( cont )
        {
          TablesCount++;

          string file = filename.ToStdString();
          string filequery = "./Tables/" + file + "/query.sql";
          string filecolumns = "./Tables/" + file + "/columns.txt";

	        wxFileInputStream inputquery(filequery);
          wxTextInputStream textquery(inputquery, wxT("\x09"), wxConvUTF8 );

          string query = "";
          while(inputquery.IsOk() && !inputquery.Eof() )
          {
            wxString line=textquery.ReadLine();
            query = query + " " + line;
          }

          
	        wxFileInputStream inputcolumns(filecolumns);
          wxTextInputStream textcolumns(inputcolumns, wxT("\x09"), wxConvUTF8 );

          vector<string> columlist;
          string column = "";

          while(inputcolumns.IsOk() && !inputcolumns.Eof() )
          {
            wxString line=textcolumns.ReadLine();
            column = line;
            columlist.push_back(column);
          }

          Table table;
          table = Table(TablesCount, file, query, columlist);
          Tables.push_back(table);
          cont = dir.GetNext(&filename);
        }
    }
}

Files::~Files()
{
//terminate
}

int Files::GetTablesCount()
{
	return TablesCount;
 
} 

vector<Table> Files::GetTables()
{
	return Tables;
 
} 

bool Files::IsMigrated()
{
  return isMigrated;
}

bool Files::MigrateTables()
{
	bool success = true;
  DA *pDA = new DA();

  for (auto& table : Tables){
        success = success && pDA->ImportData(table.GetQuery(), table.GetColumnsList(), table.GetDescription());
  }
 
 return success;
} 