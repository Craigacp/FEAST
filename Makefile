# makefile for FEAST
# Author: Adam Pocock, apocock@cs.man.ac.uk
# Created: 29/06/2011
#
# Part of the FEAture Selection Toolbox, please reference
# "Conditional Likelihood Maximisation: A Unifying Framework for Information 
# Theoretic Feature Selection"
# G. Brown, A. Pocock, M.-J. Zhao, M. Lujan
# Journal of Machine Learning Research (JMLR), 2012
#
# Please check www.github.com/Craigacp/FEAST for updates.
# 
# Copyright (c) 2010-2017, A. Pocock, G. Brown, The University of Manchester
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
#   - Redistributions of source code must retain the above copyright notice, this 
#     list of conditions and the following disclaimer.
#   - Redistributions in binary form must reproduce the above copyright notice, 
#     this list of conditions and the following disclaimer in the documentation 
#     and/or other materials provided with the distribution.
#   - Neither the name of The University of Manchester nor the names of its 
#     contributors may be used to endorse or promote products derived from this 
#     software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

PREFIX = /usr/local
CFLAGS = -O3 -fPIC -std=c89 -Wall -Werror
CC = gcc
LINKER = gcc
INCLUDES = -I../MIToolbox/include -Iinclude
JNI_INCLUDES = -I/usr/lib/jvm/java-8-openjdk-amd64/include/ -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux
JAVA_INCLUDES = -Ijava/native/include
objects = build/BetaGamma.o build/CMIM.o build/CondMI.o build/DISR.o build/ICAP.o build/JMI.o build/MIM.o build/mRMR_D.o build/WeightedCMIM.o build/WeightedCondMI.o build/WeightedDISR.o build/WeightedJMI.o build/WeightedMIM.o

libFSToolbox.so : $(objects)
	$(LINKER) $(CFLAGS) -shared -o libFSToolbox.so $(objects) -lm -lMIToolbox

build/%.o: src/%.c 
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDES) -DCOMPILE_C -o build/$*.o -c $<
	
java: java/build/native/lib/libfeast-java.so

java/build/native/lib/libfeast-java.so: java/src/native/FEASTJNI.c java/src/native/WeightedFEASTJNI.c
	$(CC) $(CFLAGS) $(INCLUDES) $(JNI_INCLUDES) $(JAVA_INCLUDES) -DCOMPILE_C -shared -o $@ java/src/native/FEASTJNIUtil.c java/src/native/FEASTJNI.c java/src/native/WeightedFEASTJNI.c -lm -lMIToolbox -lFSToolbox

.PHONY : debug x86 x64 intel clean install

debug:
	$(MAKE) libFSToolbox.so "CXXFLAGS = -g -DDEBUG -fPIC"
	
x86:
	$(MAKE) libFSToolbox.so "CXXFLAGS = -O3 -fPIC -m32"
	
x64:
	$(MAKE) libFSToolbox.so "CXXFLAGS = -O3 -fPIC -m64"

intel:
	$(MAKE) libFSToolbox.so "COMPILER = icc" "LINKER = icc" "CXXFLAGS = -O2 -fPIC -xHost"

clean:
	-rm -fr build
	-rm -f matlab/*.o matlab/*.mex*
	-rm -f libFSToolbox.so

install:
	$(MAKE)
	@echo "Installing FEAST's libFSToolbox.so to $(PREFIX)/lib"
	@cp -v libFSToolbox.so $(PREFIX)/lib
	@echo "Installing FEAST's header files to $(PREFIX)/include/FEAST"
	@mkdir -p $(PREFIX)/include
	@cp -vr include/FEAST $(PREFIX)/include/
