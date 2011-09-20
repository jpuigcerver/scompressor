/**
 * @file CharCountExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe NullSource 
 * per a obtenir el nombre d'aparicions de cada símbol d'un fitxer.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#include <iostream>
#include <fstream>
#include <NullSource.hpp>

using namespace std;

/**
 * @brief Aquest exemple mostra com utilitzar la classe NullSource 
 * per a obtenir el nombre d'aparicions de cada símbol d'un fitxer.
 */
int main(int argc, char ** argv)
{
  if( argc < 2 ) {
    cout << "Usage: " << argv[0] << " <file>" << endl;
    return 1;
  }

  /* Obrim el fitxer indicat. */
  ifstream file;
  file.open(argv[1]);
  if ( !file.is_open() ) {
    cout << "File \"" << argv[1] << "\" couldn't been opened!" << endl;
    return 1;
  }

  /* Creem una font de memòria nula i la carreguem
     a partir del lector de caràcters. */
  NullSource source;
  source.LoadFromStream(file);

  /* Recorreguem tots els símbols de la font i mostrem el
     seu nombre d'aparicions en el fitxer. */
  for(NullSource::const_iterator it = source.begin();
      it != source.end(); ++it)
    cout << it->first << "->" << it->second << endl;
  
  file.close();

  return 0;
}
