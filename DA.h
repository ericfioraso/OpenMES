#include "occi/occi.h"
#include <iostream>
#include <iomanip>
#include <wx/wx.h>
#include "wx/grid.h"

#include "sqlite/sqlite3.h"

#include "Config.h"

using namespace std;
using namespace oracle::occi;

class DA {
  
public:
  DA();
  ~DA();
  bool ConnectOracle();
  bool DisconnectOracle();

  string List();
  string **ListTable();
  wxGrid *QueryToGrid(wxGrid *grid, string sql, vector<string> columlist);
  wxGrid *QueryToGridOld(wxGrid *grid, string sql, vector<string> columlist);
  bool ImportData(string sql, vector<string> columlist, string description);

private:
  Environment *env;
  Connection  *con;

  string user;
  string passwd;
  string db;
  static int callback(void *NotUsed, int argc, char **argv, char **azColName);
  bool CreateTable(string sql, vector<string> columlist, string description);
  bool InsertData(string sql, vector<string> columlist, string description);
};

DA::DA()
{
  
}

DA::~DA()
{

}

bool DA::ConnectOracle(){


  //db = "werum3774:1521/pasx";
  
  Config *pConfig = new Config();
  
  db = pConfig->Server() + ":" + pConfig->Port()+ "/" + pConfig->DBId();

  user = pConfig->User();
  passwd = pConfig->Pwd();

  env = Environment::createEnvironment(Environment::DEFAULT);

  try
  {    
    con = env->createConnection(user, passwd, db);
  }
  catch (SQLException& ex)
  {
        wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage(), "Error", wxOK | wxICON_ERROR );
  }

  return true;

}

bool DA::DisconnectOracle(){
 env->terminateConnection (con);

  Environment::terminateEnvironment (env);

  return true;
}
  
/*
string **DA::ListTable()
{
  // Initialize 2D array
  string **listTable;
 
  Statement *stmt = NULL;
  ResultSet *rs = NULL;
  string sql = "select ENTITYKEY, FIRSTNAME, LASTNAME " \
               "from PERSON order by LASTNAME, FIRSTNAME";
  string message = "";

  try
  {
    stmt = con->createStatement(sql);
  }
  catch (SQLException& ex)
  {
    wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage(), "Error", wxOK | wxICON_ERROR );
  }

  if (stmt)
  {
    try
    {
      stmt->setPrefetchRowCount(32);

      rs = stmt->executeQuery();
    }
    catch (SQLException& ex)
    {
      wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage(), "Error", wxOK | wxICON_ERROR );
    }

    std::vector<oracle::occi::MetaData> cmd;

    cmd = rs->getColumnListMetaData();
    //int idataType=cmd[0].getInt(MetaData::ATTR_DATA_TYPE);
    string sdataType=cmd[0].getString(oracle::occi::MetaData::ATTR_CHAR_USED);


    if (rs)
    {
      int recordCount = 1;

      recordCount = stmt->getUpdateCount();

      recordCount = 5;

      int rowpos = 0;
      
      listTable = new string*[recordCount + 1];

      listTable[rowpos] = new string[3];

      listTable[rowpos][0] = "ID";
      listTable[rowpos][1] = "First Name";
      listTable[rowpos][2] = "Last Name";
      
      

      while (rs->next()) {
        rowpos++;
        listTable[rowpos] = new string[3];
        listTable[rowpos][0] = "0";

        listTable[rowpos][0] = rs->getString(1);
        listTable[rowpos][1] = rs->getString(2);
        listTable[rowpos][2] = rs->getString(3);

      }

      stmt->closeResultSet(rs);
    }

    con->terminateStatement(stmt);

  }
    return listTable;
 
} 

*/

