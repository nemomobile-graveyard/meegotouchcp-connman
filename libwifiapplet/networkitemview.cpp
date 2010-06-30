/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "networkitemview.h"
#include "debug.h"
#include <math.h>

NetworkItemView::NetworkItemView(NetworkItem *controller)
  : MWidgetView(controller),
    m_label(NULL),
    m_strengthIcon(NULL)
{
  MDEBUG("NetworkItemView::NetworkItemView");
  setObjectName("networkItemView");
  m_controller = controller;
  m_layout = new QGraphicsLinearLayout(Qt::Horizontal);
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_controller->setLayout(m_layout);

  m_label = new MLabel(m_controller);
  m_strengthIcon = new MImageWidget(m_controller);
  m_strengthIcon->setMinimumSize(54, 54);
  m_strengthIcon->setMaximumSize(54, 54);

  m_label->setObjectName("networkItemLabel");
  m_strengthIcon->setObjectName("networkItemStrengthIcon");

  initWidget();
}

NetworkItemView::~NetworkItemView()
{
}

void NetworkItemView::setLabel(const QString &name,
			       const QString &security,
			       const NetworkItemModel::StateType &state)
{
  QString text = QString("%1 - %2");
  text = text.arg(name);
  if (NetworkItemModel::STATE_IDLE != state) {
    text = text.arg(prettyPrintState(state));
  } else {
    text = text.arg(prettyPrintSecurity(security));
  }
  m_label->setText(text);
}


void NetworkItemView::setStrengthIcon(const int &strength)
{
  QString id;
  if (strength < 25) {
    id = QString("icon-m-common-wlan-strength0");
  } else if (strength >= 25 && strength < 50) {
    id = QString("icon-m-common-wlan-strength2");
  } else if (strength >= 50 && strength < 75) {
    id = QString("icon-m-common-wlan-strength4");
  } else if (strength >= 75 ) {
    id = QString("icon-m-common-wlan-strength5");
  }
  const QSizeF size = m_strengthIcon->sizeHint(Qt::PreferredSize);
  m_strengthIcon->setImage(id, size.toSize());
}


void NetworkItemView::clearLayout()
{
  for(int i=0;i<m_layout->count();i++)
    m_layout->removeAt(0);
}

void NetworkItemView::styleChanged()
{ //never called (not expected to be)
  MWidgetView::applyStyle();

  m_label->setObjectName(style()->labelObjectName());
  m_strengthIcon->setObjectName(style()->strengthObjectName());
}

void NetworkItemView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  MDEBUG("mousePressEvent on %s", STR(model()->dumpToString()));
  m_pressedTime.start();
  style().setModePressed();
  update();
  event->accept();
}

void NetworkItemView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  MDEBUG("relase Event");
  Q_ASSERT(event);
  setDefaultModeDelayed();
  m_controller->click();
  event->accept();
}

void NetworkItemView::cancelEvent(MCancelEvent *event)
{
  MDEBUG("cancelEvent");
  Q_ASSERT(event);
  setDefaultMode();
  event->accept();
}

void NetworkItemView::setDefaultMode()
{
  style().setModeDefault();
  update();
}

void NetworkItemView::setDefaultModeDelayed()
{
 const int minimum_time = 50;
 int elapsed = m_pressedTime.elapsed();
  if (elapsed < minimum_time) {
    QTimer::singleShot(minimum_time - elapsed, this, SLOT(setDefaultMode()));
    MDEBUG("setting timer %d ms to go", minimum_time - elapsed);
  } else {
    setDefaultMode();
  }
}

void NetworkItemView::initWidget()
{
  m_layout->addItem(m_label);
  m_layout->addStretch();
  m_layout->addItem(m_strengthIcon);

  m_label->setVisible(true); //FIXME: required?
}

void NetworkItemView::modelModified(const QList<const char *> &modifications)
{
  const char* member;
  bool labelModified = false;
  for (int i=0; i<modifications.count(); i++) {
    member = modifications[i];
    if ((member == NetworkItemModel::Name ||
	 member == NetworkItemModel::Security ||
	 member == NetworkItemModel::State) &&
	!labelModified) {
      labelModified = true;
    } else if (member == NetworkItemModel::Strength) {
      setStrengthIcon(model()->strength());
    } else {
      MDEBUG("Got unknown model property: %s", member);
    }
  }
  if (labelModified) {
    setLabel(model()->name(),
	     model()->security(),
	     static_cast<NetworkItemModel::StateType>(model()->state()));
  }
}

void NetworkItemView::setupModel()
{
  MWidgetView::setupModel();

  //FIXME: I'm not 100% sure we need this hack.  run through the flow again.
  //hack to get MWidgetController::setupModel() called.
  NetworkItemModel *foo = m_controller->model();
  MDEBUG("foo = %s", STR(foo->name()));

  setLabel(model()->name(),
	   model()->security(),
	   static_cast<NetworkItemModel::StateType>(model()->state()));

  setStrengthIcon(model()->strength());

  //initLayout(static_cast<NetworkItem::LayoutType>(model()->layoutType()));
}


void NetworkItemView::updateData(const QList<const char *> &modifications)
{
  MWidgetView::updateData(modifications);

  modelModified(modifications);
}

const QString& NetworkItemView::prettyPrintState(const NetworkItemModel::StateType &state) const
{
  static const QString idle("Idle");
  static const QString failure("Failure");
  static const QString association("Associating");
  static const QString configuration("Configuring");
  static const QString ready("Connected");
  static const QString login("Login");
  static const QString online("Online");
  static const QString disconnect("Disconnect");

  switch (state) {
  case NetworkItemModel::STATE_IDLE:
    return idle;
    break;

  case NetworkItemModel::STATE_FAILURE:
    return failure;
    break;

  case NetworkItemModel::STATE_ASSOCIATION:
    return association;
    break;

  case NetworkItemModel::STATE_CONFIGURATION:
    return configuration;
    break;

  case NetworkItemModel::STATE_READY:
    return ready;
    break;

  case NetworkItemModel::STATE_LOGIN:
    return login;
    break;

  case NetworkItemModel::STATE_ONLINE:
    return online;
    break;

  case NetworkItemModel::STATE_DISCONNECT:
    return disconnect;
    break;

  default:
    qCritical() << "asked to return prettyprint for unknown state value";
    return failure; //better than crashing
    break;
  }
}

const QString& NetworkItemView::prettyPrintSecurity(const QString &security) const
{
  static const QString none("Open");

  if (security == "none") {
    return none;
  }
  return security;
}


M_REGISTER_VIEW(NetworkItemView, NetworkItem);
