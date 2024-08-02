
#include "transf.h"

int transfer(string filename,unsigned cho)
{
	ifstream in;
//	getline(cin,filename,'\n');
	
	in.open(filename.c_str());
	if(!in){
		cerr<<"open file failed!"<<endl;
        exit(1);
	}
	string trans=filename+".w";
	ofstream out;
	out.open(trans.c_str(),ios_base::out|ios_base::binary);//д��/������ 
	if(!out){
		cerr<<"open binary file failed!"<<endl;
        exit(1);
	}
	
	cout<<"Select modify format:"<<endl;
	cout<<"A: \ngraph_for_greach \n3 \n0: 1 2 # "<<endl;
	cout<<"B: \n3 \n0 1 2 "<<endl;
	cout<<"C: \n3 \n0: 1 2 -1"<<endl;
	cout<<"D: \nv e \n i degree 1 2 3"<<endl;
	
	cout<<"\n1  A to B     2  B to A"<<endl;
	cout<<"\n3  A to C     4  C to A"<<endl;
	cout<<"\n5  A to D     6  D to A"<<endl;
	cout<<"\n7  B to C     8  C to B"<<endl;
	cout<<"\n9  B to D     10 D to B"<<endl;
	cout<<"\n11 C to D     12 D to C"<<endl;
	
	while(cho<1||cho>12){
		cout<<"Please select again"<<endl;
		cin>>cho;
	}
//      A                B               C
//graph_for_greach     3               3
//3                    0 1 2 /         0: 1 2 -1/
//0: 1 2 #/
//testfile: A yago_0.gra B human.gr
	char sp=10;
	string temp;
	char c;

	switch (cho)
	{
		case 1 :
			cout<<"1 A to B"<<endl;
			getline(in,temp);
			getline(in,temp);
			out<<temp<<"\n";
			
			while(in.peek()!=EOF)
			{
				string ver1;
				in>>ver1;
				ver1[ver1.size()-1]='\0';
				out<<ver1;

				while(1)
				{
					in.get();
					if(in.get()=='#') {
						in.get();
						break;
					}
					else in.seekg(-1,ios::cur);

					in>>ver1;
					out<<ver1<<" ";
				}
				out<<"\n";
			}
			break;
		case 2 :
            cout<<"2 B to A"<<endl;
            cout<<"*"<<sp<<"*"<<endl;
            out<<"graph_for_greach"<<sp;
            getline(in,temp);
            out<<temp<<"\n";
            while(in.peek()!=EOF)
            {
                string ver2;
                in>>ver2;
                out<<ver2<<":";
                while(1)
                {
                    c=in.get();

                    if(c=='\n')
                    {
                        break;
                    }
                    if(c==' ')
                    {
                        continue;
                    }
                    else
                    {
                        in.seekg(-1,ios::cur);
                    }
                    in>>ver2;
                    out<<" "<<ver2;
                }
                out<<" #\n";
            }
			break;
		case 3:
			cout<<"3 A to C"<<endl;
			getline(in,temp);
			getline(in,temp);
			out<<temp<<"\n";
			while(in.peek()!=EOF)
			{
				string ver3;
				in>>ver3;
				out<<ver3<<" ";

				while(1)
				{
					in.get();
					if(in.get()=='#') {
						in.get();
						break;
					}
					else in.seekg(-1,ios::cur);

					in>>ver3;
					out<<ver3<<" ";//cout<<ver3<<"*";
				}
				out<<"-1\n";
			}
			break;
		case 4:
			cout<<"4 C to A"<<endl;
			getline(in,temp);
			out<<"graph_for_greach"<<sp;
			out<<temp<<"\n";
			while(in.peek()!=EOF)
			{
				string ver4;
				in>>ver4;
				out<<ver4<<" ";
				while(1)
				{
					in.get();
					if(in.get()=='-') {
						in.get();
						in.get();
						break;
					}
					else in.seekg(-1,ios::cur);

					in>>ver4;
					out<<ver4<<" ";
				}
				out<<"#\n";
			}
			break;
		case 5:
            cout<<"7 B to C"<<endl;
            getline(in,temp);
            out<<temp<<"\n";

            while(in.peek()!=EOF)
            {
                string ver7;
                in>>ver7;
                out<<ver7<<":";
                while(1)
                {
                    c=in.get();

                    if(c=='\n')
                    {
                        break;
                    }
                    if(c==' ')
                    {
                        continue;
                    }
                    else
                    {
                        in.seekg(-1,ios::cur);
                    }

                    in>>ver7;
                    out<<" "<<ver7;
                }
                out<<" -1\n";
            }
			break;
		case 6:
			cout<<"8 C to B"<<endl;
			getline(in,temp);
			out<<temp<<"\n";
			while(in.peek()!=EOF)
			{
				string ver8;
				in>>ver8;
				ver8[ver8.size()-1]='\0';
				out<<ver8;

				while(1)//&&t<1000
				{
					in.get();
					if(in.get()=='-') {
						in.get();
						in.get();
						break;
					}
					else in.seekg(-1,ios::cur);

					in>>ver8;
					out<<ver8<<" ";
				}
				out<<"\n";
			}
			break;
		default :
			break;
	}

	cout<<"Modification complete."<<endl; 
    return 0;
}
