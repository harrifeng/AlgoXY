#ifndef _PATRICIA_
#define _PATRICIA_

#include <iostream>
#include <map>
#include <string>
#include <iterator> //for std::ostream_iterator
#include <functional> //for std::ptr_fun
#include <numeric> //for std::accumulate
#include "trieutil.hpp"

template<class Key, class Value>
struct Patricia{
  typedef Patricia<Key, Value> Self;
  typedef std::map<Key, Self*> Children;
  typedef Key   KeyType;
  typedef Value ValueType;

  Patricia(Value v=Value()):value(v){}

  virtual ~Patricia(){
    for(typename Children::iterator it=children.begin();
        it!=children.end(); ++it)
      delete it->second;
  }

  Value value;
  Children children;
};

//lcp, extract the longest common prefix,
// it will modify the parameters
template<class K>
K lcp(K& s1, K& s2){
  typename K::iterator it1(s1.begin()), it2(s2.begin());
  for(; it1!=s1.end() && it2!=s2.end() && *it1 == *it2; ++it1, ++it2);
  K res(s1.begin(), it1);
  s1 = K(it1, s1.end());
  s2 = K(it2, s2.end());
  return res;
}

//branch
template<class T>
T* branch(typename T::KeyType k1, T* t1, 
          typename T::KeyType k2, T* t2){
  if(k1.empty()){ //e.g. insert "an" into "another"
    t1->children[k2] = t2;
    return t1;
  }
  T* t = new T();
  t->children[k1] = t1;
  t->children[k2] = t2;
  return t;
}

template<class K, class V>
Patricia<K, V>* insert(Patricia<K, V>* t, K key, V value=V()){
  if(!t)
    t = new Patricia<K, V>();

  Patricia<K, V>* p = t;
  typedef typename Patricia<K, V>::Children::iterator Iterator;
  for(;;){
    bool match(false);
    for(Iterator it = p->children.begin(); it!=p->children.end(); ++it){
      K k=it->first;
      if(key == k){
        p->value = value; //overwrite
        return t;
      }
      K prefix = lcp(key, k);
      if(!prefix.empty()){
        match=true;
        if(k.empty()){ //e.g. insert "another" into "an"
          p = it->second;
          break;
        }
        else{
          p->children[prefix] = branch(key, new Patricia<K, V>(value), 
                                       k, it->second);
          p->children.erase(it);
          return t;
        }
      }
    }
    if(!match){
      p->children[key] = new Patricia<K, V>(value);
      break;
    }
  }
  return t;
}

class PatriciaTest{
public:
  void run(){
    std::cout<<"\ntest alphabetic patrica\n";
    test_insert();
    test_lookup();
  }
private:
  template<class Iterator>
  void test_list_to_patricia(Iterator first, Iterator last){
    typedef Patricia<std::string, std::string> PatriciaType;
    PatriciaType* t(0);
    t = std::accumulate(first, last, t, 
			std::ptr_fun(insert_key<PatriciaType, std::string>));
    std::copy(first, last, 
	      std::ostream_iterator<std::string>(std::cout, ", "));
    std::cout<<"\n==>"<<trie_to_str(t)<<"\n";
    delete t;
  }

  void test_insert(){
    const char* lst1[] = {"a", "an", "another", "b", "bob", "bool", "home"};
    test_list_to_patricia(lst1, lst1+sizeof(lst1)/sizeof(int));

    /*t=0;
    const char* keys[] = {"001", "100", "101"};
    const char* vals[] = {"y", "x", "z"};
    for(unsigned int i=0; i<sizeof(keys)/sizeof(char*); ++i)
      t = insert(t, std::string(keys[i]), std::string(vals[i]));
    std::copy(keys, keys+sizeof(keys)/sizeof(char*),
	      std::ostream_iterator<std::string>(std::cout, ", "));
    std::cout<<"==>"<<trie_to_str(t)<<"\n";
    delete t;*/
  }

  void test_lookup(){
  }
};
#endif //_PATRICIA