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

#include "myb.h"

#include <sys/stat.h> //or maybe you change it to sysstat

#include <cstdlib>
#include <cstring>
#include <zlib.h>

using namespace myb;

struct dir
{
  /* dir specifies file name and size */
  short         sSize; 
  long long int size ;
};

struct dirv2
{
  short sSize;
  bool compressed;
  long long int originalSize;
  long long int size;
};

void mFile::set(int id , const char* fileName , byte* data , long long int fSize)
{
  /* Function to initialise mFile */
  this->fileName = fileName;
  this->raw = data;
  this->id = id;
  this->fSize = fSize;
  this->uSize = fSize;
  this->compressed = false;
}

void mFile::setv2(int id , const char* fileName , byte* data , long long int fSize , long long int uSize)
{
  /* Function to initialise mFile */
  this->fileName = fileName;
  this->raw = data;
  this->id = id;
  this->fSize = fSize;
  this->uSize = uSize;
}

byte* mFile::getData()
{
  return this->raw;
}

byte* mFile::getUData()
{
    
  /* method to retrieve uncompressed data
     without uncompressing the mFile itself 
  */
  byte* uncompressed_data = (byte*) malloc(this->uSize);
  uncompress((Bytef*) uncompressed_data, (uLongf*) &this->uSize,
	     (Bytef*) this->raw, (uLongf) this->fSize);
  return uncompressed_data;
    
}

void mFile::rename(const char* newName)
{
  /* Renames a file */
  this->fileName = newName;
}

long long int mFile::getFileSize()
{
  return this->fSize;
}

long long int mFile::getUncompressedFileSize()
{
  return this->uSize;
}

int mFile::getID()
{
  return this->id;
}

int mFile::write(const char *directory) //add mFIle::
{
  char subDir[256];
  char output[2048];

  memset(subDir,0,256);
  memset(output,0,2048);
  
  strcat(output, directory);
  strcat(output, this->fileName);

  for(int i = 0; i != strlen(output); i++)
    {
      if(output[i] != '/')
	strncat(subDir , &output[i] , 1);
      else
	{
	  mkdir(subDir , S_IRUSR | S_IWUSR | S_IXUSR);
	  strncat(subDir, &output[i] , 1);
	}
    }
    
  FILE* file = NULL;
  if((file = fopen(output, "w")) == NULL)
    {
      return FILE_PERMISSION_ERROR;      
    }
    
  
  byte* data = this->getUData();
  fwrite(data, 1, this->getUncompressedFileSize(), file);
  
  return 0;
  
}

void mFile::fcompress()
{
  if(!this->compressed)
    {
      this->compressed = true;
      byte* compressed_data = (byte*) malloc(this->uSize * 1.1 + 12);
      compress((Bytef*) compressed_data, (uLongf*) &this->fSize, (Bytef*) this->raw, (uLongf) this->uSize);
      raw = compressed_data;
    }
}

void mFile::funcompress()
{
  if(this->compressed)
    {
      byte* uncompressed_data = (byte*) malloc(this->uSize);
      uncompress((Bytef*) uncompressed_data, (uLongf*) &this->uSize,
		 (Bytef*) this->raw, (uLongf) this->fSize);
    }
}

int bin::open(char *file)
{
  FILE* mybBin = fopen(file, "rb");
  return this->open(mybBin);
}

int bin::getFileCount()
{
  return this->list.size();
}

