/**
 * @file LZ77Compressor.hpp
 * @brief Fitxer amb la definició de la classe LZ77Compressor.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April-May 2011
 */

#ifndef __LZ77Compressor_HPP__
#define __LZ77Compressor_HPP__

#include <GenericCompressor.hpp>
#include <BitStreamWriter.hpp>
#include <BitStreamReader.hpp>

#include <cstring>
#include <cassert>

/**
 * @class LZ77Compressor
 * @brief Classe que implementa la compressió i descompressió utilitzant l'algorisme LZ77.
 * 
 * L'algorisme és similar al vist en classe, l'única diferència és que la compressió no es fa
 * byte a byte, sinó per blocs. 
 *
 * Per a comprimir, intenta llegir-se un bloc de dades del fluxe d'entrada de grandària igual
 * a l'indicada per l'usuari a utilitzar en el buffer de dades. Si s'ha llegit el bloc complet,
 * aquest bloc es marca amb un bit a 0, indicant que no és l'últim bloc comprimit.
 * Si el nombre de bytes llegits, és menor que la grandària del buffer de dades, llavors
 * el bloc sí és l'últim a comprimir i és marca amb un bit a 1 seguit del nombre de bytes llegits.
 * Després, es realitza la compressió del bloc tal i com descriu l'algorisme.
 *
 * Per a descomprimir, simplement es llegeix el flag inicial del bloc. Si és un zero, sabem
 * que no és l'últim bloc i sabem quina és la seva grandària (la grandària del buffer de dades,
 * indicada per l'usuari que ha comprimit el fitxer).
 * Si el flag està marcat, es llegeix a continuació el número de bytes que hi han a continuació
 * (que podria ser zero).
 * Després, es realitza la descompressió del bloc tal i com descriu l'algorisme.
 *
 * Cal notar que aquest esquema de lectura per blocs pot obtenir una compressió diferent a la
 * compressió byte a byte, però aquesta diferència sols ve limitada per la grandària de buffer i 
 * amb una grandària suficientment, no afecta al factor de compressió.
 * A més, permet comprimir un fluxe de dades d'una única passada i sense que el cost per inserir
 * flags addicionals siga molt elevat.
 *
 * Un altre detall d'implementació és que s'utilitza una cua circular per als buffers de cerca i
 * dades, evitant així l'haver de fer còpies de zones de memòria.
 */
class LZ77Compressor : public GenericCompressor 
{
private:
  /** Versió del compressor. */
  static const unsigned char COMPRESSOR_VERSION;
  
  /** Nombre de bits utilitzats per al buffer de cerca. */
  uint8_t SEARCH_BITS;
  /** Nombre de bits utilitzats per al buffer de dades. */
  uint8_t LAHEAD_BITS;
  /** Grandària en bytes del buffer de cerca. */
  size_t SEARCH_SIZE;
  /** Grandària en bytes del buffer de dades. */
  size_t LAHEAD_SIZE;
  /** Grandària en bytes de la finestra d'anàlisi. */
  size_t WINDOW_SIZE;

  /** Finestra d'anàlisi (buffer de cerca i de dades). */
  char * window;
  /** Començament de la finestra de cerca. */
  size_t search_start;
  /** Possició actual en la finestra de cerca. */
  size_t search_pos;
  /** Començament de la finestra de dades (acabament de la finestra de cerca). */
  size_t lahead_start;
  /** Possició actual en la finestra de dades. */
  size_t lahead_pos;
  /** Acabament de la finestra de dades. */
  size_t lahead_end;

  /** 
   * @brief Inicialitza tots els atributs del compressor a partir del nombre de
   * bits a utilitzar per al buffer de cerca i de dades.
   * 
   * @param sb nombre de bits utilitzats per al buffer de cerca.
   * @param lb nombre de bits utilitzats per al buffer de dades.
   */
  inline void init (const uint8_t sb, const uint8_t lb)
  {
    assert( sb > 0 && sb < 30 );
    assert( lb > 0 && lb < sb );
    
    SEARCH_BITS = sb;
    LAHEAD_BITS = lb;
    
    SEARCH_SIZE = (0x01 << SEARCH_BITS);
    LAHEAD_SIZE = (0x01 << LAHEAD_BITS);
    WINDOW_SIZE = SEARCH_SIZE+LAHEAD_SIZE;
    
    assert( (window = new char[WINDOW_SIZE]) != 0 );
    
    memset(window, 0x00, WINDOW_SIZE);
    search_start = search_pos = lahead_start = lahead_end = 0;
  }

