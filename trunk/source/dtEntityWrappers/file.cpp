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

#include <dtEntityWrappers/file.h>

#include <dtEntity/core.h>
#include <dtEntity/systeminterface.h>
#include <dtEntityWrappers/v8helpers.h>
#include <iostream>
#include <v8.h>
#include <sys/stat.h>

using namespace v8;

/*
  This file is basically copied from the v8cgi project: http://code.google.com/p/v8cgi/
  */
namespace dtEntityWrappers
{

   Persistent<Function> s_file;

   ////////////////////////////////////////////////////////////////////////////////
   JS_METHOD(_file)
   {
      ASSERT_CONSTRUCTOR;

      SAVE_VALUE(0, args[0]);
      SAVE_VALUE(1, JS_BOOL(false));
      return args.This();
   }

   ////////////////////////////////////////////////////////////////////////////////
   JS_METHOD(_open) {
     if (args.Length() < 1) {
       return JS_TYPE_ERROR("Bad argument count. Use 'file.open(mode)'");
     }
     v8::String::Utf8Value mode(args[0]);
     v8::String::Utf8Value name(LOAD_VALUE(0));
     v8::Handle<v8::Value> file = LOAD_VALUE(1);
     if (!file->IsFalse()) {
       return JS_ERROR("File already opened");
     }

     FILE * f;
     std::string abspath = dtEntity::GetSystemInterface()->FindDataFile(*name);
     if(abspath == "") 
     {
        f = fopen(*name, *mode);
     }
     else
     {
        f = fopen(abspath.c_str(), *mode);
     }
     if (!f) {
       return JS_ERROR("Cannot open file");
     }

     SAVE_PTR(1, f);
     return args.This();
   }

   ////////////////////////////////////////////////////////////////////////////////
   JS_METHOD(_read) {


     v8::Handle<v8::Value> file = LOAD_VALUE(1);

     if (file->IsFalse()) {
       return JS_ERROR("File must be opened before reading");
     }
     FILE * f = LOAD_PTR(1, FILE *);

     size_t count = 0;
     if (args.Length() && args[0]->IsNumber()) {
       count = args[0]->IntegerValue();
     }

     std::string data;
     size_t size = 0;
     if (count == 0) { /* all */
       size_t tmp;
       char * buf = new char[1024];
       do {
         tmp = fread(buf, sizeof(char), sizeof(buf), f);
         size += tmp;
         data.insert(data.length(), buf, tmp);
       } while (tmp == sizeof(buf));
       delete[] buf;
     } else {
       char * tmp = new char[count];
       size = fread(tmp, sizeof(char), count, f);
       data.insert(0, tmp, size);
       delete[] tmp;
     }
     return JS_BUFFER(args.This()->CreationContext(), (char *) data.data(), size);
   }

   ////////////////////////////////////////////////////////////////////////////////
   JS_METHOD(_rewind) {
      v8::Handle<v8::Value> file = LOAD_VALUE(1);
      if (file->IsFalse()) {
         return JS_ERROR("File must be opened before rewinding");
      }

      FILE * f = LOAD_PTR(1, FILE *);
      rewind(f);

      return args.This();
   }

   ////////////////////////////////////////////////////////////////////////////////
   JS_METHOD(_close) {
      v8::Handle<v8::Value> file = LOAD_VALUE(1);

      if (file->IsFalse()) {
         return JS_ERROR("Cannot close non-opened file");
      }

      FILE * f = LOAD_PTR(1, FILE *);

      fclose(f);
      SAVE_VALUE(1, JS_BOOL(false));
      return args.This();
   }

   ////////////////////////////////////////////////////////////////////////////////
   JS_METHOD(_flush) {
      v8::Handle<v8::Value> file = LOAD_VALUE(1);

      if (file->IsFalse()) {
         return JS_ERROR("Cannot flush non-opened file");
      }

      FILE * f = LOAD_PTR(1, FILE *);

      fflush(f);
      return args.This();
   }

   ////////////////////////////////////////////////////////////////////////////////
   JS_METHOD(_write) {
      v8::Handle<v8::Value> file = LOAD_VALUE(1);

      if (file->IsFalse()) {
         return JS_ERROR("File must be opened before writing");
      }

      FILE * f = LOAD_PTR(1, FILE *);
      if (IS_BUFFER(args.This()->CreationContext(), args[0])) {
         size_t size = 0;
         char * data = JS_BUFFER_TO_CHAR(args[0], &size);
         fwrite(data, sizeof(char), size, f);
      } else {
         v8::String::Utf8Value data(args[0]);
         fwrite(*data, sizeof(char), args[0]->ToString()->Utf8Length(), f);
      }


      return args.This();
   }

   ////////////////////////////////////////////////////////////////////////////////
   JS_METHOD(_removefile) {
      v8::String::Utf8Value name(LOAD_VALUE(0));

      if (remove(*name) != 0) {
         return JS_ERROR("Cannot remove file");
      }

      return args.This();
   }

