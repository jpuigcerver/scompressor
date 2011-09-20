/**
 * @file ByteChunk.hpp
 * @brief Fitxer amb la definició de la classe ByteChunk.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date May 2011
 */

#ifndef __BYTECHUNK_HPP__
#define __BYTECHUNK_HPP__

#include <cstring>
#include <cassert>

/**
 * @class ByteChunk
 * @brief Classe que gestiona un bloc de bytes. Alternativa més
 * eficient i personalitzada a utilitzar un vector<char>.
 */
class ByteChunk {
  /** Bloc de bytes. */
  char * _chunk;
  /** Grandària actual del bloc de bytes. */
  size_t _size;
  /** Capacitat del bloc de bytes. */
  size_t _capacity;

  /**
   * @brief Reserva espai per al bloc de bytes.
   * @param capacity grandària del bloc de bytes (capacitat).
   */
  inline void _allocate ( size_t capacity )
  {
    if ( capacity > 0 )
      assert ( (_chunk = new char [(_capacity = capacity)]) != NULL );
  }

  /**
   * @brief Copia un segment de dades.
   * @param src punter a les dades a copiar.
   * @param size grandària del bloc de bytes a copiar.
   */
  inline void _copy ( const char * src, size_t size )
  { if ( size > 0 ) memcpy ( _chunk, src, size ); }

public:
  /**
   * @brief Constructor per defecte. Crea un bloc de bytes buit.
   */
  ByteChunk ( ) : _chunk(0), _size(0), _capacity(0) { }

  /**
   * @brief Constructor de còpia. 
   * @param bc bloc de bytes. 
   */
  ByteChunk ( const ByteChunk& bc ) : _chunk(0), _size(bc._size), _capacity(bc._size) 
  { if(_capacity > 0){ _allocate(_capacity); _copy(bc._chunk, bc._size); } }

  /**
   * @brief Constructor de còpia a partir d'una seqüència de bytes. 
   * @param chunk seqüència de bytes.
   * @param size grandària de la seqüència.
   */
  ByteChunk ( const char * chunk, size_t size ) : _chunk(0), _size(size), _capacity(size)
  { if(_capacity > 0){ _allocate(_capacity); _copy(chunk, _size); } }

  /**
   * @brief Constructor de còpia a partir d'un byte. Crea un nou bloc de bytes amb
   * contingut igual al del byte indicat.
   * @param byte byte a copiar.
   */
  ByteChunk ( const char byte ) : _chunk(0), _size(1), _capacity(_size)
  { _allocate(_capacity); _chunk[0] = byte; }

  /**
   * @brief Constructor per capacitat. Crea un nou bloc de bytes amb capacitat màxima 
   * la indicada.
   * @param capacity capacitat màxima del bloc de bytes.
   */
  ByteChunk ( const size_t capacity ) : _chunk(0), _size(0), _capacity(capacity)
  { if(_capacity > 0){ _allocate(_capacity); } }

  /**
   * @brief Destructor.
   */
  ~ByteChunk ( )
  { clear(); }

  /**
   * @brief Copia el contingut d'un bloc de bytes a un altre.
   * @param bc bloc de bytes a copiar.
   * @return aquest mètode torna *this.
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
   * @brief Compara el contingut del bloc amb un altre per veure si coincideixen.
   * @param bc bloc de bytes a comparar.
   * @return torna true si els dos blocs tenen la mateixa grandària i el seu
   * contingut és identic, o false en cas contrari.
   */
  inline bool operator == ( const ByteChunk& bc ) const
  { return ( _size == bc._size && memcmp(_chunk, bc._chunk, _size) == 0 ); }

  /**
   * @brief Compara el contingut del bloc amb un altre per veure si difereixen.
   * @param bc bloc de bytes a comparar.
   * @return torna true si els dos blocs tenen grandària diferent o si el seu
   * contingut és diferent. Torna false en cas contrari.
   */
  inline bool operator != ( const ByteChunk& bc ) const
  { return ( _size != bc._size || memcmp(_chunk, bc._chunk, _size) != 0 ); }

