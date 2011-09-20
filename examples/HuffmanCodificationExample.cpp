/**
 * @file HuffmanCodificationExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe HuffmanTree
 * per a obtenir la codificació de Huffman de cada símbol d'un fitxer.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#include <iostream>
#include <NullSource.hpp>
#include <HuffmanTree.hpp>

using namespace std;

/**
 * @brief Aquest exemple mostra com utilitzar la classe HuffmanTree
 * per a obtenir la codificació de Huffman de cada símbol d'un fitxer.
 */
int main(int argc, char ** argv)
{
  if( argc < 2 ) {
    cout << "Usage: " << argv[0] << " <file>" << endl;
    return 1;
  }

  /* Carreguem la font de memoria nula amb 
     les dades del fitxer d'entrada. */
  NullSource source;
  if ( !source.LoadFromFile(argv[1]) ) {
    cout << "File \"" << argv[1] << "\" couldn't been opened!" << endl;
    return 1;
  }

  /* Obtenim la codificació huffman a partir de la 
     font de memòria nula. */
  HuffmanTree huffman(source);
  Codification<char, Bit> codif = huffman.getCodification();

  cout << codif;
  cout << "Median length = " << huffman.getMedianLength(source.getReadSymbols()) << endl;

  return 0;
}