	////////////////////////////////////////////////////////////////////////////////
	JS_METHOD(_tostring) {
		return LOAD_VALUE(0);
	}

	////////////////////////////////////////////////////////////////////////////////
	JS_METHOD(_exists) {
      return JS_BOOL(dtEntity::GetSystemInterface()->FileExists(ToStdString(LOAD_VALUE(0))));
	}

	////////////////////////////////////////////////////////////////////////////////
	JS_METHOD(_isfile) {
		v8::String::Utf8Value name(LOAD_VALUE(0));
      return JS_BOOL(dtEntity::GetSystemInterface()->FileExists(*name));
	}

	////////////////////////////////////////////////////////////////////////////////
	v8::Handle<v8::Value> _copy(char * name1, char * name2) {
		size_t size = 0;
		void * data = mmap_read(name1, &size);
		if (data == NULL) { return JS_ERROR("Cannot open source file"); }

		int result = mmap_write(name2, data, size);
		mmap_free((char *)data, size);

		if (result == -1) { return JS_ERROR("Cannot open target file"); }
		return JS_BOOL(true);
	}

	////////////////////////////////////////////////////////////////////////////////
	JS_METHOD(_movefile) {
		if (args.Length() < 1) {
			return JS_TYPE_ERROR("Bad argument count. Use 'file.rename(newname)'");
		}

		v8::String::Utf8Value name(LOAD_VALUE(0));
		v8::String::Utf8Value newname(args[0]);

		int renres = rename(*name, *newname);

		if (renres != 0) {
			v8::Handle<v8::Value> result = _copy(*name, *newname);
			if (result->IsTrue()) {
				remove(*name);
			} else {
				return result;
			}
		}

		SAVE_VALUE(0, args[0]);
		return args.This();
	}

	////////////////////////////////////////////////////////////////////////////////
	JS_METHOD(_copyfile) {
		if (args.Length() < 1) {
			return JS_TYPE_ERROR("Bad argument count. Use 'file.copy(newname)'");
		}

		v8::String::Utf8Value name(LOAD_VALUE(0));
		v8::String::Utf8Value newname(args[0]);

		v8::Handle<v8::Value> result = _copy(*name, *newname);
		if (result->IsTrue()) {
			v8::Handle<v8::Value> fargs[] = { args[0] };
			return s_file->NewInstance(1, fargs);
		} else {
			return result;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	JS_METHOD(_stat) {
		v8::String::Utf8Value name(LOAD_VALUE(0));
		struct stat st;
		if (stat(*name, &st) == 0) {
			v8::Handle<v8::Object> obj = v8::Object::New();
			obj->Set(JS_STR("size"), JS_INT(st.st_size));
			obj->Set(JS_STR("mtime"), JS_INT(st.st_mtime));
			obj->Set(JS_STR("atime"), JS_INT(st.st_atime));
			obj->Set(JS_STR("ctime"), JS_INT(st.st_ctime));
			obj->Set(JS_STR("mode"), JS_INT(st.st_mode));
			obj->Set(JS_STR("uid"), JS_INT(st.st_uid));
			obj->Set(JS_STR("gid"), JS_INT(st.st_gid));
			return obj;
		} else {
			return JS_BOOL(false);
		}
	}

   ////////////////////////////////////////////////////////////////////////////////
   v8::Handle<v8::Function> CreateFile()
   {
     v8::Handle<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(_file);

     v8::Handle<v8::ObjectTemplate> pt = ft->PrototypeTemplate();
     v8::Handle<v8::ObjectTemplate> ot = ft->InstanceTemplate();

     ft->SetClassName(JS_STR("File"));
     ot->SetInternalFieldCount(2);

     pt->Set("open", v8::FunctionTemplate::New(_open));
     pt->Set("read", v8::FunctionTemplate::New(_read));
     pt->Set("rewind", v8::FunctionTemplate::New(_rewind));
     pt->Set("close", v8::FunctionTemplate::New(_close));
     pt->Set("flush", v8::FunctionTemplate::New(_flush));
     pt->Set("write", v8::FunctionTemplate::New(_write));
     pt->Set("remove", v8::FunctionTemplate::New(_removefile));
     pt->Set("toString", v8::FunctionTemplate::New(_tostring));
     pt->Set("exists", v8::FunctionTemplate::New(_exists));
     pt->Set("move", v8::FunctionTemplate::New(_movefile));
     pt->Set("copy", v8::FunctionTemplate::New(_copyfile));
     pt->Set("stat", v8::FunctionTemplate::New(_stat));
     pt->Set("isFile", v8::FunctionTemplate::New(_isfile));

     s_file = v8::Persistent<v8::Function>::New(ft->GetFunction());
     return ft->GetFunction();

   }
}
