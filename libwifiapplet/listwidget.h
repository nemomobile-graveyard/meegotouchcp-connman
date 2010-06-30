/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include "technologybutton.h"
#include <DcpWidget>
#include <MLabel>
#include <MList>
#include <QStringList>
#include <MDialog>
#include <QSortFilterProxyModel>
#include <MLinearLayoutPolicy>
#include <QPointer>
#include <QGraphicsLinearLayout>
#include <MAction>

class ListWidget : public DcpWidget
{
  Q_OBJECT;

public:
  ListWidget(QGraphicsWidget *parent = 0);
  virtual ~ListWidget();
  bool pagePans() const; //FIXME: is this Pure Virtual?
  bool back();

protected:
  void timerEvent(QTimerEvent *event);

private:
  void createContent(); //carryover from MApplicationPage
  void removeLayoutItem(QGraphicsWidget *item, const bool remove);
  void addLayoutItem(QGraphicsWidget *item, const bool add);
  void singleShot();
  MLinearLayoutPolicy *mainLayout;
  QGraphicsLinearLayout *m_layout;

  MLabel *m_wifiLabel;
  TechnologyButton *m_wifiSwitch;
  MList *m_list;
  NetworkListModel *m_networkListModel;
  QSortFilterProxyModel *m_filter;
  MWidgetController *m_buttonRow;
  int m_timerId;
  
  const QString m_noNetworksString;
  const QString m_turnOnWifiString;
  const QString m_wifiNotAvailableString;

  QPointer<MLabel> m_noNetworksLabel;
  QPointer<MLabel> m_turnOnWifiLabel;
  QPointer<MLabel> m_wifiNotAvailableLabel;
  bool m_wifiEnabled;
  bool m_wifiAvailable;
  bool m_noWifiInRange;

  //used to connect to hidden networks
  QStringList m_securityTypes;
  MDialog *m_addNetworkDialog;
  QHash<QString, QString> m_securityTypesMap;

  int m_secondsBetweenScans;
  MAction* m_addNetworkAction;

private slots:
  void startScanning(const int seconds = 60); //FIXME: swag
  void stopScanning(void);
  void addNetworkClicked(void);
  void onVisibleChanged();

  void showListOrLabel();
  void onRowsInserted();
  void onRowsRemoved();
  void onWifiToggled(bool checked);
  void onTechnologiesChanged(const QStringList &availableTechnologies,
			     const QStringList &enabledTechnologies,
			     const QStringList &connectedTechnologies);
};

#endif //LISTWIDGET_H
