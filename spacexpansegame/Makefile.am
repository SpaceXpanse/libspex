lib_LTLIBRARIES = libspex.la
bin_PROGRAMS = dumpspacexpansedb
spacexpansegamedir = $(includedir)/spacexpansegame
rpcstubdir = $(spacexpansegamedir)/rpc-stubs

pkgconfig_DATA = libspex.pc

EXTRA_DIST = rpc-stubs/game.json rpc-stubs/spacexpanse.json rpc-stubs/spacexpanse-wallet.json

RPC_STUBS = \
  rpc-stubs/gamerpcclient.h \
  rpc-stubs/gamerpcserverstub.h \
  rpc-stubs/spacexpanserpcclient.h \
  rpc-stubs/spacexpanserpcserverstub.h \
  rpc-stubs/spacexpansewalletrpcclient.h \
  rpc-stubs/spacexpansewalletrpcserverstub.h
BUILT_SOURCES = $(RPC_STUBS)
CLEANFILES = $(RPC_STUBS)

libspex_la_CXXFLAGS = \
  -I$(top_srcdir) \
  $(OPENSSL_CFLAGS) \
  $(JSONCPP_CFLAGS) $(JSONRPCCLIENT_CFLAGS) $(JSONRPCSERVER_CFLAGS) \
  $(ZLIB_CFLAGS) $(CURL_CFLAGS) $(MHD_CFLAGS) \
  $(GLOG_CFLAGS) $(SQLITE3_CFLAGS) $(LMDB_CFLAGS) $(ZMQ_CFLAGS)
libspex_la_LIBADD = \
  $(top_builddir)/spacexpanseutil/libspacexpanseutil.la \
  $(OPENSSL_LIBS) \
  $(JSONCPP_LIBS) $(JSONRPCCLIENT_LIBS) $(JSONRPCSERVER_LIBS) \
  $(ZLIB_LIBS) $(CURL_LIBS) $(MHD_LIBS) \
  $(GLOG_LIBS) $(SQLITE3_LIBS) $(LMDB_LIBS) $(ZMQ_LIBS) \
  -lstdc++fs
libspex_la_SOURCES = \
  defaultmain.cpp \
  game.cpp \
  gamelogic.cpp \
  gamerpcserver.cpp \
  heightcache.cpp \
  lmdbstorage.cpp \
  mainloop.cpp \
  pendingmoves.cpp \
  pruningqueue.cpp \
  rest.cpp \
  signatures.cpp \
  sqlitegame.cpp \
  sqliteintro.cpp \
  sqliteproc.cpp \
  sqlitestorage.cpp \
  storage.cpp \
  transactionmanager.cpp \
  zmqsubscriber.cpp
spacexpansegame_HEADERS = \
  defaultmain.hpp \
  game.hpp \
  gamelogic.hpp \
  gamerpcserver.hpp \
  heightcache.hpp \
  lmdbstorage.hpp \
  mainloop.hpp \
  pendingmoves.hpp \
  pruningqueue.hpp \
  rest.hpp \
  signatures.hpp \
  sqlitegame.hpp \
  sqliteintro.hpp sqliteintro.tpp \
  sqliteproc.hpp \
  sqlitestorage.hpp \
  storage.hpp \
  transactionmanager.hpp \
  zmqsubscriber.hpp
rpcstub_HEADERS = $(RPC_STUBS)

dumpspacexpansedb_CXXFLAGS = \
  -I$(top_srcdir) \
  $(GLOG_CFLAGS) $(SQLITE3_CFLAGS) $(GFLAGS_CFLAGS)
dumpspacexpansedb_LDADD = \
  $(builddir)/libspex.la \
  $(top_builddir)/spacexpanseutil/libspacexpanseutil.la \
  $(GLOG_LIBS) $(SQLITE3_LIBS) $(GFLAGS_LIBS)
dumpspacexpansedb_SOURCES = main-dumpdb.cpp

