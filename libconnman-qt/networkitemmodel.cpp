/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "networkitemmodel.h"
#include "debug.h"
#include "commondbustypes.h"

const char* const NetworkItemModel::Name = "Name";
const char* const NetworkItemModel::Security = "Security";
const char* const NetworkItemModel::Strength = "Strength";
const char* const NetworkItemModel::State = "State";
const char* const NetworkItemModel::Type = "Type";
const char* const NetworkItemModel::PassphraseRequired = "PassphraseRequired";
const char* const NetworkItemModel::Passphrase = "Passphrase";
const char* const NetworkItemModel::IPv4 = "IPv4.Configuration";
const char* const NetworkItemModel::Nameservers = "Nameservers";
const char* const NetworkItemModel::DeviceAddress = "DeviceAddress";
const char* const NetworkItemModel::Mode = "Mode";

int NetworkItemModel::instances = 0;
int NetworkItemModel::idCounter = 0;

NetworkItemModel::NetworkItemModel(const QString &path, QObject *parent) :
  MWidgetModel(),
  id(idCounter),
  m_service(NULL),
  m_getPropertiesWatcher(NULL),
  m_setPropertyWatcher(NULL),
  m_disconnectWatcher(NULL),
  m_connectWatcher(NULL),
  m_state(STATE_NONE),
  m_strength(0),
  m_passphraseRequired(false)
{
  //setParent(parent);
  instances++;
  idCounter++;
  m_servicePath = path;
  if (!path.isEmpty()) {
    m_service = new Service("net.connman", path,
			    QDBusConnection::systemBus(),
			    this);
    if (!m_service->isValid()) {
      MDEBUG("service %s is invalid!", STR(path));
      throw -1; //FIXME
    }
    QDBusPendingReply<QVariantMap> reply = m_service->GetProperties();
    m_getPropertiesWatcher = new QDBusPendingCallWatcher(reply, this);
    connect(m_getPropertiesWatcher,
	    SIGNAL(finished(QDBusPendingCallWatcher*)),
	    this,
	    SLOT(getPropertiesReply(QDBusPendingCallWatcher*)));
  }

  //this is an attempt to avoid having lots of networks show in the
  //list with no type.  The thought process is that the filter is
  //matching on empty strings.  It doesn't appear to work in the
  //nothing in the list except one thing problem.  But other problems
  //haven't been tried
  m_type = "????";
}

NetworkItemModel::~NetworkItemModel()
{
  instances--;
  MDEBUG("destructing instance %d.  There are %d models around", id, instances);
}


void NetworkItemModel::connectService()
{
#ifdef PRETEND
  MDEBUG("connectService pretending");
#else
  MDEBUG("connectService FOR REAL");
  m_service->Connect();
#endif
}

void NetworkItemModel::disconnectService()
{
#ifdef PRETEND
  MDEBUG("disconnectService pretending");
#else
  MDEBUG("disconnectService FOR REAL");
  m_service->Disconnect();
#endif
}

void NetworkItemModel::removeService()
{
#ifdef PRETEND
  MDEBUG("removeService pretending");
#else
  MDEBUG("removeService FOR REAL");
  m_service->Remove();
#endif
}

const QString& NetworkItemModel::name() const
{
  return m_name;
}

const QString& NetworkItemModel::security() const
{
  return m_security;
}

const NetworkItemModel::StateType& NetworkItemModel::state() const
{
  return m_state;
}

const int& NetworkItemModel::strength() const
{
  return m_strength;
}

const QString& NetworkItemModel::type() const
{
  return m_type;
}

const bool& NetworkItemModel::passphraseRequired() const
{
  return m_passphraseRequired;
}

const QString& NetworkItemModel::passphrase() const
{
  return m_passphrase;
}

const NetworkItemModel::IPv4Type& NetworkItemModel::ipv4() const
{
  return m_ipv4;
}

const QString& NetworkItemModel::mode() const
{
  return m_mode;
}

void NetworkItemModel::_setName(const QString &name)
{
  if (m_name != name) {
  m_name = name;
  memberModified(Name);
  }
}

