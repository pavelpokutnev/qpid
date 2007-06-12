/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

/**
 * This file provides one half of a test and example of a pub-sub
 * style of interaction. See topic_listener.cpp for the other half, in
 * which the logic for subscribers is defined.
 * 
 * This file contains the publisher logic. The publisher will send a
 * number of messages to the exchange with the appropriate routing key
 * for the logical 'topic'. Once it has done this it will then send a
 * request that each subscriber report back with the number of message
 * it has received and the time that elapsed between receiving the
 * first one and receiving the report request. Once the expected
 * number of reports are received, it sends out a request that each
 * subscriber shutdown.
 */

#include "qpid/QpidError.h"
#include "qpid/client/ClientChannel.h"
#include "qpid/client/Connection.h"
#include "qpid/client/ClientExchange.h"
#include "qpid/client/MessageListener.h"
#include "qpid/client/ClientQueue.h"
#include "qpid/sys/Monitor.h"
#include <unistd.h>
#include "qpid/sys/Time.h"
#include <cstdlib>
#include <iostream>
#include <time.h>

using namespace qpid::client;
using namespace qpid::sys;
using std::string;


bool done = 0;

class Listener : public MessageListener{
public:
    virtual void received(Message& msg);
    ~Listener() { };
};


int main() {
    Connection connection;
    Channel channel;
    Message msg;
    try {
        connection.open("127.0.0.1", 5672, "guest", "guest", "/test");
        connection.openChannel(channel);

  //--------- Main body of program --------------------------------------------
	Queue response("listener");
        channel.bind(Exchange::STANDARD_DIRECT_EXCHANGE, response, "listener");

        Listener listener;
        string routingKey="listener";
        channel.consume(response, routingKey, &listener);

        channel.start();

	    while (!done)
	        ;
  //-----------------------------------------------------------------------------

        channel.close();
        connection.close();
        return 0;
    } catch(qpid::QpidError error) {
        // cout << error.what() << std::endl;
    }
    return 1;
}

void Listener::received(Message& msg) {
     std::cout << msg.getData();
     if (msg.getData() == "That's all, folks!")
       done = 1;
}
