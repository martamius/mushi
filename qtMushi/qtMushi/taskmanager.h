#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "ui_taskmanager.h"
#include "taskFinder.h"
#include "servercommand.h"
class TaskManager : public QWidget, private Ui::TaskManager {
    Q_OBJECT
    Q_DISABLE_COPY(TaskManager)
public:
    explicit TaskManager(QWidget *parent = 0);

protected:
    virtual void changeEvent(QEvent *e);
    taskFinder *finder;

protected slots:
    void newTask();
    void deleteTask();
    void quickSearch();
};

#endif // TASKMANAGER_H
