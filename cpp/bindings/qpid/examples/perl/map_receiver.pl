#! /usr/bin/env perl
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
use strict;
use warnings;
use Data::Dumper;

use qpid;

my $url               = ( @ARGV > 0 ) ? $ARGV[0] : "amqp:tcp:127.0.0.1:5672";
my $address           = ( @ARGV > 1 ) ? $ARGV[0] : "message_queue; {create: always}";
my $connectionOptions = ( @ARGV > 2 ) ? $ARGV[1] : "";

my $connection = new qpid::messaging::Connection($url, $connectionOptions);

eval {
    $connection->open();
    my $session  = $connection->create_session();
    my $receiver = $session->create_receiver($address);
    my $message  = $receiver->fetch();
    my $content = $message->get_content();

    print Dumper($content);

    $session->acknowledge();
    $receiver->close();
    $connection->close();
};

die $@ if ($@);
