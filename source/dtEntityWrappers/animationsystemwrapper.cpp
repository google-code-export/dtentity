/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
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

#include <dtEntityWrappers/animationsystemwrapper.h>
#include <dtEntityWrappers/entitysystemwrapper.h>
#include <dtEntityWrappers/propertyconverter.h>
#include <dtEntity/animationcomponent.h>

#include <dtEntityWrappers/scriptcomponent.h>
#include <dtEntityWrappers/v8helpers.h>
#include <dtEntityWrappers/wrappers.h>

#include <dtAnim/animatable.h>
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace v8;

namespace dtEntityWrappers
{
   class AnimationEndCallbackHolder
   {
      typedef std::map<const dtAnim::Animatable*,Persistent<Function> > CallbackMap;
   public:

      AnimationEndCallbackHolder()
      {
         mAnimEndCallback = dtAnim::AnimationCallback(this, &AnimationEndCallbackHolder::OnAnimationCompleted);
      }

      ~AnimationEndCallbackHolder()
      {

      }
      void AddAnimationEndCallback(dtAnim::Animatable* animatable, Handle<Function> funct)
      {
         mCallbacks[animatable] = Persistent<Function>::New(funct);
         animatable->SetEndCallback(mAnimEndCallback);
      }

      void OnAnimationCompleted(const dtAnim::Animatable& anim)
      {
         CallbackMap::iterator iter = mCallbacks.find(&anim);
         if(iter == mCallbacks.end())
         {
            LOG_ERROR("Callback called but was not registered ?!?");
            return;
         }
         // only execute once

         HandleScope scope;
         TryCatch try_catch;
         Handle<Value> result = iter->second->Call(iter->second, 0, NULL);
         mCallbacks.erase(iter);
         if(result.IsEmpty())
         {
            ReportException(&try_catch);
         }
      }

   private:
      CallbackMap mCallbacks;
      dtAnim::AnimationCallback mAnimEndCallback;
   };

   AnimationEndCallbackHolder s_animationEndCallbackHolder;

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::AnimationSystem* UnwrapAnimationSystem(v8::Handle<v8::Value> val)
   {
      dtEntity::AnimationSystem* v;
      GetInternal(Handle<Object>::Cast(val), 0, v);
      return v;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ASToString(const Arguments& args)
   {
      return String::New("<AnimationSystem>");
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ASPlayAnimation(const Arguments& args)
   {
      dtEntity::AnimationSystem* ss = UnwrapAnimationSystem(args.This());
      dtEntity::Component* acomp = ss->GetComponent(args[0]->Uint32Value());
      if(acomp == NULL) 
      {
         return ThrowError("Cannot play animation: component with this entity id not found!");
      }
      dtEntity::AnimationComponent* animComp = static_cast<dtEntity::AnimationComponent*>(acomp);
      std::string animname = ToStdString(args[1]);
      animComp->PlayAnimation(animname);

      // register callback to be executed when animation ends
      if(args.Length() >= 2 && args[2]->IsFunction())
      {
         dtAnim::Animatable* anim = animComp->GetActiveAnimation(animname);
         assert(anim != NULL);
         s_animationEndCallbackHolder.AddAnimationEndCallback(anim, Handle<Function>::Cast(args[2]));
      }

      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ASClearAnimation(const Arguments& args)
   {
      dtEntity::AnimationSystem* ss = UnwrapAnimationSystem(args.This());
      
      dtEntity::Component* acomp = ss->GetComponent(args[0]->Uint32Value());
      if(acomp == NULL) 
      {
         return ThrowError("Cannot clear animation: component with this entity id not found!");
      }
      std::string animname = ToStdString(args[1]);
      static_cast<dtEntity::AnimationComponent*>(acomp)->ClearAnimation(animname, (float)args[2]->NumberValue());
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ASHasAnimation(const Arguments& args)
   {
      dtEntity::AnimationSystem* ss = UnwrapAnimationSystem(args.This());
      dtEntity::Component* acomp = ss->GetComponent(args[0]->Uint32Value());
      if(acomp == NULL) 
      {
         return False();
      }
      dtEntity::AnimationComponent* animComp = static_cast<dtEntity::AnimationComponent*>(acomp);
      std::string animname = ToStdString(args[1]);
      return Boolean::New(animComp->HasAnimation(animname));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ASIsAnimationPlaying(const Arguments& args)
   {
      dtEntity::AnimationSystem* ss = UnwrapAnimationSystem(args.This());
      dtEntity::Component* acomp = ss->GetComponent(args[0]->Uint32Value());
      if(acomp == NULL) 
      {
         return False();
      }
      dtEntity::AnimationComponent* animComp = static_cast<dtEntity::AnimationComponent*>(acomp);
      std::string animname = ToStdString(args[1]);
      return Boolean::New(animComp->IsAnimationPlaying(animname));
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ASClearAll(const Arguments& args)
   {
      dtEntity::AnimationSystem* ss = UnwrapAnimationSystem(args.This());
      dtEntity::Component* acomp = ss->GetComponent(args[0]->Uint32Value());
      if(acomp == NULL) 
      {
         return ThrowError("Cannot clear all animations: component with this entity id not found!");
      }

      // a fadeout of 0 does not work somehow. Make default higher
      float fadeout = 0.5f;
      if(args.Length() > 1)
      {
         fadeout = (float)args[1]->NumberValue();
      }
      static_cast<dtEntity::AnimationComponent*>(acomp)->ClearAll(fadeout);
      return Undefined();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Handle<Value> ASGetRegisteredAnimations(const Arguments& args)
   {
      dtEntity::AnimationSystem* ss = UnwrapAnimationSystem(args.This());
      dtEntity::Component* acomp = ss->GetComponent(args[0]->Uint32Value());
      if(acomp == NULL) 
      {
         return ThrowError("Cannot get registered animations: component with this entity id not found!");
      }
      std::vector<std::string> names;
      static_cast<dtEntity::AnimationComponent*>(acomp)->GetRegisteredAnimations(names);
      HandleScope scope;
      Handle<Array> arr = Array::New(names.size());
      for(unsigned int i = 0; i < names.size(); ++i)
      {
         arr->Set(Int32::New(i), String::New(names[i].c_str()));
      }
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InitAnimationSystemWrapper(ScriptSystem* ss)
   {
      HandleScope scope;
      Context::Scope context_scope(ss->GetGlobalContext());

      Handle<FunctionTemplate> templt = FunctionTemplate::New();
      templt->SetClassName(String::New("SoundSystem"));
      templt->InstanceTemplate()->SetInternalFieldCount(1);

      Handle<ObjectTemplate> proto = templt->PrototypeTemplate();

      proto->Set("toString", FunctionTemplate::New(ASToString));
      proto->Set("playAnimation", FunctionTemplate::New(ASPlayAnimation));
      proto->Set("clearAnimation", FunctionTemplate::New(ASClearAnimation));
      proto->Set("hasAnimation", FunctionTemplate::New(ASHasAnimation));
      proto->Set("isAnimationPlaying", FunctionTemplate::New(ASIsAnimationPlaying));
      proto->Set("clearAll", FunctionTemplate::New(ASClearAll));
      proto->Set("getRegisteredAnimations", FunctionTemplate::New(ASGetRegisteredAnimations));
      
      RegisterEntitySystempWrapper(ss, dtEntity::AnimationComponent::TYPE, templt);
   }

}