bool DA::CreateTable(string sql, vector<string> columlist, string description){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  string createsql = "";

  /* Open database */
  rc = sqlite3_open("DB/DB.db", &db);
   
  if( rc ) {
    wxMessageBox("Can't open database", "Error", wxOK | wxICON_ERROR );
    return(0);
  }




  Statement *stmt = NULL;
  ResultSet *rs = NULL;

  string message = "";

  try
  {
    ConnectOracle();
    stmt = con->createStatement(sql);

  }
  catch (SQLException& ex)
  {
    wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage() + " for " + description , "Error", wxOK | wxICON_ERROR );
    return false;
  }

  if (stmt)
  {
    try
    {
      stmt->setPrefetchRowCount(32);

      rs = stmt->executeQuery();
    }
    catch (SQLException& ex)
    {
      wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage() + " for " + description, "Error", wxOK | wxICON_ERROR );
      return false;
    }

    if (rs)
    {

      createsql = "CREATE TABLE " + description + " (";
      vector<int> columsize;

      for (auto& column : columlist){
        columsize.push_back(0);
      }

      int irow = 0;
      irow = 0;

      try
      {


rs->setMaxColumnSize(1,1);
        while (rs->next()) {

          for (int icol = 0; icol < columlist.size(); icol++) {
              string value = "";
              value = rs->getString(icol + 1);
              if (value.length() > columsize[icol]){
                columsize[icol] = value.length();
              }

          }
        irow++;
        }

        if (irow == 0)
        {
          wxMessageBox("No data found for " + description, "Error", wxOK | wxICON_ERROR );
          return false;
        }

      }
      catch (SQLException& ex)
      {
        wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage() + " for " + description, "Error", wxOK | wxICON_ERROR );
        return false;
      }

      stmt->closeResultSet(rs);

      char letters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};


      int icol = 0;
      for (auto& column : columlist){
        int h = 100;
        int t = 10;
        int u = 1;

        h = int((int)icol / 100);
        t = int((int)(icol - h) / 10);
        u = icol - (h * 100) - (t * 10);

        createsql = createsql + " '"
        + letters[h]
        + letters[t]
        + letters[u]
        + "' CHAR(" + to_string(columsize[icol]) + "),";
        icol ++;
      }

      
      createsql = createsql.substr(0, createsql.length() - 1) + ");";

      /* Execute SQL statement */
      rc = sqlite3_exec(db, createsql.c_str(), callback, 0, &zErrMsg);
      
      if( rc != SQLITE_OK ){
        string szErrMsg = zErrMsg;
        wxMessageBox(szErrMsg  + " for " + description , "Error", wxOK | wxICON_ERROR );
        sqlite3_free(zErrMsg);
      }
      sqlite3_close(db);

    }
    con->terminateStatement(stmt);

    DisconnectOracle();

  }
  return true;
}

bool DA::InsertData(string sql, vector<string> columlist, string description){

  Statement *stmt = NULL;
  ResultSet *rs = NULL;

  string message = "";

  try
  {
    ConnectOracle();
    stmt = con->createStatement(sql);
  }
  catch (SQLException& ex)
  {
    wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage() + " for " + description, "Error", wxOK | wxICON_ERROR );
    return false;
  }

  if (stmt)
  {
    try
    {
      stmt->setPrefetchRowCount(32);

      rs = stmt->executeQuery();
    }
    catch (SQLException& ex)
    {
      wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage() + " for " + description, "Error", wxOK | wxICON_ERROR );
      return false;
    }

    if (rs)
    {

      string createinserta = "";

      createinserta = "INSERT INTO " + description + " (";

      char letters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

      int icol = 0;
      for (auto& column : columlist){
        int h = 100;
        int t = 10;
        int u = 1;

        h = int((int)icol / 100);
        t = int((int)(icol - h) / 10);
        u = icol - (h * 100) - (t * 10);

        createinserta = createinserta + " '"
        + letters[h]
        + letters[t]
        + letters[u]
        + "', ";
        icol ++;
      }

      createinserta = createinserta.substr(0, createinserta.length() - 2) + ") VALUES (";

      string createinsertb = "";

      int irow = 0;
      irow = 0;

      try
      {

        while (rs->next()) {

          createinsertb = createinserta;

          for (int icol = 0; icol < columlist.size(); icol++) {
              string value = "";
              value = rs->getString(icol + 1);
              createinsertb = createinsertb + "'" + value + "', ";
          }

          createinsertb = createinsertb.substr(0, createinsertb.length() - 2) + ");";

          sqlite3 *db;
          char *zErrMsg = 0;
          int rc;


          /* Open database */
          rc = sqlite3_open("DB/DB.db", &db);

          if( rc ) {
            wxMessageBox("Can't open database", "Error", wxOK | wxICON_ERROR );
            return(0);
          }

          rc = sqlite3_exec(db, createinsertb.c_str(), callback, 0, &zErrMsg);

          if( rc != SQLITE_OK ){
            string szErrMsg = zErrMsg;
            wxMessageBox(szErrMsg , "Error", wxOK | wxICON_ERROR );
            sqlite3_free(zErrMsg);
            return false;
          }
          sqlite3_close(db);

          irow++;
        }

      }
      catch (SQLException& ex)
      {
        wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage()  + " for " + description, "Error", wxOK | wxICON_ERROR );
      }

      stmt->closeResultSet(rs);


    }
    con->terminateStatement(stmt);

    DisconnectOracle();

  }

  

  return true;
}

bool DA::ImportData(string sql, vector<string> columlist, string description)
{



  bool result = true;
  result = result && CreateTable(sql, columlist, description);
  result = result && InsertData(sql, columlist, description);
  
  
 return result;
} 

int DA::callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      string sazColName = azColName[i];
      string sargv = argv[i];
      wxMessageBox(sazColName + " = " + sargv , "Error", wxOK | wxICON_ERROR );
   }

   return 0;
}

