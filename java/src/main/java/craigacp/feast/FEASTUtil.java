/*
** FEASTUtil.java
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
