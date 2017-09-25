# Coala: 
## Linker modification 
change  
```
  ROM (rx)         : ORIGIN = 0x4400, LENGTH = 0xBB80 /* END=0xFF7F, size 48000 */

```
with 
```
  ROM (rx)         : ORIGIN = 0x4400, LENGTH = 0x7B70 /* END=0xBF7F, size 48000 */
  PRS (rx)         : ORIGIN = 0xBF70, LENGTH = 0x400f /* END=0xFF7F, size 8016 */
```