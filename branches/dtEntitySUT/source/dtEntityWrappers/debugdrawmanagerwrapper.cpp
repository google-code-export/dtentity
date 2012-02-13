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

#include <dtEntity/entitymanager.h>
#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntity/debugdrawmanager.h>
#include <dtEntityWrappers/v8helpers.h>
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace v8;

namespace dtEntityWrappers
{
   ////////////////////////////////////////////////////////////////////////////////
   Persistent<FunctionTemplate> s_debugDrawManagerTemplate;

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerToString(const Arguments& args)
   {
      return String::New("<DebugDrawManager>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerSetEnabled(const Arguments& args)
   {
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);
      ddm->SetEnabled(args[0]->BooleanValue());
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerIsEnabled(const Arguments& args)
   {
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);
      return Boolean::New(ddm->IsEnabled());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> DebugDrawManagerAddLine(const Arguments& args)
   {
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);

      if(args.Length() < 2 || !IsVec3(args[0]) || !IsVec3(args[1]))
      {
         return ThrowError("usage: addLine(Vec3 start, Vec3 end, [Vec4 color, Int lineWidth, Number duration, bool useDepthTest])");
      }

      osg::Vec3f start = UnwrapVec3(args[0]);
      osg::Vec3f end = UnwrapVec3(args[1]);

      osg::Vec4f color(1,0,0,1);
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
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);

      if(args.Length() < 1 || !args[0]->IsArray())
      {
         return ThrowError("usage: addLines(Array(Vec3) lines,[Vec4 color, Int lineWidth, Number duration, bool useDepthTest])");
      }

      std::vector<osg::Vec3> lines;
      HandleScope scope;
      Handle<Array> arr = Handle<Array>::Cast(args[0]);
      
      unsigned int l = arr->Length();
      for(unsigned int i = 0; i < l; ++i)
      {
         lines.push_back(UnwrapVec3(arr->Get(i)));
      }
      
      osg::Vec4f color(1,0,0,1);
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
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);

      if(args.Length() < 2 || !IsVec3(args[0]) || !IsVec3(args[1]))
      {
         return ThrowError("usage: addAABB(Vec3 min, Vec3 max, [Vec4 color, Int lineWidth, Number duration, bool useDepthTest])");
      }

      osg::Vec3f min = UnwrapVec3(args[0]);
      osg::Vec3f max = UnwrapVec3(args[1]);

      osg::Vec4f color(1,0,0,1);
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
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);

      if(args.Length() < 2 || !IsVec3(args[0]) || !IsVec4(args[1]))
      {
         return ThrowError("usage: addCross(Vec3 position, Vec4 color, [int linewidth, Number duration, bool useDepthTest])");
      }
      osg::Vec3f pos = UnwrapVec3(args[0]);

      osg::Vec4f color = UnwrapVec4(args[1]);


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
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);

      if(args.Length() < 3 || !IsVec3(args[0]) || !IsVec3(args[1]))
      {
         return ThrowError("usage: addCircle(Vec3 position, Vec3 normal, number radius, Vec4 color, [Number duration, bool useDepthTest])");
      }
      osg::Vec3f pos = UnwrapVec3(args[0]);
      osg::Vec3f nrml = UnwrapVec3(args[1]);
      double radius = args[2]->NumberValue();

      osg::Vec4f color(1,0,0,1);
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
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);

      if(args.Length() < 2 || !IsVec3(args[0]))
      {
         return ThrowError("usage: addSphere(Vec3 position, number radius, Vec4 color, Number duration, bool useDepthTest])");
      }
      osg::Vec3f pos = UnwrapVec3(args[0]);

      float radius = args[1]->NumberValue();

      osg::Vec4f color(1,0,0,1);
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
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);

      if(args.Length() < 2 || !IsVec3(args[0]))
      {
         return ThrowError("usage: addString(Vec3 position, text, Vec4 color, Number duration, bool useDepthTest])");
      }
      osg::Vec3f pos = UnwrapVec3(args[0]);

		std::string text = ToStdString(args[1]);

      osg::Vec4f color(1,0,0,1);
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
      dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);

      if(args.Length() < 4 || !IsVec3(args[0]) || !IsVec3(args[1]) || !IsVec3(args[2])||
         !IsVec4(args[3]))
      {
         return ThrowError("usage: addTriangle(Vec3 v1, Vec3 v2, Vec3 v3, Vec4 color, [int linewidth, Number duration, bool useDepthTest])");
      }

      osg::Vec3f v0 = UnwrapVec3(args[0]);
      osg::Vec3f v1 = UnwrapVec3(args[1]);
      osg::Vec3f v2 = UnwrapVec3(args[2]);

      osg::Vec4f color = UnwrapVec4(args[3]);

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
	void DebugDrawManager_destroy(Persistent<Value> v, void* ptr) {
		HandleScope scope;
		Handle<Object> o = Handle<Object>::Cast(v);
		dtEntity::DebugDrawManager * ddm = 
         reinterpret_cast<dtEntity::DebugDrawManager*>(o->GetPointerFromInternalField(0));
		delete ddm;
		v.Dispose();
	}

   ////////////////////////////////////////////////////////////////////////////////
	Handle<Value> DebugDrawManagerClear(const Arguments& args) 
   {
		dtEntity::DebugDrawManager* ddm; GetInternal(args.This(), 0, ddm);
      ddm->Clear();
      return Undefined();
	}
   
   ////////////////////////////////////////////////////////////////////////////////
	Handle<Value> DebugDrawManager_create(const Arguments& args) {
      v8::HandleScope handle_scope;
      Handle<Context> context = args.Holder()->CreationContext();
     
		Handle<Object> emh = Handle<Object>::Cast(context->Global()->Get(String::New("EntityManager")));
      dtEntity::EntityManager* em = UnwrapEntityManager(emh);
      args.This()->SetInternalField(0, External::New(new dtEntity::DebugDrawManager(*em)));
      return args.This();
	}

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Function> CreateDebugDrawManager(Handle<Context> context)
   {
      v8::HandleScope handle_scope;
      v8::Context::Scope context_scope(context);

      if(s_debugDrawManagerTemplate.IsEmpty())
      {
        Handle<FunctionTemplate> templt = FunctionTemplate::New(DebugDrawManager_create);
        s_debugDrawManagerTemplate = Persistent<FunctionTemplate>::New(templt);
        templt->SetClassName(String::New("DebugDrawManager"));
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
        
      }
      return handle_scope.Close(s_debugDrawManagerTemplate->GetFunction());
   }
}

