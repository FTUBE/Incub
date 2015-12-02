#include</usr/local/curl/include/curl/curl.h>
#include<fstream>
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<climits>
#include<cstring>
#include<iconv.h>
#include<boost/regex.hpp>
#include<map>
#include<queue>

/*#ifndef ICONV_CONST
# define ICONV_CONST const
#endif*/

bool httpget(char*,char*,char*);
bool checkunicode(char c);
void readbuf(char *buf);
int convert(const char *from, const char *to, char* save, int savelen, char *src, int srclen);
char* singlecatch(char *url, char*burl, char*hurl,bool decode);
std::queue<std::string> freq_ana(char*);
std::map<std::string, int> globalfreq;

using namespace std;
using namespace boost;



int main(){
  
  char url[] = "";
  char burl[] = "new.html";
  char hurl[] = "new_header";
  int i = 0;
  while(true){
    cout<<i++<<endl;
    singlecatch(url,burl,hurl,false);
  }
  /*char *flexible = new char[128];
    
  queue<string> q;
  string root(url);
  q.push(root);
  globalfreq[root]++;
  
  while(!q.empty()){
    cout<<"MAP SIZE : "<<globalfreq.size()<<endl;
    string current = q.front();
    q.pop();
    strcpy(flexible,current.c_str());
    char *data = singlecatch(flexible,burl,hurl);
    queue<string> next = freq_ana(data);
    while(!next.empty()){
      q.push(next.front());
      next.pop();
    }
    }*/
  
  return 0;
} 

void readbuf(char *buf){
  while((*buf)!= '\0') cout<<*(buf++);
}

bool checkunicode(char c){
  return (!(c>=0 && c<=127));
}

bool httpget(char *url, char *save,char *saveh){

  FILE *fp, *fh;

  fp = fopen(save,"w");
  fh = fopen(saveh,"w");
  
  CURL *curl;
  CURLcode res;

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers,"Accept: Agent-007");
  curl = curl_easy_init();
  
  if(curl){
    
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL,url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, fh);
    //    curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
    curl_easy_setopt(curl,CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    res = curl_easy_perform(curl);

    //    if(res != 0){
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    //}
    fclose(fp);
    fclose(fh);
    return true;
  }
  return false;
}


int convert(const char *from, const char *to, char* save, int savelen, char *src, int srclen)
{
  iconv_t cd;
  char   *inbuf = src;
  char *outbuf = save;
  size_t outbufsize = savelen;
  int status = 0;
  size_t  savesize = 0;
  size_t inbufsize = srclen;
  char* inptr = inbuf;
  size_t      insize = inbufsize;
  char* outptr = outbuf;
  size_t outsize = outbufsize;

  cd = iconv_open(to, from);
  iconv(cd,NULL,NULL,NULL,NULL);
  if (inbufsize == 0) {
    status = -1;
    goto done;
  }
  while (insize > 0) {
    size_t res = iconv(cd,&inptr,&insize,&outptr,&outsize);
    if (outptr != outbuf) {
      int saved_errno = errno;
      int outsize = outptr - outbuf;
      strncpy(save+savesize, outbuf, outsize);
      errno = saved_errno;
    }
    if (res == (size_t)(-1)) {
      if (errno == EILSEQ) {
	int one = 1;
	iconvctl(cd,ICONV_SET_DISCARD_ILSEQ,&one);
	status = -3;
      } else if (errno == EINVAL) {
	if (inbufsize == 0) {
	  status = -4;
	  goto done;
	} else {
	  break;
	}
      } else if (errno == E2BIG) {
	status = -5;
	goto done;
      } else {
	status = -6;
	goto done;
      }
    }
  }
  status = strlen(save);
 done:
  iconv_close(cd);
  return status;
}

char* singlecatch(char *url, char*burl, char*hurl,bool decode){

  //  cout << "Catching : " << url << endl;
  httpget(url,burl,hurl);
  
  // cout << "Body and Header stored.\n";
  if(!decode){
    return NULL;
  }
  FILE *bfile,*hfile;
  
  hfile = fopen(hurl,"r");
  
  int ph = 0;
  char *hmem = new char[INT_MAX];
  while(!feof(hfile)){
    hmem[ph++] = fgetc(hfile);
  }
  
  regex pattern("charset=([0-9A-Za-z\\-_\\.]+)");
  string header_str(hmem);
  smatch mat;
  string encoding = "utf-8";
  
  if(regex_search(header_str,mat,pattern)){
    cout << "Encoding : ";
    encoding = mat.format("$1");
    cout << encoding<<endl;
  }
  else{
    cout <<"Encoding : utf-8 (By default)\n";
  }

  delete []hmem;
  
  fclose(hfile);
  
  bfile = fopen(burl,"r");

  char *bmem = new char[INT_MAX];
  int pointer = 0;

  while(!feof(bfile)){
    bmem[pointer++] = fgetc(bfile);
  }
  
  fclose(bfile);

  //  cout <<pointer<<endl;
  char *outbuf = new char[INT_MAX];
  convert("utf-8", encoding.c_str(), outbuf, INT_MAX, bmem, strlen(bmem));
  delete []bmem;
  
  return outbuf;
  
}

queue<string> freq_ana(char* data){

  queue<string> freqmap;
  regex pattern("http://([0-9A-Za-z\\-_\\.]+)/");
  string data_str(data);
  smatch mat;
  string::const_iterator start = data_str.begin();
  string::const_iterator end = data_str.end();
  while(regex_search(start,end,mat,pattern)){
    string craw(mat[1].first,mat[1].second);
    map<string,int>::iterator it = globalfreq.find(craw);

    if(it == globalfreq.end()){
      freqmap.push(craw);
    }
    
    globalfreq[craw]++; 
    //cout << find<<endl;
    start = mat[1].second;
    //    cout <<"w";
  }

  delete []data;
  
  return freqmap;    
}
