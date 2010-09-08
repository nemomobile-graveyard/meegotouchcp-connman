/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "addnetworkpage.h"
#include <MAction>
#include <MButton>
#include <QGraphicsLinearLayout>

AddNetworkPage::AddNetworkPage(NetworkListModel *networkListModel, QGraphicsWidget *parent) :
  MApplicationPage(parent), m_layout(NULL), m_gridPolicy(NULL), m_linearPolicy(NULL),
  m_buttonGroup(NULL), m_passwordEdit(NULL), m_passwordLabel(NULL)
{
  setObjectName("AddNetworkPage");
  if (networkListModel) {
    setNetworkListModel(networkListModel);
  }
}

AddNetworkPage::~AddNetworkPage()
{
}

void AddNetworkPage::createContent()
{
  m_layout = new MLayout(centralWidget());
  m_gridPolicy = new MGridLayoutPolicy(m_layout);
  m_linearPolicy = new MLinearLayoutPolicy(m_layout, Qt::Vertical);
  m_layout->setLandscapePolicy(m_gridPolicy);
  m_layout->setPortraitPolicy(m_linearPolicy);

  //Sub heading
  m_header = new MWidgetController();
  m_header->setViewType("background");
  m_header->setObjectName("headerRow");
  {
    QGraphicsLinearLayout *headerLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    m_header->setLayout(headerLayout);

    //% "WIRELESS NETWORK DETAILS"
    MLabel *headerLabel = new MLabel(qtTrId("qtn_add_network_header"));
    headerLayout->addItem(headerLabel);
  }
  m_gridPolicy->addItem(m_header, 0, 0, 1, 2);
  m_linearPolicy->addItem(m_header);


  //% "Network:"
  MLabel *label = new MLabel(qtTrId("qtn_add_network_label"));
  label->setObjectName("AddNetworkNetworkLabel");
  m_gridPolicy->addItem(label, 1, 0);
  m_linearPolicy->addItem(label);

  m_nameEdit = new MTextEdit(MTextEditModel::SingleLine,
			       QString());
  //% "Enter network name here"
  m_nameEdit->setPrompt(qtTrId("qtn_enter_network_name"));
  m_nameEdit->setObjectName("AddNetworkNetworkNameEdit");
  m_gridPolicy->addItem(m_nameEdit, 1, 1);
  m_linearPolicy->addItem(m_nameEdit);

  //security type
  //% "Security:"
  label = new MLabel(qtTrId("qtn_add_network_security_label"));
  label->setObjectName("AddNetworkSecurityLabel");
  m_gridPolicy->addItem(label, 2, 0);
  m_linearPolicy->addItem(label);
  
  //% "Open"
  m_buttons.append(new TypeButton("none", qtTrId("qtn_open")));
  m_buttons[0]->button->setChecked(true);
  //% "WEP"
  m_buttons.append(new TypeButton("wep", qtTrId("qtn_wep")));
  //% "WPA"
  m_buttons.append(new TypeButton("wpa", qtTrId("qtn_wpa")));

  m_buttonGroup = new MButtonGroup();
  QGraphicsLinearLayout *buttonLayout = new QGraphicsLinearLayout(Qt::Horizontal);
  for (int i= 0; i < m_buttons.count(); i++) {
    TypeButton *typeButton = m_buttons[i];
    m_buttonGroup->addButton(typeButton->button, i);
    buttonLayout->addItem(typeButton->button);
  }
  m_gridPolicy->addItem(buttonLayout, 2, 1);
  m_linearPolicy->addItem(buttonLayout);

  //password entry
  //% "Password:"
  m_passwordLabel = new MLabel(qtTrId("qtn_add_network_password_label"));
  m_passwordLabel->setObjectName("AddNetworkPasswordLabel");
  m_gridPolicy->addItem(m_passwordLabel, 3, 0);
  m_linearPolicy->addItem(m_passwordLabel);
  m_passwordLabel->hide();
  m_passwordEdit = new MTextEdit(MTextEditModel::SingleLine,
			       QString());
  //% "Enter password here"
  m_passwordEdit->setPrompt(qtTrId("qtn_add_network_password_edit"));
  m_passwordEdit->setObjectName("AddNetworkPasswordEdit");
  m_passwordEdit->setEchoMode(MTextEditModel::Password);
  m_gridPolicy->addItem(m_passwordEdit, 3, 1);
  m_linearPolicy->addItem(m_passwordEdit);
  m_passwordEdit->hide();

  //% "Apply"
  MAction *action = new MAction(qtTrId("qtn_add_network_apply_action"), this);
  action->setLocation(MAction::ToolBarLocation);
  addAction(action);
  connect(action, SIGNAL(triggered()), this, SLOT(applyTriggered()));
  //% "Cancel"
  action = new MAction(qtTrId("qtn_add_network_cancel_action"), this);
  action->setLocation(MAction::ToolBarLocation);
  addAction(action);
  connect(action, SIGNAL(triggered()), this, SLOT(onBackButtonClicked()));

  // setEscapeButtonMode(MEscapeButtonPanelModel::BackMode);
  connect(this, SIGNAL(backButtonClicked()), SLOT(onBackButtonClicked()));
  connect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));
}

void AddNetworkPage::setNetworkListModel(NetworkListModel *networkListModel)
{
  m_networkListModel = networkListModel;
}

void AddNetworkPage::onBackButtonClicked()
{
  dismiss();
}

void AddNetworkPage::applyTriggered()
{
  const QString &connmanName = m_buttons[m_buttonGroup->checkedId()]->connmanName;
  m_networkListModel->connectService(m_nameEdit->text(),
				     connmanName,
   				     m_passwordEdit->text());
  dismiss();
}


AddNetworkPage::TypeButton::TypeButton(const QString &ConnmanName, const QString &FriendlyName)
{
  connmanName = ConnmanName;
  friendlyName = FriendlyName;
  button = new MButton(friendlyName);
  button->setViewType(MButton::toggleType);
  button->setCheckable(true);
  if (connmanName == "none") {
    button->setChecked(true);
  }
}

AddNetworkPage::TypeButton::~TypeButton()
{
}

void AddNetworkPage::buttonClicked(int id)
{
  if (!id) {
    m_passwordEdit->hide();
    m_passwordLabel->hide();
  } else {
    m_passwordEdit->show();
    m_passwordLabel->show();
  }
}