int bin::open(FILE * mybBin)
{
  if(mybBin == NULL)
    {
      this->lastError = FILE_DOES_NOT_EXIST;
      return FILE_DOES_NOT_EXIST;
    }
  header Header;
  fread(&Header, sizeof(header), 1, mybBin);
  if(strncmp(Header.str , "AGC" , 3) != 0)
    {
      this->lastError = FILE_PARSE_ERROR;
      return FILE_PARSE_ERROR;
    }
  if(Header.ver < 1 || Header.ver > 2)
    {
      this->lastError = FILE_PARSE_ERROR;
      return FILE_PARSE_ERROR;
    }
  dir dir_Temp;
  dirv2 dir_Tempv2;
  
  int id = 0;
  while((Header.ver == 1) ? fread(&dir_Temp, sizeof(dir), 1, mybBin) != sizeof(dir) : fread(&dir_Tempv2, sizeof(dirv2), 1, mybBin) != sizeof(dir))
    {
      char* fileName = (char*) malloc((Header.ver == 1) ? dir_Temp.sSize : dir_Tempv2.sSize);
      byte* data = (byte*) malloc((Header.ver == 1) ? dir_Temp.size : dir_Tempv2.size);
      
      if(fread(fileName, 1, (Header.ver == 1) ? dir_Temp.sSize : dir_Tempv2.sSize, mybBin)
	 != ((Header.ver == 1) ? dir_Temp.sSize : dir_Tempv2.sSize))
	{
	  if(feof(mybBin) != 0)
	    {
	      return SUCCESS;
	    } 
	}
      if(fread(data, sizeof(byte), (Header.ver == 1) ? dir_Temp.size : dir_Tempv2.size, mybBin)
	 != (unsigned int) ((Header.ver == 1) ? dir_Temp.size : dir_Tempv2.size))
	{
	  this->lastError = FILE_PARSE_ERROR;
	  return FILE_PARSE_ERROR;
	}
      mFile file;
      if(Header.ver == 1){
	file.set(id, fileName, data , dir_Temp.size);
	file.compressed = false;
      }
      else if(Header.ver == 2){
	file.setv2(id, fileName, data, dir_Tempv2.size , dir_Tempv2.originalSize);
	file.compressed = dir_Tempv2.compressed;
      }
       
      this->list.push_back(file);
      //free(data);
      id++;
    }
  return SUCCESS;
}

void bin::add(mFile file)
{
  this->list.push_back(file);
}

int bin::remove(int id)
{
  for(int i = 0; i != (int) this->list.size(); i++)
    {
      if(this->list[i].getID() == id)
	{
	  this->list.erase(this->list.begin() + i);
	  return SUCCESS;
	}
    }
  this->lastError = NO_SUCH_ID;
  return NO_SUCH_ID;
}

void bin::close()
{
  this->list.clear();
}

int bin::getError()
{
  return this->lastError;
}

const char* bin::getErrorString(int code)
{
  switch(code)
    {
    case 0:
      return "No such file or directory";
    case 2:
      return "File could not be parsed";
    case 3:
      return "Unable to open file";
    case 4:
      return "Couldn't find the ID/FileName";
    default:
      return "Farbod is very nice :)";
    }
}

int bin::getFileFromName(const char* fileName , mFile &file)
{
  for(int i = 0; i != (int) this->list.size(); i++)
    {
      if(strncmp(this->list[i].fileName,fileName,1024) != 0)
	{
	  file =  this->list[i];
	  return SUCCESS;
	}
    }
  this->lastError = NO_SUCH_ID;
  return NO_SUCH_ID;
}

int bin::getFileFromID(int id , mFile &file)
{
  for(int i = 0; i != (int) this->list.size(); i++)
    {
      if(this->list[i].getID() == id)
	{
	  file = this->list[i];
	  return SUCCESS;
	}
    }
  this->lastError = NO_SUCH_ID;
  return NO_SUCH_ID;
}

int bin::write(char* file)
{
  FILE* mybBin = fopen(file, "wb");
  if(mybBin == NULL)
    {
      this->lastError = FILE_DOES_NOT_EXIST;
      return FILE_DOES_NOT_EXIST;
    }
  header head;
  memcpy(head.str, "AGC", 3);
  head.ver = 2;
  

  fwrite(&head, sizeof(header), 1, mybBin);
  dirv2 temp;
  for(int i = 0; i != this->getFileCount(); i++)
    {
      temp.size = this->list[i].getFileSize();
      temp.sSize = strlen(this->list[i].fileName);
      temp.compressed = this->list[i].compressed;
      temp.originalSize = this->list[i].getUncompressedFileSize();

      fwrite(&temp, sizeof(temp), 1, mybBin);

      fwrite(this->list[i].fileName, 1, temp.sSize, mybBin);
      fwrite(this->list[i].getData() , 1 , temp.size, mybBin);
      
    }
  
  return SUCCESS;
}
