/**
 * @file HuffmanTree.hpp
 * @brief Fitxer amb la implementació de la classe HuffmanTree.
 * @author Joan Puigcerver Pérez <joapuipe@inf.upv.es>
 * @date April 2011
 */

#ifndef __HUFFMANTREE_HPP__
#define __HUFFMANTREE_HPP__

#include <queue>
#include <stack>
#include <vector>
#include <iostream>

#include <cassert>

#include <NullSource.hpp>
#include <Codification.hpp>
#include <Bit.hpp>
#include <BitStreamWriter.hpp>
#include <BitStreamReader.hpp>

/**
 * @class HuffmanTree
 * @brief Implementació d'un arbre de Huffman utilitzat en la codificació
 * i descodificació de Huffman.
 *
 * L'arbre es construeix a partir d'una font de memòria nula.
 * Una vegada construit l'abre, pot determinar-se el símbol corresponent a una
 * seqüència de bits que seria la seva codificació recorrent l'arbre des de l'arrel
 * fins al node que codifica el símbol (un dels nodes fulla). 
 *
 * Per a això, s'utilitzen els mètodes addToCurrentPath(), currentNodeIsLeaf(), 
 * getCurrentSymbol() i resetCurrentNode(). Amb aquests mètodes pot anar construint-se
 * iterativament el camí (bit a bit) fins a arribar a un node fulla i llavors obtenir
 * el símbol codificat amb el codi binari que ha format el camí.
 *
 * @see HuffmanCompressor
 * @see NullSource
 */
class HuffmanTree {
private:
  /**
   * @class HNode
   * @brief Implementa un node de l'arbre de Huffman.
   * @see HSymbNode
   */
  class HNode {
  public:
    /** Pes del node. Utilitzat en la codificació. */
    size_t weight;
    /** Fill esquerre. */
    HNode * lchild;
    /** Fill dret. */
    HNode * rchild;

    /** 
     * @brief Constructor.
     * @param weight pes del node.
     * @param left fill esquerre.
     * @param right fill dret.
     */
    HNode(size_t weight, HNode * left, HNode * right) 
      : weight(weight), lchild(left), rchild(right)			       
    { }

    /**
     * @brief Destructor.
     */
    ~HNode()
    { 
      if(lchild != NULL) delete lchild;
      if(rchild != NULL) delete rchild;
    }

    /**
     * @brief Indica si el node és una fulla (node terminal) o no.
     * @return torna true si és una fulla i false si no ho és.
     */
    virtual bool isLeaf(void) const
    { return false; }
  };

  /**
   * @class HSymbNode
   * @brief Implementa un node terminal de l'arbre de Huffman.
   * @see HNode
   */  
  class HSymbNode : public HNode {
  public: 
    /** Símbol codificat en el node. */
    char symbol;
    
    /**
     * @brief Constructor.
     * @param symb símbol que codifica el node.
     * @param weight pes del símbol (nombre d'aparicions en l'entrada).
     */
    HSymbNode(const char& symb, size_t weight)
      : HNode(weight, NULL, NULL), symbol(symb)
    { }
    
    virtual bool isLeaf(void) const
    { return true; }
  };
  
  /** 
   * @class HNodePComparator
   * @brief Classe utilitzada per a comparar els pesos de dos nodes de l'arbre
   * a partir dels seus punters.
   * @param a,b nodes a comparar.
   * @return true si a < b, false en cas contrari.
   */
  class HNodePComparator {
  public:
    /** 
     * @brief Determina si el pes del node a és major que el del node b. 
     * @param a,b nodes a comparar.
     * @return torna true si el pes de a és major que el de b, false en cas contrari.
     */
    bool operator () (const HNode * a, const HNode * b) const
    {
      assert(a != NULL);
      assert(b != NULL);
      return a->weight > b->weight;
    }
  };
  
  /** Arrel de l'arbre. */
  HNode * root;
  /** Node actual en el camí binari que parteix des de l'arrel. */
  const HNode * curr_node; 
public:
  /**
   * @brief Constructor per defecte. 
   */
  HuffmanTree()
    : root(NULL), curr_node(NULL)
  { }

  /**
   * @brief Constructor.
   * Construeix un arbre de Huffman a partir d'una 
   * font de memòria nula.
   * @param source font de memòria nula.
   * 
   * La construcció es fa en temps O(n log n).
   * @see buildTree()
   */
  HuffmanTree(const NullSource & source) 
    : root(NULL), curr_node(NULL)
  {
    buildTree(source);
  }

  /** 
   * @brief Destructor.
   */
  ~HuffmanTree()
  {
    clear();
  }

