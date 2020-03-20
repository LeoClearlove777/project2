#pragma once
#include <iostream>
using namespace std;
#include <vector>
#include <algorithm>
#include <sstream>
class Util
{
	public:
		static void StringTolower(string& s)
		{
			transform(s.begin(),s.end(),s.begin(),::tolower);
		}
		static void StringToUpper(string& s)
		{
			transform(s.begin(),s.end(),s.begin(),::toupper);
		}
		static void TransformToVector(string& s,vector<string>& v)
		{
			size_t start=0;
			while(1)
			{
				size_t pos=s.find("\n",start);
				if(string::npos==pos)
				{
					break;
				}
				string sub_str=s.substr(start,pos-start);
				v.push_back(sub_str);
				start=pos+1;
			}
		}
		static void MakeKV(string s ,string& key,string& value)
		{
			size_t pos=s.find(": ",0);
			if(string::npos==pos)
				return;
			key=s.substr(0,pos);
			value=s.substr(pos+2);//冒号空格占用两个字符
		}
		static int StringToInt(string s)
		{
			stringstream ss(s);
			int result=0;
			ss>>result;
			return result;
		}
		static string IntToString(int i)
		{
			stringstream ss;
			ss<<i;
			return ss.str();
		}
		static string CodeToDec(int code)
		{
			switch(code)
			{
				case 200:
					return "OK";
				case 404:
					return "NOTF FOUND";
				default:
					return "Unknow";
			}
		}
		static string SuffixToType(const string& suffix)
		{
			string ct="Content-Type: ";
			if(suffix==".html")
				ct+="text/html";
			else if(suffix==".css")
				ct+="text/css";
			else if(suffix==".js")
				ct+="application/x-javascript";
			else if(suffix==".jpg")
				ct+="image/jpeg";
			else if(suffix==".png")
				ct+="image/png";
			else if(suffix==".mp4")
				ct+="video/mpeg4";
			else
				ct+="text/css";
			return ct;
		}
};
