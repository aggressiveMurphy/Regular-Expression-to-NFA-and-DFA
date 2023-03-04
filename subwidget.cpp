#include "subwidget.h"
#include "ui_subwidget.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QMap>
SubWidget::SubWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubWidget)
{
    ui->setupUi(this);
}

SubWidget::~SubWidget()
{
    delete ui;
}

void SubWidget::on_pushButton_clicked()
{//打开存放正则表达式的文件，并将正则表达式展示在文本框内
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个代码文件", ".", "Text(*.txt);;all(*.*)");
    ui->lineEdit->clear();//清除文本框内上一次留下的文本内容
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&file);
    while(!in.atEnd())
    {//读取文件内容
        QString line = in.readLine();
        ui->lineEdit->setText(line);//展示正则表达式
    }
    file.close();
}

void SubWidget::on_pushButton_2_clicked()
{//存放正则表达式，将用户在文本框内输入的正则表达式保存到文件中
    QString file_path = QFileDialog::getSaveFileName(this,"保存文件","","Text(*.txt);;all(*.*)");
    QFile f1(file_path);
    if(!f1.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)){
        QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
        return ;
    }
    else{
        QTextStream stream(&f1);
        QString line=ui->lineEdit->text();
        stream <<line<< endl;
    }
    f1.close();
}

void SubWidget::on_pushButton_3_clicked()
{//将正则表达式转换成NFA、DFA和最小DFA
    Convert *c=new Convert();
    QString s1=ui->lineEdit->text();
    string s2 = s1.toStdString();//将QString转换成string
    string s3=c->infixToSuffix(s2);//将中缀表达式转换为后缀表达式
    //将后缀表达式转换成NFA
    NFA n = c->strToNfa(s3);
    //显示NFA
    displayNFA(c,n);
    //将NFA转换成DFA
    DFA d = c->nfaToDfa(n, s3);
    //显示DFA
    displayDFA(c,d);
    //显示DFA
    displayDFA2(c,d);
    //求最小DFA
    DFA minDfa = c->minDFA(d);
    //显示最小DFA
    displayMinDFA(c,minDfa);
//  c->~Convert();
}

void SubWidget::displayNFA(Convert *c,NFA n)
{//显示NFA
    //先清除上一次留下的内容
    ui->tableWidget->clear();
    //设置表格中所有单元格不可以编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
   //设置表头样式
    ui->tableWidget->horizontalHeader()->setStyleSheet(
    "QHeaderView::section{background-color:rgb(225,225,225);"
    "font:10pt '宋体';color: black;};");
    /*列数的大小是字符数+2，其中+2是因为一列表头是状态，
     * 一列表头是初始结束状态*/
    int column = (c->sigma).size()+2;
    //设置列数
    ui->tableWidget->setColumnCount(column);
    //表头内容:状态+字符
    QStringList headList;
    headList<<"状态";
    for(auto it=c->sigma.begin();it!=c->sigma.end();it++)
    {
        headList<<(*it);
    }
    headList<<"开始/结束状态";
    //设置水平表头
    ui->tableWidget->setHorizontalHeaderLabels(headList);
    //行数
    int row = c->nfaStateNum;
    //设置行数
    ui->tableWidget->setRowCount(row);
    //一行一行绘制
    for(int i = 0; i < row; i++)
    {//i为行号
        //每一行的第0列为NFA的每个状态的状态号
         ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(c->NfaStates[i].index)));
         //设置表格内容居中
         ui->tableWidget->item(c->NfaStates[i].index,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

         //j为列号,从第1列到第column-2列为经过列头字符转换得到的下一个状态的状态号
         int j=1;
         //如果弧上的值为该字符，则存在通过这个字符进行状态转换,将转换后得到的状态号显示出来
             for(auto it=c->sigma.begin();it!=c->sigma.end();it++)
            {
             if(c->NfaStates[i].input!='#'&&c->NfaStates[i].input==(*it))
             {//弧上的值是表头的字符且不是初始的'#'时
                 ui->tableWidget->setItem(i,j,new QTableWidgetItem(QString::number(c->NfaStates[i].chTrans)));
                 //设置表格内容居中
                 ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
             }
             if(c->NfaStates[i].input=='#'&&c->NfaStates[i].input==(*it))
             {//弧上的值是表头的字符且为'#'时
                 //该状态经过ε到达的状态可能有多个，用逗号分隔再填入单元格中
                 QString s="";
                 for(auto it1 = c->NfaStates[i].epTrans.begin(); it1 != c->NfaStates[i].epTrans.end(); it1++)
                 {
                    if(it1==c->NfaStates[i].epTrans.begin())
                    {
                        s+=QString::number(*it1);
                    }
                    else
                    {
                        s+=",";
                        s+=QString::number(*it1);
                    }

                 }
                 ui->tableWidget->setItem(i,j,new QTableWidgetItem(s));
                 //设置表格内容居中
                 ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
             }
             j++;
             }
            //每一行的最后一列
            if(i==n.tail->index)//是结束状态
            {
                ui->tableWidget->setItem(i,j,new QTableWidgetItem("结束状态"));//填入该列
                //设置表格内容居中
                ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            }
            if(i==c->NfaStates[0].index)//是初始状态
            {
                ui->tableWidget->setItem(i,j,new QTableWidgetItem("开始状态"));//填入该列
                //设置表格内容居中
                ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            }
        }
}

