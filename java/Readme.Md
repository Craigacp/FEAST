FEAST Java API
==============

This package provides a Java API for FEAST. 
It exposes both the weighted and the standard feature selection APIs to Java through JNI.

To compile the Java API run ```mvn package``` in this directory. To build the JNI library, first ```make install``` FEAST and MIToolbox, then run ```make java``` from the FEAST root directory.

The build procedure works with Ubuntu Linux. Other Linux distros will require the JNI include path to be set in the Makefile. 
On Windows it requires mingw64 or cygwin, and the libfeast-java.so file produced by the Makefile will need to be renamed to "feast-java.dll" before running.
On Mac the libfeast-java.so file needs to be renamed to libfeast-java.dylib.

To run the example Java program from scratch (assuming you are in the FEAST directory):
```
cd ../MIToolbox
make; sudo make install
cd ../FEAST
make; sudo make install
make java
cd java
mvn package
java -d64 -Djava.library.path=build/native/lib/ -cp target/FEAST-2.0.jar craigacp.feast.Test -f <path_to_csv_file> -c <fs_method_name> -l <index_of_label> -n <num_features_to_select> <-w>
```
