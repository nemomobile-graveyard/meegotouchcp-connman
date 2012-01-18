/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "networkitem.h"
#include "networkitemmodel.h"
#include "debug.h"
#include "advancedpage.h"
#include "listwidget.h"

#include <MWidgetCreator>
#include <MWidgetView>
#include <MDialog>
#include <MLabel>
#include <MTextEdit>
#include <QGraphicsLinearLayout>

#include <MBasicSheetHeader>
#include <QAction>

const MTheme::ViewType NetworkItem::defaultType = "";

int NetworkItem::instances = 0;
int NetworkItem::idCounter = 0;
MDialog* NetworkItem::dialog = 0;

NetworkItem::NetworkItem(MWidget *parent)
  :  MWidgetController( new NetworkItemModel(), parent),
     m_disconnectAction(0), m_removeAction(0), m_advancedAction(0),
     id(idCounter)
{
  instances++;
  idCounter++;
  MDEBUG("there are %d instances of NetworkItem around", instances);

  //% "Disconnect"
  m_disconnectAction = new MAction(qtTrId("qtn_network_item_disconnect_action"), this);
  //% "Remove"
  m_removeAction = new MAction(qtTrId("qtn_network_item_remove_action"), this);
  //% "Clear password"
  m_clearPassphraseAction = new MAction(qtTrId("qtn_network_item_clear_password_action"), this);
  //% "Advanced"
  m_advancedAction = new MAction(qtTrId("qtn_network_item_advanced_action"), this);

  m_disconnectAction->setLocation(MAction::ObjectMenuLocation);
  m_removeAction->setLocation(MAction::ObjectMenuLocation);
  m_clearPassphraseAction->setLocation(MAction::ObjectMenuLocation);
  m_advancedAction->setLocation(MAction::ObjectMenuLocation);

  addAction(m_disconnectAction);
  addAction(m_removeAction);
  addAction(m_clearPassphraseAction);
  addAction(m_advancedAction);

  setActionsByState();
  connectActionSignals();
}

NetworkItem::~NetworkItem()
{
  MDEBUG("NetworkItem::~NetworkItem");
  instances--;
}

QString NetworkItem::name() const
{
  return model()->name();
}

QString NetworkItem::security() const
{
  return model()->security();
}

int NetworkItem::strength() const
{
  return model()->strength();
}

NetworkItemModel::StateType NetworkItem::state() const
{
  return static_cast<NetworkItemModel::StateType>(model()->state());
}


void NetworkItem::click()
{
  //FIXME: not for all clicks (context menu)
  if (model()->state() != NetworkItemModel::STATE_READY) {
    connectTriggered();
    MDEBUG("click!");
    emit clicked();
  }
}

QVariant NetworkItem::itemChange(GraphicsItemChange change, const QVariant &val)
{
    // if (change == QGraphicsItem::ItemSelectedHasChanged)
    //   model()->setSelected(val.toBool());
    return QGraphicsItem::itemChange(change, val);
}

void NetworkItem::setActionsByState(void)
{
  MDEBUG("setActionsByState on model: %s", STR(model()->dumpToString()));
  switch (model()->state()) {
  case NetworkItemModel::STATE_ASSOCIATION:
  case NetworkItemModel::STATE_CONFIGURATION:
  case NetworkItemModel::STATE_READY:
  case NetworkItemModel::STATE_ONLINE:
  case NetworkItemModel::STATE_LOGIN:
    MDEBUG("setActionsByState:visible!");
    m_disconnectAction->setVisible(true);
    m_removeAction->setVisible(true);
    m_clearPassphraseAction->setVisible(true);
    m_advancedAction->setVisible(true);
    break;

    //TODO: captive portal for STATE_LOGIN

  default:
    // NOTE. ALL actions are now possible at all times.
    // Logic: let ConnMan handle the commands as it sees
    // best in the different connection state scenarios.
    m_disconnectAction->setVisible(true);
    m_removeAction->setVisible(true);
    m_clearPassphraseAction->setVisible(true);
    m_advancedAction->setVisible(true);
    break;
  }
}

void NetworkItem::connectActionSignals(void)
{
  connect(m_disconnectAction, SIGNAL(triggered(bool)),
	  model(), SLOT(disconnectService()));
  connect(m_removeAction, SIGNAL(triggered(bool)),
	  this, SLOT(removeTriggered()));
  connect(m_clearPassphraseAction, SIGNAL(triggered(bool)),
	  this, SLOT(clearPassphraseTriggered()));
  connect(m_advancedAction, SIGNAL(triggered(bool)),
	  this, SLOT(advancedTriggered()));
}

void NetworkItem::updateData (const QList< const char * > &modifications)
{
  foreach (const char* modification, modifications) {
    MDEBUG("modified: %s", modification);
  }
  if (modifications.contains(NetworkItemModel::State)) {
    setActionsByState();
  }
}

void NetworkItem::setupModel()
{
  MDEBUG("NetworkItem::setupModel");
  MWidgetController::setupModel();

  m_disconnectAction->disconnect(SIGNAL(triggered(bool)));
  m_removeAction->disconnect(SIGNAL(triggered(bool)));
  m_clearPassphraseAction->disconnect(SIGNAL(triggered(bool)));
  m_advancedAction->disconnect(SIGNAL(triggered(bool)));
  setActionsByState();
  connectActionSignals();
}

