/*******************************************************************************
** WeightedFSAlgorithms.h
** Provides the function definitions for the list of weighted feature selection
** algorithms implemented in FEAST.
**
** Author: Adam Pocock
** Created - 27/06/2011
** Updated - 14/09/2016 - Added double and uint entry points for all functions.
**           18/12/2016 - Added an output variable for the feature scores.
**
** Part of the FEAture Selection Toolbox (FEAST), please reference
** "Information Theoretic Feature Selection for Cost-Sensitive Problems"
** A. Pocock, N. Edakunni, M.-J. Zhao, M. Lujan, G. Brown
** ArXiv, 2017
**
** Please check www.github.com/Craigacp/FEAST for updates.
** 
** Copyright (c) 2010-2017, A. Pocock, G. Brown, The University of Manchester
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
** 
**   - Redistributions of source code must retain the above copyright notice, this 
**     list of conditions and the following disclaimer.
**   - Redistributions in binary form must reproduce the above copyright notice, 
**     this list of conditions and the following disclaimer in the documentation 
**     and/or other materials provided with the distribution.
**   - Neither the name of The University of Manchester nor the names of its 
**     contributors may be used to endorse or promote products derived from this 
**     software without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
** ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
** ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*******************************************************************************/

/*******************************************************************************
 * All algorithms take an integer k which determines how many features to 
 * select, the number of samples and the number of features. Additionally each
 * algorithm takes pointers to the data matrix, the label vector, the weight
 * vector, the output vector and the featureScores vector. The output and 
 * featureScores vector should be pre-allocated with sizeof(double)*k bytes.
 *
 * FSToolbox expects all matrices to be in column-major (Fortan style) format.
 *
 * Some algorithms take additional parameters, which given at the end of the 
 * standard parameter list.
 *
 * Each algorithm uses a forward search, and selects the feature which has
 * the maxmimum MI with the labels first.
 *
 * All the algorithms except CMIM use an optimised variant which caches the
 * previously calculated MI values. This trades space for time, but can 
 * allocate large amounts of memory. CMIM uses the optimised implementation
 * given in Fleuret (2004). 
 *****************************************************************************/

#ifndef __WFSAlgorithms_H
#define __WFSAlgorithms_H

#include "MIToolbox/MIToolbox.h"

/*******************************************************************************
** weightedCMIM() implements a discrete version of the 
** Conditional Mutual Information Maximisation criterion, using the fast
** exact implementation from
**
** "Fast Binary Feature Selection using Conditional Mutual Information Maximisation"
** F. Fleuret, JMLR (2004)
*******************************************************************************/
uint* weightedCMIM(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, double *weightVector, uint *outputFeatures, double *featureScores);
double* discWeightedCMIM(uint k, uint noOfSamples, uint noOfFeatures, double **featureMatrix, double *classColumn, double *weightVector, double *outputFeatures, double *featureScores);

/*******************************************************************************
** WeightedJMI() implements the JMI criterion from
**
** "Data Visualization and Feature Selection: New Algorithms for Nongaussian Data"
** H. Yang and J. Moody, NIPS (1999)
*******************************************************************************/
uint* weightedJMI(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, double *weightVector, uint *outputFeatures, double *featureScores);
double* discWeightedJMI(uint k, uint noOfSamples, uint noOfFeatures, double **featureMatrix, double *classColumn, double *weightVector, double *outputFeatures, double *featureScores);

/*******************************************************************************
** WeightedMIM() implements the MIM criterion using a greedy forward search
*******************************************************************************/
uint* weightedMIM(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, double *weightVector, uint *outputFeatures, double *featureScores);
double* discWeightedMIM(uint k, uint noOfSamples, uint noOfFeatures, double **featureMatrix, double *classColumn, double *weightVector, double *outputFeatures, double *featureScores);

/*******************************************************************************
** weightedDISR() implements the Double Input Symmetrical Relevance criterion
** from
** 
** "On the Use of Variable Complementarity for Feature Selection in Cancer Classification"
** P. Meyer and G. Bontempi, (2006)
*******************************************************************************/
uint* weightedDISR(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, double *weightVector, uint *outputFeatures, double *featureScores);
double* discWeightedDISR(uint k, uint noOfSamples, uint noOfFeatures, double **featureMatrix, double *classColumn, double *weightVector, double *outputFeatures, double *featureScores);

/*******************************************************************************
** weightedCondMI() implements the CMI criterion using a greedy forward search
**
** It returns an int array, not a uint array, as -1 is a sentinel value signifying
** there was not enough information to select a feature.
*******************************************************************************/
int* weightedCondMI(uint k, uint noOfSamples, uint noOfFeatures, uint **featureMatrix, uint *classColumn, double *weightVector, int *outputFeatures, double *featureScores);
double* discWeightedCondMI(uint k, uint noOfSamples, uint noOfFeatures, double **featureMatrix, double *classColumn, double *weightVector, double *outputFeatures, double *featureScores);

#endif
