/**
 * @file ByteChunk.hpp
 * @brief File including the implementation of ByteChunk class.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date May 2011
 */

#ifndef __BYTECHUNK_HPP__
#define __BYTECHUNK_HPP__

#include <cstring>
#include <cassert>

/**
 * @class ByteChunk
 * @brief This class manages a chunk of bytes. This is a personalized
 * alternative to vector<char>.
 */
class ByteChunk {
  /** Chunk of bytes. */
  char * _chunk;
  /** Current size of the chunk of bytes. */
  size_t _size;
  /** Capacity of the chunk. */
  size_t _capacity;

  /**
   * @brief This allocates the space for the chunk.
   * @param capacity number of bytes to be allocated.
   */
  inline void _allocate ( size_t capacity )
  {
    if ( capacity > 0 )
      assert ( (_chunk = new char [(_capacity = capacity)]) != NULL );
  }

  /**
   * @brief This copies a sequence of bytes to the chunk.
   * @param src sequence to be copied.
   * @param size length of the sequence.
   */
  inline void _copy ( const char * src, size_t size )
  { if ( size > 0 ) memcpy ( _chunk, src, size ); }

public:
  /**
   * @brief Default constructor.
   */
  ByteChunk ( ) : _chunk(0), _size(0), _capacity(0) { }

  /**
   * @brief Copy constructor.
   * @param bc chunk of bytes.
   */
  ByteChunk ( const ByteChunk& bc ) : _chunk(0), _size(bc._size), _capacity(bc._size) 
  { if(_capacity > 0){ _allocate(_capacity); _copy(bc._chunk, bc._size); } }

  /**
   * @brief Copy constructor.
   * @param chunk sequence of bytes.
   * @param size length of the sequence.
   */
  ByteChunk ( const char * chunk, size_t size ) : _chunk(0), _size(size), _capacity(size)
  { if(_capacity > 0){ _allocate(_capacity); _copy(chunk, _size); } }

  /**
   * @brief Copy constructor.
   * This creates a chunk of size 1 and the value as indicated.
   * @param byte byte to be allocated.
   */
  ByteChunk ( const char byte ) : _chunk(0), _size(1), _capacity(_size)
  { _allocate(_capacity); _chunk[0] = byte; }

  /**
   * @brief Constructor. This creates a new chunk with the indicated capacity.
   * @param capacity capacity of the chunk.
   */
  ByteChunk ( const size_t capacity ) : _chunk(0), _size(0), _capacity(capacity)
  { if(_capacity > 0){ _allocate(_capacity); } }

  /**
   * @brief Destructor.
   */
  ~ByteChunk ( )
  { clear(); }

  /**
   * @brief Copy operator.
   * @param bc ByteChunk to be copied.
   * @return this method returns *this.
   */
  ByteChunk& operator = ( const ByteChunk& bc )
  {
    if ( bc._size > _capacity ) {
	clear();
	_allocate(bc._size);
    }
    _size = bc._size;
    _copy(bc._chunk, bc._size);
    return *this;
  }

  /**
   * @brief Compares the content of two chunks to determine whether they are equal.
   * @param bc the second chunk the be compared.
   * @return true if the two chunks are equal, false otherwise.
   */
  inline bool operator == ( const ByteChunk& bc ) const
  { return ( _size == bc._size && memcmp(_chunk, bc._chunk, _size) == 0 ); }

  /**
   * @brief Compares the content of two chunks to determine wheter they are distinct.
   * @param bc the second chunk the be compared.
   * @return true if the two chunks are distinct, false otherwise.
   */
  inline bool operator != ( const ByteChunk& bc ) const
  { return ( _size != bc._size || memcmp(_chunk, bc._chunk, _size) != 0 ); }

  /**
   * @brief Compares the content of two chunks in lexicographic order.
   * @param bc the second chunk the be compared.
   * @return true if the chunk is lower than the second chunk, false otherwise.
   */
  inline bool operator < ( const ByteChunk& bc ) const
  { 
    int t = memcmp(_chunk, bc._chunk, std::min(_size, bc._size));
    return (t == 0 ? _size < bc._size : t < 0); 
  }

  /**
   * @brief Compares the content of two chunks in lexicographic order.
   * @param bc the second chunk the be compared.
   * @return true if the chunk is higher than the second chunk, false otherwise.
   */
  inline bool operator > ( const ByteChunk& bc ) const
  { 
    int t = memcmp(_chunk, bc._chunk, std::min(_size, bc._size));
    return (t == 0 ? _size > bc._size : t > 0); 
  }

