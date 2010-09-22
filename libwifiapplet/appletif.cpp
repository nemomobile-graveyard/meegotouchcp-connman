/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "appletif.h"
#include "wifibrief.h"
#include "listwidget.h"
#include <MAction>
#include <MLibrary>
#include <MTheme>
#include <QDebug>


void WifiApplet::init()
{
}

DcpWidget* WifiApplet::constructWidget(int widgetId)
{
  DcpWidget *widget = NULL;

  switch (widgetId) {
  case 0:
    widget = new ListWidget();
    break;
    
  default:
    qDebug() << "don't know widgetid: " << widgetId;
    break;
  }
  return widget;
}

QVector<MAction *> WifiApplet::viewMenuItems()
{
  //we don't intend any having menu items
  return QVector<MAction*>(); 
}

QString WifiApplet::title() const
{
  return QString(); //temp
}

DcpBrief* WifiApplet::constructBrief(int)
{
  return new WifiBrief();
}

Q_EXPORT_PLUGIN2(wifiapplet, WifiApplet)
M_LIBRARY

