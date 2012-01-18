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

  methodName.connmanName = "";
  //% "none"
  methodName.friendlyName = qtTrId("qtn_none_friendly_name");
  m_methodNames.append(methodName);

  methodName.connmanName = "dhcp";
  //% "dhcp"
  methodName.friendlyName = qtTrId("qtn_dhcp_friendly_name");
  m_methodNames.append(methodName);

  methodName.connmanName = "manual";
  //% "manual"
  methodName.friendlyName = qtTrId("qtn_manual_friendly_name");
  m_methodNames.append(methodName);

  QString text = "<b>";
  //% "Configuration"
  text += qtTrId("qtn_advanced_settings_prompt");
  text += "<b>";
  m_configurationLabel = new MLabel(text);
  m_configurationLabel->setObjectName("advancedSettingsPrompt");

  //% "Connect by:"
  m_methodLabel = new MLabel(qtTrId("qtn_advanced_settings_connectby_label"));
  m_methodLabel->setObjectName("advancedSettingsMethodLabel");

  m_combo = new MComboBox();
  //% "Method"
  m_combo->setTitle(qtTrId("qtn_advanced_settings_method_label"));
  m_combo->setObjectName("advancedSettingsMethodCombo");
  for (int i= 0; i < m_methodNames.count(); i++) {
    m_combo->insertItem(i, m_methodNames[i].friendlyName);
  }
  m_combo->setCurrentIndex(0); //none
  
  //% "IP address:"
  m_ipLabel = new MLabel(qtTrId("qtn_advanced_settings_ip_label"));
  m_ipLabel->setObjectName("advancedSettingsIPLabel");
  m_ipEdit = new MTextEdit();

  //% "Subnet mask:"
  m_subnetLabel = new MLabel(qtTrId("qtn_advanced_settings_subnet_label"));
  m_subnetLabel->setObjectName("advancedSettingsSubnetLabel");
  m_subnetEdit = new MTextEdit();

  //% "Gateway:"
  m_gatewayLabel = new MLabel(qtTrId("qtn_advanced_settings_gateway_label"));
  m_gatewayLabel->setObjectName("advancedSettingsGatewayLabel");
  m_gatewayEdit = new MTextEdit();

  //% "DNS:"
  m_dnsLabel = new MLabel(qtTrId("qtn_advanced_settings_dns_label"));
  m_dnsLabel->setObjectName("advancedSettingsDNSLabel");
  m_dnsEdit = new MTextEdit();

  //% "MAC address:"
  m_macLabel = new MLabel(qtTrId("qtn_advanced_settings_mac_label"));
  m_macLabel->setObjectName("advancedSettingsMACLabel");
  //% "??:??:??:??:??:??:??:??"
  m_macValue = new MLabel(qtTrId("qtn_advanced_settings_mac_value"));
  m_macValue->setObjectName("advancedSettingsMACValue");

  text = "<b>";
  //% "Active Settings"
  text += qtTrId("qtn_advanced_settings_active_settings_label");
  text += "</b>";
  m_activeSettingsLabel = new MLabel(text);
  m_activeSettingsLabel->setObjectName("advancedSettingsActiveSettingsLabel");

  m_activeMethodLabel = new MLabel(qtTrId("qtn_advanced_settings_connectby_label"));
  m_activeMethodLabel->setObjectName("advancedSettingsActiveMethodLabel");

  m_activeMethodValue = new MLabel();
  m_activeMethodValue->setObjectName("advancedSettingsActiveMethodValue");

  m_activeIpLabel = new MLabel(qtTrId("qtn_advanced_settings_ip_label"));
  m_activeIpLabel->setObjectName("advancedSettingsActiveIpLabel");

  m_activeIpValue = new MLabel();
  m_activeIpValue->setObjectName("advancedSettingsActiveIpValue");

  m_activeMaskLabel = new MLabel(qtTrId("qtn_advanced_settings_subnet_label"));
  m_activeMaskLabel->setObjectName("advancedSettingsActiveMaskLabel");

  m_activeMaskValue = new MLabel();
  m_activeMaskValue->setObjectName("advancedSettingsActiveMaskValue");

  m_activeGatewayLabel = new MLabel(qtTrId("qtn_advanced_settings_gateway_label"));
  m_activeGatewayLabel->setObjectName("advancedSettingsActiveGatewayLabel");

  m_activeGatewayValue = new MLabel();
  m_activeGatewayValue->setObjectName("advancedSettingsActiveGatewayValue");

  m_activeDnsLabel = new MLabel(qtTrId("qtn_advanced_settings_dns_label"));
  m_activeDnsLabel->setObjectName("advancedSettingsActiveDnsLabel");

  m_activeDnsValue = new MLabel();
  m_activeDnsValue->setObjectName("advancedSettingsActiveDnsValue");
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

  int row = 0;

  m_gridPolicy->addItem(m_configurationLabel, row, 0);
  m_linearPolicy->addItem(m_configurationLabel);
  row++;

  // Method
  m_gridPolicy->addItem(m_methodLabel, row, 0);
  m_linearPolicy->addItem(m_methodLabel);

  m_gridPolicy->addItem(m_combo, row, 1);
  m_linearPolicy->addItem(m_combo);
  row++;

  // IP address
  m_gridPolicy->addItem(m_ipLabel, row, 0);
  m_linearPolicy->addItem(m_ipLabel);

  m_ipEdit->setValidator(rx);
  m_gridPolicy->addItem(m_ipEdit, row, 1);
  m_linearPolicy->addItem(m_ipEdit);
  row++;

  // Netmask
  m_gridPolicy->addItem(m_subnetLabel, row, 0);
  m_linearPolicy->addItem(m_subnetLabel);

  m_subnetEdit->setValidator(rx); //FIXME: 123.123.123/24 and other notations
  m_gridPolicy->addItem(m_subnetEdit, row, 1);
  m_linearPolicy->addItem(m_subnetEdit);
  row++;

  // Gateway
  m_gridPolicy->addItem(m_gatewayLabel, row, 0);
  m_linearPolicy->addItem(m_gatewayLabel);

  m_gatewayEdit->setValidator(rx);
  m_gridPolicy->addItem(m_gatewayEdit, row,1);
  m_linearPolicy->addItem(m_gatewayEdit);
  row++;

  // DNS
  m_gridPolicy->addItem(m_dnsLabel, row, 0);
  m_linearPolicy->addItem(m_dnsLabel);

  //FIXME: validation
  m_gridPolicy->addItem(m_dnsEdit, row, 1);
  m_linearPolicy->addItem(m_dnsEdit);
  row++;

  // Active settings
  m_gridPolicy->addItem(m_activeSettingsLabel, row, 0);
  m_linearPolicy->addItem(m_activeSettingsLabel);
  row++;

  // Method
  m_gridPolicy->addItem(m_activeMethodLabel, row, 0);
  m_linearPolicy->addItem(m_activeMethodLabel);
  m_gridPolicy->addItem(m_activeMethodValue, row, 1);
  m_linearPolicy->addItem(m_activeMethodValue);
  row++;

  // Address
  m_gridPolicy->addItem(m_activeIpLabel, row, 0);
  m_linearPolicy->addItem(m_activeIpLabel);
  m_gridPolicy->addItem(m_activeIpValue, row, 1);
  m_linearPolicy->addItem(m_activeIpValue);
  row++;

  // Mask
  m_gridPolicy->addItem(m_activeMaskLabel, row, 0);
  m_linearPolicy->addItem(m_activeMaskLabel);
  m_gridPolicy->addItem(m_activeMaskValue, row, 1);
  m_linearPolicy->addItem(m_activeMaskValue);
  row++;

  // Gateway
  m_gridPolicy->addItem(m_activeGatewayLabel, row, 0);
  m_linearPolicy->addItem(m_activeGatewayLabel);
  m_gridPolicy->addItem(m_activeGatewayValue, row, 1);
  m_linearPolicy->addItem(m_activeGatewayValue);
  row++;

  // DNS
  m_gridPolicy->addItem(m_activeDnsLabel, row, 0);
  m_linearPolicy->addItem(m_activeDnsLabel);
  m_gridPolicy->addItem(m_activeDnsValue, row, 1);
  m_linearPolicy->addItem(m_activeDnsValue);
  row++;

  // MAC
  m_gridPolicy->addItem(m_macLabel, row, 0);
  m_linearPolicy->addItem(m_macLabel);

  m_gridPolicy->addItem(m_macValue, row, 1);
  m_linearPolicy->addItem(m_macValue);
  row++;

  //% "Apply"
  MAction *action = new MAction(qtTrId("advanced_page_apply"), this);
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

