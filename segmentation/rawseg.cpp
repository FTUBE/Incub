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

  char s1[] ="本书为《红楼梦》诸多续书中格调最为低下的一种。语言淫秽，情节以《红楼梦》中人物为主，但时有色情场面出现，对于少年男女间两性关系，远较《红楼梦》更为直露，一经刊出，不仅立遭禁毁，即连大批推崇《红楼梦》的文人学士，亦同声讨伐攻讦，成为一时盛事。清道光年间著名情爱小说，主要情节由丑陋男子艳羡风情所产生的的性幻想构成。书中鄙弃世俗情爱价值，大写喜新而不厌旧的花痴型男子走马灯般更换情人，不仅先后与十女发生性爱，且同时以数女为妻妾，日日聊床大战，而女性亦纵情享乐，丫环拉小姐下水，小姐为情郎猎艳，最终姊妹、主仆、母女、闺友网常颠覆秽乱不堪，极逞性想像之奇，令人目瞪口呆。通篇皆为男女情事，尤以大量的婚外性关系描写令人啧舌。其间男女道德观念淡薄，无视理法，随意通奸而无羞耻感，反映了当时社会风尚的变迁，人的本能欲望得到重视，对个体生命、感官快乐的追求得到强调，是中国社会早期婚外恋现象的真切记载。";
  char s2[] ="因为很重要我必须说两遍，知识点最重要";

  char *raw_data = load_from_page("data");

  vector<string> sentences = getsentences(parse(raw_data));
  cout << "Sentences : \n\n";
  //  for(int i = 0; i < sentences.size();i++) cout << sentences[i] << endl;
  map<string,int> learn = try_inset(sentences);
  cout << "\nLearn Set : \n\n";
  map<string,int>::iterator it = learn.begin();
  for(;it != learn.end();it++) cout << it->first << endl;
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

  char special_char[] = "，。／；、［］｛｝＝－＋＊（）：；！～｀《》？的了是这么那";

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
