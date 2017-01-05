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
        int maxVal = 0;
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
