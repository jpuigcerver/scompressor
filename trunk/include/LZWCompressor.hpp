/**
 * @file LZWCompressor.hpp
 * @brief Fitxer amb la definició de la classe LZWCompressor.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date May 2011
 */

#ifndef __LZWCompressor_HPP__
#define __LZWCompressor_HPP__

#include <GenericCompressor.hpp>
#include <BitStreamWriter.hpp>
#include <BitStreamReader.hpp>

#include <ByteChunk.hpp>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <tr1/unordered_map>
#else
#include <map>
#endif

/**
 * @class LZWCompressor
 * @brief Classe que implementa la compressió i descompressió utilitzant l'algorisme LZW.
 *
 * L'algorisme és pràcticament idèntic al LZ78, tenint en compte les diferències entre els dos.
 * Els detalls de la implementació poden veure's explicats en la classe LZ78Compressor, ja
 * que són idèntics en gran mesura.
 *
 * @see LZ78Compressor
 */
class LZWCompressor : public GenericCompressor {
private:
  /** Versió del compressor. */
  static const unsigned char COMPRESSOR_VERSION;

  /** Nombre de bits per al diccionari. */
  size_t DICTIONARY_BITS;
  /** Nombre d'entrades màximes en el diccionari. */
  size_t DICTIONARY_MAXSIZE;
  /** Nombre de bits per a la grandària de bloc. */
  size_t BLOCK_BITS;
  /** Grandària en bytes del bloc de lectura. */
  size_t BLOCK_SIZE;

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  /** Si s'està utilitzant el compilador de GNU C++, s'utilitza la classe
      unordered_map (una taula hash) per a implementar el diccionari de compressió. */
  typedef std::tr1::unordered_map<ByteChunk,size_t,ByteChunkHash> CompDictionary;
#else
  /** Si s'està utilitzant un compilador diferent de GNU C++, s'utilitza la classe
      map (una arbre binari) per a implementar el diccionari de compressió. */
  typedef std::map<ByteChunk,size_t> CompDictionary;
#endif
  
  /** Diccionari utilitzat en la compressió: taula hash o arbre. */
  CompDictionary com_dictionary;
  /** Diccionari utilitzat en la descompressió: vector. */
  ByteChunk * dec_dictionary;
  /** Nombre d'elements (grandària) del diccionari de descompressió. */
  size_t dec_dictionary_csize;

  /** Buffer de lectura utilitzat en la compressió. */
  char * buffer;
  /** Grandària del buffer de lectura. */
  size_t block_bytes;
  /** Possició actual en el buffer de lectura. */
  size_t block_pos;

  /** 
   * @brief Inicialitza tots els atributs del compressor a partir del nombre de
   * bits a utilitzar per a les entrades del diccionari i el buffer de lectura.
   * 
   * @param db nombre de bits utilitzats per al diccionari.
   * @param bb nombre de bits utilitzats per al buffer de lectura.
   */
  inline void init_comp ( uint8_t db, uint8_t bb ) 
  {
    DICTIONARY_BITS = std::max(db,(uint8_t)8);
    DICTIONARY_MAXSIZE = (0x01 << DICTIONARY_BITS);
    BLOCK_BITS = bb;
    BLOCK_SIZE = (0x01 << BLOCK_BITS);
    assert((buffer = new char [BLOCK_SIZE]) != 0);
    block_bytes = block_pos = 0;

    /* Diccionari per defecte. */
    com_dictionary.clear();
    uint8_t c = 0x00;
    for(;c != 0xFF; ++c)
      com_dictionary.insert(std::pair<ByteChunk,size_t>(ByteChunk((char)c), com_dictionary.size()));
    com_dictionary.insert(std::pair<ByteChunk,size_t>(ByteChunk((char)0xFF), com_dictionary.size()));
  }
  
  /** 
   * @brief Inicialitza tots els atributs del descompressor a partir del nombre de
   * bits a utilitzar per a les entrades del diccionari i el buffer de lectura.
   * 
   * @param db nombre de bits utilitzats per al diccionari.
   * @param bb nombre de bits utilitzats per al buffer de lectura.
   */
  inline void init_deco ( uint8_t db, uint8_t bb ) 
  {
    DICTIONARY_BITS = std::max(db,(uint8_t)8);
    DICTIONARY_MAXSIZE = (0x01 << DICTIONARY_BITS);
    BLOCK_BITS = bb;
    BLOCK_SIZE = (0x01 << BLOCK_BITS);
    assert((dec_dictionary = new ByteChunk [DICTIONARY_MAXSIZE]) != 0);
    
    /* Diccionari per defecte. */
    dec_dictionary_csize = 0;
    uint8_t c = 0x00;
    for(; c != 0xFF; ++c)
      dec_dictionary[dec_dictionary_csize++] = ByteChunk((char)c);
    dec_dictionary[dec_dictionary_csize++] = ByteChunk((char)0xFF);
  }
  
public:
  /**
   * @brief Comprimeix el fluxe d'entrada de input i escriu el resultat en output.
   * 
   * S'utilitzen \f$2^{13} = 8192\f$ entrades en el diccionari i
   * \f$2^6 = 64\f$ bytes per a la grandària del buffer de lectura.
   *
   * @param input fluxe d'entrada que vol comprimir-se.
   * @param output fluxe d'eixida on es deixen les dades comprimides.
   */
  bool compress( std::istream& input, std::ostream& output )
  {
    return compress(input, output, 13, 6);
  }
  
