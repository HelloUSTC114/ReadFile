# ReadFile
Read file codes for Digtizer (mainly from wavedump), or Oscilloscope(mainly Lecroy) 
## Wavedump
### Text files
- All channels are in different files, all events of one channel in once acquisition are saved in one file
- Each single event are represented by 1024 integrals, count 1024 numbers can get one event
### Binary files
- same as in Text files, still lots of lines, 1024 lines
### Headers
- Format of headers is fixex, binary files also have headers