#include <wx/wfstream.h>
#include <wx/txtstrm.h>

using namespace std;

class Config {
public:
  Config();
  virtual ~Config();

  string Server();
  string Port();
  string DBId();
  string User();
  string Pwd();

private:
	string server;
	string port;
	string dBId;
	string user;
	string pwd;
};

Config::Config()
{
	wxFileInputStream input(wxT("config.txt"));
	wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
	while(input.IsOk() && !input.Eof() )
	{
		while(input.IsOk() && !input.Eof() ){
  			wxString line=text.ReadLine();
			if(line.substr(0, 15) == "server-address=") {
				server = line.substr(15, line.length() - 15);
			}
			else if(line.substr(0, 12) == "server-port=") {
				port = line.substr(12, line.length() - 12);
			}
			else if(line.substr(0, 5) == "DBId=") {
				dBId = line.substr(5, line.length() - 5);
			}
			else if(line.substr(0, 5) == "user=") {
				user = line.substr(5, line.length() - 5);
			}
			else if(line.substr(0, 4) == "pwd=") {
				pwd = line.substr(4, line.length() - 4);
			}
		}
	}
}

Config::~Config()
{
//terminate
}

string Config::Server()
{
	return server;
 
} 

string Config::Port()
{
	return port;
} 

string Config::DBId()
{
	return dBId;
} 

string Config::User()
{
	return user;
} 

string Config::Pwd()
{
	return pwd;
} 