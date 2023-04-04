lib_LTLIBRARIES = libspacexpanseutil.la
spacexpanseutildir = $(includedir)/spacexpanseutil

pkgconfig_DATA = libspacexpanseutil.pc

libspacexpanseutil_la_CXXFLAGS = \
  $(JSONCPP_CFLAGS) $(ZLIB_CFLAGS) $(OPENSSL_CFLAGS) $(GLOG_CFLAGS)
libspacexpanseutil_la_LIBADD = \
  $(JSONCPP_LIBS) $(ZLIB_LIBS) $(OPENSSL_LIBS) $(GLOG_LIBS)
libspacexpanseutil_la_SOURCES = \
  base64.cpp \
  compression.cpp \
  cryptorand.cpp \
  hash.cpp \
  jsonutils.cpp \
  random.cpp \
  uint256.cpp
spacexpanseutil_HEADERS = \
  base64.hpp \
  compression.hpp \
  cryptorand.hpp \
  hash.hpp \
  jsonutils.hpp \
  random.hpp random.tpp \
  uint256.hpp
noinst_HEADERS = \
  compression_internal.hpp

check_PROGRAMS = tests
TESTS = tests

tests_CXXFLAGS = $(JSONCPP_CFLAGS) $(ZLIB_CFLAGS) $(GLOG_CFLAGS) $(GTEST_CFLAGS)
tests_LDADD = $(builddir)/libspacexpanseutil.la \
  $(JSONCPP_LIBS) $(ZLIB_LIBS) $(GLOG_LIBS) $(GTEST_LIBS)
tests_SOURCES = \
  base64_tests.cpp \
  compression_tests.cpp \
  cryptorand_tests.cpp \
  hash_tests.cpp \
  jsonutils_tests.cpp \
  random_tests.cpp \
  uint256_tests.cpp
