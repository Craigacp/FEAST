from ctypes import cdll, POINTER, c_uint, c_double
import inspect
from numpy import zeros
import numpy as np
from numpy.ctypeslib import ndpointer
from logging import getLogger


log = getLogger()

# load the library
fstoolbox_lib = cdll.LoadLibrary("libFSToolbox.so")

_uintpp = ndpointer(dtype=np.uintp, ndim=1, flags='F')
_doublepp = ndpointer(dtype=np.uintp, ndim=1, flags='C')

_library_functions = ["CMIM", "CondMI", "DISR", "ICAP", "JMI", "MIM", "mRMR_D"]
_library_weighted_functions = [
    "weightedCMIM", "weightedCondMI", "weightedDISR", "weightedJMI", "weightedMIM"
]
_disc_library_functions = [
    "discCMIM", "discCondMI", "discDISR", "discICAP", 
    "discJMI", "discMIM", "disc_mRMR_D"
]
_disc_weighted_library_functions = [
    "discWeightedCMIM", "discWeightedCondMI", "discWeightedDISR", 
    "discWeightedJMI", "discWeightedMIM"
]

# Set the expected argtypes and return types for library functions
for func_name in _library_functions:
    f = getattr(fstoolbox_lib, func_name, None)
    if f is not None:
        f.argtypes = [
            c_uint, c_uint, c_uint, _uintpp, POINTER(c_uint), POINTER(c_uint), POINTER(c_double)
        ]
        f.restype = POINTER(c_uint)
    else:
        log.warning("Failed to set types for %s", func_name)

# Set the expected argtypes and return types for discrete library functions
for func_name in _disc_library_functions:
    f = getattr(fstoolbox_lib, func_name, None)
    if f is not None:
        f.argtypes = [
            c_uint, c_uint, c_uint, _doublepp, POINTER(c_double), 
            POINTER(c_double), POINTER(c_double)
        ]
        f.restype = POINTER(c_double)
    else:
        log.warning("Failed to set types for %s", func_name)

# Set the expected argtypes and return types for weighted library functions
for func_name in _library_weighted_functions:
    f = getattr(fstoolbox_lib, func_name, None)
    if f is not None:
        f.argtypes = [
            c_uint, c_uint, c_uint, _uintpp, POINTER(c_uint), POINTER(c_double), 
            POINTER(c_uint), POINTER(c_double)
        ]
        f.restype = POINTER(c_uint)
    else:
        log.warning("Failed to set types for %s", func_name)

# Set the expected argtypes and return types for discrete library functions
for func_name in _disc_weighted_library_functions:
    f = getattr(fstoolbox_lib, func_name, None)
    if f is not None:
        f.argtypes = [
            c_uint, c_uint, c_uint, _doublepp, POINTER(c_double), POINTER(c_double), 
            POINTER(c_double), POINTER(c_double)
        ]
        f.restype = POINTER(c_double)
    else:
        log.warning("Failed to set types for %s", func_name)

# Special Cases - BetaGamma
fstoolbox_lib.BetaGamma.argtypes = [
    c_uint, c_uint, c_uint, _uintpp, POINTER(c_uint), POINTER(c_uint), 
    POINTER(c_double), c_double, c_double
]
fstoolbox_lib.BetaGamma.restype = POINTER(c_uint)
fstoolbox_lib.discBetaGamma.argtypes = [
    c_uint, c_uint, c_uint, _doublepp, POINTER(c_double), POINTER(c_double), 
    POINTER(c_double), c_double, c_double
]
fstoolbox_lib.discBetaGamma.restype = POINTER(c_double)


