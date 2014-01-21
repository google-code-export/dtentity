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

#include <dtEntityWrappers/debugdrawmanagerwrapper.h>

#include <dtEntity/core.h>
#include <dtEntity/entitymanager.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntity/debugdrawinterface.h>
#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace v8;

namespace dtEntityWrappers
{

#if 0

   dtEntity::StringId s_debugDrawWrapper = dtEntity::SID("DebugDrawWrapper");

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerToString(const Arguments& args)
   {
      return String::NewFromUtf8(v8::Isolate::GetCurrent(), "<DebugDrawInterface>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerSetEnabled(const Arguments& args)
   {
      dtEntity::GetDebugDrawInterface()->SetEnabled(args[0]->BooleanValue());
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerIsEnabled(const Arguments& args)
   {      
      return Boolean::New(dtEntity::GetDebugDrawInterface()->IsEnabled());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerAddLine(const Arguments& args)
   {
      dtEntity::DebugDrawInterface* ddm = dtEntity::GetDebugDrawInterface();

      if(!ddm->IsEnabled() )
      {
         return Undefined();
      }

      if(args.Length() < 2 || !IsVec3(args[0]) || !IsVec3(args[1]))
      {
         return ThrowError("usage: addLine(Vec3 start, Vec3 end, [Vec4 color, Int lineWidth, Number duration, bool useDepthTest])");
      }

      dtEntity::Vec3f start = UnwrapVec3(args[0]);
      dtEntity::Vec3f end = UnwrapVec3(args[1]);

      dtEntity::Vec4f color(1,0,0,1);
      if(args.Length() > 2 && IsVec4(args[2]))
      {
         color = UnwrapVec4(args[2]);
      }

      int linewidth = 1;
      if(args.Length() > 3)
      {
         linewidth = args[3]->Int32Value();
         if(linewidth == 0) 
         {
            linewidth = 1;
         }
      }

      float duration = 0;
      if(args.Length() > 4)
      {
         duration = args[4]->NumberValue();
      }

      bool depth = true;
      if(args.Length() > 5)
      {
         depth = args[5]->BooleanValue();
      }

      ddm->AddLine(start, end, color, linewidth, duration, depth);
      return Undefined();
   }


   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerAddLines(const Arguments& args)
   {
      dtEntity::DebugDrawInterface* ddm = dtEntity::GetDebugDrawInterface();

      if(!ddm->IsEnabled() )
      {
         return Undefined();
      }

      if(args.Length() < 1 || !args[0]->IsArray())
      {
         return ThrowError("usage: addLines(Array(Vec3) lines,[Vec4 color, Int lineWidth, Number duration, bool useDepthTest])");
      }

      std::vector<dtEntity::Vec3f> lines;
      HandleScope scope(Isolate::GetCurrent());
      Handle<Array> arr = Handle<Array>::Cast(args[0]);
      
      unsigned int l = arr->Length();
      for(unsigned int i = 0; i < l; ++i)
      {
         lines.push_back(UnwrapVec3(arr->Get(i)));
      }
      
      dtEntity::Vec4f color(1,0,0,1);
      if(args.Length() > 1 && IsVec4(args[1]))
      {
         color = UnwrapVec4(args[1]);
      }

      int linewidth = 1;
      if(args.Length() > 2)
      {
         linewidth = args[2]->Int32Value();
         if(linewidth == 0) 
         {
            linewidth = 1;
         }
      }

      float duration = 0;
      if(args.Length() > 3)
      {
         duration = args[3]->NumberValue();
      }

      bool depth = true;
      if(args.Length() > 4)
      {
         depth = args[4]->BooleanValue();
      }

      ddm->AddLines(lines, color, linewidth, duration, depth);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerAddAABB(const Arguments& args)
   {
      dtEntity::DebugDrawInterface* ddm = dtEntity::GetDebugDrawInterface();

      if(!ddm->IsEnabled() )
      {
         return Undefined();
      }

      if(args.Length() < 2 || !IsVec3(args[0]) || !IsVec3(args[1]))
      {
         return ThrowError("usage: addAABB(Vec3 min, Vec3 max, [Vec4 color, Int lineWidth, Number duration, bool useDepthTest])");
      }

      dtEntity::Vec3f min = UnwrapVec3(args[0]);
      dtEntity::Vec3f max = UnwrapVec3(args[1]);

      dtEntity::Vec4f color(1,0,0,1);
      if(args.Length() > 2  && IsVec4(args[2]))
      {
         color = UnwrapVec4(args[2]);
      }

      int linewidth = 1;
      if(args.Length() > 3)
      {
         linewidth = args[3]->Int32Value();
         if(linewidth == 0) 
         {
            linewidth = 1;
         }
      }

      float duration = 0;
      if(args.Length() > 4)
      {
         duration = args[4]->NumberValue();
      }

      bool depth = true;
      if(args.Length() > 5)
      {
         depth = args[5]->BooleanValue();
      }

      ddm->AddAABB(min, max, color, linewidth, duration, depth);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerAddCross(const Arguments& args)
   {
      dtEntity::DebugDrawInterface* ddm = dtEntity::GetDebugDrawInterface();

      if(!ddm->IsEnabled() )
      {
         return Undefined();
      }

      if(args.Length() < 2 || !IsVec3(args[0]) || !IsVec4(args[1]))
      {
         return ThrowError("usage: addCross(Vec3 position, Vec4 color, [int linewidth, Number duration, bool useDepthTest])");
      }
      dtEntity::Vec3f pos = UnwrapVec3(args[0]);

      dtEntity::Vec4f color = UnwrapVec4(args[1]);


      int linewidth = 1;
      if(args.Length() > 2)
      {
         linewidth = args[2]->Int32Value();
      }

      float duration = 0;
      if(args.Length() > 3)
      {
         duration = args[3]->NumberValue();
      }

      bool depth = true;
      if(args.Length() > 4)
      {
         depth = args[4]->BooleanValue();
      }

      ddm->AddCross(pos, color, linewidth, duration, depth);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerAddCircle(const Arguments& args)
   {
      dtEntity::DebugDrawInterface* ddm = dtEntity::GetDebugDrawInterface();

      if(!ddm->IsEnabled() )
      {
         return Undefined();
      }

      if(args.Length() < 3 || !IsVec3(args[0]) || !IsVec3(args[1]))
      {
         return ThrowError("usage: addCircle(Vec3 position, Vec3 normal, number radius, Vec4 color, [Number duration, bool useDepthTest])");
      }
      dtEntity::Vec3f pos = UnwrapVec3(args[0]);
      dtEntity::Vec3f nrml = UnwrapVec3(args[1]);
      double radius = args[2]->NumberValue();

      dtEntity::Vec4f color(1,0,0,1);
      if(args.Length() > 3 && IsVec4(args[3]))
      {
         color = UnwrapVec4(args[3]);
      }

      float duration = 0;
      if(args.Length() > 4)
      {
         duration = args[4]->NumberValue();
      }

      bool depth = true;
      if(args.Length() > 5)
      {
         depth = args[5]->BooleanValue();
      }

      ddm->AddCircle(pos, nrml, radius, color, duration, depth);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerAddSphere(const Arguments& args)
   {
      dtEntity::DebugDrawInterface* ddm = dtEntity::GetDebugDrawInterface();

      if(!ddm->IsEnabled() )
      {
         return Undefined();
      }

      if(args.Length() < 2 || !IsVec3(args[0]))
      {
         return ThrowError("usage: addSphere(Vec3 position, number radius, Vec4 color, Number duration, bool useDepthTest])");
      }
      dtEntity::Vec3f pos = UnwrapVec3(args[0]);

      float radius = args[1]->NumberValue();

      dtEntity::Vec4f color(1,0,0,1);
      if(args.Length() > 2 && IsVec4(args[2]))
      {
         color = UnwrapVec4(args[2]);
      }

      float duration = 0;
      if(args.Length() > 3)
      {
         duration = args[3]->NumberValue();
      }

      bool depth = true;
      if(args.Length() > 4)
      {
         depth = args[4]->BooleanValue();
      }

      ddm->AddSphere(pos, radius, color, duration, depth);
      return Undefined();
   }

	////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerAddString(const Arguments& args)
   {
      dtEntity::DebugDrawInterface* ddm = dtEntity::GetDebugDrawInterface();

      if(!ddm->IsEnabled() )
      {
         return Undefined();
      }

      if(args.Length() < 2 || !IsVec3(args[0]))
      {
         return ThrowError("usage: addString(Vec3 position, text, Vec4 color, Number duration, bool useDepthTest])");
      }
      dtEntity::Vec3f pos = UnwrapVec3(args[0]);

		std::string text = ToStdString(args[1]);

      dtEntity::Vec4f color(1,0,0,1);
      if(args.Length() > 2 && IsVec4(args[2]))
      {
         color = UnwrapVec4(args[2]);
      }

      float duration = 0;
      if(args.Length() > 3)
      {
         duration = args[3]->NumberValue();
      }

      bool depth = true;
      if(args.Length() > 4)
      {
         depth = args[4]->BooleanValue();
      }

      ddm->AddString(pos, text, color, duration, depth);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerAddTriangle(const Arguments& args)
   {
      dtEntity::DebugDrawInterface* ddm = dtEntity::GetDebugDrawInterface();

      if(!ddm->IsEnabled() )
      {
         return Undefined();
      }

      if(args.Length() < 4 || !IsVec3(args[0]) || !IsVec3(args[1]) || !IsVec3(args[2])||
         !IsVec4(args[3]))
      {
         return ThrowError("usage: addTriangle(Vec3 v1, Vec3 v2, Vec3 v3, Vec4 color, [int linewidth, Number duration, bool useDepthTest])");
      }

      dtEntity::Vec3f v0 = UnwrapVec3(args[0]);
      dtEntity::Vec3f v1 = UnwrapVec3(args[1]);
      dtEntity::Vec3f v2 = UnwrapVec3(args[2]);

      dtEntity::Vec4f color = UnwrapVec4(args[3]);

      int linewidth = 1;
      if(args.Length() > 4)
      {
         linewidth = args[4]->Int32Value();
      }

      float duration = 0;
      if(args.Length() > 5)
      {
         duration = args[5]->NumberValue();
      }

      bool depth = true;
      if(args.Length() > 6)
      {
         depth = args[6]->BooleanValue();
      }

      ddm->AddTriangle(v0, v1, v2, color, linewidth, duration, depth);
      return Undefined();
   }



   ////////////////////////////////////////////////////////////////////////////////
	Handle<Value> DebugDrawManagerClear(const Arguments& args) 
   {
      dtEntity::GetDebugDrawInterface()->Clear();
      return Undefined();
	}

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Function> CreateDebugDrawManager(Handle<Context> context)
   {
      EscapableHandleScope scope(Isolate::GetCurrent());

      Handle<FunctionTemplate> templt = GetScriptSystem()->GetTemplateBySID(s_debugDrawWrapper);
      if(templt.IsEmpty())
      {

        templt = FunctionTemplate::New();
        templt->SetClassName(String::NewFromUtf8(v8::Isolate::GetCurrent(), "DebugDrawManager"));
        templt->InstanceTemplate()->SetInternalFieldCount(1);

        Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

        proto->Set("addAABB", FunctionTemplate::New(DebugDrawManagerAddAABB));
        proto->Set("addCircle", FunctionTemplate::New(DebugDrawManagerAddCircle));
        proto->Set("addCross", FunctionTemplate::New(DebugDrawManagerAddCross));
        proto->Set("addLine", FunctionTemplate::New(DebugDrawManagerAddLine));
        proto->Set("addLines", FunctionTemplate::New(DebugDrawManagerAddLines));
        proto->Set("addSphere", FunctionTemplate::New(DebugDrawManagerAddSphere));
		  proto->Set("addString", FunctionTemplate::New(DebugDrawManagerAddString));
        proto->Set("addTriangle", FunctionTemplate::New(DebugDrawManagerAddTriangle));
        proto->Set("clear", FunctionTemplate::New(DebugDrawManagerClear));
        proto->Set("isEnabled", FunctionTemplate::New(DebugDrawManagerIsEnabled));
        proto->Set("setEnabled", FunctionTemplate::New(DebugDrawManagerSetEnabled));
        proto->Set("toString", FunctionTemplate::New(DebugDrawManagerToString));
        
        GetScriptSystem()->SetTemplateBySID(s_debugDrawWrapper, templt);
      }
      return handle_scope.Escape(templt->GetFunction());
   }


#endif
}

