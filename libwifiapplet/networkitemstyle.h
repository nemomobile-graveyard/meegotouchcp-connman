/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef NETWORK_ITEM_STYLE_H
#define NETWORK_ITEM_STYLE_H

#include <mwidgetstyle.h>

class NetworkItemStyle : public MWidgetStyle
{
  Q_OBJECT;

  M_STYLE (NetworkItemStyle);

  M_STYLE_ATTRIBUTE (QString, labelObjectName, LabelObjectName);
  M_STYLE_ATTRIBUTE (QString, strengthObjectName, StrengthObjectName);
};

class NetworkItemStyleContainer : public MWidgetStyleContainer
{
  M_STYLE_CONTAINER(NetworkItemStyle);
};


#endif //NETWORK_ITEM_STYLE_H
