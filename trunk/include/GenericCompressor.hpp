/**
 * @file GenericCompressor.hpp
 * @brief File including the definition of the abstract GenericCompressor class.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __GENERICCOMPRESSOR_HPP__
#define __GENERICCOMPRESSOR_HPP__

/**
 * @class GenericCompressor
 * @brief Abstract class that defines the methods that all the compressors must provide.
 */
class GenericCompressor {
public:
  /**
   * @brief Compresses data from the input stream and the result is written to the output stream.
   * @param input input stream to be compressed.
   * @param output output stream where the compressed data will be written.
   * @return true if the compression was successful, false if it was not.
   */
  virtual bool compress(std::istream& input, std::ostream& output) = 0;

  /**
   * @brief Decompresses data from the input stream and the result is written to the output stream.
   * @param input input stream to be decompressed.
   * @param output output stream where the decompressed data will be written.
   * @return true if the decompression was successful, false if it was not.
   */
  virtual bool decompress(std::istream& input, std::ostream& output) = 0;
};

#endif

