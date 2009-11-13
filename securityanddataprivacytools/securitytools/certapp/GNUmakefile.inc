# Copyright (c) Symbian Software Ltd 2008.  All rights reserved.
CPPFLAGS=-MMD -D_DEBUG -D_UNICODE -D__LINUX__ -D__TOOLS2_LINUX__ $(INC)
CXXFLAGS=-g -Wall -W -fstrict-aliasing
CFLAGS=-g -Wall -W -fstrict-aliasing
CXX=g++-3.4
CC=gcc-3.4

TOPTARGETSNORUN=all tags cleanall
TOPTARGETSRUN=run runtapi runapp1 runapp2 runapp3 runtcert runswi
TOPTARGETS=$(TOPTARGETSNORUN) $(TOPTARGETSRUN) 

.PHONY: force

