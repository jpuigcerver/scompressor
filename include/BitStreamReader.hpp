/**
 * @file BitStreamReader.hpp
 * @brief Fitxer amb la implementació de la classe BitStreamReader.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __BITSTREAMREADER_HPP__
#define __BITSTREAMREADER_HPP__

#include <cassert>
#include <stdint.h>
#include <StreamReader.hpp>
#include <Bit.hpp>

/**
 * @class BitStreamReader
 * @brief Implementació d'un lector de bits des d'un fluxe de dades.
 */
class BitStreamReader: public StreamReader<Bit> {
private:
  /** Definició del tipus byte com un char. */
  typedef char byte;
  /** Buffer de lectura. */
  byte bit_buffer;
  /** Possició en el buffer de lectura. */
  uint8_t buffer_pos;
  /** Nombre de símbols llegits en l'última operació de lectura. */
  size_t last_read;

public:
  /**
   * @brief Constructor per defecte.
   * Els bits es llegeixen de l'entrada estàndard.
   */
  BitStreamReader() 
    : StreamReader<Bit>(), bit_buffer(0x00), buffer_pos(0xFF), last_read(0)
  { }

  /**
   * @brief Constructor.
   * Els bits es llegeixen des del fluxe indicat.
   */
  BitStreamReader(std::istream& in)
    : StreamReader<Bit>(in), bit_buffer(0x00), buffer_pos(0xFF), last_read(0)
  { }
    
  /**
   * @brief Llegeix un bit des del fluxe.
   *
   * Es llegeix un byte sencer del fluxe d'entrada
   * i van llegint-se un a un els bits. Quan s'han 
   * llegit tots els bits del byte, es llegeix un nou
   * byte del fluxe.
   * @return Bit llegit des de l'entrada.
   */
  Bit get()
  {
    last_read = 0;
    if( buffer_pos == 0xFF ) {
      bit_buffer = ((std::istream*)this)->get();
      if ( !good() ) return 0;
      buffer_pos = BYTES2BITS(sizeof(byte))-1;
    }

    Bit v(bit_buffer & (0x01 << buffer_pos--));
    last_read = 1;
    return v;
  }

  /**
   * @brief Llegeix un nombre determinat de bits i ho interpreta com un valor 
   * de tipus size_t.
   * @param bits nombre de bits a llegir des del fluxe.
   * @return valor llegit des del fluxe.
   * @see get()
   */
  size_t get(char bits)
  {
    assert(bits >= 1 && (unsigned)bits <= BYTES2BITS(sizeof(size_t)));
    size_t res = 0x00;
    for(--bits; bits >= 0 && good(); --bits)
      res |= (get() << bits);
    return res;
  }

  /**
   * @brief Llegeix una seqüència de n bits des del fluxe d'entrada.
   * @param vec adreça on emmagatzemar la seqüència llegida.
   * @param n nombre de bits a llegir.
   * @return Aquest mètode torna *this.
   * @see get()
   */
  std::istream& read(Bit * vec, size_t n)
  {
    for(last_read = 0; last_read < n && good(); ++last_read)
      vec[last_read] = get();
    return *this;
  }

  /**
   * @brief Llegeix una seqüència de n bytes des del fluxe d'entrada.
   * @param vec adreça on emmagatzemar la seqüència llegida.
   * @param n nombre de bits a llegir.
   * @return Aquest mètode torna *this.
   * @see get()
   */
  std::istream& read(byte * vec, size_t n) 
  {
    last_read = 0;
    for(size_t i = 0; i < n && good(); ++i) {
      vec[0] = 0x00;
      for(int j = 7; j >= 0 && good(); --j, ++last_read)
	vec[i] |= (get() << j);
    }
    return *this;
  }

  /**
   * @brief Llegeix un bit des del fluxe d'entrada.
   * @param d bit on emmagatzemar el valor llegit del fluxe d'entrada.
   * @return Aquest mètode torna *this.
   * @see get()
   */
  std::istream& operator >> (Bit& d)
  {
    d = get();
    return *this;
  }

  /** 
   * @brief Recupera el nombre de bits llegits en l'última operació
   * de lectura.
   * @return bits llegits en l'última operació.
   */
  std::streamsize gcount() const
  {
    return last_read;
  }
  
};

#endif
