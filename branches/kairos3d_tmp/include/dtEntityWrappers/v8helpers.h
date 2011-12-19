#pragma once

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

#include <v8.h>
#include <string>
#include <dtEntity/entitymanager.h>
#include <dtEntityWrappers/export.h>
#include <dtEntityWrappers/wrappermanager.h>
#include <dtEntityWrappers/bytestorage.h>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Matrix>


// These macros are copied from v8cgi macros.h
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define SAVE_PTR(index, ptr) args.This()->SetPointerInInternalField(index, (void *)(ptr))
#define LOAD_PTR(index, type) reinterpret_cast<type>(args.This()->GetPointerFromInternalField(index))
#define SAVE_VALUE(index, val) args.This()->SetInternalField(index, val)
#define LOAD_VALUE(index) args.This()->GetInternalField(index)
#define JS_STR(...) v8::String::New(__VA_ARGS__)
#define JS_INT(val) v8::Integer::New(val)
#define JS_FLOAT(val) v8::Number::New(val)
#define JS_BOOL(val) v8::Boolean::New(val)
#define JS_NULL v8::Null()
#define JS_UNDEFINED v8::Undefined()
#define JS_METHOD(name) v8::Handle<v8::Value> name(const v8::Arguments& args)
#define INSTANCEOF(obj, func) func->HasInstance(obj)

#define JS_THROW(type, reason) v8::ThrowException(v8::Exception::type(JS_STR(reason)))
#define JS_ERROR(reason) JS_THROW(Error, reason)
#define JS_TYPE_ERROR(reason) JS_THROW(TypeError, reason)
#define JS_RANGE_ERROR(reason) JS_THROW(RangeError, reason)
#define JS_SYNTAX_ERROR(reason) JS_THROW(SyntaxError, reason)
#define JS_REFERENCE_ERROR(reason) JS_THROW(ReferenceError, reason)
#define JS_RETHROW(tc) v8::Local<v8::Value>::New(tc.Exception());

#define JS_GLOBAL v8::Context::GetCurrent()->Global()
#define GLOBAL_PROTO v8::Handle<v8::Object>::Cast(JS_GLOBAL->GetPrototype())
#define GC_PTR reinterpret_cast<GC *>(v8::Handle<v8::External>::Cast(GLOBAL_PROTO->GetInternalField(1))->Value())

#define ASSERT_CONSTRUCTOR if (!args.IsConstructCall()) { return JS_ERROR("Invalid call format. Please use the 'new' operator."); }
#define ASSERT_NOT_CONSTRUCTOR if (args.IsConstructCall()) { return JS_ERROR("Invalid call format. Please do not use the 'new' operator."); }
#define RETURN_CONSTRUCT_CALL \
	std::vector< v8::Handle<v8::Value> > params(args.Length()); \
	for (size_t i=0; i<params.size(); i++) { params[i] = args[i]; } \
	return args.Callee()->NewInstance(args.Length(), &params[0]);

#ifdef _WIN32
#   define SHARED_INIT() extern "C" __declspec(dllexport) void init(v8::Handle<v8::Function> require, v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module)
#else
#   define SHARED_INIT() extern "C" void init(v8::Handle<v8::Function> require, v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module)
#endif



namespace dtEntityWrappers
{

   inline v8::Handle<v8::Context> DTENTITY_WRAPPERS_EXPORT GetGlobalContext()
   {
      return WrapperManager::GetInstance().GetGlobalContext();
   }

   inline v8::Handle<v8::Value> BYTESTORAGE_TO_JS(ByteStorage * bs) {
      using namespace v8;
      HandleScope scope;
      Handle<Context> context = GetGlobalContext();
      Handle<Function> buffer = Handle<Function>::Cast(context->Global()->Get(String::New("Buffer")));
      assert(!buffer.IsEmpty() && buffer->IsFunction());
      Handle<v8::Value> newargs[] = { External::New((void*)bs) };
      return scope.Close(Handle<Function>::Cast(buffer)->NewInstance(1, newargs));
   }

	inline ByteStorage * JS_TO_BYTESTORAGE(v8::Handle<v8::Value> value) {
		v8::Handle<v8::Object> object = value->ToObject();
		return reinterpret_cast<ByteStorage *>(object->GetPointerFromInternalField(0));
	}

	inline v8::Handle<v8::Value> JS_BUFFER(char * data, size_t length) {
		ByteStorage * bs = new ByteStorage(data, length);
		return BYTESTORAGE_TO_JS(bs);
	}

	inline char * JS_BUFFER_TO_CHAR(v8::Handle<v8::Value> value, size_t * size) {
		ByteStorage * bs = JS_TO_BYTESTORAGE(value);
		*size = bs->getLength();
		return bs->getData();
	}

