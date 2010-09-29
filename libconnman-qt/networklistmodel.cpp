/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "networklistmodel.h"
#include "debug.h"

const QString NetworkListModel::availTechs("AvailableTechnologies");
const QString NetworkListModel::enablTechs("EnabledTechnologies");
const QString NetworkListModel::connTechs("ConnectedTechnologies");
const QString NetworkListModel::OfflineMode("OfflineMode");
const QString NetworkListModel::DefaultTechnology("DefaultTechnology");
const QString NetworkListModel::State("State");

NetworkListModel* NetworkListModel::instance()
{
  static NetworkListModel *that = NULL;

  if (!that) {
    that = new NetworkListModel();
  }
  return that;
}


NetworkListModel::NetworkListModel()
  : QAbstractTableModel(),
    m_manager(NULL),
    m_getPropertiesWatcher(NULL),
    m_connectServiceWatcher(NULL)
{
  m_headerData.append("NetworkItemModel");
  m_headerData.append("Type");
  connectToConnman();
  startTimer(60000);
}

NetworkListModel::~NetworkListModel()
{
}

int NetworkListModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_networks.size();
}

int NetworkListModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_headerData.size();
}

QVariant NetworkListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(orientation);
  if (role == Qt::DisplayRole) {
    if (section < m_headerData.size()) {
      return QVariant(m_headerData.at(section));
    }
  }
  return QVariant();
}


QVariant NetworkListModel::data(const QModelIndex &index, int role) const
{
  //MDEBUG("NetworkListModel::data");
  if (role == Qt::DisplayRole) {
    //MDEBUG("index is at: %d, %d", index.row(), index.column());
    if(index.isValid() && index.row() < m_networks.size()) {
      Q_ASSERT(m_networks[index.row()]);
      if (0 == index.column()) {
	return qVariantFromValue(qobject_cast<QObject*>(m_networks[index.row()]));
      } else if (1 == index.column()) {
	m_networks[index.row()]->dump();
	return QVariant(m_networks[index.row()]->type());
      } else {
	MDEBUG("column %d is out of range", index.column());
      }
    } else {
      MDEBUG("row is out of range, but index is valid");
    }
  } else {
    MDEBUG("role is not display role");
  }
  return QVariant();
}

void NetworkListModel::enableTechnology(const QString &technology)
{
  MDEBUG("enabling technology \"%s\"", STR(technology));
  m_manager->EnableTechnology(technology);
}

void NetworkListModel::disableTechnology(const QString &technology)
{
  m_manager->DisableTechnology(technology);
}

void NetworkListModel::connectService(const QString &name, const QString &security,
				      const QString &passphrase)
{
  Q_ASSERT(m_manager);

  MDEBUG("name: %s", STR(name));
  MDEBUG("security: %s", STR(security));
  MDEBUG("passphrase: %s", STR(passphrase));

  QVariantMap dict;
  dict.insert(QString("Type"), QVariant(QString("wifi")));
  dict.insert(QString("SSID"), QVariant(name));
  dict.insert(QString("Mode"), QVariant(QString("managed")));
  dict.insert(QString("Security"), QVariant(security));
  if (security != QString("none")) {
    dict.insert(QString("Passphrase"), QVariant(passphrase));
  }

  QDBusPendingReply<QDBusObjectPath> reply = m_manager->ConnectService(dict);
  m_connectServiceWatcher = new QDBusPendingCallWatcher(reply, m_manager);
  connect(m_connectServiceWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
	  this,
	  SLOT(connectServiceReply(QDBusPendingCallWatcher*)));
}

const QStringList NetworkListModel::availableTechnologies() const
{
  return qdbus_cast<QStringList>
    (m_propertiesCache[NetworkListModel::availTechs]);
}

const QStringList NetworkListModel::enabledTechnologies() const
{
  return qdbus_cast<QStringList>
      (m_propertiesCache[NetworkListModel::enablTechs]);
}

