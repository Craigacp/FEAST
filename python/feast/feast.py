Afrom ctypes import cdll, POINTER, c_uint32, c_double
import inspect


# load the library
fstoolbox_lib = cdll.LoadLibrary("libFSToolbox.so")


def handle_params(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
):
    """
    Convert all parameters from python types to the C types. The function is used 
    to handle parameters for nondiscretized functions.

    For more information see `_internal_lib_call()` for the usage and list of parameters

    :return: A tuple containing the convert c-type version of the paramters provided
    """
    _k = c_uint32(k)
    _numOfSamples = c_uint32(numOfSamples)
    _numOfFeatures = c_uint32(numOfFeatures)

    twod_fm = (c_uint32 * len(featureMatrix[0])) * len(featureMatrix)
    _featureMatrix = twod_fm()
    for i in range(len(featureMatrix)):
        _featureMatrix[i] = (c_uint32 * len(featureMatrix[i]))(*featureMatrix[i])
    
    _classColumn = (c_uint32 * len(classColumn))(*classColumn)

    if discretized:
        _outputFeatures = (c_double * len(outputFeatures))(*outputFeatures)
    else:
        _outputFeatures = (c_uint32 * len(outputFeatures))(*outputFeatures)
        
    _featureScores = (c_uint32 * len(featureScores))(*featureScores)
    
    return _k, _numOfSamples, _numOfFeatures, _featureMatrix, _classColumn, _outputFeatures, _featureScores


def _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featuresScores,
        discretized=False
):
    """
    
    :param k (python: `int`, c: `uint`):
    :param numOfSamples (python: `int`, c: `uint`):
    :param numOfFeatures (python: `int`, c: `uint`):
    :param featureMatrix (python: `list(list(int))`, c: `**uint`):
    :param classColumn (python: `list(int)`, c: `*uint`):
    :param outputFeatures (python: `list(int)`, c: `*uint`):
    :param featureScores (python: `list(double)`, c: `*double`):
    
    :param discretized [default=`False`]: 
    
    :return:
    """

    # get the name of the calling function which will determine the function name
    # from libfstoolbox to use. THIS MUST MATCH or it will fail
    # [1] -> call stack where [0] is the current function
    # [3] -> index where the function name is stored    
    internal_function = getattr(fstoolbox_lib, inspect.stack()[1][3], None)
    if callable(internal_function):
        converted_c_types = handle_params(
            k,
            numOfSamples,
            numOfFeatures,
            featureMatrix,
            classColumn,
            outputFeatures,
            featureScores,
            discretized=discretized
        )
        
        c_type_ret = internal_function(*converted_c_types)

        # convert the type back to its python type - list (pointer or array of uints)
        pytype_ret = [c_type_ret[i] for i in range(len(c_type_ret))]
        return pytype_ret

def CMIM(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    CMIM() implements a discrete version of the 
    Conditional Mutual Information Maximisation criterion, using the fast
    exact implementation from

    "Fast Binary Feature Selection using Conditional Mutual Information Maximisation"
    F. Fleuret, JMLR (2004)

    See `_internal_lib_call()` for more information.

    :return:
    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discCMIM(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def JMI(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    JMI() implements the JMI criterion from
    "Data Visualization and Feature Selection: New Algorithms for Nongaussian Data"
    H. Yang and J. Moody, NIPS (1999)
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discJMI(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def DISR(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    DISR() implements the Double Input Symmetrical Relevance criterion from
    "On the Use of Variable Complementarity for Feature Selection in Cancer Classification"
    P. Meyer and G. Bontempi, (2006)
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discDISR(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def ICAP(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    ICAP() implements the Interaction Capping criterion from 
    "Machine Learning Based on Attribute Interactions"
    A. Jakulin, PhD Thesis (2005)
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discICAP(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def CondMI(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    CondMI() implements the CMI criterion using a greedy forward search
    It returns an int array, not a uint array, as -1 is a sentinel value 
    signifying there was not enough information to select a feature.
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discCondMI(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def MIM(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    MIM() implements the MIM criterion using a greedy forward search
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discMIM(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def BetaGamma(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    betaGamma() implements the Beta-Gamma space from Brown (2009).
    This incoporates MIFS, CIFE, and CondRed.
    MIFS - "Using mutual information for selecting features in supervised neural net learning"
    R. Battiti, IEEE Transactions on Neural Networks, 1994
    CIFE - "Conditional Infomax Learning: An Integrated Framework for Feature Extraction and Fusion"
    D. Lin and X. Tang, European Conference on Computer Vision (2006)
    The Beta Gamma space is explained in our paper 
    "Conditional Likelihood Maximisation: A Unifying Framework for Mutual Information Feature Selection"
    G. Brown, A. Pocock, M.-J. Zhao, M. Lujan
    Journal of Machine Learning Research (JMLR), 2011
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discBetaGamma(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def weightedCMIM(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    weightedCMIM() implements a discrete version of the 
    Conditional Mutual Information Maximisation criterion, using the fast
    exact implementation from
    "Fast Binary Feature Selection using Conditional Mutual Information Maximisation"
    F. Fleuret, JMLR (2004)
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discWeightedCMIM(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def weightedJMI(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    WeightedJMI() implements the JMI criterion from
    "Data Visualization and Feature Selection: New Algorithms for Nongaussian Data"
    H. Yang and J. Moody, NIPS (1999)
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discWeightedJMI(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def weightedDISR(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    weightedDISR() implements the Double Input Symmetrical Relevance criterion from
    "On the Use of Variable Complementarity for Feature Selection in Cancer Classification"
    P. Meyer and G. Bontempi, (2006)
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discWeightedDISR(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )


def weightedCondMI(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    """
    weightedCondMI() implements the CMI criterion using a greedy forward search
    It returns an int array, not a uint array, as -1 is a sentinel value signifying
    there was not enough information to select a feature.
    
    See `_internal_lib_call()` for more information.

    """
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=False
    )


def discWeightedCondMI(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
    return _internal_lib_call(
        k,
        numOfSamples,
        numOfFeatures,
        featureMatrix,
        classColumn,
        outputFeatures,
        featureScores,
        discretized=True
    )
