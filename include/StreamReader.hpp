/**
 * @file StreamReader.hpp
 * @brief Fitxer amb la definició de la classe abstracta StreamReader.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __STREAMREADER_HPP__
#define __STREAMREADER_HPP__

#include <iostream>

/**
 * @class StreamReader
 * @brief Classe abastracta que implementa un lector 
 * de símbols genèric sobre un fluxe d'entrada.
 */
template <typename T>
class StreamReader : public std::istream {
public:
  /** 
   * @brief Constructor per defecte. 
   * Els símbols es llegeixen de l'entrada estàndard. 
   */
  StreamReader()
    : std::istream(std::cin.rdbuf())
  { }
  
  /** 
   * @brief Constructor. 
   * Les símbols es llegeixen del fluxe indicat. 
   * @param is fluxe sobre el que es llegiran els símbols.
   */
  StreamReader(std::istream& is)
    : std::istream(is.rdbuf())
  { }
  
  /**
   * @brief Llegeix un símbol des del fluxe d'entrada.
   * @return torna el símbol llegit.
   */
  virtual T get() = 0;

  /**
   * @brief Llegeix un bloc de símbols del fluxe d'entrada.
   * @param vec buffer on s'emmagatzemen els símbols llegits.
   * @param n nombre de símbols a llegir.
   * @return Aquest mètode torna *this.
   */
  virtual std::istream& read(T * vec, size_t n) = 0;

  /**
   * @brief Llegeix un símbol del fluxe d'entrada.
   * @param d símbol llegit del fluxe.
   * @return Aquest mètode torna *this.
   */
  virtual std::istream& operator >> (T& d) = 0;

  /**
   * @brief Retorna el nombre de símbols llegits
   * en l'última operació de lectura.
   * @return nombre de símbols llegits.
   */
  virtual std::streamsize gcount() const = 0;
};

#endif
