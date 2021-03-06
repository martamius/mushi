/*
 *  FindTaskCommand.cpp
 *  server
 *
 *  Created by Jordan Wambaugh on 11/10/08.
 *
 */
#include <stdio.h>
#include <sstream>
#include <QStringList>
#include "../../lib_json/value.h"
#include "FindTaskCommand.h"
#include "../MushiServer.h"


Json::Value &FindTaskCommand::run(MushiSession &sess, Json::Value &command, Json::Value &ret, QScriptEngine &engine, MushiDB &db){
	
	
	
	if(command["command"].asString()=="findTask"){
		Json::Value results(Json::arrayValue);
		std::ostringstream query;
		MushiDBResult *r;

		
                query	<< "SELECT t.id, t.title, t.percentComplete, t.estimate, t.createDate"
                                << " , t.reporterID, r.firstName as reporter_firstName, r.lastName as reporter_lastName"
                                << " ,t.ownerId as ownerID, t.parentTaskID,t.dueDate, o.firstName as owner_firstName, o.lastName as owner_lastName"
                                << " ,s.name as status_name, s.isOpen as status_isOpen, s.id as status_id"
                                << " FROM task t"
                                << " LEFT JOIN user r on r.id = t.reporterID"
                                << " LEFT JOIN user o on o.id = t.ownerID"
                                << " LEFT JOIN status s on s.id = t.statusID";

		//printf("%s\n",query.str().c_str());
                QString operat(command.get("operator","AND").asString().c_str());
                if(operat=="AND"){
                    query <<" WHERE 1";
                }

                //query <<"WHERE 1=0";


                QStringList params;
                params.append("title");
                params.append("t.id");
                for(int x=0; x<params.size();x++){
                    QString paramVal(command.get(params.at(x).toStdString(),"").asString().c_str());
                    if(paramVal!=""){

                        query << " AND " << params.at(x).toStdString() << " like '%" << paramVal.toStdString() << "%'";
                    }
                }
                query<<" ORDER BY t.createDate ASC";
                r=db.query(query.str());
                results=r->getNestedJson();
		
		ret["status"]="success";
	
		ret["results"]=results;
		
	}
	return ret;
}
