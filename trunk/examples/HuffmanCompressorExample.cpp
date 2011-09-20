/**
 * @file HuffmanCompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe HuffmanCompressor
 * per a comprimir un fitxer de dades.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#include <iostream>
#include <HuffmanCompressor.hpp>

using namespace std;

/**
 * @brief Aquest exemple mostra com utilitzar la classe HuffmanCompressor
 * per a comprimir un fitxer de dades.
 */
int main(int argc, char ** argv)
{
  HuffmanCompressor compressor;
  ifstream f_input;
  ofstream f_output;
 
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " FILE" << endl;
    cerr << "This generates a compressed copy of FILE named FILE.huff" << endl;
    return 1;
  }
  
  /* Obrim el fitxer d'entrada. */
  f_input.open(argv[1], ios::binary);
  if ( !f_input.is_open() ) {
    cerr << "File \"" << argv[1] << "\" couldn't been opened!" << endl;
    return 1;
  }

  /* El fitxer d'eixida tindrà el mateix nom que el d'entrada
     però afegint l'extensió ".huf". */
  char * outname = new char [strlen(argv[1])+strlen(".huf")+1];
  strcpy(outname, argv[1]);
  strcat(outname, ".huf");
  outname[strlen(argv[1])+strlen(".huf")] = '\0';
  
  /* Obrim el fitxer d'eixida. */
  f_output.open(outname, ios::binary);
  if ( !f_output.is_open() ) {
    cerr << "File \"" << outname << "\" couldn't been opened!" << endl;
    return 1;
  }

  /* Cridem al compressor indicant-li els fitxers d'entrada i d'eixida. */
  if ( !compressor.compress(f_input, f_output) ) {
    cerr << "Compressing error!" << endl;
  }

  /* Alliberem memòria utilitzada. */
  delete [] outname;

  /* Tanquem fitxers de manera segura. */
  f_input.close();
  f_output.close();

  return 0;
}
