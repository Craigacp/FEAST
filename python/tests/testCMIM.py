#######################################################################
##  Contributed by Evgeni Dubov from Intel Advanced Analitics group  ##
#######################################################################

import unittest
import pandas as pd
import numpy as np
from sklearn.datasets import load_boston
from sklearn.preprocessing import MinMaxScaler
from feast.CMIM import CMIM

class TestCMIM(unittest.TestCase):

    def setUp(self):

        # prepare testing dataset
        boston = load_boston()
        self.X = pd.DataFrame(data=boston.data, columns=boston.feature_names)
        y = pd.DataFrame(data=boston.target, columns=['Labels'])
        
        # feast CMIM expects discretized data

        # normalize each feature
        min_max_scaler = MinMaxScaler()
        X_normalized = min_max_scaler.fit_transform(self.X)
        y_normalized = min_max_scaler.fit_transform(y)

        # discretize
        n_bins = 6
        X_n_samples, X_n_features = self.X.shape
        y_n_samples, y_n_features = y.shape
        self.X_discrete = np.zeros((X_n_samples, X_n_features))
        self.y_discrete = np.zeros((y_n_samples, y_n_features))
        bins = np.linspace(0, 1, n_bins)
        for i in range(X_n_features):
            self.X_discrete[:, i] = np.digitize(X_normalized[:, i], bins)
        for i in range(y_n_features):
            self.y_discrete[:, i] = np.digitize(y_normalized[:, i], bins)
        self.X_discrete = self.X_discrete - 1
        self.y_discrete = self.y_discrete - 1

    def test_CMIM_boston_feature_names(self):

        # run feast CMIM fit
        feast_cmim = CMIM(num_of_features=2)
        feast_cmim.fit(self.X_discrete, self.y_discrete)

        # get selected features names
        if feast_cmim.feature_names is not None:
            selected_features = feast_cmim.feature_names
        else:
            feature_names = self.X.columns.tolist()
            selected_features = [feature_names[idx] for idx in feast_cmim.feature_indexes]

        # validate selected features names
        expected_result = ['LSTAT', 'RM']
        self.assertEqual(set(selected_features), set(expected_result))

    def test_CMIM_boston_numpy_scores(self):

        # run feast CMIM fit
        feast_cmim = CMIM(num_of_features=2)
        feast_cmim.fit(self.X_discrete, self.y_discrete)

        # validate selected features scores
        expected_scores = [0.5788220081872999, 0.2731987912650301]
        self.assertEqual(set(feast_cmim.feature_scores), set(expected_scores))

    def test_CMIM_boston_pandas_scores(self):

        # run feast CMIM fit
        feast_cmim = CMIM(num_of_features=2)
        feast_cmim.fit(pd.DataFrame(self.X_discrete), self.y_discrete)

        # validate selected features scores
        expected_scores = [0.5788220081872999, 0.2731987912650301]
        self.assertEqual(set(feast_cmim.feature_scores), set(expected_scores))

    def test_CMIM_boston_pandas_transform(self):

        # prepare data pandas dataframe
        df_column_names = ['column{}'.format(i) for i in range(self.X_discrete.shape[1])]
        X_discrete_df = pd.DataFrame(data=self.X_discrete, columns=df_column_names)

        # run feast CMIM
        feast_cmim = CMIM(num_of_features=2)
        feast_cmim.fit(X_discrete_df, self.y_discrete)

        # transform the data according to features selection
        X_df_transform = feast_cmim.transform(X_discrete_df)

        # validate transform with features names
        expected_columns = ['column12', 'column5']
        self.assertTrue(X_discrete_df.loc[:, expected_columns].equals(X_df_transform))

    def test_CMIM_boston_pandas_transform(self):

        # run feast CMIM fit
        feast_cmim = CMIM(num_of_features=2)
        feast_cmim.fit(self.X_discrete, self.y_discrete)

        # transform the data according to features selection
        X_transform = feast_cmim.transform(self.X_discrete)

        # validate transform with features indexes 
        expected_columns = [12, 5]
        self.assertTrue(np.array_equal(self.X_discrete[:, expected_columns], X_transform))
