#####################################################
##  Contributed by Intel Advanced Analitics group  ##
#####################################################

import os
import sys
import pandas as pd
import numpy as np
from numpy.ctypeslib import ndpointer
import ctypes as c


class CMIM():
    """ Class for CMIM features selection
        based on C implementation in Feature Selection Tools box
        https://github.com/Craigacp/FEAST

        Parameters
        ----------
        num_of_features : integer, default = 20
            the amount of features to choose.

        Attributes
        ----------
        feature_names : list of strings
            Names of the selected features.
            * will be available only in case data for fit is pandas DataFrame
        feature_scores : list of floats
            Scores of selected feature. The score is calculated by min{I(Xn ; Y | Xk)}
        feature_indexes : list of floats
            Indexes of selected feature.
        """

    def __init__(self, num_of_features=20):
        if sys.platform.startswith('linux'):
            # in case of Linux OS load feast linux library (.so file)
            self.libFSToolbox = c.CDLL('{}/{}/{}'.format(os.path.dirname(__file__), 'lib', 'libFSToolbox.so'))
        elif sys.platform.startswith('win'):
            # in case of Windows OS load feast windows library (.dll file)
            self.libFSToolbox = c.WinDLL('{}/{}/{}'.format(os.path.dirname(__file__), 'lib', 'libFSToolbox.dll'))
        else:
            # OSs other than Linux and Windows are not supported
            raise NotImplementedError('FeastCMIM is not supported on {} operating system'.format(sys.platform))

        self.num_of_features = num_of_features
        self.feature_indexes = None
        self.feature_scores = None
        self.feature_names = None

    def fit(self, data, labels):
        """
        Fits a defined CMIM filter to a given data set.
        data and labels are expected to be discretized

        Parameters
        ----------
        data : pandas or numpy data object
            The training input samples.
        labels : pandas or numpy data object. labels should contain a binary label.
            The label of the training samples.
        """

        # in case the data is a pandas dataframe we can store features by name
        data_labels = None
        if isinstance(data, pd.DataFrame):
            data_labels = data.columns.tolist()

        # python variables adaptation for C parameters initialization
        data = np.array(data, dtype=np.uint32, order="F")
        labels = np.array(labels, dtype=np.uint32)
        n_samples, n_features = data.shape
        output = np.zeros(self.num_of_features).astype(np.uint)
        selected_features_score = np.zeros(self.num_of_features)

        # cast as C types
        _uintpp = ndpointer(dtype=np.uintp, ndim=1, flags='F')
        c_k = c.c_uint(self.num_of_features)
        c_no_of_samples = c.c_uint(n_samples)
        c_no_of_features = c.c_uint(n_features)
        c_feature_matrix = (data.__array_interface__['data'][0] + np.arange(data.shape[1]) * (data.strides[1])).astype(
            np.uintp)
        c_class_column = labels.ctypes.data_as(c.POINTER(c.c_uint))
        c_output_features = output.ctypes.data_as(c.POINTER(c.c_uint))
        c_feature_scores = selected_features_score.ctypes.data_as(c.POINTER(c.c_double))

        self.libFSToolbox.CMIM.argtypes = [c.c_uint, c.c_uint, c.c_uint, _uintpp, c.POINTER(c.c_uint),
                                           c.POINTER(c.c_uint), c.POINTER(c.c_double)]
        self.libFSToolbox.CMIM.restype = c.POINTER(c.c_uint)

        # call the C lib implementation
        c_selected_features = self.libFSToolbox.CMIM(
            c_k, c_no_of_samples, c_no_of_features, c_feature_matrix,
            c_class_column, c_output_features, c_feature_scores)

        # result transition from C to Python
        features_iterator = np.fromiter(c_selected_features, dtype=np.uint, count=self.num_of_features)
        selected_features = []
        for c_selected_feature_index in features_iterator:
            selected_features.append(c_selected_feature_index)

        # store the selection results
        self.feature_scores = [c_feature_scores[idx] for idx in range(self.num_of_features)]
        self.feature_indexes = selected_features

        if data_labels is not None:
            self.feature_names = [data_labels[idx] for idx in self.feature_indexes]

    def transform(self, X):
        if isinstance(X, np.ndarray):
            return(X[:, self.feature_indexes])
        elif isinstance(X, pd.DataFrame):
            return(X.loc[:, self.feature_names])
        else:
            raise AttributeError('Transform accepts data of type numpy.ndarray or pandas.DataFrame')
