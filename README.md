# scompressor

This program was developed during the Coding Theory course at the Universitat Politècnica de València (Polytechnic University of Valencia). 

It is a simple compressor able to use Huffman, LZ77, LZ78 and LZW compression/decompression methods to compress files from your computer.

This program has only educational purposes. If you want to use a real compressor, use [gzip](http://www.gzip.org/) or [bzip2](http://bzip.org/) instead.


```
Usage: scompressor [-c input | -x input] [-a algorithm] [-o output] [-h]
Options: 
-c <input>      Compresses from the input source. Use '-' to use stdin.
-x <input>      Decompresses from the input source. Use '-' to use stdin.
-o <output>     The result is written to output. Use '-' to use stdout.
-a <algorithm>  Valid algorithms are 'huf', 'lz77', 'lz78' and 'lzw'.
-h              Shows this help.
```
