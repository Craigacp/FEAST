package craigacp.feast;

import java.util.HashMap;
import java.util.List;

/**
 * Util class for FEAST.
 *
 * @author craigacp
 */
public abstract class FEASTUtil {

    private FEASTUtil() {
    }

    public static int[] convertArray(double[] input) {
        int[] output = new int[input.length];
        int counter = 0;

        HashMap<Double, Integer> idMapping = new HashMap<>();
        int idxCounter = 0;

        for (Double element : input) {
            Integer idx = idMapping.getOrDefault(element, -1);
            if (idx == -1) {
                idx = idxCounter;
                idMapping.put(element, idx);
                idxCounter++;
            }
            output[counter] = idx;
            counter++;
        }

        return output;
    }

    public static <T> int[] convertList(List<T> input) {
        int[] output = new int[input.size()];
        int counter = 0;

        HashMap<T, Integer> idMapping = new HashMap<>();
        int idxCounter = 0;

        for (T element : input) {
            Integer idx = idMapping.getOrDefault(element, -1);
            if (idx == -1) {
                idx = idxCounter;
                idMapping.put(element, idx);
                idxCounter++;
            }
            output[counter] = idx;
            counter++;
        }

        return output;
    }

    public static <T> int[][] convertMatrix(List<List<T>> input) {
        int[][] output = new int[input.size()][];

        int i = 0;

        for (List<T> element : input) {
            output[i] = convertList(element);
            i++;
        }

        return output;
    }

    public static int[][] convertMatrix(double[][] input) {
        int[][] output = new int[input.length][];

        int i = 0;

        for (double[] element : input) {
            output[i] = convertArray(element);
            i++;
        }

        return output;
    }

    public static int[][] transpose(int[][] input) {
        int[][] output = new int[input[0].length][input.length];

        for (int i = 0; i < input.length; i++) {
            for (int j = 0; j < input[0].length; j++) {
                output[j][i] = input[i][j];
            }
        }
        
        return output;
    }

}
