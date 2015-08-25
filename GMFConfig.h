#include <string>  
#include <map>  
#include <iostream>  
#include <fstream>  
#include <sstream>  

#define GMF_CONFIG_APP_SERVER_PORT "app_server_port"
#define GMF_CONFIG_REDIS_HOST "redis_host"
#define GMF_CONFIG_REDIS_PORT "redis_port"


using namespace std;
class GMFConfig{
public:
	GMFConfig( string config_file )
		: config_file_( config_file )
	{
		std::ifstream fstream( config_file_.c_str() );
		if ( !fstream.is_open() ){
			std::cerr << ( "File " + config_file_ + " not found" ).c_str();
			return;
		}
	}


	map<string, string> loadConfig()
	{
		map<string, string> config_set;

		ifstream fstream( config_file_.c_str() );
		if ( !fstream.is_open() ){
			std::cerr << ( "File " + config_file_ + " not found" ).c_str();
			return config_set;
		}

		string config_line;
		string key;
		string value;
		string::size_type pos = string::npos; 
		while( ! fstream.eof() ) {
			std::getline( fstream, config_line );

			config_line = trim( config_line );
			//trim the content of line
			if( config_line.empty() || '#' == config_line.at(0) ){
				continue;
			}
			if( (pos = config_line.find('=')) == string::npos ){
				continue;
			}

			key = trim( config_line.substr(0, pos) );
			value = trim( config_line.substr(pos + 1, config_line.size() - pos - 1) );

			if( !key.empty() ){
				config_set[key] = value;
			} 
		}

		fstream.close();
		return config_set;
	}

	string trim( const string& str ){
		if( str.empty() ){
			return str;
		}

		string::size_type pos = str.find_first_not_of(" \t\n\r\0\x0B");
		if( string::npos == pos ){
			return str;
		}
		string::size_type pos2 = str.find_last_not_of(" \t\n\r\0\x0B");
		return str.substr(pos, pos2 - pos + 1 );
	}
private:
	string config_file_;
	map<string, string> cnofig_set_;
};