  /**
   * @brief Incrementa una possició en la finestra d'anàlisi una unitat.
   *
   * Operació \f$n = (n+1)\%WINDOW\_SIZE\f$.
   * @param[in,out] n possició a incrementar.
   */
  inline void INC_ROUND(size_t& n) const
  { n = (n+1)%WINDOW_SIZE; }

  /**
   * @brief Incrementa una possició en la finestra d'anàlisi m unitats.
   *
   * Operació \f$n=(n+m)\%WINDOW\_SIZE\f$.
   * @param[in,out] n possició a incrementar.
   * @param[in] m nombre d'unitats a incrementar.
   */
  inline void INC_N_ROUND(size_t& n, size_t m) const
  { n = (n+m)%WINDOW_SIZE; }

  /**
   * @brief Obté la possició absoluta a partir de la possició relativa a una base,
   * tenint en compte la grandària de la finestra d'anàlisi.
   * @param position possició relativa.
   * @param base base de les possicions relatives.
   * @return possició absoluta.
   */
  inline size_t ABSOLUTE_POSITION(size_t position, size_t base) const
  { return (base+position)%WINDOW_SIZE; }
  
  /**
   * @brief Obté la possició relativa a partir de la possició absoluta i una base,
   * tenint en compte la grandària de la finestra d'anàlisi.
   * @param position possició absoluta.
   * @param base base de la possició relatives.
   * @return possició relativa.
   */
  inline size_t RELATIVE_POSITION(size_t position, size_t base) const
  { return (position>=base ? position-base : WINDOW_SIZE-base+position); }

  /**
   * @brief Calcula la grandària actual del buffer de cerca.
   *
   * Quan s'estan comprimint els primers bytes del fluxe d'entrada, el buffer
   * de cerca serà menor que el màxim indicat SEARCH_SIZE.
   * @return grandària en bytes del buffer de cerca.
   */
  inline size_t SEARCH_CSIZE(void) const
  { return  ((lahead_start>=search_start) ? (lahead_start-search_start) : 
	     (WINDOW_SIZE-search_start+lahead_start)); }

  /**
   * @brief Busca la possició en el buffer de cerca on es troba
   * el prefixe més llarg possible del buffer de dades.
   * @param[out] max_l Longitud del prefixe.
   * @param[out] max_p Possició en el buffer de cerca on s'ha trobat el prefixe.
   */
  inline void find_prefix(size_t& max_l, size_t &max_p)
  {
    size_t sb_size = SEARCH_CSIZE();
    search_pos = search_start;
    for(size_t i = 0; i < sb_size; ) {   
      /* Busquem prefixe en el buffer de cerca... */
      while( search_pos!=lahead_start && window[search_pos]!=window[lahead_start] )
	{ INC_ROUND(search_pos); ++i; }
      
      /* Si no s'ha trobat el prefixe, acabem. */
      if ( search_pos == lahead_start ) return;
      
      /* Avancem en el prefixe fins que deixe de coincidir
	 amb el buffer de cerca. */
      size_t prefix_start = search_pos;
      lahead_pos = lahead_start;
      while( lahead_pos!=lahead_end && window[search_pos]==window[lahead_pos] )
	{ INC_ROUND(search_pos); INC_ROUND(lahead_pos); ++i; }
      
      /* Si el prefixe trobat en el buffer de cerca és major
	 que l'anterior, el substituïm. */
      if ( lahead_pos-lahead_start > max_l ) 
	{ max_l = lahead_pos-lahead_start; max_p = prefix_start; }  
    }
  }
  
public:
  /**
   * @brief Comprimeix el fluxe d'entrada de input i escriu el resultat en output.
   * 
   * S'utilitzen \f$2^{9} = 512\f$ bytes per a la grandària del buffer de cerca i
   * \f$2^5 = 32\f$ bytes per a la grandària del buffer de dades.
   *
   * @param input fluxe d'entrada que vol comprimir-se.
   * @param output fluxe d'eixida on es deixen les dades comprimides.
   */
  bool compress(std::istream& input, std::ostream& output)
  {
    return compress(input, output, 9, 5);
  }

