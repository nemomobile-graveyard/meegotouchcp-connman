/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef ADDNETWORKPAGE_H
#define ADDNETWORKPAGE_H

#include "networklistmodel.h"
#include <MApplicationPage>
#include <MLabel>
#include <MTextEdit>
#include <MButtonGroup>
#include <MWidgetController>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>

class AddNetworkPage : public MApplicationPage
{
  Q_OBJECT;

public:
  AddNetworkPage(NetworkListModel *networkListModel = 0, QGraphicsWidget *parent = 0);
  virtual ~AddNetworkPage();
  void createContent();

public slots:
  void setNetworkListModel(NetworkListModel *networkListModel);

private slots:
  void onBackButtonClicked();
  void applyTriggered();
  void buttonClicked(int);

private:
  class TypeButton
  {
  public:
    TypeButton(const QString &ConnmanName, const QString &FriendlyName);
    ~TypeButton();

    QString connmanName;
    QString friendlyName;
    MButton *button;

    // operator==(const TypeButton &other);
    // operator==(const QString &connmanName);
  };

  MWidgetController *m_header;
  MLayout *m_layout;
  MGridLayoutPolicy *m_gridPolicy;
  MLinearLayoutPolicy *m_linearPolicy;
  QList<TypeButton*> m_buttons;
  MTextEdit *m_nameEdit;
  MButtonGroup *m_buttonGroup;
  NetworkListModel *m_networkListModel;
  MTextEdit *m_passwordEdit;
  MLabel *m_passwordLabel;
};

#endif //ADDNETWORKPAGE_H
