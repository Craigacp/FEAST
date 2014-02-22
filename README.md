FEAST v1.01 
===========

A FEAture Selection Toolbox for C/C++ &amp; MATLAB/OCTAVE

FEAST provides implementations of common mutual information based filter
feature selection algorithms, and an implementation of RELIEF. All
functions expect discrete inputs (except RELIEF, which does not depend
on the MIToolbox), and they return the selected feature indices. These
implementations were developed to help our research into the similarities
between these algorithms, and our results are presented in the following paper:

 Conditional Likelihood Maximisation: A Unifying Framework for Information Theoretic Feature Selection
 G. Brown, A. Pocock, M.-J.Zhao, M. Lujan
 Journal of Machine Learning Research, 13:27-66 (2012)

All FEAST code is licensed under the BSD 3-Clause License.
If you use these implementations for academic research please cite the paper above.

Contains implementations of:
   mim, mrmr, mifs, cmim, jmi, disr, cife, icap, condred, cmi, relief, fcbf, betagamma

References for these algorithms are provided in the accompanying feast.bib file (in BibTeX format).

MATLAB Example (using "data" as our feature matrix, and "labels" as the class label vector):

>> size(data)
ans = 
     (569,30)                                     %% denoting 569 examples, and 30 features

>> selectedIndices = feast('jmi',5,data,labels) %% selecting the top 5 features using the jmi algorithm
selectedIndices =

    28
    21
     8
    27
    23

>> selectedIndices = feast('mrmr',10,data,labels) %% selecting the top 10 features using the mrmr algorithm
selectedIndices =

    28
    24
    22
     8
    27
    21
    29
     4
     7
    25

>> selectedIndices = feast('mifs',5,data,labels,0.7) %% selecting the top 5 features using the mifs algorithm with beta = 0.7
selectedIndices =

    28
    24
    22
    20
    29

The library is written in ANSI C for compatibility with the MATLAB mex compiler,
except for MIM, FCBF and RELIEF, which are written in MATLAB/OCTAVE script.

If you wish to use MIM in a C program you can use the BetaGamma function with
Beta = 0, Gamma = 0, as this is equivalent to MIM (but slower than the other implementation).
MIToolbox is required to compile these algorithms, and these implementations
supercede the example implementations given in that package (they have more robust behaviour 
when used with unexpected inputs).

MIToolbox can be found at:
 http://www.cs.man.ac.uk/~gbrown/mitoolbox/
and v1.03 is included in the ZIP for the FEAST package.

Compilation instructions:
 MATLAB/OCTAVE - run CompileFEAST.m,
 Linux C shared library - use the included makefile

Update History
08/11/2011 - v1.0 - Public Release to complement the JMLR publication.
12/02/2013 - v1.01 - Bug fix for 32-bit Windows MATLAB's lcc.

