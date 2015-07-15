//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string.h>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "json/json.h"


#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#else
#include "config.h"
#endif

#include "quickfix/FileStore.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Log.h"
#include "Application.h"


using boost::asio::ip::tcp;
using namespace std;

class session
{
public:
  session(boost::asio::io_service& io_service, Application& app)
    : socket_(io_service),
      fix_app_(app)
  {}

  tcp::socket& socket()
  {
    return socket_;
  }

  void test_fix_msg(){
      try{
        std::cout << "start fix message test in boost server" << std::endl;
        fix_app_.testOrderEntry();
        int h_keys[] = {8  , 49, 56, 35};
        string h_vals[] = {"FIX.4.2", "GOLDMF", "KGITEST", "D"};

        int m_keys[] = {1, 11, 21, 38, 40, 44, 54, 55, 60};
        string m_vals[] = {"acc in Main", "1234567", "1", "1000", "7", "9,54", "1", "0700", "2015-07-09T01:08:18Z"};
/**
        int h_keys[] = {8, 49, 56, 35};
        string h_vals[] = {"FIX.4.2", "GOLDMF", "KGITEST", "1"};

        int m_keys[] = {112};
        string m_vals[] = {"heart beat test string"};
*/
        fix_app_.sendMessage( 4, h_keys, h_vals, 9, m_keys, m_vals );

      }catch (std::exception& e){
        std::cout << "Message Not Sent: " << e.what();
      }
  }

  void start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

private:
  void parse_input( std::string json_str ){
	try{
	Json::Value j_root;
        Json::Reader j_reader;
        bool ret_j_parse = j_reader.parse( json_str, j_root );
        if( !ret_j_parse ){
                std::cout << "parse error\n" << j_reader.getFormattedErrorMessages();
                return;
        }
	
	const Json::Value j_head_keys = j_root["head_keys"];
	const Json::Value j_head_vals = j_root["head_vals"];
	const Json::Value j_body_keys = j_root["body_keys"];
	const Json::Value j_body_vals = j_root["body_vals"];

	int h_size = j_head_keys.size();
	int head_keys[h_size];
	string head_vals[h_size];
	for( int i = 0; i < h_size; ++i ){
		head_keys[i]= j_head_keys[i].asInt();
		head_vals[i] = j_head_vals[i].asString();
	}
	
	int b_size = j_body_keys.size();
	int body_keys[b_size];
	string body_vals[b_size];
	for ( int i =0; i < b_size; ++i ){
		body_keys[i] = j_body_keys[i].asInt();
		body_vals[i] = j_body_vals[i].asString();
	}


	fix_app_.sendMessage( h_size, head_keys, head_vals, 
				b_size, body_keys, body_vals );
	
      	}catch (std::exception& e){
        	std::cout << "Message Not Sent: " << e.what();
      	}
  }

  std::string pack_output(){
   	Json::Value j_out;
  	j_out["ret"] = 0;
  	j_out["msg"] = "success";

  	std::string str_out = j_out.toStyledString();
  	return str_out;	
	
  }

  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
	
    	std::string in_buf_str(data_);	
    	std::cout << "got data: " << in_buf_str << std::endl; 
    	
//	process input string 
    	parse_input( in_buf_str );	
    	
    	std::string str_out = pack_output();

//      test_fix_msg();


      boost::asio::async_write(socket_,
          boost::asio::buffer(str_out.c_str(), str_out.size() ),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
    }
    else
    {
      delete this;
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      delete this;
    }
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];

  Application& fix_app_;
};

class server
{
public:
  server(boost::asio::io_service& io_service, short port, Application& app)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      fix_app_(app)
  {
    start_accept();
  }

private:
  void start_accept()
  {
    session* new_session = new session(io_service_, fix_app_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  void handle_accept(session* new_session,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_session->start();
    }
    else
    {
      delete new_session;
    }

    start_accept();
  }

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  Application& fix_app_;
};

int main(int argc, char* argv[])
{
  std::string file("./fix.cfg");
  FIX::SessionSettings settings( file );
  Application application;
  FIX::FileStoreFactory storeFactory( settings );
  FIX::ScreenLogFactory logFactory( settings );
  FIX::SocketInitiator initiator( application, storeFactory, settings, logFactory );

  try
  {
    initiator.start();

//	application.run();

    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]), application);


    io_service.run();

    initiator.stop();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
    initiator.stop();
  }
  return 0;
}
