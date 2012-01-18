/*   -*- Mode: C++ -*-
 * meegotouchcp-connman - connectivity plugin for duicontrolpanel
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include "technologybutton.h"

TechnologyButton::TechnologyButton(const QString &technology,
				   NetworkListModel *model,
				   QGraphicsItem *parent) :
  MButton(parent),
  m_technology(technology),
  m_model(model),
  m_enableOrDisableOngoing(false)
{
  setViewType(MButton::switchType);

  if (m_model->availableTechnologies().contains(m_technology)) {
    setCheckable(true);
  } else {
    setCheckable(false);
  }
  if (m_model->enabledTechnologies().contains(m_technology)) {
    setChecked(true);
  } else {
    setChecked(false);
  }
  switchButtonObserving(true);
  connect(model, SIGNAL(technologiesChanged(const QStringList&,
					    const QStringList&,
					    const QStringList&,
					    const QString&)),
	  this, SLOT(technologiesChanged(const QStringList&,
					 const QStringList&,
					 const QStringList&,
					 const QString&)));
}

TechnologyButton::~TechnologyButton()
{
}

void TechnologyButton::technologiesChanged(const QStringList &availableTechnologies,
					   const QStringList &enabledTechnologies,
					   const QStringList &connectedTechnologies,
					   const QString &whatChanged)
{
  Q_UNUSED(connectedTechnologies);
  const bool available = availableTechnologies.contains(m_technology);
  const bool enabled = enabledTechnologies.contains(m_technology);

  setCheckable(available);

  // Only change WiFi on/off button state if the Enabled status changes
  if (whatChanged == EnabledTechnologies || whatChanged == AllTechnologies) {

    // Switch button observing is disabled for the duration of setChecked 
    // in order to prevent toggledSlot (and the enableTechnology, disableTechnology 
    // calls therein) based on signals received from ConnMan. With this, toggledSlot 
    // is now only called when the _user_ touches the button.
    switchButtonObserving(false);
    setChecked(enabled);
    switchButtonObserving(true);

    if (m_enableOrDisableOngoing) {
      // To make more sure that the user does not toggle
      // the button again when things are still "settling down"
      // on ConnMan side, show the button only after "a while"
      // (no, this is not beautiful, but toggling the button at 
      // this point for a couple of times seemed like a certain 
      // way to crash ConnMan). 
      // Use a bit different times depending on whether WiFi
      // was turned ON or OFF for a bit better user experience.
      if (enabled) {
	QTimer::singleShot(10000, this, SLOT(delayedShowButton()));
      }
      else {
	QTimer::singleShot(5000, this, SLOT(delayedShowButton()));
      }
    }
  }
}

void TechnologyButton::delayedShowButton() 
{
  m_model->emitStatusChangeTrigger(false);
  setVisible(true);
  m_enableOrDisableOngoing = false;
}

void TechnologyButton::toggledSlot(bool checked)
{
  m_model->emitStatusChangeTrigger(true);
  if (checked) {
    m_model->enableTechnology(m_technology);
  } else {
    m_model->disableTechnology(m_technology);
  }
  // Hide the button until the requested change has happened on
  // ConnMan side (we notice that in technologiesChanged above) in 
  // order to prevent us from sending a new enable/disable request 
  // to ConnMan while the earlier request is still being processed
  // (which seemed to cause some stability issues on ConnMan side).
  m_enableOrDisableOngoing = true;
  setVisible(false);
}

void TechnologyButton::switchButtonObserving(bool on)
{
  if (on) {
    connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
  }
  else {
    disconnect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
  }
}
