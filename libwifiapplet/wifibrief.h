/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef WIFIBRIEF_H
#define WIFIBRIEF_H

#include <DcpBrief>

class WifiBrief : public DcpBrief
{
  Q_OBJECT;
 public:
  WifiBrief();
  virtual ~WifiBrief();
  QString titleText() const;
  int widgetTypeID() const;
};


#endif //WIFIBRIEF_H
