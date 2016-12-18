function [selectedFeatures, featureScores] = weighted_feast(criteria,numToSelect,weights,data,labels,varargin)
%function [selectedFeatures, featureScores] = weighted_feast(criteria,numToSelect,weights,data,labels,varargin)
%
%Provides access to the weighted feature selection algorithms in WeightedFSToolboxMex
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
% FEAST is BSD-licensed, see the LICENSE file.

%Internal FSToolbox Criteria to number mapping
%CMIM      = 3
%JMI       = 4 
%DISR      = 5 
%CMI       = 10

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
    [selectedFeatures, featureScores] = WMIM(numToSelect,weights,data,labels);
elseif (strcmpi(criteria,'cmim'))
    [selectedFeatures, featureScores] = WeightedFSToolboxMex(3,numToSelect,weights,data,labels);
elseif (strcmpi(criteria,'jmi'))
    [selectedFeatures, featureScores] = WeightedFSToolboxMex(4,numToSelect,weights,data,labels);
elseif (strcmpi(criteria,'disr'))
    [selectedFeatures, featureScores] = WeightedFSToolboxMex(5,numToSelect,weights,data,labels);
elseif (strcmpi(criteria,'cmi'))
    [selectedFeatures, featureScores] = WeightedFSToolboxMex(10,numToSelect,weights,data,labels);
else
    selectedFeatures = [];
    disp(['Unrecognised criteria ' criteria]);
end
