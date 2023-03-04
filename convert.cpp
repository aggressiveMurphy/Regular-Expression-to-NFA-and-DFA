#include "convert.h"
Convert::Convert()
{//构造函数
    nfaStateNum=0;
    dfaStateNum=0;
    minDfaStateNum=0;
    //存放字符的集合sigma初始时先存放ε(用#代替)
    sigma.insert('#');
    //初始化所有数组，状态号全为-1，弧上的值全为空（用#表示）
    int i, j;
    for(i = 0; i < MAX; i++)
    {
        NfaStates[i].index = i;
        NfaStates[i].input = '#';
        NfaStates[i].chTrans = -1;
    }
    for(i = 0; i < MAX; i++)
    {
        DfaStates[i].index = i;
        DfaStates[i].isEnd = false;
        for(j = 0; j < 10; j++)
        {
            DfaStates[i].Edges[j].input = '#';
            DfaStates[i].Edges[j].Trans = -1;
        }
    }
    for(i = 0; i < MAX; i++)
    {
        minDfaStates[i].index = i;
        minDfaStates[i].isEnd = false;
        for(int j = 0; j < 10; j++)
        {
            minDfaStates[i].Edges[j].input = '#';
            minDfaStates[i].Edges[j].Trans = -1;
        }
    }
}
void Convert::clearall(NFA n,DFA d)
{//清空所有数组和集合
    nfaStateNum=0;
    dfaStateNum=-1;
    minDfaStateNum=0;
    sigma.clear();
    int i, j;
    for(i=0;i<MAX;i++)
    {
        s[i].clear();
    }
    for(i = 0; i < MAX; i++)
    {
        NfaStates[i].index = 0;
        NfaStates[i].input = ' ';
        NfaStates[i].chTrans = 0;
    }
    for(i = 0; i < MAX; i++)
    {
        DfaStates[i].index = 0;
        DfaStates[i].isEnd = false;
        DfaStates[i].edgeNum=0;
        for(j = 0; j < 10; j++)
        {
            DfaStates[i].Edges[j].input = ' ';
            DfaStates[i].Edges[j].Trans = 0;
        }
    }
    for(i = 0; i < MAX; i++)
    {
        minDfaStates[i].index = 0;
        minDfaStates[i].isEnd = false;
        for(int j = 0; j < 10; j++)
        {
            minDfaStates[i].Edges[j].input = ' ';
            minDfaStates[i].Edges[j].Trans = 0;
        }
    }
    n.head->index=0;
    n.head->input=' ';
    n.head->chTrans=0;
    n.head->epTrans.clear();
    n.head=NULL;
    n.tail->index=0;
    n.tail->input=' ';
    n.tail->chTrans=0;
    n.tail->epTrans.clear();
    n.tail=NULL;
    d.startState=0;//DFA的初态
    d.endStates.clear();//DFA的终态集合
    d.terminator.clear();//DFA的终结符集
    memset(d.trans, -1, sizeof(d.trans));
    this->~Convert();
}
void Convert::add(NfaState *n1, NfaState *n2, char ch)
{//从状态n1到状态n2添加一条弧，弧上的值为ch
    n1->input = ch;
    n1->chTrans = n2->index;
}

void Convert::add(NfaState *n1, NfaState *n2)
{//从状态n1到状态n2添加一条弧，弧上的值为ε
    n1->epTrans.insert(n2->index);
}

NFA Convert::creatNFA(int sum)
{//新建一个NFA（从NFA状态数组中取出两个状态）
    NFA n;
    n.head = &NfaStates[sum];
    n.tail = &NfaStates[sum + 1];
    return n;
}

void Convert::insert(string s, int n, char ch)
{//在字符串s的第n位后面插入字符，用来给连接运算插入&
    s += '#';
    for(int i = s.size() - 1; i > n; i--)
    {
        s[i] = s[i - 1];
    }
    s[n] = ch;
}

void Convert::preprocess(string s)
{//对字符串s进行预处理，给连接运算添加&
    //给连接运算添加&
    int k = 0 , length = s.size();
    while(k < length)
    {
        if((s[k] >= 'a' && s[k] <= 'z') || (s[k] == '*') ||(s[k] == '+')||(s[k] == '?')||(s[k] == ')'))
        {//在第一位是操作数、‘*’、‘+’、‘？’或‘)’且第二位是操作数或‘(’之间加入连接符‘&’
            if((s[k + 1] >= 'a' && s[k + 1] <= 'z') || s[k + 1] == '(')
            {
                insert(s, k+1 , '&');
                length ++;
            }
        }
        k++;
    }
}

