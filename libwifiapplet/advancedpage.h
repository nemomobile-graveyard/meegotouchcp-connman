/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef ADVANCEDPAGE_H
#define ADVANCEDPAGE_H

#include "networkitemmodel.h"

#include <MApplicationPage>
#include <MTextEdit>
#include <MComboBox>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MLabel>


class AdvancedPage : public MApplicationPage
{
  Q_OBJECT;

public:
  AdvancedPage(QGraphicsWidget *parent = 0);
  virtual ~AdvancedPage();
  void createContent();

public slots:
  QStringList nameservers() const;
  void setNameservers(const QStringList &nameservers);
  void setIPV4(const NetworkItemModel::IPv4Type &ipv4);
  void setMAC(const QString &mac);
  void setNetworkItemModel(NetworkItemModel *networkItemModel);


private slots:
  void onBackButtonClicked();
  void comboIndexChanged(int index);
  void applyTriggered();
  void modified(const QList<const char *>& members); /* experiment */

private:
  struct MethodName {
    QString connmanName;
    QString friendlyName;
  };

  void showRouter(const bool show);
  void setIPEditable(const bool editable);

  MComboBox *m_combo;

  MLabel *m_ipLabel;
  MTextEdit *m_ipEdit;

  MLabel *m_subnetLabel;
  MTextEdit *m_subnetEdit;

  MLabel *m_routerLabel;
  MTextEdit *m_routerEdit;


  MTextEdit *m_dnsEdit;
  MLayout *m_layout;
  MLabel *m_MAC;
  MGridLayoutPolicy *m_gridPolicy;
  MLinearLayoutPolicy *m_linearPolicy;
  QVector<MethodName> m_methodNames;
  NetworkItemModel *m_NIModel;


  static const QString Method;
  static const QString Address;
  static const QString Netmask;
  static const QString Gateway;
};


#endif // ADVANCEDPAGE_H
