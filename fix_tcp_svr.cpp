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

class session
{
public:
  session(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
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
	Json::Value _j_root;
        Json::Reader _j_reader;
        bool ret_j_parse = _j_reader.parse( json_str, _j_root );
        if( !ret_j_parse ){
                std::cout << "parse error\n" << _j_reader.getFormattedErrorMessages();
                return;
        }

	const Json::Value _j_keys = _j_root["k_set"];
	const Json::Value _j_vals = _j_root["v_set"];
	for( int i = 0; i < _j_keys.size(); ++i ){
		int k = _j_keys[i].asInt();
		std::string v = _j_vals[i].asString();
		
		std::cout << "get input: " << k << " = " << v << std::endl;
	}

        std::string out_str;
        out_str = _j_root["out"].asString();
        std::cout << "get output section as: " << out_str << std::endl;	  

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
	std::cout << "read complete\n";
    if (!error)
    {
	
	std::string in_buf_str(data_);	
	std::cout << "got data: " << in_buf_str << std::endl; 
	
	parse_input( in_buf_str );	

	const char res[max_length] = "fix reuslt\n";
	size_t res_len = strlen(res);
	
	std::string str_out = pack_output();

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
};

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    start_accept();
  }

private:
  void start_accept()
  {
    session* new_session = new session(io_service_);
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
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}