ifdef BOOST_ROOT
INCLUDE_BOOST := -I$(BOOST_ROOT)
BOOST_LINK := -L$(BOOST_ROOT)/stage/lib
endif

# Assume we have a boost that contains Boost.Beast unless specified
ifdef BEAST_ROOT
INCLUDE_BEAST := -I$(BEAST_ROOT)/include
endif

PWD := $(shell pwd)

JSON_VERSION ?= 2.1.1

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:%.cpp=%.o)
PROGRAM = $(shell basename `pwd`)

CC ?= $(shell which clang || which gcc)
CXXFLAGS = -Wall -W -O -std=c++11 $(INCLUDE_BOOST) $(INCLUDE_BEAST) -I$(PWD)
LIBS = boost_system boost_thread pthread ssl crypto
LDFLAGS = $(LIBS:%=-l%) $(BOOST_LINK)

$(PROGRAM) : $(OBJECTS)	
	$(CC) $(LDFLAGS) -o $@ $<

%.o : %.cpp
	wget -nc https://github.com/nlohmann/json/releases/download/v$(JSON_VERSION)/json.hpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

.PHONY : clean
clean :
	rm -f $(PROGRAM) $(OBJECTS)
