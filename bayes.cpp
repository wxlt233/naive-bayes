#include <iostream>
#include <string>
#include <io.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <map>
#include <windows.h>
#include <set>
#include <math.h>
using namespace std;
int nsample;

map<string,int> emails[5000];  //ÿ���ʼ���Ӧ��map�����е�һ��map,ÿ���ڵ�i���ʼ��г��ֵĵ��ʷֱ���һ��<string,int>����ʾ,
							   //��һ����ֵ��ʾ�õ��ʣ��ڶ�����ֵ��ʾ�õ��ʳ��ֵĴ���

set <string>wholeset;          //�洢�������ʼ��г��ֵĵ���
int label[5000];               //��ʾ�ʼ���ǩ,label[i]=1���i���ʼ�Ϊ�����ʼ�������Ϊ�����ʼ�
bool fortraining[5000];        //����ѡ��ѵ������fortraining[i]=1��ʾ��i���ʼ��˴�ʵ���б���Ϊѵ����
#define stopornot 0            //���ڿ����Ƿ�ȥ��Ӣ���г��ô�

int n;


vector<string>emailnames;

int result1[5000],result2[5000];

int totalh2s,totals2h;
int totalnham,totalnspam;

void getFiles( string path, string exd, vector<string>& files )
{
	long   hFile   =   0;
	struct _finddata_t fileinfo;
	string pathName, exdName;
	if (0 != strcmp(exd.c_str(), ""))
	{
		exdName = "\\*." + exd;
	}
	else
	{
		exdName = "\\*";
	}
	
	if((hFile = _findfirst(pathName.assign(path).append(exdName).c_str(),&fileinfo)) !=  -1)
	{
		do
		{
			if((fileinfo.attrib &  _A_SUBDIR))
			{
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
					getFiles( pathName.assign(path).append("\\").append(fileinfo.name), exd, files );
			}
			else
			{
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
					files.push_back(pathName.assign(path).append("\\").append(fileinfo.name));
			}
		}while(_findnext(hFile, &fileinfo)  == 0);
		_findclose(hFile);
	}
}
int tttt;
double laplace;
string test[]={"0", "1", "2", "3", "4", "5", "6", "7",
                                "8", "9", "about", "after", "all", "also",
                                "an", "and", "another", "any", "are", "as",
                                "at", "be", "because", "been", "before",
                                "being", "between", "both", "but", "by",
                                "came", "can", "come", "could", "did", "do",
                                "does", "each", "else", "for", "from", "get",
                                "got", "has", "had", "he", "have", "her",
                                "here", "him", "himself", "his", "how", "if",
                                "in", "into", "is", "it", "its", "just", "like",
                                "make", "many", "me", "might", "more", "most",
                                "much", "must", "my", "never", "now", "of",
                                "on", "only", "or", "other", "our", "out",
                                "over", "re", "said", "same", "see", "should",
                                "since", "so", "some", "still", "such", "take",
                                "than", "that", "the", "their", "them", "then",
                                "there", "these", "they", "this", "those",
                                "through", "to", "too", "under", "up", "use",
                                "very", "want", "was", "way", "we", "well",
                                "were", "what", "when", "where", "which", "while",
                                "who", "will", "with", "would", "you", "your",
                                "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
                                "k", "l", "m", "n", "o", "p", "q", "r", "s","t",
								"u", "v", "w", "x", "y", "z", ""};

set<string>stopword;
void choosetrainingset( )
{
	DWORD seed=GetTickCount();
	srand(int(seed));
	int testcase=0;
	int t=0;
	while (testcase<nsample)
	{
		t=rand()%n;
		if (fortraining[t])
			continue;
		else 
		{
			fortraining[t]=1;
			testcase++;
		}
	}
}

