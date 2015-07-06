#include <lua.hpp>
#include "LuaIntf/LuaIntf.h"

/*
 * Class declaration
 */
class Base
{
  public:
  virtual ~Base() { }
};

class Derived : public Base
{
  public:
    int a;
};

/*
 * Global variables
 */
Base *b;
std::shared_ptr<Base> sp;

/*
 * Global accessor functions
 */
Base *getBase()
{
  return b;
}

std::shared_ptr<Base> getSP()
{
  return sp;
}

/*
 * Downcast function
 */

namespace LuaIntf
{
  LUA_USING_SHARED_PTR_TYPE(std::shared_ptr);
  
  template <>
  struct LuaAutomaticDowncast<Base>
  {
	static void *getClassID(Base *b, bool is_const)
	{
	  if (dynamic_cast<Derived*>(b) != nullptr)
	  {
	    return is_const?CppConstSignature<Derived>::value():CppClassSignature<Derived>::value();
	  }
	  else
	  {
	    return is_const?CppConstSignature<Base>::value():CppClassSignature<Base>::value();
	  }
	}
  };
}


int main(int argc, char *argv[])
{
  using namespace LuaIntf;
  LuaContext L;

  LuaBinding(L)
  .beginClass<Base>("Base")
  .endClass()
  
  .beginExtendClass<Derived, Base>("Derived")
  .addVariable("a", &Derived::a)
  .endClass()
  
  .beginModule("globals")
  .addVariableRef("b", &b)
  .addVariableRef("sp", &sp)
  
  .addFunction("getBase", &getBase)
  .addFunction("getSP", &getSP)
  .endModule();
  
  b = new Derived;
  static_cast<Derived*>(b)->a = 5;
  sp.reset(new Derived);
  static_cast<Derived&>(*sp).a = 10;
  
  // Can access the field a in all cases
  Lua::exec(L, "print (globals.b.a)");
  Lua::exec(L, "print (globals.sp.a)");
  Lua::exec(L, "print (globals.getBase().a)");
  Lua::exec(L, "print (globals.getSP().a)");
  
}