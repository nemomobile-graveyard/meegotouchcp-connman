/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "wifibrief.h"
#include <DcpWidgetTypes>

WifiBrief::WifiBrief() : DcpBrief()
{
}

WifiBrief::~WifiBrief()
{
}

QString WifiBrief::titleText() const
{
  return "Wifi";
}

int WifiBrief::widgetTypeID() const
{
  return DcpWidgetType::Label;
}
