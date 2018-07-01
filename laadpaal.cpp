#include "laadpaal.h"
#include "ui_laadpaal.h"

Laadpaal::Laadpaal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Laadpaal)
{
    ui->setupUi(this);
}

Laadpaal::~Laadpaal()
{
    delete ui;
}