void AdvancedPage::setNameserversConfiguration(const QStringList &nameservers)
{
  Q_ASSERT(m_dnsEdit);
  m_dnsEdit->setText(nameservers.join(", "));
}
void AdvancedPage::setNameserversActive(const QStringList &nameservers)
{
  Q_ASSERT(m_activeDnsValue);
  m_activeDnsValue->setText(nameservers.join(", "));
}

void AdvancedPage::setIPV4Configuration(const NetworkItemModel::IPv4Type &ipv4)
{
  Q_ASSERT(m_combo);
  Q_ASSERT(m_ipEdit);
  Q_ASSERT(m_subnetEdit);
  Q_ASSERT(m_gatewayEdit);

  m_ipEdit->setText(ipv4.Address);
  m_subnetEdit->setText(ipv4.Netmask);
  m_gatewayEdit->setText(ipv4.Gateway);

  MDEBUG("ipv4.Method: %s", STR(ipv4.Method));
  for (int i= 0; i < m_methodNames.count(); i++) {
    if (m_methodNames[i].connmanName == ipv4.Method) {
      m_combo->setCurrentIndex(i);
      break;
    }
  }
}

void AdvancedPage::setIPV4Active(const NetworkItemModel::IPv4Type &ipv4)
{
  Q_ASSERT(m_activeMethodValue);
  Q_ASSERT(m_activeIpValue);
  Q_ASSERT(m_activeMaskValue);
  Q_ASSERT(m_activeGatewayValue);

  m_activeMethodValue->setText(ipv4.Method);
  m_activeIpValue->setText(ipv4.Address);
  m_activeMaskValue->setText(ipv4.Netmask);
  m_activeGatewayValue->setText(ipv4.Gateway);
}