int Convert::priority(char ch)
{//优先级比较,即为每个操作符赋一个权重，通过权重大小比较优先级
    //优先级: */+/? > & > | >括号
    if(ch == '*'||ch=='+'||ch=='?')
    {
        return 3;
    }
    if(ch == '&')
    {
        return 2;
    }
    if(ch == '|')
    {
        return 1;
    }
    if(ch == '(')
    {//为了保证弹出优先级大于等于当前运算符的运算符时，左括号不会出栈，规定左括号的优先级为0
        return 0;
    }
}

string Convert::infixToSuffix(string s)
{//中缀表达式转后缀表达式
    preprocess(s);//对字符串进行预处理
    string str;//要输出的后缀字符串
    stack<char> oper;//运算符栈
    for(unsigned int i = 0; i < s.size(); i++)
    {
        if(s[i] >= 'a' && s[i] <= 'z')
        {//如果是操作数直接输出,并插入集合中
            str += s[i];
            sigma.insert(s[i]);
        }
        else
        {//遇到运算符时
            if(s[i] == '(')
            {//遇到左括号压入栈中
                oper.push(s[i]);
            }
            else if(s[i] == ')')
            {//遇到右括号时
                char ch = oper.top();
                while(ch != '(')
                {//将栈中元素出栈，直到栈顶为左括号
                    str += ch;
                    oper.pop();
                    ch = oper.top();
                }
                oper.pop();	//最后将左括号出栈
            }
            else
            {//遇到其他操作符时
                if(!oper.empty())
                {//如果栈不为空
                    char ch = oper.top();
                    while(priority(ch) >= priority(s[i]))
                    {//弹出栈中优先级大于等于当前运算符的运算符
                        str +=	ch;
                        oper.pop();
                        if(oper.empty())
                        {//如果栈为空则结束循环
                            break;
                        }
                        else ch = oper.top();
                    }
                    oper.push(s[i]);//再将当前运算符入栈
                }
                else
                {//如果栈为空，直接将运算符入栈
                    oper.push(s[i]);
                }
            }
        }
    }
    while(!oper.empty())
    {//最后如果栈不为空，则出栈并输出到字符串
        char ch = oper.top();
        oper.pop();
        str += ch;
    }
    return str;
}

