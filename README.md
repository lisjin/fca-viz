# fca-viz
Tool for formal concept analysis visualization

## Setup
#### FCA
1. Create an Xcode project and copy the contents of FCA_viz (`concept.cpp`, `concept.h`, `set_ops.h`)
  * Within the same directory, open Terminal and run the following two lines:
  ```
  $ touch input.txt     # Contains input dataset
  $ touch com.txt       # File of commands (may be left blank)
  ```
2. Return to the root directory and open `script.sh`
  * On line 1, change `FCA_XCODE` to the absolute path of the Xcode project from the previous step
  * On line 2, change `VIZ` to the location of the viz folder within the root directory

#### Visualization
1. To set up, `cd` into the viz directory and run:

  ```
  $ bash setup_mac.sh    # Installs necessary packages
  ```

## Usage
Simply run:
```
$ bash script.sh
```
This will lead you through the following steps:

1. Input file: Insert a Boolean dataset of m lines and n characters per line
  * A '1' denotes that the object on that line has the attribute at that column, '0' means it does not:

    ```
    1010    # Object 1 has attributes {1,3}
    0110    # Object 2 has attributes {2,3}
    1101    # Object 3 has attributes {1,2,4}
    ```

2. Commands file (optional): Fill with set operations to compute on the dataset
  * Indicate a single command on each line in the format of `<cmd> <e_0> <e_1> ... <e_n>`
    * `<cmd>` is a character indicating the type of set operation:
      * `c`: closure
      * `b`: boundary
      * `i`: interior
      * `d`: derived set
      * `s`: isolated point
    * `<e_i>` is member i of the extent set parameter
  * Example:
  
    ```
    c 1 2 3   # Find closure given extent set {1,2,3}
    s 4 5     # Find the isolated point given extent set {4,5}
    ```
3. Run program: Click the 'run' button when the Xcode project appears to generate output .txt files
  * The results of any set operations will be seen in the Xcode output window
4. View output concept lattice
  * An image of the resulting concept lattice will appear
  * This is saved in the viz directory as `output.png`
