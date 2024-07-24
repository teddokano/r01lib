# r01lib

## What is this?

`r01lib` is a source code for class libraries to abstract MCUs and peripheral devices.  
From application code, GPIO, I²C, I3C, SPI, Pin-interrupt and timer (ticker) can be accessed with simple APIs.  

This is a submodule of [r01lib_MCUXpresso](https://github.com/teddokano/r01lib_MCUXpresso).  
The library will be built as `_r01lib_*.a` in MCUXpresso workspace with drivers which is required for each MCUs.  
This submodule is placed as `source` folder in the library projects.  

## References

### Sample code
Sample code projects are available as test code in [r01lib_MCUXpresso](https://github.com/teddokano/r01lib_MCUXpresso).  

### API document
API documents are available as Doxygen generated file. `Doxyfile` is set to generate the document folder as `../r01lib_docs` 
which is intended to have the folder directly under the project.  

## History

Project which has been moved from [r01lib_old](https://github.com/teddokano/r01lib_old).  