def FeatureSelectionCommon(data_set, labels, num_features_to_select, *args, **kwargs):
    '''See calling function for more information'''
    # grab the function name as it matches the one from the C-library
    internal_function = getattr(fstoolbox_lib, inspect.stack()[1][3], None)
    
    if internal_function is None:
        raise RuntimeError("Failed to find %s in fstoolbox", str(inspect.stack()[1][3]))
    else:
        log.info("Running %s", str(inspect.stack()[1][3]))

    _data_set = np.array(data_set, dtype=np.uint32, order="F")
    _labels = np.array(labels, dtype=np.uint32)
    
    num_observations, num_features = _data_set.shape
    output = zeros(num_features_to_select).astype(np.uint)
    selected_features_scores = zeros(num_features)

    # convert/create the types to C-Types
    c_num_features_to_select = c_uint(num_features_to_select)
    c_num_observations = c_uint(num_observations)
    c_num_features = c_uint(num_features)    
    c_output_features = output.ctypes.data_as(POINTER(c_uint))
    
    # filled in feature scores through the library
    selected_features_scores = zeros(num_features)
    c_selected_feature_types = selected_features_scores.ctypes.data_as(POINTER(c_double))
    
    # convert the parameters to the C-Type variables expected
    c_labels = _labels.ctypes.data_as(POINTER(c_uint))
    c_feature_matrix = (
        _data_set.__array_interface__['data'][0] +\
            np.arange(_data_set.shape[1]) * (_data_set.strides[1])).astype(np.uintp)
    
    if len(args) > 0:
        features = internal_function(
            c_num_features_to_select,
            c_num_observations,
            c_num_features, 
            c_feature_matrix,
            c_labels,
            c_output_features,
            c_selected_feature_types,
            *args
        )
    else:    
        features = internal_function(
            c_num_features_to_select,
            c_num_observations,
            c_num_features, 
            c_feature_matrix,
            c_labels,
            c_output_features,
            c_selected_feature_types
        )

    # result transition from C to Python
    features_iterator = np.fromiter(features, dtype=np.uint, count=num_features_to_select)
    selected_features = []
    for c_selected_feature_index in features_iterator:
        selected_features.append(c_selected_feature_index)

    # store the selection results
    feature_scores = [c_selected_feature_types[idx] for idx in range(num_features_to_select)]

    return selected_features, feature_scores


def discFeatureSelectionCommon(data_set, labels, num_features_to_select, *args, **kwargs):
    '''See calling function for more information'''
    # grab the function name as it matches the one from the C-library
    internal_function = getattr(fstoolbox_lib, inspect.stack()[1][3], None)
    
    if internal_function is None:
        raise RuntimeError("Failed to find %s in fstoolbox", str(inspect.stack()[1][3]))
    else:
        log.info("Running %s", str(inspect.stack()[1][3]))
        
    _data_set = np.array(data_set, dtype=np.uint32, order="C")
    _labels = np.array(labels, dtype=np.uint32)
    
    num_observations, num_features = _data_set.shape
    output = zeros(num_features_to_select).astype(np.uint)
    selected_features_scores = zeros(num_features)

    # convert/create the types to C-Types
    c_num_features_to_select = c_uint(num_features_to_select)
    c_num_observations = c_uint(num_observations)
    c_num_features = c_uint(num_features)    
    c_output_features = output.ctypes.data_as(POINTER(c_double))
    
    # filled in feature scores through the library
    selected_features_scores = zeros(num_features)
    c_selected_feature_types = selected_features_scores.ctypes.data_as(POINTER(c_double))
    
    # convert the parameters to the C-Type variables expected
    c_labels = _labels.ctypes.data_as(POINTER(c_double))
    c_feature_matrix = (_data_set.__array_interface__['data'][0] +\
        np.arange(_data_set.shape[0]) * (_data_set.strides[0])).astype(np.uintp)
    
    if len(args) > 0:
        features = internal_function(
            c_num_features_to_select,
            c_num_observations,
            c_num_features, 
            c_feature_matrix,
            c_labels,
            c_output_features,
            c_selected_feature_types,
            *args
        )
    else:    
        features = internal_function(
            c_num_features_to_select,
            c_num_observations,
            c_num_features, 
            c_feature_matrix,
            c_labels,
            c_output_features,
            c_selected_feature_types
        )

    # result transition from C to Python
    features_iterator = np.fromiter(features, dtype=np.double, count=num_features_to_select)
    selected_features = []
    for c_selected_feature_index in features_iterator:
        selected_features.append(c_selected_feature_index)

    # store the selection results
    feature_scores = [c_selected_feature_types[idx] for idx in range(num_features_to_select)]

    return selected_features, feature_scores


def WeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select):
    '''See calling function for more information'''
    # grab the function name as it matches the one from the C-library
    internal_function = getattr(fstoolbox_lib, inspect.stack()[1][3], None)
    
    if internal_function is None:
        raise RuntimeError("Failed to find %s in fstoolbox", str(inspect.stack()[1][3]))
    else:
        log.info("Running %s", str(inspect.stack()[1][3]))

    _data_set = np.array(data_set, dtype=np.uint32, order="F")
    _labels = np.array(labels, dtype=np.uint32)
    _weights = np.array(weights, dtype=np.double)
    
    num_observations, num_features = _data_set.shape
    output = zeros(num_features_to_select).astype(np.uint)
    selected_features_scores = zeros(num_features)

    # convert/create the types to C-Types
    c_num_features_to_select = c_uint(num_features_to_select)
    c_num_observations = c_uint(num_observations)
    c_num_features = c_uint(num_features)    
    c_output_features = output.ctypes.data_as(POINTER(c_uint))
    
    # filled in feature scores through the library
    selected_features_scores = zeros(num_features)
    c_selected_feature_types = selected_features_scores.ctypes.data_as(POINTER(c_double))
    
    # convert the parameters to the C-Type variables expected
    c_labels = _labels.ctypes.data_as(POINTER(c_uint))
    c_weights = _weights.ctypes.data_as(POINTER(c_double))
    c_feature_matrix = (
        _data_set.__array_interface__['data'][0] +\
            np.arange(_data_set.shape[1]) * (_data_set.strides[1])).astype(np.uintp)
      
    features = internal_function(
        c_num_features_to_select,
        c_num_observations,
        c_num_features, 
        c_feature_matrix,
        c_labels,
        c_weights,
        c_output_features,
        c_selected_feature_types
    )

    # result transition from C to Python
    features_iterator = np.fromiter(features, dtype=np.uint, count=num_features_to_select)
    selected_features = []
    for c_selected_feature_index in features_iterator:
        selected_features.append(c_selected_feature_index)

    # store the selection results
    feature_scores = [c_selected_feature_types[idx] for idx in range(num_features_to_select)]

    return selected_features, feature_scores


def discWeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select):
    '''See calling function for more information'''
    # grab the function name as it matches the one from the C-library
    internal_function = getattr(fstoolbox_lib, inspect.stack()[1][3], None)
    
    if internal_function is None:
        raise RuntimeError("Failed to find %s in fstoolbox", str(inspect.stack()[1][3]))
    else:
        log.info("Running %s", str(inspect.stack()[1][3]))
    
    _data_set = np.array(data_set, dtype=np.uint32, order="C")
    _labels = np.array(labels, dtype=np.uint32)
    _weights = np.array(weights, dtype=np.double)
    
    num_observations, num_features = _data_set.shape
    output = zeros(num_features_to_select).astype(np.uint)
    selected_features_scores = zeros(num_features)

    # convert/create the types to C-Types
    c_num_features_to_select = c_uint(num_features_to_select)
    c_num_observations = c_uint(num_observations)
    c_num_features = c_uint(num_features)    
    c_output_features = output.ctypes.data_as(POINTER(c_double))
    
    # filled in feature scores through the library
    selected_features_scores = zeros(num_features)
    c_selected_feature_types = selected_features_scores.ctypes.data_as(POINTER(c_double))
    
    # convert the parameters to the C-Type variables expected
    c_labels = _labels.ctypes.data_as(POINTER(c_double))
    c_weights = _weights.ctypes.data_as(POINTER(c_double))
    c_feature_matrix = (_data_set.__array_interface__['data'][0] +\
        np.arange(_data_set.shape[0]) * (_data_set.strides[0])).astype(np.uintp)
    
    features = internal_function(
        c_num_features_to_select,
        c_num_observations,
        c_num_features, 
        c_feature_matrix,
        c_labels,
        c_weights,
        c_output_features,
        c_selected_feature_types
    )

    # result transition from C to Python
    features_iterator = np.fromiter(features, dtype=np.double, count=num_features_to_select)
    selected_features = []
    for c_selected_feature_index in features_iterator:
        selected_features.append(c_selected_feature_index)

    # store the selection results
    feature_scores = [c_selected_feature_types[idx] for idx in range(num_features_to_select)]

    return selected_features, feature_scores