NFA Convert::strToNfa(string s)
{//后缀表达式转NFA
    stack<NFA> NfaStack;//定义一个NFA栈
    for(unsigned int i = 0; i < s.size(); i++)
    {//读取后缀表达式，每次读一个字符
        if(s[i] >= 'a' && s[i] <= 'z')
        {//遇到操作数
            NFA n = creatNFA(nfaStateNum);//新建一个NFA
            nfaStateNum += 2;//NFA状态总数加2
            add(n.head, n.tail, s[i]);//NFA的头指向尾，弧上的值为s[i]
            NfaStack.push(n);//将该NFA入栈
        }
        else if(s[i] == '*')
        {//遇到闭包运算符
            NFA n1 = creatNFA(nfaStateNum);//新建一个NFA
            nfaStateNum += 2;//NFA状态总数加2
            NFA n2 = NfaStack.top();//从栈中弹出一个NFA
            NfaStack.pop();
            add(n2.tail, n1.head);//n2的尾通过ε指向n1的头
            add(n2.tail, n1.tail);//n2的尾通过ε指向n1的尾
            add(n1.head, n2.head);//n1的头通过ε指向n2的头
            add(n1.head, n1.tail);//n1的头通过ε指向n1的尾
            NfaStack.push(n1);//最后将新生成的NFA入栈
        }
        else if(s[i]=='+')
        {//遇到正闭包运算符(至少重复一次)
            NFA n1 = creatNFA(nfaStateNum);//新建一个NFA
            nfaStateNum += 2;//NFA状态总数加2
            NFA n2 = NfaStack.top();//从栈中弹出一个NFA
            NfaStack.pop();
            add(n2.tail, n1.head);//n2的尾通过ε指向n1的头
            add(n2.tail, n1.tail);//n2的尾通过ε指向n1的尾
            add(n1.head, n2.head);//n1的头通过ε指向n2的头
            NfaStack.push(n1);//最后将新生成的NFA入栈
        }
        else if(s[i]=='?')
        {//遇到可选运算符（重复0次或1次）
            NFA n1 = creatNFA(nfaStateNum);//新建一个NFA
            nfaStateNum += 2;//NFA状态总数加2
            NFA n2 = NfaStack.top();//从栈中弹出一个NFA
            NfaStack.pop();
            add(n2.tail, n1.head);//n2的尾通过ε指向n1的头
            add(n1.head, n2.head);//n1的头通过ε指向n2的头
            add(n1.head, n1.tail);//n1的头通过ε指向n1的尾
            NfaStack.push(n1);//最后将新生成的NFA入栈
        }
        else if(s[i] == '|')
        {//遇到选择运算符
            NFA n1, n2;	//从栈中弹出两个NFA，栈顶为n2，次栈顶为n1
            n2 = NfaStack.top();
            NfaStack.pop();
            n1 = NfaStack.top();
            NfaStack.pop();
            NFA n = creatNFA(nfaStateNum);//新建一个NFA
            nfaStateNum +=2;//NFA状态总数加2
            add(n.head, n1.head);//n的头通过ε指向n1的头
            add(n.head, n2.head);//n的头通过ε指向n2的头
            add(n1.tail, n.tail);//n1的尾通过ε指向n的尾
            add(n2.tail, n.tail);//n2的尾通过ε指向n的尾
            NfaStack.push(n);//最后将新生成的NFA入栈
        }
        else if(s[i] == '&')
        {//遇到连接运算符
            NFA n1, n2, n;//定义一个新的NFA n
            n2 = NfaStack.top();//从栈中弹出两个NFA，栈顶为n2，次栈顶为n1
            NfaStack.pop();
            n1 = NfaStack.top();
            NfaStack.pop();
            add(n1.tail, n2.head);//n1的尾通过ε指向n2的尾
            n.head = n1.head;//n的头为n1的头
            n.tail = n2.tail;//n的尾为n2的尾
            NfaStack.push(n);//最后将新生成的NFA入栈
        }
    }
    return NfaStack.top();//最后的栈顶元素即为生成好的NFA
}

IntSet Convert::epcloure(IntSet s)
{//求状态集s的ε-闭包
    stack<int> epStack;
    IntSet::iterator it;
    for(it = s.begin(); it != s.end(); it++)
    {
        epStack.push(*it);//将该状态集中的每一个元素都压入栈中
    }

    while(!epStack.empty())//只要栈不为空
    {
        int temp = epStack.top();//从栈中弹出一个元素
        epStack.pop();
        IntSet::iterator iter;
        for(iter = NfaStates[temp].epTrans.begin(); iter != NfaStates[temp].epTrans.end(); iter++)
        {
            if(!s.count(*iter))//遍历它通过ε能转换到的状态集
            {//如果当前元素没有在集合中出现
                s.insert(*iter);//则把它加入集合中
                epStack.push(*iter);//同时压入栈中
            }
        }
    }
    return s;
}

IntSet Convert::moveEpCloure(IntSet s, char ch)
{//求状态集s中的状态经过ch的后续状态的ε-闭包，即ε-cloure(move(ch))
    IntSet temp;
    IntSet::iterator it;
    for(it = s.begin(); it != s.end(); it++)//遍历当前集合s中的每个元素
    {
        if(NfaStates[*it].input == ch)//如果对应转换弧上的值为ch
        {
            temp.insert(NfaStates[*it].chTrans);//则把该弧通过ch转换到的状态加入到集合temp中
        }
    }
    temp = epcloure(temp);//最后求temp的ε闭包
    return temp;
}

bool Convert::IsEnd(NFA n, IntSet s)
{//判断一个状态是否为终态
    IntSet::iterator it;
    for(it = s.begin(); it != s.end(); it++)//遍历该状态所包含的nfa状态集
    {
        if(*it == n.tail->index)//如果包含nfa的终态，则该状态为终态，返回true
        {
            return true;
        }
    }
    return false;//如果不包含，则不是终态，返回false
}

