/*
* dtEntity Game and Simulation Engine
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Martin Scheffler
*/

#include <UnitTest++.h>
#include <dtEntity/scriptaccessor.h>

using namespace UnitTest;
using namespace dtEntity;


class TestObj
   : public dtEntity::ScriptAccessor
{
public:
   
   dtEntity::Property* TestMethod(const PropertyArgs& args)
   {  
      int ret = args[0]->IntValue() + args[1]->IntValue();
      return new dtEntity::IntProperty(ret);
   }

   TestObj()
   {
      AddScriptedMethod("testMethod", dtEntity::ScriptMethodFunctor(this, &TestObj::TestMethod));
   }
};


TEST(DefineAndCallScript)
{
   
   dtEntity::IntProperty arg0(7);
   dtEntity::IntProperty arg1(8);
   dtEntity::PropertyArgs args;
   args.push_back(&arg0);
   args.push_back(&arg1);

   TestObj obj;
   dtEntity::Property* ret = obj.CallScriptedMethod("testMethod", args);
   CHECK_EQUAL(ret->IntValue(), 15);
   delete ret;
}