  /**
   * @brief Comprimeix el fluxe d'entrada de input i escriu el resultat en output.
   * 
   * S'utilitzen \f$2^{search\_bits}\f$ bytes per a la grandària del buffer de cerca i
   * \f$2^{lahead\_bits}\f$ bytes per a la grandària del buffer de dades.
   *
   * @param input fluxe d'entrada que vol comprimir-se.
   * @param output fluxe d'eixida on es deixen les dades comprimides.
   * @param search_bits nombre de bits utilitzats per al buffer de cerca.
   * @param lahead_bits nombre de bits utilitzats per al buffer de dades.
   */
  bool compress(std::istream& input, std::ostream& output, 
		const uint8_t search_bits, const uint8_t lahead_bits)
  {
    BitStreamWriter bos(output);

    init(search_bits, lahead_bits);

    /* Escrivim versió del compressor. */
    bos.put(COMPRESSOR_VERSION, 8);

    /* Escrivim grandàries dels buffers (el nombre de bits utilitzats). */
    bos.put(SEARCH_BITS, 5);
    bos.put(LAHEAD_BITS, 5);
    if ( !bos.good() ) return false;

    while ( input.good() ) {
      size_t bytes_block = 0;

      /* Llegim un bloc de dades. */
      if ( LAHEAD_SIZE > WINDOW_SIZE - lahead_start ) {
	input.read(&window[lahead_start], WINDOW_SIZE-lahead_start);
	bytes_block = input.gcount();
	input.read(window, LAHEAD_SIZE-(WINDOW_SIZE-lahead_start));
	bytes_block += input.gcount();
      } else {
	input.read(&window[lahead_start], LAHEAD_SIZE);
	bytes_block = input.gcount();
      }

      /* En cas d'haver menys bytes que la grandària del buffer
	 de dades, escrivim el nombre de bytes que hi han codificats. */
      if ( bytes_block == LAHEAD_SIZE )
	bos.put(0);
      else {
	bos.put(1);
	bos.put(bytes_block, LAHEAD_BITS);
      }

      /* Final del buffer de dades. */
      lahead_end = (lahead_start + bytes_block)%WINDOW_SIZE;

      /* Mentre queden bytes per a decodificar... */
      while ( bytes_block > 0 ) {
	/* Determinem la posició en el buffer de cerca
	   del prefixe més gran possible en el buffer de dades. */
	size_t max_l = 0, max_p = 0;
	find_prefix(max_l, max_p);
	
	/* Si el prefixe es tan gran, com tots els bytes que quedaven
	   per comprimir, considerarem un byte menys en el prefixe. */
	if ( max_l+1 > bytes_block )
	  max_l = bytes_block-1;

#ifdef DEBUG	
	if ( max_l == 0 ) {
	  std::clog << 0 << " " << window[lahead_start+max_l] << std::endl;
	} else {
	  std::clog << max_l <<" " << max_p << " " 
		    << window[lahead_start+max_l] << std::endl;
	}
#endif

	if (max_l == 0) { 
	  /* El prefixe no estava en el buffer de cerca. */
	  bos.put(0);
	  bos.put(window[lahead_start+max_l], 8);
	} else {
	  size_t rpos = RELATIVE_POSITION(max_p, search_start);
	  bos.put(1);
	  bos.put(max_l, LAHEAD_BITS);
	  bos.put(rpos, SEARCH_BITS);
	  bos.put(window[lahead_start+max_l], 8);
	}
	
	/* Escrivim el prefixe comprimit. */
	if ( !bos.good() ) return false;

	/* Actualitzem les possicios dels buffers. */
	INC_N_ROUND(lahead_start, max_l+1);
	if ( SEARCH_CSIZE() > SEARCH_SIZE ) {
	  if ( lahead_start >= SEARCH_SIZE) search_start = lahead_start-SEARCH_SIZE;
	  else search_start = WINDOW_SIZE - SEARCH_SIZE + lahead_start;
	}

	/* Decrementem el nombre de bytes que queden per comprimir. */
	bytes_block -= max_l+1;
      }
    }

    delete [] window;

    return (bos.flush().good());
  }