  /**
   * @brief Compara el contingut del bloc amb un altre en ordre lexicogràfic.
   * @param bc bloc de bytes a comparar.
   * @return torna true si el bloc és menor que el bloc a comparar en ordre
   * lexicogràfic, o false en cas contrari.
   */
  inline bool operator < ( const ByteChunk& bc ) const
  { 
    int t = memcmp(_chunk, bc._chunk, std::min(_size, bc._size));
    return (t == 0 ? _size < bc._size : t < 0); 
  }

  /**
   * @brief Compara el contingut del bloc amb un altre en ordre lexicogràfic.
   * @param bc bloc de bytes a comparar.
   * @return torna true si el bloc és major que el bloc a comparar en ordre
   * lexicogràfic, o false en cas contrari.
   */
  inline bool operator > ( const ByteChunk& bc ) const
  { 
    int t = memcmp(_chunk, bc._chunk, std::min(_size, bc._size));
    return (t == 0 ? _size > bc._size : t > 0); 
  }

  /**
   * @brief Accedeix a l'element d'una possició en el bloc de bytes.
   * @param p possició de l'element a accedir.
   * @return referència a l'element en la possició p.
   */
  inline char& operator [] ( size_t p )
  { assert( _size > 0 && p < _size ); return _chunk[p]; }

  /**
   * @brief Accedeix a l'element d'una possició en el bloc de bytes.
   * @param p possició de l'element a accedir.
   * @return valor de l'element en la possició p.
   */
  inline char operator [] ( size_t p ) const
  { assert( _size > 0 && p < _size ); return _chunk[p]; }

  /**
   * @brief Recupera la grandària del bloc de bytes.
   * @return grandària del bloc de bytes.
   */
  inline size_t size ( ) const
  { return _size; }

  /** 
   * @brief Recupera la capacitat del bloc de bytes, és a dir
   * quina és la grandària màxima d'aquest bloc.
   * @return capacitat del bloc de bytes.
   */
  inline size_t capacity ( ) const
  { return _capacity; }

  /**
   * @brief Allibera la memòria utilitzada pel bloc de bytes.
   */
  inline void clear ( )
  { if ( _capacity > 0 ) { delete [] _chunk; _size = _capacity = 0; } }

  /**
   * @brief Canvia la grandària del bloc. 
   *
   * Si la nova grandària és major que la capacitat actual, 
   * s'augmenta la capacitat del bloc. El contingut dels elements
   * nous és indeterminat.
   *
   * @param n nova grandària del bloc de bytes.
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
   * @brief Reserva espai per al bloc de dades.
   * 
   * Si l'espai a reservar és menor o igual que la capacitat actual es
   * canvia la grandària del bloc al mínim entre la grandària actual
   * i la nova capacitat, però no es modifica la capacitat real
   * del bloc.
   *
   * En canvi, si l'espai a reservar és major que la capacitat del bloc,
   * s'augmenta la capacitat del bloc a l'indicada.
   *
   * @param n nova capacitat del bloc de bytes.
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
   * @brief Copia un byte al final del bloc.
   *
   * Si el bloc no té més capacitat, s'augmenta aquesta al doble de l'actual.
   * @param b byte a copiar.
   */
  inline void push_back ( char b )
  {
    if ( _size == _capacity ) reserve(_capacity*2);
    _chunk[_size++] = b;
  }

  /**
   * @brief Afegeix un bloc de bytes al final del bloc.
   * @param bc bloc de bytes a copiar.
   */
  inline void append ( const ByteChunk& bc )
  {
    if ( _capacity-_size < bc._size ) reserve(bc._capacity);
    memcpy(_chunk+_size, bc._chunk, bc._size);
    _size += bc._size;
  }

  /**
   * @brief Recupera el punter al bloc de bytes.
   * @return punter al bloc de bytes.
   */
  inline const char * pchar ( ) const
  { return _chunk; }

  /**
   * @brief Recupera l'últim element del bloc de bytes.
   * @return últim byte.
   */
  inline char back() const
  { assert(_size > 0); return _chunk[_size-1]; }

  /**
   * @brief Recupera el primer element del bloc de bytes.
   * @return primer byte.
   */
  inline char front() const
  { assert(_size > 0); return _chunk[0]; }

  /**
   * @brief Volca el contingut (en hexadecimal) del en un fluxe d'eixida i 
   * separa cada byte del bloc per un espai.
   * @param os fluxe d'eixida.
   * @param bc bloc de bytes a volcar.
   * @return torna la referència al fluxe d'eixida.
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
