/**
 * @file Codification.hpp
 * @brief File including the implementation of Codification class.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __CODIFICATION_HPP__
#define __CODIFICATION_HPP__

#include <map>
#include <vector>

/**
 * @class Codification
 * @brief Class representing a codification between symbols of an alphabet A and
 * strings formed by symbols of an alphabet B.
 */
template <typename SymbA, typename SymbB>
class Codification : public std::map< SymbA, std::vector<SymbB> >{
  /**
   * @brief Shows the codification to a output stream as follows:
   *
   * symbol_1: code_1\n
   * symbol_2: code_2\n
   * ...\n
   * symbol_n: code_n
   * @param os output stream.
   * @param codif codification.
   * @return This method returns the output stream.
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
