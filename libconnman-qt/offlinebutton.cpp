/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "offlinebutton.h"


OfflineButton::OfflineButton(NetworkListModel *networkListModel,
			     QGraphicsItem *parent) :
  MButton(parent), m_networkListModel(networkListModel)
{
  setViewType(MButton::switchType);
  if (m_networkListModel->offlineMode()) {
    setChecked(true);
  } else {
    setChecked(false);
  }
  connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
  connect(m_networkListModel, SIGNAL(offlineModeChanged(bool)),
	  this, SLOT(offlineModeChanged(bool)));

}

OfflineButton::~OfflineButton()
{
}

void OfflineButton::toggledSlot(bool checked)
{
  m_networkListModel->setOfflineMode(checked);
}

void OfflineButton::offlineModeChanged(const bool offlineMode)
{
  setChecked(offlineMode);
}
