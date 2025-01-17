/*****************************************************************************
 *                                  _   _ ____  _     
 *  Project                     ___| | | |  _ \| |    
 *                             / __| | | | |_) | |    
 *                            | (__| |_| |  _ <| |___ 
 *                             \___|\___/|_| \_\_____|
 *
 * $Id: getinmemory.c,v 1.1.1.1 2012/03/29 17:21:21 uid42307 Exp $
 *
 * Example source code to show how the callback function can be used to
 * download data into a chunk of memory instead of storing it in a file.
 *
 * This exact source code has not been verified to work.
 */

/* to make this work under windows, use the win32-functions from the
   win32socket.c file as well */

#include "curl/curl.h"
#include "curl/types.h"
#include "curl/easy.h"

struct MemoryStruct {
  char *memory;
  size_t size;
};

size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  int realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)data;
  
  mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) {
    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }
  return realsize;
}

int main(int argc, char **argv)
{
  CURL *curl_handle;

  struct MemoryStruct chunk;

  chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
  chunk.size = 0;    /* no data at this point */

  curl_global_init(CURL_GLOBAL_DEFAULT);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.cmake.org/HTML/Index.html");

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_FILE, (void *)&chunk);

  /* get it! */
  curl_easy_perform(curl_handle);

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  /*
   * Now, our chunk.memory points to a memory block that is chunk.size
   * bytes big and contains the remote file.
   *
   * Do something nice with it!
   */

  /* For example display it... */
  write(1, chunk.memory, chunk.size);

  return 0;
}