wxGrid *DA::QueryToGrid(wxGrid *grid, string table, vector<string> columlist)
{

  string message = "";
  sqlite3 *db;
  sqlite3_stmt* stmt;
  char *zErrMsg = 0;
  int rc;

  int icolumns = 0;
  for (auto& column : columlist){
    grid->AppendCols(1);
    grid->SetColMinimalWidth(icolumns, 20);
    grid->SetColLabelValue(icolumns, column);
    icolumns++;
  }

  int irow = 0;
  irow = 0;


  /* Open database */
  rc = sqlite3_open("DB/DB.db", &db);

  if( rc ) {
    wxMessageBox("Can't open database", "Error", wxOK | wxICON_ERROR );
  }


  string sql = "";
  sql = "SELECT * FROM " + table + ";";
  if(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    string error = "";
    error = sqlite3_errmsg(db);
    error = "ERROR: while compiling sql: " + error;
    wxMessageBox(error, "Error", wxOK | wxICON_ERROR );
    sqlite3_close(db);
    sqlite3_finalize(stmt);
  }



    int ret_code = 0;
  while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {

        grid->AppendRows(1);

    for (int icol = 0; icol < columlist.size(); icol++) {
        string value = "";
        value = (char*)sqlite3_column_text(stmt, icol);
        grid->SetCellValue(irow, icol, value);
        grid->SetReadOnly(irow, icol);
    }
    irow++;
  }
  if(ret_code != SQLITE_DONE) {
    string error = "";
    error = sqlite3_errmsg(db);
    error = "ERROR: while compiling sql: " + error;
    wxMessageBox(error, "Error", wxOK | wxICON_ERROR );
  }

      sqlite3_finalize(stmt);
    sqlite3_close(db);

  if( rc != SQLITE_OK ){
    string szErrMsg = zErrMsg;
    wxMessageBox(szErrMsg , "Error", wxOK | wxICON_ERROR );
    sqlite3_free(zErrMsg);
  }
  sqlite3_close(db);


  return grid;
 
} 

wxGrid *DA::QueryToGridOld(wxGrid *grid, string sql, vector<string> columlist)
{

  Statement *stmt = NULL;
  ResultSet *rs = NULL;

  string message = "";

  try
  {
    stmt = con->createStatement(sql);
  }
  catch (SQLException& ex)
  {
    wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage(), "Error", wxOK | wxICON_ERROR );
  }

  if (stmt)
  {
    try
    {
      stmt->setPrefetchRowCount(32);

      rs = stmt->executeQuery();
    }
    catch (SQLException& ex)
    {
      wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage(), "Error", wxOK | wxICON_ERROR );
    }

    if (rs)
    {


      //std::vector<oracle::occi::MetaData> cmd;
      //cmd = rs->getColumnListMetaData();
      //string test = "";
      //for (auto& cmdv : cmd){
      //  test = cmdv.getString(MetaData::ATTR_NAME);
      //}


      //grid->CreateGrid(0, columlist.size());



      int icolumns = 0;
      for (auto& column : columlist){
        grid->AppendCols(1);
        grid->SetColMinimalWidth(icolumns, 20);
        grid->SetColLabelValue(icolumns, column);
        icolumns++;
      }

      int irow = 0;
      irow = 0;

      try
      {

        while (rs->next()) {
          grid->AppendRows(1);

          for (int icol = 0; icol < columlist.size(); icol++) {
              string value = "";
              value = rs->getString(icol + 1);
              grid->SetCellValue(irow, icol, value);
              grid->SetReadOnly(irow, icol);
          }
        irow++;
        }

      }
      catch (SQLException& ex)
      {
        wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage(), "Error", wxOK | wxICON_ERROR );
      }

      stmt->closeResultSet(rs);
    }

    con->terminateStatement(stmt);

  }
  return grid;
 
} 

string DA::List()
{

  Statement *stmt = NULL;
  ResultSet *rs = NULL;
  string sql = "select ENTITYKEY, FIRSTNAME, LASTNAME " \
               "from PERSON order by LASTNAME, FIRSTNAME";
  string message = "";

  try
  {
    stmt = con->createStatement(sql);
  }
  catch (SQLException& ex)
  {
    wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage(), "Error", wxOK | wxICON_ERROR );
  }

  if (stmt)
  {
    try
    {
      stmt->setPrefetchRowCount(32);

      rs = stmt->executeQuery();
    }
    catch (SQLException& ex)
    {
      wxMessageBox(ex.getErrorCode() + " - " + ex.getMessage(), "Error", wxOK | wxICON_ERROR );
    }

    if (rs)
    {
      message = "";

      message = "ID      FIRST NAME            LAST NAME";
      message = message + "\n======  ====================  =========================";
      

      while (rs->next()) {
        message = message + "\n" + rs->getString(1) + " - " + rs->getString(2) + " - " + rs->getString(3);
      }

      stmt->closeResultSet(rs);
    }

    con->terminateStatement(stmt);
  }

  return message;
}