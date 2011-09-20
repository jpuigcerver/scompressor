/**
 * @file StreamWriter.hpp
 * @brief Fitxer amb la definició de la classe abstracta StreamWriter.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __STREAMWRITER_HPP__
#define __STREAMWRITER_HPP__

#include <iostream>

/**
 * @class StreamWriter
 * @brief Classe abastracta que implementa un escriptor 
 * de dades genèric sobre un fluxe d'eixida.
 */
template <typename T>
class StreamWriter : public std::ostream {
public:
  /** 
   * @brief Constructor per defecte. 
   * Les dades s'escriuen en l'eixida estàndard. 
   */
  StreamWriter()
    : std::ostream(std::cout.rdbuf())
  { }
  
  /** 
   * @brief Constructor. 
   * Les dades s'escriuen en el fluxe indicat. 
   * @param out fluxe sobre el que s'escriuran les dades.
   */
  StreamWriter(std::ostream& out)
    : std::ostream(out.rdbuf())
  { }
  
  /**
   * @brief Escriu un símbol en el fluxe d'eixida.
   * @param d símbol a escriure.
   * @return Aquest mètode torna *this.
   */
  virtual std::ostream& put(const T& d) = 0;

  /**
   * @brief Escriu un bloc de símbols en el fluxe d'eixida.
   * @param vec bloc de símbols a escriure.
   * @param n nombre de símbols en el bloc.
   * @return Aquest mètode torna *this.
   */
  virtual std::ostream& write(const T * vec, size_t n) = 0;

  /**
   * @brief Escriu un símbol en el fluxe d'eixida.
   * @param d símbol a escriure.
   * @return Aquest mètode torna *this.
   */
  virtual std::ostream& operator << (const T& d) = 0;
};

#endif
