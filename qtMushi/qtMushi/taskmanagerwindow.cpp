#include "taskmanagerwindow.h"
#include "ui_taskmanagerwindow.h"

TaskManagerWindow::TaskManagerWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::TaskManagerWindow)
{
    m_ui->setupUi(this);
    QVBoxLayout *taskListLayout=new QVBoxLayout;
    taskListLayout->setContentsMargins(1,1,1,1);

    this->finder=new taskFinder(this);
    taskListLayout->addWidget(this->finder);
    this->m_ui->dockWidgetContents->setLayout(taskListLayout);


    this->centralTabWidget=new QTabWidget();
    this->centralTabWidget->setTabsClosable(true);


    HomePage *page = new HomePage();
    this->centralTabWidget->addTab(page,"Home");
    this->m_ui->centralWidgetLayout->addWidget(this->centralTabWidget);
    //this->centralWidgetLayout->addWidget(this->centralTabWidget);
    //this->m_ui->centralwidget->setLayout(this->centralWidgetLayout);


    //make connections
    connect(finder,SIGNAL(taskSelected(Json::Value)),this,SLOT(openTask(Json::Value))); //connect finder activating opening an editor

    connect(this->centralTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeCentralTab(int)));
    connect(this->m_ui->actionNew,SIGNAL(triggered()),this,SLOT(newTask()));
    connect(this->m_ui->actionDelete,SIGNAL(triggered()),this,SLOT(deleteTask()));

}

TaskManagerWindow::~TaskManagerWindow()
{
    delete m_ui;
}


void TaskManagerWindow::openTask(Json::Value value){
    TaskEditor* editor;
    //first look for an already open tab
    for(int x=0;x<this->centralTabWidget->count();x++){
        editor=dynamic_cast<TaskEditor*>(this->centralTabWidget->widget(x));
        if(editor!=NULL){
            if(editor->store==value){
                this->centralTabWidget->setCurrentWidget(editor);
                return;
            }
        }
    }
    editor=new TaskEditor();
    editor->setStore(value);
    connect(editor,SIGNAL(saveComplete()),this->finder,SLOT(search()));
    //editor->show();
    this->centralTabWidget->addTab(editor,value.get("title","").asString().c_str());
    this->centralTabWidget->setCurrentWidget(editor);

    connect(editor,SIGNAL(destroyed(QObject*)),this,SLOT(removeCentralTab(QObject*)));
}


void TaskManagerWindow::removeCentralTab(QObject *page){

    this->centralTabWidget->removeTab(this->centralTabWidget->indexOf(static_cast<QWidget *>(page)));

}




void TaskManagerWindow::closeCentralTab(int index){
    QWidget *page;
    page=this->centralTabWidget->widget(index);
    this->centralTabWidget->removeTab(index);
    page->deleteLater();
}


void TaskManagerWindow::newTask(){
    TaskEditor* task = new TaskEditor();
    connect(task,SIGNAL(saveComplete()),this->finder,SLOT(search()));
    this->centralTabWidget->addTab(task,"New Task");
    this->centralTabWidget->setCurrentWidget(task);

    connect(task,SIGNAL(destroyed(QObject*)),this,SLOT(removeCentralTab(QObject*)));
}


void TaskManagerWindow::deleteTask(){
    ServerCommand *command = new ServerCommand(this);
    Json::Value task = this->finder->getSelectedRecord();

    command->set("command","deleteTask");
    command->set("id",task.get("id","").asString().c_str());
    command->send();
    connect(command,SIGNAL(saveComplete(Json::Value)),this->finder,SLOT(search()));
    connect(command,SIGNAL(saveComplete(Json::Value)),command,SLOT(deleteLater()));
    //find and close any editors of the deleted task
    TaskEditor* editor;
    for(int x=0;x<this->centralTabWidget->count();x++){
        editor=dynamic_cast<TaskEditor*>(this->centralTabWidget->widget(x));
        if(editor!=NULL){
            if(editor->store==task){
                editor->deleteLater();
            }
        }
    }
}