const QStringList NetworkListModel::connectedTechnologies() const
{
  return qdbus_cast<QStringList>
      (m_propertiesCache[NetworkListModel::connTechs]);
}

void NetworkListModel::connectToConnman()
{
  disconnectFromConnman();
  m_manager = new Manager("org.moblin.connman", "/",
			  QDBusConnection::systemBus(),
			  this);
  if (!m_manager->isValid()) {
    //This shouldn't happen
  //  DCP_CRITICAL("manager is invalid");
    delete m_manager;
  } else {
    QDBusPendingReply<QVariantMap> reply = m_manager->GetProperties();
    m_getPropertiesWatcher = new QDBusPendingCallWatcher(reply, m_manager);
    connect(m_getPropertiesWatcher,
	    SIGNAL(finished(QDBusPendingCallWatcher*)),
	    this,
	    SLOT(getPropertiesReply(QDBusPendingCallWatcher*)));

  }
}

void NetworkListModel::disconnectFromConnman()
{
  if (m_manager) {
    delete m_manager; //we think that m_getPropertiesWatcher will be
		      //deleted due to m_manager getting deleted
    beginResetModel();
    m_manager = NULL;
    foreach(NetworkItemModel *pNIM, m_networks) {
      pNIM->decreaseReferenceCount();
    }
    m_networks.clear();
    endResetModel();
  }
}

void NetworkListModel::getPropertiesReply(QDBusPendingCallWatcher *call)
{
  Q_ASSERT(call);
  QDBusPendingReply<QVariantMap> reply = *call;
  if (reply.isError()) {
    disconnectFromConnman();
    //TODO set up timer to reconnect in a bit
  } else {
    m_propertiesCache = reply.value();
    QList<QDBusObjectPath> services =
      qdbus_cast<QList<QDBusObjectPath> >(m_propertiesCache["Services"]);
    beginInsertRows(QModelIndex(), 0, services.count() - 1);
    foreach (QDBusObjectPath p, services) {
   //   DCP_CRITICAL( QString("service path:\t%1").arg(p.path()).toAscii());
      NetworkItemModel *pNIM = new NetworkItemModel(p.path(), this);
      connect(pNIM, SIGNAL(modified(const QList<const char *>&)),
	      this, SLOT(networkItemModified(const QList<const char *>&)));
      pNIM->increaseReferenceCount();
      m_networks.append(pNIM);
    }
    endInsertRows();
    connect(m_manager,
	    SIGNAL(PropertyChanged(const QString&, const QDBusVariant&)),
	    this,
	    SLOT(propertyChanged(const QString&, const QDBusVariant&)));
    emitTechnologiesChanged();
    emit defaultTechnologyChanged(m_propertiesCache[DefaultTechnology].toString());
    emit stateChanged(m_propertiesCache[State].toString());
  }
}

void NetworkListModel::connectServiceReply(QDBusPendingCallWatcher *call)
{
  Q_ASSERT(call);
  QDBusPendingReply<QDBusObjectPath> reply = *call;
  if (reply.isError()) {
    QDBusError error = reply.error();
    MDEBUG("ERROR!");
    MDEBUG("message: %s", STR(error.message()));
    MDEBUG("name: %s", STR(error.name()));
    MDEBUG("error type: %s", STR(QDBusError::errorString(error.type())));
  } else {
    QDBusObjectPath p = reply.value();
    MDEBUG("object path: %s", STR(p.path()));
  }
}

