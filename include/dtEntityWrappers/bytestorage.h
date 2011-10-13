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
#include <stdlib.h>


/*
  This file is basically copied from the v8cgi project: http://code.google.com/p/v8cgi/
  */

namespace dtEntityWrappers
{
	class ByteStorageData {
	public:
		ByteStorageData(size_t length) {
			this->instances = 1;
			if (length) {
				this->data = (char *) malloc(length);
				if (!this->data) { throw std::string("Cannot allocate enough memory"); }
			} else {
				this->data = NULL;
			}
		}

		~ByteStorageData() {
			if (this->data) { free(this->data); }
		}

		size_t getInstances() {
			return this->instances;
		}

		void setInstances(size_t instances) {
			this->instances = instances;
		}

		char * getData() {
			return this->data;
		}

	private:
		char * data;
		size_t instances;
	};

	/**
	 * Generic byte storage class. Every Buffer instance has this one.
	 */
	class ByteStorage {
	public:
		ByteStorage(size_t length); /* empty */
		ByteStorage(char * data, size_t length); /* with contents (copied) */
		ByteStorage(ByteStorage * master, size_t index1, size_t index2); /* new view */
		~ByteStorage();

		ByteStorageData * getStorage();

		char * getData();
		size_t getLength();
		char getByte(size_t index);
		void setByte(size_t index, char byte);

		void fill(char fill);
		void fill(char * data, size_t length);

		ByteStorage * transcode(const char * from, const char * to);

	protected:

	private:
		char * data;
		size_t length;
		ByteStorageData * storage;
	};
}
