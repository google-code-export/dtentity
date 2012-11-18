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
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntity/core.h>
#include <dtEntityOSG/osginputinterface.h>
using namespace dtEntityWrappers;
using namespace dtEntity;
using namespace v8;

struct ScriptFixture
{
   ScriptFixture()
   {
      mScriptSystem = new ScriptSystem(mEntityManager);
      
      //dtEntity::SetSystemInterface(new dtEntityOSG::OSGSystemInterface(mEntityManager->GetMessagePump()));
      //dtEntity::SetWindowInterface(new dtEntityOSG::OSGWindowInterface(*mEntityManager));
      dtEntity::SetInputInterface(new dtEntityOSG::OSGInputInterface(mEntityManager.GetMessagePump()));
      mEntityManager.AddEntitySystem(*mScriptSystem);
   }

   ~ScriptFixture()
   {
   }

   EntityManager mEntityManager;
   ScriptSystem* mScriptSystem;
};

TEST(ExecScript)
{
   ScriptFixture f;
   std::string code = "3 + 4";
   HandleScope scope;
   Handle<Value> val = f.mScriptSystem->ExecuteJS(code);
   CHECK_EQUAL(val->Int32Value(), 7);
}

TEST(ScriptToString)
{
   ScriptFixture f;
   std::string code = "\"Test2: \" + Math.sin(0)";
   HandleScope scope;
   Handle<Value> val = f.mScriptSystem->ExecuteJS(code);
   std::string str = ToStdString(val);
   CHECK_EQUAL(str, "Test2: 0");
}

TEST(Println)
{
   ScriptFixture f;
   std::string code = "println(\"Test2: \" + Math.sin(0));";
   HandleScope scope;
   f.mScriptSystem->ExecuteFile("Scripts/test.js");

}