DFA Convert::nfaToDfa(NFA n, string str)	//参数为nfa和后缀表达式
{//nfa转dfa主函数:子集构造法
    int i;
    DFA d;
    /*定义一个存储整数集合的集合，用于判断求出一个状态集s的ε闭包后是否出现新状态*/
    set<IntSet> states;//子集族s
    memset(d.trans, -1, sizeof(d.trans));/*初始化dfa的转移矩阵*/
    for(i = 0; i < int(str.size()); i++)/*遍历后缀表达式*/
    {
        if(str[i] >= 'a' && str[i] <= 'z')/*如果遇到操作数，则把它加入到dfa的终结符集中*/
        {
            d.terminator.insert(str[i]);
        }
    }
    d.startState = 0;/*dfa的初态为0*/
    IntSet tempSet;//子集
    tempSet.insert(n.head->index);/*将nfa的初态加入到集合中*/
    /*求dfa的初态,计算ε-closure(0)，令T0 = ε-closure(0)*/
    DfaStates[0].closure = epcloure(tempSet);//将子集T0加入子集族中
    DfaStates[0].isEnd = IsEnd(n, DfaStates[0].closure);/*判断初态是否为终态*/
    dfaStateNum++;			/*dfa数量加一*/

    queue<int> q;
    q.push(d.startState);		/*把dfa的初态（子集T0）存入队列中*/

    while(!q.empty())		/*只要队列不为空，就一直循环*/
    {

        int num = q.front();/*弹出队首元素*/
        q.pop();
        for(auto it = d.terminator.begin(); it != d.terminator.end(); it++)/*遍历终结符集*/
        {
            //对子集中的每个终结符进行ε-closure(move(T,ch))运算
            IntSet temp = moveEpCloure(DfaStates[num].closure, *it);
            if(!states.count(temp) && !temp.empty())/*如果求出来的状态集不为空且与之前求出来的状态集不同，则新建一个DFA状态*/
            {//如果子集不在子集族states中
                states.insert(temp);/*将新求出来的状态集加入到状态集合中*/
                DfaStates[dfaStateNum].closure = temp;
                DfaStates[num].Edges[DfaStates[num].edgeNum].input = *it;/*该状态弧的输入即为当前终结符*/
                DfaStates[num].Edges[DfaStates[num].edgeNum].Trans = dfaStateNum;/*弧转移到的状态为最后一个DFA状态*/
                DfaStates[num].edgeNum++;/*该状态弧的数目加一*/
                d.trans[num][*it-'a'] = dfaStateNum;/*更新转移矩阵*/
                DfaStates[dfaStateNum].isEnd = IsEnd(n, DfaStates[dfaStateNum].closure);/*判断是否为终态*/
                q.push(dfaStateNum);/*将新的状态号加入队列中*/
                dfaStateNum++;/*DFA状态总数加一*/
            }
            else
            {/*求出来的状态集在之前求出的某个状态集相同*/
                //如果子集在子集族states中
                for(i = 0; i < dfaStateNum; i++)/*遍历之前求出来的状态集合*/
                {
                    if(temp == DfaStates[i].closure)/*找到与该集合相同的DFA状态*/
                    {
                        DfaStates[num].Edges[DfaStates[num].edgeNum].input = *it;/*该状态弧的输入即为当前终结符*/
                        DfaStates[num].Edges[DfaStates[num].edgeNum].Trans = i;	/*该弧转移到的状态即为i*/
                        DfaStates[num].edgeNum++;/*该状态弧的数目加一*/
                        d.trans[num][*it - 'a'] = i;/*更新转移矩阵*/
                        break;
                    }
                }
            }
        }
    }
    /*计算dfa的终态集*/
    for(i = 0; i < dfaStateNum; i++)/*遍历dfa的所有状态*/
    {
        if(DfaStates[i].isEnd == true)/*如果该状态是终态*/
        {
            d.endStates.insert(i);/*则将该状态号加入到dfa的终态集中*/
        }
    }
    return d;
}

int Convert::findSetNum(int count, int n)
{/*当前划分总数为count，返回状态n所属的状态集标号i*/
    for(int i = 0; i < count; i++)
    {
        if(s[i].count(n))
        {
            return i;
        }
    }
}

