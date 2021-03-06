#include "quicktaskcreatordefaults.h"
#include "ui_quicktaskcreatordefaults.h"

QuickTaskCreatorDefaults::QuickTaskCreatorDefaults(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuickTaskCreatorDefaults)
{

    ui->setupUi(this);
    QList<Json::Value>*tasks=static_cast <qtMushi *>(qApp)->taskDirectory.getAllTasks();
    this->ui->taskCombo->addItem("",QVariant(""));
    for(int x=0;x<tasks->count();x++){
        this->ui->taskCombo->addItem(tasks->at(x).get("title","").asString().c_str(),QVariant(tasks->at(x).get("id","").asCString()));
    }

    QList<Json::Value>*statuses=static_cast <qtMushi *>(qApp)->statusDirectory.getAllStatuses();
    for(int x=0;x<statuses->count();x++){
        this->ui->statusCombo->addItem(statuses->at(x).get("name","").asString().c_str(),QVariant(statuses->at(x).get("id","").asCString()));
    }

    QList<Json::Value>*users=static_cast <qtMushi *>(qApp)->userDirectory.getAllUsers();
    for(int x=0;x<users->count();x++){
        this->ui->ownerCombo->addItem(users->at(x).get("firstName","").asString().append(" ").append(users->at(x).get("lastName","").asString()).c_str(),QVariant(users->at(x).get("id","").asCString()));
    }
}

QString QuickTaskCreatorDefaults::getParent(){
    return this->ui->taskCombo->itemData(this->ui->taskCombo->currentIndex()).toString();
}

QString QuickTaskCreatorDefaults::getStatus(){
    return this->ui->statusCombo->itemData(this->ui->statusCombo->currentIndex()).toString();
}

QString QuickTaskCreatorDefaults::getOwner(){
    return this->ui->ownerCombo->itemData(this->ui->ownerCombo->currentIndex()).toString();
}


QuickTaskCreatorDefaults::~QuickTaskCreatorDefaults()
{
    delete ui;
}
