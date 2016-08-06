/*
   Copyright 2016 Parikshit Singh

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <deque>
#include <cstdio>

typedef char byte;

namespace myb
{
  struct mFile
  {
  private:
    byte* raw;
    int id;
    long long int fSize;
    long long int uSize; //un compressed size
  public:
    bool compressed; //do not edit it.
    const char* fileName;
    void rename(const char* newName);
    
    byte *getData(); //compressed data
    byte *getUData(); //Uncompressed data
    
    int write(const char* directory);
    
    int getID();
    long long int getFileSize();
    long long int getUncompressedFileSize();
    
    void set(int id , const char* fileName , byte* data , long long int fSize);
    void setv2(int id , const char* fileName , byte* data , long long int fSize , long long int uSize);

    void fcompress();
    void funcompress();
    
  };
  
  struct header
  {
    char str[4];
    int ver; 
  };
  
  /* Return Values for bin functions */
  const int SUCCESS = 1;
  const int FILE_DOES_NOT_EXIST = 0;
  const int FILE_PARSE_ERROR = 2;
  const int FILE_PERMISSION_ERROR = 3;
  const int NO_SUCH_ID = 4;
  
  struct bin
  {
  private:
    header mHeader;
    int lastError;

  public:

    std::deque<mFile> list;
    
    int open(char* file);
    int open(FILE* file);

    int write(char* file);
    int write(FILE* file);

    int writev1(char* file); //mFile will get uncompressed if it is compressed.
    int writev1(FILE* file);

    int getFileFromName(const char* fileName , mFile &file);
    int getFileFromID(int id , mFile &file);

    void add(mFile file);
    int remove(int id);

    int getFileCount();
    
    void close();

    int getError();
    const char* getErrorString(int code);
    
  };

}