DFA Convert::minDFA(DFA d)
{/*最小化DFA*/
    int i, j;
    DFA minDfa;
    minDfa.terminator = d.terminator;/*把dfa的终结符集赋给minDfa*/
    memset(minDfa.trans, -1, sizeof(minDfa.trans));/*初始化minDfa转移矩阵*/
    /*做第一次划分，即将终态与非终态分开*/
    bool endFlag = true;/*判断dfa的所有状态是否全为终态的标志*/
    for(i = 0; i < dfaStateNum; i++)/*遍历dfa状态数组*/
    {
        if(DfaStates[i].isEnd == false)	/*如果该dfa状态不是终态*/
        {
            endFlag = false;/*标志应为false*/
            minDfaStateNum = 2;	/*第一次划分应该有两个集合*/
            s[1].insert(DfaStates[i].index);/*把该状态的状态号加入s[1]集合中*/
        }
        else
        {/*如果该dfa状态是终态*/
            s[0].insert(DfaStates[i].index);/*把该状态的状态号加入s[0]集合中*/
        }
    }
    if(endFlag)/*如果标志为真，则所有dfa状态都是终态*/
    {
        minDfaStateNum = 1;	/*第一次划分结束应只有一个集合*/
    }
    bool cutFlag = true;/*上一次是否产生新的划分的标志*/
    while(cutFlag)/*只要上一次产生新的划分就继续循环*/
    {
        int cutCount = 0;/*需要产生新的划分的数量*/
        for(i = 0; i < minDfaStateNum; i++)/*遍历每个划分集合*/
        {
            for(auto it = d.terminator.begin(); it != d.terminator.end(); it++)/*遍历dfa的终结符集*/
            {
                int setNum = 0;	/*当前缓冲区中的状态集个数*/
                stateSet temp[20];/*划分状态集“缓冲区”*/
                IntSet::iterator iter;
                for(iter = s[i].begin(); iter != s[i].end(); iter++)/*遍历集合中的每个状态号*/
                {
                    bool epFlag = true;	/*判断该集合中是否存在没有该终结符对应的转换弧的状态*/
                    for(j = 0; j < DfaStates[*iter].edgeNum; j++)/*遍历该状态的所有边*/
                    {
                        if(DfaStates[*iter].Edges[j].input == *it)/*如果该边的输入为该终结符*/
                        {
                            epFlag = false;	/*则标志为false*/
                            /*计算该状态转换到的状态集的标号*/
                            int transNum = findSetNum(minDfaStateNum, DfaStates[*iter].Edges[j].Trans);
                            int curSetNum = 0;/*遍历缓冲区，寻找是否存在到达这个标号的状态集*/
                            while((temp[curSetNum].index != transNum) && (curSetNum < setNum))
                            {
                                curSetNum++;
                            }
                            if(curSetNum == setNum)/*缓冲区中不存在到达这个标号的状态集*/
                            {
                                /*在缓冲区中新建一个状态集*/
                                temp[setNum].index = transNum;/*该状态集所能转换到的状态集标号为transNum*/
                                temp[setNum].s.insert(*iter);/*把当前状态添加到该状态集中*/
                                setNum++;/*缓冲区中的状态集个数加一*/
                            }
                            else/*缓冲区中存在到达这个标号的状态集*/
                            {
                                temp[curSetNum].s.insert(*iter);/*把当前状态加入到该状态集中*/
                            }
                        }
                    }
                    if(epFlag)/*如果该状态不存在与该终结符对应的转换弧*/
                    {
                        /*寻找缓冲区中是否存在转换到标号为-1的状态集
                        这里规定如果不存在转换弧，则它所到达的状态集标号为-1*/
                        int curSetNum = 0;
                        while((temp[curSetNum].index != -1) && (curSetNum < setNum))
                        {
                            curSetNum++;
                        }
                        if(curSetNum == setNum)/*如果不存在这样的状态集*/
                        {
                            /*在缓冲区中新建一个状态集*/
                            temp[setNum].index = -1;/*该状态集转移到的状态集标号为-1*/
                            temp[setNum].s.insert(*iter);/*把当前状态加入到该状态集中*/
                            setNum++;/*缓冲区中的状态集个数加一*/
                        }
                        else/*缓冲区中存在到达这个标号的状态集*/
                        {
                            temp[curSetNum].s.insert(*iter);/*把当前状态加入到该状态集中*/
                        }
                    }
                }
                if(setNum > 1)/*如果缓冲区中的状态集个数大于1，表示同一个状态集中的元素能转换到不同的状态集，则需要划分*/
                {
                    cutCount++;	/*划分次数加一*/
                    /*为每组划分创建新的dfa状态*/
                    for(j = 1; j < setNum; j++)	/*遍历缓冲区，这里从1开始是将第0组划分留在原集合中*/
                    {
                        IntSet::iterator t;
                        for(t = temp[j].s.begin(); t != temp[j].s.end(); t++)
                        {
                            s[i].erase(*t);	/*在原来的状态集中删除该状态*/
                            s[minDfaStateNum].insert(*t);/*在新的状态集中加入该状态*/
                        }
                        minDfaStateNum++;/*最小化DFA状态总数加一*/
                    }
                }
            }
        }
        if(cutCount == 0)/*如果需要划分的次数为0，表示本次不需要进行划分*/
        {
            cutFlag = false;
        }
    }
    /*遍历每个划分好的状态集*/
    for(i = 0; i < minDfaStateNum; i++)
    {
        IntSet::iterator y;
        for(y = s[i].begin(); y != s[i].end(); y++)	/*遍历集合中的每个元素*/
        {
            if(*y == d.startState)/*如果当前状态为dfa的初态，则该最小化DFA状态也为初态*/
            {
                minDfa.startState = i;
            }
            if(d.endStates.count(*y))/*如果当前状态是终态，则该最小化DFA状态也为终态*/
            {
                minDfaStates[i].isEnd = true;
                minDfa.endStates.insert(i);	/*将该最小化DFA状态加入终态集中*/
            }
            for(j = 0; j < DfaStates[*y].edgeNum; j++)/*遍历该DFA状态的每条弧，为最小化DFA创建弧*/
            {
                /*遍历划分好的状态集合，找出该弧转移到的状态现在属于哪个集合*/
                for(int t = 0; t < minDfaStateNum; t++)
                {
                    if(s[t].count(DfaStates[*y].Edges[j].Trans))
                    {
                        bool haveEdge = false;/*判断该弧是否已经创建的标志*/
                        for(int l = 0; l < minDfaStates[i].edgeNum; l++)/*遍历已创建的弧*/
                        {/*如果该弧已经存在*/
                            if((minDfaStates[i].Edges[l].input == DfaStates[*y].Edges[j].input) && (minDfaStates[i].Edges[l].Trans == t))
                            {
                                haveEdge = true;/*标志为真*/
                            }
                        }
                        if(!haveEdge)/*如果该弧不存在，则创建一条新的弧*/
                        {
                            minDfaStates[i].Edges[minDfaStates[i].edgeNum].input = DfaStates[*y].Edges[j].input;	/*弧的值与DFA的相同*/
                            minDfaStates[i].Edges[minDfaStates[i].edgeNum].Trans = t;	/*该弧转移到的状态为这个状态集的标号*/
                            minDfa.trans[i][DfaStates[*y].Edges[j].input - 'a'] = t;	/*更新转移矩阵*/
                            minDfaStates[i].edgeNum++;		/*该状态的弧的数目加一*/
                        }
                        break;
                    }
                }
            }
        }
    }
    return minDfa;
}

