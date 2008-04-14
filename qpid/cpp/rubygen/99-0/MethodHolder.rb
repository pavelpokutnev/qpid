#!/usr/bin/env ruby
$: << ".."                      # Include .. in load path
require 'cppgen'

class MethodHolderGen < CppGen
  
  def initialize(outdir, amqp)
    super(outdir, amqp)
    @namespace="qpid::framing"
    @classname="BodyHolder"
    @filename="qpid/framing/BodyHolder"
  end

  def gen_max_size()
    # Generate program to generate MaxSize.h
    cpp_file("generate_MaxMethodBodySize_h") {
      include "qpid/framing/AMQHeaderBody"
      include "qpid/framing/AMQContentBody"
      include "qpid/framing/AMQHeartbeatBody"
      @amqp.methods_.each { |m| include "qpid/framing/#{m.body_name}" }
      genl
      include "<algorithm>"
      include "<fstream>"
      genl
      genl "using namespace std;"
      genl "using namespace qpid::framing;"
      genl
      scope("int main(int, char** argv) {") {
        genl "size_t maxSize=0;"
        genl "maxSize=max(maxSize, sizeof(AMQHeaderBody));" 
        genl "maxSize=max(maxSize, sizeof(AMQContentBody));" 
        genl "maxSize=max(maxSize, sizeof(AMQHeartbeatBody));" 
        @amqp.methods_.each { |m|
          genl "maxSize=max(maxSize, sizeof(#{m.body_name}));" }
        gen <<EOS
ofstream out("qpid/framing/MaxMethodBodySize.h");
out << "// GENERATED CODE: generated by " << argv[0] << endl;
out << "namespace qpid{ namespace framing { " << endl;
out << "const size_t MAX_METHOD_BODY_SIZE=" << maxSize << ";" << endl;
out << "}}" << endl;
EOS
      }
    }
  end

  def gen_construct
    cpp_file(@filename+"_gen") {
      include @filename
      include "qpid/framing/AMQHeaderBody"
      include "qpid/framing/AMQContentBody"
      include "qpid/framing/AMQHeartbeatBody"
      @amqp.methods_.each { |m| include "qpid/framing/#{m.body_name}" }
      include "qpid/framing/FrameDefaultVisitor.h"
      include "qpid/Exception.h"
      genl
      namespace(@namespace) {
        scope("void #{@classname}::setMethod(ClassId c, MethodId m) {") {
          scope("switch (c) {") {
            @amqp.classes.each { |c|
              scope("case #{c.index}: switch(m) {") {
                c.methods_.each { |m|
                  genl "case #{m.index}: blob = in_place<#{m.body_name}>(); break;"
                }
                genl "default: throw Exception(QPID_MSG(\"Invalid method id \" << int(m) << \" for class #{c.name} \"));"
              }
              genl "break;"
            }
            genl "default: throw Exception(QPID_MSG(\"Invalid class id \" << int(c)));"
          }
        }

        struct("CopyVisitor", "public FrameDefaultVisitor") {
          genl "using FrameDefaultVisitor::visit;"
          genl "using FrameDefaultVisitor::defaultVisit;"
          genl "BodyHolder& holder;"
          genl "CopyVisitor(BodyHolder& h) : holder(h) {}"
          ["Header", "Content", "Heartbeat"].each { |type|
            genl "void visit(const AMQ#{type}Body& x) { holder=x; }"
          }
          @amqp.methods_.each { |m|
            genl "void visit(const #{m.body_name}& x) { holder=x; }"
          }
          genl "void defaultVisit(const AMQBody&) { assert(0); }"
        }
        genl

        scope("void BodyHolder::setBody(const AMQBody& b) {") {
          genl "CopyVisitor cv(*this); b.accept(cv);"
        }
      }}
  end

  def generate
    gen_max_size
    gen_construct
  end
end

MethodHolderGen.new($outdir, $amqp).generate();

