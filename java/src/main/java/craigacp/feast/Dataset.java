package craigacp.feast;

/**
 *
 * @author craigacp
 */
public class Dataset {

    public final int[] labels;
    public final int[][] data;

    public Dataset(int[] labels, int[][] data) {
        this.labels = labels;
        this.data = data;
    }
    
}
