
#include "taskFinder.h"

#include "taskeditor.h"
taskFinder::taskFinder(QWidget *parent) : QWidget(parent){
        this->setStyleSheet("QTreeView::branch:has-siblings:!adjoins-item {"
                            "border-image: url(:img/stylesheet-vline.png) 0;"
        "}"
        "QTreeView::branch:has-siblings:adjoins-item {"
        " border-image: url(:img/stylesheet-branch-more.png) 0;"
        "}"

        "QTreeView::branch:!has-children:!has-siblings:adjoins-item {"
        "border-image: url(:img/stylesheet-branch-end.png) 0;"
        "}"

        "QTreeView::branch:has-children:!has-siblings:closed,"
        "QTreeView::branch:closed:has-children:has-siblings {"
                "border-image: none;"
                "image: url(:img/stylesheet-branch-closed.png);"
        "}"

        "QTreeView::branch:open:has-children:!has-siblings,"
        "QTreeView::branch:open:has-children:has-siblings  {"
                "border-image: none;"
                "image: url(:img/stylesheet-branch-open.png);"
        "}");

        this->reply=0;
        treeWidget = new QTreeWidget(this);
        treeWidget->setColumnCount(3);
        QStringList header;

        header<<"Title"<<"Owner"<<"Status";
        treeWidget->setHeaderLabels(header);
        treeWidget->setColumnWidth(0,500);
        layout = new QVBoxLayout();
        QToolBar *toolbar=new QToolBar(this);
        this->hideClosed = new QAction(this);
        hideClosed->setCheckable(true);
        hideClosed->setText("Hide Closed");
        hideClosed->setChecked(true);
        toolbar->addAction(hideClosed);
        toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
        layout->addWidget(toolbar);



        layout->addWidget(treeWidget);




        this->setLayout(layout);
        search();
        connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(itemActivated(QTreeWidgetItem*,int)));
        connect(&static_cast <qtMushi *>(qApp)->taskDirectory,SIGNAL(updated()),this,SLOT(networkResponse()));
        connect(hideClosed,SIGNAL(toggled(bool)),this,SLOT(networkResponse()));
}

void taskFinder::search(){
        //attempt to get all tickets
        static_cast <qtMushi *>(qApp)->taskDirectory.refresh();
}


void taskFinder::search(QString text){
        static_cast <qtMushi *>(qApp)->taskDirectory.refresh();
}

QString html2plaintext(QString string);

void taskFinder::networkResponse(){
        QList<Json::Value> *list;
        list=static_cast <qtMushi *>(qApp)->taskDirectory.getTopLevelTasks();
        treeWidget->clear();
        taskTreeWidgetItem *item;
        for(int index=0;index<list->size();index++){
            qDebug()<<list->at(index).get("isOpen","").asString().c_str();
            //if we are filtering closed tickets and this is closed, don't show it.
            if(this->hideClosed->isChecked() && list->at(index).get("status","").get("isOpen","").asString()!="1" ){
                continue;
            }
            item =  new taskTreeWidgetItem(treeWidget);
            item->setValue(list->at(index));
            item->setText(0,item->getValue().get("title","NULL").asCString());
            item->setText(1,item->getValue().get("owner","").get("firstName","").asCString());
           // item->setText(2,html2plaintext(item->taskValue.get("description","NULL").asCString()).left(100).replace("\n"," "));

            item->setText(2,item->getValue().get("status","NULL").get("name","null").asCString());

            treeWidget->addTopLevelItem(item);
            this->addChildrenToTree(item);
        }
}

void taskFinder::addChildrenToTree(taskTreeWidgetItem *parent){
    QList<Json::Value> *list;
    list=static_cast <qtMushi *>(qApp)->taskDirectory.getChildrenOfTask(parent->getValue());
    taskTreeWidgetItem *item;
    for(int x=0;x<list->count();x++){
        //if we are filtering closed tickets and this is closed, don't show it.

        if(this->hideClosed->isChecked() && list->at(x).get("status","").get("isOpen","").asString()!="1" ){
            continue;
        }
        parent->setExpanded(true);
        item =  new taskTreeWidgetItem();
        item->setValue(list->at(x));
        item->setText(0,item->getValue().get("title","NULL").asCString());
        item->setText(1,item->getValue().get("owner","").get("firstName","").asCString());
       // item->setText(2,html2plaintext(item->taskValue.get("description","NULL").asCString()).left(100).replace("\n"," "));
        item->setText(2,item->getValue().get("status","NULL").get("name","null").asCString());
        parent->addChild(item);
        this->addChildrenToTree(item);
    }
}


QString html2plaintext(QString string){
    QTextEdit *editor=new QTextEdit();
    editor->setHtml(string);
    string = editor->toPlainText();
    delete editor;
    return string;
}

Json::Value taskFinder::getSelectedRecord(){
    // if there isn't a selected item, return a blank value
    if(!this->treeWidget->currentItem()){
        Json::Value blankVal;
        return blankVal;
    }
    taskTreeWidgetItem *item = static_cast<taskTreeWidgetItem *>(this->treeWidget->currentItem());
    return item->getValue();
}

void taskFinder::itemActivated(QTreeWidgetItem *item,int column){
    taskTreeWidgetItem *tItem = static_cast<taskTreeWidgetItem *> (item);
    emit this->taskSelected(tItem->getValue());
   /* TaskEditor *editor=new TaskEditor();
    editor->setStore(tItem->taskValue);
    connect(editor,SIGNAL(saveComplete()),this,SLOT(search()));
    editor->show();*/
}


