package craigacp.feast;

/**
 * A tuple class which contains the selected feature indices and their scores.
 *
 * @author craigacp
 */
public class ScoredFeatures {

    public final int[] featureIndices;
    public final double[] featureScores;

    public ScoredFeatures(int[] featureIndices, double[] featureScores) {
        this.featureIndices = featureIndices;
        this.featureScores = featureScores;
    }

}
