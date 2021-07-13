#include<bits/stdc++.h>
#include<fstream>
#include<omp.h>
using namespace std;

vector<string> fixedcode(128);

struct node
{
    int node_number;
    int value;
    char c;
    bool isNYT;
    node* left;
    node* right;
};

bool find(node* root, char ch, string &temp)
{
    if(root==NULL)
        return false;
    
    if(root->left==NULL && root->right==NULL)
    {
        if(root->isNYT==false && root->c==ch)
        {
            root->value++;
            return true;
        }
        else 
            return false;
    }
    
    if(find(root->left,ch,temp))
    {
        temp+='0';
        if(root->right->value<root->left->value)
        {
            node* t=new node;
            t=root->left;
            root->left=root->right;
            root->right=t;
        }
        root->value=root->left->value+root->right->value;
        return true;
    }

    if(find(root->right,ch,temp))
    {
        temp+='1';
        if(root->right->value<root->left->value)
        {
            node* t=new node;
            t=root->left;
            root->left=root->right;
            root->right=t;
        }
        root->value=root->left->value+root->right->value;
        return true;
    }

    return false;
}

bool NYTCode(node* root, string &temp, char ch)
{
    if(root==NULL)
        return false;
    
    if(root->left==NULL && root->right==NULL && root->isNYT==true)
    {
        root->isNYT=false;

        node* l = new node;
        node* r = new node;

        l->c='\0';
        l->isNYT=true;
        l->left=NULL;
        l->node_number=1;
        l->right=NULL;
        l->value=0;

        r->c=ch;
        r->isNYT=false;
        r->left=NULL;
        r->node_number=1;
        r->right=NULL;
        r->value=1;

        root->left=l;
        root->right=r;
        root->value=1;
        return true;
    }
    
    if(NYTCode(root->left,temp,ch))
    {
        temp+='0';
        if(root->right->value<root->left->value)
        {
            node* t=new node;
            t=root->left;
            root->left=root->right;
            root->right=t;
        }
        root->value=root->left->value+root->right->value;

        return true;
    }
    if(NYTCode(root->right,temp,ch))
    {
        temp+='1';
        if(root->right->value<root->left->value)
        {
            node* t=new node;
            t=root->left;
            root->left=root->right;
            root->right=t;
        }
        root->value=root->left->value+root->right->value;

        return true;
    }

    return false;
}

string code(int e, int r, char ch)
{
    int k,bits;

    k=ch-'\0';

    if(k>=0 && k<=2*r)
    {
        k--;
        bits=e+1;
    }
    else
    {
        k=k-r-1;
        bits=e;
    }
    string fixed_code;
    while(bits>0)
    {
        if(k%2==0)
            fixed_code+='0';
        else
            fixed_code+='1';
        k=k/2;
        bits--;
    }
            
    reverse(fixed_code.begin(),fixed_code.end());
    return fixed_code;
    // temp+=fixed_code;

    // return temp;
}

vector<string> Encoder(string &s, int &e, int &r, int &p)
{
    
    vector<string> encoded_string(p);
    omp_set_num_threads(p);

    #pragma omp parallel            // parallel region 
    {
        int me=omp_get_thread_num();
        int nt=omp_get_num_threads();
        
        
        node* root = new node;
        root->node_number=256;
        root->value=0;
        root->c='\0';
        root->isNYT=true;
        root->left=NULL;
        root->right=NULL;


        int n=s.length();
        int block=n/p;      // divide input string in blocks of size n/p, input-decomposition
        int i;
        string encoded_me;
        vector<bool> m(128,0);
        if(me!=p-1)
        {
            for(i=me*block;i<(me+1)*block;i++)
            {
                char mine;
                mine=s[i];

                string temp;
                
                if(m[mine-'\0']==true)
                {
                    int isPresent=find(root,mine,temp);
                    reverse(temp.begin(),temp.end());
                    encoded_me+=temp;
                }
                else
                {
                    temp.clear();
                    int z=NYTCode(root,temp,mine);
                    reverse(temp.begin(),temp.end());
                    temp+=fixedcode[mine-'\0'];
                    encoded_me+=temp;
                }
                m[mine-'\0']=true;
            }
        }
        else
        {
            for(i=me*block;i<n;i++)
            {
                char mine;
                mine=s[i];

                string temp;
                
                if(m[mine-'\0']==true)
                {
                    int isPresent=find(root,mine,temp);
                    reverse(temp.begin(),temp.end());
                    encoded_me+=temp;
                }
                else
                {
                    temp.clear();
                    int z=NYTCode(root,temp,mine);
                    reverse(temp.begin(),temp.end());
                    temp+=fixedcode[mine-'\0'];
                    
                    encoded_me+=temp;
                }
                m[mine-'\0']=true;
            }
        }

        #pragma omp critical        //critical region to collect encoded string from each procs
        {
            encoded_string[me]=encoded_me;
        }

        m.clear();
    }

    return encoded_string;
}

