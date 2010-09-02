/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "advancedpage.h"
#include "debug.h"
#include <MAction>
#include <MLabel>
#include <QRegExpValidator>

const QString AdvancedPage::Method("Method");
const QString AdvancedPage::Address("Address");
const QString AdvancedPage::Netmask("Netmask");
const QString AdvancedPage::Gateway("Gateway");


AdvancedPage::AdvancedPage(QGraphicsWidget *parent) :
  MApplicationPage(parent),
  m_layout(NULL), m_gridPolicy(NULL), m_linearPolicy(NULL),
  m_NIModel(NULL)
{
  setObjectName("advancedPage");

  MethodName methodName;
  methodName.connmanName = "dchp";
  methodName.friendlyName = "dhcp";
  m_methodNames.append(methodName);
  methodName.connmanName = "manual";
  methodName.friendlyName = "static";
  m_methodNames.append(methodName);

  m_combo = new MComboBox();
  m_combo->setObjectName("advancedSettingsModeCombo");
  for (int i= 0; i < m_methodNames.count(); i++) {
    m_combo->insertItem(i, m_methodNames[i].friendlyName);
  }
  m_combo->setCurrentIndex(0); //dhcp

  m_ipLabel = new MLabel("IP Address:");
  m_ipLabel->setObjectName("advancedSettingsIPLabel");
  m_ipEdit = new MTextEdit();


  m_subnetLabel = new MLabel("Subnet Mask:");
  m_subnetLabel->setObjectName("advancedSettingsSubnetLabel");
  m_subnetEdit = new MTextEdit();

  m_routerLabel = new MLabel("Router:");
  m_routerLabel->setObjectName("advancedSettingsRouterLabel");
  m_routerEdit = new MTextEdit();

  m_dnsEdit = new MTextEdit();

  m_MAC = new MLabel("??:??:??:??:??:??:??:??");
  m_MAC->setObjectName("advancedSettingsMACValue");
}

AdvancedPage::~AdvancedPage()
{
  if (m_NIModel) {
    m_NIModel->decreaseReferenceCount();
  }
}

