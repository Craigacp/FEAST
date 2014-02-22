% RELIEF - Kira & Rendell 1992
% T is number of patterns to use
% Defaults to all patterns if not specified.
%
% The license is in the license.txt provided.
%
% function w = RELIEF( data, labels, T ) 
%
function [w bestidx] = RELIEF ( data, labels, T )

if ~exist('T','var')
    T=size(data,1);
end

idx = randperm(length(labels));
idx = idx(1:T);

w = zeros(size(data,2),1);
for t = 1:T
    
    x = data(idx(t),:);
    y = labels(idx(t));
    
    %copy the x
    protos = repmat(x, length(labels), 1);
    %measure the distance from x to every other example
    distances = [sqrt(sum((data-protos).^2,2)) labels];
    %sort them according to distances (find nearest neighbours)
    [distances originalidx] = sortrows(distances,1);
   
    foundhit = false;  hitidx=0;
    foundmiss = false; missidx=0;
    i=2; %start from the second one
    while (~foundhit || ~foundmiss)

        if distances(i,2) == y
            hitidx = originalidx(i);
            foundhit = true;
        end
        if distances(i,2) ~= y
            missidx = originalidx(i);
            foundmiss = true;
        end
        
        i=i+1;

    end
    
    alpha = 1/T;
    for f = 1:size(data,2)%each feature
        hitpenalty  = (x(f)-data(hitidx,f))  / (max(data(:,f))-min(data(:,f)));
        misspenalty = (x(f)-data(missidx,f)) / (max(data(:,f))-min(data(:,f)));
        
        w(f) = w(f) - alpha*hitpenalty^2 + alpha*misspenalty^2;
    end
    
end

[~,bestidx] = sort(w,'descend');