  /**
   * @brief Comprimeix el fluxe d'entrada de input i escriu el resultat en output.
   * 
   * S'utilitzen \f$2^{dictionary\_bits}\f$ entrades per al diccionari i
   * \f$2^{block\_bits}\f$ bytes per a la grandària del buffer de lectura.
   *
   * @param input fluxe d'entrada que vol comprimir-se.
   * @param output fluxe d'eixida on es deixen les dades comprimides.
   * @param dictionary_bits nombre de bits utilitzats per a les entrades del diccionari.
   * @param block_bits nombre de bits utilitzats per al buffer de lectura.
   */
  bool compress( std::istream& input, std::ostream& output,
		 uint8_t dictionary_bits, uint8_t block_bits )
  {
    BitStreamWriter bos(output);

    init_comp(dictionary_bits, block_bits);
    
    /* Escrivim versió del compressor. */
    bos.put(COMPRESSOR_VERSION, 8);

    /* Escrivim paràmetres del compressor. */
    bos.put(DICTIONARY_BITS, 5);
    bos.put(BLOCK_BITS, 5);
    if ( !bos.good() ) return false;

    ByteChunk chunk(BLOCK_SIZE);
    while( input.good() ) {
      /* Llegim bloc de dades... */
      input.read(buffer, BLOCK_SIZE);
      block_bytes = input.gcount();
      block_pos = 0;

      /* Indiquem si la grandària del bloc és menor que BLOCK_SIZE i 
	 en aquest cas la indiquem (últim bloc). */
      if ( block_bytes == BLOCK_SIZE )
	bos.put(0);
      else {
	bos.put(1);
	bos.put(block_bytes, BLOCK_BITS);
      }
      
      /* Mentres queden dades a comprimir en el bloc... */
      chunk.resize(0);
      while ( block_pos < block_bytes ) {
	chunk.push_back(buffer[block_pos]);
	CompDictionary::const_iterator found = com_dictionary.find(chunk);
	if ( found != com_dictionary.end() ) { ++block_pos; continue; }
	
	/* Si caben entrades en el diccionari, afegim al diccionari.. */
	if ( com_dictionary.size() < DICTIONARY_MAXSIZE )
	  com_dictionary.insert(std::pair<ByteChunk,size_t>(chunk, com_dictionary.size()));

	ByteChunk pre_chunk = ByteChunk(chunk.pchar(), chunk.size()-1);
#ifdef DEBUG
	std::clog << com_dictionary[pre_chunk] << std::endl;
#endif
	bos.put(com_dictionary[pre_chunk], DICTIONARY_BITS);
	
	chunk.resize(0);
	chunk.push_back(buffer[block_pos++]);
	
	/* Escrivim el prefixe comprimit. */
	if ( !bos.good() ) return false;
      }

      if ( chunk.size() > 0 ) {
#ifdef DEBUG
	std::clog << com_dictionary[chunk] << std::endl;
#endif
	bos.put(com_dictionary[chunk], DICTIONARY_BITS);
      }
    }

    delete [] buffer;
    
    return ( bos.flush().good() );
  }

  bool decompress(std::istream& input, std::ostream& output)
  {
    BitStreamReader bis(input);

    /* Llegim versió del compressor. */
    if ( bis.get(8) != COMPRESSOR_VERSION ) return false;

    /* Llegim els paràmetres de compressió. */
    DICTIONARY_BITS = bis.get(5);
    BLOCK_BITS = bis.get(5);
    init_deco(DICTIONARY_BITS, BLOCK_BITS);

    /* Error en la capçalera? */
    if ( !bis.good() ) return false;

    /* Mentres queden dades per descomprimir i tot vaja bé... */
    Bit lb = 0;
    ByteChunk x(BLOCK_SIZE), w(BLOCK_SIZE);
    while ( bis.good() && output.good() && lb == 0 ) {
      /* Llegim el nombre de bytes a descomprimir... */
      lb = bis.get();
      if ( !bis.good() ) return false;
      block_bytes = (lb == 0 ? BLOCK_SIZE : bis.get(BLOCK_BITS));
      if ( block_bytes == 0 ) break;

      /* Mentres queden bytes a descomprimir i tot vaja bé... */
      size_t p = bis.get(DICTIONARY_BITS);
      x = dec_dictionary[p];
      output.write(x.pchar(), x.size());
#ifdef DEBUG
      std::clog << p << std::endl;
#endif
      block_bytes -= x.size();

      size_t pant = p;
      while ( block_bytes > 0 && output.good() ) {
	p = bis.get(DICTIONARY_BITS);
	if ( p >= dec_dictionary_csize ) {
	  x = dec_dictionary[pant];
	  x.push_back(x.front());
	  output.write(x.pchar(), x.size());
	  block_bytes -= x.size();

	  if ( dec_dictionary_csize < DICTIONARY_MAXSIZE ) {
	    dec_dictionary[dec_dictionary_csize++] = x;
	  }
	} else {
	  x = dec_dictionary[p];
	  output.write(x.pchar(), x.size());
	  block_bytes -= x.size();
	  
	  w = dec_dictionary[pant];
	  w.push_back(x.front());
	  if ( dec_dictionary_csize < DICTIONARY_MAXSIZE )
	    dec_dictionary[dec_dictionary_csize++] = w;
	}
#ifdef DEBUG
	std::clog << p << std::endl; 
#endif
	pant = p;
      }
    }
    
    delete [] dec_dictionary;
    
    if ( lb == 1 && output.good() ) return true;
    else return false;
  }
};

const unsigned char LZWCompressor::COMPRESSOR_VERSION = 1;

#endif

/**
 * @example LZWCompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe LZWCompressor
 * per a comprimir un fitxer de dades utilitzant l'algorisme LZW.
 *
 * @example LZWDecompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe LZWCompressor
 * per a descomprimir un fitxer de comprimit utilitzant l'algorisme LZW.
 */
