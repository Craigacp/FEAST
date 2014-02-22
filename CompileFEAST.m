%Compiles the FEAST Toolbox into a mex executable for use with MATLAB

mex -I../MIToolbox FSToolboxMex.c BetaGamma.c CMIM.c CondMI.c DISR.c ICAP.c JMI.c mRMR_D.c ../MIToolbox/MutualInformation.c ../MIToolbox/Entropy.c ../MIToolbox/CalculateProbability.c ../MIToolbox/ArrayOperations.c

