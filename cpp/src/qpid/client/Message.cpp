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

#include "Message.h"

namespace qpid {
namespace client {

    Message::Message(const std::string& data_,
                     const std::string& routingKey,
                     const std::string& exchange) : TransferContent(data_, routingKey, exchange) {}

    std::string Message::getDestination() const 
    { 
        return method.getDestination(); 
    }

    bool Message::isRedelivered() const 
    { 
        return hasDeliveryProperties() && getDeliveryProperties().getRedelivered(); 
    }

    void Message::setRedelivered(bool redelivered) 
    { 
        getDeliveryProperties().setRedelivered(redelivered); 
    }

    framing::FieldTable& Message::getHeaders() 
    { 
        return getMessageProperties().getApplicationHeaders(); 
    }

    void Message::acknowledge(bool cumulative, bool notifyPeer) const
    {
        const_cast<Session&>(session).getExecution().markCompleted(id, cumulative, notifyPeer);
    }

    const framing::MessageTransferBody& Message::getMethod() const
    {
        return method;
    }

    const framing::SequenceNumber& Message::getId() const
    {
        return id;
    }

    /**@internal for incoming messages */
    Message::Message(const framing::FrameSet& frameset, Session s) :
        method(*frameset.as<framing::MessageTransferBody>()), id(frameset.getId()), session(s)
    {
        populate(frameset);
    }

    /**@internal use for incoming messages. */
    void Message::setSession(Session s) { session=s; }

}}
