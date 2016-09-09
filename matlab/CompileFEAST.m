%Compiles the FEAST Toolbox into a mex executable for use with MATLAB

disp 'Compiling FEAST'
mex -I../../MIToolbox/include -I../include FSToolboxMex.c ../src/BetaGamma.c ../src/CMIM.c ../src/CondMI.c ../src/DISR.c ../src/ICAP.c ../src/JMI.c ../src/mRMR_D.c ../../MIToolbox/src/MutualInformation.c ../../MIToolbox/src/Entropy.c ../../MIToolbox/src/CalculateProbability.c ../../MIToolbox/src/ArrayOperations.c

disp 'Compiling Weighted FEAST'
mex -I../../MIToolbox/include -I../include WeightedFSToolboxMex.c ../src/WeightedCMIM.c ../src/WeightedCondMI.c ../src/WeightedDISR.c ../src/WeightedJMI.c ../../MIToolbox/src/WeightedMutualInformation.c ../../MIToolbox/src/WeightedEntropy.c ../../MIToolbox/src/Entropy.c ../../MIToolbox/src/CalculateProbability.c ../../MIToolbox/src/ArrayOperations.c

