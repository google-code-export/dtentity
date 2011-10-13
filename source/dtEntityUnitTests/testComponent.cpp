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
#include <dtEntity/policycomponent.h>
#include <dtEntity/property.h>
#include <osg/Vec2>

using namespace UnitTest;
using namespace dtEntity;

struct MyComponent : public PolicyComponent<CallbackPolicyVector>
{
   StringProperty mStringProp;

   MyComponent()
   {
      Register(SID("testprop"), &mStringProp);
   }

   virtual ComponentType GetType() const { return StringId(); }
};

//typedef fastdelegate::FastDelegate3<const ComponentBase*, StringId, const Property*, void> PropertyChangedFunctor;

static StringId s_propname = StringId();

struct TestClass
{
   static void TestFunc(const Component*, StringId propname, const Property*)
   {
      s_propname = propname;
   }
};

TEST(Callbacks)
{
   s_propname = StringId();
   PropertyChangedFunctor ftr(TestClass::TestFunc);
   MyComponent comp;
   comp.RegisterForPropertyChanged(SID("testprop"), ftr);
   comp.OnPropertyChanged(SID("testprop"), comp.mStringProp);
   CHECK_EQUAL(SID("testprop"), s_propname);
}

TEST(Callbacks_Unregister)
{
   s_propname = StringId();
   PropertyChangedFunctor ftr(TestClass::TestFunc);
   MyComponent comp;
   comp.RegisterForPropertyChanged(SID("testprop"), ftr);
   comp.UnregisterForPropertyChanged(SID("testprop"), ftr);
   comp.OnPropertyChanged(SID("testprop"), comp.mStringProp);
   CHECK_EQUAL(StringId(), s_propname);
}

