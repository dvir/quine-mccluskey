Quine-McCluskey algorithm implementation
=====                                                                          
       
Compiling
---------
* libboost_program_options is required.
* Clone this project.
* ```cd ./quine-mccluskey```
* ```make```

Manual
---------
```./qmc --help```
```
Allowed options:
    --help                      produce help message
    --sop arg                   SOP terms
    --pos arg                   POS terms
    --dont-care arg             Dont-Care terms
    -l [ --literals-count ] arg Literals count
    --debug arg (=0)            Show debug information
```

Usage
---------
```./qmc -l 4 --sop 0 1 2 5 6 7 8 9 10 14```

Output
---------
For the example usage shown above, the output is:
```f = cd' + b'c' + a'bd```
