/**
 * @file BitStreamReader.hpp
 * @brief File including the implementation of BitStreamReader class.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __BITSTREAMREADER_HPP__
#define __BITSTREAMREADER_HPP__

#include <cassert>
#include <stdint.h>
#include <StreamReader.hpp>
#include <Bit.hpp>

/**
 * @class BitStreamReader
 * @brief This class implements a bit reader from a data stream.
 * @see StreamReader
 */
class BitStreamReader: public StreamReader<Bit> {
private:
  /** Definition of byte. */
  typedef char byte;
  /** Buffer. */
  byte bit_buffer;
  /** Possition in the buffer. */
  uint8_t buffer_pos;
  /** Number of read symbols since the last input operation. */
  size_t last_read;

public:
  /**
   * @brief Default constructor.
   * The input stream is the standard input.
   */
  BitStreamReader() 
    : StreamReader<Bit>(), bit_buffer(0x00), buffer_pos(0xFF), last_read(0)
  { }

  /**
   * @brief Constructor.
   * Bits will be written from the indicated data stream.
   * @param in input stream.
   */
  BitStreamReader(std::istream& in)
    : StreamReader<Bit>(in), bit_buffer(0x00), buffer_pos(0xFF), last_read(0)
  { }
    
  /**
   * @brief Reads a single bit from the input stream.
   *
   * Actually, it reads a byte from the imput stream
   * and each bit is returned in the successive calls.
   * When all the bits of the read byte are returned,
   * a new byte will be read then.
   * @return read bit.
   */
  Bit get()
  {
    last_read = 0;
    if( buffer_pos == 0xFF ) {
      bit_buffer = ((std::istream*)this)->get();
      if ( !good() ) return 0;
      buffer_pos = BYTES2BITS(sizeof(byte))-1;
    }

    Bit v(bit_buffer & (0x01 << buffer_pos--));
    last_read = 1;
    return v;
  }

  /**
   * @brief Reads a given number of bits from the input stream. The 
   * read chunk of bits is interpreted as a size_t unsigned number.
   * @param bits number of bits to read.
   * @return read value.
   * @see get()
   */
  size_t get(unsigned char bits)
  {
    assert(bits >= 1 && (unsigned)bits <= BYTES2BITS(sizeof(size_t)));
    size_t res = 0x00;
    for(--bits; bits >= 0 && good(); --bits)
      res |= (get() << bits);
    return res;
  }

  /**
   * @brief Reads a sequence of bits from the input stream.
   * @param vec Adress where the sequence will be stored.
   * @param n number of bits to read.
   * @return This method returns *this.
   * @see get()
   */
  std::istream& read(Bit * vec, size_t n)
  {
    for(last_read = 0; last_read < n && good(); ++last_read)
      vec[last_read] = get();
    return *this;
  }

  /**
   * @brief Reads a sequence of bytes from the input stream.
   * @param vec Adress where the sequence will be stored.
   * @param n number of bytes to read.
   * @return This method returns *this.
   * @see get()
   */
  std::istream& read(byte * vec, size_t n) 
  {
    last_read = 0;
    for(size_t i = 0; i < n && good(); ++i) {
      vec[0] = 0x00;
      for(int j = 7; j >= 0 && good(); --j, ++last_read)
	vec[i] |= (get() << j);
    }
    return *this;
  }

  /**
   * @brief Reads a bit from the input stream.
   * @param d bit object that will contain the read value.
   * @return This method returns *this.
   * @see get()
   */
  std::istream& operator >> (Bit& d)
  {
    d = get();
    return *this;
  }

  /** 
   * @brief Retrieves the number of read bits in the last input operation.
   * @return number of read bits in the last input operation.
   */
  std::streamsize gcount() const
  {
    return last_read;
  }
  
};

#endif