  bool decompress(std::istream& input, std::ostream& output)
  {
    BitStreamReader bis(input);

    /* Llegim versió del compressor. */
    if ( bis.get(8) != COMPRESSOR_VERSION ) return false;
    
    /* Llegim grandària dels buffers. */
    SEARCH_BITS = bis.get(5);
    LAHEAD_BITS = bis.get(5);
    init(SEARCH_BITS, LAHEAD_BITS);

    /* Error en la capçalera? */
    if ( !bis.good() ) return false;

    /* Mentres queden dades per descomprimir i tot vaja bé... */
    Bit lb = 0;
    while ( bis.good() && output.good() && lb == 0 ) {
      /* Llegim el nombre de bytes a descomprimir... */
      lb = bis.get();
      size_t block_bytes = (lb == 0 ? LAHEAD_SIZE : bis.get(LAHEAD_BITS));
      if ( !bis.good() ) return false;

      /* Mentres queden bytes a descomprimir i tot vaja bé... */
      while( block_bytes > 0 && output.good() ) {
	/* Llegim longitud del prefixe comprimit. */
	Bit ml = bis.get();
	if ( !bis.good() ) return false;
	
	if ( ml == 0 ) {
	  /* La longitud és zero... */
	  char c = bis.get(8); if (!bis.good()) return false;
	  window[lahead_start] = c;
	  INC_ROUND(lahead_start);
	  output.put(c);

	  --block_bytes;
#ifdef DEBUG
	  std::clog << window << "\t" << 0 << " " << c << std::endl;
#endif
	} else {
	  /* La longitud és major que zero... */
	  size_t max_l = bis.get(LAHEAD_BITS); // Longitud del prefixe
	  size_t max_p = bis.get(SEARCH_BITS); // Possició relativa en el buffer.
	  char c = bis.get(8); if ( !bis.good() ) return false;
	  
	  /* Possició absoluta del començament prefixe en el buffer. */
	  size_t st = ABSOLUTE_POSITION(max_p, search_start); 
	  /* Possició absoluta del final del prefixe en el buffer. */
	  size_t en = ABSOLUTE_POSITION(max_p+max_l, search_start);
	  /* Descomprimim el prefixe. */
	  for(size_t i = st; i != en; INC_ROUND(i)) {
	    window[lahead_start] = window[i];
	    INC_ROUND(lahead_start);
	    output.put(window[i]);
	  }
	  window[lahead_start] = c;
	  INC_ROUND(lahead_start);
	  output.put(c);
	  
	  block_bytes -= max_l+1;
#ifdef DEBUG
	  std::clog << window << "\t" << max_l << " " << st << " " << c << std::endl;
#endif
	}

	/* Actualitzem les possicios dels buffers. */
	if ( SEARCH_CSIZE() > SEARCH_SIZE ) {
	  if ( lahead_start >= SEARCH_SIZE) search_start = lahead_start-SEARCH_SIZE;
	  else search_start = WINDOW_SIZE - SEARCH_SIZE + lahead_start;
	}
      }
      
    }
    
    delete [] window;
    
    if ( lb == 1 && output.good() ) return true;
    else return false;
  }
};

const unsigned char LZ77Compressor::COMPRESSOR_VERSION = 1;

#endif

/**
 * @example LZ77CompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe LZ77Compressor
 * per a comprimir un fitxer de dades utilitzant l'algorisme LZ77.
 *
 * @example LZ77DecompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe LZ77Compressor
 * per a descomprimir un fitxer de comprimit utilitzant l'algorisme LZ77.
 */