void NetworkItemModel::_setSecurity(const QString &security)
{
  if (m_security != security) {
  m_security = security;
  memberModified(Security);
  }
}

void NetworkItemModel::_setState(const NetworkItemModel::StateType &state)
{
  if (m_state != state) {
    m_state = state;
    memberModified(State);
  }
}

void NetworkItemModel::_setState(const int &state)
{
  _setState(static_cast<NetworkItemModel::StateType>(state));
}


void NetworkItemModel::_setStrength(const int &strength)
{
  if (m_strength != strength) {
    m_strength = strength;
    memberModified(Strength);
  }
}

void NetworkItemModel::_setType(const QString &type)
{
  if (m_type != type) {
    m_type = type;
    if (type == "ethernet") {
      _setName("Ethernet"); //hack
    }
    memberModified(Type);
  }
}

void NetworkItemModel::_setPassphraseRequired(const bool &passphraseRequired)
{
  if (m_passphraseRequired != passphraseRequired) {
    m_passphraseRequired = passphraseRequired;
    memberModified(PassphraseRequired);
  }
}

void NetworkItemModel::_setPassphrase(const QString &passphrase)
{
  if (m_passphrase != passphrase) {
    m_passphrase = passphrase;
    memberModified(Passphrase);
  }
}

void NetworkItemModel::setPassphrase(const QString &passphrase)
{
  Q_ASSERT(m_service);
  QDBusPendingReply<void> reply =
    m_service->SetProperty("Passphrase", QDBusVariant(QVariant(passphrase)));
  reply.waitForFinished(); //FIXME: BAD
  if (reply.isError()) {
    MDEBUG("got error from setProperty");
    throw -1; //FIXME: don't throw
  }
  _setPassphrase(passphrase);
}

void NetworkItemModel::setIpv4(const IPv4Type &ipv4)
{
  Q_ASSERT(m_service);


  StringMap dict;
  Q_ASSERT(!ipv4.Method.isEmpty());
  dict.insert("Method", ipv4.Method);
  if (ipv4.Method != "dhcp") {
    //FIXME: what do to if address and such are empty!?!
    dict.insert("Address", ipv4.Address);
    dict.insert("Netmask", ipv4.Netmask);
    dict.insert("Gateway", ipv4.Gateway);
  }
  QVariant variant = qVariantFromValue(dict);
  QDBusPendingReply<void> reply =
    m_service->SetProperty(QString(IPv4), QDBusVariant(variant));
  if (m_setPropertyWatcher) {
    delete m_setPropertyWatcher;
  }
  m_setPropertyWatcher = new QDBusPendingCallWatcher(reply, this);
  connect(m_setPropertyWatcher,
	  SIGNAL(finished(QDBusPendingCallWatcher*)),
	  this,
	  SLOT(setPropertyFinished(QDBusPendingCallWatcher*)));
}


const QString& NetworkItemModel::servicePath() const
{
  return m_servicePath;
}

const QStringList& NetworkItemModel::nameservers() const
{
  return m_nameservers;
}

const QString& NetworkItemModel::deviceAddress() const
{
  return m_deviceAddress;
}

void NetworkItemModel::setNameservers(const QStringList &nameservers)
{
  Q_ASSERT(m_service);

  if (!isListEqual(m_nameservers, nameservers)) {
    QVariant variant = qVariantFromValue(nameservers);
    QDBusPendingReply<void> reply =
      m_service->SetProperty(Nameservers, QDBusVariant(variant));
      //I hate to wait here, but I'm not sure what else to do
      reply.waitForFinished();
      if (reply.isError()) {
	MDEBUG("got error from setProperty");
	throw -1; //FIXME: don't throw
      }
  }
  memberModified(Nameservers);
}

