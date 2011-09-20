/**
 * @file LZ78CompressorExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe LZ78Compressor
 * per a comprimir un fitxer de dades utilitzant l'algorisme LZ78.
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <LZ78Compressor.hpp>

using namespace std;

/**
 * @brief Mostra l'ajuda del programa.
 */
void show_usage (const char * name)
{
  cerr << "Usage: " << name << " [--ds ds] [--bs bs] FILE" << endl;
  cerr << "This generates a compressed copy of FILE named FILE.lz78" << endl;
  cerr << "--db db    db bits used to store the position in the dictionary" << endl;
  cerr << "           (default: 14)." << endl;
  cerr << "--bb bb    works with blocks of 2^bb bytes (default: 5)." << endl;
}

/**
 * @brief Aquest exemple mostra com utilitzar la classe LZ78Compressor
 * per a comprimir un fitxer de dades utilitzant l'algorisme LZ78.
 */
int main(int argc, char ** argv)
{
  LZ78Compressor compressor;
  ifstream f_input;
  ofstream f_output;
 
  if (argc < 2) {
    show_usage(argv[0]);
    return 1;
  }

  uint8_t dictionary_bits = 14, block_bits = 5;
  int a = 1;
  for(; a < argc && !strncmp(argv[a], "--", 2); ++a) {
    if ( !strcmp(argv[a], "--bb") ) block_bits = (uint8_t)atoi(argv[++a]);
    else if ( !strcmp(argv[a], "--db") ) dictionary_bits = (uint8_t)atoi(argv[++a]);
    else { show_usage(argv[0]); return 1; }
  }
  
  /* Obrim el fitxer d'entrada. */
  f_input.open(argv[a], ios::binary);
  if ( !f_input.is_open() ) {
    cerr << "File \"" << argv[a] << "\" couldn't been opened!" << endl;
    return 1;
  }

  /* El fitxer d'eixida tindrà el mateix nom que el d'entrada
     però afegint l'extensió ".huf". */
  char * outname = new char [strlen(argv[a])+strlen(".lz78")+1];
  strcpy(outname, argv[a]);
  strcat(outname, ".lz78");
  outname[strlen(argv[a])+strlen(".lz78")] = '\0';
  
  /* Obrim el fitxer d'eixida. */
  f_output.open(outname, ios::binary);
  if ( !f_output.is_open() ) {
    cerr << "File \"" << outname << "\" couldn't been opened!" << endl;
    return 1;
  }

  /* Cridem al compressor indicant-li els fitxers d'entrada i d'eixida. */
  if ( !compressor.compress(f_input, f_output, dictionary_bits, block_bits) ) {
    cerr << "Compressing error!" << endl;
  }

  /* Alliberem memòria utilitzada. */
  delete [] outname;

  /* Tanquem fitxers de manera segura. */
  f_input.close();
  f_output.close();

  return 0;
}
