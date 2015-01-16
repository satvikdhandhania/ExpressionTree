/*
 * Author: Asim Ali [11BCE0139]
 *         Satvik Dhandhania[11BCE0431]
 * DATE: 5/4/2014 4:55 PM IST
 */

#include <new>
#include <map>
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>

#include <ogdf/basic/Graph.h> 
#include <ogdf/basic/Graph_d.h>
#include <ogdf/tree/TreeLayout.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/DfsAcyclicSubgraph.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>

using namespace ogdf;

std::vector<string> v;
std::map<string, int> mp;
std::map<int, char> rev;
int flag, n=1, adj[111][111], vis[111], syn[111][111];

struct Node
{
    Node *left, *right;
    char val;
    string inOrder;
    int done, num;
    Node()
    {
        done=0;
        num=n++;
    }
}*head, *ptr, *lnk;

bool cmp(string a, string b)
{
    return a.length()>=b.length();
}

bool delim(char c)
{
    return c==' ';
}

bool isOperator(char c)
{
    return c=='+' || c=='-' || c=='/' || c=='*' || c=='%' || c=='^';
}

int priority(char op)
{
    if(op=='+' || op=='-')
        return 1;
    if(op=='*' || op=='/' || op=='%' || op=='^')
        return 2;
    return -1;
}

void processOperator(std::vector<Node*> &st, char op)
{
    Node *r, *l;
    r=new Node;
    l=new Node;
    r=st.back(); st.pop_back();
    l=st.back(); st.pop_back();
    rev[r->num]=r->val;
    rev[l->num]=l->val;
    ptr=new Node;
    ptr->left=l, ptr->right=r;
    ptr->val=op;
    rev[ptr->num]=op;
    st.push_back(ptr);
}

bool left_associativity(char op)
{
    if(op=='^')
        return false;
    else
        return true;
}

Node* calc(string &s)
{
    std::vector<Node*> st;
    std::vector<char> op;
    st.clear(), op.clear();
    int i;
    for(i=0;i<(int)s.length();++i)
    {
        if(!delim(s[i]))
        {
            if(s[i]=='(')
                op.push_back('(');
            else
            if(s[i]==')')
            {
                while(op.back()!='(')
                    processOperator(st, op.back()), op.pop_back();
                op.pop_back();
            }
            else
            if(isOperator(s[i]))
            {
                while(!op.empty() && (left_associativity(s[i]) && priority(op.back())>=priority(s[i]) || !left_associativity(s[i]) && priority(op.back())>priority(s[i])))
                    processOperator(st, op.back()), op.pop_back();
                op.push_back(s[i]);
            }
            else
            {
                ptr=new Node;
                ptr->val=s[i];
                ptr->left=ptr->right=NULL;
                rev[ptr->num]=s[i];
                st.push_back(ptr);
            }
        }
    }
    while(!op.empty())
        processOperator(st, op.back()), op.pop_back();
    return st.back();
}

void Calculate_Inorder(Node *tmp)
{
    string t="";
    if(tmp->left!=NULL)
    {
        Calculate_Inorder(tmp->left);
        syn[tmp->num][tmp->left->num]=1;
        t+=tmp->left->inOrder;
        printf("Parent = %c, Left Child = %c\n", tmp->val, tmp->left->val);
    }
    t+=tmp->val;
    if(tmp->right!=NULL)
    {
        syn[tmp->num][tmp->right->num]=1;
        Calculate_Inorder(tmp->right);
        t+=tmp->right->inOrder;
        printf("Parent = %c, Right Child = %c\n", tmp->val, tmp->right->val);
    }
    tmp->inOrder=t;
    v.push_back(t);
    if(mp.count(t)==0)
        mp[t]=1;
    else
        mp[t]++;
}

void search(Node *tmp, string s)
{
    if(tmp->inOrder.compare(s)==0)
    {
        flag=1;
        lnk=tmp;
        return;
    }
    if(tmp->left!=NULL)
    {
        search(tmp->left, s);
        if(flag)
            return;
    }
    if(tmp->right!=NULL)
    {
        search(tmp->right, s);
        if(flag)
            return;
    }
    if(flag)
        return;
}

void free_memory(Node *tmp)
{
    if(tmp->left!=NULL)
        free_memory(tmp->left);
    if(tmp->right!=NULL)
        free_memory(tmp->right);
    free(tmp);
}

void link(Node *tmp)
{
    if(tmp->left!=NULL)
    {
        if(tmp->left->done==0 && tmp->left->inOrder.compare(lnk->inOrder)==0)
        {
            free_memory(tmp->left);
            tmp->left=lnk;
        }
        else
            link(tmp->left);
    }
    if(tmp->right!=NULL)
    {
        if(tmp->right->done==0 && tmp->right->inOrder.compare(lnk->inOrder)==0)
        {
            free_memory(tmp->right);
            tmp->right=lnk;
        }
        else
            link(tmp->right);   
    }
}

