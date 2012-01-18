/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "listwidget.h"
#include "networkitem.h"
#include "debug.h"
#include "addnetworkpage.h"
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MTextEdit>
#include <MComboBox>
#include <QSortFilterProxyModel>
#include <MApplicationPage>
#include <QTimer>
#include <mwidgetcreator.h>

M_REGISTER_WIDGET_NO_CREATE(ListWidget);

ListWidget::ListWidget(QGraphicsWidget *parent) :
  DcpStylableWidget(parent),
  m_wifiLabel(NULL),
  m_wifiSwitch(NULL), m_list(NULL),
  m_filter(NULL), m_buttonRow(NULL),
  m_timerId(0),
  //% "There are no networks currently available"
  m_noNetworksString(qtTrId("qtn_no_networks")),
  //% "To see wireless networks, turn on Wifi"
  m_turnOnWifiString(qtTrId("qtn_turn_on_wifi")),
  //% "Wifi is not available on this device"
  m_wifiNotAvailableString(qtTrId("qtn_wifi_not_available")),
  m_wifiEnabled(false),
  m_wifiAvailable(false), m_noWifiInRange(false),
  m_addNetworkDialog(NULL),
  m_secondsBetweenScans(DEFAULT_SCAN_INTERVAL),
  m_addNetworkAction(NULL)
{
  MDEBUG("%s", __FUNCTION__);
  m_securityTypes << "Open" << "WEP" << "WPA";
  setObjectName("listPage");
  m_securityTypesMap.insert("Open", "none");
  m_securityTypesMap.insert("WEP", "wep");
  m_securityTypesMap.insert("WPA", "wpa");

  createContent();
}

ListWidget::~ListWidget()
{
  MDEBUG("destructor");
}

bool ListWidget::pagePans() const
{
  return true;
}

bool ListWidget::back()
{
  stopScanning();
  return DcpStylableWidget::back(); //for now
}

NetworkListModel* ListWidget::networkListModel() 
{
  return m_networkListModel;
}

void ListWidget::createContent()
{
  m_networkListModel = NetworkListModel::instance();
  m_filter = new QSortFilterProxyModel(this);
  m_filter->setSourceModel(m_networkListModel);
  m_filter->setDynamicSortFilter(true);
  m_filter->setFilterKeyColumn(1);
  m_filter->setFilterFixedString("wifi");

  m_layout = new QGraphicsLinearLayout(Qt::Vertical);
  setLayout(m_layout);
  m_layout->setSpacing(0);
  m_layout->setContentsMargins(0, 0, 0, 0);

  m_buttonRow = new MWidgetController();
  m_buttonRow->setViewType("background");
  m_buttonRow->setObjectName("buttonRow");
  {
    MLayout *rowLayout = new MLayout(m_buttonRow);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *buttonPolicy = new MLinearLayoutPolicy(rowLayout, Qt::Horizontal);
    buttonPolicy->setObjectName("buttonRowPolicy");

    //% "WIFI STATUS"
    m_wifiLabel = new MLabel(qtTrId("qtn_wifi_label"), m_buttonRow);
    m_wifiSwitch = new TechnologyButton("wifi", m_networkListModel);
    buttonPolicy->addItem(m_wifiLabel, Qt::AlignLeft);
    buttonPolicy->addStretch();
    buttonPolicy->addItem(m_wifiSwitch, Qt::AlignRight);
  }
  m_layout->addItem(m_buttonRow);

  m_noNetworksLabel= new MLabel(m_noNetworksString);
  m_noNetworksLabel->setStyleName("CommonBodyTextInverted");
  m_turnOnWifiLabel = new MLabel(m_turnOnWifiString);
  m_turnOnWifiLabel->setStyleName("CommonBodyTextInverted");
  m_wifiNotAvailableLabel = new MLabel(m_wifiNotAvailableString);
  m_wifiNotAvailableLabel->setStyleName("CommonBodyTextInverted");

  m_list = new MList();
  m_list->setObjectName("list");
  m_list->setViewType("fast");
  m_list->setItemModel(m_filter);
  m_list->setCellCreator(new NetworkItemCellCreator());

  m_wifiAvailable = m_networkListModel->availableTechnologies().contains("wifi");
  m_wifiEnabled = m_networkListModel->enabledTechnologies().contains("wifi");

  m_layout->addItem(m_list);
  showListOrLabel();

  startScanning();

  connect(m_filter, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
	  this, SLOT(onRowsRemoved()));

  connect(m_filter, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
	  this, SLOT(onRowsInserted()));

  connect(m_networkListModel, SIGNAL(technologiesChanged(const QStringList &,const QStringList &, const QStringList &, const QString &)),
	  this, SLOT(onTechnologiesChanged(const QStringList &,const QStringList &, const QStringList &, const QString &)));

  connect(m_networkListModel, SIGNAL(statusChangeTrigger(bool)), this, SLOT(onStatusChangeTrigger(bool)));

  connect(m_wifiSwitch, SIGNAL(toggled(bool)),
	  this, SLOT(onWifiToggled(bool)));

  connect(this, SIGNAL(visibleChanged()),
	  this, SLOT(onVisibleChanged()));

  singleShot();
}

