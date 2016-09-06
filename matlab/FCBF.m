function [selectedFeatures] = FCBF(featureMatrix,classColumn,threshold)
%function [selectedFeatures] = FCBF(featureMatrix,classColumn,threshold)
%
%Performs feature selection using the FCBF measure by Yu and Liu 2004.
%
%Instead of selecting a fixed number of features it provides a relevancy threshold and selects all
%features which score above that and are not redundant
%
% The license is in the license.txt provided.

numFeatures = size(featureMatrix,2);
classScore = zeros(numFeatures,1);

for i = 1:numFeatures
    classScore(i) = SU(featureMatrix(:,i),classColumn);
end

[classScore indexScore] = sort(classScore,1,'descend');

indexScore = indexScore(classScore > threshold);
classScore = classScore(classScore > threshold);

if ~isempty(indexScore)
    curPosition = 1;
else
    curPosition = 0;
end
    
while curPosition <= length(indexScore)
    j = curPosition + 1;
    curFeature = indexScore(curPosition);
    while j <= length(indexScore)
        scoreij = SU(featureMatrix(:,curFeature),featureMatrix(:,indexScore(j)));
        if scoreij > classScore(j)
            indexScore(j) = [];
            classScore(j) = [];
        else
            j = j + 1;
        end
    end
    curPosition = curPosition + 1;
end

selectedFeatures = indexScore;

end

function [score] = SU(firstVector,secondVector)
%function [score] = SU(firstVector,secondVector)
%
%calculates SU = 2 * (I(X;Y)/(H(X) + H(Y)))

hX = h(firstVector);
hY = h(secondVector);
iXY = mi(firstVector,secondVector);

score = (2 * iXY) / (hX + hY);
end
