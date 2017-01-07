package craigacp.feast;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author craigacp
 */
public class Test {

    public static Dataset readDataset(String filename, int labelIndex) {
        try {
            List<Integer> labels = new ArrayList<>();
            List<List<Integer>> data = new ArrayList<>();
            BufferedReader reader = new BufferedReader(new FileReader(filename));
            while (reader.ready()) {
                String line = reader.readLine();
                if (line != null) {
                    String[] splitLine = line.split(",");
                    labels.add(Integer.parseInt(splitLine[labelIndex]));
                    if (data.isEmpty()) {
                        for (int i = 0; i < splitLine.length-1; i++) {
                            data.add(new ArrayList<>());
                        }
                    }
                    for (int i = 0; i < splitLine.length; i++) {
                        if (i < labelIndex) {
                            data.get(i).add(Integer.parseInt(splitLine[i]));
                        } else if (i > labelIndex) {
                            data.get(i-1).add(Integer.parseInt(splitLine[i]));
                        }
                    }
                }
            }

            int[] labelsArray = FEASTUtil.convertList(labels);
            int[][] dataArray = FEASTUtil.convertMatrix(data);

            System.out.println("Dataset loaded from " + filename);
            System.out.println("Found " + labelsArray.length + " labels and " + dataArray.length + " features.");

            return new Dataset(labelsArray,dataArray);
        } catch (FileNotFoundException e) {
            System.err.println("File " + filename + " not found. " + e.toString());
        } catch (IOException e) {
            System.err.println("IOException when reading from " + filename + ". " + e.toString());
        }
        return null;
    }

    public static String usage() {
        StringBuilder buffer = new StringBuilder();

        buffer.append("Test - arguments:\n");
        buffer.append("     -c {CIFE,CMIM,CondMI,DISR,ICAP,JMI,MIFS,MIM,mRMR}\n");
        buffer.append("         Selection criteria to use.\n");
        buffer.append("     -f <filename>\n");
        buffer.append("         CSV file to read.\n");
        buffer.append("     -l <int>\n");
        buffer.append("         Index of the label in the csv.\n");
        buffer.append("     -n <int>\n");
        buffer.append("         Number of features to select. Defaults to a full ranking.\n");

        return buffer.toString();
    }

    public static void main(String[] args) {
        String filename = null;
        String fsMethod = null;
        int numFeatures = -1;
        int labelIndex = -1;

        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "-c":
                    if ((i + 1) < args.length) {
                        i++;
                        fsMethod = args[i];
                    } else {
                        System.err.println("Please supply an argument to -c");
                        System.err.println(usage());
                        return;
                    }
                    break;
                case "-f":
                    if ((i + 1) < args.length) {
                        i++;
                        filename = args[i];
                    } else {
                        System.err.println("Please supply an argument to -f");
                        System.err.println(usage());
                        return;
                    }
                    break;
                case "-l":
                    if ((i + 1) < args.length) {
                        i++;
                        try {
                            labelIndex = Integer.parseInt(args[i]);
                        } catch (NumberFormatException e) {
                            System.err.println(args[i] + " is not a valid integer");
                            System.err.println(usage());
                            return;
                        }
                    } else {
                        System.err.println("Please supply an argument to -l");
                        System.err.println(usage());
                        return;
                    }
                    break;
                case "-n":
                    if ((i + 1) < args.length) {
                        i++;
                        try {
                            numFeatures = Integer.parseInt(args[i]);
                        } catch (NumberFormatException e) {
                            System.err.println(args[i] + " is not a valid integer");
                            System.err.println(usage());
                            return;
                        }
                    } else {
                        System.err.println("Please supply an argument to -n");
                        System.err.println(usage());
                        return;
                    }
                    break;
                default:
                    System.err.println("Unknown argument " + args[i]);
                    System.err.println(usage());
                    return;
            }
        }

        Dataset dataset = null;
        if ((filename != null) && (labelIndex != -1)) {
            dataset = readDataset(filename, labelIndex);
        } else {
            System.err.println("Please supply a filename and a label index.");
            System.err.println(usage());
            return;
        }

        if ((dataset != null) && (fsMethod != null)) {
            if (numFeatures == -1) {
                numFeatures = dataset.data.length;
                System.out.println("Setting numFeatures to " + numFeatures);
            }
            ScoredFeatures output = null;
            //{CIFE,CMIM,CondMI,DISR,ICAP,JMI,MIM,mRMR}
            switch (fsMethod) {
                case "cife":
                case "CIFE":
                case "fou":
                case "FOU":
                    output = FEAST.BetaGamma(numFeatures, dataset.data, dataset.labels, 1.0, 1.0);
                    break;
                case "cmim":
                case "CMIM":
                    output = FEAST.CMIM(numFeatures, dataset.data, dataset.labels);
                    break;
                case "condmi":
                case "CondMI":
                    output = FEAST.CondMI(numFeatures, dataset.data, dataset.labels);
                    break;
                case "disr":
                case "DISR":
                    output = FEAST.DISR(numFeatures, dataset.data, dataset.labels);
                    break;
                case "icap":
                case "ICAP":
                    output = FEAST.ICAP(numFeatures, dataset.data, dataset.labels);
                    break;
                case "jmi":
                case "JMI":
                    output = FEAST.JMI(numFeatures, dataset.data, dataset.labels);
                    break;
                case "mifs":
                case "MIFS":
                    output = FEAST.BetaGamma(numFeatures, dataset.data, dataset.labels, 1.0, 0.0);
                    break;
                case "mim":
                case "MIM":
                    output = FEAST.MIM(numFeatures, dataset.data, dataset.labels);
                    break;
                case "mrmr":
                case "mRMR":
                    output = FEAST.mRMR(numFeatures, dataset.data, dataset.labels);
                    break;
                default:
                    System.err.println("Unknown fs method " + fsMethod);
                    System.err.println(usage());
                    return;
            }

            if (output != null) {
                System.out.println("Selected features (index,score):");
                for (int i = 0; i < output.featureIndices.length; i++) {
                    System.out.println("\t" + output.featureIndices[i] + ",\t" + output.featureScores[i]);
                }
            } else {
                System.err.println("Feature selection failed");
            }
        } else {
            System.err.println("Please select a fs method, and load a valid dataset");
            System.err.println(usage());
            return;
        }

    }
}
