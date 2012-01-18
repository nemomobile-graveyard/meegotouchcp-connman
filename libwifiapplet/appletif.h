/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef APPLETIF_H
#define APPLETIF_H

#include <DcpAppletIf>
#include <QObject>
class DcpStylableWidget;

class WifiApplet: public QObject, public DcpAppletIf
{
  Q_OBJECT;
  Q_INTERFACES(DcpAppletIf);

public:
  virtual void init();
  virtual DcpStylableWidget* constructStylableWidget(int widgetId);
  virtual QVector<MAction *> viewMenuItems();
  virtual QString title() const;
  virtual DcpBrief* constructBrief(int);
};

#endif //APPLETIF_H
