/*
** FEAST.java
**
** Initial Version - 07/01/2017
** Author - Adam Pocock
** 
** Part of the FEAture Selection Toolbox (FEAST), please reference
** "Conditional Likelihood Maximisation: A Unifying Framework for Information
** Theoretic Feature Selection"
** G. Brown, A. Pocock, M.-J. Zhao, M. Lujan
** Journal of Machine Learning Research (JMLR), 2012
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
*/
package craigacp.feast;

import java.util.List;

/**
 * The accessor class for FEAST algorithms.
 * 
 * Flag mapping:
 * 1 = CMIM
 * 2 = CondMI
 * 3 = DISR
 * 4 = ICAP
 * 5 = JMI
 * 6 = MIM
 * 7 = mRMR
 * 
 * @author craigacp
 */
public abstract class FEAST {

    private FEAST() {}

    static {
        System.loadLibrary("feast-java");
    }

    public static native ScoredFeatures BetaGamma(int numFeatures, int[][] dataMatrix, int[] labels, double beta, double gamma);

    public static <T> ScoredFeatures BetaGamma(int numFeatures, List<List<T>> dataMatrix, List<T> labels, double beta, double gamma) {
        int[][] data = FEASTUtil.convertMatrix(dataMatrix);
        int[] lab = FEASTUtil.convertList(labels);
        return BetaGamma(numFeatures,data,lab,beta,gamma);
    }
    
    public static ScoredFeatures CMIM(int numFeatures, int[][] dataMatrix, int[] labels) {
        return feast(1,numFeatures,dataMatrix,labels);
    }

    public static <T> ScoredFeatures CMIM(int numFeatures, List<List<T>> dataMatrix, List<T> labels) {
        return feast(1,numFeatures,dataMatrix,labels);
    }
    
    private static ScoredFeatures condMIFixup(ScoredFeatures f) {
        int maxVal = f.featureIndices.length;
        for (int i = 0; i < f.featureIndices.length; i++) {
            if (f.featureIndices[i] == -1) {
               maxVal = i;
               break;
            }
        }
        int[] newIndices = new int[maxVal];
        double[] newScores = new double[maxVal];
        for (int i = 0; i < maxVal; i++) {
                newIndices[i] = f.featureIndices[i];
                newScores[i] = f.featureScores[i];
        }
        return new ScoredFeatures(newIndices,newScores);
    } 
    
    public static ScoredFeatures CondMI(int numFeatures, int[][] dataMatrix, int[] labels) {
        return condMIFixup(feast(2,numFeatures,dataMatrix,labels));
    }

    public static <T> ScoredFeatures CondMI(int numFeatures, List<List<T>> dataMatrix, List<T> labels) {
        return condMIFixup(feast(2,numFeatures,dataMatrix,labels));
    }
    
    public static ScoredFeatures DISR(int numFeatures, int[][] dataMatrix, int[] labels) {
        return feast(3,numFeatures,dataMatrix,labels);
    }

    public static <T> ScoredFeatures DISR(int numFeatures, List<List<T>> dataMatrix, List<T> labels) {
        return feast(3,numFeatures,dataMatrix,labels);
    }
    
    public static ScoredFeatures ICAP(int numFeatures, int[][] dataMatrix, int[] labels) {
        return feast(4,numFeatures,dataMatrix,labels);
    }

    public static <T> ScoredFeatures ICAP(int numFeatures, List<List<T>> dataMatrix, List<T> labels) {
        return feast(4,numFeatures,dataMatrix,labels);
    }
    
    public static ScoredFeatures JMI(int numFeatures, int[][] dataMatrix, int[] labels) {
        return feast(5,numFeatures,dataMatrix,labels);
    }

    public static <T> ScoredFeatures JMI(int numFeatures, List<List<T>> dataMatrix, List<T> labels) {
        return feast(5,numFeatures,dataMatrix,labels);
    }
    
    public static ScoredFeatures MIM(int numFeatures, int[][] dataMatrix, int[] labels) {
        return feast(6,numFeatures,dataMatrix,labels);
    }

    public static <T> ScoredFeatures MIM(int numFeatures, List<List<T>> dataMatrix, List<T> labels) {
        return feast(6,numFeatures,dataMatrix,labels);
    }

    public static ScoredFeatures mRMR(int numFeatures, int[][] dataMatrix, int[] labels) {
        return feast(7,numFeatures,dataMatrix,labels);
    }

    public static <T> ScoredFeatures mRMR(int numFeatures, List<List<T>> dataMatrix, List<T> labels) {
        return feast(7,numFeatures,dataMatrix,labels);
    }

    private static <T> ScoredFeatures feast(int flag, int numFeatures, List<List<T>> dataMatrix, List<T> labels) {
        return feast(flag,numFeatures,FEASTUtil.convertMatrix(dataMatrix),FEASTUtil.convertList(labels));
    }

    private static native ScoredFeatures feast(int flag, int numFeatures, int[][] dataMatrix, int[] labels);
}
