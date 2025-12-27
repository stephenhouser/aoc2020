# Makefile - make all advent of code problems for the year
#
# 2025-10-24/Stephen Houser
#
MAKEFLAGS += --silent

# C Preprocessor flags (for c and c++ code)
CPPFLAGS = -O3 -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror -Wpedantic

# C specific flags
CC = gcc
CFLAGS = -std=c23
LFLAGS =

# C++ specific flags
CXX = g++
CXXFLAGS = -std=c++23
LXXFLAGS =

SUBDIRS := $(shell find . -mindepth 1 -maxdepth 1 -type d ! -name 'day.\*' -exec test -e '{}/Makefile' \; -print | sed 's|^./||')

.PHONY: default all clean distclean $(SUBDIRS)

default: all

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ input

# lint: $(SOURCES)
# 	clang-tidy $(SOURCES)

summary:
	@find . -mindepth 1 -maxdepth 1 -type d ! -name 'day.\*' -exec test -e '{}/Makefile' \; -print | sort | xargs -I % $(MAKE) -C '%' summary

clean:
	@find . -mindepth 1 -maxdepth 1 -type d ! -name 'day.\*' -exec test -e '{}/Makefile' \; -exec $(MAKE) -C '{}' clean \;

distclean:
	@find . -mindepth 1 -maxdepth 1 -type d ! -name 'day.\*' -exec test -e '{}/Makefile' \; -exec $(MAKE) -C '{}' distclean \;
	-rm -f summary