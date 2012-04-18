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
#include <cerrno>
#include <string.h>
#include <string>
#include <iconv.h>
#include <stdlib.h>
#include <dtEntityWrappers/bytestorage.h>
#include <dtEntityWrappers/v8helpers.h>


/*
  This file is basically copied from the v8cgi project: http://code.google.com/p/v8cgi/
  */

#define ALLOC_ERROR throw std::string("Cannot allocate enough memory")

#if defined WIN32 || bsd
#	define ICONV_INPUT_T const char *
#else
#	define ICONV_INPUT_T char *
#endif

#if _MSC_VER
#define snprintf _snprintf
#endif

namespace dtEntityWrappers
{
   /**
    * Create blank with a given length
    */
   ByteStorage::ByteStorage(size_t length) {
      this->length = length;
      this->storage = new ByteStorageData(length);
      this->data = this->storage->getData();
   }

   /**
    * Use a given buffer + length
    */
   ByteStorage::ByteStorage(char * data, size_t length) {
      this->length = length;
      this->storage = new ByteStorageData(length);
      this->data = this->storage->getData();

      if (length) { memcpy(this->data, data, length); }
   }

   ByteStorage::ByteStorage(ByteStorage * bs, size_t index1, size_t index2) {
      this->storage = bs->getStorage();
      this->storage->setInstances(this->storage->getInstances() + 1);
      this->length = index2-index1;
      this->data = bs->getData() + index1;
   }

   ByteStorage::~ByteStorage() {
      this->data = NULL;
      size_t inst = this->storage->getInstances();
      inst--;
      this->storage->setInstances(inst);
      if (!inst) { delete this->storage; } /* last reference */

      this->storage = NULL;
   }

   void ByteStorage::fill(char fill) {
      memset(this->data, fill, this->length);
   }

   void ByteStorage::fill(char * data, size_t length) {
      memcpy(this->data, data, length);
   }

   ByteStorageData * ByteStorage::getStorage() {
      return this->storage;
   }

   size_t ByteStorage::getLength() {
      return this->length;
   }

   void ByteStorage::setByte(size_t index, char byte) {
      if (index >= this->length) { return; }
      this->data[index] = byte;
   }

   char ByteStorage::getByte(size_t index) {
      if (index >= this->length) { return (char)NULL; }
      return this->data[index];
   }

   char * ByteStorage::getData() {
      return this->data;
   }

   ByteStorage * ByteStorage::transcode(const char * from, const char * to) {
      /* no data */
      if (!this->length) { return new ByteStorage(0); }

      /* source and target are the same -> copy */
   /*
      if ((from == to) || (from && to && (strcasecmp(from, to) == 0))) {
         return new ByteStorage(this->data, this->length);
      }
   */
      iconv_t cd = iconv_open(to, from);
      if (cd == (iconv_t)(-1)) {
         std::string error = "Cannot transcode from ";
         error += from;
         error += " to ";
         error += to;
         throw error;
      }

      size_t allocated = this->length + (this->length/8) + 32; /* WAG */
      char * output = (char *) malloc(allocated);
      if (!output) {
         iconv_close(cd);
         ALLOC_ERROR;
      }

      size_t inBytesLeft = this->length;
      size_t outBytesLeft = allocated;
      ICONV_INPUT_T inBuf = (ICONV_INPUT_T) this->data;
      char * outBuf = output;

      size_t result = 0;
      do {
         result = iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
         if (result == (size_t)(-1)) {
            switch (errno) {
               case E2BIG: {
                  /* estimate a new buffer size */
                  size_t newAllocated = allocated + inBytesLeft + (inBytesLeft/4) + 32; /* WAG -- +32 implies output charset cannot exceed 32 bytes per character */

                  char * newOutput = (char *) realloc(output, newAllocated);
                  if (!newOutput) {
                     free(output);
                     iconv_close(cd);
                     ALLOC_ERROR;
                  }

                  /* so far, outBuf-output bytes were used; move outBuf to this position in newOutput */
                  outBuf = newOutput + (outBuf - output);

                  /* adjust target pointer */
                  output = newOutput;

                  /* we allocated this much new bytes */
                  outBytesLeft += newAllocated - allocated;
                  allocated = newAllocated;
               } break;

               default: {
                  free(output);
                  iconv_close(cd);
                  std::string error = "Transcoding error at source byte ";

                  size_t tmpSize = 100;
                  char * tmp = (char *) malloc(tmpSize);
                  if (tmp) {
#ifdef _WIN32
                     #pragma warning (disable : 4996)
#endif
                     size_t result = snprintf(tmp, tmpSize, "%lu", (unsigned long) (inBuf-(char *)this->data));
                     if (result < tmpSize) { error += tmp; }
                     free(tmp);
                  }
                  error += " - ";
                  switch (errno) {
                     case EILSEQ:
                        error += "an invalid multibyte sequence has been encountered in the input";
                     break;
                     case EINVAL:
                        error += "an incomplete multibyte sequence has been encountered in the input";
                     break;
                     default:
                        error += "unknown error";
                     break;
                  }
                  throw error;
               } break;
            }
         }
      } while (result == (size_t)(-1));

      iconv_close(cd);

      /* this is the resulting length */
      size_t len = outBuf - output;
      ByteStorage * bs = new ByteStorage((char *) output, len);

      free(output);
      return bs;
   }
}
