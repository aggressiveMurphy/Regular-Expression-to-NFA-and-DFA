#ifndef SUBWIDGET_H
#define SUBWIDGET_H

#include <QWidget>
#include "convert.h"

namespace Ui {
class SubWidget;
}

class SubWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SubWidget(QWidget *parent = nullptr);
    ~SubWidget();
    void displayNFA(Convert *c,NFA n);
    void displayDFA(Convert *c,DFA d);
    void displayDFA2(Convert *c, DFA d);
    void displayMinDFA(Convert *c,DFA d);
private:
    Ui::SubWidget *ui;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
};

#endif // SUBWIDGET_H
