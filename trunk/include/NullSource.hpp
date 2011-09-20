/**
 * @file NullSource.hpp
 * @brief Fitxer amb la implementació de la classe NullSource.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __NULLSOURCE_HPP__
#define __NULLSOURCE_HPP__

#include <cassert>
#include <fstream>
#include <string>
#include <map>

/**
 * @class NullSource
 * @brief Implementació d'una font de memòria nula.
 *
 * Una font de memòria nula és una funció \f$f : \Sigma \rightarrow \mathbb{R}\f$, de manera 
 * que assigna una probabilitat d'emissió a cada símbol d'un alfabet \f$\Sigma\f$.
 * En aquesta implementació els símbols de l'alfabet seran bytes (tipus char de C/C++), ja un
 * byte és la unitat mínima d'informació que pot emmagatzemar-se en un fitxer.
 *
 * La font de memòria nula assignarà les probabilitats d'emissió dels diferents símbols que 
 * emeta a partir de les freqüències de cada símbol en un fluxe d'entrada.
 * Expressat matemàticament \f$f(s) = \frac{N_s}{N}\f$ on \f$N_s\f$ és el nombre d'aparicions del
 * símbol \f$s\f$ en el fluxe d'entrada i \f$N\f$ és el nombre total de símbols llegits.
 *
 * Internament, sols s'associa a cada símbol \f$N_s\f$ i no la fracció \f$\frac{N_s}{N}\f$, ja que
 * la segona pot obtindre's a partir de la primera i això simplifica i fa més eficient la 
 * creació de la memòria de font nula.
 */
class NullSource : public std::map<char, size_t> {
private:
  /** Nombre de símbols llegits des del fluxe d'entrada. */
  size_t read_symbols;
public:
  /** Iterador per a la font. */
  typedef std::map<char,size_t>::iterator iterator;
  /** Iterador constant per a la font. */
  typedef std::map<char,size_t>::const_iterator const_iterator;
  
  /**
   * @brief Constructor per defecte. Inicialitza la font.
   */
  NullSource() : std::map<char,size_t>(), read_symbols(0)
  { }
  
  /**
   * @brief Construeix la font de memòria nula a partir d'un fluxe d'entrada.
   * @param reader fluxe d'entrada.
   * @return torna true si s'ha acabat de llegir el fitxer o false si ha hagut algun
   * error en la lectura.
   */
  bool LoadFromStream(std::istream & reader)
  {
    this->clear();
    read_symbols = 0;
    while ( !reader.eof() ) {
      char sym = reader.get();
      if (!reader.good()) break;
      (*this)[sym]++;
      ++read_symbols;
    }
    return reader.eof();
  }
  
  /**
   * @brief Construeix la font de memòria nula a partir d'un fitxer de dades.
   * @param filename nom del fitxer a utilitzar.
   * @return Torna true si s'ha pogut obrir el fitxer i s'ha llegit correctament,
   * o false en cas contrari.
   */
  bool LoadFromFile(const std::string& filename)
  {
    std::ifstream file;
    file.open(filename.c_str(), std::ios::binary);
    if ( !file.is_open() ) return false;
    bool res = LoadFromStream(file);
    file.close();
    return res;
  }

  /**
   * @brief Obté les freqüències d'emissió associades a cada símbol de la font.
   * @return torna un map que associa cada símbol amb la seva freqüència.
   */
  std::map<char, double> getFrequencies(void) const
  {
    typedef std::map<char,double> FreqMapT;
    typedef FreqMapT::iterator FreqMapTIt;
    FreqMapT fmap( this->begin(), this->end() );
    for(FreqMapTIt it = fmap.begin(); it != fmap.end(); ++it)
      it->second /= read_symbols;
    return fmap;
  }

  /**
   * @brief Obté el nombre de símbols que es van llegir des del fluxe d'entrada (\f$N\f$).
   * @return nombre de símbols llegits.
   */
  size_t getReadSymbols(void) const 
  {
    return read_symbols;
  }
};

#endif

/**
 * @example CharCountExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe NullSource 
 * per a obtenir el nombre d'aparicions de cada símbol d'un fitxer.
 *
 * @example CharFrequenciesExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe NullSource 
 * per a obtenir la freqüència d'aparició de cada símbol d'un fitxer.
 */
