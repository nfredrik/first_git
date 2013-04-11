#!/usr/bin/env python

import httplib

def send(host):
   
    conn = httplib.HTTPConnection(host)
    
    try:
        conn.request('GET', path)
        body = conn.getresponse().read()
        return True
    except:
        return False
    
def main():

      if send('www.microsoft.com'):
          print 'hurra contact!'
      else:
          print 'No contact :-('

if __name__ == '__main__':
    main()


