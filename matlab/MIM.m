function [selectedFeatures scoreVector] = MIM(k, data, labels)
%function [selectedFeatures scoreVector] = MIM(k, data, labels)
%
%Mutual information Maximisation
%
% The license is in the LICENSE file.

numf = size(data,2);
classMI = zeros(numf,1);

for n = 1 : numf
	classMI(n) = mi(data(:,n),labels);
end

[scoreVector index] = sort(classMI,'descend');

selectedFeatures = index(1:k);
scoreVector = scoreVector(1:k);