void SubWidget::displayDFA(Convert *c, DFA d)
{//显示DFA
    //先清除上一次留下的内容
    ui->tableWidget_2->clear();
    //设置表格中所有单元格不可以编辑
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
   //设置表头样式
    ui->tableWidget_2->horizontalHeader()->setStyleSheet(
    "QHeaderView::section{background-color:rgb(225,225,225);"
    "font:10pt '宋体';color: black;};");
    /*列数的大小是有穷字母集中的字符数+2，其中+2是因为一列表头是状态编号，
     * 一列表头是初始结束状态*/
    int column = d.terminator.size()+2;
    //设置列数
    ui->tableWidget_2->setColumnCount(column);
    //表头内容:状态集合+有穷字母表里面的字符
    QStringList headList;
    headList<<"状态集合";
    QMap<char,int> map;
    int k=1;
    for(auto it = d.terminator.begin(); it != d.terminator.end(); it++)
    {//遍历有穷字母表
        map.insert(*it,k);//建立字母表与列号的映射
        QChar qc=(*it);
        headList<<qc;
        k++;
    }
    headList<<"开始/结束状态";
    //设置水平表头
    ui->tableWidget_2->setHorizontalHeaderLabels(headList);
    //设置行数
    //行数
    int row = d.endStates.size();
    ui->tableWidget_2->setRowCount(row);
    //一行一行绘制
    for(int i = 0; i < row; i++)
    {//i为行号
        //每一行的第0列为DFA状态
        QString s1="";
        auto it1=c->DfaStates[i].closure.end();
        it1--;//it1指向集合的最后一个元素
        for(auto it=c->DfaStates[i].closure.begin();it!=c->DfaStates[i].closure.end();it++)
        {
            if(it==c->DfaStates[i].closure.begin())
            {
                s1+="{";
                s1+=QString::number(*it);
            }else if(it==it1)
            {
                s1+=",";
                s1+=QString::number(*it);
                s1+="}";
            }
            else
            {
                s1+=",";
                s1+=QString::number(*it);
            }
        }
        ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(s1));
        //设置表格内容居中
        ui->tableWidget_2->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        for(auto it = d.terminator.begin(); it != d.terminator.end(); it++)
        {//遍历有穷字母表
                if(d.trans[i][(*it)-'a']!=-1)
                {
                    int t=d.trans[i][(*it)-'a'];
                    QString s2="";
                    auto it3=c->DfaStates[t].closure.end();
                    it3--;//it1指向集合的最后一个元素
                    for(auto it2=c->DfaStates[t].closure.begin();it2!=c->DfaStates[t].closure.end();it2++)
                    {
                        if(it2==c->DfaStates[t].closure.begin())
                        {
                            s2+="{";
                            s2+=QString::number(*it2);
                        }else if(it2==it3)
                        {
                            s2+=",";
                            s2+=QString::number(*it2);
                            s2+="}";
                        }
                        else
                        {
                            s2+=",";
                            s2+=QString::number(*it2);
                        }
                    }
                    //从第1列到第column-2列为DFA状态集合的ε-cloure(move(ch))
                    ui->tableWidget_2->setItem(i,map[*it],new QTableWidgetItem(s2));
                    //设置表格内容居中
                    ui->tableWidget_2->item(i,map[*it])->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                }
        }
        //每一行最后一列显示开始/结束状态
        if(i==d.startState)
        {//状态集合是终态
            ui->tableWidget_2->setItem(i,(column-1),new QTableWidgetItem("开始状态"));
            //设置表格内容居中
            ui->tableWidget_2->item(i,(column-1))->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
        else
        {//状态集合是终态
            ui->tableWidget_2->setItem(i,(column-1),new QTableWidgetItem("结束状态"));
            //设置表格内容居中
            ui->tableWidget_2->item(i,(column-1))->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
    }
}

void SubWidget::displayDFA2(Convert *c, DFA d)
{//显示DFA转换矩阵
    //先清除上一次留下的内容
    ui->tableWidget_4->clear();
    //设置表格中所有单元格不可以编辑
    ui->tableWidget_4->setEditTriggers(QAbstractItemView::NoEditTriggers);
   //设置表头样式
    ui->tableWidget_4->horizontalHeader()->setStyleSheet(
    "QHeaderView::section{background-color:rgb(225,225,225);"
    "font:10pt '宋体';color: black;};");
    /*列数的大小是有穷字母集中的字符数+2，其中+2是因为一列表头是状态编号，
     * 一列表头是初始结束状态*/
    int column = d.terminator.size()+2;
    //设置列数
    ui->tableWidget_4->setColumnCount(column);
    //表头内容:状态集合+有穷字母表里面的字符
    QStringList headList;
    headList<<"状态";
    QMap<char,int> map;
    int k=1;
    for(auto it = d.terminator.begin(); it != d.terminator.end(); it++)
    {//遍历有穷字母表
        map.insert(*it,k);//建立字母表与列号的映射
        QChar qc=(*it);
        headList<<qc;
        k++;
    }
    headList<<"开始/结束状态";
    //设置水平表头
    ui->tableWidget_4->setHorizontalHeaderLabels(headList);
    //设置行数
    //行数为DFA的状态总数
    int row = d.endStates.size();
    qDebug()<<row;
    ui->tableWidget_4->setRowCount(row);
    //一行一行绘制
    for(int i = 0; i < row; i++)
    {
        //i为行号
        //每一行的第0列为DFA状态
        int t=c->DfaStates[i].index;
        ui->tableWidget_4->setItem(i,0,new QTableWidgetItem(QString::number(t)));
        //设置表格内容居中
        ui->tableWidget_4->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        //从第1列到第column-2列为经过字符ch转移到的下一个状态
        for(auto it1 = d.terminator.begin(); it1 != d.terminator.end(); it1++)
        {//遍历有穷字母表
            if(d.trans[i][*it1-'a']!=-1)
            {
                ui->tableWidget_4->setItem(i,map[*it1],new QTableWidgetItem(QString::number(d.trans[i][*it1-'a'])));
                //设置表格内容居中
                ui->tableWidget_4->item(i,map[*it1])->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            }
        }
        //每一行最后一列显示开始/结束状态
        if(i==d.startState)
        {//该状态是初态
            ui->tableWidget_4->setItem(i,(column-1),new QTableWidgetItem("开始状态"));
            //设置表格内容居中
            ui->tableWidget_4->item(i,(column-1))->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
        else
        {//该状态是终态
            ui->tableWidget_4->setItem(i,(column-1),new QTableWidgetItem("结束状态"));
            //设置表格内容居中
            ui->tableWidget_4->item(i,(column-1))->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
    }

}
void SubWidget::displayMinDFA(Convert *c,DFA minDfa)
{//显示最小DFA
    //先清除上一次留下的内容
    ui->tableWidget_3->clear();
    //设置表格中所有单元格不可以编辑
    ui->tableWidget_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
   //设置表头样式
    ui->tableWidget_3->horizontalHeader()->setStyleSheet(
    "QHeaderView::section{background-color:rgb(225,225,225);"
    "font:10pt '宋体';color: black;};");
    /*列数的大小是有穷字母集中的字符数+2，其中+2是因为一列表头是状态编号，
     * 一列表头是初始结束状态*/
    int column = minDfa.terminator.size()+2;
    //设置列数
    ui->tableWidget_3->setColumnCount(column);
    //表头内容:状态集合+有穷字母表里面的字符
    QStringList headList;
    headList<<"状态";
    QMap<char,int> map;
    int k=1;
    for(auto it = minDfa.terminator.begin(); it != minDfa.terminator.end(); it++)
    {//遍历有穷字母表
        map.insert(*it,k);//建立字母表与列号的映射
        QChar qc=(*it);
        headList<<qc;
        k++;
    }
    headList<<"开始/结束状态";
    //设置水平表头
    ui->tableWidget_3->setHorizontalHeaderLabels(headList);
    //设置行数
    //行数为最小DFA的状态总数
    int row = c->minDfaStateNum;
    ui->tableWidget_3->setRowCount(row);
    //一行一行绘制
    for(int i = 0; i < row; i++)
    {
        //i为行号
        //每一行的第0列为最小DFA状态
        int t=c->minDfaStates[i].index;
        ui->tableWidget_3->setItem(i,0,new QTableWidgetItem(QString::number(t)));
        //设置表格内容居中
        ui->tableWidget_3->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        //从第1列到第column-2列为经过字符ch转移到的下一个状态
        for(auto it1 = minDfa.terminator.begin(); it1 != minDfa.terminator.end(); it1++)
        {//遍历有穷字母表
            if(minDfa.trans[i][*it1-'a']!=-1)
            {
                ui->tableWidget_3->setItem(i,map[*it1],new QTableWidgetItem(QString::number(minDfa.trans[i][*it1-'a'])));
                //设置表格内容居中
                ui->tableWidget_3->item(i,map[*it1])->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            }
        }
        //每一行最后一列显示开始/结束状态
        if(c->minDfaStates[i].isEnd==true)
        {//该状态是终态
            ui->tableWidget_3->setItem(i,(column-1),new QTableWidgetItem("结束状态"));
            //设置表格内容居中
            ui->tableWidget_3->item(i,(column-1))->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
        else
        {//该状态是初态
            ui->tableWidget_3->setItem(i,(column-1),new QTableWidgetItem("开始状态"));
            //设置表格内容居中
            ui->tableWidget_3->item(i,(column-1))->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
    }
}
void SubWidget::on_pushButton_4_clicked()
{//点击“返回”按钮，关闭子窗口，切换到主窗口
    this->close();
    MainWindow *w=new MainWindow();
    w->show();
}
