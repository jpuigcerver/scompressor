/**
 * @file Codification.hpp
 * @brief Fitxer amb la implementació de la classe Codification.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __CODIFICATION_HPP__
#define __CODIFICATION_HPP__

#include <map>
#include <vector>

/**
 * @class Codification
 * @brief Classe que representa una funció de codificació entre els
 * símbols d'un alfabet A i cadenes formades amb símbols d'un alfabet B.
 *
 * Els símbols poden ser objectes de qualsevol tipus, encara que generalment
 * seran de tipus char. Les cadenes es representen com un vector de 
 * símbols.
 */
template <typename SymbA, typename SymbB>
class Codification : public std::map< SymbA, std::vector<SymbB> >{
  /**
   * @brief Volca una codificació en un fluxe d'eixida de la forma:
   *
   * simbol_1: codi_1\n
   * simbol_2: codi_2\n
   * ...\n
   * simbol_n: codi_n
   * @param os Fluxe d'eixida sobre el que escriure la codificació.
   * @param codif Codificació a escriure.
   * @return Aquest mètode torna el fluxe d'eixida en el que s'ha volcat la codificació.
   */
  friend std::ostream& operator << (std::ostream& os, const Codification<SymbA,SymbB> & codif)
  {
    typedef typename Codification<SymbA,SymbB>::const_iterator Cod_const_iterator;
    typedef typename std::vector<SymbB>::const_iterator Vec_const_iterator;

    for(Cod_const_iterator itc = codif.begin(); itc != codif.end(); ++itc) {
      os << itc->first << ": ";
      for(Vec_const_iterator itv = itc->second.begin(); itv != itc->second.end(); ++itv) 
	os << *itv;
      os << std::endl;
    }
    return os;
  }
};

#endif 
