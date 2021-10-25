from ctypes import cdll, POINTER, c_uint, c_double
from os import path
from logging import getLogger


log = getLogger()


class FeastFunc:

    """
    Base class for the feast library functions. In order to call/load the
    function from the C Library, the name of the child class MUST be the name
    of the function in which it is to extend/implement.
    """

    lib = None
    func = None

    def __init__(self, discretized=False):
	"""
	:param discretized: When true the function arguments and return type will
	be set according to the parameters and return type of ALL discrete function.
	When False, the parameters and return type will be set for all non-discrete 
	functions. [Default=False]

	:raises: RuntimeError When the C function could not be found
	"""
        try:
            # TRY WINDOWS VS LINUX/OSX
            self.lib = cdll.LoadLibrary("libFSToolbox.so")
        except OSError as e:
            log.error(e)
            raise
        
        self.func = getattr(self.lib, str(self.__class__.__name__), None)
        if self.func is None:
	    raise RuntimeError("%s is not a function of the library" % str(self.__class__.__name__))
		
	if discretized:

	    self.func.restype = POINTER(c_double)

	    self.func.argtypes = [
        	c_uint,                    # k
        	c_uint,                    # numOfSamples
        	c_uint,                    # numOfFeatures
        	POINTER(POINTER(c_uint)),  # featureMatrix
        	POINTER(c_uint),           # classColumn
        	POINTER(c_double),         # outputFeatures
        	POINTER(c_double)          # featureScores
	    ]

	else:  # Nondiscretized
            
	    self.func.restype = POINTER(c_uint)
            
	    self.func.argtypes = [
		c_uint,                    # k
		c_uint,                    # numOfSamples
		c_uint,                    # numOfFeatures
		POINTER(POINTER(c_uint)),  # featureMatrix
		POINTER(c_uint),           # classColumn
		POINTER(c_uint),           # outputFeatures
		POINTER(c_double)          # featureScores
	    ]


    def __call__(k, numOfSamples, numOfFeatures, featureMatrix, classColumn, outputFeatures, featureScores):
	"""
	:param k:
	:param numOfSamples:
	:param numOfFeatures: 
	:param featureMatrix:
	:param classColumn:
	:param outputFeatures:
	:param featureScores:

	:return:
	"""



class mRMR_D(FeastFunc):

    """
    mRMR_D() implements the minimum Relevance Maximum Redundancy criterion
    using the difference variant, from
    "Feature Selection Based on Mutual Information: Criteria of Max-Dependency,
    Max-Relevance, and Min-Redundancy"
    H. Peng et al. IEEE Pattern Analysis and Machine Intelligence (PAMI) (2005)
    """

    def __init__(self):
	super().__init__(False)

class disc_mRMR_D(FeastFunc):
	
    def __init__(self):
	super().__init__(True)	

class CMIM(FeastFunc):
    """
    CMIM() implements a discrete version of the 
    Conditional Mutual Information Maximisation criterion, using the fast
    exact implementation from
    "Fast Binary Feature Selection using Conditional Mutual Information Maximisation"
    F. Fleuret, JMLR (2004)
    """

    def __init__(self):
        super().__init__(False)

class discCMIM(FeastFunc):

    def __init__(self):
        super().__init__(True)

        
#*******************************************************************************
#** JMI() implements the JMI criterion from
#**
#** "Data Visualization and Feature Selection: New Algorithms for Nongaussian Data"
#** H. Yang and J. Moody, NIPS (1999)
#*******************************************************************************
JMI = fstoolbox_lib.JMI
JMI.restype = POINTER(c_uint)
JMI.argtypes = __non_discretized_argtypes

discJMI = fstoolbox_lib.discJMI
discJMI.restype = POINTER(c_double)
discJMI.argtypes = __discretized_argtypes

#*******************************************************************************
#** DISR() implements the Double Input Symmetrical Relevance criterion
#** from
#** 
#** "On the Use of Variable Complementarity for Feature Selection in Cancer Classification"
#** P. Meyer and G. Bontempi, (2006)
#*******************************************************************************
DISR = fstoolbox_lib.DISR
DISR.restype = POINTER(c_uint)
DISR.argtypes = __non_discretized_argtypes

discDISR = fstoolbox_lib.discDISR
discDISR.restype = POINTER(c_double)
discDISR.argtypes = __discretized_argtypes

#*******************************************************************************
#** ICAP() implements the Interaction Capping criterion from 
#** 
#** "Machine Learning Based on Attribute Interactions"
#** A. Jakulin, PhD Thesis (2005)
#*******************************************************************************
ICAP = fstoolbox_lib.ICAP
ICAP.restype = POINTER(c_uint)
ICAP.argtypes = __non_discretized_argtypes

discICAP = fstoolbox_lib.discICAP
discICAP.restype = POINTER(c_double)
discICAP.argtypes = __discretized_argtypes

