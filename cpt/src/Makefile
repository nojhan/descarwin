#!/bin/sh
#
# Makefile for CPT
#

MAKEFLAGS += -j3

CC     	= gcc
AR	= ar
BISON   	= bison
FLEX    	= flex

EXEDIR = ../bin
LIBDIR = ../lib
LIBINCDIR = ../include

CFLAGS =  -O3 -Wall -g -fopenmp
EXE     	= $(EXEDIR)/cpt
RATP     	= $(EXEDIR)/cpt-ratp
LIB      	= $(LIBDIR)/libcpt.a
LDLIBS   =  -lm -lgmp -fopenmp
#-static

FLEX_SOURCES = \
	parser.lex

BISON_SOURCES = \
	parser.y

SOURCES =  \
	pddl.c \
	instantiation.c \
	var.c \
	options.c \
	max_atom.c \
	problem.c \
	plan.c \
	preprocess.c \
	propagations.c \
	resources.c \
	scheduling.c \
	heuristics.c \
	branching.c \
	trace_planner.c \
	control.c \
	yahsp.c \
	solve.c

EXESOURCES = \
	main.c

RATPSOURCES = \
	ratp.c

LIBSOURCES =  \
	dae.c

ALLSOURCES = $(BISON_SOURCES) $(FLEX_SOURCES) $(SOURCES) $(EXESOURCES) $(RATPSOURCES) $(LIBSOURCES)

GDSL_DIR = gdsl

GDSL_STATIC = $(GDSL_DIR)/src/.libs/libgdsl.a 

BISON_GEN = $(BISON_SOURCES:.y=.tab.c)
FLEX_GEN =  $(FLEX_SOURCES:.lex=.yy.c)

OBJECTS = $(BISON_GEN:.c=.o)  $(FLEX_GEN:.c=.o) $(SOURCES:.c=.o) 
EXEOBJECTS = $(EXESOURCES:.c=.o) 
RATPOBJECTS = $(RATPSOURCES:.c=.o) 
LIBOBJECTS = $(LIBSOURCES:.c=.o) 

STATIC_LIBS = $(GDSL_STATIC) 



.PHONY : all pp ratp dep clean distclean win

all: lib exe

exe: $(EXE) 

lib: $(LIB)

pp: CC = g++
pp: exe

ratp: CPPFLAGS += -DSCHED 
ratp: LDLIBS += -lX11
ratp: $(RATP) 

wct: CFLAGS += -DWALLCLOCK_TIME
wct: exe

win: GDSL_OPT = CFLAGS="-O3 -mno-cygwin"
win: CPPFLAGS += -DWALLCLOCK_TIME -DSCHED
win: CFLAGS = -mno-cygwin
win: LDLIBS = -static -mno-cygwin
win: STATIC_LIBS += /usr/lib/mingw/libgmp.a
win: $(RATP)

$(EXE): $(GDSL_STATIC) $(OBJECTS) $(EXEOBJECTS)  
	$(CC) -o $@  $(OBJECTS) $(EXEOBJECTS) $(STATIC_LIBS) $(LDLIBS)

$(RATP): $(GDSL_STATIC) $(OBJECTS) $(RATPOBJECTS)  
	$(CC) -o $@  $(OBJECTS) $(RATPOBJECTS) $(STATIC_LIBS) $(LDLIBS)

$(LIB): dae.h $(GDSL_STATIC) $(OBJECTS) $(LIBOBJECTS)
	cp dae.h $(LIBINCDIR)/dae.h
	cp $(GDSL_STATIC) $@
	$(AR) rcs $@ $(OBJECTS) $(LIBOBJECTS)

$(BISON_GEN): $(BISON_SOURCES)
	$(BISON) $(BISON_SOURCES)

$(FLEX_GEN): $(FLEX_SOURCES) $(BISON_GEN)
	$(FLEX) $(FLEX_SOURCES)

$(GDSL_STATIC):
	cd $(GDSL_DIR) ; ./configure $(GDSL_OPT) ; make

dep:
	makedepend -- $(ALLSOURCES) 

depr:
	makedepend -- -DSCHED -- $(ALLSOURCES) 

clean:
	rm -f $(EXE) $(LIB) $(BISON_GEN) $(FLEX_GEN) $(OBJECTS) $(EXEOBJECTS)  $(RATPOBJECTS) $(LIBOBJECTS) parser.tab.h *~ core

distclean: clean
	rm -f $(GDSL_STATIC) ; cd $(GDSL_DIR) ; make distclean

