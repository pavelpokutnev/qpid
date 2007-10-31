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
 * "AS IS" BASIS, WITHOUT WARRANTIE4bS OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */
#include "SessionState.h"
#include "qpid/framing/reply_exceptions.h"
#include "qpid/framing/constants.h"
#include "qpid/framing/AMQMethodBody.h"
#include "qpid/log/Statement.h"

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/none.hpp>

namespace qpid {
namespace framing {

SessionState::SessionState(uint32_t ack,  const Uuid& uuid) :
    id(uuid),
    lastReceived(-1),
    lastSent(-1),
    ackInterval(ack),
    sendAckAt(lastReceived+ackInterval),
    solicitAckAt(lastSent+ackInterval),
    ackSolicited(false),
    resumable(true)
{}

SessionState::SessionState(const Uuid& uuid) :
    id(uuid),
    lastReceived(-1),
    lastSent(-1),
    ackInterval(0),
    sendAckAt(0),
    solicitAckAt(0),
    ackSolicited(false),
    resumable(false)
{
}
namespace {
bool isSessionCommand(const AMQFrame& f) {
    return f.getMethod() && f.getMethod()->amqpClassId() == SESSION_CLASS_ID;
}
}

boost::optional<SequenceNumber> SessionState::received(const AMQFrame& f) {
    if (isSessionCommand(f))
        return boost::none;
    ++lastReceived;
    QPID_LOG(trace, "Recv # "<< lastReceived << " " << id);
    if (ackInterval && lastReceived == sendAckAt)
        return sendingAck();
    else
        return boost::none;
}

bool SessionState::sent(const AMQFrame& f) {
    if (isSessionCommand(f))
        return false;
    if (resumable)
        unackedOut.push_back(f);
    ++lastSent;
    QPID_LOG(trace, "Sent # "<< lastSent << " " << id);
    return ackInterval &&
        (lastSent == solicitAckAt) &&
        sendingSolicit();
}

SessionState::Replay SessionState::replay() {
    Replay r(unackedOut.size());
    std::copy(unackedOut.begin(), unackedOut.end(), r.begin());
    return r;
}

void SessionState::receivedAck(SequenceNumber acked) {
     if (lastSent < acked)
        throw InvalidArgumentException("Invalid sequence number in ack");
    size_t keep = lastSent - acked;
    if (keep < unackedOut.size()) 
        unackedOut.erase(unackedOut.begin(), unackedOut.end()-keep);
    solicitAckAt = std::max(solicitAckAt, SequenceNumber(acked+ackInterval));
}

SequenceNumber SessionState::sendingAck() {
    sendAckAt = lastReceived+ackInterval;
    return lastReceived;
}

bool SessionState::sendingSolicit() {
    if (ackSolicited)
        return false;
    solicitAckAt = lastSent + ackInterval;
    return ackInterval != 0;
}

}} // namespace qpid::framing
