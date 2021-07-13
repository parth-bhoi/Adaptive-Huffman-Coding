#include<bits/stdc++.h>
#include<omp.h>
#include<fstream>
#include<mpi.h>
using namespace std;

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

void add_freq(node* root, char ch)
{
    if(root==NULL)
        return ;
    
    if(root->left==NULL && root->right==NULL)
    {
        if(root->c==ch)
            root->value++;
        
        return ;
    }

    add_freq(root->left,ch);
    add_freq(root->right,ch);
}

void build(node* root)
{
    if(root->left==NULL && root->right==NULL)
        return ;
    
    build(root->left);
    build(root->right);

    if(root->right->value<root->left->value)
    {
        node* t=new node;
        t=root->left;
        root->left=root->right;
        root->right=t;
    }

    root->value=root->left->value+root->right->value;

    return ;
}

void swap(node* root)
{
    if(root->left==NULL && root->right==NULL)
        return ;
    
    swap(root->left);
    swap(root->right);

    if(root->right->value<root->left->value)
    {
        node* t=new node;
        t=root->left;
        root->left=root->right;
        root->right=t;
    }

    return ;
}

string code(string temp, int e, int r, char ch)
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
    temp+=fixed_code;

    return temp;
}

void numbering(node* root)
{
    queue<node*> q;
    int v=256;
    q.push(root);

    while(!q.empty())
    {
        int sz=q.size();
        while(sz--)
        {
            node* t=q.front();
            q.pop();
            t->node_number=v;
            v--;

            if(t->right!=NULL)
            {
                q.push(t->right);
                q.push(t->left);
            }
        }
    }
}

void print(node* root)
{
    queue<node*> q;
    q.push(root);

    while(!q.empty())
    {
        int sz=q.size();
        while(sz--)
        {
            node* t=q.front();
            q.pop();
            cout<<t->c<<" "<<t->value<<" "<<t->node_number<<" "<<t->isNYT<<endl;
            if(t->left!=NULL)
                q.push(t->left);
            if(t->right!=NULL)
                q.push(t->right);
        }
        cout<<endl;
    }
}

string Encoder(string &s, int &e, int &r, int start, int end)
{
    string encoded_string;
    node* root = new node;
    root->node_number=256;
    root->value=0;
    root->c='\0';
    root->isNYT=true;
    root->left=NULL;
    root->right=NULL;

    vector<bool> m(128,false);

    int i;

    for(i=start;i<=end;i++)
    {
        string temp;
        
        if(m[s[i]-'\0']==true)
        {
            int isPresent=find(root,s[i],temp);
            //add_freq(root,s[i]);
            //build(root);
            //swap(root);
            //numbering(root);
            //temp.clear();
            //int zz=find(root,s[i],temp);
            reverse(temp.begin(),temp.end());
            encoded_string+=temp;
        }
        else
        {
            temp.clear();
            int z=NYTCode(root,temp,s[i]);
            //build(root);
            //swap(root);
            //numbering(root);
        
            reverse(temp.begin(),temp.end());
            string fixed_code=code(temp,e,r,s[i]);
            encoded_string+=fixed_code;
        }
        m[s[i]-'\0']=true;
        //cout<<encoded_string<<endl;
    }

    m.clear();
    //print(root);

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
            //cout<<r->c<<endl;
            //tnode=root->right;
            //cout<<tnode->c<<endl;

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

string Decoder(string &encoded, int &e, int &r)
{
    string decoded;
    //vector<bool> m(128,false);
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
            //string temp;
            // int z=NYTCode(root,temp,nw);
            //cout<<tnode->c<<endl;
            tnode->c=nw;
            //build(root);
            //swap(root);
            //numbering(root);
        }
        else
        {
            decoded+=dec;
            //add_freq(root,dec);
            //build(root);
            //swap(root);
            //numbering(root);
            
        }
        //m[dec-'\0']=true;
        
    }
    
    return decoded;
}

int main(int argc, char** argv) 
{
    srand(5);
    // Initialize variables
    // *********************************************
    int my_rank, nprocs;
    float begin, finish, total;
    int tot_size = 0;
    int e=6,r=64;
    int start,end;
    string s,t; 
    // ifstream MyReadFile("bonsai.txt");
    // while (getline (MyReadFile,t)) 
    // {
    //     s+=t;
    // }
    long long n = 10e9;
    for(int i = 0; i < n; i++){
        s += rand()%128;
    }
    int len = s.length();
    // *********************************************



    // MPI Initialize
    // *********************************************
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


    // Start Measuring Time
    if(my_rank == 0)
        begin=omp_get_wtime();
    // *********************************************
    
    
    
    // Overhead to manage message length distribution
    // *********************************************
    vector<string> final(nprocs);
    
    int part = (len/nprocs);

    start = part*my_rank;
    if(my_rank == nprocs - 1){
        end = len - 1;
    }
    else{end = (my_rank + 1)*part - 1;}
    
    // Encoding messages
    // *********************************************
    string encoded=Encoder(s,e,r, start, end);
    // *********************************************

    // Recieving and sending Messages
    // *********************************************
    if(my_rank == 0){
        final[0] = encoded;
        tot_size += encoded.size();
        cout<<"Input length: "<<s.length()<<endl; 
        
        int i = 0;
        for(i = 1; i < nprocs; i++){
            int number_amount;
            MPI_Status status;
            MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_CHAR, &number_amount);
            char* number_buf = (char*)malloc(sizeof(char) * (number_amount));
            
            MPI_Recv(number_buf, number_amount, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
            
            string rec = "";
            int j = 0;
            for(j = 0; j < number_amount; j++){
                rec += number_buf[j];
            }
            tot_size += number_amount;
            
            final[i] = rec;
            
            free(number_buf);
        }
    }
    else{
        MPI_Send(encoded.c_str(), encoded.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    // End measuring time
    if(my_rank == 0)
        finish=omp_get_wtime();
    MPI_Finalize();
    // *********************************************


    
    // Decoding and checking the encoded message
    // *********************************************
    if(my_rank == 0){
        string dec = "";
        cout << "Encoder time " << finish - begin << endl;
        int i = 0;
        cout << tot_size << endl;
        begin=omp_get_wtime();
        for(i = 0; i < nprocs; i++){
            dec += Decoder(final[i],e,r);
        }
        finish=omp_get_wtime();

        total=finish-begin;

        cout<<"Decoder Time: "<<total<<endl;
        if(s==dec)
            cout<<"Le Betaa"<<endl;
        else{cout << "NOPE" << endl;}
    }
    // *********************************************

    return 0;
}