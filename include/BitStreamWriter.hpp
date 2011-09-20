/**
 * @file BitStreamWriter.hpp
 * @brief Fitxer amb la implementació de la classe BitStreamWriter.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __BITSTREAMWRITER_HPP__
#define __BITSTREAMWRITER_HPP__

#include <cassert>
#include <stdint.h>
#include <StreamWriter.hpp>
#include <Bit.hpp>

/**
 * @class BitStreamWriter
 * @brief Implementació d'un escriptor de bits sobre un fluxe d'eixida.
 * @see StreamWriter
 * @see CharStreamWriter
 */
class BitStreamWriter : public StreamWriter<Bit> {
private:
  /** Definició del tipus byte com un char. */
  typedef char byte;
  /** Buffer d'escriptura. */
  byte bit_buffer;
  /** Possició en el buffer d'escriptura. */
  uint8_t buffer_pos;
  
  /** Mètode per a inicialitzar el buffer. */
  inline void initBuffer(void)
  {
    bit_buffer = 0x00;
    buffer_pos = BYTES2BITS(sizeof(byte))-1;
  }
  
public:
  /**
   * @brief Constructor per defecte.
   * Els bits s'escriuran en l'eixida estàndard.
   */
  BitStreamWriter() 
    : StreamWriter<Bit>(), 
      bit_buffer(0x00), buffer_pos(BYTES2BITS(sizeof(byte))-1)
  { }
  
  /** 
   * @brief Constructor. 
   * Els bits s'escriuen en el fluxe indicat. 
   * @param out fluxe sobre el que s'escriuran els bits.
   */
  BitStreamWriter(std::ostream& out)
    : StreamWriter<Bit>(out),
      bit_buffer(0x00), buffer_pos(BYTES2BITS(sizeof(byte))-1)
  { }
  
  /**
   * @brief Escriu un bit en el fluxe d'eixida.
   *
   * Els bits s'escriuen primer en un buffer d'eixida i quan aquest buffer s'ompli,
   * el seu contingut es volca al fluxe d'eixida i es reinicialitza el buffer.
   * 
   * IMPORTANT: Pot succeir que queden bits en el buffer que no han estat
   * volcats al fluxe d'eixida. Per a forçar el volcat s'utilitza el mètode flush().
   * @param d bit a escriure en el fluxe.
   * @return Aquest mètode torna *this.
   * @see flush()
   */
  std::ostream& put(const Bit& d)
  {
    bit_buffer |= ((byte)d << buffer_pos);
    if(buffer_pos == 0) {
      ((std::ostream*)this)->put(bit_buffer);
      initBuffer();
    } else --buffer_pos;
    return *this;
  }

  /**
   * @brief Escriu un valor de tipus size_t utilitzant un nombre determinat 
   * de bits.
   *
   * IMPORTANT: Pot succeir que queden bits en el buffer que no han estat
   * volcats al fluxe d'eixida. Per a forçar el volcat s'utilitza el mètode flush().
   * @param val valor a escriure.
   * @param bits bits a utilitzar.
   * @return Aquest mètode torna *this.
   * @see put()
   * @see flush()
   */
  std::ostream& put(const size_t val, int8_t bits)
  {
    assert(bits >= 1 && (unsigned)bits <= BYTES2BITS(sizeof(size_t)));
    for(--bits; bits >= 0 && good(); --bits)
      put((val >> bits) & 0x01);
    return *this;
  }

  /**
   * @brief Escriu una seqüència de bits sobre el fluxe d'eixida.
   *
   * IMPORTANT: Pot succeir que queden bits en el buffer que no han estat
   * volcats al fluxe d'eixida. Per a forçar el volcat s'utilitza el mètode flush().
   * @param vec seqüència de bits a escriure en el fluxe.
   * @param n nombre de bits en la seqüència.
   * @return Aquest mètode torna *this.
   * @see put()
   * @see flush()
   */
  std::ostream& write(const Bit * vec, size_t n)
  {
    for(size_t i = 0; i < n && good(); ++i)
      put(vec[i]);
    return *this;
  }

  /**
   * @brief Escriu una seqüència de bytes sobre el fluxe d'eixida.
   *
   * IMPORTANT: Pot succeir que queden bits en el buffer que no han estat
   * volcats al fluxe d'eixida. Per a forçar el volcat s'utilitza el mètode flush().
   * @param vec seqüència de bytes a escriure en el fluxe.
   * @param n nombre de bytes en la seqüència.
   * @return Aquest mètode torna *this.
   * @see put()
   * @see flush()
   */
  std::ostream& write(const byte * vec, size_t n) 
  {
    for(size_t i = 0; i < n && good(); ++i)
      for(int j = 7; j >= 0 && good(); --j)
	put(((vec[i]>>j)&0x01));
    return *this;
  }

  /**
   * @brief Escriu un bit en el fluxe d'eixida.
   * 
   * IMPORTANT: Pot succeir que queden bits en el buffer que no han estat
   * volcats al fluxe d'eixida. Per a forçar el volcat s'utilitza el mètode flush().
   * @param d bit a escriure en el fluxe.
   * @return Aquest mètode torna *this.
   * @see put()
   * @see flush()
   */
  std::ostream& operator << (const Bit& d)
  {
    return put(d);
  }
  
  /**
   * @brief Força l'escriptura del buffer en el fluxe d'eixida i reinicia
   * el buffer d'eixida.
   *
   * Les dades escrites després de l'últim bit ocupat en el buffer 
   * són desconegudes (contindran els bits de l'operació d'escriptura anterior).
   * Per exemple, si el buffer sols té ocupades les 2 primeres possicions, 
   * el contingut del buffer serà p.ex: 01XXXXXX. El byte sencer s'escriurà en
   * el fluxe d'eixida.
   * @return Aquest mètode torna *this.
   */
  std::ostream& flush(void)
  {
    if ( buffer_pos != BYTES2BITS(sizeof(byte))-1 ) {
      ((std::ostream*)this)->put(bit_buffer);
      initBuffer();   // reiniciem buffer
    }
    ((std::ostream*)this)->flush();
    return *this;
  }
};

#endif