def BetaGamma(data_set, labels, num_features_to_select, beta, gamma):
    '''Python implementation of the FEAST::BetaGamma Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/BetaGamma.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :param beta: Penalty attached to I(X_j;X_k); Value between 0.0 and 1.0 
    :param gamma: Positive weight attached to I(X_k;X_j|Y); Value between 0.0 and 1.0 
    :return: `num_features_to_select` Features in order that they were selected
    '''
    c_beta = c_double(beta)
    c_gamma = c_double(gamma)
    return FeatureSelectionCommon(data_set, labels, num_features_to_select, c_beta, c_gamma)


def discBetaGamma(data_set, labels, num_features_to_select, beta, gamma):
    '''Python implementation of the FEAST::discBetaGamma Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/BetaGamma.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :param beta: Penalty attached to I(X_j;X_k); Value between 0.0 and 1.0 
    :param gamma: Positive weight attached to I(X_k;X_j|Y); Value between 0.0 and 1.0 
    :return: `num_features_to_select` Features in order that they were selected
    '''
    c_beta = c_double(beta)
    c_gamma = c_double(gamma)
    return discFeatureSelectionCommon(data_set, labels, num_features_to_select, c_beta, c_gamma)


def CMIM(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::CMIM Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/CMIM.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return FeatureSelectionCommon(data_set, labels, num_features_to_select)


def discCMIM(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::discCMIM Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/CMIM.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discFeatureSelectionCommon(data_set, labels, num_features_to_select)


def CondMI(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::CondMI Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/CondMI.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return FeatureSelectionCommon(data_set, labels, num_features_to_select)


def discCondMI(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::discCondMI Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/CondMI.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discFeatureSelectionCommon(data_set, labels, num_features_to_select)


def DISR(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::DISR Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/DISR.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return FeatureSelectionCommon(data_set, labels, num_features_to_select)


def discDISR(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::discDISR Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/DISR.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discFeatureSelectionCommon(data_set, labels, num_features_to_select)


def ICAP(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::ICAP Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/ICAP.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return FeatureSelectionCommon(data_set, labels, num_features_to_select)


def discICAP(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::discICAP Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/ICAP.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discFeatureSelectionCommon(data_set, labels, num_features_to_select)


def JMI(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::JMI Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/JMI.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return FeatureSelectionCommon(data_set, labels, num_features_to_select)


def discJMI(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::discJMI Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/JMI.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discFeatureSelectionCommon(data_set, labels, num_features_to_select)


def MIM(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::MIM Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/MIM.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return FeatureSelectionCommon(data_set, labels, num_features_to_select)


def discMIM(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::discMIM Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/MIM.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discFeatureSelectionCommon(data_set, labels, num_features_to_select)


def mRMR_D(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::mRMR_D Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/mRMR_D.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return FeatureSelectionCommon(data_set, labels, num_features_to_select)


def disc_mRMR_D(data_set, labels, num_features_to_select):
    '''Python implementation of the FEAST::disc_mRMR_D Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/mRMR_D.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discFeatureSelectionCommon(data_set, labels, num_features_to_select)


def weightedCMIM(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::WeightedCMIM Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedCMIM.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return WeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)


def discWeightedCMIM(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::discWeightedCMIM Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedCMIM.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discWeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)


def weightedCondMI(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::WeightedCondMI Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedCondMI.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return WeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)


def discWeightedCondMI(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::discWeightedCondMI Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedCondMI.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discWeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)


def weightedDISR(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::WeightedCondMI Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedDISR.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return WeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)


def discWeightedDISR(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::discWeightedDISR Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedDISR.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discWeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)


def weightedJMI(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::WeightedJMI Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedJMI.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return WeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)


def discWeightedJMI(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::discWeightedJMI Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedJMI.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discWeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)


def weightedMIM(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::WeightedMIM Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedMIM.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return WeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)


def discWeightedMIM(data_set, labels, weights, num_features_to_select):
    '''Python implementation of the FEAST::discWeightedMIM Selection Algorithm.
    See `https://github.com/Craigacp/FEAST/blob/master/src/WeightedMIM.c` for more
    information.
    
    :param data_set: ND Array where len() == number of observations and len(transpose()) ==
    number of features
    :param labels: ND Array where len() == number of observations. The length of labels
    sould be equal to the length of the `data_set`
    :param weights: ND Array of weights for each label where len() == len(labels)
    :param num_features_to_select: Number of features to select. The value must be less than
    or equal to the length of the `data_set`
    :return: `num_features_to_select` Features in order that they were selected
    '''
    return discWeightedFeatureSelectionCommon(data_set, labels, weights, num_features_to_select)
