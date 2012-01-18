/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef NETWORK_ITEM_H
#define NETWORK_ITEM_H

#include "networkitemmodel.h"

#include <MWidgetController>
#include <MAction>
#include <MDialog>
#include <QGraphicsSceneEvent>

class NetworkItem : public MWidgetController
{
  Q_OBJECT;
  M_CONTROLLER(NetworkItem);


  Q_PROPERTY(QString name READ name);
  Q_PROPERTY(QString security READ security);
  Q_PROPERTY(int strength READ strength);
  Q_PROPERTY(NetworkItemModel::StateType state READ state);

public:
  NetworkItem(MWidget *parent = 0);
  virtual ~NetworkItem();

  static const MTheme::ViewType defaultType;

  QString name() const;
  QString security() const;
  int strength() const;
  NetworkItemModel::StateType state() const;

signals:
  void clicked();

public Q_SLOTS:
  void click();

protected Q_SLOTS:
  virtual void updateData(const QList< const char * > &modifications);
  virtual void setupModel();

private Q_SLOTS:
  void removeTriggered();
  void connectTriggered();
  void clearPassphraseTriggered();
  void advancedTriggered();

private:
  void initWidget();
  QVariant itemChange(GraphicsItemChange change, const QVariant &val);
  void setActionsByState(void);
  void connectActionSignals(void);
  MAction *m_disconnectAction;
  MAction *m_removeAction;
  MAction *m_clearPassphraseAction;
  MAction *m_advancedAction;
  static MDialog *dialog;

  //temporary
  static int instances;
  static int idCounter;
  int id;

  Q_DISABLE_COPY(NetworkItem);
};

#include <MAbstractCellCreator>

class NetworkItemCellCreator : public MAbstractCellCreator<NetworkItem>
{
 public:
  void updateCell(const QModelIndex& index, MWidget * cell) const;
};

#include <MSheet>

class MTextEdit;
class MMessageBox;

class NetworkItemModel;
class NetworkListModel;

class LoginSheet : public MSheet
{
  Q_OBJECT

public:
  LoginSheet(QString const &title, 
	     NetworkItemModel* itemModel, 
	     NetworkListModel* listModel);

private Q_SLOTS:
  void ok();
  void cancel();
  void setfocusOnPasswordTextEdit();

private:
  void createCentralWidget(QString const &title);
  void createHeaderWidget();
  QGraphicsWidget *createSpacer() const;
  MTextEdit *m_passwordTextEdit;
  NetworkItemModel* m_itemModel;
  NetworkListModel* m_listModel;
};

#endif //NETWORK_ITEM_H
