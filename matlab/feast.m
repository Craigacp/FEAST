function [selectedFeatures, featureScores] = feast(criteria,numToSelect,data,labels,varargin)
%function [selectedFeatures, featureScores] = feast(criteria,numToSelect,data,labels,varargin)
%
%Provides access to the feature selection algorithms in FSToolboxMex
%
%Expects the features to be columns of the data matrix, and
%requires that both the features and labels are integers.
%
%Algorithms are called as follows
%
%[selectedFeatures] = feast('algName',numToSelect,data,labels)
% where algName is:
%  mim, mrmr, cmim, jmi, disr, cife, icap, condred, cmi, relief
%
%[selectedFeatures] = feast('algName',numToSelect,data,labels,beta)
% where algName is:
%  mifs (defaults to beta = 1.0 if unspecified)
%
%[selectedFeatures] = feast('algName',numToSelect,data,labels,beta,gamma)
% where algName is:
% betagamma
%[selectedFeatures] = feast('algName',numToSelect,data,labels,threshold)
% where algName is:
% fcbf (note this ignores the numToSelect)
%
% FEAST is BSD-licensed, see the LICENSE file.


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

if (~isa(data,'double') || ~isa(labels,'double'))
  error('Error, inputs must be double vectors or matrices')
end

finiteDataCount = sum(sum(isfinite(data)));
finiteLabelsCount = sum(sum(isfinite(labels)));

totalData = numel(data);
totalLabels = numel(labels);

if ((finiteDataCount ~= totalData) || (finiteLabelsCount ~= totalLabels))
    error(['Some elements are NaNs or infinite. Please check your data']);
end

if (strcmpi(criteria,'mim'))
    [selectedFeatures, featureScores] = MIM(numToSelect,data,labels);
elseif (strcmpi(criteria,'mifs'))
    if (nargin == 4)
        beta = 1;
    else
        beta = varargin{1};
    end
    [selectedFeatures, featureScores] = FSToolboxMex(1,numToSelect,data,labels,beta);
elseif (strcmpi(criteria,'mrmr'))
    [selectedFeatures, featureScores] = FSToolboxMex(2,numToSelect,data,labels);
elseif (strcmpi(criteria,'cmim'))
    [selectedFeatures, featureScores] = FSToolboxMex(3,numToSelect,data,labels);
elseif (strcmpi(criteria,'jmi'))
    [selectedFeatures, featureScores] = FSToolboxMex(4,numToSelect,data,labels);
elseif (strcmpi(criteria,'disr'))
    [selectedFeatures, featureScores] = FSToolboxMex(5,numToSelect,data,labels);
elseif ((strcmpi(criteria,'cife')) || (strcmpi(criteria,'fou')))
    [selectedFeatures, featureScores] = FSToolboxMex(6,numToSelect,data,labels);
elseif (strcmpi(criteria,'icap'))
    [selectedFeatures, featureScores] = FSToolboxMex(7,numToSelect,data,labels);
elseif (strcmpi(criteria,'condred'))
    [selectedFeatures, featureScores] = FSToolboxMex(8,numToSelect,data,labels);
elseif (strcmpi(criteria,'betagamma'))
    if (nargin ~= 6)
        error('BetaGamma criteria expects a beta and a gamma');
    else
        beta = varargin{1};
        gamma = varargin{2};
    end
    [selectedFeatures, featureScores] = FSToolboxMex(9,numToSelect,data,labels,beta,gamma);
elseif (strcmpi(criteria,'cmi'))
    [selectedFeatures, featureScores] = FSToolboxMex(10,numToSelect,data,labels);
elseif (strcmpi(criteria,'fcbf'))
    if (nargin == 4)
        error('Threshold for FCBF not supplied');
    else
        selectedFeatures = FCBF(data,labels,varargin{1});
    end
elseif (strcmpi(criteria,'relief'))
    [tmp selectedFeatures] = RELIEF(data,labels);
else
    selectedFeatures = [];
    disp(['Unrecognised criteria ' criteria]);
end
