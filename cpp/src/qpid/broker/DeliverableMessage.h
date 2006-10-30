/*
 *
 * Copyright (c) 2006 The Apache Software Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef _DeliverableMessage_
#define _DeliverableMessage_

#include "qpid/broker/Deliverable.h"
#include "qpid/broker/Message.h"
#include "qpid/broker/Queue.h"

namespace qpid {
    namespace broker {
        class DeliverableMessage : public Deliverable{
            Message::shared_ptr msg;
        public:
            DeliverableMessage(Message::shared_ptr& msg);
            virtual void deliverTo(Queue::shared_ptr& queue);
            virtual ~DeliverableMessage(){}
        };
    }
}


#endif
