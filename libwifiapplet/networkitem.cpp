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

#include <MWidgetCreator>
#include <MWidgetView>
#include <MDialog>
#include <MLabel>
#include <MTextEdit>
#include <QGraphicsLinearLayout>


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
  //% "Advanced"
  m_advancedAction = new MAction(qtTrId("qtn_network_item_advanced_action"), this);

  m_disconnectAction->setLocation(MAction::ObjectMenuLocation);
  m_removeAction->setLocation(MAction::ObjectMenuLocation);
  m_advancedAction->setLocation(MAction::ObjectMenuLocation);

  addAction(m_disconnectAction);
  addAction(m_removeAction);
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
    m_advancedAction->setVisible(true);
    break;

    //TODO: captive portal for STATE_LOGIN

  default:
    m_disconnectAction->setVisible(false);
    m_removeAction->setVisible(false);
    m_advancedAction->setVisible(false);
    break;
  }
}

void NetworkItem::connectActionSignals(void)
{
  connect(m_disconnectAction, SIGNAL(triggered(bool)),
	  model(), SLOT(disconnectService()));
  connect(m_removeAction, SIGNAL(triggered(bool)),
	  this, SLOT(removeTriggered()));
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

void NetworkItem::connectTriggered()
{
  MDEBUG("connectTriggered on %s", STR(name()));
  if (model()->passphraseRequired()) {
    MDEBUG("password path");

    MWidget *centralWidget = new MWidget;
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    //% "Enter network password for %1"
    MLabel *label = new MLabel(QString(qtTrId("qtn_network_item_enter_network_password_%1")).arg(name()),
				   centralWidget);
    label->setStyleName("CommonBodyTextInverted");
    label->setWordWrap(true);
    MTextEdit *textEdit = new MTextEdit(MTextEditModel::SingleLine,
					    QString(), centralWidget);
    textEdit->setEchoMode(MTextEditModel::Password);
    centralWidget->setLayout(layout);

    layout->addItem(label);
    layout->addItem(textEdit);

    if (dialog != NULL)
      delete dialog;
    //% "%1 - %2"
    dialog = new MDialog(QString(qtTrId("qtn_network_item_network_password_required_dialog_title_%1_%2")).arg(name(), security()),
			   M::OkButton | M::CancelButton);
    dialog->setCentralWidget(centralWidget);
    if (dialog->exec() == MDialog::Accepted) {
      model()->setPassphrase(textEdit->text());
      model()->connectService();
    }
  } else {
    MDEBUG("no pasword path");
    model()->connectService();
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

M_REGISTER_WIDGET(NetworkItem);
