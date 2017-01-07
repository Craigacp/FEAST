package craigacp.feast;

import java.util.List;

/**
 * The accessor class for Weighted FEAST algorithms.
 * 
 * Flag mapping:
 * 1 = CMIM
 * 2 = CondMI
 * 3 = DISR
 * 4 = JMI
 * 5 = MIM
 * 
 * @author craigacp
 */
public abstract class WeightedFEAST {

    private WeightedFEAST() {}

    static {
        System.loadLibrary("feast-java");
    }

    public static ScoredFeatures CMIM(int numFeatures, int[][] dataMatrix, int[] labels, double[] weights) {
        return weightedFeast(1,numFeatures,dataMatrix,labels,weights);
    }

    public static <T> ScoredFeatures CMIM(int numFeatures, List<List<T>> dataMatrix, List<T> labels, double[] weights) {
        return weightedFeast(1,numFeatures,dataMatrix,labels,weights);
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
    
    public static ScoredFeatures CondMI(int numFeatures, int[][] dataMatrix, int[] labels, double[] weights) {
        return condMIFixup(weightedFeast(2,numFeatures,dataMatrix,labels,weights));
    }

    public static <T> ScoredFeatures CondMI(int numFeatures, List<List<T>> dataMatrix, List<T> labels, double[] weights) {
        return condMIFixup(weightedFeast(2,numFeatures,dataMatrix,labels,weights));
    }
    
    public static ScoredFeatures DISR(int numFeatures, int[][] dataMatrix, int[] labels, double[] weights) {
        return weightedFeast(3,numFeatures,dataMatrix,labels,weights);
    }

    public static <T> ScoredFeatures DISR(int numFeatures, List<List<T>> dataMatrix, List<T> labels, double[] weights) {
        return weightedFeast(3,numFeatures,dataMatrix,labels,weights);
    }
    
    public static ScoredFeatures JMI(int numFeatures, int[][] dataMatrix, int[] labels, double[] weights) {
        return weightedFeast(4,numFeatures,dataMatrix,labels,weights);
    }

    public static <T> ScoredFeatures JMI(int numFeatures, List<List<T>> dataMatrix, List<T> labels, double[] weights) {
        return weightedFeast(4,numFeatures,dataMatrix,labels,weights);
    }
    
    public static ScoredFeatures MIM(int numFeatures, int[][] dataMatrix, int[] labels, double[] weights) {
        return weightedFeast(5,numFeatures,dataMatrix,labels,weights);
    }

    public static <T> ScoredFeatures MIM(int numFeatures, List<List<T>> dataMatrix, List<T> labels, double[] weights) {
        return weightedFeast(5,numFeatures,dataMatrix,labels,weights);
    }

    private static <T> ScoredFeatures weightedFeast(int flag, int numFeatures, List<List<T>> dataMatrix, List<T> labels, double[] weights) {
        return weightedFeast(flag,numFeatures,FEASTUtil.convertMatrix(dataMatrix),FEASTUtil.convertList(labels),weights);
    }

    private static native ScoredFeatures weightedFeast(int flag, int numFeatures, int[][] dataMatrix, int[] labels, double[] weights);
}
