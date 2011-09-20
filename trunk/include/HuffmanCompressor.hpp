/**
 * @file HuffmanCompressor.hpp
 * @brief File including the implementation of HuffmanCompressor class.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __HUFFMANCOMPRESSOR_HPP__
#define __HUFFMANCOMPRESSOR_HPP__

#include <iostream>
#include <GenericCompressor.hpp>
#include <NullSource.hpp>
#include <HuffmanTree.hpp>
#include <BitStreamWriter.hpp>
#include <BitStreamReader.hpp>
#include <Bit.hpp>

/**
 * @class HuffmanCompressor
 * @brief Implementation of a compressor/decompressor using Huffman's algorithm.
 *
 * To compress data, first all the data is read and a null source and a null memory source is generated
 * associated to this data.
 *
 * Then, a Huffman tree is built from this source. This tree will be serialized and written to the
 * headers section of the compressed output.
 *
 * Finally, the Huffam tree is used to get the optimal codification of the data and the codification of 
 * each symbol of the input is written to the compressed output.
 *
 * To decompress data, the header section is read and the Huffman tree is reconstructed.
 *
 * Then, the remaining data in the input stream is read bit by bit and is decompressed using the Huffman's tree and 
 * is written to the output stream.
 *
 * Notes:
 * It is only possible to compress data from files, since the data must be read twice.
 * The maximum size of the compressed data is limited to \f$2^{32}\f$ bytes.
 *
 * @see NullSource
 * @see HuffmanTree
 */
class HuffmanCompressor : public GenericCompressor {
private:
  /** Version number. */
  static const unsigned char COMPRESSOR_VERSION;

  /** Null memory source used in the compression. */
  NullSource source;
  /** Huffman tree used to compress and decompress. */
  HuffmanTree huffman;
  /** Codification of the null memory source. */
  Codification<char,Bit> codification;
  /** Number of compressed symbols (bytes). */
  uint32_t numCompressedSymbols;

  /** 
   * @brief Writes the header to the output stream.
   * @param output binary stream writer.
   * @return true if it was successful, false otherwise.
   */
  inline bool writeHeader(BitStreamWriter& output)
  {
    if( !output.put(COMPRESSOR_VERSION, 8).good() ) return false;
    if( !output.put(numCompressedSymbols, 32).good() ) return false;
    if( !huffman.serializeTree(output) ) return false;
    return true;
  }

  /** 
   * @brief Reads the header from the input stream.
   * @param input binary stream reader.
   * @return true if it was successful, false otherwise.
   */
  inline bool readHeader(BitStreamReader& input)
  {
    unsigned char version = input.get(8);
    if ( version != COMPRESSOR_VERSION) return false;
    numCompressedSymbols = input.get(32);
    if( numCompressedSymbols > 0 && !(huffman.deserializeTree(input)) ) return false;
    if ( !input.good() ) return false;
    return true;
  }

  /**
   * @brief Reads the uncompressed data and creates the null memory source and computes 
   * its optimal codification.
   * @param input input stream to be compressed.
   * @return true if it was successful, false otherwise.
   */
  inline bool readUncompressedData(std::istream& input) 
  {
    if( !source.LoadFromStream(input) ) return false;
    numCompressedSymbols = source.getReadSymbols();
    huffman.buildTree(source);
    codification = huffman.getCodification();
    return true;
  }

  /**
   * @brief Writes the compressed data to a output stream using a binary stream writer.
   * @param input input stream to be compressed.
   * @param output binary stream writer.
   * @return true if it was successful, false otherwise.
   */
  bool writeCompressedData(std::istream& input, BitStreamWriter& output)
  {
    /* We need to read all the data again. */
    input.clear();
    input.seekg(0, std::ios::beg);
    
    /* If there is one or no symbol, then there is nothing to be compressed. */
    if ( codification.size() <= 1 ) return true;

    while( !input.eof() ) {
      char s = input.get();
      if( input.good() ) {
	const std::vector<Bit> cod = codification[s];
	for(std::vector<Bit>::const_iterator it = cod.begin(); it != cod.end(); ++it) {
	  output.put(*it);
	  if( !output.good() ) return false;
	}
      }
    }
    
    return true;
  }

  /**
   * @brief Read the compressed data using a binary stream reader and writes the uncompressed
   * data to a output stream.
   * @param input binary stream reader.
   * @param output output stream.
   * @return true if it was successful, false otherwise.
   */
  bool writeUncompressedData(BitStreamReader& input, std::ostream& output) 
  {
    size_t read_symbols = 0;

    /* If no symbols were compressed, it is done. */
    if ( numCompressedSymbols == 0 ) return true;

    /* If the root is a leaf, there is just one symbol. It will be written as times as indicated by
       the numCompressedSymbols field in the header. */
    if ( huffman.currentNodeIsLeaf() ) {
      for(size_t i = 0; i < numCompressedSymbols; ++i) {
	output.put( huffman.getCurrentSymbol() );
	if ( !output.good() ) return false;
      }
      return true;
    }

    /* While there are symbols to be decompressed... */
    while ( input.good() && read_symbols < numCompressedSymbols ) {
      /* Read a bit. */
      Bit b = input.get();
      if ( input.good() ) {
	/* The bit is added to the current path in the Huffman tree. */
	huffman.addToCurrentPath(b);
	if ( huffman.currentNodeIsLeaf() ) {
	  /* If the current node is a leaf, the codified symbols is write and
	     the path is reset. */
	  ++read_symbols;
	  output.put( huffman.getCurrentSymbol() );
	  if ( !output.good() ) return false;
	  huffman.resetCurrentNode();
	}
      }
    }

    return (read_symbols == numCompressedSymbols);
  }
  
public:
  bool compress(std::istream& input, std::ostream& output) 
  {
    BitStreamWriter bos(output);
    if ( !readUncompressedData(input) ) return false;
    if ( !writeHeader(bos) ) return false;
    if ( !writeCompressedData(input, bos) ) return false;
    bos.flush();
    if ( !bos.good() ) return false;
    return true;
  }
  
  bool decompress(std::istream& input, std::ostream& output)
  {
    BitStreamReader bis(input);
    if ( !readHeader(bis) ) return false;
    if ( !writeUncompressedData(bis, output) ) return false;
    return true;
  }
};

const unsigned char HuffmanCompressor::COMPRESSOR_VERSION = 1;

#endif

/**
 * @example HuffmanCompressorExample.cpp
 * @brief This example explains how to use the HuffmanCompressor class
 * to compress data from a file.
 *
 * @example HuffmanDecompressorExample.cpp
 * @brief This example explains how to use the HuffmanCompressor class
 * to decompress data.
 */