	inline bool IS_BUFFER(v8::Handle<v8::Value> value) {
      using namespace v8;
		if (!value->IsObject()) { return false; }
		Handle<Value> proto = value->ToObject()->GetPrototype();
		HandleScope scope;
		Handle<Context> context = GetGlobalContext();
		Handle<Function> buffer = Handle<Function>::Cast(context->Global()->Get(String::New("Buffer")));
		assert(!buffer.IsEmpty() && buffer->IsFunction());
		Handle<Value> prototype = buffer->Get(JS_STR("prototype"));
		return proto->Equals(prototype);
	}

	void * mmap_read(char * name, size_t * size);
	void mmap_free(char * data, size_t size);
	int mmap_write(char * name, void * data, size_t size);


   /** helper function to get string from file */
   bool GetFileContents(const std::string& fileName, std::string& recipient);

   dtEntity::EntityManager* GetEntityManager();

   /** 
     * helper function to extract internal field from object.
     * @param obj The object containing an internal field
     * @param internalFieldIndex Extract this internal field
     * @param compType address of internal field value is written to this pointer 
     */
   template <typename T> 
   inline void GetInternal(v8::Handle<v8::Object> obj, 
      unsigned int internalFieldIndex, T*& compType)
   {
      compType = static_cast<T*>(obj->GetPointerFromInternalField(internalFieldIndex));
   }


   /** add a function to function template.
     * @param fn The function template that will have the new function
     * @param name The javascript name of the new function
     * @param cb The C callback when function is executed from javascript
     */
   void inline AddFunctionToTpl(v8::Handle<v8::Template> fn,
      const char* name, v8::InvocationCallback cb)
   {
      fn->Set(v8::String::New(name), v8::FunctionTemplate::New(cb));
   }

   /** convert ANY js value to string 
     * If this crashes: val is probably broken.
     * This happens if a value was defined as local and not returned
     * with handle_scope.Close(value)
     * or if an execution context was not set
     */
   inline std::string ToStdString(v8::Handle<v8::Value> val)
   {
      v8::String::Utf8Value str(val);
      return *str ? *str : "<string conversion failed>";
   }

   /** write a JavaScript exception to log */
   void DTENTITY_WRAPPERS_EXPORT ReportException(v8::TryCatch* try_catch, bool showline = true);

   /**
     * shortcut to throw error with std string error message
     */
   inline v8::Handle<v8::Value> ThrowError(const std::string& err)
   {
      return v8::ThrowException(v8::Exception::Error(v8::String::New(err.c_str())));
   }

   inline v8::Handle<v8::Value> ThrowSyntaxError(const std::string& err)
   {
      return v8::ThrowException(v8::Exception::SyntaxError(v8::String::New(err.c_str())));
   }

   DTENTITY_WRAPPERS_EXPORT v8::Handle<v8::Value> WrapVec2(const osg::Vec2d& v);
   DTENTITY_WRAPPERS_EXPORT v8::Handle<v8::Value> WrapVec3(const osg::Vec3d& v);
   DTENTITY_WRAPPERS_EXPORT v8::Handle<v8::Value> WrapVec4(const osg::Vec4d& v);
   DTENTITY_WRAPPERS_EXPORT v8::Handle<v8::Value> WrapQuat(const osg::Quat& v);
   DTENTITY_WRAPPERS_EXPORT v8::Handle<v8::Value> WrapMatrix(const osg::Matrix& v);

   DTENTITY_WRAPPERS_EXPORT bool IsVec2(v8::Handle<v8::Value> v);
   DTENTITY_WRAPPERS_EXPORT bool IsVec3(v8::Handle<v8::Value> v);
   DTENTITY_WRAPPERS_EXPORT bool IsVec4(v8::Handle<v8::Value> v);
   DTENTITY_WRAPPERS_EXPORT bool IsQuat(v8::Handle<v8::Value> v);
   DTENTITY_WRAPPERS_EXPORT bool IsMatrix(v8::Handle<v8::Value> v);

   DTENTITY_WRAPPERS_EXPORT osg::Vec2d UnwrapVec2(v8::Handle<v8::Value> v);
   DTENTITY_WRAPPERS_EXPORT osg::Vec3d UnwrapVec3(v8::Handle<v8::Value> v);
   DTENTITY_WRAPPERS_EXPORT osg::Vec4d UnwrapVec4(v8::Handle<v8::Value> v);
   DTENTITY_WRAPPERS_EXPORT osg::Quat UnwrapQuat(v8::Handle<v8::Value> v);
   DTENTITY_WRAPPERS_EXPORT osg::Matrix UnwrapMatrix(v8::Handle<v8::Value> v);

}
