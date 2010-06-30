/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef NETWORK_ITEM_VIEW_H
#define NETWORK_ITEM_VIEW_H

#include "networkitemmodel.h"
#include "networkitemstyle.h"
#include "networkitem.h"

#include <MWidgetView>
#include <MImageWidget>
#include <MLabel>
#include <QGraphicsLinearLayout>
#include <MCancelEvent>
#include <QTime>

class NetworkItemView : public MWidgetView
{
  Q_OBJECT;
  M_VIEW(NetworkItemModel, NetworkItemStyle);

public:
  NetworkItemView(NetworkItem *controller);
  virtual ~NetworkItemView();

protected slots:
  virtual void modelModified(const QList<const char *> &modifications);
  virtual void updateData(const QList<const char *> &modifications);

protected:
  virtual void setupModel();
  virtual void styleChanged();
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void cancelEvent(MCancelEvent *event);

private slots:
  void setDefaultMode();

private:
  void setLabel(const QString &name,
		const QString &security,
		const NetworkItemModel::StateType &state);
  void setStrengthIcon(const int &strength);

  void clearLayout();

  NetworkItem *m_controller;
  MLabel *m_label;
  MImageWidget *m_strengthIcon;
  QGraphicsLinearLayout *m_layout;
  QTime m_pressedTime;

  void initWidget();
  const QString& prettyPrintState(const NetworkItemModel::StateType &state) const;
  const QString& prettyPrintSecurity(const QString &security) const;
  void setDefaultModeDelayed();

  Q_DISABLE_COPY(NetworkItemView);
};

#endif //NETWORK_ITEM_VIEW_H