void NetworkItemModel::_setState(const QString &state)
{
  NetworkItemModel::StateType _state;
  if (state == "idle") {
    _state = STATE_IDLE;
  } else if (state == "failure") {
    _state = STATE_FAILURE;
  } else if (state == "association") {
    _state = STATE_ASSOCIATION;
  } else if (state == "configuration") {
    _state = STATE_CONFIGURATION;
  } else if (state == "ready") {
    _state = STATE_READY;
  } else if (state == "login") {
    _state = STATE_LOGIN;
  } else if (state == "online") {
    _state = STATE_ONLINE;
  } else if (state == "disconnect") {
    _state = STATE_DISCONNECT;
  } else {
    _state = STATE_NONE;
    MDEBUG("setting to to STATE_NONE because of unknown state returned: \"%s\"", STR(state));
  }
  _setState(_state);
}

//This sets the m_ipv4 with data.  It does not set the data on the
//Service object
void NetworkItemModel::_setIpv4(const QVariantMap &ipv4)
{
  bool modified = false;
  QString string;

  string = ipv4["Method"].toString();
  MDEBUG("Method: %s", STR(string));
  if (m_ipv4.Method != string) {
    m_ipv4.Method = string;
    modified = true;
  }
  string = ipv4["Address"].toString();
  MDEBUG("Address: %s", STR(string));
  if (m_ipv4.Address != string) {
    m_ipv4.Address = string;
    modified = true;
  }
  string =  ipv4["Netmask"].toString();
  MDEBUG("Netmask: %s", STR(string));
  if (m_ipv4.Netmask != string) {
    m_ipv4.Netmask = string;
    modified = true;
  }
  string = ipv4["Gateway"].toString();
  MDEBUG("Gateway: %s", STR(string));
  if (m_ipv4.Gateway != string) {
    m_ipv4.Gateway = string;
    modified = true;
  }
  if (modified) {
    memberModified(IPv4);
  }
}

void NetworkItemModel::_setNameservers(const QStringList &nameservers)
{
  MDEBUG("nameservers: %s", STR(nameservers.join(", ")));
  m_nameservers = nameservers;
  memberModified(Nameservers);
}

void NetworkItemModel::_setDeviceAddress(const QString &deviceAddress)
{
  if (deviceAddress != m_deviceAddress) {
    MDEBUG("\tAddress = %s", STR(deviceAddress));
    m_deviceAddress = deviceAddress;
    memberModified(DeviceAddress);
  }
}

void NetworkItemModel::_setDeviceAddress(const QVariantMap &ethernet)
{
  if (ethernet.contains("Address")) {
    _setDeviceAddress(ethernet["Address"].toString());
  }
}

void NetworkItemModel::_setMode(const QString &mode)
{
  if (m_mode != mode) {
    m_mode = mode;
    memberModified(Mode);
  }
}

bool NetworkItemModel::isListEqual(const QStringList &a, const QStringList &b) const
{
  if (a.count() != b.count()) {
    return false;
  }
  for (int i=0; i < a.count(); i++) {
    if (a.at(i) != b.at(i)) {
      return false;
    }
  }
  return true;
}



void NetworkItemModel::getPropertiesReply(QDBusPendingCallWatcher *call)
{
  QDBusPendingReply<QVariantMap> reply = *call;
  if (reply.isError()) {
    MDEBUG("getPropertiesReply is error!");
    QDBusError error = reply.error();
    MDEBUG("service: %s", STR(servicePath()));
    /*MDEBUG(QString("type: %1 name: %2 message: %3").
		 arg(QDBusError::errorString(error.type()))
		 .arg(error.name())
		 .arg(error.message()).toAscii());*/
    throw -1; //FIXME
  }
  QVariantMap properties = reply.value();
  //although it seems dangerous as some of these properties are not
  //present, grabbing them is not, because QMap will insert the
  //default value into the map if it isn't present.  That's "" for
  //strings and 0 for ints/bools
  beginTransaction();
  _setName(qdbus_cast<QString>(properties[Name]));
  _setType(qdbus_cast<QString>(properties[Type]));
  _setMode(qdbus_cast<QString>(properties[Mode]));
  _setSecurity(qdbus_cast<QString>(properties[Security]));
  _setPassphraseRequired(qdbus_cast<bool>(properties[PassphraseRequired]));
  _setPassphrase(qdbus_cast<QString>(properties[Passphrase]));
  _setStrength(qdbus_cast<int>(properties[Strength]));
  _setState(qdbus_cast<QString>(properties[State]));
  _setIpv4(qdbus_cast<QVariantMap>(properties["IPv4"]));
  _setNameservers(qdbus_cast<QStringList>(properties[Nameservers]));
  _setDeviceAddress(qdbus_cast<QVariantMap>(properties["Ethernet"]));
  commitTransaction();
  connect(m_service,
	  SIGNAL(PropertyChanged(const QString&,
				 const QDBusVariant &)),
	  this,
	  SLOT(propertyChanged(const QString&,
			       const QDBusVariant &)));

}