void NetworkListModel::propertyChanged(const QString &name,
				       const QDBusVariant &value)
{
  MDEBUG("Property \"%s\" changed", STR(name));
  m_propertiesCache[name] = value.variant();
  if (name == NetworkListModel::availTechs ||
      name == NetworkListModel::enablTechs ||
      name == NetworkListModel::connTechs) {
    emitTechnologiesChanged();
  } else if (name == DefaultTechnology) {
    emit defaultTechnologyChanged(m_propertiesCache[DefaultTechnology].toString());
  } else if (name == State) {
    emit stateChanged(m_propertiesCache[State].toString());
  }else if (name == "Services") {
    beginResetModel();
    for (int i= 0; i < m_networks.count(); i++) {
      m_networks[i]->decreaseReferenceCount();
    } 
    m_networks.clear();
    QList<QDBusObjectPath> new_services =
      qdbus_cast<QList<QDBusObjectPath> >(value.variant());
   
    foreach(QDBusObjectPath path, new_services) {
      NetworkItemModel *pNIM = new NetworkItemModel(path.path(), this);
      connect(pNIM, SIGNAL(modified(const QList<const char *>&)),
	      this, SLOT(networkItemModified(const QList<const char *>&)));
      pNIM->increaseReferenceCount();
      m_networks.append(pNIM);
    }
    endResetModel();
  } else if (name == OfflineMode) {
    m_propertiesCache[OfflineMode] = value.variant();
    emit offlineModeChanged(m_propertiesCache[OfflineMode].toBool());
  }
}

 void NetworkListModel::networkItemModified(const QList<const char *> &members)
 {
   //this is a gross hack to keep from doing things again and again we
   //only really care for the sake of the model when the type changes
   //the type changes as a result from the initial getProperties call
   //on the service object after we get the service object paths from
   //the Manager in getProperties
   if (members.contains(NetworkItemModel::Type))
     {
       int row = m_networks.indexOf(static_cast<NetworkItemModel*>(sender()));
       Q_ASSERT(row != -1);
       emit dataChanged(createIndex(row, 0), createIndex(row, 1));
     }
 }

int NetworkListModel::findNetworkItemModel(const QDBusObjectPath &path) const
{
  for (int i= 0; i < m_networks.count(); i++) {
    if (m_networks[i]->servicePath() == path.path()) return i;
  }
  return -1;
}

 void NetworkListModel::emitTechnologiesChanged()
 {
   const QStringList availableTechnologies = qdbus_cast<QStringList>
     (m_propertiesCache[NetworkListModel::availTechs]);
   const QStringList enabledTechnologies = qdbus_cast<QStringList>
     (m_propertiesCache[NetworkListModel::enablTechs]);
   const QStringList connectedTechnologies = qdbus_cast<QStringList>
     (m_propertiesCache[NetworkListModel::connTechs]);
   qDebug() << availTechs << ": " << m_propertiesCache[NetworkListModel::availTechs];
   qDebug() << enablTechs << ": " << m_propertiesCache[NetworkListModel::enablTechs];
   qDebug() << connTechs << ": " << m_propertiesCache[NetworkListModel::connTechs];
   emit technologiesChanged(availableTechnologies,
			    enabledTechnologies,
			    connectedTechnologies);
 }


void NetworkListModel::timerEvent(QTimerEvent *event)
{
  Q_UNUSED(event);
//  DCP_CRITICAL("Dumping list of networks");
  foreach(NetworkItemModel* pNIM, m_networks) {
    pNIM->dump();
  }
}

void NetworkListModel::requestScan()
{
  const QString wifi("wifi");
  m_manager->RequestScan(wifi);
  //FIXME: error returns, etc
}

void NetworkListModel::setOfflineMode(const bool &offlineMode)
{
  Q_ASSERT(m_manager);

    QDBusPendingReply<void> reply =
      m_manager->SetProperty(OfflineMode, QDBusVariant(QVariant(offlineMode)));


#if 0 //It is fundamentally broken to wait here and it doesn't even do
      //anything but crash the app
    reply.waitForFinished();
    if (reply.isError()) {
      MDEBUG("got error from setProperty");
      throw -1;
    }
#endif
}

bool NetworkListModel::offlineMode() const
{
  return m_propertiesCache[OfflineMode].toBool();
}

QString NetworkListModel::defaultTechnology() const
{
  return m_propertiesCache[DefaultTechnology].toString();
}

QString NetworkListModel::state() const
{
  return m_propertiesCache[State].toString();
}