  /**
   * @brief Access operator to a given position of the chunk.
   * @param p position.
   * @return reference to the byte in the position p of the chunk.
   */
  inline char& operator [] ( size_t p )
  { assert( _size > 0 && p < _size ); return _chunk[p]; }

  /**
   * @brief Access operator to a given position of the chunk.
   * @param p position.
   * @return value of the byte in the position p of the chunk.
   */
  inline char operator [] ( size_t p ) const
  { assert( _size > 0 && p < _size ); return _chunk[p]; }

  /**
   * @brief Returns the current size of the chunk.
   * @return size of the chunk.
   */
  inline size_t size ( ) const
  { return _size; }

  /** 
   * @brief Returns the capacity of the chunk. This is, the maximum size.
   * @return capacity of the chunk.
   */
  inline size_t capacity ( ) const
  { return _capacity; }

  /**
   * @brief Frees memory allocated.
   */
  inline void clear ( )
  { if ( _capacity > 0 ) { delete [] _chunk; _size = _capacity = 0; } }

  /**
   * @brief Changes the chunk size.
   *
   * If the new size is greater than the current capacity,
   * the capacity of the chunk is increased. 
   * The content of the new elements is unknown.
   *
   * @param n new size.
   */
  void resize ( size_t n )
  {
    _size = n;
    if ( n > _capacity ) {
      char * old_chunk = _chunk;
      _allocate(n);
      _copy(old_chunk, _capacity);
      memset(_chunk+_capacity, 0x00, n-_capacity);
      delete [] old_chunk;
    }
  }

  /**
   * @brief Reserves size for the chunk.
   * 
   * If the indicated space is lower or equal to the current capacity,
   * the size is changed to the minimum between the current size and the new
   * capacity, but the capacity is not modified in fact.
   *
   * @param n new capacity.
   */
  void reserve ( size_t n )
  {
    if ( n > _capacity ) {
      char * old_chunk = _chunk;
      _allocate(n);
      _copy(old_chunk, _capacity);
      delete [] old_chunk;
    } else _size = std::min(_size,n);
  }

  /**
   * @brief A new byte is added at the end of the chunk.
   *
   * If the chunk ran out of capacity, it is increased.
   * @param b byte to be added.
   */
  inline void push_back ( char b )
  {
    if ( _size == _capacity ) reserve(_capacity*2);
    _chunk[_size++] = b;
  }

  /**
   * @brief A chunk of bytes is added at the end of the chunk.
   * @param bc chunk to be added.
   */
  inline void append ( const ByteChunk& bc )
  {
    if ( _capacity-_size < bc._size ) reserve(bc._capacity);
    memcpy(_chunk+_size, bc._chunk, bc._size);
    _size += bc._size;
  }

  /**
   * @brief Returns the pointer to the chunk of bytes.
   * @return pointer to the chunk of bytes.
   */
  inline const char * pchar ( ) const
  { return _chunk; }

  /**
   * @brief Returns the last element in the chunk.
   * @return last byte.
   */
  inline char back() const
  { assert(_size > 0); return _chunk[_size-1]; }

  /**
   * @brief Returns the first element in the chunk.
   * @return first byte.
   */
  inline char front() const
  { assert(_size > 0); return _chunk[0]; }

  /**
   * @brief This method writes the content of a ByteChunk to a output stream.
   * Each element is written as an hexadecimal number and separated by an space symbol.
   * @param os output stream.
   * @param bc chunk to be written.
   * @return output stream.
   */
  friend std::ostream& operator << (std::ostream& os, const ByteChunk& bc)
  {
    os << std::hex;
    for(size_t i = 0; i < bc._size; ++i)
      os << bc._chunk[i] << " ";
    os << std::dec;
    return os;
  }
};

#ifdef __GXX_EXPERIMENTAL_CXX0X__
/**
 * @class ByteChunkHash
 * @brief This class computes the hash value of a chunk of bytes.
 */
struct ByteChunkHash {
  inline size_t operator()(const ByteChunk& x) const {
    size_t a = 1, b = 0;
    size_t index;
    for (index = 0; index != x.size(); ++index) {
      a = (a + x[index]) % 65521;
      b = (b + a) % 65521;
    }
    return (b << 16) | a;
  }
};
#endif

#endif
