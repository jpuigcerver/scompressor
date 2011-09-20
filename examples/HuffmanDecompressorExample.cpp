/**
 * @file HuffmanDecompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe HuffmanCompressor
 * per a descomprimir un fitxer de comprimit utilitzant el mètode Huffman.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#include <iostream>
#include <HuffmanCompressor.hpp>

using namespace std;

/**
 * @brief Aquest exemple mostra com utilitzar la classe HuffmanCompressor
 * per a descomprimir un fitxer de comprimit utilitzant el mètode Huffman.
 */
int main(int argc, char ** argv)
{
  HuffmanCompressor compressor;
  ifstream f_input;
  ofstream f_output;
  ostream * output;
  
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " INPUT [OUTPUT]" << endl;
    cerr << "This decompress the file INPUT and writes the result to OUTPUT" << endl;
    cerr << "If OUTPUT is omitted, the result is written to stdout" << endl;
    return 1;
  }
  
  /* Obrim el fitxer d'entrada. */
  f_input.open(argv[1], ios::binary);
  if ( !f_input.is_open() ) {
    cerr << "File \"" << argv[1] << "\" couldn't been opened!" << endl;
    return 1;
  }

  /* Decidim quin serà el fitxer d'eixida. 
     Si no s'ha indicat cap nom, l'eixida serà l'estàndard. */
  if ( argc < 3 ) {
    output = &cout;
  } else {  
    /* Obrim el fitxer d'eixida. */
    f_output.open(argv[2], ios::binary);
    if ( !f_output.is_open() ) {
      cerr << "File \"" << argv[2] << "\" couldn't been opened!" << endl;
      return 1;
    }
    output = &f_output;
  }

  /* Cridem al descompressor amb el fitxer d'entrada i el d'eixida. */
  if ( !compressor.decompress(f_input, *output) ) {
    cerr << "Decompressing error!" << endl;
  }

  /* Tanquem els fitxers de manera segura. */
  f_input.close();
  f_output.close();

  return 0;
}