char find_leaf(string &encoded, int &i, node* root, node* &tnode)
{
    if(root->left==NULL && root->right==NULL)
    {
        if(root->isNYT==false)
        {
            root->value++;
            return root->c;
        }
        else
        {
            root->isNYT=false;

            node* l = new node;
            node* r = new node;

            l->c='\0';
            l->isNYT=true;
            l->left=NULL;
            l->node_number=1;
            l->right=NULL;
            l->value=0;

            r->c='k';
            r->isNYT=false;
            r->left=NULL;
            r->node_number=1;
            r->right=NULL;
            r->value=1;
            tnode=r;

            root->left=l;
            root->right=r;
            root->value=1;

            return '\0';
        }
    }
    if(encoded[i]=='0')
    {  
        i++;
        char k=find_leaf(encoded,i,root->left,tnode);
        if(root->right->value<root->left->value)
        {
            node* t=new node;
            t=root->left;
            root->left=root->right;
            root->right=t;
        }
        root->value=root->left->value+root->right->value;

        return k;
    }
    else
    {
        i++;
        char k=find_leaf(encoded,i,root->right,tnode);
        if(root->right->value<root->left->value)
        {
            node* t=new node;
            t=root->left;
            root->left=root->right;
            root->right=t;
        }
        root->value=root->left->value+root->right->value;

        return k;
    }
    if(root->right->value<root->left->value)
    {
        node* t=new node;
        t=root->left;
        root->left=root->right;
        root->right=t;
    }
    root->value=root->left->value+root->right->value;

    return '\0';
}

int calculate(string &encoded, int &i, int &e)
{
    int j=0;
    int num=0;
    while(j<e)
    {
        if(encoded[i]!='0')
        {
            num+=pow(2,e-j-1);
        }
        i++;
        j++;
    }
    return num;
}

string Decoder(vector<string> &encoded, int &e, int &r, int &p)
{
    vector<string> decoded(p);

    #pragma omp parallel            //parallel region each procs decode the encoded string or corresponding procs of encoder
    {

        int me=omp_get_thread_num();

        node* root = new node;
        root->node_number=256;
        root->value=0;
        root->c='\0';
        root->isNYT=true;
        root->left=NULL;
        root->right=NULL;

        node* tnode=root;
        int i=0;
        int n=encoded[me].length();

        char dec;
        while(i<n)
        {
            dec=find_leaf(encoded[me],i,root,tnode);
            if(dec=='\0')
            {
                int num=calculate(encoded[me],i,e);
                char nw;
                if(num<r)
                {
                    num=num*2;
                    if(encoded[me][i]=='1')
                        num++;
                    
                    num++;
                    nw=num+'\0';
                    i++;
                    decoded[me]+=nw;
                }
                else
                {
                    num=num+r+1;
                    nw=num+'\0';
                    decoded[me]+=nw;
                }
                tnode->c=nw;
            }
            else
                decoded[me]+=dec;
        }
    }

    string ans;
    int i;

    for(i=0;i<p;i++)        // add the decoded string of each procs to final string
    {
        ans+=decoded[i]; 
    }
        

    return ans;
}

int main() 
{
    int e=6,r=64,p=16;       // p=Number of procs
    cout<<p<<endl;

    fixedcode.clear();
    for(int i=0;i<128;i++)
    {
        fixedcode[i]=code(e,r,i+'\0');
    }

    string s,t; 

    ifstream MyReadFile("1000.txt");
    while (getline (MyReadFile,t)) 
    {
        s+=t;
    }
    cout<<"Input length: "<<s.length()<<endl;

    float start,end,total;

    start=omp_get_wtime();
    vector<string> encoded=Encoder(s,e,r,p);
    end=omp_get_wtime();

    total=end-start;

    int len=0;
    for(int i=0;i<encoded.size();i++)
        len+=encoded[i].length();

    cout<<"Encoded Length: "<<len<<endl;
    cout<<"Encoder Time: "<<total<<endl;
    
    start=omp_get_wtime();
    string decoded=Decoder(encoded,e,r,p);
    end=omp_get_wtime();

    total=end-start;

    cout<<"Decoder Time: "<<total<<endl;

    
    if(s==decoded)
        cout<<"Message Decoded Successfully"<<endl;

    fixedcode.clear();
    return 0;
}
