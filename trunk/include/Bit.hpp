/**
 * @file Bit.hpp
 * @brief Fitxer amb la implementació de la classe Bit.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __BIT_HPP__
#define __BIT_HPP__

#include <iostream>

/**
 * @def BYTES2BITS
 * @brief Converteix un valor en bytes a bits (multiplica per 8).
 */
#define BYTES2BITS(A) (A << 3)

/**
 * @class Bit
 * @brief Implementació del tipus 'bit'. 
 *
 * Internament és un char, però que sols pot tenir el valor 
 * 0 o 1, útil per a fer operacions amb bits.
 * A més, es sobrecarrega l'operador << de ostreams per a que 
 * mostre el caràcter '0' o '1' depenent del seu valor.
 */
class Bit {
private:
  /** Valor del bit. */
  char value;

public:
  /**
   * @brief Constructor buit per defecte. Bit a zero.
   */
  Bit() 
    : value(0)
  {}

  /**
   * @brief Constructor de còpia a partir d'un char.
   * @param val Valor del bit a copiar.
   * Qualsevol valor distint de 0, s'interpreta com a 1. 
   */ 
  Bit(char val)
    : value(val ? 1 : 0)
  {}

  /**
   * @brief Operador de casting per al tipus (char).
   * @return valor del bit.
   */
  operator char () const
  {
    return value;
  }

  /**
   * @brief Operador de casting per al tipus (const char *).
   * @return representació en forma de cadena de caràcters del valor del bit.
   */
  operator const char * () const
  {
    return (value ? "1" : "0");
  }

  /**
   * @brief Sobrecàrrega de l'operador << per a la classe ostream.
   * Escriu en el stream la representació en forma de caràcter del valor
   * del bit.
   * @param os objecte de tipus ostream sobre el que s'escriurà.
   * @param bit objecte Bit a escriure en el stream.
   * @return ostream d'entrada.
   */
  friend std::ostream& operator << (std::ostream& os, const Bit bit) {
    os << (bit.value ? '1' : '0');
    return os;
  }
};

#endif
