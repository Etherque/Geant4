#ifndef G4AnyType_h
#define G4AnyType_h 1

#include <algorithm>
#include <typeinfo>
#include <iostream>
#include <sstream>

#include "G4UIcommand.hh"

class G4String;
namespace CLHEP {
  class Hep3Vector;
}

/**
 * @class G4AnyType G4AnyType.hh
 * This class represents any data type. The class only holds a reference to the type and not the value.
 */
class  G4AnyType {  
public:
  /** Constructor */
  G4AnyType():
  fContent(0) {}
  
  /** Constructor */
  template <typename ValueType> G4AnyType(ValueType &value):
  fContent(new Ref<ValueType>(value)) {}
  
  /** Copy Constructor */
  G4AnyType(const G4AnyType &other):
  fContent(other.fContent ? other.fContent->Clone() : 0) {}
  
  /** Dtor */
  ~G4AnyType() {
    delete fContent;
  }
  
  /** bool operator */
  operator bool() {
    return !Empty();
  }
  /** Modifier */
  G4AnyType& Swap(G4AnyType& rhs) {
    std::swap(fContent, rhs.fContent);
    return *this;
  }
  /** Modifier */
  template <typename ValueType> G4AnyType& operator =(const ValueType& rhs) {
    G4AnyType(rhs).Swap(*this);
    return *this;
  }
  /** Modifier */
  G4AnyType& operator =(const G4AnyType& rhs) {
    G4AnyType(rhs).Swap(*this);
    return *this;
  }
  /** Query */
  bool Empty() const {
    return !fContent;
  }
  /** Query */
  const std::type_info& TypeInfo() const {
    return fContent ? fContent->TypeInfo() : typeid(void);
  }
  /** Adress */
  void* Address() const {
    return fContent ? fContent->Address() : 0;
  }
  /** String conversion */
  std::string ToString() const {
    return fContent->ToString();
  }
  /** String conversion */
  void FromString(const std::string& val) {
    fContent->FromString(val);
  }
private:
  /**
   * @class Placeholder G4AnyType.h G4AnyType.h
   */
  class Placeholder {
  public:
    /** Constructor */
    Placeholder() {}
    /** Destructor */
    virtual ~Placeholder() {}
    /** Query */
    virtual const std::type_info& TypeInfo() const = 0;
    /** Query */
    virtual Placeholder* Clone() const = 0;
    /** Query */
    virtual void* Address() const = 0;
    /** ToString */
    virtual std::string ToString() const = 0;
    /** FromString */
    virtual void FromString(const std::string& val) = 0;
  };
  
  template <typename ValueType> class Ref: public Placeholder {
  public:
    /** Constructor */
    Ref(ValueType& value): fRef(value) {}
    /** Query */
    virtual const std::type_info& TypeInfo() const {
      return typeid(ValueType);
    }
    /** Clone */
    virtual Placeholder* Clone() const {
      return new Ref(fRef);
    }
    /** Address */
    virtual void* Address() const {
      return (void*) (&fRef);
    }
    /** ToString */
    virtual std::string ToString() const {
      std::stringstream ss;
      ss << fRef;
      return ss.str();
    }
    /** FromString */
    virtual void FromString(const std::string& val) {
      std::stringstream ss(val);
      ss >> fRef;
    }
    /** representation */
    ValueType& fRef;
  };
  /** representation */
  template <typename ValueType> friend ValueType* any_cast(G4AnyType*);
  /** representation */
  Placeholder* fContent;
};

/**
 * Specializations
 */

template <> inline void G4AnyType::Ref<bool>::FromString(const std::string& val)  {
  fRef = G4UIcommand::ConvertToBool(val.c_str());
}

template <> inline void G4AnyType::Ref<G4String>::FromString(const std::string& val)  {
  if (val[0] == '"' ) fRef = val.substr(1,val.size()-2);
  else fRef = val;
}

template <> inline void G4AnyType::Ref<G4ThreeVector>::FromString(const std::string& val)  {
  fRef = G4UIcommand::ConvertTo3Vector(val.c_str());
}


/**
 * @class G4BadAnyCast G4AnyType.h Reflex/G4AnyType.h
 * @author K. Henney
 */
class G4BadAnyCast: public std::bad_cast {
public:
  /** Constructor */
  G4BadAnyCast() {}
  
  /** Query */
  virtual const char* what() const throw() {
    return "G4BadAnyCast: failed conversion using any_cast";
  }
};

/** value */
template <typename ValueType> ValueType* any_cast(G4AnyType* operand) {
  return operand && operand->TypeInfo() == typeid(ValueType)
  ? &static_cast<G4AnyType::Ref<ValueType>*>(operand->fContent)->fRef : 0;
}
/** value */
template <typename ValueType> const ValueType* any_cast(const G4AnyType* operand) {
  return any_cast<ValueType>(const_cast<G4AnyType*>(operand));
}
/** value */
template <typename ValueType> ValueType any_cast(const G4AnyType& operand) {
  const ValueType* result = any_cast<ValueType>(&operand);
  if (!result) {
    throw G4BadAnyCast();
  }
  return *result;
}

#endif
