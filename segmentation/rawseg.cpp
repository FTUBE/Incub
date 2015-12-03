#include<iostream>
#include<map>
#include<cstring>
#include<string>
#include<vector>
#include<fstream>

using namespace std;

map<string, int> rs(string a, string b);//
map<string, int> rs(char*,char*);//

map<string, int> shiftcomp(char *s1,char *s2,int ost1, int ost2);
char* parse(char*);//
vector<string> getsentences(char*);//
bool match(char a1, char a2, char a3, char *spec_chars);

map<string, int> try_inset(vector<string>& pass_in);//
char* load_from_page(const char*);

int main(){

  char s1[] ="";
  char s2[] ="";

  char *raw_data = load_from_page("data");
  vector<string> sentences = getsentences(parse(raw_data));
  cout << "Sentences : \n\n";
  //for(int i = 0; i < sentences.size();i++) cout << sentences[i] << endl;
  map<string,int> learn = try_inset(sentences);
  cout << "\nLearn Set : \n\n";
  map<string,int>::iterator it = learn.begin();
  for(;it != learn.end();it++) cout << it->first <<" "<<it->second<< endl;
  /*  cout <<strlen(s1)<<endl;
  cout << strlen(s2)<<endl;
  */
  
  /*
  map<string,int> result= rs(s1,s2);
  
  map<string,int>::iterator it;
  
  for(it = result.begin(); it != result.end();it++){
    cout << it->first<<endl;
  }
  */
    
  return 0;
}

map<string, int> rs(string a, string b){

  char *s1 = new char[a.length()+1];
  char *s2 = new char[b.length()+1];
  
  strcpy(s1,a.c_str());
  strcpy(s2,b.c_str());
  
  map<string, int> frd_res = rs(s1,s2);
  delete []s1;
  delete []s2;
  
  return frd_res;
}

map<string, int> rs(char *s1,char *s2){

  map<string, int> toret;

  int len1 = strlen(s1);
  int len2 = strlen(s2);

  for(int i = len2-3;i >= 0;i-=3){
    map<string, int> cur_rest = shiftcomp(s1,s2,0,i);
    map<string, int>::iterator it = cur_rest.begin();
    for(;it != cur_rest.end();it++) toret[it->first] += it->second;    
  }
  
  for(int i = len1-3;i >= 3;i-=3){ 
    map<string, int> cur_rest = shiftcomp(s1,s2,i,0);
    map<string, int>::iterator it = cur_rest.begin();
    for(;it != cur_rest.end();it++) toret[it->first] += it->second;    
  }
  
  return toret;
}

map<string,int> shiftcomp(char *s1,char *s2,int ost1, int ost2){
  
  char *p1 = s1 + ost1;
  char *p2 = s2 + ost2;

  string buf = "";
  string max = "";

  map<string, int> toret;
  
  while((*p1) != '\0' && (*p2) !='\0'){
    if((*p1) == (*p2)&&(*(p1+1)==*(p2+1))&&(*(p1+2)==*(p2+2))){
      buf = buf.append(1,*p2);
      buf = buf.append(1,*(p2+1));
      buf = buf.append(1,*(p2+2));
      //      cout << (int)*p1<<" "<<(int)*(p1+1)<<endl;
    }
    else{
      if(buf==""){
	p1+=3;
	p2+=3;
	continue;
      }
      if(buf.length()>=6)
	toret[buf]++;
      buf = "";
    }
    p1+=3;
    p2+=3;
  }

  if(buf != "" && buf.length() >=6){
    toret[buf]++;
  }
  
  return toret;
}

char* parse(char *s1){

  char *toret = new char[strlen(s1)+1];
  int pointer = 0;
  
  while((*s1) != '\0'){
    if( (*s1)>127 || (*s1)<0){
      toret[pointer++] = *s1;
    }
    s1++;
  }
  return toret;
}

vector<string> getsentences(char *raw){

  vector<string> s_set;

  char special_char[] = "，。／；、［］｛｝＝－＋＊（）：；！～｀《》？的了是着这那在";

  char *pointer = raw;

  string buf = "";
  
  while((*pointer) != '\0'){
    
    char a1 = *pointer;
    char a2 = *(pointer+1);
    char a3 = *(pointer+2);

    if(match(a1,a2,a3,special_char)) {
      if(buf.length() != 0){
	s_set.push_back(buf);
	buf = "";
      }
      pointer += 3;
      continue;
    }
    buf.append(1,a1);
    buf.append(1,a2);
    buf.append(1,a3);
    pointer += 3;
  }

  if(buf.length()!=0) s_set.push_back(buf);
  
  return s_set;
}

bool match(char a1, char a2, char a3, char *spec_chars){
  char *pointer = spec_chars;

  while((*pointer) != '\0'){
    char a = *pointer;
    char b = *(pointer+1);
    char c = *(pointer+2);
    if(a == a1 && b == a2 && c == a3) return true;
    pointer += 3;
  }

  return false;
}

map<string, int> try_inset(vector<string>& pass_in){
  map<string, int> ttl_res;
  
  for(int i = 0; i < pass_in.size(); i++){
    for(int j = i + 1; j < pass_in.size();j++){
      map<string,int> partial_res = rs(pass_in[i],pass_in[j]);
      map<string,int>::iterator it = partial_res.begin();
      for(;it != partial_res.end();it++) ttl_res[it->first] += it->second;      
    }
  }
  return ttl_res;
}

char* load_from_page(const char *url){

  ifstream in;
  in.open(url);
  
  char *mem = new char[INT_MAX];
  
  char c;

  int p = 0;

  while(in >> c){
    mem[p++] = c;
  }

  in.close();
  return mem;
}
