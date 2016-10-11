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

map<string,int> emails[5000];  //每封邮件对应该map数组中的一个map,每个在第i封邮件中出现的单词分别用一个<string,int>来表示,
							   //第一个键值表示该单词，第二个键值表示该单词出现的次数

set <string>wholeset;          //存储所有在邮件中出现的单词
int label[5000];               //表示邮件标签,label[i]=1则第i封邮件为正常邮件，否则为垃圾邮件
bool fortraining[5000];        //用于选择训练集，fortraining[i]=1表示第i封邮件此次实验中被作为训练集
#define stopornot 0            //用于控制是否去掉英文中常用词

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
	cout<<"请选择使用50封邮件的小数据集或是4327封邮件的大数据集：(0:小样本,1:大样本)"<<endl;
	cin>>smallorbig;
	cout<<"请选择训练样本数量"<<endl;
	cin>>nsample;
	cout<<"请选择平滑因子"<<endl;
	cin>>laplace;
	cout<<"请选择测试次数（推荐至少10次,偶然性较少）"<<endl;
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
						emails[i][tt]+=1;            //每封邮件以一个map<string,int>存储，第一关键字和第二关键字分别为某个单词及该单词在该邮件中出现的次数
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
		cout<<"第"<<wholei<<"次测试"<<endl;
		memset(fortraining,0,sizeof(fortraining));
		choosetrainingset();                          //从全部4000+封邮件中选择nsample封作为训练集,nsample暂定为2000
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
		double probforspam=double(nspam)/(nsample);  //分别计算先验概率p(ham)和p(spam)
		double  probforham=double(nham)/(nsample);
		//cout<<probforspam<<" "<<probforham<<endl;
		map<string,int> mapforspam,mapforham;
		int nspamwords=0,nhamwords=0;
		for (int i=0;i<n;i++)
		{
			if (!fortraining[i])
				continue;
			if (!label[i])                                  //!label[i]即垃圾邮件,统计垃圾邮件中的总词数和每个单词在垃圾邮件中出现的次数
			{
				for (it=emails[i].begin();it!=emails[i].end();it++)
				{
					mapforspam[it->first]+=it->second;          //it->first 为迭代器it对应的单词，it->second为该单词在emails[i]出现的次数
					                                            //将该单词在spam邮件出现的总次数加上该单词在emails[i]出现的次数
					nspamwords+=it->second;                     //nspamwords为训练集中垃圾邮件中的总词数
				}
			}
			else       //label[i]即正常邮件，统计正常邮件中的总词数和每个单词在正常邮件中出现的次数
			{ 
				for (it=emails[i].begin();it!=emails[i].end();it++)
				{
					mapforham[it->first]+=it->second;
					nhamwords+=it->second;                      //nhamwords为训练集中正常邮件中的总词数
				}
			}

		}


		int error=0,errorh2s=0,errors2h=0;      //分别记录分类错误,h2s(ham to spam),s2h(spam to ham)出现的次数
		for (int i=0;i<n;i++)
		{
			if (fortraining[i])
				continue;
			double probofspam=log(probforspam);            //由于可能存在浮点数下溢，使用log进行计算
			double probofham=log(probforham);
			double totalsp=nspamwords+laplace*nwords;      // 对于每个单词xi,条件概率P（xi|ci）=xi在ci类邮件中出现的总次数/ci类邮件中的总词数，
														   // 考虑到有些单词可能在训练集中不出现但在测试集中出现而导致条件概率为0，需要进行拉普拉斯平滑
														   //从而计算该单词的条件概率的分子为xi在ci类中出现的总词数加平滑因子，
														   //分母为ci类邮件的总词数加上平滑因子*不同词的个数

			double totalh=nhamwords+laplace*nwords;
			for (it=emails[i].begin();it!=emails[i].end();it++)  //对于第i封邮件，遍历所有出现过的词
			{
					double t1=mapforspam[it->first]+laplace;    //mapforspam[it->first]为当前单词在垃圾邮件中出现的次数，计算条件概率的分子
					double t2=mapforham[it->first]+laplace;     //同上
					for (int ii=0;ii<it->second;ii++)
					{
						probofspam+=log(t1/totalsp);        //t1/total为该单词的条件概率P(xi|spam)，为防止下溢使用累加log(条件概率)代替连乘条件概率
						probofham+=log(t2/totalh);          //同上,P(xi|ham)
					}
			}
			int result;
			if (probofspam>probofham)                 //比较后验概率Pspam|x)和后验概率P（ham|x），判定属于ham或者spam
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
	
		cout<<"总错误率为 "<<errorrate<<"%"<<endl;
		cout<<"将正常邮件分为垃圾邮件的概率为 "<<h2srate<<"%"<<endl;
		cout<<"将垃圾邮件分为正常邮件的概率为 "<<s2hrate<<"%"<<endl;



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
	
		
		cout<<"总错误率为 "<<errorrate<<"%"<<endl;
		cout<<"将正常邮件分为垃圾邮件的概率为 "<<h2srate<<"%"<<endl;
		cout<<"将垃圾邮件分为正常邮件的概率为 "<<s2hrate<<"%"<<endl;
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
		cout<<"总错误率为 "<<errorrate<<"%"<<endl;
		cout<<"将正常邮件分为垃圾邮件的概率为 "<<h2srate<<"%"<<endl;
		cout<<"将垃圾邮件分为正常邮件的概率为 "<<s2hrate<<"%"<<endl;
		cout<<"among these files ,the result of two different bayes :"<<diff<<endl;*/
	}

	cout<<"综合"<<totaltest<<"次实验："<<endl;
	double errorrate=double(totalh2s+totals2h)/double(n-nsample)*100/totaltest;
	double h2srate=(double(totalh2s)/double(totaltest*allham-totalnham))*100;
	double s2hrate=(double(totals2h)/double(totaltest*allspam-totalnspam))*100;
	cout<<"总错误率为 "<<errorrate<<"%"<<endl;
	cout<<"将正常邮件分为垃圾邮件的概率为 "<<h2srate<<"%"<<endl;
	cout<<"将垃圾邮件分为正常邮件的概率为 "<<s2hrate<<"%"<<endl;


	DWORD end=GetTickCount();
	cout<<(double)(end-start)/1000<<"s"<<endl;	
	
	system("pause");
}