void Create_Adjacency(Node *tmp)
{
    if(tmp->left!=NULL)
    {
        adj[tmp->num][tmp->left->num]++;
        if(!vis[tmp->left->num])
        {
            Create_Adjacency(tmp->left);
        }
    }
    if(tmp->right!=NULL)
    {
        adj[tmp->num][tmp->right->num]++;
        if(!vis[tmp->right->num])
        {
            Create_Adjacency(tmp->right);
        }
    }
    vis[tmp->num]=1;
}

int main()
{
    string s;
    cin>>s;
    head=calc(s);
    v.clear(), mp.clear();
    Calculate_Inorder(head);
    sort(v.begin(), v.end(), cmp);
    for(int i=0;i<(int)v.size();++i)
    {
        if(mp[v[i]]>1)
        {
            flag=0;
            search(head, v[i]);
            lnk->done=1;
            link(head);
            lnk->done=0;
            mp[v[i]]=0;
        }
    }
    Create_Adjacency(head);

    Graph T;
    std::map <int, node> Gmap1;
    std::map <int, int> Gdeg1;

    for(int i=0; i<n; i++){
        Gmap1[i] = T.newNode();
    }

    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(syn[i][j]){
                int k = syn[i][j];
                while(k--){
                    T.newEdge(Gmap1[i],Gmap1[j]);
                    Gdeg1[i]++;
                    Gdeg1[j]++;
                }
            }
        }
    }

    for(int i=0; i<n; i++){
        if(!Gdeg1[i]){
            T.delNode(Gmap1[i]);
        }
    }

    GraphAttributes GA1( T, GraphAttributes::nodeGraphics |
        GraphAttributes::edgeGraphics |
        GraphAttributes::nodeLabel |
        GraphAttributes::nodeStyle |
        GraphAttributes::edgeType |
        GraphAttributes::edgeArrow |
        GraphAttributes::edgeStyle );

    node u;
    forall_nodes( u, T ){
        GA1.fillColor( u ) = Color( "#FFFF00" ); 
 
        GA1.height( u ) = 20.0; 
        GA1.width( u ) = 20.0; 
        GA1.shape(u) = ogdf::Shape::shEllipse;
 
        string s="";
        s+=rev[u->index()];
        GA1.label(u) = s.c_str(); 
    }
 
    edge e1;
    forall_edges(e1, T)
    {
        GA1.bends(e1);
        GA1.strokeColor(e1) = Color("#0000FF");
    }
 
    SugiyamaLayout SL1;
    SL1.setRanking( new OptimalRanking );
    SL1.setCrossMin( new MedianHeuristic );
 
    OptimalHierarchyLayout *ohl1 = new OptimalHierarchyLayout;
 
    SL1.setLayout( ohl1 );
    SL1.call( GA1 );
 
    GraphIO::drawSVG( GA1, "syntax-tree.svg" );
    
    Graph G;
 
    std::map <int, node> Gmap;
    std::map <int, int> Gdeg;

    for(int i=0; i<n; i++){
        Gmap[i] = G.newNode();
    }

    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(adj[i][j]){
                int k = adj[i][j];
                while(k--){
                    G.newEdge(Gmap[i],Gmap[j]);
                    Gdeg[i]++;
                    Gdeg[j]++;
                }
            }
        }
    }

    for(int i=0; i<n; i++){
        if(!Gdeg[i]){
            G.delNode(Gmap[i]);
        }
    }
 
    GraphAttributes GA( G, GraphAttributes::nodeGraphics |
        GraphAttributes::edgeGraphics |
        GraphAttributes::nodeLabel |
        GraphAttributes::nodeStyle |
        GraphAttributes::edgeType |
        GraphAttributes::edgeArrow |
        GraphAttributes::edgeStyle );

    node v;
    forall_nodes( v, G ){
        GA.fillColor( v ) = Color( "#FFFF00" ); 
 
        GA.height( v ) = 20.0; 
        GA.width( v ) = 20.0; 
        GA.shape(v) = ogdf::Shape::shEllipse;
 
        string s="";
        s+=rev[v->index()];
        GA.label(v) = s.c_str(); 
    }
 
    edge e;
    forall_edges(e ,G)
    {
        GA.bends(e);
        GA.strokeColor(e) = Color("#0000FF");
    }
 
    SugiyamaLayout SL;
    SL.setRanking( new OptimalRanking );
    SL.setCrossMin( new MedianHeuristic );
 
    OptimalHierarchyLayout *ohl = new OptimalHierarchyLayout;
 
    SL.setLayout( ohl );
    SL.call( GA );
 
    GraphIO::drawSVG( GA, "DAG.svg" );

    return 0;
}
