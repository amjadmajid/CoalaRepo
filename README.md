# Coala: 
## Linker modification 
change    
`ROM (rx)         : ORIGIN = 0x4400, LENGTH = 0xBB80 /* END=0xFF7F, size 48000 */`  
with  
`ROM (rx)         : ORIGIN = 0x4400, LENGTH = 0x7B70 /* END=0xBF7F, size 48000 */`   
`PRS (rx)         : ORIGIN = 0xBF70, LENGTH = 0x400f /* END=0xFF7F, size 8016 */`  

## Protected variables 
### Annotation 
All protected variables must be global and annotated with `__p`  
### Access 
A protected vairable must be written to with `WP(var)`  
A protected vairable must be read from with `RP(var)`  
