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
#include <dtEntity/property.h>
#include <dtEntity/propertycontainer.h>

using namespace UnitTest;
using namespace dtEntity;


class MyPropertyContainer : public PropertyContainer
{
public:
   MyPropertyContainer()
   {
      Register(SID("mIntProp"), &mIntProp);
      Register(SID("mStringIdProp"), &mStringIdProp);
      Register(SID("mFloatProp"), &mFloatProp);
      Register(SID("mBoolProp"), &mBoolProp);
      Register(SID("mDoubleProp"), &mDoubleProp);
      Register(SID("mStringProp"), &mStringProp);
   }
private:
   IntProperty mIntProp;
   StringIdProperty mStringIdProp;
   FloatProperty mFloatProp;
   BoolProperty mBoolProp;
   DoubleProperty mDoubleProp;
   StringProperty mStringProp;
};



TEST(PropertyContainerInt)
{
   #if BOOST_MSVC
      _CrtSetDbgFlag(0);
   #endif
   
   MyPropertyContainer container;   
   int v = 666;
   StringId sid = SID("mIntProp");
   container.SetInt(sid, v);
   CHECK_EQUAL(container.GetInt(sid), v);
}


TEST(PropertyContainerStringId)
{
   
   MyPropertyContainer container;   
   StringId v = SID("BLABLA");
   StringId sid = SID("mStringIdProp");
   container.SetStringId(sid, v);
   CHECK_EQUAL(container.GetStringId(sid), v);
}


TEST(PropertyContainerFloat)
{
   
   MyPropertyContainer container;   
   bool v = true;
   StringId sid = SID("mBoolProp");
   container.SetBool(sid, v);
   CHECK_EQUAL(container.GetBool(sid), v);
}

TEST(PropertyContainerDouble)
{
   MyPropertyContainer container;   
   double v = 1.234;
   StringId sid = SID("mDoubleProp");
   container.SetDouble(sid, v);
   CHECK_EQUAL(container.GetDouble(sid), v);
}

TEST(PropertyContainerString)
{
   
   MyPropertyContainer container;   
   std::string v = "Test";
   StringId sid = SID("mStringProp");
   container.SetString(sid, v);
   CHECK_EQUAL(container.GetString(sid), v);
}
