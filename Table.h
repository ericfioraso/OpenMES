#include <iostream>
#include <vector>

using namespace std;

class Table {
public:
  Table();
  Table(int _id, string _description, string _query, vector<string> _columnslist);
  virtual ~Table();

  int GetId();
  string GetDescription();
  string GetQuery();
  vector<string> GetColumnsList();

private:
  int id;
  string description;
  string query;
  vector<string> columnslist {};
};

Table::Table()
{
  
}

Table::~Table()
{
//terminate
}

Table::Table(int _id, string _description, string _query, vector<string> _columnslist)
{
    id = _id;
    description = _description;
    query = _query;
    columnslist = _columnslist;
}

int Table::GetId()
{
    return id;
}

string Table::GetDescription()
{
	return description;
 
} 

string Table::GetQuery()
{
	return query ;
 
} 

vector<string> Table::GetColumnsList()
{
	return columnslist;
 
} 
