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

#include <dtEntityWrappers/v8helpers.h>

#include <dtEntityWrappers/entitymanagerwrapper.h>
#include <dtEntity/log.h>
#include <sstream>
#include <iostream>
#include <osg/MatrixTransform>
#include <osgDB/FileUtils>
#include <fstream>
#ifdef HAVE_MMAN_H
#  include <sys/mman.h>
#  include <sys/stat.h>
#  include <string.h>
#  include <unistd.h>
#  include <fcntl.h>
#else
#  include <stdio.h>
#endif

namespace dtEntityWrappers
{

   using namespace v8;



   ////////////////////////////////////////////////////////////////////////////////
   // lifted from node.js file node.cc
   void ReportException(TryCatch *try_catch) 
   {
        
	  HandleScope scope;

      Handle<Message> message = try_catch->Message();
	  std::string filename_string = "";
	  std::string sourceline_string = "";
	  int linenum = 0;
	  int start = 0;
	  int end = 0;

      if (!message.IsEmpty()) {
       
         String::Utf8Value filename(message->GetScriptResourceName());
         filename_string = *filename;
         linenum = message->GetLineNumber();       
         String::Utf8Value sourceline(message->GetSourceLine());
         sourceline_string = *sourceline;
         start = message->GetStartColumn();       
         end = message->GetEndColumn();

      }

     String::Utf8Value trace(try_catch->StackTrace());
	  std::string tracestr = *trace;
     
	  std::ostringstream os;
     os << std::endl << sourceline_string << std::endl;	  

	  for (int i = 0; i < start; i++) 
	  {
         os << " ";
	  }
	  for (int i = start; i < end; i++) 
	  {
         os << "^";
      }

     os << tracestr << "\n";
	 
	  std::string msg = os.str();
      dtEntity::LogManager::GetInstance().LogMessage(dtEntity::LogLevel::LVL_ERROR, filename_string, "", linenum, msg);     
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GetFileContents(const std::string& fileName, std::string& recipient)
   {
      // load file contents to string variable
      std::ostringstream stream;

      std::string path = osgDB::findDataFile(fileName);

      std::ifstream inFile;
      inFile.open(path.c_str(), std::ios::in);
      if (!inFile) {
         LOG_ERROR("Cannot open script file " + fileName);
         return false;
      }

      char buffer[256];
      while(!inFile.eof() )
      {
         inFile.getline(buffer, sizeof(buffer));
         stream << buffer << "\n";
      }

      inFile.close();
      recipient = stream.str();
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::EntityManager* GetEntityManager(v8::Handle<v8::Context> context)
   {
      Handle<Object> emh = Handle<Object>::Cast(context->Global()->Get(String::New("EntityManager")));
      return UnwrapEntityManager(emh);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Value> WrapVec2(const osg::Vec2d& v)
   {
      HandleScope scope;
      Handle<Array> arr = Array::New(2);
      arr->Set(0, Number::New(v[0]));
      arr->Set(1, Number::New(v[1]));
      arr->Set(String::New("__TYPE_HINT"), String::New("V2"));
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Value> WrapVec3(const osg::Vec3d& v)
   {
      HandleScope scope;
      Handle<Array> arr = Array::New(3);
      arr->Set(0, Number::New(v[0]));
      arr->Set(1, Number::New(v[1]));
      arr->Set(2, Number::New(v[2]));
      arr->Set(String::New("__TYPE_HINT"), String::New("V3"));
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Value> WrapVec4(const osg::Vec4d& v)
   {
      HandleScope scope;
      Handle<Array> arr = Array::New(4);
      arr->Set(0, Number::New(v[0]));
      arr->Set(1, Number::New(v[1]));
      arr->Set(2, Number::New(v[2]));
      arr->Set(3, Number::New(v[3]));
      arr->Set(String::New("__TYPE_HINT"), String::New("V4"));
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Value> WrapQuat(const osg::Quat& v)
   {
      HandleScope scope;
      Handle<Array> arr = Array::New(4);
      arr->Set(0, Number::New(v[0]));
      arr->Set(1, Number::New(v[1]));
      arr->Set(2, Number::New(v[2]));
      arr->Set(3, Number::New(v[3]));
      arr->Set(String::New("__TYPE_HINT"), String::New("QT"));
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Value> WrapMatrix(const osg::Matrix& v)
   {
      HandleScope scope;
      Handle<Array> arr = Array::New(4);
      for(unsigned int i = 0; i < 4; ++i)
      {
         Handle<Array> inner = Array::New(4);
         inner->Set(0, Number::New(v(i, 0)));
         inner->Set(1, Number::New(v(i, 1)));
         inner->Set(2, Number::New(v(i, 2)));
         inner->Set(3, Number::New(v(i, 3)));
         arr->Set(i, inner);
      }
      arr->Set(String::New("__TYPE_HINT"), String::New("MT"));
      return scope.Close(arr);
   }

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Value> WrapSID(dtEntity::StringId v)
   {
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      return String::New(v.c_str());
#else
      return Uint32::New(v);
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::StringId UnwrapSID(v8::Handle<v8::Value> v)
   {
#if DTENTITY_USE_STRINGS_AS_STRINGIDS
      return ToStdString(v);
#else
      return v->Uint32Value();
#endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsVec2(v8::Handle<v8::Value> v)
   {
      return (!v.IsEmpty() && v->IsArray() && Handle<Array>::Cast(v)->Length() >= 2);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsVec3(v8::Handle<v8::Value> v)
   {
      return (!v.IsEmpty() && v->IsArray() && Handle<Array>::Cast(v)->Length() >= 3);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsVec4(v8::Handle<v8::Value> v)
   {
      return (!v.IsEmpty() && v->IsArray() && Handle<Array>::Cast(v)->Length() >= 4);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsQuat(v8::Handle<v8::Value> v)
   {
      return (!v.IsEmpty() && v->IsArray() && Handle<Array>::Cast(v)->Length() >= 4);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool IsMatrix(v8::Handle<v8::Value> v)
   {
      return (!v.IsEmpty() && v->IsArray() && Handle<Array>::Cast(v)->Length() >= 16);
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2d UnwrapVec2(v8::Handle<v8::Value> v)
   {
      HandleScope scope;
      Handle<Array> arr = Handle<Array>::Cast(v);
      return osg::Vec2d(arr->Get(0)->NumberValue(),
                       arr->Get(1)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3d UnwrapVec3(v8::Handle<v8::Value> v)
   {
      HandleScope scope;
      Handle<Array> arr = Handle<Array>::Cast(v);
      return osg::Vec3d(arr->Get(0)->NumberValue(),
                       arr->Get(1)->NumberValue(),
                       arr->Get(2)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec4d UnwrapVec4(v8::Handle<v8::Value> v)
   {
      HandleScope scope;
      Handle<Array> arr = Handle<Array>::Cast(v);
      return osg::Vec4d(arr->Get(0)->NumberValue(),
                       arr->Get(1)->NumberValue(),
                       arr->Get(2)->NumberValue(),
                       arr->Get(3)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Quat UnwrapQuat(v8::Handle<v8::Value> v)
   {
      HandleScope scope;
      Handle<Array> arr = Handle<Array>::Cast(v);
      return osg::Quat(arr->Get(0)->NumberValue(),
                       arr->Get(1)->NumberValue(),
                       arr->Get(2)->NumberValue(),
                       arr->Get(3)->NumberValue());
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Matrix UnwrapMatrix(v8::Handle<v8::Value> v)
   {
      HandleScope scope;
      Handle<Array> arr = Handle<Array>::Cast(v);

      return osg::Matrix(arr->Get(0)->NumberValue(),
                         arr->Get(1)->NumberValue(),
                         arr->Get(2)->NumberValue(),
                         arr->Get(3)->NumberValue(),
						 arr->Get(4)->NumberValue(),
						 arr->Get(5)->NumberValue(),
						 arr->Get(6)->NumberValue(),
						 arr->Get(7)->NumberValue(),
						 arr->Get(8)->NumberValue(),
						 arr->Get(9)->NumberValue(),
						 arr->Get(10)->NumberValue(),
						 arr->Get(11)->NumberValue(),
						 arr->Get(12)->NumberValue(),
						 arr->Get(13)->NumberValue(),
						 arr->Get(14)->NumberValue(),
						 arr->Get(15)->NumberValue()
                       );
   }


   ////////////////////////////////////////////////////////////////////////////////
   void * mmap_read(char * name, size_t * size) {
   #ifdef HAVE_MMAN_H
      int f = open(name, O_RDONLY);
      if (f == -1) { return NULL; }
      *size = lseek(f, 0, SEEK_END);
      void * data = mmap(0, *size, PROT_READ, MAP_SHARED, f, 0);
      close(f);
   #else
#ifdef _WIN32
      #	pragma warning(push)
      #pragma warning (disable : 4996)
#endif
      FILE * f = fopen(name, "rb");
#ifdef _WIN32
      #	pragma warning(pop)
#endif
      if (f == NULL) { return NULL; }
      fseek(f, 0, SEEK_END);
      size_t s = ftell(f);
      *size = s;

      rewind(f);
      char * data = new char[s];
      for (unsigned int i=0; i<s;) {
         size_t read = fread(& data[i], 1, s-i, f);
         i += read;
      }
      fclose(f);
   #endif
      return data;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void mmap_free(char * data, size_t size) {
   #ifdef HAVE_MMAN_H
      munmap(data, size);
   #else
      delete[] data;
   #endif
   }

   ////////////////////////////////////////////////////////////////////////////////
   int mmap_write(char * name, void * data, size_t size) {
   #ifdef HAVE_MMAN_H
      int f = open(name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      if (f == -1) { return -1; }
      lseek(f, size - 1, SEEK_SET);
      size_t written = write(f, "", 1);
      if (written != 1) { return -1; }

   void * mmap_read(char * name, size_t * size);
   void mmap_free(char * data, size_t size);
   int mmap_write(char * name, void * data, size_t size);
      void * dst = mmap(0, size, PROT_WRITE, MAP_SHARED, f, 0);
      memcpy(dst, data, size);
      munmap(dst, size);
      close(f);

   #else
#ifdef _WIN32
      #	pragma warning(push)
      #pragma warning (disable : 4996)
#endif
      FILE * f = fopen(name, "wb");

#ifdef _WIN32
      #	pragma warning(pop)
#endif
      if (f == NULL) { return -1; }
      fwrite(data, size, 1,f);
      fclose(f);
   #endif
      return 0;
   }
}

