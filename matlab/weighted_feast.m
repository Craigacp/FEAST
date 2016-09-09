function [selectedFeatures] = weighted_feast(criteria,numToSelect,weights,data,labels,varargin)
%function [selectedFeatures] = weighted_feast(criteria,numToSelect,weights,data,labels,varargin)
%
%Provides access to the feature selection algorithms in FSToolboxMex
%
%Expects the features to be columns of the data matrix, and
%requires that both the features and labels are integers.
%
%Algorithms are called as follows
%
%[selectedFeatures] = weighted_feast('algName',numToSelect,weights,data,labels)
% where algName is:
%  mim, cmim, jmi, disr
%
% The license is in the license.txt provided.


%Internal FSToolbox Criteria to number mapping
%MIFS      = 1
%mRMR      = 2
%CMIM      = 3
%JMI       = 4 
%DISR      = 5 
%CIFE      = 6 
%ICAP      = 7 
%CondRed   = 8 
%BetaGamma = 9 
%CMI       = 10
%

if ((numToSelect < 1) || (numToSelect > size(data,2)))
    error(['You have requested ' num2str(numToSelect) ' features, which is not possible']);
end

finiteDataCount = sum(sum(isfinite(data)));
finiteLabelsCount = sum(sum(isfinite(labels)));

totalData = numel(data);
totalLabels = numel(labels);

if ((finiteDataCount ~= totalData) || (finiteLabelsCount ~= totalLabels))
    error(['Some elements are NaNs or infinite. Please check your data']);
end

if (strcmpi(criteria,'mim'))
    selectedFeatures = WMIM(numToSelect,weights,data,labels);
elseif (strcmpi(criteria,'cmim'))
    selectedFeatures = WeightedFSToolboxMex(3,numToSelect,weights,data,labels);
elseif (strcmpi(criteria,'jmi'))
    selectedFeatures = WeightedFSToolboxMex(4,numToSelect,weights,data,labels);
elseif (strcmpi(criteria,'disr'))
    selectedFeatures = WeightedFSToolboxMex(5,numToSelect,weights,data,labels);
elseif (strcmpi(criteria,'cmi'))
    selectedFeatures = WeightedFSToolboxMex(10,numToSelect,weights,data,labels);
else
    selectedFeatures = [];
    disp(['Unrecognised criteria ' criteria]);
end