  /**
   * @brief Allibera la memòria utilitzada
   * per l'arbre. 
   */
  void clear(void)
  {
    if(root != NULL) delete root;
  }
  
  /**
   * @brief Construeix un arbre de Huffman a partir d'una 
   * font de memòria nula.
   *
   * La construcció es fa en temps O(n log n). 
   * @param source font de memòria nula.
   */
  void buildTree(const NullSource & source)
  {
    /* Cua de prioritats utilitzada per a construir un arbre de Huffman. */
    std::priority_queue<HNode*, std::vector<HNode*>, HNodePComparator> nodes;

    typedef NullSource::const_iterator NSconst_iterator;
    
    /* Destruim l'arbre anterior, en cas d'haver-ne. */
    clear();

    /* Si no hi ha símbols a codificar, acabem. */
    if (source.size() == 0) return;
    
    /* Posem en la cua de prioritats els símbols de la font nula amb les
       seves freqüències com a pes. */
    for(NSconst_iterator it = source.begin(); it != source.end(); ++it) {
      nodes.push( new HSymbNode(it->first, it->second) );
    }
    
    /* Mentre queden nodes a unir en la cua de prioritats... */
    while ( nodes.size() > 1 ) {
      /* Obtenim els dos nodes amb major prioritat... */
      HNode * a = nodes.top(); nodes.pop();
      HNode * b = nodes.top(); nodes.pop();
      /* i afegit un nou node com a pare dels dos anteriors
	 i prioritat igual a la suma dels pesos dels nodes anteriors. */
      nodes.push(new HNode(a->weight+b->weight, a, b) );
    }
    
    /* Quan sols queda un node, aquest és l'arrel. */
    root = nodes.top();
    curr_node = root;
  }

  /** 
   * @brief Obté una codificació de Huffman per a la font nula que ha
   * generat l'arbre actual.
   * @return codificació de Huffman per a l'arbre actual.
   */
  Codification<char, Bit> getCodification(void) const
  {
    Codification<char, Bit> codif;

    /* Si no hi ha arbre, avortem. */
    if ( root == NULL ) 
      return codif;

    /* Si l'arrel és fulla (codifica un símbol),
       la seva codificació serà el bit '1' (arbitràiament). */
    if( root->isLeaf() ) {
      std::vector<Bit> path(1);
      codif[((const HSymbNode*)root)->symbol] = path; 
      return codif;
    }

    typedef std::pair< const HNode *, std::vector<Bit> > SPair;

    /* Recorreguem en profunditat tot l'arbre per recuperar 
       la codificació dels símbols. 
       Guardarem per a cada node actiu, el camí fins a ell (codi). */
    std::stack<SPair> active_nodes;
    active_nodes.push( SPair(root, std::vector<Bit>()) ) ;
    
    /* Mentre queden símbols per recórrer... */
    while( !active_nodes.empty() ) {
      SPair n = active_nodes.top();
      active_nodes.pop();

      /* Codi per al node. */
      std::vector<Bit> path = n.second;
      
      /* Si té fill esquerre... */
      if(n.first->lchild != NULL) {
	/* Afegim un '0' al camí. */
	path.push_back(0);
	/* Afegim el fill amb el seu camí a la pila 
	   de nodes actius. */
	active_nodes.push ( SPair(n.first->lchild, path) );
	path.pop_back();
      }
      
      /* Si té fill dret... */
      if(n.first->rchild != NULL) {
	/* Afegim un '1' al camí. */
	path.push_back(1);
	/* Afegim el fill amb el seu camí a la pila 
	   de nodes actius. */
	active_nodes.push ( SPair(n.first->rchild, path) );
	path.pop_back();
      }

      /* Si el símbol és una fulla, hem recuperat la codificació
	 del símbol d'aquesta. */
      if( n.first->isLeaf() )
	codif[((const HSymbNode*)n.first)->symbol] = path;
    }

    return codif;
  }

  /**
   * @brief Obté la longitud mitjana del codi dels símbols
   * tenint en compte la seva freqüència d'aparició.
   * @param reference_num_symbols nombre de símbols de referència
   * (ha de ser la suma dels pesos de tots els símbols en l'arbre).
   * @return longitud mitjana.
   */
  double getMedianLength(size_t reference_num_symbols) const
  {
    if ( root == NULL ) 
      return 0.0;

    if( root->isLeaf() ) {
      return 1.0;
    }
    
    typedef std::pair< const HNode *, size_t > SPair;
    
    std::stack<SPair> active_nodes;
    active_nodes.push( SPair(root, 0) ) ;
    
    double med_length = 0.0;
    
    while( !active_nodes.empty() ) {
      SPair n = active_nodes.top();
      active_nodes.pop();
      
      if(n.first->lchild != NULL)
	active_nodes.push ( SPair(n.first->lchild, n.second+1) );

      if(n.first->rchild != NULL)
	active_nodes.push ( SPair(n.first->rchild, n.second+1) );

      if( n.first->isLeaf() )
	med_length += (double)n.first->weight/(double)reference_num_symbols * n.second;
    }
    
    return med_length;    
  }