void ListWidget::onStatusChangeTrigger(bool on)
{
  if (on) {
    //% "WIFI STATUS - processing request..."
    m_wifiLabel->setText(qtTrId("qtn_wifi_label_processing"));
  }
  else {
    m_wifiLabel->setText(qtTrId("qtn_wifi_label"));
  }
}

void ListWidget::timerEvent(QTimerEvent *event)
{
  Q_UNUSED(event);
  MDEBUG("calling requestScan from timer: %d", event->timerId());
  m_networkListModel->requestScan();
}

void ListWidget::startScanning(const int seconds)
{
  Q_UNUSED(seconds);
  // If periodic scanning is NOT already ongoing, we start it,
  // otherwise we just let it continue as is
  if (!m_timerId) {
    m_timerId = startTimer(m_secondsBetweenScans*1000);
  }
}

void ListWidget::stopScanning(void)
{
  MDEBUG("stopped scanning");
  if (m_timerId) {
    killTimer(m_timerId);
    m_timerId = 0;
  }
}

void ListWidget::addNetworkClicked(void)
{
  AddNetworkPage *addNetworkPage = new AddNetworkPage(m_networkListModel);
  addNetworkPage->appear(MSceneWindow::DestroyWhenDone);
}

void ListWidget::onRowsRemoved()
{
  Q_ASSERT(m_filter);

  if (!m_filter->rowCount()) {
    m_noWifiInRange = true;
    showListOrLabel();
  }
}

void ListWidget::onRowsInserted()
{
  Q_ASSERT(m_list);
  //why are these failing??
  //Q_ASSERT(m_wifiEnabled);
  //Q_ASSERT(m_wifiAvailable);

  m_noWifiInRange = false;
  showListOrLabel();
}

// This is called both when the user toggles
// the on/off button and when it is toggled based
// on a technologiesChanged signal from ConnMan
void ListWidget::onWifiToggled(bool checked)
{
  showListOrLabel();
}

void ListWidget::showListOrLabel()
{
  Q_ASSERT(m_list);
  Q_ASSERT(m_wifiNotAvailableLabel);
  Q_ASSERT(m_turnOnWifiLabel);
  Q_ASSERT(m_noNetworksLabel);

  // First clear the whole display

  removeLayoutItem(m_wifiNotAvailableLabel, true);
  removeLayoutItem(m_turnOnWifiLabel, true);
  removeLayoutItem(m_noNetworksLabel, true);
  removeLayoutItem(m_list, true);

  // Then add the right layout item depending on the situation

  if (!m_wifiAvailable) {
    addLayoutItem(m_wifiNotAvailableLabel, true);
  }
  else if (!m_wifiEnabled) {
    addLayoutItem(m_turnOnWifiLabel, true);
  }
  else if (m_noWifiInRange) {
    addLayoutItem(m_noNetworksLabel, true);
  }
  else {
    addLayoutItem(m_list, true);
  }
}

void ListWidget::removeLayoutItem(QGraphicsWidget* thing, const bool remove)
{
  Q_ASSERT(thing);
  Q_ASSERT(m_layout);

  if (remove) {
    thing->setVisible(false);
    m_layout->removeItem(thing);
  }
}

void ListWidget::addLayoutItem(QGraphicsWidget* thing, const bool add)
{
  Q_ASSERT(thing);
  Q_ASSERT(m_layout);

  if (add) {
    thing->setVisible(true);
    m_layout->addItem(thing);
  }
}

void ListWidget::onTechnologiesChanged(const QStringList &availableTechnologies,
				       const QStringList &enabledTechnologies,
				       const QStringList &connectedTechnologies,
				       const QString &whatChanged)
{
  Q_UNUSED(availableTechnologies);
  Q_UNUSED(connectedTechnologies);
  Q_UNUSED(whatChanged);

  Q_ASSERT(m_networkListModel);

  m_wifiEnabled = enabledTechnologies.contains("wifi");
  m_wifiAvailable = availableTechnologies.contains("wifi");

  // Keep scanning on only when WiFi is enabled (a part of an effort to avoid
  // problems/crashes on the ConnMan side)
  if (whatChanged == EnabledTechnologies || whatChanged == AllTechnologies) {
    if (m_wifiEnabled) {
      startScanning();
    }
    else {
      stopScanning();
    }
  }
  showListOrLabel();
}

void ListWidget::onVisibleChanged()
{
  MApplicationPage *page = NULL;

  QGraphicsItem *item = this->parentItem();
  while (item)
    {
      page = dynamic_cast<MApplicationPage *>(item);
      if (page)
	{
	  //page->addAction(action);
	  break;
	}
      item = item->parentItem();
    }
  if (page && !m_addNetworkAction) {
    //% "Add New Network"
    m_addNetworkAction = new MAction(qtTrId("qtn_add_network_action"), this);
    m_addNetworkAction->setLocation(MAction::ToolBarLocation);
    page->addAction(m_addNetworkAction);

    connect(m_addNetworkAction, SIGNAL(triggered()),
	    this, SLOT(addNetworkClicked(void)));
  }
}

void ListWidget::singleShot()
{
  QTimer *timer = new QTimer(this);
  timer->setSingleShot(true);
  connect(timer, SIGNAL(timeout()),
	  this, SLOT(onVisibleChanged()));
  timer->start(0);
}