void AdvancedPage::setMAC(const QString &MAC)
{
  if (!MAC.isEmpty()) {
    m_macValue->setText(MAC);
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
  setIPV4Configuration(m_NIModel->ipv4Configuration());
  setIPV4Active(m_NIModel->ipv4());
  setNameserversConfiguration(m_NIModel->nameserversConfiguration());
  setNameserversActive(m_NIModel->nameservers());
  setMAC(m_NIModel->deviceAddress());
  connect(m_NIModel, SIGNAL(modified(const QList<const char*>&)),
	  this, SLOT(modified(const QList<const char*>&)));
}

void AdvancedPage::onBackButtonClicked()
{
  dismiss();
}

void AdvancedPage::comboIndexChanged(int index)
{
  // 0 = none, 1 = dhcp, 2 = manual
  if (index == 0) {
    setIpFieldsEditable(false);
    setDnsFieldEditable(false);
  }
  else if (index == 1) {
    setIpFieldsEditable(false);
    setDnsFieldEditable(true);
  } else {
    setIpFieldsEditable(true);
    setDnsFieldEditable(true);
  }
}

void AdvancedPage::applyTriggered()
{
  if (m_NIModel) {
    NetworkItemModel::IPv4Type ipv4;
    ipv4.Method = m_methodNames[m_combo->currentIndex()].connmanName;
    ipv4.Address = m_ipEdit->text();
    ipv4.Netmask = m_subnetEdit->text();
    ipv4.Gateway = m_gatewayEdit->text();

    m_NIModel->setIpv4Configuration(ipv4);
    m_NIModel->setNameserversConfiguration(nameservers());
  }
  dismiss();
}

void AdvancedPage::modified(const QList<const char *>& members)
{
  if (members.contains(NetworkItemModel::IPv4Configuration)) {
    setIPV4Configuration(m_NIModel->ipv4Configuration());
  } 
  if (members.contains(NetworkItemModel::IPv4)) {
    setIPV4Active(m_NIModel->ipv4());
  }
  if (members.contains(NetworkItemModel::NameserversConfiguration)) {
    setNameserversConfiguration(m_NIModel->nameservers());
  } 
  if (members.contains(NetworkItemModel::Nameservers)) {
    setNameserversActive(m_NIModel->nameservers());
  } 
  if (members.contains(NetworkItemModel::DeviceAddress)) {
    setMAC(m_NIModel->deviceAddress());
  }

}

void AdvancedPage::setIpFieldsEditable(const bool editable)
{
  m_ipEdit->setEnabled(editable);
  m_subnetEdit->setEnabled(editable);
  m_gatewayLabel->setEnabled(editable);
  m_gatewayEdit->setEnabled(editable);
}

void AdvancedPage::setDnsFieldEditable(const bool editable)
{
  m_dnsEdit->setEnabled(editable);
}

