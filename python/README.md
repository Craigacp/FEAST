# FEAST for Python

The information provided here is only relavant to PyFeast. PyFeast is the python extension to the [FEAST](https://github.com/Craigacp/FEAST) library.

# Installation

The user may opt to install FEAST and MIToolbox on their own, but a build script is supplied to automatically build the information as well. Please see instructions for installing FEAST and MIToolbox [here](https://github.com/Craigacp/FEAST). 

After the installation is complete for FEAST and MIToolbox, the user can run the installation (locally) for PyFeast. Navigate to the the python folder of the [FEAST](https://github.com/Craigacp/FEAST) library and execute `pip3 install .`

A _requirements.txt_ file is supplied in the event that a user would like to install all requirements before installing the package. 

**Note**: _MIToolbox should be installed prior to FEAST_.
**Note**: _The [build.sh](./build.sh) script will only work on linux systems_.

# Example

The following example will illustrate the (simple) usage of the library.

```python
from fstoolbox import CMIM
import pandas as pd
import numpy as np
from sklearn.datasets import load_boston


boston = load_boston()

# Read the dataset and set the appropriate columns
data_set = pd.DataFrame(data=boston.data, columns=boston.feature_names).to_numpy()

# labels is the regression target for the dataset
labels = pd.DataFrame(data=boston.target, columns=['Labels']).to_numpy()
```

In the code snippet above, the data set is loaded from a [known set](https://scikit-learn.org/stable/modules/generated/sklearn.datasets.load_boston.html).
The data set is trimmed from the raw input and the labels are set to the regression targets for the data set. 

**Note**: _Upon investigation the size of data set is X by Y and the size of labels is X by 1_.


```python
selected_features, feature_scores = CMIM(data_set, labels, 2)
````

Continuing from the imports/provisioning example, the snippet above will execute the [CMIM](https://github.com/Craigacp/FEAST/blob/master/src/CMIM.c) source code
and expects `2` features to be returned. The indexes of the returned features and the scores of those features are provided by **all** functions in the 
python extension.

```python
feature_names = dataset.columns.tolist()
selected_features = [feature_names[idx] for idx in fn_cols]
```

If the user wants to determine what those features amount to, they can execute a similar set of lines to those in the snippet above.
