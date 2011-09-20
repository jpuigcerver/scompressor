/**
 * @file BitStreamWriter.hpp
 * @brief File including the implementation of BitStreamWriter class.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __BITSTREAMWRITER_HPP__
#define __BITSTREAMWRITER_HPP__

#include <cassert>
#include <stdint.h>
#include <StreamWriter.hpp>
#include <Bit.hpp>

/**
 * @class BitStreamWriter
 * @brief This class implements a bit writer to a data stream.
 * @see StreamWriter
 */
class BitStreamWriter : public StreamWriter<Bit> {
private:
  /** Definition of byte. */
  typedef char byte;
  /** Buffer. */
  byte bit_buffer;
  /** Possition in the buffer. */
  uint8_t buffer_pos;
  
  /** Initializes the buffer. */
  inline void initBuffer(void)
  {
    bit_buffer = 0x00;
    buffer_pos = BYTES2BITS(sizeof(byte))-1;
  }
  
public:
  /**
   * @brief Default constructor.
   * Bits will be written to the standard output.
   */
  BitStreamWriter() 
    : StreamWriter<Bit>(), 
      bit_buffer(0x00), buffer_pos(BYTES2BITS(sizeof(byte))-1)
  { }
  
  /** 
   * @brief Constructor. 
   * Bits will be written to the indicated data stream.
   * @param out output stream.
   */
  BitStreamWriter(std::ostream& out)
    : StreamWriter<Bit>(out),
      bit_buffer(0x00), buffer_pos(BYTES2BITS(sizeof(byte))-1)
  { }
  
  /**
   * @brief Writes a bit to the output stream.
   *
   * Bits are written to a output buffer and when this is full,
   * its content is flushed to the output stream and the buffer is reset.
   * 
   * WARNING: It is important to use the flush() method to ensure that all bits
   * are written after the last output operation.
   * @param d bit to be written.
   * @return This method returns *this.
   * @see flush()
   */
  std::ostream& put(const Bit& d)
  {
    bit_buffer |= ((byte)d << buffer_pos);
    if(buffer_pos == 0) {
      ((std::ostream*)this)->put(bit_buffer);
      initBuffer();
    } else --buffer_pos;
    return *this;
  }

  /**
   * @brief Writes a size_t number using a given number of bits.
   *
   * WARNING: It is important to use the flush() method to ensure that all bits
   * are written after the last output operation.
   * @param val value to be written.
   * @param bits number of bits to use.
   * @return This method returns *this.
   * @see put()
   * @see flush()
   */
  std::ostream& put(const size_t val, int8_t bits)
  {
    assert(bits >= 1 && (unsigned)bits <= BYTES2BITS(sizeof(size_t)));
    for(--bits; bits >= 0 && good(); --bits)
      put((val >> bits) & 0x01);
    return *this;
  }

  /**
   * @brief Writes a sequence of bits to the output stream.
   *
   * WARNING: It is important to use the flush() method to ensure that all bits
   * are written after the last output operation.
   * @param vec bits sequence to be written.
   * @param n number of bits in the sequence.
   * @return This method returns *this.
   * @see put()
   * @see flush()
   */
  std::ostream& write(const Bit * vec, size_t n)
  {
    for(size_t i = 0; i < n && good(); ++i)
      put(vec[i]);
    return *this;
  }

  /**
   * @brief Writes a sequence of bytes to the output stream.
   *
   * WARNING: It is important to use the flush() method to ensure that all bits
   * are written after the last output operation.
   * @param vec bytes sequence to be written.
   * @param n number of bytes in the sequence.
   * @return This method returns *this.
   * @see put()
   * @see flush()
   */
  std::ostream& write(const byte * vec, size_t n) 
  {
    for(size_t i = 0; i < n && good(); ++i)
      for(int j = 7; j >= 0 && good(); --j)
	put(((vec[i]>>j)&0x01));
    return *this;
  }

  /**
   * @brief Writes a bit to the output stream.
   * 
   * WARNING: It is important to use the flush() method to ensure that all bits
   * are written after the last output operation.
   * @param d bit to be written.
   * @return This method returns *this.
   * @see put()
   * @see flush()
   */
  std::ostream& operator << (const Bit& d)
  {
    return put(d);
  }
  
  /**
   * @brief Forces the content of the buffer to be written to the output stream.
   *
   * The data after the last bit in the buffer are unknown (they will probably contain
   * bits from the previous operations).
   * For example, if the buffer has only its two firs positions occupied,
   * the content of the buffer will be for instance: 01XXXXXX. The whole byte will be
   * written in the output stream.
   * @return This method returns *this.
   */
  std::ostream& flush(void)
  {
    if ( buffer_pos != BYTES2BITS(sizeof(byte))-1 ) {
      ((std::ostream*)this)->put(bit_buffer);
      initBuffer();   // reset buffer
    }
    ((std::ostream*)this)->flush();
    return *this;
  }
};

#endif