void NetworkItem::removeTriggered()
{
  MDEBUG("remove triggered");

    MWidget *centralWidget = new MWidget;
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    //% "Do you want to remove the network %1?"
    MLabel *label = new MLabel(QString(qtTrId("qtn_network_item_remove_question_%1")).arg(name()),
				   centralWidget);
    label->setStyleName("CommonBodyTextInverted");
    label->setWordWrap(true);
    centralWidget->setLayout(layout);

    layout->addItem(label);

    if (dialog != NULL)
      delete dialog;
    //% "Remove Device"
    dialog = new MDialog(qtTrId("qtn_network_item_remove_dialog_title"),
			   M::OkButton | M::CancelButton);
    dialog->setCentralWidget(centralWidget);
    if (dialog->exec() == MDialog::Accepted) {
      model()->removeService();
    }
}

void NetworkItem::clearPassphraseTriggered()
{
  model()->clearPassphrase();
}

void NetworkItem::connectTriggered()
{
  MDEBUG("connectTriggered on %s", STR(name()));

  // Used to allow the network item model access the network list model
  ListWidget* listWidget = static_cast<ListWidget*>(parentItem()->parentItem());
  
  if (model()->passphraseRequired()) {
    MDEBUG("password path");
    //% "Password for %1 (%2)"
    LoginSheet *loginSheet = 
      new LoginSheet(QString(qtTrId("qtn_network_item_enter_network_password_%1_%2")).arg(name(), security()),
		     model(), listWidget->networkListModel());
    loginSheet->setObjectName("loginSheet");
    loginSheet->appear(scene(), MSceneWindow::DestroyWhenDone);
  } else {
    MDEBUG("no pasword path");
    model()->connectService(listWidget->networkListModel());
  }
}

void NetworkItem::advancedTriggered()
{
  MDEBUG("advanced triggered");
  AdvancedPage *ap = new AdvancedPage();
  ap->setNetworkItemModel(model());
  ap->appear(MSceneWindow::DestroyWhenDone);
}

void NetworkItemCellCreator::updateCell(const QModelIndex& index,
					MWidget * cell) const
{
  do {
    if (!index.isValid()) {
      MWARNING("got an invalid index!");
      break;
    }
    if (NULL == cell) {
      MWARNING("passed an invalid widget!");
      break;
    }
    NetworkItem *card = qobject_cast<NetworkItem *>(cell);
    QVariant data = index.data(Qt::DisplayRole);
    QObject* qdata = data.value<QObject*>();
    if (qdata == NULL) {
      //when using the filter, we sometimes get NULL here.  I don't
      //know why, but we just bail when this happens
      break;
    }
    NetworkItemModel *pNIM = qobject_cast<NetworkItemModel*>(data.value<QObject*>());
    Q_ASSERT(pNIM);
    card->setModel(pNIM);
  } while (0);
}

LoginSheet::LoginSheet(QString const &title, 
		       NetworkItemModel* itemModel, 
		       NetworkListModel* listModel) :
  m_itemModel(itemModel), 
  m_listModel(listModel)
{
  createCentralWidget(title);
  createHeaderWidget();

  connect(this, SIGNAL(appeared()), SLOT(setfocusOnPasswordTextEdit()));
}

void LoginSheet::createCentralWidget(QString const &title)
{
  QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical,
                                                                  centralWidget());
  mainLayout->setContentsMargins(0,0,0,0);
  mainLayout->setSpacing(0);

  MLabel *label = new MLabel(title);
  label->setObjectName("PasswordLabel");
  mainLayout->addItem(label);

  m_passwordTextEdit = new MTextEdit;
  m_passwordTextEdit->setObjectName("passwordTextEdit");
  m_passwordTextEdit->setEchoMode(MTextEditModel::Password);
  mainLayout->addItem(m_passwordTextEdit);

  mainLayout->addStretch();
}

void LoginSheet::createHeaderWidget()
{
  MBasicSheetHeader *basicHeader = new MBasicSheetHeader(this);
  basicHeader->setObjectName("basicSheetHeader");

  //% "Cancel"
  basicHeader->setNegativeAction(new QAction(qtTrId("qtn_cancel"), basicHeader));
  //% "OK"
  basicHeader->setPositiveAction(new QAction(qtTrId("qtn_ok"), basicHeader));

  connect(basicHeader->negativeAction(), SIGNAL(triggered()), SLOT(cancel()));
  connect(basicHeader->positiveAction(), SIGNAL(triggered()), SLOT(ok()));

  setHeaderWidget(basicHeader);
}

void LoginSheet::setfocusOnPasswordTextEdit()
{
  m_passwordTextEdit->setFocus();
}

void LoginSheet::ok()
{
  m_itemModel->setPassphrase(m_passwordTextEdit->text());
  m_itemModel->connectService(m_listModel);

  dismiss();
}

void LoginSheet::cancel()
{
  dismiss();
}

M_REGISTER_WIDGET(NetworkItem);
