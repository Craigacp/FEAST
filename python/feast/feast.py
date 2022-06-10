from ctypes import cdll, POINTER, c_uint, c_double
import inspect
from typing import List
from numpy import ndarray, array, zeros

import numpy as np
from numpy.ctypeslib import ndpointer

# load the library
fstoolbox_lib = cdll.LoadLibrary("libFSToolbox.so")

_uintpp = ndpointer(dtype=np.uintp, ndim=1, flags='F')
_doublepp = ndpointer(dtype=np.uintp, ndim=1, flags='C')

# Set the expected argtypes and return types for library functions
fstoolbox_lib.BetaGamma.argtypes = [c_uint, c_uint, c_uint, _uintpp, POINTER(c_uint), POINTER(c_uint), POINTER(c_double), c_double, c_double]
fstoolbox_lib.BetaGamma.restype = POINTER(c_uint)
fstoolbox_lib.discBetaGamma.argtypes = [c_uint, c_uint, c_uint, _doublepp, POINTER(c_double), POINTER(c_double), POINTER(c_double), c_double, c_double]
fstoolbox_lib.discBetaGamma.restype = POINTER(c_double)
fstoolbox_lib.CMIM.argtypes = [c_uint, c_uint, c_uint, _uintpp, POINTER(c_uint), POINTER(c_uint), POINTER(c_double)]
fstoolbox_lib.CMIM.restype = POINTER(c_uint)
fstoolbox_lib.discCMIM.argtypes = [c_uint, c_uint, c_uint, _doublepp, POINTER(c_double), POINTER(c_double), POINTER(c_double)]
fstoolbox_lib.discCMIM.restype = POINTER(c_double)


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
    c_beta = c_double(beta)
    c_gamma = c_double(gamma)
    
    # filled in feature scores through the library
    selected_features_scores = zeros(num_features)
    c_selected_feature_types = selected_features_scores.ctypes.data_as(POINTER(c_double))
    
    # convert the parameters to the C-Type variables expected
    c_labels = _labels.ctypes.data_as(POINTER(c_uint))
    c_feature_matrix = (_data_set.__array_interface__['data'][0] + np.arange(_data_set.shape[1]) * (_data_set.strides[1])).astype(np.uintp)
    
    features = fstoolbox_lib.BetaGamma(
        c_num_features_to_select,
        c_num_observations,
        c_num_features, 
        c_feature_matrix,
        c_labels,
        c_output_features,
        c_selected_feature_types,
        c_beta,
        c_gamma
    )

    # result transition from C to Python
    features_iterator = np.fromiter(features, dtype=np.uint, count=num_features_to_select)
    selected_features = []
    for c_selected_feature_index in features_iterator:
        selected_features.append(c_selected_feature_index)

    # store the selection results
    feature_scores = [c_selected_feature_types[idx] for idx in range(num_features_to_select)]

    return selected_features, feature_scores


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
    c_beta = c_double(beta)
    c_gamma = c_double(gamma)
    
    # filled in feature scores through the library
    selected_features_scores = zeros(num_features)
    c_selected_feature_types = selected_features_scores.ctypes.data_as(POINTER(c_double))
    
    # convert the parameters to the C-Type variables expected
    c_labels = _labels.ctypes.data_as(POINTER(c_double))
    c_feature_matrix = (_data_set.__array_interface__['data'][0] + np.arange(_data_set.shape[0]) * (_data_set.strides[0])).astype(np.uintp)
    
    features = fstoolbox_lib.discBetaGamma(
        c_num_features_to_select,
        c_num_observations,
        c_num_features, 
        c_feature_matrix,
        c_labels,
        c_output_features,
        c_selected_feature_types,
        c_beta,
        c_gamma
    )

    # result transition from C to Python
    features_iterator = np.fromiter(features, dtype=np.uint, count=num_features_to_select)
    selected_features = []
    for c_selected_feature_index in features_iterator:
        selected_features.append(c_selected_feature_index)

    # store the selection results
    feature_scores = [c_selected_feature_types[idx] for idx in range(num_features_to_select)]

    return selected_features, feature_scores


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
    c_feature_matrix = (_data_set.__array_interface__['data'][0] + np.arange(_data_set.shape[1]) * (_data_set.strides[1])).astype(np.uintp)
    
    features = fstoolbox_lib.CMIM(
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
    c_feature_matrix = (_data_set.__array_interface__['data'][0] + np.arange(_data_set.shape[1]) * (_data_set.strides[1])).astype(np.double)
    
    features = fstoolbox_lib.discCMIM(
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


