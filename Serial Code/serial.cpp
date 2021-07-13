#include<bits/stdc++.h>
#include<omp.h>
#include<fstream>
using namespace std;

vector<string> fixedcode(128);          //store fixed-code for each symbol


//************TreeNode**************
struct node
{
    int node_number;
    int value;
    char c;
    bool isNYT;
    node* left;
    node* right;
};


// Function to build the huffman path of required symbol and update the tree
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


// Function to build the huffman path of the NYT node and update the tree
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


// Function to calculate fixed code for any symbol
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
}


// Encoder Function (at transmitter's side) takes input the string to be encoded and parameters e and r
string Encoder(string &s, int &e, int &r)
{
    string encoded_string;
    node* root = new node;
    root->node_number=256;
    root->value=0;
    root->c='\0';
    root->isNYT=true;
    root->left=NULL;
    root->right=NULL;

    vector<bool> m(128,false);      // maps true the already transmitted symbol

    int n=s.length();
    int i;

    for(i=0;i<n;i++)
    {
        string temp;
        
        if(m[s[i]-'\0']==true)
        {
            int isPresent=find(root,s[i],temp);
            reverse(temp.begin(),temp.end());
            encoded_string+=temp;
        }
        else
        {
            temp.clear();
            int z=NYTCode(root,temp,s[i]);
            reverse(temp.begin(),temp.end());
            temp+=fixedcode[s[i]-'\0'];
            encoded_string+=temp;
        }
        m[s[i]-'\0']=true;
    }

    m.clear();

    return encoded_string;
}


// Function to decode the encoded-bits to corresponding symbol and update the tree at receiver's end
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


// Function to decode the symbol from bit string, e and r
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


// Decoder function (at receiver's end) takes input the string to be decoded and parameters e and r
string Decoder(string &encoded, int &e, int &r)
{
    string decoded;
    node* root = new node;
    root->node_number=256;
    root->value=0;
    root->c='\0';
    root->isNYT=true;
    root->left=NULL;
    root->right=NULL;

    int i=0;
    int n=encoded.length();

    char dec;
    node* tnode = root;

    while(i<n)
    {
        dec=find_leaf(encoded,i,root,tnode);
        if(dec=='\0')
        {
            int num=calculate(encoded,i,e);
            
            char nw;
            if(num<r)
            {
                num=num*2;
                if(encoded[i]=='1')
                    num++;
                
                num++;
                nw=num+'\0';
                i++;
                decoded+=nw;
            }
            else
            {
                num=num+r+1;
                nw=num+'\0';
                decoded+=nw;
            }
            tnode->c=nw;
        }
        else
            decoded+=dec;
    }
    
    return decoded;
}


int main() 
{
    int e=6,r=64;       // for ASCII symbol-set 

    fixedcode.clear();
    for(int i=0;i<128;i++)
    {
        fixedcode[i]=code(e,r,i+'\0');
    }

    string s,t; 

    ifstream MyReadFile("10000.txt");       //file-name
    while (getline (MyReadFile,t)) 
    {
        s+=t;
    }
    cout<<"Input length: "<<s.length()<<endl;

    float start,end,total;

    start=omp_get_wtime();
    string encoded=Encoder(s,e,r);
    end=omp_get_wtime();

    total=end-start;
    cout<<"Encoded Length: "<<encoded.length()<<endl;
    cout<<"Encoder Time: "<<total<<endl;
    
    start=omp_get_wtime();
    string decoded=Decoder(encoded,e,r);
    end=omp_get_wtime();

    total=end-start;

    cout<<"Decoder Time: "<<total<<endl;

    
    if(s==decoded)
        cout<<"Message Decoded Successfully"<<endl;

    fixedcode.clear();
    return 0;
}