#*******************************************************************************
#** CondMI() implements the CMI criterion using a greedy forward search
#**
#** It returns an int array, not a uint array, as -1 is a sentinel value 
#** signifying there was not enough information to select a feature.
#*******************************************************************************
CondMI = fstoolbox_lib.CondMI
CondMI.restype = POINTER(c_uint)
CondMI.argtypes = __non_discretized_argtypes

CondMIICAP = fstoolbox_lib.discCondMI
CondMIICAP.restype = POINTER(c_double)
CondMIICAP.argtypes = __discretized_argtypes

#*******************************************************************************
#** MIM() implements the MIM criterion using a greedy forward search
#*******************************************************************************
MIM = fstoolbox_lib.MIM
MIM.restype = POINTER(c_uint)
MIM.argtypes = __non_discretized_argtypes

discMIM = fstoolbox_lib.discMIM
discMIM.restype = POINTER(c_double)
discMIM.argtypes = __discretized_argtypes

#*******************************************************************************
#** betaGamma() implements the Beta-Gamma space from Brown (2009).
#** This incoporates MIFS, CIFE, and CondRed.
#**
#** MIFS - "Using mutual information for selecting features in supervised neural net learning"
#** R. Battiti, IEEE Transactions on Neural Networks, 1994
#**
#** CIFE - "Conditional Infomax Learning: An Integrated Framework for Feature Extraction and Fusion"
#** D. Lin and X. Tang, European Conference on Computer Vision (2006)
#**
#** The Beta Gamma space is explained in our paper 
#** "Conditional Likelihood Maximisation: A Unifying Framework for Mutual Information Feature Selection"
#** G. Brown, A. Pocock, M.-J. Zhao, M. Lujan
#** Journal of Machine Learning Research (JMLR), 2011
#*******************************************************************************/
BetaGamma = fstoolbox_lib.BetaGamma
BetaGamma.restype = POINTER(c_uint)
BetaGamma.argtypes = __non_discretized_argtypes

discBetaGamma = fstoolbox_lib.discBetaGamma
discBetaGamma.restype = POINTER(c_double)
discBetaGamma.argtypes = __discretized_argtypes

#*******************************************************************************
#** weightedCMIM() implements a discrete version of the 
#** Conditional Mutual Information Maximisation criterion, using the fast
#** exact implementation from
#**
#** "Fast Binary Feature Selection using Conditional Mutual Information Maximisation"
#** F. Fleuret, JMLR (2004)
#*******************************************************************************
weightedCMIM = fstoolbox_lib.weightedCMIM
weightedCMIM.restype = POINTER(c_uint)
weightedCMIM.argtypes = __non_discretized_argtypes

discWeightedCMIM = fstoolbox_lib.discWeightedCMIM
discWeightedCMIM.restype = POINTER(c_double)
discWeightedCMIM.argtypes = __discretized_argtypes

#*******************************************************************************
#** WeightedJMI() implements the JMI criterion from
#**
#** "Data Visualization and Feature Selection: New Algorithms for Nongaussian Data"
#** H. Yang and J. Moody, NIPS (1999)
#*******************************************************************************
weightedJMI = fstoolbox_lib.weightedJMI
weightedJMI.restype = POINTER(c_uint)
weightedJMI.argtypes = __non_discretized_argtypes

discWeightedJMI = fstoolbox_lib.discWeightedJMI
discWeightedJMI.restype = POINTER(c_double)
discWeightedJMI.argtypes = __discretized_argtypes

#*******************************************************************************
#** weightedDISR() implements the Double Input Symmetrical Relevance criterion
#** from
#** 
#** "On the Use of Variable Complementarity for Feature Selection in Cancer Classification"
#** P. Meyer and G. Bontempi, (2006)
#*******************************************************************************
weightedDISR = fstoolbox_lib.weightedDISR
weightedDISR.restype = POINTER(c_uint)
weightedDISR.argtypes = __non_discretized_argtypes

discWeightedDISR = fstoolbox_lib.discWeightedDISR
discWeightedDISR.restype = POINTER(c_double)
discWeightedDISR.argtypes = __discretized_argtypes

#*******************************************************************************
#** weightedCondMI() implements the CMI criterion using a greedy forward search
#**
#** It returns an int array, not a uint array, as -1 is a sentinel value signifying
#** there was not enough information to select a feature.
#*******************************************************************************
weightedCondMI = fstoolbox_lib.weightedCondMI
weightedCondMI.restype = POINTER(c_uint)
weightedCondMI.argtypes = __non_discretized_argtypes

discWeightedCondMI = fstoolbox_lib.discWeightedCondMI
discWeightedCondMI.restype = POINTER(c_double)
discWeightedCondMI.argtypes = __discretized_argtypes
