/**
 * @file CharFrequenciesExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe NullSource 
 * per a obtenir la freqüència d'aparició de cada símbol d'un fitxer.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#include <iostream>
#include <NullSource.hpp>

using namespace std;

/** 
 * @brief Aquest exemple mostra com utilitzar la classe NullSource 
 * per a obtenir la freqüència d'aparició de cada símbol d'un fitxer. 
 */
int main(int argc, char ** argv)
{
  if( argc < 2 ) {
    cout << "Usage: " << argv[0] << " <file>" << endl;
    return 1;
  }

  NullSource source;
  if ( !source.LoadFromFile(argv[1]) ) {
    cout << "File \"" << argv[1] << "\" couldn't been opened!" << endl;
    return 1;
  }
  
  std::map<char,double> mf = source.getFrequencies();

  for(std::map<char,double>::const_iterator it = mf.begin();
      it != mf.end(); ++it) 
    cout << it->first << "->" << it->second << endl;

  return 0;
}
