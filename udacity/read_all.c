#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

/*

    Testing a little unix utility, read_all().

    read_all() makes sure that we always get the
    number of bytes requested.

 */


/* 
   Fault injection.
   
   Return a randomly numbers of bytes return.

 */
ssize_t read_fi(int fildes, void *buf, size_t nbyte)
{
  nbyte = (rand() % nbyte) + 1;
  return read(fildes, buf, nbyte);
  
}

/* Good stress test, just read one byte */
ssize_t read_one(int fildes, void *buf, size_t nbyte)
{
  return read(fildes, buf, 1);
  
}

/* Simulate error conditions */
ssize_t read_error(int fildes, void *buf, size_t nbyte)
{
  errno = -EIO;
  /*
  switch (rand() % 9) {
  case 0:
    errno = -EIO;
    break;
  default:
    errno = -EIO;    
  }
  */
  return -1;
  
}

/* Simulate end of file, EOF */
ssize_t read_eof(int fildes, void *buf, size_t nbyte)
{
  return 0;
  
}


/*
Simulate:

 - First 100 calls succeeds
 - then fail 1 promille of the calls
*/
ssize_t read_one_percentage_fail(int fildes, void *buf, size_t nbyte)
{
  static count = 0;

  if (count++ < 100)
    return  read_fi(fildes, buf, nbyte);
  
  if ((rand() % 1000) == 37) {
    printf("ajjjj\n");
    return read_error(fildes, buf, nbyte);
  }
  else
    return read_fi(fildes, buf, nbyte);
}

/*
  This part is the software under test - S.U.T.
 */
ssize_t read_all(int fildes, void *buf, size_t nbyte)
{

  /* Sanity Check */
  assert(fildes >=0);
  assert(buf);
  assert(nbyte >= 0);

  size_t left = nbyte;

  while(1) {
    int res = read_one_percentage_fail(fildes, buf, left);
    //int res = read_one(fildes, buf, left);
    // int res = read(fildes, buf, left);
    printf("res:%d\n", res);
    if ( res < 1) 
       return res; 
    buf += res; 
    left -= res; 
    assert (left >= 0);
    if (left == 0)
      return nbyte;
  }
}



int main(void)
{
  srand(time(NULL));
  int fd = open("nosse.txt", O_RDONLY);

  assert( fd >= 0);

  struct stat buf;
  int res = fstat(fd, &buf);
  assert (res == 0);

  off_t len = buf.st_size;
  char *definitive = (char*) malloc(len);
  assert (definitive);

  res = read(fd, definitive, len);
  assert ( res == len );

  int i;
  char *test = (char *) malloc(len);

  for(i=0; i<100;i++ ) {

    res = lseek(fd, 0, SEEK_SET);
    assert(res == 0);

    int j;
    for(j=0; j<len;j++ ) {
      test[j]= rand();
    }

    res = read_all(fd, test, len);
    assert(res==len);
    assert(strcmp(test, definitive, len) == 0);
  }

  return 0;

}
