/**
 * @file GenericCompressor.hpp
 * @brief Fitxer amb la definició de la classe abstracta GenericCompressor.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __GENERICCOMPRESSOR_HPP__
#define __GENERICCOMPRESSOR_HPP__

/**
 * @class GenericCompressor
 * @brief Classe abstracta que defineix els mètodes que han de proporcionar les 
 * classes compressores/descompressores per a utilitzar-se adequadament.
 */
class GenericCompressor {
public:
  /**
   * @brief Comprimeix el fluxe d'entrada de input i escriu el resultat en output.
   * @param input fluxe d'entrada que vol comprimir-se.
   * @param output fluxe d'eixida on es deixen les dades comprimides.
   * @return true si tot ha anat bé, false en cas contrari.
   */
  virtual bool compress(std::istream& input, std::ostream& output) = 0;

  /**
   * @brief Descomprimeix el fluxe d'entrada d'input i escriu el resultat en output.
   * @param input fluxe de d'entrada que vol descomprimir-se.
   * @param output fluxe de d'eixida on s'escriuen les dades descomprimides.
   * @return true si tot ha anat bé, false en cas contrari.
   */
  virtual bool decompress(std::istream& input, std::ostream& output) = 0;
};

#endif