void Convert::disNFA(NFA nfa)
{
    qDebug()<<"***************     NFA     ***************"<<endl<<endl;
        qDebug()<<"NFA总共有"<<nfaStateNum<<"个状态，"<<endl;
        qDebug()<<"初态为"<<nfa.head->index<<"，终态为" <<nfa.tail->index<<"。"<<endl<<endl<<"转移函数为："<<endl;
        for(int i = 0; i < nfaStateNum; i++)/*遍历NFA状态数组*/
        {
            if(NfaStates[i].input != '#')/*如果弧上的值不是初始时的‘#’则输出*/
            {
                qDebug()<<NfaStates[i].index<<"1-->'"<<NfaStates[i].input<<"'-->"<<NfaStates[i].chTrans<<'\t';
            }
            IntSet::iterator it;/*输出该状态经过ε到达的状态*/
            for(it = NfaStates[i].epTrans.begin(); it != NfaStates[i].epTrans.end(); it++)
            {
                qDebug()<<NfaStates[i].index<<"-->'"<<' '<<"'-->"<<*it<<'\t';
            }
            qDebug()<<endl;
        }
}

void Convert::disDFA(DFA d)
{
    int i, j;
        qDebug()<<"DFA总共有"<<dfaStateNum<<"个状态，"<<"初态为"<<d.startState<<endl<<endl;
        qDebug()<<"有穷字母表为｛ ";
        for(auto it = d.terminator.begin(); it != d.terminator.end(); it++)
        {
            qDebug()<<*it<<' ';
        }
        qDebug()<<'}'<<endl<<endl;
        qDebug()<<"终态集为｛ ";
        IntSet::iterator iter;
        for(iter = d.endStates.begin(); iter != d.endStates.end(); iter++)
        {
            qDebug()<<*iter<<' ';
        }
        qDebug()<<'}'<<endl<<endl;
       qDebug()<<"转移函数为："<<endl;
        for(i = 0; i < dfaStateNum; i++)
        {
            for(j = 0; j < DfaStates[i].edgeNum; j++)
            {
                if(DfaStates[DfaStates[i].Edges[j].Trans].isEnd == true)
                {
                    qDebug()<<DfaStates[i].index<<"-->'"<<DfaStates[i].Edges[j].input;
                    qDebug()<<"'--><"<<DfaStates[i].Edges[j].Trans<<">\t";
                }
                else
                {
                    qDebug()<<DfaStates[i].index<<"-->'"<<DfaStates[i].Edges[j].input;
                    qDebug()<<"'-->"<<DfaStates[i].Edges[j].Trans<<'\t';
                }
            }
            qDebug()<<endl;
        }
        qDebug()<<endl<<"转移矩阵为："<<endl<<"     ";
        for(auto t = d.terminator.begin(); t != d.terminator.end(); t++)
        {
            qDebug()<<*t<<"   ";
        }
        qDebug()<<endl;
        for(i = 0; i < dfaStateNum; i++)
        {

            if(d.endStates.count(i))
            {
                qDebug()<<'<'<<i<<">  ";
            }
            else
            {
                qDebug()<<' '<<i<<"   ";
            }

            for(j = 0; j < 26; j++)
            {
                if(d.terminator.count(j + 'a'))
                {
                    if(d.trans[i][j] != -1)
                    {
                        qDebug()<<d.trans[i][j]<<"   ";
                    }
                    else
                    {
                        qDebug()<<"    ";
                    }
                }
            }
            qDebug()<<endl;
        }
}

