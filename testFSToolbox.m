datafiles = { 'breast', 'congress', 'heart', ...
              'krvskp', 'landsat', ...
              'lungcancer', 'parkinsons', 'semeion', ...
              'sonar', 'soybeansmall', 'spect', ...
              'splice', 'waveform', 'wine', 'ionosphere' };

criteria = { 'mim', 'jmi','disr', ...
                    'condred', 'mrmr', 'cmim', ...
                    'mifs', 'icap', 'cife', 'cmi' };
passed = 0;

for i = 1:length(datafiles)
    datafile = [ datafiles{i} 'EW' ];
    fprintf([ 'Data: ' char(datafile) '\t\t\n']);

    load(datafile);
    
    numF = size(data,2);
    for k = 1:numF

        disp('New time');
        tic;
        for j = 1:length(criteria)
            currentFeatures{i,j} = FSToolbox(criteria{j},k,data,labels);
        end
        toc;

        disp('Old time');
        tic;
        mimFeatures{i} = mim(k,data,labels);
        jmiFeatures{i} = JMI_Mex(k,data,labels);
        disrFeatures{i} = DISR_Mex(k,data,labels);
        mrmrFeatures{i} = mRMR_D_Mex(k,data,labels);
        cmimFeatures{i} = CMIM_Mex(k,data,labels);
        icapFeatures{i} = ICAP_Mex(k,data,labels);
        mifsFeatures{i} = MIFSMex(k,data,labels);
        cifeFeatures{i} = FOUMex(k,data,labels);
        condredFeatures{i} = BetaGammaMex(k,data,labels,0.0,1.0);
        cmiFeatures{i} = CondMI_Mex(k,data,labels);
        toc;

        disp('Testing for equality');
        passedTests = 0;

        if (sum(mimFeatures{i} == currentFeatures{i,1}) ~= k)
            disp(['Error in MIM in ' datafile]);
            passedTests = 1;
        else
            passed = passed + 1;
        end
        if (sum(jmiFeatures{i}  == currentFeatures{i,2}) ~= k)
            disp(['Error in JMI in ' datafile]); 
            passedTests = 1;
        else
            passed = passed + 1;
        end
        if (sum(disrFeatures{i} == currentFeatures{i,3}) ~= k)
            disp(['Error in DISR in ' datafile]);
            passedTests = 1;
        else
            passed = passed + 1;
        end
        if (sum(condredFeatures{i} == currentFeatures{i,4}) ~= k)
            disp(['Error in CondRed in ' datafile]);
            passedTests = 1;
        else
            passed = passed + 1;
        end
        if (sum(mrmrFeatures{i} == currentFeatures{i,5}) ~= k)
            disp(['Error in mRMR in ' datafile]);
            passedTests = 1;
        else
            passed = passed + 1;
        end
        if (sum(cmimFeatures{i} == currentFeatures{i,6}) ~= k)
            disp(['Error in CMIM in ' datafile]);
            passedTests = 1;
        else
            passed = passed + 1;
        end
        if (sum(mifsFeatures{i} == currentFeatures{i,7}) ~= k)
            disp(['Error in MIFS in ' datafile]);
            passedTests = 1;
        else
            passed = passed + 1;
        end
        if (sum(icapFeatures{i} == currentFeatures{i,8}) ~= k)
            disp(['Error in ICAP in ' datafile]);
            passedTests = 1;
        else
            passed = passed + 1;
        end
        if (sum(cifeFeatures{i} == currentFeatures{i,9}) ~= k)
            disp(['Error in CIFE in ' datafile]);
            passedTests = 1;
        else
            passed = passed + 1;
        end
        if (sum(cmiFeatures{i} == currentFeatures{i,10}) ~= length(cmiFeatures{i}))
            disp(['Error in CMI in ' datafile]);
            passedTests = 1;
        else
            passed = passed + 1;
        end

        if (passedTests == 0)
            disp('Passed equality tests');
        end
    end
end

passed
