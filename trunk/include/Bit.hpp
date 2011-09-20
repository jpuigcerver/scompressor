/**
 * @file Bit.hpp
 * @brief File including the implementation of Bit class.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __BIT_HPP__
#define __BIT_HPP__

#include <iostream>

/**
 * @def BYTES2BITS
 * @brief Convert a value in bytes to bits. x8 multiplication. (i.e: 2 bytes = 1 bits).
 */
#define BYTES2BITS(A) (A << 3)

/**
 * @class Bit
 * @brief This class implements the abstract type of 'bit'.
 *
 * Internally is a char, but it is only allowed to have the value
 * 0 or 1. This is useful to do bit operations.
 * Moreover, this class overload the << operator of class ostream to
 * show the value correctly.
 */
class Bit {
private:
  /** Value. */
  char value;

public:
  /**
   * @brief Default constructor. Default value is zero.
   */
  Bit() 
    : value(0)
  {}

  /**
   * @brief Copy constructor.
   * @param val bit's value.
   * Any value distinct to 0 is considered 1.
   */ 
  Bit(char val)
    : value(val ? 1 : 0)
  {}

  /**
   * @brief Casting operator to char.
   * @return bit's value.
   */
  operator char () const
  {
    return value;
  }

  /**
   * @brief Casting operator to const char *.
   * @return bit's value as a character string (0->"0", 1->"1").
   */
  operator const char * () const
  {
    return (value ? "1" : "0");
  }

  /**
   * @brief Operator << of ostream class is overloaded.
   * This writes in the output stream the bit's value
   * as a character (0->'0', 1->'1').
   * @param os output stream.
   * @param bit bit object to write.
   * @return ostream output stream.
   */
  friend std::ostream& operator << (std::ostream& os, const Bit bit) {
    os << (bit.value ? '1' : '0');
    return os;
  }
};

#endif
