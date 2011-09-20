#ifndef __OPTIONSPARSER_HPP__
#define __OPTIONSPARSER_HPP__

#include <cctype>
#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <string>

using namespace std;

class OptionsParser {
public:
  typedef enum {Compression = 0, Decompression} WorkMode;
  typedef enum {Huffman = 0, LZ77, LZ78, LZW, None} CompressionMethod;
private:
  WorkMode workMode;
  CompressionMethod comprMethod;
  string inputFile, outputFile;
  bool parsed, showhelp;
  int argc;
  char * const * argv;
public:
  OptionsParser(int argc, char * const * argv) :
    parsed(false), argc(argc), argv(argv)
  {
  }

  void help() const 
  {
    cerr << "Usage: " << argv[0] << " [-c input | -x input] [-a algorithm] [-o output] [-h]" << endl;
    cerr << "Options: " << endl;
    cerr << "-c <input>" << "\t"
	 << "Compresses from the input source. Use '-' to use stdin." 
	 << endl;
    cerr << "-x <input>" << "\t"
	 << "Decompresses from the input source. Use '-' to use stdin." 
	 << endl;
    cerr << "-o <output>" << "\t"
	 << "The result is written to output. Use '-' to write to stdout." 
	 << endl;
    cerr << "-a <algorithm>" << "\t"
	 << "Valid algorithms are 'huf', 'lz77', 'lz78' and 'lzw'." 
	 << endl;
    cerr << "-h" << "\t"
	 << "Shows this help." 
	 << endl;
  }

  bool parse()
  {
    int c;

    workMode = Decompression;
    inputFile = "-"; // stdin
    outputFile = "-"; // stdout
    showhelp = false;
    parsed = false;
    comprMethod = None;

    while( (c = getopt(argc, argv, "c:x:o:a:h")) != -1 ) {
      switch(c) {
      case 'c': 
	workMode = Compression; 
	inputFile = string(optarg);
	break;
      case 'x': 
	workMode = Decompression; 
	inputFile = string(optarg);
	break;
      case 'o':
	outputFile = string(optarg);
	break;
      case 'a':
	if ( !strcmp(optarg, "lzw") )
	  comprMethod = LZW;
	else if ( !strcmp(optarg, "lz77") )
	  comprMethod = LZ77;
	else if ( !strcmp(optarg, "lz78") )
	  comprMethod = LZ78;
	else if ( !strcmp(optarg, "huf") )
	  comprMethod = Huffman;
	else {
	  cerr << "Unknown compression method: " << optarg << endl;
	  return false;
	}
      case 'h': 
	showhelp = true; 
	break;
      default:
	if ( optopt == 'c' || optopt == 'x' || 
	     optopt == 'a' || optopt == 'o')
	  cerr << "Option -" << (char)optopt 
	       << " requires an argument." << endl;
	else 
	  cerr << "Unknow option character -" << (char)optopt << endl;
	return false;
      }
    }

    if (workMode == Compression && comprMethod == None)
      comprMethod = LZW;

    if (workMode == Decompression && comprMethod != None)
      cerr << "The decompression will be selected from the input." << endl;

    if (comprMethod == Huffman && inputFile == "-") {
      cerr << "Huffman can't compress from a stream. Choose a file." << endl;
      return false;
    }

    return (parsed = true);
  }

  WorkMode getWorkMode() const {
    if ( !parsed ) { /* throw */ }
    return workMode;
  }

  CompressionMethod getCompressionMethod() const
  {
    return comprMethod;
  }

  string getInputFile() const 
  {
    return inputFile;
  }

  string getOutputFile() const 
  {
    return outputFile;
  }

  bool showHelp() const
  { return showhelp; }
};

#endif
