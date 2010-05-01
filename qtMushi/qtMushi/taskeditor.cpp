#include "taskeditor.h"
#include "servercommand.h"
#include <QDebug>
#include <string>
TaskEditor::TaskEditor(QWidget *parent) :
    QWidget(parent){
    setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->taskCombo->addItem("");
    connect(this->saveButton,SIGNAL(clicked()),this,SLOT(save()));
    QList<Json::Value>*tasks=static_cast <qtMushi *>(qApp)->taskDirectory.getAllTasks();
    for(int x=0;x<tasks->count();x++){
        this->taskCombo->addItem(tasks->at(x).get("title","").asString().c_str(),QVariant(tasks->at(x).get("id","").asCString()));
    }

    QList<Json::Value>*statuses=static_cast <qtMushi *>(qApp)->statusDirectory.getAllStatuses();
    for(int x=0;x<statuses->count();x++){
        this->statusCombo->addItem(statuses->at(x).get("name","").asString().c_str(),QVariant(statuses->at(x).get("id","").asCString()));
    }

    QList<Json::Value>*users=static_cast <qtMushi *>(qApp)->userDirectory.getAllUsers();
    for(int x=0;x<users->count();x++){
        this->ownerCombo->addItem(users->at(x).get("firstName","").asString().append(" ").append(users->at(x).get("lastName","").asString()).c_str(),QVariant(users->at(x).get("id","").asCString()));
    }

    this->connect(this->addNoteButton,SIGNAL(clicked()),this,SLOT(addNote()));

}

void TaskEditor::changeEvent(QEvent *e)
{
    switch(e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}


void TaskEditor::save(){


    this->updateStore();

    if(this->store["id"].asString()==""){
        this->store["command"]="addTask";
    }else{
        this->store["command"]="editTask";
    }
    ServerCommand *command = new ServerCommand(this->store,this);
    command->send();

    connect(command, SIGNAL(saveComplete(Json::Value)), this, SLOT(saveCompleted(Json::Value)));
    qDebug()<<"saved.";
}


void TaskEditor::updateStore(){

    this->store["title"]=this->title->text().toStdString();
    this->store["description"]=this->description->toHtml().toStdString();
    this->store["parentTaskID"]=this->taskCombo->itemData(this->taskCombo->currentIndex()).toString().toStdString();
    this->store["percentComplete"]=QString::number(this->percentComplete->value()).toStdString();
    this->store["estimate"]=QString::number(this->currentEstimate->value()).toStdString();
    this->store["originalEstimate"]=QString::number(this->originalEstimate->value()).toStdString();
    //update status
    this->store["statusID"]=this->statusCombo->itemData(this->statusCombo->currentIndex()).toString().toStdString();
    this->store["status"]=static_cast <qtMushi *>(qApp)->statusDirectory.getStatusByID(this->statusCombo->itemData(this->statusCombo->currentIndex()).toString());
    //update owner
    this->store["ownerID"]=this->ownerCombo->itemData(this->ownerCombo->currentIndex()).toString().toStdString();
}


void TaskEditor::saveCompleted(Json::Value val){
    //this->close();
    if(val.get("taskID","")!=""){
        this->store["id"]=val.get("taskID","");
    }
    this->saveNotes();
    emit saveComplete();
}


void TaskEditor::updateFromStore(){
    this->title->setText(this->store["title"].asString().c_str());
    this->description->setHtml(this->store["description"].asString().c_str());
    qDebug()<<"gotHere";
    this->percentComplete->setValue(this->store.get("percentComplete","0").asInt());

    this->currentEstimate->setValue(this->currentEstimate->valueFromText(this->store.get("estimate","0").asString().c_str()));
    qDebug()<<"gotHere";
    this->originalEstimate->setValue(this->currentEstimate->valueFromText(this->store.get("originalEstimate","0").asString().c_str()));
    for(int x=0;x<this->taskCombo->count();x++){
        if(this->taskCombo->itemData(x).toString().toStdString()==this->store["parentTaskID"].asString()){
            this->taskCombo->setCurrentIndex(x);
        }
    }

    for(int x=0;x<this->statusCombo->count();x++){
        if(this->statusCombo->itemData(x).toString().toStdString()==this->store.get("status","").get("id","").asString()){
            this->statusCombo->setCurrentIndex(x);
        }
    }

    for(int x=0;x<this->ownerCombo->count();x++){
        if(this->ownerCombo->itemData(x).toString().toStdString()==this->store.get("ownerID","").asString()){
            this->ownerCombo->setCurrentIndex(x);
        }
    }
}


void TaskEditor::setStore(Json::Value &s){
    this->store=s;
    //remove all notes
    while(this->notes.length()){
        this->notes.takeFirst()->deleteLater();
    }
    if(this->store.get("id","")!=""){
        ServerCommand *command =new ServerCommand(this);
        command->set("command","getTaskNotes");
        command->set("taskID",this->store.get("id","").asCString());
        this->connect(command,SIGNAL(saveComplete(Json::Value)),this,SLOT(getNotesResponse(Json::Value)));
        command->send();
    }
    this->updateFromStore();
}

void TaskEditor::addNote(){
    TaskComment *note = new TaskComment;
    this->notes.append(note);
    note->store["taskID"]=this->store.get("id","");
    this->noteSplitter->addWidget(note);
}

void TaskEditor::getNotesResponse(Json::Value val){
    Json::Value data=val.get("data","");
    if(data.isArray()){
        for ( int index = 0; index < data.size(); ++index ){
            TaskComment *note = new TaskComment;
            note->store=data[index];
            note->updateFromStore();
            this->notes.append(note);
            note->store["taskID"]=this->store.get("id","");
            this->noteSplitter->addWidget(note);
        }
    }

}

void TaskEditor::saveNotes(){
    for(int x=0;x<notes.length();x++){
        this->notes.at(x)->setParentTaskID(this->store.get("id","").asInt());
        this->notes.at(x)->save();
    }
}

