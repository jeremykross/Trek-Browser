#ifndef RELATIONSHIPMANAGER_H
#define RELATIONSHIPMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QUuid>
#include "Controller.h"
#include "UserData.h"
#include "UserDataFactory.h"
#include "RelationshipData.h"

class RelationshipManager: public QObject {
	Q_OBJECT
	public:
		RelationshipManager();
		~RelationshipManager(); 
	protected slots:
		void controller_modelRequested(const QUuid&);
	protected:
		QMap<QUuid, RelationshipData*> mRelationships; 
};

#endif
