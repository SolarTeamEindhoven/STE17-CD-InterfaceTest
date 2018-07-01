#ifndef LAADPAAL_H
#define LAADPAAL_H

#include <QMainWindow>

namespace Ui {
class Laadpaal;
}

class Laadpaal : public QMainWindow
{
    Q_OBJECT

public:
    explicit Laadpaal(QWidget *parent = 0);
    ~Laadpaal();

private:
    Ui::Laadpaal *ui;
};

#endif // LAADPAAL_H
