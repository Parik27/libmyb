# libmyb
A simple archive format library with compression support.

# Format

## Header
* 'AGC' - (char[4])
* Version - (int)

## mFile (version 1)
* sSize - (short int)
* size - (long long int)
* fileName - (char[sSize])
* data - (byte[size])

## mFile (version 2)
* sSize - (short)
* size - (long long int)
* uncompressed_size - (long long int)
* fileName - (char[sSize])
* data - (byte[size])

# Compiling

The library requires no serious compiling , you can just add myb.cxx to your project (Don't forget to link zlib).
If you want to use libMYB with a example.cpp file you have , you can use

``` g++ example.cpp myb.cxx -lz -o example ```

# Usage

There are two main structures , mFile and bin. The bin the the myb file you work with.

```bin::open(char* file)``` - Opens a myb file , returns 1 on success or an error code on error , use ```getErrorString(int code);``` to get a description

```bin::open(FILE* file)``` - Same as above , you just pass a stream instead of filename.

```bin::close()``` - Closes a myb files and frees the data of all the mFiles.

```bin::list``` - is a deque type , which contains mFiles.

```bin::add``` - adds mFile to the list.

```bin::remove``` - removed mFile from the list.

```bin::write(char* file)``` - Writes a bin file to disk.

```bin::write(FILE* file)``` - same as above.

---

mFiles are elements that contain the data.


```bool compressed;``` - is the file compressed?

```const char* fileName;``` - file's name (may contain subdirectories too)

```void rename(const char* newName);``` - renames a file
    
```byte *getData();``` - returns raw data (may or may not be compressed)

```byte *getUData(); ``` - returns uncompressed data
    
```int write(const char* directory);``` - writes the file to disk , and makes directories if necessary.
    
```int getID();``` - returns id of the file

```long long int getFileSize();``` - returns file size of the raw file (may or may not be compressed size)

```long long int getUncompressedFileSize();```- returns the size of the file when it becomes uncompressed
    
```void set(int id , const char* fileName , byte* data , long long int fSize);``` - sets a mFile 

```void setv2(int id , const char* fileName , byte* data , long long int fSize , long long int uSize);``` - sets a v2 mFile

```void fcompress();``` - Compresses a file

```void funcompress();``` - decompresses a file (compresses with fun :) )
