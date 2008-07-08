#ifndef TRIPLE_HPP
#define TRIPLE_HPP

template<class A, class B, class C> 
struct Triple {
  Triple() : first(A()), second(B()), third(C()) {}
  Triple(const A& a, const B& b, const C& c) : first(a), second(b), third(c) {} 
  
  template<class U1, class U2, class U3>
  Triple(const Triple<U1,U2,U3>& t) : first(t.first), second(t.second), third(t.third) {}
  
  bool operator==(const Triple&);
  void operator=(const Triple&);

  A first;
  B second;
  C third;
};

template<class A, class B, class C>
inline Triple<A, B, C> make_triple(const A &a, const B &b, const C &c) {
  return Triple<A,B,C>(a, b, c);
}

template<class A, class B, class C> 
bool Triple<A,B,C>::operator==(const Triple<A,B,C>& t) {
  return first == t.first && second == t.second && third == t.third;
}

template<class A, class B, class C> 
void Triple<A,B,C>::operator=(const Triple<A,B,C>& t) {
  first = t.first;
  second = t.second;
  third = t.third;
}


#endif // TRIPLE_HPP
