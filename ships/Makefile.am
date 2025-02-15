SUBDIRS = gametest channeltest

noinst_LTLIBRARIES = libships.la
bin_PROGRAMS = shipsd ships-channel

RPC_STUBS = \
  rpc-stubs/shipschannelrpcserverstub.h

PROTOS = \
  proto/boardmove.proto \
  proto/boardstate.proto
PROTOHEADERS = $(PROTOS:.proto=.pb.h)
PROTOSOURCES = $(PROTOS:.proto=.pb.cc)
PROTOPY = $(PROTOS:.proto=_pb2.py)

EXTRA_DIST = $(PROTOS) \
  rpc-stubs/channel.json \
  schema.sql schema_head.cpp schema_tail.cpp

BUILT_SOURCES = $(RPC_STUBS) $(PROTOHEADERS) $(PROTOPY)
CLEANFILES = $(RPC_STUBS) $(PROTOHEADERS) $(PROTOSOURCES) $(PROTOPY) schema.cpp

libships_la_CXXFLAGS = \
  -I$(top_srcdir) \
  $(JSONCPP_CFLAGS) $(SQLITE3_CFLAGS) $(GLOG_CFLAGS) $(PROTOBUF_CFLAGS)
libships_la_LIBADD = \
  $(top_builddir)/spacexpanseutil/libspacexpanseutil.la \
  $(top_builddir)/spacexpansegame/libspex.la \
  $(top_builddir)/sidechannel/libchannelcore.la \
  $(top_builddir)/sidechannel/libsidechannel.la \
  $(JSONCPP_LIBS) $(SQLITE3_CFLAGS) $(GLOG_LIBS) $(PROTOBUF_LIBS)
libships_la_SOURCES = \
  board.cpp \
  channel.cpp \
  coord.cpp \
  gamestatejson.cpp \
  grid.cpp \
  logic.cpp \
  schema.cpp \
  $(PROTOSOURCES)
LIBSHIPSHEADERS = \
  board.hpp \
  channel.hpp \
  coord.hpp \
  gamestatejson.hpp \
  grid.hpp \
  logic.hpp \
  schema.hpp \
  $(RPC_STUBS) \
  $(PROTOHEADERS)

shipsd_CXXFLAGS = \
  -I$(top_srcdir) \
  $(JSONCPP_CFLAGS) $(GLOG_CFLAGS) $(GFLAGS_CFLAGS) $(PROTOBUF_CFLAGS)
shipsd_LDADD = \
  $(builddir)/libships.la \
  $(top_builddir)/spacexpanseutil/libspacexpanseutil.la \
  $(top_builddir)/spacexpansegame/libspex.la \
  $(top_builddir)/sidechannel/libchannelcore.la \
  $(top_builddir)/sidechannel/libsidechannel.la \
  $(JSONCPP_LIBS) $(GLOG_LIBS) $(GFLAGS_LIBS) $(PROTOBUF_LIBS)
shipsd_SOURCES = main-gsp.cpp

ships_channel_CXXFLAGS = \
  -I$(top_srcdir) \
  $(JSONCPP_CFLAGS) $(JSONRPCSERVER_CFLAGS) \
  $(GLOG_CFLAGS) $(GFLAGS_CFLAGS) $(PROTOBUF_CFLAGS)
ships_channel_LDADD = \
  $(builddir)/libships.la \
  $(top_builddir)/spacexpanseutil/libspacexpanseutil.la \
  $(top_builddir)/spacexpansegame/libspex.la \
  $(top_builddir)/sidechannel/libchannelcore.la \
  $(top_builddir)/sidechannel/libsidechannel.la \
  $(JSONCPP_LIBS) $(JSONRPCSERVER_LIBS) \
  $(GLOG_LIBS) $(GFLAGS_LIBS) $(PROTOBUF_LIBS)
ships_channel_SOURCES = main-channel.cpp \
  channelrpc.cpp
CHANNELHEADERS = \
  channelrpc.hpp

noinst_HEADERS = $(LIBSHIPSHEADERS) $(CHANNELHEADERS)

noinst_PYTHON = proto/__init__.py $(PROTOPY)

check_PROGRAMS = tests
TESTS = tests

tests_CXXFLAGS = \
  -I$(top_srcdir) \
  $(JSONCPP_CFLAGS) $(GLOG_CFLAGS) $(GTEST_CFLAGS) $(PROTOBUF_CFLAGS)
tests_LDADD = \
  $(builddir)/libships.la \
  $(top_builddir)/spacexpanseutil/libspacexpanseutil.la \
  $(top_builddir)/spacexpansegame/libtestutils.la \
  $(top_builddir)/spacexpansegame/libspex.la \
  $(top_builddir)/sidechannel/libchannelcore.la \
  $(top_builddir)/sidechannel/libsidechannel.la \
  $(top_builddir)/sidechannel/libtestutils.la \
  $(JSONCPP_LIBS) $(GLOG_LIBS) $(GTEST_LIBS) $(PROTOBUF_LIBS)
tests_SOURCES = \
  board_tests.cpp \
  channel_tests.cpp \
  coord_tests.cpp \
  gamestatejson_tests.cpp \
  grid_tests.cpp \
  logic_tests.cpp \
  schema_tests.cpp \
  \
  testutils.cpp
check_HEADERS = \
  testutils.hpp

schema.cpp: schema_head.cpp schema.sql schema_tail.cpp
	cat $^ >$@

rpc-stubs/shipschannelrpcserverstub.h: $(srcdir)/rpc-stubs/channel.json
	jsonrpcstub "$<" \
	  --cpp-server=ShipsChannelRpcServerStub \
	  --cpp-server-file="$@"

proto/%.pb.h proto/%.pb.cc: $(srcdir)/proto/%.proto
	protoc -I$(top_srcdir) -I$(srcdir)/proto --cpp_out=proto "$<"
proto/%_pb2.py: $(srcdir)/proto/%.proto
	protoc -I$(top_srcdir) -I$(srcdir)/proto --python_out=proto "$<"
