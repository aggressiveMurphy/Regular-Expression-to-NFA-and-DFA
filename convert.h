#ifndef CONVERT_H
#define CONVERT_H
//定义数据结构和声明转换过程中需要用到的函数
#include <string>
#include <stack>
#include <vector>
#include <set>
#include <queue>
#include <QDebug>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QChar>
#include <iostream>

#define MAX 128
using namespace std;

typedef set<int> IntSet;//存放状态号的整数集合
typedef set<char> CharSet;//存放弧上的值的字符集合

struct NfaState
{//定义NFA状态
    int index;//NFA中每个状态的状态号，从0开始
    char input;//NFA中状态弧上的值,char类型,1个字节
    int chTrans;//NFA状态弧转换到的状态的状态号
    IntSet epTrans;//当前状态通过ε转换到的状态的状态号集合
};

struct NFA
{//定义NFA表
    NfaState *head;//头指针
    NfaState *tail;//尾指针
};


struct Edge
{//定义DFA的转换弧
    char input;//弧上的值
    int Trans;//弧所指向的状态号
};

struct DfaState
{//定义DFA状态
    //该状态是否为终态，是为true，不是为false
    bool isEnd;
    //DFA状态的状态号，从0开始
    int index;
    //状态集合的ε-闭包
    IntSet closure;
    //DFA状态上的弧数
    int edgeNum;
    //DFA状态上的弧
    Edge Edges[10];
};

struct DFA
{//定义DFA结构
    int startState;//DFA的初态
    set<int> endStates;//DFA的终态集合
    set<char> terminator;//DFA的终结符集
    int trans[MAX][26];//DFA的状态转换矩阵
};

struct stateSet
{//划分的状态集
    int index;//该状态集所能转换到的状态集标号
    IntSet s;//该状态集中的dfa的状态号集合
};

class Convert
{
public:
    set<QChar>sigma;//输入的字符集合
    NfaState NfaStates[MAX];//NFA状态数组
    int nfaStateNum;//NFA状态总数
    DfaState DfaStates[MAX];//DFA状态数组
    int dfaStateNum;//DFA状态总数
    IntSet s[MAX];//划分出来的集合数组
    DfaState minDfaStates[MAX];//最小Dfa状态数组
    int minDfaStateNum;	//最小Dfa的状态总数，同时也是划分出的集合数
    Convert();
    //在状态n1与n2之间添加一条弧，弧上的值为字符
    void add(NfaState *n1, NfaState *n2, char ch);
    //在状态n1与n2之间添加一条弧，弧上的值为ε
    void add(NfaState *n1, NfaState *n2);
    //创建NFA
    NFA creatNFA(int sum);
    //在字符串s的第n位后面插入字符ch
    void insert(string s, int n, char ch);
    //对正则表达式进行预处理
    void preprocess(string s);
    //运算符优先级比较
    int priority(char ch);
    //中缀表达式转换成后缀表达式
    string infixToSuffix(string s);
    //后缀表达式转换为NFA
    NFA strToNfa(string s);
    //求状态集s的ε-闭包
    IntSet epcloure(IntSet s);
    //求状态集s中的状态经过ch的后续状态的ε-闭包
    IntSet moveEpCloure(IntSet s, char ch);
    //判断一个状态是否为终态
    bool IsEnd(NFA n, IntSet s);
    //nfa转dfa的主函数
    DFA nfaToDfa(NFA n, string str);
    //返回状态n所属的状态集标号i
    int findSetNum(int count, int n);
    //最小化DFA
    DFA minDFA(DFA d);
    //显示NFA状态表
    void disNFA(NFA n);
    //显示DFA状态表
    void disDFA(DFA d);
    //显示最小DFA状态表
    void disMinDFA(DFA d);
    void clearall(NFA n,DFA d);
};

#endif // CONVERT_H