void Convert::disMinDFA(DFA d)
{
    int i, j;
        qDebug()<<"minDFA总共有"<<minDfaStateNum<<"个状态，"<<"初态为"<<d.startState<<endl<<endl;

        qDebug()<<"有穷字母表为｛ ";
        for(auto it = d.terminator.begin(); it != d.terminator.end(); it++)
        {
            qDebug()<<*it<<' ';
        }
        qDebug()<<'}'<<endl<<endl;
        qDebug()<<"终态集为｛ ";
        IntSet::iterator iter;
        for(iter = d.endStates.begin(); iter != d.endStates.end(); iter++)
        {
            qDebug()<<*iter<<' ';
        }
        qDebug()<<'}'<<endl<<endl;
        qDebug()<<"转移函数为："<<endl;
        for(i = 0; i < minDfaStateNum; i++)
        {
            for(j = 0; j < minDfaStates[i].edgeNum; j++)
            {

                if(minDfaStates[minDfaStates[i].Edges[j].Trans].isEnd == true)
                {
                    qDebug()<<minDfaStates[i].index<<"-->'"<<minDfaStates[i].Edges[j].input;
                    qDebug()<<"'--><"<<minDfaStates[i].Edges[j].Trans<<">\t";
                }
                else
                {
                    qDebug()<<minDfaStates[i].index<<"-->'"<<minDfaStates[i].Edges[j].input;
                    qDebug()<<"'-->"<<minDfaStates[i].Edges[j].Trans<<'\t';
                }
            }
            qDebug()<<endl;
        }
        qDebug()<<endl<<"转移矩阵为："<<endl<<"     ";
        CharSet::iterator t;
        for(auto t = d.terminator.begin(); t != d.terminator.end(); t++)
        {
            qDebug()<<*t<<"   ";
        }
        qDebug()<<endl;

        for(i = 0; i < minDfaStateNum; i++)
        {

            if(d.endStates.count(i))
            {
                qDebug()<<'<'<<i<<">  ";
            }
            else
            {
                qDebug()<<' '<<i<<"   ";
            }

            for(j = 0; j < 26; j++)
            {
                if(d.terminator.count(j + 'a'))
                {
                    if(d.trans[i][j] != -1)
                    {
                        qDebug()<<d.trans[i][j]<<"   ";
                    }
                    else
                    {
                        qDebug()<<"    ";
                    }
                }
            }
            qDebug()<<endl;
        }
        qDebug()<<endl<<"*******************************************";
}
