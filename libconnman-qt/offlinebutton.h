/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef OFFLINEBUTTON_H
#define OFFLINEBUTTON_H

#include "networklistmodel.h"
#include <MButton>

class OfflineButton : public MButton
{
  Q_OBJECT;

public:
  OfflineButton(NetworkListModel *networkListModel,
		QGraphicsItem *parent = 0);
  virtual ~OfflineButton();

private slots:
  void toggledSlot(bool checked);
  void offlineModeChanged(const bool offlineMode);

private:
  OfflineButton();
  NetworkListModel *m_networkListModel;
};

#endif //OFFLINEBUTTON_H
