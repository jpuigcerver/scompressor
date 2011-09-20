/**
 * @file HuffmanCompressor.hpp
 * @brief Fitxer amb la implementació de la classe HuffmanCompressor.
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

/**
 * @class HuffmanCompressor
 * @brief Implementació d'un compressor i descompressor que utilitza 
 * l'algorisme de Huffman.
 *
 * Per a comprimir, en primer lloc es llegeixen totes les dades a llegir i es genera una font
 * de memòria nula associada a aquestes dades.
 *
 * En segon lloc es construeix un arbre de Huffman a partir d'aquesta font.
 * 
 * En al capçalera del fitxer comprimit s'escriu el número de versió de l'algorisme,
 * el nombre de símbols que hi ha comprimits a continuació (nombre de bytes del fitxer original)
 * i l'abre de Huffman serialitzat.
 *
 * Finalment, s'utilitza la codificació de la font de memòria nula obtinguda
 * de l'abre de Huffman (que associa a cada símbol un codi binari) per comprimir el fitxer,
 * escrivint el codi de Huffman de cada símbol.
 *
 * Per a descomprimir, es llegeix la capçalera i es deserialitza l'abre de Huffman serialitzat.
 *
 * En segon lloc i mentre queden símbols per a descomprimir, van llegint-se bits i va creant-se
 * un camí en l'abre de Huffman fins arribar a un node que conté un terminal, que s'escriurà en
 * la sortida. El camí és esborrat i torna a l'arrel de l'arbre com a origen.
 *
 * Notes:
 * La grandària del fitxer a comprimir està limitada a \f$2^{32}\f$ bytes.
 *
 * @see NullSource
 * @see HuffmanTree
 */
class HuffmanCompressor : public GenericCompressor {
private:
  /** Número de versió del compressor. */
  static const unsigned char COMPRESSOR_VERSION;

  /** Font de memòria nula utilitzada per a la compressió. */
  NullSource source;
  /** Arbre de Huffman utilitzat per a la compressió i descompressió. */
  HuffmanTree huffman;
  /** Codificació de Huffman de la font de memòria nula. */
  Codification<char,Bit> codification;
  /** Nombre de símbols comprimits. */
  uint32_t numCompressedSymbols;

  /** 
   * @brief Escriu la capçalera sobre un fluxe d'eixida utilitzant un 
   * escriptor binari. 
   *
   * Primer escriu un número de versió (8 bits), després el nombre de 
   * símbols comprimits (32 bits) i finalment l'abre de Huffman
   * serialitzat (longitud variable).
   * @param output escriptor binari sobre el flux d'eixida.
   * @return true si tot ha anat bé, false en cas d'error.
   */
  inline bool writeHeader(BitStreamWriter& output)
  {
    if( !output.put(COMPRESSOR_VERSION, 8).good() ) return false;
    if( !output.put(numCompressedSymbols, 32).good() ) return false;
    if( !huffman.serializeTree(output) ) return false;
#ifdef DEBUG
    std::cout << "HUFFMAN HEADER:" << std::endl;
    std::cout << "VERSION: " << (size_t)COMPRESSOR_VERSION << std::endl;
    std::cout << "TOTAL SYMBOLS: " << numCompressedSymbols << std::endl;
    std::cout << "HUFFMAN CODE: " << std::endl;
    std::cout << codification;
#endif
    return true;
  }

  /** 
   * @brief Llegeix la capçalera des d'un fluxe d'entrada utilitzant un 
   * lector binari. 
   *
   * Primer llegeix el número de versió (8 bits), després l'abre de Huffman
   * serialitzat (longitud variable) i finalment el nombre de símbols
   * comprimits (32 bits).
   * @param input lector binari sobre el flux d'entrada.
   * @return true si tot ha anat bé, false en cas d'error.
   */
  inline bool readHeader(BitStreamReader& input)
  {
    unsigned char version = input.get(8);
    if ( version != COMPRESSOR_VERSION) return false;
    numCompressedSymbols = input.get(32);
    if( numCompressedSymbols > 0 && !(huffman.deserializeTree(input)) ) return false;
    if ( !input.good() ) return false;
#ifdef DEBUG
    std::cout << "HUFFMAN HEADER:" << std::endl;
    std::cout << "VERSION: " << (size_t)version << std::endl;
    std::cout << "TOTAL SYMBOLS: " << numCompressedSymbols << std::endl;
    std::cout << "HUFFMAN CODE: " << std::endl;
    std::cout << codification;
#endif
    return true;
  }

  /**
   * @brief Llegeix les dades a comprimir i crea una font de memòria nula i la seva
   * codificació.
   * @param input fluxe de dades a comprimir.
   * @return true si tot ha anat bé, false en cas d'error.
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
   * @brief Escriu les dades a comprimir sobre un fluxe d'eixida
   * manejat per un escriptor binari.
   * @param input fluxe de dades a comprimir.
   * @param output escriptor binari que gestiona l'eixida.
   * @return true si tot ha anat bé, false en cas d'error.
   */
  bool writeCompressedData(std::istream& input, BitStreamWriter& output)
  {
    /* Com que s'haurà arribat al final del fluxe per a inicialitzar
       l'abre, tornem al principi i esborrem els flags del fluxe d'entrada. */
    input.clear();
    input.seekg(0, std::ios::beg);
    
    /* Si sols tenim un símbol no em de comprimir res, 
       únicament amb la capçalera tenim prou per 
       a descomprimir el fitxer. */
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
   * @brief Llegeix les dades comprimides utilitzant un lector binari i escriu
   * les dades descomprimides sobre un fluxe d'eixida.
   * @param input lector binari que gestiona la lectura binària des del fluxe d'entrada.
   * @param output fluxe d'eixida de les dades descomprimides.
   * @return true si tot ha anat bé, false en cas d'error.
   */
  bool writeUncompressedData(BitStreamReader& input, std::ostream& output) 
  {
    size_t read_symbols = 0;

    /* Si no es van comprimir símbols, hem acabat. */
    if ( numCompressedSymbols == 0 ) return true;

    /* Si l'arrel és fulla, sols hi ha un símbol. S'escriurà
       eixe símbol tantes vegades com indique el camp 
       Número de símbols del fitxer. */
    if ( huffman.currentNodeIsLeaf() ) {
      for(size_t i = 0; i < numCompressedSymbols; ++i) {
	output.put( huffman.getCurrentSymbol() );
	if ( !output.good() ) return false;
      }
      return true;
    }

    /* Mentre queden símbols per descomprimir i puguem llegir de l'entrada... */
    while ( input.good() && read_symbols < numCompressedSymbols ) {
      /* Llegim bit. */
      Bit b = input.get();
      if ( input.good() ) {
	/* Afegim bit al camí. */
	huffman.addToCurrentPath(b);
	if ( huffman.currentNodeIsLeaf() ) {
	  /* Si hem arribat a una arrel, emitim símbol comprimit
	     i reiniciem el camí en l'abre. */
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
 * @brief Aquest exemple mostra com utilitzar la classe HuffmanCompressor
 * per a comprimir un fitxer de dades.
 *
 * @example HuffmanDecompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe HuffmanCompressor
 * per a descomprimir un fitxer de comprimit utilitzant el mètode Huffman.
 */