  /**
   * @brief Escriu l'arbre de Huffman en un fluxe d'eixida utilitzant un
   * escriptor de fluxe binari. 
   *
   * Utilitza l'algorisme de serialització vist en classe.
   * @param output escriptor de fluxe d'eixida binari.
   * @return true si tot ha anat bé, false en cas d'error.
   */
  bool serializeTree(BitStreamWriter& output) const
  { 
    if( root == NULL ) return true;
    
    std::stack<const HNode*> active_nodes;
    active_nodes.push(root);
    while ( !active_nodes.empty() ) {
      const HNode * n = active_nodes.top();
      active_nodes.pop();
      
      if( !n->isLeaf() ) {
	if( !output.put(0).good() ) return false;
	if( n->lchild != NULL ) active_nodes.push(n->lchild);
	if( n->rchild != NULL ) active_nodes.push(n->rchild);
      } else {
	if( !output.put(1).good() ) return false;
	if( !output.put(((const HSymbNode*)n)->symbol, 8).good() ) return false;
      }
    }

    return true;
  }

  /**
   * @brief Llegeix l'arbre de Huffman des d'un fluxe d'entrada utilitzant un
   * lector de fluxe binari.
   *
   * Utilitza l'algorisme de deserialització vist en classe.
   * @param input lector de fluxe d'entrada binari.
   * @return true si tot ha anat bé, false en cas d'error.
   */
  bool deserializeTree(BitStreamReader& input) {
    clear();

    root = new HNode(0, NULL, NULL);
    std::stack<HNode **> active_nodes;
    active_nodes.push(&root);

    while( !input.eof() && !active_nodes.empty() ) {
      HNode** pnode = active_nodes.top();
      active_nodes.pop();
      
      Bit b = input.get();
      if ( b == 1 ) {
	char symb = input.get(8);
	*pnode = new HSymbNode(symb, 0);
      } else {
	*pnode = new HNode(0, NULL, NULL);
	active_nodes.push( &(*pnode)->lchild );
	active_nodes.push( &(*pnode)->rchild );
      }
    }
    curr_node = root;
    return active_nodes.empty();
  }

  /**
   * @brief Afegeix un bit al camí actual que parteix de l'arrel de l'arbre.
   *
   * Una seqüència de bits construeixen un camí en l'arbre que parteix des del
   * camí i arriba a una de les fulles (on hi ha codificat un símbol). Aquest
   * camí és la codificació del símbol. Si el bit és 0, es desplaça cam al fill
   * esquerre i si és 1 cap al de la dreta.
   * @param b bit a afegir en el camí.
   * @return torna true si no hi ha hagut cap error, o false en cas contrari,
   * és a dir, no hi ha camí en l'abre amb aquesta seqüència de bits.
   */
  bool addToCurrentPath(Bit b) 
  {
    if ( b == 0 && curr_node->lchild != NULL )
      curr_node = curr_node->lchild;
    else if ( curr_node->rchild != NULL )
      curr_node = curr_node->rchild;
    else return false;
    return true;
  }

  /**
   * @brief Determina si l'últim node del camí és fulla o no (té un símbol codificat o no).
   * @return true si es fulla, false en cas contrari.
   */
  bool currentNodeIsLeaf(void) const 
  {
    if ( curr_node != NULL && curr_node->isLeaf() ) return true;
    else return false;
  }

  /**
   * @brief Torna el símbol codificat amb el codi binari corresponent al camí
   * actual de en l'abre. Sols deuria utilitzar-se després d'haver comprovat que
   * el node actual és una fulla.
   * @return símbol codificat.
   */
  char getCurrentSymbol(void) const
  {
    return ((const HSymbNode*)curr_node)->symbol;
  }

  /**
   * @brief Reinicia el camí en l'abre, tornant a l'arrel.
   */
  void resetCurrentNode(void)
  {
    curr_node = root;
  }

  
};

#endif

/**
 * @example HuffmanCodificationExample.cpp
 * @brief Aquest exemple mostra com utilitzar la classe HuffmanTree
 * per a obtenir la codificació de Huffman de cada símbol d'un fitxer.
 */