void AdvancedPage::createContent()
{
  m_layout = new MLayout(centralWidget());
  m_gridPolicy = new MGridLayoutPolicy(m_layout);
  m_linearPolicy = new MLinearLayoutPolicy(m_layout, Qt::Vertical);
  m_layout->setLandscapePolicy(m_gridPolicy);
  m_layout->setPortraitPolicy(m_linearPolicy);

  //FIXME: 666.666.666.666
  QRegExpValidator *rx = new QRegExpValidator(QRegExp("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}"), this);

  MLabel *label;
  int row = 0;
  label = new MLabel("Advanced Settings:");
  label->setObjectName("advancedSettingsPrompt");
  m_gridPolicy->addItem(label, row, 0);
  m_linearPolicy->addItem(label);
  row++;

  //connection method
  label = new MLabel("Connect by:");
  label->setObjectName("advancedSettingsModeLabel");
  m_gridPolicy->addItem(label, row, 0);
  m_linearPolicy->addItem(label);

  m_gridPolicy->addItem(m_combo, row, 1);
  m_linearPolicy->addItem(m_combo);
  row++;

  //ip address
  m_gridPolicy->addItem(m_ipLabel, row, 0);
  m_linearPolicy->addItem(m_ipLabel);

  m_ipEdit->setValidator(rx);
  m_gridPolicy->addItem(m_ipEdit, row, 1);
  m_linearPolicy->addItem(m_ipEdit);
  row++;

  //subnet
  m_gridPolicy->addItem(m_subnetLabel, row, 0);
  m_linearPolicy->addItem(m_subnetLabel);

  m_subnetEdit->setValidator(rx); //FIXME: 123.123.123/24 and other notations
  m_gridPolicy->addItem(m_subnetEdit, row, 1);
  m_linearPolicy->addItem(m_subnetEdit);
  row++;

  //router
  m_gridPolicy->addItem(m_routerLabel, row, 0);
  m_linearPolicy->addItem(m_routerLabel);

  m_routerEdit->setValidator(rx);
  m_gridPolicy->addItem(m_routerEdit, row,1);
  m_linearPolicy->addItem(m_routerEdit);
  row++;

  //dns
  label = new MLabel("DNS:");
  label->setObjectName("advancedSettingsDNSLabel");
  m_gridPolicy->addItem(label, row, 0);
  m_linearPolicy->addItem(label);

  //FIXME: validation
  m_gridPolicy->addItem(m_dnsEdit, row, 1);
  m_linearPolicy->addItem(m_dnsEdit);
  row++;

  //MAC address
  label = new MLabel("Your Mac Address:");
  label->setObjectName("advancedSettingsMACLabel");
  m_gridPolicy->addItem(label, row, 0);
  m_linearPolicy->addItem(label);

  m_gridPolicy->addItem(m_MAC, row, 1);
  m_linearPolicy->addItem(m_MAC);
  row++;

  MAction *action = new MAction("Apply", this);
  action->setLocation(MAction::ToolBarLocation);
  addAction(action);

  comboIndexChanged(m_combo->currentIndex());
  connect(this, SIGNAL(backButtonClicked()), SLOT(onBackButtonClicked()));
  connect(m_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
  connect(action, SIGNAL(triggered()), this, SLOT(applyTriggered()));
}

QStringList AdvancedPage::nameservers() const
{
  QStringList list = m_dnsEdit->text().split(QRegExp(",+\\s*|\\s+"));
  MDEBUG("nameservers: %s", STR(list.join(", ")));
  return list;
}

void AdvancedPage::setNameservers(const QStringList &nameservers)
{
  Q_ASSERT(m_dnsEdit);
  m_dnsEdit->setText(nameservers.join(", "));
}


void AdvancedPage::setIPV4(const NetworkItemModel::IPv4Type &ipv4)
{
  Q_ASSERT(m_combo);
  Q_ASSERT(m_ipEdit);
  Q_ASSERT(m_subnetEdit);
  Q_ASSERT(m_routerEdit);


  m_ipEdit->setText(ipv4.Address);
  m_subnetEdit->setText(ipv4.Netmask);
  m_routerEdit->setText(ipv4.Gateway);

  MDEBUG("ipv4.Method: %s", STR(ipv4.Method));
  for (int i= 0; i < m_methodNames.count(); i++) {
    if (m_methodNames[i].connmanName == ipv4.Method) {
      m_combo->setCurrentIndex(i);
      break;
    }
  }
}

void AdvancedPage::setMAC(const QString &MAC)
{
  if (!MAC.isEmpty()) {
    m_MAC->setText(MAC);
  }
}

void AdvancedPage::setNetworkItemModel(NetworkItemModel *networkItemModel)
{
  Q_ASSERT(networkItemModel);
  if (m_NIModel) {
    disconnect(m_NIModel);
    m_NIModel->decreaseReferenceCount();
  }
  m_NIModel = networkItemModel;
  m_NIModel->increaseReferenceCount();
  setIPV4(m_NIModel->ipv4());
  setNameservers(m_NIModel->nameservers());
  setMAC(m_NIModel->deviceAddress());
  connect(m_NIModel, SIGNAL(modified(const QList<const char*>&)),
	  this, SLOT(modified(const QList<const char*>&)));
}


void AdvancedPage::onBackButtonClicked()
{
  MDEBUG("backButtonClicked");
  dismiss();
}

void AdvancedPage::comboIndexChanged(int index)
{
  if (!index) {
    setIPEditable(false);
    showRouter(false);
  } else {
    setIPEditable(true);
    showRouter(true);
  }

}

void AdvancedPage::applyTriggered()
{
  if (m_NIModel) {
    NetworkItemModel::IPv4Type ipv4;
    ipv4.Method = m_methodNames[m_combo->currentIndex()].connmanName;
    ipv4.Address = m_ipEdit->text();
    ipv4.Netmask = m_subnetEdit->text();
    ipv4.Gateway = m_routerEdit->text();

    m_NIModel->setIpv4(ipv4);
    //m_NIModel->setNameservers(nameservers());
  }
  dismiss();
}

void AdvancedPage::modified(const QList<const char *>& members)
{
  if (members.contains(NetworkItemModel::IPv4)) {
    setIPV4(m_NIModel->ipv4());
  } if (members.contains(NetworkItemModel::Nameservers)) {
    setNameservers(m_NIModel->nameservers());
  } if (members.contains(NetworkItemModel::DeviceAddress)) {
    setMAC(m_NIModel->deviceAddress());
  }

}

void AdvancedPage::showRouter(const bool show)
{
  m_routerLabel->setVisible(show);
  m_routerEdit->setVisible(show);
}

void AdvancedPage::setIPEditable(const bool editable)
{
  m_ipEdit->setReadOnly(!editable);
  m_subnetEdit->setReadOnly(!editable);
}
