# Sliding-Window-Protocol
Implementing sending data using UDP with Error Control and Flow Control. This project is for academic purposes.  

# Features  
1. Using UDP with Sliding Window Protocol (Selective Repeat)
2. Error detection using CRC checksum
3. Transmitter resending when packet loss (Not receiveing ACK in time or receive NAK)
4. Flow Control by not sending ACK if receiver buffer is full

# What's not in this project
1. Flow Control using XON/XOFF. It is better to implement so that the transmitter does not waste bandwith 

# How to compile
Type `make` from the root directory project to compile both transmitter and receiver. The executables will be in the bin folder.

# How to run 
From the root directory project  
> For receiver: `./bin/receiver [portNo]`  
> For transmitter: `./bin/transmitter [ipAddress] [portNo] [textfile]`

# LICENSE
The MIT License (MIT)  

Copyright (c) 2015 davidkwan95  

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
