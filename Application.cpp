/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#else
#include "config.h"
#endif

#include "Application.h"
#include "quickfix/Session.h"
#include <iostream>

using namespace std;

void Application::test()
{
  std::cout << "\nTest functoin here e\n";

  FIX::Message message;
  //  order = queryNewOrderSingle42();
  message.getHeader().setField(8, "FIX.4.2");
  message.getHeader().setField(49, "GOLDMF");
  message.getHeader().setField(56, "KGITEST");
  message.getHeader().setField(35, "1");

  message.setField(112, "9987"),
  FIX::Session::sendToTarget( message );
  return;
}

void Application::testOrderEntry()
{
  std::cout << "\nTest Order Entry\n";

  FIX::Message message;
  //  order = queryNewOrderSingle42();
  message.getHeader().setField(8, "FIX.4.2");
  message.getHeader().setField(49, "GOLDMF");
  message.getHeader().setField(56, "KGITEST");
  message.getHeader().setField(35, "D");

  message.setField(1, "acc in App"),		//account
  message.setField(11, "1234567"),	//ClOrdID
  message.setField(21, "1"),		//HandInst
  message.setField(38, "1000"),		//OrderQty
  message.setField(40, "7"),		//OrdType -- 7:limit
  message.setField(44, "9.54"),		//Price
  message.setField(54, "1"),		//Side -- 1:buy
  message.setField(55, "0700"),		//symbol
//  message.setField(59, "0"),		//TimeInForce
  message.setField(60, "2015-07-09T01:08:18Z"),		//TransactTime


  FIX::Session::sendToTarget( message );
  return;
}

void Application::sendMessage( int head_field_count, int head_keys[], string head_vals[],
                int body_field_count, int body_keys[], string body_vals[]){

  std::cout << "\nTest send Message of GMF App\n";
  FIX::Message message;

  for( int i = 0; i < head_field_count; i++ ){
      int k = head_keys[i];
      string v = head_vals[i];
      message.getHeader().setField( k, v.c_str() );
  }

  for( int i = 0; i < body_field_count; i++ ){
    int k = body_keys[i];
    string v = body_vals[i];
    message.setField( k, v.c_str() );
  }

  FIX::Session::sendToTarget( message );
};

void Application::onLogon( const FIX::SessionID& sessionID )
{
  std::cout << std::endl << "Logon - " << sessionID << std::endl;
  redis_cli_.command( "SET", "fix app logon", "a" );
}

void Application::onLogout( const FIX::SessionID& sessionID )
{
  std::cout << std::endl << "Logout - " << sessionID << std::endl;
}

void Application::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
  crack( message, sessionID );
  std::cout << std::endl << "IN: " << message << std::endl;
}

void Application::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::DoNotSend )
{
  try
  {
    FIX::PossDupFlag possDupFlag;
    message.getHeader().getField( possDupFlag );
    if ( possDupFlag ) throw FIX::DoNotSend();
  }
  catch ( FIX::FieldNotFound& ) {}

  std::cout << std::endl
  << "OUT: " << message << std::endl;
}

void Application::onMessage
( const FIX42::ExecutionReport& report, const FIX::SessionID& ) {
	string msg_key = report.getField(11);
	string redis_key = "FIX_EXE_" + msg_key;
	string redis_val = report.toXML();
	redis_cli_.command( "SET", redis_key.c_str(), redis_val.c_str() );

	std::cout << std::endl << "execution report" << std::endl;
		
}


void Application::onMessage
( const FIX42::OrderCancelReject& report, const FIX::SessionID& ) {
	std::cout << std::endl << "reject cancel report" << std::endl;
}

void Application::run()
{

  while ( true )
  {
    try
    {
   

     std::cout << "press any key to start..." << std::endl;
  char input_c;
  std::cin >> input_c;

  testOrderEntry();


      char action = queryAction();

      if( action == '0' ){
        testOrderEntry();
      }
      else if ( action == '1' ){}
      else if ( action == '2' ){}
      else if ( action == '3' ){}
      else if ( action == '4' ){}
      else if ( action == '5' ){
        break;
      }
    }
    catch ( std::exception & e )
    {
      std::cout << "Message Not Sent: " << e.what();
    }
  }
}


char Application::queryAction()
{
  char value;
  std::cout << std::endl

  << "0) Customer Test" << std::endl
  << "1) Enter Order" << std::endl
  << "2) Cancel Order" << std::endl
  << "3) Replace Order" << std::endl
  << "4) Market data test" << std::endl
  << "5) Quit" << std::endl
  << "Action: ";
  std::cin >> value;
  switch ( value )
  {
    case '0': case '1': case '2': case '3': case '4': case '5': break;
    default: throw std::exception();
  }
  return value;
}
