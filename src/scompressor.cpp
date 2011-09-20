#include <iostream>
#include <fstream>
#include <netinet/in.h>

#include <HuffmanCompressor.hpp>
#include <LZ77Compressor.hpp>
#include <LZ78Compressor.hpp>
#include <LZWCompressor.hpp>
#include <OptionsParser.hpp>

using namespace std;

static const uint16_t MAGIC_NUMBER[4] = {
  0x27AB, // Huffman
  0xA5E8, // LZ77
  0x7869, // LZ78
  0x8E83  // LZW
};

uint16_t readMagicNumber(istream * input)
{
  uint16_t magicnum;
  input->read((char *)(&magicnum), 2);
  return ntohs(magicnum);
}

void writeMagicNumber(ostream * output, uint16_t mn)
{
  mn = htons(mn);
  output->write((char*)(&mn), 2);
}

bool checkMagicNumber(istream * input, uint16_t ref)
{
  return (readMagicNumber(input) == ref);
}

int main(int argc, char ** argv)
{
  OptionsParser options(argc, argv);
  if ( !options.parse() ) {
    return 1;
  } else if ( options.showHelp() ) {
    options.help();
    return 0;
  }

  istream * input; ifstream fin;
  ostream * output; ofstream fout;
  if ( options.getInputFile() == "-" ) input = &cin;
  else {
    fin.open(options.getInputFile());
    input = &fin;
    if ( !fin.is_open() ) {
      cerr << "File " << options.getInputFile() 
	   << " could not been opened!" << endl;
      return 1;
    }
  }
  
  if ( options.getOutputFile() == "-" ) output = &cout;
  else {
    fout.open(options.getOutputFile());
    output = &fout;
    if ( !fout.is_open() ) {
      cerr << "File " << options.getOutputFile() 
	   << " could not been opened!" << endl;
      return 1;
    }
  }
  
  GenericCompressor * compr = 0;
  uint16_t magicnum;
  switch ( options.getCompressionMethod() ) {
  case OptionsParser::Huffman:
    compr = new HuffmanCompressor();
    break;
  case OptionsParser::LZ77:
    compr = new LZ77Compressor();
    break;
  case OptionsParser::LZ78:
    compr = new LZ78Compressor();
    break;
  case OptionsParser::LZW:
    compr = new LZWCompressor();
    break;
  default:
    // decompressing
    magicnum = readMagicNumber(input);
    if ( magicnum == MAGIC_NUMBER[0] )
      compr = new HuffmanCompressor();
    else if ( magicnum == MAGIC_NUMBER[1] )
      compr = new LZ77Compressor();
    else if ( magicnum == MAGIC_NUMBER[2] )
      compr = new LZ78Compressor();
    else if ( magicnum == MAGIC_NUMBER[3] )
      compr = new LZWCompressor();
    else {
      cerr << "Bad magic number!" << endl;
      return 1;
    }
  }

  if ( options.getWorkMode() == OptionsParser::Compression ) {
    writeMagicNumber(output, MAGIC_NUMBER[options.getCompressionMethod()]);
    compr->compress(*input, *output);
  } else
    compr->decompress(*input, *output);

  if ( fin.is_open() ) fin.close();
  if ( fout.is_open() ) fout.close();
  delete compr;
    
  return 0;
}
