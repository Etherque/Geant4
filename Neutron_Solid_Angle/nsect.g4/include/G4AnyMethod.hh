#ifndef G4AnyMethod_h
#define G4AnyMethod_h 1


#include <functional>

/** Bad Argument exception */
class G4BadArgument: public std::bad_cast {
public:
  G4BadArgument() {}
  virtual const char* what() const throw() {
    return "G4BadArgument: failed operator()";
  }
};

template<typename T> struct remove_reference {typedef T type;};
template<typename T> struct remove_reference<T&> {typedef T type;};
template<typename T> struct remove_reference<const T&> {typedef T type;};

/**
 * @class G4AnyMethod G4AnyMothod.hh
 * This class represents any object method. The class only holds a member pointer.
 */
class G4AnyMethod {
public:
  /** contructor */
  G4AnyMethod(): fContent(0), narg(0) {}
  template <class S, class T> G4AnyMethod(S (T::*f)()) : narg(0) {
    fContent = new FuncRef<S,T>(f);
  }
  template <class S, class T, class A0> G4AnyMethod(S (T::*f)(A0)) : narg(1) {
    fContent = new FuncRef1<S,T,A0>(f);
  }
  template <class S, class T, class A0, class A1> G4AnyMethod(S (T::*f)(A0,A1)) : narg(2) {
    fContent = new FuncRef2<S,T,A0,A1>(f);
  }
  G4AnyMethod(const G4AnyMethod &other):
  fContent(other.fContent ? other.fContent->Clone() : 0),narg(other.narg) {}
  /** destructor */
  ~G4AnyMethod() {
    delete fContent;
  }
  
  G4AnyMethod& Swap(G4AnyMethod& rhs) {
    std::swap(fContent, rhs.fContent);
    std::swap(narg, rhs.narg);
    return *this;
  }
  /** Asignment operator */
  template <class S, class T>  G4AnyMethod& operator =(S (T::*f)()) {
    G4AnyMethod(f).Swap(*this);
    narg = 0;
    return *this;
  }
  template <class S, class T, class A0> G4AnyMethod& operator =(S (T::*f)(A0)) {
    G4AnyMethod(f).Swap(*this);
    narg = 1;
    return *this;
  }
  template <class S, class T, class A0, class A1> G4AnyMethod& operator =(S (T::*f)(A0, A1)) {
    G4AnyMethod(f).Swap(*this);
    narg = 1;
    return *this;
  }
  /** Asigment operator */
  G4AnyMethod& operator =(const G4AnyMethod& rhs) {
    G4AnyMethod(rhs).Swap(*this);
    narg = rhs.narg;
    return *this;
  }
  /** Query */
  bool Empty() const {
    return !fContent;
  }
  /** call operator */
  void operator()(void* obj) {
    fContent->operator()(obj);
  }
  void operator()(void* obj, const std::string& a0) {
    fContent->operator()(obj, a0);
  }
  /** Number of arguments */
  size_t NArg() const { return narg; }
  
  const std::type_info& ArgType(size_t n = 0) const {
    return fContent ? fContent->ArgType(n) : typeid(void);
  }
  
private:
  class Placeholder {
  public:
    Placeholder() {}
    virtual ~Placeholder() {}
    virtual Placeholder* Clone() const = 0;
    virtual void operator()(void*) = 0;
    virtual void operator()(void*,const std::string&) = 0;
    virtual const std::type_info& ArgType(size_t) const = 0;
  };
  
  template <class S, class T> struct FuncRef: public Placeholder {
    FuncRef(S (T::*f)()) : fRef(f) {}
    
    virtual void operator()(void* obj) {
      ((T*)obj->*fRef)();
    }
    virtual void operator()(void*, const std::string&) {
      throw G4BadArgument();
    }
    virtual Placeholder* Clone() const {
      return new FuncRef(fRef);
    }
    virtual const std::type_info& ArgType(size_t) const {
      return typeid(void);
    }
    S (T::*fRef)();
  };
  
  template <class S, class T, class A0> struct FuncRef1: public Placeholder {
    typedef typename remove_reference<A0>::type  nakedA0;
    
    FuncRef1(S (T::*f)(A0)) : fRef(f) {}
    
    virtual void operator()(void*) {
      throw G4BadArgument();
    }
    virtual void operator()(void* obj, const std::string& s0) {
      nakedA0 a0;
      std::stringstream strs(s0);
      strs >> a0;
      ((T*)obj->*fRef)(a0);
    }
    virtual Placeholder* Clone() const {
      return new FuncRef1(fRef);
    }
    virtual const std::type_info& ArgType(size_t) const {
      return typeid(A0);
    }
    S (T::*fRef)(A0);
  };

  template <class S, class T, class A0, class A1> struct FuncRef2: public Placeholder {
    typedef typename remove_reference<A0>::type  nakedA0;
    typedef typename remove_reference<A1>::type  nakedA1;
    
    FuncRef2(S (T::*f)(A0, A1)) : fRef(f) {}
    
    virtual void operator()(void*) {
      throw G4BadArgument();
    }
    virtual void operator()(void* obj, const std::string& s0) {
      nakedA0 a0;
      nakedA1 a1;
      std::stringstream strs(s0);
      strs >> a0 >> a1;
      ((T*)obj->*fRef)(a0, a1);
    }
    virtual Placeholder* Clone() const {
      return new FuncRef2(fRef);
    }
    virtual const std::type_info& ArgType(size_t i) const {
      return i == 0 ? typeid(A0) : typeid(A1);
    }
    S (T::*fRef)(A0, A1);
  };
  
  Placeholder* fContent;
  size_t narg;
};




#endif