void NetworkItemModel::propertyChanged(const QString &name,
				       const QDBusVariant &value)
{
  MDEBUG("property %s changed", STR(name));
    if (name == State) {
      _setState(value.variant().toString());
    } else if (name == Name) {
      _setName(value.variant().toString());
    } else if (name == Type) {
      _setType(value.variant().toString());
    } else if (name == Mode) {
      _setMode(value.variant().toString());
    } else if (name == Security) {
      _setSecurity(value.variant().toString());
    } else if (name == PassphraseRequired) {
      _setPassphraseRequired(value.variant().toBool());
    } else if (name == Passphrase) {
      _setPassphrase(value.variant().toString());
    } else if (name == Strength) {
      _setStrength(value.variant().toInt());
    } else if (name == IPv4 || name == "IPv4") {
      _setIpv4(qdbus_cast<QVariantMap>(value.variant()));
    } else if (name == Nameservers) {
      _setNameservers(value.variant().toStringList());
    } else if (name == "Ethernet") {
      _setDeviceAddress(qdbus_cast<QVariantMap>(value.variant()));
    } else {
      MDEBUG("We don't do anything with property: %s", STR(name));
    }
}


void NetworkItemModel::timerEvent(QTimerEvent *event)
{
  Q_UNUSED(event);
  //MDEBUG("hello!");
  //setStrength(rand()*100.0/RAND_MAX);
}

void NetworkItemModel::dump() const
{
  MDEBUG("%s", STR(dumpToString()));
}

QString NetworkItemModel::dumpToString() const
{
  return QString("id: %1 name: %2 state: %3 type: %4: path: %5").arg(id).arg(m_name).arg(m_state).arg(type()).arg(m_servicePath);
}

void NetworkItemModel::setPropertyFinished(QDBusPendingCallWatcher *call)
{
  QDBusPendingReply<void> reply = *call;
  if (reply.isError()) {
    MDEBUG("not continuing because of error in setProperty!");
  } else {
    QDBusPendingReply<void> nextReply = m_service->Disconnect();
    if (m_setPropertyWatcher) {
      delete m_setPropertyWatcher;
    }
    m_setPropertyWatcher = new QDBusPendingCallWatcher(nextReply, this);
    connect(m_setPropertyWatcher,
	    SIGNAL(finished(QDBusPendingCallWatcher*)),
	    this,
	    SLOT(disconnectFinished(QDBusPendingCallWatcher*)));
  }
}

void NetworkItemModel::disconnectFinished(QDBusPendingCallWatcher *call)
{
  QDBusPendingReply<void> reply = *call;
  if (reply.isError()) {
    MDEBUG("not continuing because of error in disconnect!");
  } else {
    QDBusPendingReply<void> nextReply = m_service->Connect();
    if (m_setPropertyWatcher) {
      delete m_setPropertyWatcher;
    }
    m_disconnectWatcher = new QDBusPendingCallWatcher(nextReply, this);
    connect(m_disconnectWatcher,
	    SIGNAL(finished(QDBusPendingCallWatcher*)),
	    this,
	    SLOT(connectFinished(QDBusPendingCallWatcher*)));
  }
}

void NetworkItemModel::connectFinished(QDBusPendingCallWatcher *call)
{
  QDBusPendingReply<void> reply = *call;
  if (reply.isError()) {
    MDEBUG("error calling connect!");
  } else {
    MDEBUG("connect finished without error");
  }
}

