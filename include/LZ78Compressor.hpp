/**
 * @file LZ78Compressor.hpp
 * @brief Fitxer amb la definició de la classe LZ78Compressor.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April-May 2011
 */

#ifndef __LZ78Compressor_HPP__
#define __LZ78Compressor_HPP__

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
 * @class LZ78Compressor
 * @brief Classe que implementa la compressió i descompressió utilitzant l'algorisme LZ78.
 *
 * L'algorisme és similar al vist en classe, l'única diferència és que la compressió no es fa
 * byte a byte, sinó per blocs.
 *
 * Si el compilador utilitzat és el GNU C++ Compiler, s'utilitza una taula hash per al diccionari,
 * si no, s'utilitza la classe map tradicional de C++, que implementa un arbre binari roig-negre.
 *
 * Pel que fa a la grandària màxima del diccionari, quan aquest arriba al seu nombre màxim d'elements,
 * es congela i ja no se n'insereixen més.
 *
 * Per a comprimir, intenta llegir-se un bloc de dades del fluxe d'entrada d'una grandària indicada
 * per l'usuari. Si s'ha llegit el bloc complet, aquest bloc es marca amb un bit a 0, 
 * indicant que no és l'últim bloc comprimit.
 * Si el nombre de bytes llegits, és menor que la grandària per defecte, llavors
 * el bloc sí és l'últim a comprimir i és marca amb un bit a 1 seguit del nombre de bytes llegits.
 * Després, es realitza la compressió del bloc tal i com descriu l'algorisme.
 *
 * Per a descomprimir, simplement es llegeix el flag inicial del bloc. Si és un zero, sabem
 * que no és l'últim bloc i sabem quina és la seva grandària (la indicada pel compressor).
 * Si el flag està marcat, es llegeix a continuació el número de bytes que hi han a continuació
 * (que podria ser zero).
 * Després, es realitza la descompressió del bloc tal i com descriu l'algorisme.
 *
 * Cal notar que aquest esquema de lectura per blocs pot obtenir una compressió diferent a la
 * compressió byte a byte (la longitud d'una entrada del diccionari és igual al bloc de lectura), 
 * però aquesta diferència sols ve limitada per la grandària de buffer i amb una grandària
 * suficientment, no afecta al factor de compressió.
 * A més, permet comprimir un fluxe de dades d'una única passada i sense que el cost espacial 
 * al inserir flags addicionals siga molt elevat.
 */
class LZ78Compressor : public GenericCompressor {
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
    DICTIONARY_BITS = db;
    DICTIONARY_MAXSIZE = (0x01 << DICTIONARY_BITS);
    BLOCK_BITS = bb;
    BLOCK_SIZE = (0x01 << BLOCK_BITS);
    assert((buffer = new char [BLOCK_SIZE]) != 0);
    block_bytes = block_pos = 0;
    com_dictionary.clear();
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
    DICTIONARY_BITS = db;
    DICTIONARY_MAXSIZE = (0x01 << DICTIONARY_BITS);
    BLOCK_BITS = bb;
    BLOCK_SIZE = (0x01 << BLOCK_BITS);
    assert((dec_dictionary = new ByteChunk [DICTIONARY_MAXSIZE]) != 0);
    dec_dictionary_csize = 0;
  }
  
  /**
   * @brief Busca l'entrada en el diccionari de major longitud que casa amb el 
   * prefixe de les dades a comprimir.
   *
   * @param[out] bc seqüència de dades a comprimir. Es tracta del prefixe trobat en el
   * diccionari més el byte que ha fet que la bloc de dades no estiga en el diccionari.
   * @return torna un iterador al final del diccionari, a no ser que s'arribe abans
   * al final del bloc, en aquest cas torna un iterador a la entrada que casa amb el prefixe.
   */
  CompDictionary::const_iterator find_prefix(ByteChunk & bc)
  {
    CompDictionary::const_iterator found = com_dictionary.end();
    bc.resize(0);
    while(block_pos < block_bytes) {
      bc.push_back(buffer[block_pos++]);
      found = com_dictionary.find(bc);
      if ( found == com_dictionary.end() ) return found;
    }
    return found;
  }

public:
  /**
   * @brief Comprimeix el fluxe d'entrada de input i escriu el resultat en output.
   * 
   * S'utilitzen \f$2^{14} = 16384\f$ entrades en el diccionari i
   * \f$2^{5} = 32\f$ bytes per a la grandària del buffer de lectura.
   *
   * @param input fluxe d'entrada que vol comprimir-se.
   * @param output fluxe d'eixida on es deixen les dades comprimides.
   */
  bool compress( std::istream& input, std::ostream& output )
  {
    return compress(input, output, 14, 5);
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
      while ( block_pos < block_bytes ) {
	CompDictionary::const_iterator found = find_prefix(chunk);
	
	/* Si caben entrades en el diccionari i el bloc de dades a comprimir no
	   estava ja en el diccionari (això sols pot passar a final de bloc),
	   creem una nova entrada en el diccionari. */
	if ( com_dictionary.size() < DICTIONARY_MAXSIZE && block_pos < block_bytes )
	  com_dictionary.insert(std::pair<ByteChunk,size_t>(chunk, com_dictionary.size()));
	
#ifdef DEBUG
	if ( chunk.size() == 1 ) {
	  std::clog << "0 0 "<<std::hex<<(size_t(chunk.back()) & 0x0FF) 
		    <<std::dec<<std::endl;
	} else {
	  ByteChunk pre_chunk = ByteChunk(chunk.pchar(), chunk.size()-1);
	  std::clog <<"1 "<<com_dictionary[pre_chunk] << " " 
		    <<std::hex<<(size_t(chunk.back()) & 0x0FF)<<std::dec<<std::endl;
	}
#endif 
	
	if ( chunk.size() == 1 ) {
	  /* El prefixe no estava en el buffer de cerca. */
	  bos.put(0);
	  bos.put(chunk.back(), 8);
	} else {  
	  ByteChunk pre_chunk = ByteChunk(chunk.pchar(), chunk.size()-1);
	  bos.put(1);
	  bos.put(com_dictionary[pre_chunk], DICTIONARY_BITS);
	  bos.put(chunk.back(), 8);
	} 

	/* Escrivim el prefixe comprimit. */
	if ( !bos.good() ) return false;
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
    ByteChunk chunk(BLOCK_SIZE);
    while ( bis.good() && output.good() && lb == 0 ) {
      /* Llegim el nombre de bytes a descomprimir... */
      lb = bis.get();
      if ( !bis.good() ) return false;
      block_bytes = (lb == 0 ? BLOCK_SIZE : bis.get(BLOCK_BITS));

      /* Mentres queden bytes a descomprimir i tot vaja bé... */
      while ( block_bytes > 0 && output.good() ) {
	/* Llegim el bit que indica si el prefixe estava en el diccionari. */
	Bit ff = bis.get();
	if ( !bis.good() ) return false;

	chunk.resize(0);
	if ( ff == 0 ) {
	  /* Si el prefixe no estava, llegim el byte comprimit. */
	  chunk.push_back( bis.get(8) );
	  if ( !bis.good() ) return false;
	  output.put( chunk.back() );
#ifdef DEBUG
	  std::clog << "0 0 "<<std::hex<<(size_t(chunk.back()) & 0x0FF) 
		    <<std::dec<<std::endl;
#endif
	} else {
	  /* Si el prefixe si estava, llegim l'entrada del diccionari. */
	  size_t p = bis.get(DICTIONARY_BITS);
	  if ( !bis.good() ) return false;
	  output.write(dec_dictionary[p].pchar(), dec_dictionary[p].size());
	  chunk.append(dec_dictionary[p]);
	  /* Llegim també el caràcter següent al prefixe. */
	  chunk.push_back(bis.get(8));
	  output.put(chunk.back());
	
#ifdef DEBUG
	  std::clog <<"1 "<<p<<" "<<std::hex<<(size_t(chunk.back())&0x0FF)
		    <<std::dec<<std::endl;
#endif
	}

	/* Si caben entrades en el diccionari i el prefixe no estava ja
	   en el diccionari, llavors afegim una nova entrada. */
	block_bytes -= chunk.size();
	if ( dec_dictionary_csize < DICTIONARY_MAXSIZE && block_bytes > 0 )
	  dec_dictionary[dec_dictionary_csize++] = chunk;
      }
    }
    
    delete [] dec_dictionary;
    
    if ( lb == 1 && output.good() ) return true;
    else return false;
  }
};

const unsigned char LZ78Compressor::COMPRESSOR_VERSION = 1;

#endif

/**
 * @example LZ78CompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe LZ78Compressor
 * per a comprimir un fitxer de dades utilitzant l'algorisme LZ78.
 *
 * @example LZ78DecompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe LZ78Compressor
 * per a descomprimir un fitxer de comprimit utilitzant l'algorisme LZ78.
 */