/*void choosetrainingset()
{
	srand(int(time(0)));
	int testcase1=0;
	int t=0;
	while (testcase1<0.6*nsample)
	{
		t=rand()%n;
		if (fortraining[t]||label[t])
			continue;
		else 
		{
			fortraining[t]=1;
			testcase1++;
		}
	}
	int testcase2=0;
	while (testcase2<0.4*nsample)
	{
		t=rand()%n;
		if (fortraining[t]||!label[t])
			continue;
		else 
		{
			fortraining[t]=1;
			testcase2++;
		}
	}
}*/

int main()
{
	int totaltest;
	bool smallorbig;
	cout<<"��ѡ��ʹ��50���ʼ���С���ݼ�����4327���ʼ��Ĵ����ݼ���(0:С����,1:������)"<<endl;
	cin>>smallorbig;
	cout<<"��ѡ��ѵ����������"<<endl;
	cin>>nsample;
	cout<<"��ѡ��ƽ������"<<endl;
	cin>>laplace;
	cout<<"��ѡ����Դ������Ƽ�����10��,żȻ�Խ��٣�"<<endl;
	cin>>totaltest;

	char *emailpath;
	if (smallorbig)
		emailpath="emails";
    else 
		emailpath="emails1";

	getFiles(emailpath,"txt",emailnames);
	n=emailnames.size();
	
	for (int i=0;i<153;i++)
	{
		if (test[i].length()>2)
		{
			stopword.insert(test[i]);
		}
	}
	
	DWORD start=GetTickCount();
	
	ifstream file;
	string line; 
	if (smallorbig)
		file.open("SPAMTrain.label");
	else 
		file.open("SPAMTrain1.label");
	int check=0;
	int i=0;
	while (getline(file,line))
	{
	    istringstream test(line);
		string word;
		while (test>>word)
		{
			check=1-check;
			if (check) 
					label[i]=atoi(word.c_str());
		}
		i+=1;
	}


	int allspam=0;
	for (int i=0;i<n;i++)
	allspam+=1-label[i];
	int allham=n-allspam;

	string word;
	for (int i=0;i<n;i++)
	{
		ifstream email(emailnames[i]);
		//cout<<emailnames[i]<<endl;
		while (getline(email,word,'\n'))
		{
			string tt;
			for (int j=0;j<word.length();j++)
			{
				if ((word[j]>='a'&&word[j]<='z')||(word[j]>='A'&&word[j]<='Z')||(word[j]>='0'&&word[j]<='9'))
				{
					tt+=tolower(word[j]);
				}
				else 
				{
					if (stopornot)
					{
						set <string>::iterator it1;
						it1=stopword.find(tt);
						if (it1==stopword.end())
						{
							if (tt.length()>2)
							emails[i][tt]+=1;
						
						}
					}
					else 
					{
						if (tt.length()>2)
						emails[i][tt]+=1;            //ÿ���ʼ���һ��map<string,int>�洢����һ�ؼ��ֺ͵ڶ��ؼ��ֱַ�Ϊĳ�����ʼ��õ����ڸ��ʼ��г��ֵĴ���
						//cout<<tt<<endl;					
					}
					//cout<<tt<<' '<<endl;
					tt="";
				}
			}
		}
		email.clear();
		email.close();
	}
	
	map<string,int>::iterator it;
		
	for (int i=0;i<n;i++)
	{
		for (it=emails[i].begin();it!=emails[i].end();it++)
		{
			wholeset.insert(it->first);
		}
	}
	
	int nwords=wholeset.size();

	for (int wholei=0;wholei<totaltest;wholei++)
	{
		cout<<"��"<<wholei<<"�β���"<<endl;
		memset(fortraining,0,sizeof(fortraining));
		choosetrainingset();                          //��ȫ��4000+���ʼ���ѡ��nsample����Ϊѵ����,nsample�ݶ�Ϊ2000
		int nspam=0,nham=0;
		for (int i=0;i<n;i++)
		{
			if (fortraining[i])
			{
				if (!label[i])
					nspam++;
				else 
					nham++;
			}
		}
		totalnspam+=nspam;
		totalnham+=nham;
		double probforspam=double(nspam)/(nsample);  //�ֱ�����������p(ham)��p(spam)
		double  probforham=double(nham)/(nsample);
		//cout<<probforspam<<" "<<probforham<<endl;
		map<string,int> mapforspam,mapforham;
		int nspamwords=0,nhamwords=0;
		for (int i=0;i<n;i++)
		{
			if (!fortraining[i])
				continue;
			if (!label[i])                                  //!label[i]�������ʼ�,ͳ�������ʼ��е��ܴ�����ÿ�������������ʼ��г��ֵĴ���
			{
				for (it=emails[i].begin();it!=emails[i].end();it++)
				{
					mapforspam[it->first]+=it->second;          //it->first Ϊ������it��Ӧ�ĵ��ʣ�it->secondΪ�õ�����emails[i]���ֵĴ���
					                                            //���õ�����spam�ʼ����ֵ��ܴ������ϸõ�����emails[i]���ֵĴ���
					nspamwords+=it->second;                     //nspamwordsΪѵ�����������ʼ��е��ܴ���
				}
			}
			else       //label[i]�������ʼ���ͳ�������ʼ��е��ܴ�����ÿ�������������ʼ��г��ֵĴ���
			{ 
				for (it=emails[i].begin();it!=emails[i].end();it++)
				{
					mapforham[it->first]+=it->second;
					nhamwords+=it->second;                      //nhamwordsΪѵ�����������ʼ��е��ܴ���
				}
			}

		}


		int error=0,errorh2s=0,errors2h=0;      //�ֱ��¼�������,h2s(ham to spam),s2h(spam to ham)���ֵĴ���
		for (int i=0;i<n;i++)
		{
			if (fortraining[i])
				continue;
			double probofspam=log(probforspam);            //���ڿ��ܴ��ڸ��������磬ʹ��log���м���
			double probofham=log(probforham);
			double totalsp=nspamwords+laplace*nwords;      // ����ÿ������xi,��������P��xi|ci��=xi��ci���ʼ��г��ֵ��ܴ���/ci���ʼ��е��ܴ�����
														   // ���ǵ���Щ���ʿ�����ѵ�����в����ֵ��ڲ��Լ��г��ֶ�������������Ϊ0����Ҫ����������˹ƽ��
														   //�Ӷ�����õ��ʵ��������ʵķ���Ϊxi��ci���г��ֵ��ܴ�����ƽ�����ӣ�
														   //��ĸΪci���ʼ����ܴ�������ƽ������*��ͬ�ʵĸ���

			double totalh=nhamwords+laplace*nwords;
			for (it=emails[i].begin();it!=emails[i].end();it++)  //���ڵ�i���ʼ����������г��ֹ��Ĵ�
			{
					double t1=mapforspam[it->first]+laplace;    //mapforspam[it->first]Ϊ��ǰ�����������ʼ��г��ֵĴ����������������ʵķ���
					double t2=mapforham[it->first]+laplace;     //ͬ��
					for (int ii=0;ii<it->second;ii++)
					{
						probofspam+=log(t1/totalsp);        //t1/totalΪ�õ��ʵ���������P(xi|spam)��Ϊ��ֹ����ʹ���ۼ�log(��������)����������������
						probofham+=log(t2/totalh);          //ͬ��,P(xi|ham)
					}
			}
			int result;
			if (probofspam>probofham)                 //�ȽϺ������Pspam|x)�ͺ������P��ham|x�����ж�����ham����spam
				result=0;
			else result=1;
	
			if (!result&&label[i])
				errorh2s++;
			if (result&&!label[i])
				errors2h++;
			result1[i]=result;

		}
		totalh2s+=errorh2s;
		totals2h+=errors2h;
		double errorrate=double(errorh2s+errors2h)/double(n-nsample)*100;
		double h2srate=(double(errorh2s)/double(allham-nham))*100;
		double s2hrate=(double(errors2h)/double(allspam-nspam))*100;
	
		cout<<"�ܴ�����Ϊ "<<errorrate<<"%"<<endl;
		cout<<"�������ʼ���Ϊ�����ʼ��ĸ���Ϊ "<<h2srate<<"%"<<endl;
		cout<<"�������ʼ���Ϊ�����ʼ��ĸ���Ϊ "<<s2hrate<<"%"<<endl;



	/*	error=0,errorh2s=0,errors2h=0;
		for (int i=0;i<n;i++)
		{
			if (fortraining[i])
				continue;
			double probofspam=0;
			double totalsp=nspamwords+nwords;
			double totalh=nhamwords+nwords;
			multiset<double> tempset;
			for (it=emails[i].begin();it!=emails[i].end();it++)
			{
					double t1=mapforspam[it->first]+1;
					double t2=mapforham[it->first]+1;
					t1=t1/totalsp;
					t2=t2/totalh;
					double answer=(t1)/(t1+t2);
				
					for (int ii=0;ii<it->second;ii++)
					{
						probofspam+=log(1-answer)-log(answer);
					}
			}
		
			int result;
			double finalprob=double(1)/(1+pow(10,probofspam));
			if (finalprob>0.9)
				result=0;
			else 
				result=1;
			result2[i]=result;
			if (!result&&label[i])
				errorh2s++;
			if (result&&!label[i])
				errors2h++;


		}
		errorrate=double(errorh2s+errors2h)/double(n-nsample)*100;
		h2srate=(double(errorh2s)/double(allham-nham))*100;
		s2hrate=(double(errors2h)/double(allspam-nspam))*100;
	
		
		cout<<"�ܴ�����Ϊ "<<errorrate<<"%"<<endl;
		cout<<"�������ʼ���Ϊ�����ʼ��ĸ���Ϊ "<<h2srate<<"%"<<endl;
		cout<<"�������ʼ���Ϊ�����ʼ��ĸ���Ϊ "<<s2hrate<<"%"<<endl;
		*/

	/*	int diff=0;
		errorh2s=0;
		errors2h=0;
		error=0;
		for (int i=0;i<n;i++)
		{
			if (fortraining[i]) continue;
	
			if (result1[i]!=result2[i])
			{
				diff++;
				result1[i]=1;
			}
			if (result1[i]&&!label[i])
				errors2h++;
			if (!result1[i]&&label[i])
				errorh2s++;
		}
		errorrate=double(errorh2s+errors2h)/double(n-nsample)*100;
		h2srate=(double(errorh2s)/double(allham-nham))*100;
		s2hrate=(double(errors2h)/double(allspam-nspam))*100;
		cout<<"�ܴ�����Ϊ "<<errorrate<<"%"<<endl;
		cout<<"�������ʼ���Ϊ�����ʼ��ĸ���Ϊ "<<h2srate<<"%"<<endl;
		cout<<"�������ʼ���Ϊ�����ʼ��ĸ���Ϊ "<<s2hrate<<"%"<<endl;
		cout<<"among these files ,the result of two different bayes :"<<diff<<endl;*/
	}

	cout<<"�ۺ�"<<totaltest<<"��ʵ�飺"<<endl;
	double errorrate=double(totalh2s+totals2h)/double(n-nsample)*100/totaltest;
	double h2srate=(double(totalh2s)/double(totaltest*allham-totalnham))*100;
	double s2hrate=(double(totals2h)/double(totaltest*allspam-totalnspam))*100;
	cout<<"�ܴ�����Ϊ "<<errorrate<<"%"<<endl;
	cout<<"�������ʼ���Ϊ�����ʼ��ĸ���Ϊ "<<h2srate<<"%"<<endl;
	cout<<"�������ʼ���Ϊ�����ʼ��ĸ���Ϊ "<<s2hrate<<"%"<<endl;


	DWORD end=GetTickCount();
	cout<<(double)(end-start)/1000<<"s"<<endl;	
	
	system("pause");
}
