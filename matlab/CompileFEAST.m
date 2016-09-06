%Compiles the FEAST Toolbox into a mex executable for use with MATLAB

mex -I../../MIToolbox/include -I../include FSToolboxMex.c ../src/BetaGamma.c ../src/CMIM.c ../src/CondMI.c ../src/DISR.c ../src/ICAP.c ../src/JMI.c ../src/mRMR_D.c ../../MIToolbox/src/MutualInformation.c ../../MIToolbox/src/Entropy.c ../../MIToolbox/src/CalculateProbability.c ../../MIToolbox/src/ArrayOperations.c