check_LTLIBRARIES = libtestutils.la
check_PROGRAMS = tests
TESTS = tests

libtestutils_la_CXXFLAGS = \
  -I$(top_srcdir) \
  $(JSONCPP_CFLAGS) $(JSONRPCCLIENT_CFLAGS) $(JSONRPCSERVER_CFLAGS) \
  $(GLOG_CFLAGS) $(GTEST_CFLAGS)
libtestutils_la_LIBADD = \
  $(builddir)/libspex.la \
  $(top_builddir)/spacexpanseutil/libspacexpanseutil.la \
  $(JSONCPP_LIBS) $(JSONRPCCLIENT_LIBS) $(JSONRPCSERVER_LIBS) \
  $(GLOG_LIBS) $(GTEST_LIBS)
libtestutils_la_SOURCES = \
  testutils.cpp
TESTUTILHEADERS = testutils.hpp

tests_CXXFLAGS = \
  -I$(top_srcdir) \
  $(JSONCPP_CFLAGS) $(JSONRPCCLIENT_CFLAGS) $(JSONRPCSERVER_CFLAGS) \
  $(GLOG_CFLAGS) $(GTEST_CFLAGS) $(SQLITE3_CFLAGS) $(LMDB_CFLAGS) $(ZMQ_CFLAGS)
tests_LDADD = \
  $(builddir)/libtestutils.la \
  $(builddir)/libspex.la \
  $(top_builddir)/spacexpanseutil/libspacexpanseutil.la \
  $(JSONCPP_LIBS) $(JSONRPCCLIENT_LIBS) $(JSONRPCSERVER_LIBS) \
  $(GLOG_LIBS) $(GTEST_LIBS) $(SQLITE3_LIBS) $(LMDB_LIBS) $(ZMQ_LIBS)
tests_SOURCES = \
  game_tests.cpp \
  gamelogic_tests.cpp \
  heightcache_tests.cpp \
  lmdbstorage_tests.cpp \
  mainloop_tests.cpp \
  pendingmoves_tests.cpp \
  pruningqueue_tests.cpp \
  rest_tests.cpp \
  signatures_tests.cpp \
  sqlitegame_tests.cpp \
  sqliteintro_tests.cpp \
  sqliteproc_tests.cpp \
  sqlitestorage_tests.cpp \
  storage_tests.cpp \
  transactionmanager_tests.cpp \
  zmqsubscriber_tests.cpp
TESTHEADERS = storage_tests.hpp

check_HEADERS = $(TESTUTILHEADERS) $(TESTHEADERS)

rpc-stubs/gamerpcclient.h: $(srcdir)/rpc-stubs/game.json
	jsonrpcstub "$<" --cpp-client=GameRpcClient --cpp-client-file="$@"
rpc-stubs/gamerpcserverstub.h: $(srcdir)/rpc-stubs/game.json
	jsonrpcstub "$<" --cpp-server=GameRpcServerStub --cpp-server-file="$@"

rpc-stubs/spacexpanserpcclient.h: $(srcdir)/rpc-stubs/spacexpanse.json
	jsonrpcstub "$<" --cpp-client=SpaceXpanseRpcClient --cpp-client-file="$@"
rpc-stubs/spacexpanserpcserverstub.h: $(srcdir)/rpc-stubs/spacexpanse.json
	jsonrpcstub "$<" --cpp-server=SpaceXpanseRpcServerStub --cpp-server-file="$@"

rpc-stubs/spacexpansewalletrpcclient.h: $(srcdir)/rpc-stubs/spacexpanse-wallet.json
	jsonrpcstub "$<" --cpp-client=SpaceXpanseWalletRpcClient --cpp-client-file="$@"
rpc-stubs/spacexpansewalletrpcserverstub.h: $(srcdir)/rpc-stubs/spacexpanse-wallet.json
	jsonrpcstub "$<" \
	  --cpp-server=SpaceXpanseWalletRpcServerStub \
	  --cpp-server-file="$@"
