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
# Please check www.cs.manchester.ac.uk/~gbrown/fstoolbox for updates.
# 
# Copyright (c) 2010-2013, A. Pocock, G. Brown, The University of Manchester
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
CXXFLAGS = -O3 -fPIC
COMPILER = gcc
LINKER = gcc
MITOOLBOXPATH = ../MIToolbox/
objects = mRMR_D.o CMIM.o JMI.o DISR.o CondMI.o ICAP.o BetaGamma.o MIM.o

libFSToolbox.so : $(objects)
	$(LINKER) -lMIToolbox -lm -L$(MITOOLBOXPATH) -shared -o libFSToolbox.so $(objects)

mRMR_D.o: mRMR_D.c  
	$(COMPILER) $(CXXFLAGS) -DCOMPILE_C -c mRMR_D.c -I$(MITOOLBOXPATH)

CMIM.o: CMIM.c 
	$(COMPILER) $(CXXFLAGS) -DCOMPILE_C -c CMIM.c -I$(MITOOLBOXPATH)

MIM.o: MIM.c 
	$(COMPILER) $(CXXFLAGS) -DCOMPILE_C -c MIM.c -I$(MITOOLBOXPATH)

JMI.o: JMI.c 
	$(COMPILER) $(CXXFLAGS) -DCOMPILE_C -c JMI.c -I$(MITOOLBOXPATH)

DISR.o: DISR.c 
	$(COMPILER) $(CXXFLAGS) -DCOMPILE_C -c DISR.c -I$(MITOOLBOXPATH)

CondMI.o: CondMI.c 
	$(COMPILER) $(CXXFLAGS) -DCOMPILE_C -c CondMI.c -I$(MITOOLBOXPATH)

ICAP.o: ICAP.c 
	$(COMPILER) $(CXXFLAGS) -DCOMPILE_C -c ICAP.c -I$(MITOOLBOXPATH)

BetaGamma.o: BetaGamma.c 
	$(COMPILER) $(CXXFLAGS) -DCOMPILE_C -c BetaGamma.c -I$(MITOOLBOXPATH)
	
.PHONY : debug
debug:
	$(MAKE) libFSToolbox.so "CXXFLAGS = -g -DDEBUG -fPIC"
	
.PHONY : x86
x86:
	$(MAKE) libFSToolbox.so "CXXFLAGS = -O3 -fPIC -m32"
	
.PHONY : x64
x64:
	$(MAKE) libFSToolbox.so "CXXFLAGS = -O3 -fPIC -m64"
	
.PHONY : matlab
matlab:
	mex -I$(MITOOLBOXPATH) FSToolboxMex.c BetaGamma.c CMIM.c CondMI.c DISR.c ICAP.c JMI.c MIM.c mRMR_D.c $(MITOOLBOXPATH)MutualInformation.c $(MITOOLBOXPATH)Entropy.c $(MITOOLBOXPATH)CalculateProbability.c $(MITOOLBOXPATH)ArrayOperations.c
  
.PHONY : matlab-debug
matlab-debug:
	mex -g -I$(MITOOLBOXPATH) FSToolboxMex.c BetaGamma.c CMIM.c CondMI.c DISR.c ICAP.c JMI.c MIM.c mRMR_D.c $(MITOOLBOXPATH)MutualInformation.c $(MITOOLBOXPATH)Entropy.c $(MITOOLBOXPATH)CalculateProbability.c $(MITOOLBOXPATH)ArrayOperations.c

.PHONY : intel
intel:
	$(MAKE) libFSToolbox.so "COMPILER = icc" "LINKER = icc" "CXXFLAGS = -O2 -fPIC -xHost"

.PHONY : clean
clean:
	rm *.o 
	rm libFSToolbox.so

.PHONY : install
install:
	$(MAKE)
	@echo "Installing FEAST's libFSToolbox.so to $(PREFIX)/lib"
	cp -v libFSToolbox.so $(PREFIX)/lib
