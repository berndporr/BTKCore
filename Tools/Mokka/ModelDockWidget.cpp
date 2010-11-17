/* 
 * The Biomechanical ToolKit
 * Copyright (c) 2009-2010, Arnaud Barré
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *     * Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *     * Neither the name(s) of the copyright holders nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "ModelDockWidget.h"
#include "UserRoles.h"

#include <QPainter>
#include <QFileDialog>
#include <QMouseEvent>
#include <QColorDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QXmlStreamReader>

ModelDockWidget::ModelDockWidget(QWidget* parent)
: QDockWidget(parent), m_ConfigurationItems(), m_RecentColors(5)
{
  this->m_CurrentConfigurationIndex = -1;
  this->mp_Acquisition = 0;
  this->mp_DownArrowIcon = new QIcon(QString::fromUtf8(":/Resources/Images/disclosureTriangleSmallDownBlack.png"));
  this->mp_RightArrowIcon = new QIcon(QString::fromUtf8(":/Resources/Images/disclosureTriangleSmallRightBlack.png"));
  this->mp_MarkersIcon  = new QIcon(QString::fromUtf8(":/Resources/Images/markers.png"));
  this->mp_AnalogsIcon = new QIcon(QString::fromUtf8(":/Resources/Images/chart_line.png"));
  this->mp_ModelOutputsIcon = new QIcon(QString::fromUtf8(":/Resources/Images/chart_curve.png"));
  
  this->setupUi(this);
   
   // Finalize the UI
   QFont f = this->modelTree->font();
#ifdef Q_OS_MAC
  // TODO: What about the widget's attributes Qt::WA_MacNormalSize, Qt::WA_MacSmallSize	 and Qt::WA_MacMiniSize for mac?
  
  this->setStyleSheet("QDockWidget {color: white;} QDockWidget::title {background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(128, 128, 128, 255), stop:0.5 rgba(0, 0, 0, 255), stop:0.95 rgba(64, 64, 64, 255), stop:1 rgba(200, 200, 200, 255)); text-align: center; padding-left: -25px;}");
  
  f.setPointSize(10);
  this->modelTree->setFont(f);
  this->propertiesButton->setFont(f);
  //  - Marker page
  this->markerLabelLabel->setFont(f);
  this->markerLabelEdit->setFont(f);
  this->markerRadiusLabel->setFont(f);
  // FIX: Setting the font for the spinbox has no effect with Mac
  // this->markerRadiusSpinBox->setFont(f);
  this->markerRadiusSpinBox->setStyleSheet("QDoubleSpinBox {font-size: 10px;};");
  this->markerColorLabel->setFont(f);
  this->markerDescLabel->setFont(f);
  this->markerDescEdit->setFont(f);
  // - Point page
  this->pointLabelLabel->setFont(f);
  this->pointLabelEdit->setFont(f);
  this->pointDescLabel->setFont(f);
  this->pointDescEdit->setFont(f);
  // - Analog page
  this->analogLabelLabel->setFont(f);
  this->analogLabelEdit->setFont(f);
  this->analogUnitLabel->setFont(f);
  this->analogUnitEdit->setFont(f);
  this->analogGainLabel->setFont(f);
  this->analogGainComboBox->setFont(f);
  this->analogScaleLabel->setFont(f);
  // this->analogScaleSpinBox->setFont(f);
  this->analogScaleSpinBox->setStyleSheet("QDoubleSpinBox {font-size: 10px;};");
  this->analogOffsetLabel->setFont(f);
  //this->analogOffsetSpinBox->setFont(f);
  this->analogOffsetSpinBox->setStyleSheet("QSpinBox {font-size: 10px;};");
  this->analogDescLabel->setFont(f);
  this->analogDescEdit->setFont(f);
#endif
  // Model configuration menu
  QMenu* modelConfigurationMenu = new QMenu(this);
  this->mp_NewConfiguration = new QAction(tr("New configuration"), this);
  this->mp_LoadConfiguration = new QAction(tr("Load configuration"), this);
  this->mp_SaveConfiguration = new QAction(tr("Save configuration"), this);
  this->mp_SaveConfiguration->setEnabled(false);
  this->mp_RemoveConfiguration = new QAction(tr("Remove configuration"), this);
  this->mp_RemoveConfiguration->setEnabled(false);
  this->mp_DeselectConfiguration = new QAction(tr("Deselect configuration"), this);
  this->mp_DeselectConfiguration->setEnabled(false);
  this->mp_ClearConfigurations = new QAction(tr("Clear configurations"), this);
  this->mp_SelectAllMarkers = new QAction(tr("Select all markers"), this);
  this->mp_HideSelectedMarkers = new QAction(tr("Hide selected markers"), this);
  this->mp_HideSelectedMarkers->setEnabled(false);
  this->mp_UnhideSelectedMarkers = new QAction(tr("Unhide selected markers"), this);
  this->mp_UnhideSelectedMarkers->setEnabled(false);
  this->mp_SelectAllAnalogs = new QAction(tr("Select all analog channels"), this);
  this->mp_SelectAllModelOutputs = new QAction(tr("Select all model outputs"), this);
  this->mp_SelectAllAngles = new QAction(tr("Select all angles"), this);
  this->mp_SelectAllForces = new QAction(tr("Select all forces"), this);
  this->mp_SelectAllMoments = new QAction(tr("Select all moments"), this);
  this->mp_SelectAllPowers = new QAction(tr("Select all powers"), this);
  this->mp_SelectAllScalars = new QAction(tr("Select all scalars"), this);
  this->mp_RemoveSelectedItems = new QAction(tr("Remove selected items"), this);
  this->mp_RemoveSelectedItems->setEnabled(false);
  modelConfigurationMenu->addAction(this->mp_NewConfiguration);
  modelConfigurationMenu->addAction(this->mp_LoadConfiguration);
  modelConfigurationMenu->addAction(this->mp_SaveConfiguration);
  modelConfigurationMenu->addAction(this->mp_RemoveConfiguration);
  modelConfigurationMenu->addAction(this->mp_DeselectConfiguration);
  modelConfigurationMenu->addSeparator();
  modelConfigurationMenu->addAction(this->mp_ClearConfigurations);
  modelConfigurationMenu->addSeparator();
  modelConfigurationMenu->addAction(this->mp_SelectAllMarkers);
  modelConfigurationMenu->addAction(this->mp_HideSelectedMarkers);
  modelConfigurationMenu->addAction(this->mp_UnhideSelectedMarkers);
  modelConfigurationMenu->addSeparator();
  modelConfigurationMenu->addAction(this->mp_SelectAllAnalogs);
  modelConfigurationMenu->addAction(this->mp_SelectAllModelOutputs);
  modelConfigurationMenu->addAction(this->mp_SelectAllAngles);
  modelConfigurationMenu->addAction(this->mp_SelectAllForces);
  modelConfigurationMenu->addAction(this->mp_SelectAllMoments);
  modelConfigurationMenu->addAction(this->mp_SelectAllPowers);
  modelConfigurationMenu->addAction(this->mp_SelectAllScalars);
  modelConfigurationMenu->addSeparator();
  modelConfigurationMenu->addAction(this->mp_RemoveSelectedItems);
  this->modelConfigurationButton->setMenu(modelConfigurationMenu);
  // Marker color menu
  QMenu* markerColorMenu = new QMenu(this);
  this->mp_ResetMarkerColorHistory = new QAction(tr("Reset recent colors"), this);
  this->mp_SetMarkerColorInHistory1 = new QAction(tr("Set recent color #1"), this);
  this->mp_SetMarkerColorInHistory2 = new QAction(tr("Set recent color #2"), this);
  this->mp_SetMarkerColorInHistory3 = new QAction(tr("Set recent color #3"), this);
  this->mp_SetMarkerColorInHistory4 = new QAction(tr("Set recent color #4"), this);
  this->mp_SetMarkerColorInHistory5 = new QAction(tr("Set recent color #5"), this);
  markerColorMenu->addAction(this->mp_ResetMarkerColorHistory);
  markerColorMenu->addSeparator();
  markerColorMenu->addAction(this->mp_SetMarkerColorInHistory1);
  markerColorMenu->addAction(this->mp_SetMarkerColorInHistory2);
  markerColorMenu->addAction(this->mp_SetMarkerColorInHistory3);
  markerColorMenu->addAction(this->mp_SetMarkerColorInHistory4);
  markerColorMenu->addAction(this->mp_SetMarkerColorInHistory5);
  this->markerColorButtonMenu->setMenu(markerColorMenu);
  // Tree
  QList<QTreeWidgetItem *> items;
  f = this->modelTree->font();
  f.setBold(true);
  // Header size
  QHeaderView* modelTreeHeader = this->modelTree->header();
  modelTreeHeader->setDefaultSectionSize(26);
  modelTreeHeader->setResizeMode(LabelHeader, QHeaderView::Stretch);
  modelTreeHeader->setResizeMode(VisibleHeader, QHeaderView::Fixed);
  modelTreeHeader->setResizeMode(TrajectoryHeader, QHeaderView::Fixed);
  // Markers
  QTreeWidgetItem* markersRoot = new QTreeWidgetItem(QStringList(QString("Markers")));
  markersRoot->setFont(LabelHeader, f);
  markersRoot->setIcon(LabelHeader, *this->mp_MarkersIcon);
  markersRoot->setFlags(markersRoot->flags() & ~Qt::ItemIsSelectable);
  items.append(markersRoot);
  // Analog channels
  QTreeWidgetItem* analogsRoot = new QTreeWidgetItem(QStringList(QString("Analog channels")));
  analogsRoot->setFont(LabelHeader, f);
  analogsRoot->setIcon(LabelHeader, *this->mp_AnalogsIcon);
  analogsRoot->setFlags(analogsRoot->flags() & ~Qt::ItemIsSelectable);
  items.append(analogsRoot);
  // Model outputs
  QTreeWidgetItem* modelOutputsRoot = new QTreeWidgetItem(QStringList(QString("Model outputs")));
  modelOutputsRoot->setFont(LabelHeader, f);
  modelOutputsRoot->setIcon(LabelHeader, *this->mp_ModelOutputsIcon);
  modelOutputsRoot->setFlags(modelOutputsRoot->flags() & ~Qt::ItemIsSelectable);
  items.append(modelOutputsRoot);
  // Insert top level items
  this->modelTree->addTopLevelItems(items);
  markersRoot->setHidden(true);
  analogsRoot->setHidden(true);
  modelOutputsRoot->setHidden(true);
  items.clear();
  // Model outputs: Angles
  QTreeWidgetItem* modelOutputAngles = new QTreeWidgetItem(QStringList(QString("Angles")));
  modelOutputAngles->setFont(LabelHeader, f);
  modelOutputAngles->setIcon(LabelHeader, *this->mp_ModelOutputsIcon);
  modelOutputAngles->setFlags(modelOutputAngles->flags() & ~Qt::ItemIsSelectable);
  items.append(modelOutputAngles);
  // Model outputs: Forces
  QTreeWidgetItem* modelOutputForces = new QTreeWidgetItem(QStringList(QString("Forces")));
  modelOutputForces->setFont(LabelHeader, f);
  modelOutputForces->setIcon(LabelHeader, *this->mp_ModelOutputsIcon);
  modelOutputForces->setFlags(modelOutputForces->flags() & ~Qt::ItemIsSelectable);
  items.append(modelOutputForces);
  // Model outputs: Moments
  QTreeWidgetItem* modelOutputMoments = new QTreeWidgetItem(QStringList(QString("Moments")));
  modelOutputMoments->setFont(LabelHeader, f);
  modelOutputMoments->setIcon(LabelHeader, *this->mp_ModelOutputsIcon);
  modelOutputMoments->setFlags(modelOutputMoments->flags() & ~Qt::ItemIsSelectable);
  items.append(modelOutputMoments);
  // Model outputs: Powers
  QTreeWidgetItem* modelOutputPowers = new QTreeWidgetItem(QStringList(QString("Powers")));
  modelOutputPowers->setFont(LabelHeader, f);
  modelOutputPowers->setIcon(LabelHeader, *this->mp_ModelOutputsIcon);
  modelOutputPowers->setFlags(modelOutputPowers->flags() & ~Qt::ItemIsSelectable);
  items.append(modelOutputPowers);
  // Model outputs: Scalars
  QTreeWidgetItem* modelOutputScalars = new QTreeWidgetItem(QStringList(QString("Scalars")));
  modelOutputScalars->setFont(LabelHeader, f);
  modelOutputScalars->setIcon(LabelHeader, *this->mp_ModelOutputsIcon);
  modelOutputScalars->setFlags(modelOutputScalars->flags() & ~Qt::ItemIsSelectable);
  items.append(modelOutputScalars);
  // Insert model output items
  modelOutputsRoot->addChildren(items);
  // Strech
  this->splitter->setStretchFactor(0, 10);
  // Resize the splitter's items
  /*
  this->toggleProperties();
  QList<int> sizes = this->splitter->sizes();
  sizes[0] = sizes[0] + sizes[1] - 16;
  sizes[1] = 16;
  this->splitter->setSizes(sizes);
  this->toggleProperties();
  */
  // Force to the empty property page
  this->propertiesStack->setCurrentIndex(0);
  
  // Connections
  connect(this->modelConfigurationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectConfiguration(int)));
  connect(this->mp_NewConfiguration, SIGNAL(triggered()), this, SLOT(newConfiguration()));
  connect(this->mp_LoadConfiguration, SIGNAL(triggered()), this, SLOT(loadConfiguration()));
  connect(this->mp_SaveConfiguration, SIGNAL(triggered()), this, SLOT(saveConfiguration()));
  connect(this->mp_RemoveConfiguration, SIGNAL(triggered()), this, SLOT(removeConfiguration()));
  connect(this->mp_DeselectConfiguration, SIGNAL(triggered()), this, SLOT(deselectConfiguration()));
  connect(this->mp_ClearConfigurations, SIGNAL(triggered()), this, SLOT(clearConfigurations()));
  connect(this->mp_SelectAllMarkers, SIGNAL(triggered()), this, SLOT(selectAllMarkers()));
  connect(this->mp_HideSelectedMarkers, SIGNAL(triggered()), this, SLOT(hideSelectedMarkers()));
  connect(this->mp_UnhideSelectedMarkers, SIGNAL(triggered()), this, SLOT(unhideSelectedMarkers()));
  connect(this->mp_SelectAllAnalogs, SIGNAL(triggered()), this, SLOT(selectAllAnalogs()));
  connect(this->mp_SelectAllModelOutputs, SIGNAL(triggered()), this, SLOT(selectAllModelOutputs()));
  connect(this->mp_SelectAllAngles, SIGNAL(triggered()), this, SLOT(selectAllAngles()));
  connect(this->mp_SelectAllForces, SIGNAL(triggered()), this, SLOT(selectAllForces()));
  connect(this->mp_SelectAllMoments, SIGNAL(triggered()), this, SLOT(selectAllMoments()));
  connect(this->mp_SelectAllPowers, SIGNAL(triggered()), this, SLOT(selectAllPowers()));
  connect(this->mp_SelectAllScalars, SIGNAL(triggered()), this, SLOT(selectAllScalars()));
  connect(this->mp_RemoveSelectedItems, SIGNAL(triggered()), this, SLOT(removeSelectedItems()));  
  connect(this->propertiesButton, SIGNAL(clicked()), this, SLOT(toggleProperties()));
  connect(this->modelTree, SIGNAL(itemSelectionChanged()), this, SLOT(displayProperties()));
  connect(this->modelTree, SIGNAL(itemSelectionChanged()), this, SLOT(sendSelectedMarkers()));
  connect(this->modelTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(sendModifiedMarkersState(QTreeWidgetItem*, int)));
  connect(this->markerLabelEdit, SIGNAL(editingFinished()), this, SLOT(editMarkerLabel()));
  connect(this->markerDescEdit, SIGNAL(editingFinished()), this, SLOT(editMarkersDescription()));
  connect(this->markerRadiusSlider, SIGNAL(valueChanged(int)), this, SLOT(updateMarkerRadiusSpinBox(int)));
  connect(this->markerRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateMarkerRadius(double)));
  connect(this->markerRadiusSpinBox, SIGNAL(editingFinished()), this, SLOT(editMarkersRadius()));
  connect(this->markerRadiusSlider, SIGNAL(sliderReleased()), this, SLOT(editMarkersRadius()));
  connect(this->markerColorButton, SIGNAL(clicked()), this, SLOT(editMarkersColor()));
  connect(this->markerColorRecent1, SIGNAL(clicked()), this, SLOT(setMarkerColorFromRecent1()));
  connect(this->markerColorRecent2, SIGNAL(clicked()), this, SLOT(setMarkerColorFromRecent2()));
  connect(this->markerColorRecent3, SIGNAL(clicked()), this, SLOT(setMarkerColorFromRecent3()));
  connect(this->markerColorRecent4, SIGNAL(clicked()), this, SLOT(setMarkerColorFromRecent4()));
  connect(this->markerColorRecent5, SIGNAL(clicked()), this, SLOT(setMarkerColorFromRecent5()));
  connect(this->mp_ResetMarkerColorHistory, SIGNAL(triggered()), this, SLOT(resetMarkerRecentColors()));
  connect(this->mp_SetMarkerColorInHistory1, SIGNAL(triggered()), this, SLOT(setMarkerRecentColor1()));
  connect(this->mp_SetMarkerColorInHistory2, SIGNAL(triggered()), this, SLOT(setMarkerRecentColor2()));
  connect(this->mp_SetMarkerColorInHistory3, SIGNAL(triggered()), this, SLOT(setMarkerRecentColor3()));
  connect(this->mp_SetMarkerColorInHistory4, SIGNAL(triggered()), this, SLOT(setMarkerRecentColor4()));
  connect(this->mp_SetMarkerColorInHistory5, SIGNAL(triggered()), this, SLOT(setMarkerRecentColor5()));
  connect(this->pointLabelEdit, SIGNAL(editingFinished()), this, SLOT(editPointLabel()));
  connect(this->pointDescEdit, SIGNAL(editingFinished()), this, SLOT(editPointsDescription()));
  connect(this->analogLabelEdit, SIGNAL(editingFinished()), this, SLOT(editAnalogLabel()));
  connect(this->analogUnitEdit, SIGNAL(editingFinished()), this, SLOT(editAnalogsUnit()));
  connect(this->analogGainComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(editAnalogsGain(int)));
  connect(this->analogScaleSpinBox, SIGNAL(editingFinished()), this, SLOT(editAnalogsScale()));
  connect(this->analogOffsetSpinBox, SIGNAL(editingFinished()), this, SLOT(editAnalogsOffset()));
  connect(this->analogDescEdit, SIGNAL(editingFinished()), this, SLOT(editAnalogsDescription()));
};
  
ModelDockWidget::~ModelDockWidget()
{
  delete this->mp_DownArrowIcon;
  delete this->mp_RightArrowIcon;
  delete this->mp_MarkersIcon;
  delete this->mp_AnalogsIcon;
  delete this->mp_ModelOutputsIcon;
};

void ModelDockWidget::setAcquisition(Acquisition* acq)
{
  if (this->mp_Acquisition)
    disconnect(this->mp_Acquisition, 0, this, 0);
  this->mp_Acquisition = acq;
  connect(this->mp_Acquisition, SIGNAL(pointLabelChanged(int, QString)), this, SLOT(setPointLabel(int, QString)));
  connect(this->mp_Acquisition, SIGNAL(markersRadiusChanged(QVector<int>, QVector<double>)), this, SLOT(setMarkersRadius(QVector<int>, QVector<double>)));
  connect(this->mp_Acquisition, SIGNAL(markersColorChanged(QVector<int>, QVector<QColor>)), this, SLOT(setMarkersColor(QVector<int>, QVector<QColor>)));
  connect(this->mp_Acquisition, SIGNAL(pointsDescriptionChanged(QVector<int>, QVector<QString>)), this, SLOT(setPointsDescription(QVector<int>, QVector<QString>)));
  connect(this->mp_Acquisition, SIGNAL(pointsRemoved(QList<int>, QList<Point*>)), this, SLOT(removePoints(QList<int>, QList<Point*>)));
  connect(this->mp_Acquisition, SIGNAL(pointsInserted(QList<int>, QList<Point*>)), this, SLOT(insertPoints(QList<int>, QList<Point*>)));
  connect(this->mp_Acquisition, SIGNAL(analogLabelChanged(int, QString)), this, SLOT(setAnalogLabel(int, QString)));
  connect(this->mp_Acquisition, SIGNAL(analogsUnitChanged(QVector<int>, QVector<QString>)), this, SLOT(setAnalogsUnit(QVector<int>, QVector<QString>)));
  connect(this->mp_Acquisition, SIGNAL(analogsGainChanged(QVector<int>, QVector<Analog::Gain>)), this, SLOT(setAnalogsGain(QVector<int>, QVector<Analog::Gain>)));
  connect(this->mp_Acquisition, SIGNAL(analogsOffsetChanged(QVector<int>, QVector<int>)), this, SLOT(setAnalogsOffset(QVector<int>, QVector<int>)));
  connect(this->mp_Acquisition, SIGNAL(analogsScaleChanged(QVector<int>, QVector<double>)), this, SLOT(setAnalogsScale(QVector<int>, QVector<double>)));
  connect(this->mp_Acquisition, SIGNAL(analogsDescriptionChanged(QVector<int>, QVector<QString>)), this, SLOT(setAnalogsDescription(QVector<int>, QVector<QString>)));
  connect(this->mp_Acquisition, SIGNAL(analogsRemoved(QList<int>, QList<Analog*>)), this, SLOT(removeAnalogs(QList<int>, QList<Analog*>)));
  connect(this->mp_Acquisition, SIGNAL(analogsInserted(QList<int>, QList<Analog*>)), this, SLOT(insertAnalogs(QList<int>, QList<Analog*>)));
};
  
void ModelDockWidget::load()
{
  // Clear the UI
  this->reset();
  
  if (!this->mp_Acquisition)
    return;
    
  // Create the tree
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(2);
  QTreeWidgetItem* modelOutputAngles = modelOutputsRoot->child(0);
  QTreeWidgetItem* modelOutputForces = modelOutputsRoot->child(1);
  QTreeWidgetItem* modelOutputMoments = modelOutputsRoot->child(2);
  QTreeWidgetItem* modelOutputPowers = modelOutputsRoot->child(3);
  QTreeWidgetItem* modelOutputScalars = modelOutputsRoot->child(4);
  // - Points
  for (QMap<int, Point*>::const_iterator it = this->mp_Acquisition->points().begin() ; it != this->mp_Acquisition->points().end() ; ++it)
  {
    if (it.value()->type == Point::Marker)
      markersRoot->addChild(this->createMarkerItem(it.value()->label, it.key()));
    else if (it.value()->type == Point::VirtualMarker)
      markersRoot->addChild(this->createMarkerItem(it.value()->label, it.key(), false));
    else if (it.value()->type == Point::Angle)
      modelOutputAngles->addChild(this->createModelOutputItem(it.value()->label, it.key()));
    else if (it.value()->type == Point::Force)
      modelOutputForces->addChild(this->createModelOutputItem(it.value()->label, it.key()));
    else if (it.value()->type == Point::Moment)
      modelOutputMoments->addChild(this->createModelOutputItem(it.value()->label, it.key()));
    else if (it.value()->type == Point::Power)
      modelOutputPowers->addChild(this->createModelOutputItem(it.value()->label, it.key()));
    else if (it.value()->type == Point::Scalar)
      modelOutputScalars->addChild(this->createModelOutputItem(it.value()->label, it.key()));
  }
  markersRoot->setExpanded(true);
  // - Analogs
  QTreeWidgetItem* analogsRoot = this->modelTree->topLevelItem(1);
  for (QMap<int, Analog*>::const_iterator it = this->mp_Acquisition->analogs().begin() ; it != this->mp_Acquisition->analogs().end() ; ++it)
    analogsRoot->addChild(this->createAnalogItem(it.value()->label, it.key()));
  // Refresh the tree and these actions
  this->refresh();
  this->modelTree->clearSelection();
  // Enable the visual configuration
  this->modelConfigurationComboBox->setEnabled(true);
  this->modelConfigurationButton->setEnabled(true);
  // Recent markers' color
  // Extract the 5 most colors used
  // ...
  // Update the recent colors
  this->drawRecentColors();
  
  this->sendHiddenMarkers(); // For the virtual markers hideen by default.
};

void ModelDockWidget::reset()
{
  // Clean the tree
  this->modelTree->clearSelection();
  for (int i = 0 ; i < this->modelTree->topLevelItemCount() - 1 ; ++i)
  {
    QTreeWidgetItem* item = this->modelTree->topLevelItem(i);
    while (item->childCount() > 0)
      delete item->takeChild(0);
    item->setExpanded(false);
  }
  // Special case: model outputs
  QTreeWidgetItem* item = this->modelTree->topLevelItem(this->modelTree->topLevelItemCount() - 1);
  for (int i = 0 ; i < item->childCount() ; ++i)
  {
    QTreeWidgetItem* subitem = item->child(i);
    while (subitem->childCount() > 0)
      delete subitem->takeChild(0);
    subitem->setExpanded(false);
  }
  item->setExpanded(false);
  // Disable the visual configuration
  this->modelConfigurationComboBox->setEnabled(false);
  this->modelConfigurationButton->setEnabled(false);
  // Force the properties to the empty page
  this->propertiesStack->setCurrentIndex(0);
  // Refresh the tree and its actions
  this->refresh();
};

void ModelDockWidget::visualConfigurations(QStringList& names, QStringList& filenames)
{
  for (int i = 0 ; i < this->m_ConfigurationItems.count() ; ++i)
  {
    names << this->m_ConfigurationItems[i].name;
    filenames << this->m_ConfigurationItems[i].filename;
  }
};

void ModelDockWidget::setVisualConfigurations(const QStringList& names, const QStringList& filenames)
{
  this->modelConfigurationComboBox->blockSignals(true);
  for (int i = 0 ; i < names.count() ; ++i)
  {
    ConfigurationItem config;
    config.name = names[i];
    config.filename = filenames[i];
    config.isNew = false;
    config.isModified = false;
    this->m_ConfigurationItems.push_back(config);
    
    this->modelConfigurationComboBox->insertItem(i,names[i]);
  }
  this->modelConfigurationComboBox->setCurrentIndex(-1);
  this->modelConfigurationComboBox->blockSignals(false);
};

QList<int> ModelDockWidget::selectedMarkers()
{
  QList<int> ids;
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  for (int i = 0 ; i < markersRoot->childCount() ; ++i)
  {
    QTreeWidgetItem* item = markersRoot->child(i);
    if (item->isSelected())
      ids << item->data(0, pointId).toInt();
  }
  return ids;
};

bool ModelDockWidget::isOkToContinue()
{
  int idx = this->m_CurrentConfigurationIndex;//this->modelConfigurationComboBox->currentIndex();
  if ((idx != -1) && (this->m_ConfigurationItems[idx].isModified))
  {
    QString message = "The visual configuration has been modified.\nDo you want to save your changes?";
    if (this->m_ConfigurationItems[idx].isNew)
      message += "\n\nThis configuration is a new one and will be deleted if you do not save it.";
    
    QMessageBox messageBox(QMessageBox::Question, 
                           trUtf8("Mokka"),
                           trUtf8(message.toAscii().constData()), 
                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                           this, Qt::Sheet);
    messageBox.setDefaultButton(QMessageBox::Yes);
    messageBox.setEscapeButton(QMessageBox::Cancel);
    int res = messageBox.exec();
    if (res == QMessageBox::Yes)
      return this->saveConfiguration(idx);
    else if (res == QMessageBox::No)
    {
      if (this->m_ConfigurationItems[idx].isNew)
        this->removeConfiguration(idx);
      else
        this->setConfigurationModified(idx, false);
    }
    else if (res == QMessageBox::Cancel)
      return false;
  }
  return true;
};

void ModelDockWidget::selectConfiguration(int idx)
{
  this->modelConfigurationComboBox->blockSignals(true);
  this->modelConfigurationComboBox->setCurrentIndex(this->m_CurrentConfigurationIndex);
  this->modelConfigurationComboBox->blockSignals(false);
  if (this->isOkToContinue())
    this->loadConfiguration(this->m_ConfigurationItems[idx].filename);
  this->modelTree->setFocus();
};

void ModelDockWidget::setConfigurationModified(bool cleaned)
{
  int index = this->modelConfigurationComboBox->currentIndex();
  if (index != -1)
  {
    if (cleaned)
    {
      this->mp_SaveConfiguration->setEnabled(false);
      this->m_ConfigurationItems[index].isModified = false;
      this->modelConfigurationComboBox->setItemText(index, this->m_ConfigurationItems[index].name);
    }
    else
    {
      this->mp_SaveConfiguration->setEnabled(true);
      this->m_ConfigurationItems[index].isModified = true;
      this->modelConfigurationComboBox->setItemText(index, "*" + this->m_ConfigurationItems[index].name);
    }
  }
};

void ModelDockWidget::loadConfiguration(const QString& filename)
{
  QMessageBox messageBox(QMessageBox::Critical, 
                         trUtf8("Mokka"),
                         "",
                         QMessageBox::Ok,
                         this, Qt::Sheet);
  messageBox.setDefaultButton(QMessageBox::Ok);
  
  QFile file(filename);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    messageBox.setText("Cannot read file: " + filename + "\n" + file.errorString());
    messageBox.exec();
    return;
  }
  
  QString configName;
  QList<int> ids;
  QList<double> radii;
  QList<QColor> colors;
  QXmlStreamReader xmlReader(&file);
  if (xmlReader.readNextStartElement())
  {
    if ((xmlReader.name() == "MokkaModelVisualConfiguration") && (xmlReader.attributes().value("version") == "1.0"))
    {
      configName = xmlReader.attributes().value("name").toString();
      while (xmlReader.readNextStartElement())
      {
        if (xmlReader.name() == "MarkersList")
        {
          while (xmlReader.readNextStartElement())
          {
            if (xmlReader.name() == "Marker")
            {
              QXmlStreamAttributes att = xmlReader.attributes();
              int id = this->mp_Acquisition->findMarkers(att.value("label").toString());
              if (id != -1)
              {
                ids << id;
                radii << att.value("radius").toString().toDouble();
                colors << QColor((int)(att.value("colorR").toString().toDouble() * 255),
                                 (int)(att.value("colorG").toString().toDouble() * 255),
                                 (int)(att.value("colorB").toString().toDouble() * 255));
              }
            }
            else
              xmlReader.skipCurrentElement();
            xmlReader.readNext();
          }
        }
        else
          xmlReader.skipCurrentElement();
      }
    }
    else
      xmlReader.raiseError(QObject::tr("The file is not a Mokka Model Visual Configuration file."));
  }
  file.close();
  if (xmlReader.hasError())
  {
    messageBox.setText("Failed to parse file: " + filename + "\n" + xmlReader.errorString());
    messageBox.exec();
    return;
  }
  else if (file.error() != QFile::NoError)
  {
    messageBox.setText("Cannot read file: " + filename + "\n" + file.errorString());
    messageBox.exec();
    return;
  }
  
  this->modelConfigurationComboBox->blockSignals(true);
  int id = -1;
  for (int i = 0 ; i < this->m_ConfigurationItems.count() ; ++i)
  {
    if (this->m_ConfigurationItems[i].name.compare(configName) == 0)
    {
      id = i;
      break;
    }
  }
  if (id == -1)
  {
    ConfigurationItem config;
    config.name = configName;
    config.filename = filename;
    config.isNew = false;
    config.isModified = false;
    this->m_ConfigurationItems.push_back(config);
    this->modelConfigurationComboBox->addItem(configName);
    id = this->m_ConfigurationItems.count()-1;
  }
  this->setCurrentConfiguration(id);
  this->mp_SaveConfiguration->setEnabled(false);
  this->mp_RemoveConfiguration->setEnabled(true);
  this->mp_DeselectConfiguration->setEnabled(true);
  this->modelConfigurationComboBox->blockSignals(false);
  
  this->mp_Acquisition->setMarkersRadius(ids.toVector(), radii.toVector());
  this->mp_Acquisition->setMarkersColor(ids.toVector(), colors.toVector());
};

void ModelDockWidget::newConfiguration()
{
  if (!this->isOkToContinue())
    return;
  NewModelDialog nmd(&(this->m_ConfigurationItems), this);
  if (this->mp_Acquisition)
  {
    QFileInfo fileInfo(this->mp_Acquisition->fileName());
    QString name = fileInfo.baseName();
    nmd.setConfigurationName(name);
  }
  this->modelConfigurationComboBox->blockSignals(true);
  if (nmd.exec())
  {
    ConfigurationItem config;
    config.name = nmd.configurationName();
    config.filename = "";
    config.isNew = true;
    config.isModified = true;
    this->m_ConfigurationItems.push_back(config);
    
    this->modelConfigurationComboBox->addItem("*" + config.name);
    this->setCurrentConfiguration(this->m_ConfigurationItems.count()-1);
    
    this->mp_SaveConfiguration->setEnabled(true);
    this->mp_RemoveConfiguration->setEnabled(true);
    this->mp_DeselectConfiguration->setEnabled(true);
  }
  this->modelConfigurationComboBox->blockSignals(false);
};

void ModelDockWidget::loadConfiguration()
{
  if (!this->isOkToContinue())
    return;
  
  QFileInfo fileInfo(this->mp_Acquisition->fileName());
  
#if 1
  QString filename = QFileDialog::getOpenFileName(this,
    tr("Open Model Visual Configuration"),
    fileInfo.path(),
    tr("Model Visual Configuration Files (*.mvc)"));
  if (!filename.isEmpty())
    this->loadConfiguration(filename);
#else
  QFileDialog* openConfig = new QFileDialog(this);
  openConfig->setFileMode(QFileDialog::ExistingFile);
  openConfig->setDirectory(fileInfo.dir());
  openConfig->setNameFilter(tr("Model Visual Configuration Files (*.mvc)"));
  openConfig->setViewMode(QFileDialog::List);
  if (openConfig->exec())
    this->loadConfiguration(openConfig->selectedFiles()[0]);
  delete openConfig;
#endif
};

void ModelDockWidget::saveConfiguration()
{
  this->saveConfiguration(this->modelConfigurationComboBox->currentIndex());
};

void ModelDockWidget::removeConfiguration()
{
  if (!this->isOkToContinue())
    return;
  this->removeConfiguration(this->modelConfigurationComboBox->currentIndex());
};

void ModelDockWidget::deselectConfiguration()
{
  if (!this->isOkToContinue())
    return;
  this->modelConfigurationComboBox->blockSignals(true);
  this->modelConfigurationComboBox->setCurrentIndex(-1);
  this->mp_SaveConfiguration->setEnabled(false);
  this->mp_RemoveConfiguration->setEnabled(false);
  this->mp_DeselectConfiguration->setEnabled(false);
  this->modelConfigurationComboBox->blockSignals(false);
};

void ModelDockWidget::clearConfigurations()
{
  this->m_ConfigurationItems.clear();
  this->modelConfigurationComboBox->blockSignals(true);
  this->modelConfigurationComboBox->setCurrentIndex(-1);
  this->modelConfigurationComboBox->clear();
  this->mp_SaveConfiguration->setEnabled(false);
  this->mp_RemoveConfiguration->setEnabled(false);
  this->mp_DeselectConfiguration->setEnabled(false);
  this->modelConfigurationComboBox->blockSignals(false);
};

void ModelDockWidget::updateDisplayedMarkers(const QVector<int>& ids)
{
  static const QBrush defaultLabelColor = QBrush(QColor(Qt::gray));
  static const QBrush displayLabelColor = QBrush(QColor(Qt::black));
  this->modelTree->blockSignals(true);
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  for (int i = 0 ; i < markersRoot->childCount() ; ++i)
  {
    QTreeWidgetItem* item = markersRoot->child(i);
    int id = item->data(0,pointId).toInt();
    if ((id < ids.count()) && ids[id] && (item->checkState(VisibleHeader) == Qt::Checked))
      item->setForeground(0,displayLabelColor);
    else
      item->setForeground(0,defaultLabelColor);
  }
  this->modelTree->blockSignals(false);
};

void ModelDockWidget::selectMarkers(QList<int> ids)
{
  this->modelTree->blockSignals(true);
  this->modelTree->clearSelection();
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  markersRoot->setExpanded(true);
  this->modelTree->scrollToItem(markersRoot, QAbstractItemView::PositionAtTop);
  for (int i = 0 ; i < markersRoot->childCount() ; ++i)
  {
    QTreeWidgetItem* item = markersRoot->child(i);
    if (ids.contains(item->data(0,pointId).toInt()))
    {
      item->setSelected(true);
      this->modelTree->scrollToItem(item);
    }
  }
  this->modelTree->blockSignals(false);
  this->displayProperties();
  this->sendSelectedMarkers();
};

void ModelDockWidget::selectAllMarkers()
{
  this->modelTree->blockSignals(true);
  this->modelTree->clearSelection();
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  markersRoot->setExpanded(true);
  this->modelTree->scrollToItem(markersRoot, QAbstractItemView::PositionAtTop);
  for (int i = 0 ; i < markersRoot->childCount() ; ++i)
    markersRoot->child(i)->setSelected(true);
  this->modelTree->blockSignals(false);
  this->displayProperties();
  this->sendSelectedMarkers();
};

void ModelDockWidget::hideSelectedMarkers()
{
  this->modelTree->blockSignals(true);
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
    (*it)->setCheckState(VisibleHeader, Qt::Unchecked);
  this->modelTree->blockSignals(false);
  this->sendHiddenMarkers();
};

void ModelDockWidget::unhideSelectedMarkers()
{
  this->modelTree->blockSignals(true);
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
    (*it)->setCheckState(VisibleHeader, Qt::Checked);
  this->modelTree->blockSignals(false);
  this->sendHiddenMarkers();
};

void ModelDockWidget::selectAllAnalogs()
{
  this->modelTree->blockSignals(true);
  this->modelTree->clearSelection();
  QTreeWidgetItem* analogsRoot = this->modelTree->topLevelItem(1);
  analogsRoot->setExpanded(true);
  this->modelTree->scrollToItem(analogsRoot, QAbstractItemView::PositionAtTop);
  for (int i = 0 ; i < analogsRoot->childCount() ; ++i)
    analogsRoot->child(i)->setSelected(true);
  this->modelTree->blockSignals(false);
  this->displayProperties();
};

void ModelDockWidget::selectAllModelOutputs()
{
  this->selectAllScalars(false);
  this->selectAllPowers(true);
  this->selectAllMoments(true);
  this->selectAllForces(true);
  this->selectAllAngles(true);
};

void ModelDockWidget::selectAllAngles(bool appending)
{
  this->modelTree->blockSignals(true);
  if (!appending)
    this->modelTree->clearSelection();
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(2);
  QTreeWidgetItem* anglesRoot = modelOutputsRoot->child(0);
  anglesRoot->setExpanded(true);
  modelOutputsRoot->setExpanded(true);
  this->modelTree->scrollToItem(anglesRoot, QAbstractItemView::PositionAtTop);
  for (int i = 0 ; i < anglesRoot->childCount() ; ++i)
    anglesRoot->child(i)->setSelected(true);
  this->modelTree->blockSignals(false);
  this->displayProperties();
};

void ModelDockWidget::selectAllForces(bool appending)
{
  this->modelTree->blockSignals(true);
  if (!appending)
    this->modelTree->clearSelection();
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(2);
  QTreeWidgetItem* forcesRoot = modelOutputsRoot->child(1);
  forcesRoot->setExpanded(true);
  modelOutputsRoot->setExpanded(true);
  this->modelTree->scrollToItem(forcesRoot, QAbstractItemView::PositionAtTop);
  for (int i = 0 ; i < forcesRoot->childCount() ; ++i)
    forcesRoot->child(i)->setSelected(true);
  this->modelTree->blockSignals(false);
  this->displayProperties();
};

void ModelDockWidget::selectAllMoments(bool appending)
{
  this->modelTree->blockSignals(true);
  if (!appending)
    this->modelTree->clearSelection();
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(2);
  QTreeWidgetItem* momentsRoot = modelOutputsRoot->child(2);
  momentsRoot->setExpanded(true);
  modelOutputsRoot->setExpanded(true);
  this->modelTree->scrollToItem(momentsRoot, QAbstractItemView::PositionAtTop);
  for (int i = 0 ; i < momentsRoot->childCount() ; ++i)
    momentsRoot->child(i)->setSelected(true);
  this->modelTree->blockSignals(false);
  this->displayProperties();
};

void ModelDockWidget::selectAllPowers(bool appending)
{
  this->modelTree->blockSignals(true);
  if (!appending)
    this->modelTree->clearSelection();
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(2);
  QTreeWidgetItem* powersRoot = modelOutputsRoot->child(3);
  powersRoot->setExpanded(true);
  modelOutputsRoot->setExpanded(true);
  this->modelTree->scrollToItem(powersRoot, QAbstractItemView::PositionAtTop);
  for (int i = 0 ; i < powersRoot->childCount() ; ++i)
    powersRoot->child(i)->setSelected(true);
  this->modelTree->blockSignals(false);
  this->displayProperties();
};

void ModelDockWidget::selectAllScalars(bool appending)
{
  this->modelTree->blockSignals(true);
  if (!appending)
    this->modelTree->clearSelection();
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(2);
  QTreeWidgetItem* scalarsRoot = modelOutputsRoot->child(4);
  scalarsRoot->setExpanded(true);
  modelOutputsRoot->setExpanded(true);
  this->modelTree->scrollToItem(scalarsRoot, QAbstractItemView::PositionAtTop);
  for (int i = 0 ; i < scalarsRoot->childCount() ; ++i)
    scalarsRoot->child(i)->setSelected(true);
  this->modelTree->blockSignals(false);
  this->displayProperties();
};

void ModelDockWidget::removeSelectedItems()
{
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  QList<int> pointIds, analogIds;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
  {
    QVariant pV = (*it)->data(0, pointId);
    QVariant aV = (*it)->data(0, analogId);
    if (pV.isValid())
      pointIds.push_back(pV.toInt());
    else if (aV.isValid())
      analogIds.push_back(aV.toInt());
  }
  if (!pointIds.isEmpty())
    emit pointsRemoved(pointIds);
  if (!analogIds.isEmpty())
    emit analogsRemoved(analogIds);
  this->modelTree->clearSelection();
};

void ModelDockWidget::toggleProperties()
{
  if (!this->propertiesGroup->isVisible())
  {
    this->propertiesGroup->setVisible(true);
    this->propertiesButton->setIcon(*this->mp_DownArrowIcon);
  }
  else
  {
    this->propertiesGroup->setVisible(false);
    this->propertiesButton->setIcon(*this->mp_RightArrowIcon);
  }
};

void ModelDockWidget::displayProperties()
{
  bool multipleSelection = false;
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  if (items.size() == 0)
  {
    this->mp_HideSelectedMarkers->setEnabled(false);
    this->mp_UnhideSelectedMarkers->setEnabled(false);
    this->mp_RemoveSelectedItems->setEnabled(false);
    this->propertiesStack->setCurrentIndex(0);
    return;
  }
  else if (items.size() != 1)
  {
    // Check if the selection contains mixed types (markers, others points, etc.)
    for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
    {
      if ((*it)->type() != items.first()->type())
      {
        this->mp_HideSelectedMarkers->setEnabled(false);
        this->mp_UnhideSelectedMarkers->setEnabled(false);
        this->propertiesStack->setCurrentIndex(0);
        return;
      }
    }
    multipleSelection = true;
  }
  this->mp_RemoveSelectedItems->setEnabled(true);
  switch(items.first()->type())
  {
  case MarkerType:
    {
    this->markerRadiusSpinBox->blockSignals(true);
    this->mp_HideSelectedMarkers->setEnabled(true);
    this->mp_UnhideSelectedMarkers->setEnabled(true);
    int id = items.first()->data(0,pointId).toInt();
    this->markerLabelEdit->setText(multipleSelection ? "" : this->mp_Acquisition->pointLabel(id));
    this->markerLabelEdit->setEnabled(!multipleSelection);
    this->markerDescEdit->setText(multipleSelection ? "" : this->mp_Acquisition->pointDescription(id));
    this->mp_SetMarkerColorInHistory1->setEnabled(true);
    this->mp_SetMarkerColorInHistory2->setEnabled(true);
    this->mp_SetMarkerColorInHistory3->setEnabled(true);
    this->mp_SetMarkerColorInHistory4->setEnabled(true);
    this->mp_SetMarkerColorInHistory5->setEnabled(true);
    if (!multipleSelection)
    {
      double r = this->mp_Acquisition->markerRadius(id);
      this->markerRadiusSpinBox->setValue(r);
      // this->markerRadiusSlider->setValue(static_cast<int>(r));
      this->updateMarkerRadius(r); // slider
      QColor c = this->mp_Acquisition->markerColor(id);
      this->markerColorButton->setProperty("markerColor", c);
      this->markerColorButton->setIcon(this->createMarkerIcon(c, true));
    }
    else
    {
      bool uniqueRadius = true;
      bool uniqueColor = true;
      double r = this->mp_Acquisition->markerRadius(id);
      QColor c = this->mp_Acquisition->markerColor(id);
      QList<QTreeWidgetItem*>::const_iterator it = items.begin();
      ++it;
      while (it != items.end())
      {
        id = (*it)->data(0,pointId).toInt();
        if (this->mp_Acquisition->markerRadius(id) != r)
          uniqueRadius = false;
        if (this->mp_Acquisition->markerColor(id) != c)
          uniqueColor = false;
        ++it;
      }
      if (uniqueRadius)
      {
        this->markerRadiusSpinBox->setValue(r);
        this->markerRadiusSpinBox->setSuffix(" mm");
        //this->markerRadiusSlider->setValue(static_cast<int>(r));
        this->updateMarkerRadius(r); // slider
      }
      else
      {
        this->markerRadiusSpinBox->setSuffix("");
        this->markerRadiusSpinBox->clear();
        //this->markerRadiusSlider->setValue(0);
        this->updateMarkerRadius(0.0); // slider
      } 
      this->markerColorButton->setProperty("markerColor", uniqueColor ? c : QColor::Invalid);
      this->markerColorButton->setIcon(this->createMarkerIcon(uniqueColor ? c : Qt::transparent, true, uniqueColor ? true : false));
      this->mp_SetMarkerColorInHistory1->setEnabled(uniqueColor ? true : false);
      this->mp_SetMarkerColorInHistory2->setEnabled(uniqueColor ? true : false);
      this->mp_SetMarkerColorInHistory3->setEnabled(uniqueColor ? true : false);
      this->mp_SetMarkerColorInHistory4->setEnabled(uniqueColor ? true : false);
      this->mp_SetMarkerColorInHistory5->setEnabled(uniqueColor ? true : false);
    }
    this->markerRadiusSpinBox->blockSignals(false);
    this->propertiesStack->setCurrentIndex(1);
    break;
    }
  case PointType:
    {
    int id = items.first()->data(0,pointId).toInt();
    this->pointLabelEdit->setText(multipleSelection ? "" : this->mp_Acquisition->pointLabel(id));
    this->pointLabelEdit->setEnabled(!multipleSelection);
    this->pointDescEdit->setText(multipleSelection ? "" : this->mp_Acquisition->pointDescription(id));
    this->propertiesStack->setCurrentIndex(2);
    break;
    }
  case AnalogType:
    {
    int id = items.first()->data(0,analogId).toInt();
    this->analogLabelEdit->setText(multipleSelection ? "" : this->mp_Acquisition->analogLabel(id));
    this->analogLabelEdit->setEnabled(!multipleSelection);
    this->analogDescEdit->setText(multipleSelection ? "" : this->mp_Acquisition->analogDescription(id));
    bool uniqueUnit = true, uniqueGain = true, uniqueOffset = true, uniqueScale = true;
    QString u = this->mp_Acquisition->analogUnit(id);
    Analog::Gain g = this->mp_Acquisition->analogGain(id);
    int o = this->mp_Acquisition->analogOffset(id);
    double s = this->mp_Acquisition->analogScale(id);
    QList<QTreeWidgetItem*>::const_iterator it = items.begin(); ++it;
    while (it != items.end())
    {
      id = (*it)->data(0,analogId).toInt();
      if (this->mp_Acquisition->analogUnit(id).compare(u) != 0)
        uniqueUnit = false;
      if (this->mp_Acquisition->analogGain(id) != g)
        uniqueGain = false;
      if (this->mp_Acquisition->analogOffset(id) != o)
        uniqueOffset = false;
      if (this->mp_Acquisition->analogScale(id) != s)
        uniqueScale = false;
      ++it;
    }
    this->analogUnitEdit->setText(uniqueUnit ? u : "");
    this->analogGainComboBox->blockSignals(true);
    this->analogGainComboBox->setCurrentIndex(uniqueGain ? g : 0);
    this->analogGainComboBox->blockSignals(false);
    this->analogScaleSpinBox->setValue(uniqueScale ? s : 0.0); if (!uniqueOffset) this->analogScaleSpinBox->clear();
    this->analogOffsetSpinBox->setValue(uniqueOffset ? o : 0); if (!uniqueOffset) this->analogOffsetSpinBox->clear();
    this->propertiesStack->setCurrentIndex(3);
    break;
    }
  default:
    this->propertiesStack->setCurrentIndex(0);
    break;
  }
};

void ModelDockWidget::sendSelectedMarkers()
{
  QList<int> ids;
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
  {
    if ((*it)->type() == MarkerType)
      ids << (*it)->data(0,pointId).toInt();
  }
  emit markerSelectionChanged(ids);
};

void ModelDockWidget::sendHiddenMarkers()
{
  QList<int> ids;
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  for (int i = 0 ; i < markersRoot->childCount() ; ++i)
  {
    QTreeWidgetItem* item = markersRoot->child(i);
    if ((item->checkState(VisibleHeader) == Qt::Unchecked) || item->isHidden())
      ids << item->data(0,pointId).toInt();
  }
  emit markerHiddenSelectionChanged(ids);
};

void ModelDockWidget::sendTailedMarkers()
{
  QList<int> ids;
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  for (int i = 0 ; i < markersRoot->childCount() ; ++i)
  {
    QTreeWidgetItem* item = markersRoot->child(i);
    if (item->checkState(TrajectoryHeader) == Qt::Checked)
      ids << item->data(0,pointId).toInt();
  }
  emit markerTrajectorySelectionChanged(ids);
};

void ModelDockWidget::sendModifiedMarkersState(QTreeWidgetItem* item, int column)
{
  Q_UNUSED(item);
  if (column == VisibleHeader)
    this->sendHiddenMarkers();
  else if (column == TrajectoryHeader)
    this->sendTailedMarkers();
};

void ModelDockWidget::editMarkerLabel()
{
  int id = this->modelTree->currentItem()->data(0, pointId).toInt();
  QString label = this->markerLabelEdit->text();
  if (label.compare(this->mp_Acquisition->pointLabel(id)) != 0)
    emit markerLabelChanged(id, label);
}

void ModelDockWidget::editMarkersDescription()
{
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  QVector<int> ids = QVector<int>(items.count());
  int inc = 0;
  QString desc = this->markerDescEdit->text();
  bool descModified = false;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
  {
    ids[inc] = (*it)->data(0, pointId).toInt();
    if (this->mp_Acquisition->pointDescription(ids[inc]).compare(desc) != 0)
      descModified = true;
    ++inc;
  }
  if (descModified)
    emit markersDescriptionChanged(ids, desc);
}

void ModelDockWidget::updateMarkerRadiusSpinBox(int v)
{
  this->markerRadiusSpinBox->setValue(static_cast<double>(v));
};

void ModelDockWidget::updateMarkerRadius(double r)
{
  this->markerRadiusSlider->blockSignals(true);
  this->markerRadiusSlider->setValue(static_cast<int>(r));
  this->markerRadiusSlider->blockSignals(false);
};

void ModelDockWidget::editMarkersRadius()
{
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  QVector<int> ids = QVector<int>(items.count());
  int inc = 0;
  double radius = this->markerRadiusSpinBox->value();
  bool radiusModified = false;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
  {
    ids[inc] = (*it)->data(0, pointId).toInt();
    if (this->mp_Acquisition->markerRadius(ids[inc]) != radius)
      radiusModified = true;
    ++inc;
  }
  if (radiusModified)
    emit markersRadiusChanged(ids, radius);
};

void ModelDockWidget::setMarkersRadius(const QVector<int>& ids, const QVector<double>& radii)
{
  Q_UNUSED(ids);
  Q_UNUSED(radii);
  this->displayProperties();
};

void ModelDockWidget::editMarkersColor()
{
  QColor color = QColorDialog::getColor(Qt::white, this);
  if (color.isValid())
    this->editMarkersColor(color);
};

void ModelDockWidget::setMarkerColorFromRecent1()
{
  this->editMarkersColor(this->m_RecentColors[0]);
};

void ModelDockWidget::setMarkerColorFromRecent2()
{
  this->editMarkersColor(this->m_RecentColors[1]);
};

void ModelDockWidget::setMarkerColorFromRecent3()
{
  this->editMarkersColor(this->m_RecentColors[2]);
};

void ModelDockWidget::setMarkerColorFromRecent4()
{
  this->editMarkersColor(this->m_RecentColors[3]);
};

void ModelDockWidget::setMarkerColorFromRecent5()
{
  this->editMarkersColor(this->m_RecentColors[4]);
};

void ModelDockWidget::resetMarkerRecentColors()
{
  this->m_RecentColors.clear();
  this->m_RecentColors.resize(5);
  this->drawRecentColors();
};

void ModelDockWidget::setMarkerRecentColor1()
{
  this->setRecentColor(0, this->markerColorButton->property("markerColor").value<QColor>());
};

void ModelDockWidget::setMarkerRecentColor2()
{
  this->setRecentColor(1, this->markerColorButton->property("markerColor").value<QColor>());
};

void ModelDockWidget::setMarkerRecentColor3()
{
  this->setRecentColor(2, this->markerColorButton->property("markerColor").value<QColor>());
};

void ModelDockWidget::setMarkerRecentColor4()
{
  this->setRecentColor(3, this->markerColorButton->property("markerColor").value<QColor>());
};

void ModelDockWidget::setMarkerRecentColor5()
{
  this->setRecentColor(4, this->markerColorButton->property("markerColor").value<QColor>());
};

void ModelDockWidget::editPointLabel()
{
  int id = this->modelTree->currentItem()->data(0, pointId).toInt();
  QString label = this->pointLabelEdit->text();
  if (label.compare(this->mp_Acquisition->pointLabel(id)) != 0)
    emit pointLabelChanged(id, label);
}

void ModelDockWidget::setPointLabel(int id, const QString& label)
{
  // Try with markers
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  for (int i = 0 ; i < markersRoot->childCount() ; ++i)
  {
    if (markersRoot->child(i)->data(0, pointId).toInt() == id)
    {
      markersRoot->child(i)->setText(0, label);
      this->displayProperties();
      return;
    }
  }
  // Other points
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(this->modelTree->topLevelItemCount() - 1);
  for (int i = 0 ; i < modelOutputsRoot->childCount() ; ++i)
  {
    QTreeWidgetItem* child = modelOutputsRoot->child(i);
    for (int j = 0 ; j < modelOutputsRoot->childCount() ; ++j)
    {
      if (child->child(j)->data(0, pointId).toInt() == id)
      {
        child->child(j)->setText(0, label);
        this->displayProperties();
        return;
      }
    }
  }
};

void ModelDockWidget::editPointsDescription()
{
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  QVector<int> ids = QVector<int>(items.count());
  int inc = 0;
  QString desc = this->pointDescEdit->text();
  bool descModified = false;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
  {
    ids[inc] = (*it)->data(0, pointId).toInt();
    if (this->mp_Acquisition->pointDescription(ids[inc]).compare(desc) != 0)
      descModified = true;
    ++inc;
  }
  if (descModified)
    emit pointsDescriptionChanged(ids, desc);
}

void ModelDockWidget::setPointsDescription(const QVector<int>& ids, const QVector<QString>& descs)
{
  Q_UNUSED(ids);
  Q_UNUSED(descs);
  this->displayProperties();
};

void ModelDockWidget::removePoints(const QList<int>& ids, const QList<Point*>& points)
{
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(2);
  QTreeWidgetItem* modelOutputAngles = modelOutputsRoot->child(0);
  QTreeWidgetItem* modelOutputForces = modelOutputsRoot->child(1);
  QTreeWidgetItem* modelOutputMoments = modelOutputsRoot->child(2);
  QTreeWidgetItem* modelOutputPowers = modelOutputsRoot->child(3);
  QTreeWidgetItem* modelOutputScalars = modelOutputsRoot->child(4);
  for (int i = 0 ; i < ids.count() ; ++i)
  {
    switch (points[i]->type)
    {
    case Point::Marker:
    case Point::VirtualMarker:
      this->treePointChild(markersRoot, ids[i])->setHidden(true);
      break;
    case Point::Angle:
      this->treePointChild(modelOutputAngles, ids[i])->setHidden(true);
      break;
    case Point::Force:
      this->treePointChild(modelOutputForces, ids[i])->setHidden(true);
      break;
    case Point::Moment:
      this->treePointChild(modelOutputMoments, ids[i])->setHidden(true);
      break;
    case Point::Power:
      this->treePointChild(modelOutputPowers, ids[i])->setHidden(true);
      break;
    case Point::Scalar:
      this->treePointChild(modelOutputScalars, ids[i])->setHidden(true);
      break;
    default:
      break;
    }
  }
  this->sendHiddenMarkers();
  this->refresh();
};

// TODO: Think about the case where points are created instead of hidden!
void ModelDockWidget::insertPoints(const QList<int>& ids, const QList<Point*>& points)
{
  Q_UNUSED(points);
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(2);
  QTreeWidgetItem* modelOutputAngles = modelOutputsRoot->child(0);
  QTreeWidgetItem* modelOutputForces = modelOutputsRoot->child(1);
  QTreeWidgetItem* modelOutputMoments = modelOutputsRoot->child(2);
  QTreeWidgetItem* modelOutputPowers = modelOutputsRoot->child(3);
  QTreeWidgetItem* modelOutputScalars = modelOutputsRoot->child(4);
  for (int i = 0 ; i < ids.count() ; ++i)
  {
    switch (points[i]->type)
    {
    case Point::Marker:
    case Point::VirtualMarker:
      this->treePointChild(markersRoot, ids[i])->setHidden(false);
      break;
    case Point::Angle:
      this->treePointChild(modelOutputAngles, ids[i])->setHidden(false);
      break;
    case Point::Force:
      this->treePointChild(modelOutputForces, ids[i])->setHidden(false);
      break;
    case Point::Moment:
      this->treePointChild(modelOutputMoments, ids[i])->setHidden(false);
      break;
    case Point::Power:
      this->treePointChild(modelOutputPowers, ids[i])->setHidden(false);
      break;
    case Point::Scalar:
      this->treePointChild(modelOutputScalars, ids[i])->setHidden(false);
      break;
    default:
      break;
    }
  }
  this->sendHiddenMarkers();
  this->refresh();
};

void ModelDockWidget::editAnalogLabel()
{
  int id = this->modelTree->currentItem()->data(0, analogId).toInt();
  QString label = this->analogLabelEdit->text();
  if (label.compare(this->mp_Acquisition->analogLabel(id)) != 0)
    emit analogLabelChanged(id, label);
};

void ModelDockWidget::setAnalogLabel(int id, const QString& label)
{
  QTreeWidgetItem* analogsRoot = this->modelTree->topLevelItem(1);
  for (int i = 0 ; i < analogsRoot->childCount() ; ++i)
  {
    if (analogsRoot->child(i)->data(0, analogId).toInt() == id)
    {
      analogsRoot->child(i)->setText(0, label);
      break;
    }
  }
  this->displayProperties();
};

void ModelDockWidget::editAnalogsUnit()
{
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  QVector<int> ids = QVector<int>(items.count());
  int inc = 0;
  QString unit = this->analogUnitEdit->text();
  bool unitModified = false;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
  {
    ids[inc] = (*it)->data(0, analogId).toInt();
    if (this->mp_Acquisition->analogUnit(ids[inc]).compare(unit) != 0)
      unitModified = true;
    ++inc;
  }
  if (unitModified)
    emit analogsUnitChanged(ids, unit);
};

void ModelDockWidget::setAnalogsUnit(const QVector<int>& ids, const QVector<QString>& units)
{
  Q_UNUSED(ids);
  Q_UNUSED(units);
  this->displayProperties();
};

void ModelDockWidget::editAnalogsGain(int index)
{
  if (!this->isOkToModifyAnalog())
  {
    this->displayProperties();
    return;
  }
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  QVector<int> ids = QVector<int>(items.count());
  int inc = 0;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
    ids[inc++] = (*it)->data(0, analogId).toInt();    
  emit analogsGainChanged(ids, static_cast<Analog::Gain>(index));
};

void ModelDockWidget::setAnalogsGain(const QVector<int>& ids, const QVector<Analog::Gain>& gains)
{
  Q_UNUSED(ids);
  Q_UNUSED(gains);
  this->displayProperties();
};

void ModelDockWidget::editAnalogsOffset()
{
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  QVector<int> ids = QVector<int>(items.count());
  int inc = 0;
  int offset = this->analogOffsetSpinBox->value();
  bool offsetModified = false;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
  {
    ids[inc] = (*it)->data(0, analogId).toInt();
    if (this->mp_Acquisition->analogOffset(ids[inc]) != offset)
      offsetModified = true;
    ++inc;
  }
  if (offsetModified)
  {
    this->analogOffsetSpinBox->blockSignals(true);
    if (!this->isOkToModifyAnalog())
    {
      this->analogOffsetSpinBox->clear();
      this->displayProperties();
    }
    else
      emit analogsOffsetChanged(ids, offset);
    this->analogOffsetSpinBox->blockSignals(true);
  }
};

void ModelDockWidget::setAnalogsOffset(const QVector<int>& ids, const QVector<int>& offsets)
{
  Q_UNUSED(ids);
  Q_UNUSED(offsets);
  this->displayProperties();
};

void ModelDockWidget::editAnalogsScale()
{
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  QVector<int> ids = QVector<int>(items.count());
  int inc = 0;
  double scale = this->analogScaleSpinBox->value();
  bool scaleModified = false;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
  {
    ids[inc] = (*it)->data(0, analogId).toInt();
    if (fabs(this->mp_Acquisition->analogScale(ids[inc]) - scale) >= 1.0e-6)
      scaleModified = true;
    ++inc;
  }
  if (scaleModified)
  {
    this->analogScaleSpinBox->blockSignals(true);
    if (!this->isOkToModifyAnalog())
    {
      this->analogScaleSpinBox->clear();
      this->displayProperties();
    }
    else
      emit analogsScaleChanged(ids, scale);
    this->analogScaleSpinBox->blockSignals(false);
  }
};

void ModelDockWidget::setAnalogsScale(const QVector<int>& ids, const QVector<double>& scales)
{
  Q_UNUSED(ids);
  Q_UNUSED(scales);
  this->displayProperties();
};

void ModelDockWidget::editAnalogsDescription()
{
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  QVector<int> ids = QVector<int>(items.count());
  int inc = 0;
  QString desc = this->analogDescEdit->text();
  bool descModified = false;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
  {
    ids[inc] = (*it)->data(0, analogId).toInt();
    if (this->mp_Acquisition->analogDescription(ids[inc]).compare(desc) != 0)
      descModified = true;
    ++inc;
  }
  if (descModified)
    emit analogsDescriptionChanged(ids, desc);
};

void ModelDockWidget::setAnalogsDescription(const QVector<int>& ids, const QVector<QString>& descs)
{
  Q_UNUSED(ids);
  Q_UNUSED(descs);
  this->displayProperties();
};

void ModelDockWidget::removeAnalogs(const QList<int>& ids, const QList<Analog*>& analogs)
{
  Q_UNUSED(analogs);
  QTreeWidgetItem* analogsRoot = this->modelTree->topLevelItem(1);
  for (int i = 0 ; i < ids.count() ; ++i)
    analogsRoot->child(ids[i])->setHidden(true);
  this->refresh();
};

// TODO: Think about the case where analog channels are created instead of hidden!
void ModelDockWidget::insertAnalogs(const QList<int>& ids, const QList<Analog*>& analogs)
{
  Q_UNUSED(analogs);
  QTreeWidgetItem* analogsRoot = this->modelTree->topLevelItem(1);
  for (int i = 0 ; i < ids.count() ; ++i)
    analogsRoot->child(ids[i])->setHidden(false);
  this->refresh();
};

void ModelDockWidget::setCurrentConfiguration(int idx)
{
  this->modelConfigurationComboBox->setCurrentIndex(idx);
  this->m_CurrentConfigurationIndex = idx;
};

bool ModelDockWidget::saveConfiguration(int idx)
{
  QString filename;
  if (this->m_ConfigurationItems[idx].isNew)
  {
    QFileInfo fileInfo(this->mp_Acquisition->fileName());
#if 1
    filename = QFileDialog::getSaveFileName(this,
                         tr("Save Model Visual Configuration"),
                         fileInfo.path() + "/" + this->m_ConfigurationItems[idx].name + ".mvc",
                         tr("Model Visual Configuration Files (*.mvc)"));
    if (filename.isEmpty())
      return false;
    else
      this->m_ConfigurationItems[idx].filename = filename;
#else
    QFileDialog* saveConfig = new QFileDialog(this);
    saveConfig->setFileMode(QFileDialog::AnyFile);
    saveConfig->setAcceptMode(QFileDialog::AcceptSave);
    saveConfig->setDirectory(fileInfo.dir());
    selectFile(fileName)
    saveConfig->setNameFilter(tr("Model Visual Configuration Files (*.mvc)"));
    saveConfig->setViewMode(QFileDialog::List);
    bool newFile = saveConfig->exec();
    delete saveConfig;
    if (newFile)
      this->m_ConfigurationItems[idx].filename = saveConfig->selectedFiles()[0];
    else
      return false;
#endif
  }
  else
    filename = this->m_ConfigurationItems[idx].filename;
  
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  
  QMessageBox messageBox(QMessageBox::Critical, 
                         trUtf8("Mokka"),
                         trUtf8("Cannot write file: "),
                         QMessageBox::Ok,
                         this, Qt::Sheet);
  messageBox.setDefaultButton(QMessageBox::Ok);
  
  QFile file(filename);
  if (!file.open(QFile::WriteOnly | QFile::Text))
  {
    messageBox.setText(messageBox.text() + filename + "\n" + file.errorString());
    messageBox.exec();
    return false;
  }
  
  QXmlStreamWriter xmlWriter(&file);
  xmlWriter.setAutoFormatting(true);
  xmlWriter.writeStartDocument();
  xmlWriter.writeStartElement("MokkaModelVisualConfiguration");
  xmlWriter.writeAttribute("name", this->m_ConfigurationItems[idx].name);
  xmlWriter.writeAttribute("version", "1.0");
  xmlWriter.writeStartElement("MarkersList");
  
  for (int i = 0 ; i < markersRoot->childCount() ; ++i)
  {
    QTreeWidgetItem* item = markersRoot->child(i);
    if (!item->isHidden())
    {
      int id = item->data(0,pointId).toInt();
      xmlWriter.writeStartElement("Marker");
      xmlWriter.writeAttribute("label", item->text(0));
      xmlWriter.writeAttribute("radius", QString::number(this->mp_Acquisition->markerRadius(id)));
      QColor c = this->mp_Acquisition->markerColor(id);
      xmlWriter.writeAttribute("colorR", QString::number(c.redF()));
      xmlWriter.writeAttribute("colorG", QString::number(c.greenF()));
      xmlWriter.writeAttribute("colorB", QString::number(c.blueF()));
      xmlWriter.writeEndElement();
    }
  }
  xmlWriter.writeEndElement(); // MarkersConfiguration
  xmlWriter.writeEndElement(); // MokkaModelVisualConfiguration
  xmlWriter.writeEndDocument();
  file.close();
  if (file.error())
  {
      messageBox.setText(messageBox.text() + filename + "\n" + file.errorString());
      messageBox.exec();
      return false;
  }
  
  emit configurationSaved();
  
  this->m_ConfigurationItems[idx].isNew = false;
  this->m_ConfigurationItems[idx].isModified = false;
  this->mp_SaveConfiguration->setEnabled(false);
  this->modelConfigurationComboBox->setItemText(idx, this->m_ConfigurationItems[idx].name);
  return true;
};

void ModelDockWidget::removeConfiguration(int idx)
{
  this->m_ConfigurationItems.removeAt(idx);
  this->modelConfigurationComboBox->blockSignals(true);
  this->modelConfigurationComboBox->removeItem(idx);
  this->modelConfigurationComboBox->setCurrentIndex(-1);
  this->mp_SaveConfiguration->setEnabled(false);
  this->mp_RemoveConfiguration->setEnabled(false);
  this->mp_DeselectConfiguration->setEnabled(false);
  this->modelConfigurationComboBox->blockSignals(false);
};

void ModelDockWidget::setConfigurationModified(int idx, bool modified)
{
  this->m_ConfigurationItems[idx].isModified = modified;
  this->modelConfigurationComboBox->setItemText(idx, modified ? "*" + this->m_ConfigurationItems[idx].name : this->m_ConfigurationItems[idx].name);  
  this->mp_SaveConfiguration->setEnabled(modified ? false : true);
};

QTreeWidgetItem* ModelDockWidget::createMarkerItem(const QString& label, int id, bool checked)
{
  QTreeWidgetItem* markerItem = new QTreeWidgetItem(QStringList(label), MarkerType);
  markerItem->setIcon(LabelHeader, this->createMarkerIcon(Qt::white));
  markerItem->setCheckState(VisibleHeader, checked ? Qt::Checked : Qt::Unchecked);
  markerItem->setCheckState(TrajectoryHeader, Qt::Unchecked);
  markerItem->setData(0, pointId, id);
  return markerItem;
}

QTreeWidgetItem* ModelDockWidget::createAnalogItem(const QString& label, int id)
{
  QTreeWidgetItem* analogItem = new QTreeWidgetItem(QStringList(label), AnalogType);
  analogItem->setIcon(LabelHeader, *this->mp_AnalogsIcon);
  analogItem->setData(0, analogId, id);
  return analogItem;
};

QTreeWidgetItem* ModelDockWidget::createModelOutputItem(const QString& label, int id)
{
  QTreeWidgetItem* modelOutputItem = new QTreeWidgetItem(QStringList(label), PointType);
  modelOutputItem->setIcon(LabelHeader, *this->mp_ModelOutputsIcon);
  modelOutputItem->setData(0, pointId, id);
  return modelOutputItem;
};

QPixmap ModelDockWidget::createMarkerIcon(const QColor& c, bool circled, bool enabled) const
{
  QColor col = c;
  QPen p = Qt::NoPen;
  QImage markerImage(16, 16, QImage::Format_ARGB32);
  QPainter painter(&markerImage);
  painter.setRenderHint(QPainter::Antialiasing);
  if (((c.red() >= 240) && (c.green() >= 240) && (c.blue() >= 240))
      && ((c.red() == c.green()) && (c.red() == c.blue())))
  {
    col = QColor(240,240,240); // very light gray
    p = col;
  }
  painter.setBrush(Qt::NoBrush);
  painter.setPen(Qt::NoPen);
  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.fillRect(markerImage.rect(), Qt::transparent);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  if (enabled && col.isValid())
  {
    QRadialGradient gradient = QRadialGradient(6.0, 6.0, 8.0);
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, col);
    painter.setBrush(gradient);
    painter.setPen(p);
  }
  if (!enabled || circled)
    painter.setPen(Qt::lightGray);
  painter.drawEllipse(QRectF(1.5, 1.5, 13.0, 13.0));
  return QPixmap::fromImage(markerImage);
};

void ModelDockWidget::editMarkersColor(const QColor& color)
{
  QList<QTreeWidgetItem*> items = this->modelTree->selectedItems();
  // Upate the tree
  QVector<int> ids = QVector<int>(items.count());
  int inc = 0;
  for (QList<QTreeWidgetItem*>::const_iterator it = items.begin() ; it != items.end() ; ++it)
    ids[inc++] = (*it)->data(0, pointId).toInt();
  emit markersColorChanged(ids, color);
  // Upate the button
  this->markerColorButton->setProperty("markerColor", color);
  this->markerColorButton->setIcon(this->createMarkerIcon(color, true));
  // Update the recents colors
  this->pushRecentColor(color);
};

void ModelDockWidget::setMarkersColor(const QVector<int>& ids, const QVector<QColor>& colors)
{
  // Upate the tree
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  for (int i = 0 ; i < ids.count() ; ++i)
  {
    for (int j = 0 ; j < markersRoot->childCount() ; ++j)
    {
      if (markersRoot->child(j)->data(0, pointId).toInt() == ids[i])
      {
        markersRoot->child(j)->setIcon(0, this->createMarkerIcon(colors[i]));
        break;
      }
    }
  }
  this->displayProperties();
};

void ModelDockWidget::setRecentColor(int idx, const QColor& color)
{
  if (this->m_RecentColors[idx] == color)
    return;
  for (int i = 0 ; i < (this->m_RecentColors.size()-1) ; ++i)
  {
    if (this->m_RecentColors[i] == color)
    {
      this->m_RecentColors[i] = QColor();
      break;
    }
  }
  this->m_RecentColors[idx] = color;
  this->drawRecentColors();
};

void ModelDockWidget::pushRecentColor(const QColor& color)
{
  QColor c = color; // Requires a copy when removing and assigning the same color ...
  int invalidIndex = -1;
  for (int i = 0 ; i < (this->m_RecentColors.size()-1) ; ++i)
  {
    if (!this->m_RecentColors[i].isValid() && this->m_RecentColors[i+1].isValid())
    {
      invalidIndex = i;
      break;
    }
  }
  if (invalidIndex == -1)
  {
    for (int i = 0 ; i < this->m_RecentColors.size() ; ++i)
    {
      if (this->m_RecentColors[i] == color)
      {
        this->m_RecentColors.remove(i);
        break;
      }
    }
    this->m_RecentColors.push_front(c);
    this->m_RecentColors.resize(5);
  }
  else
  {
    for (int i = invalidIndex ; i > 0 ; --i)
      this->m_RecentColors[i] = this->m_RecentColors[i-1];
    this->setRecentColor(0, c);
  }
  this->drawRecentColors();
};

void ModelDockWidget::drawRecentColors()
{
  this->markerColorRecent1->setIcon(this->createMarkerIcon(this->m_RecentColors[0],true));
  this->markerColorRecent2->setIcon(this->createMarkerIcon(this->m_RecentColors[1],true));
  this->markerColorRecent3->setIcon(this->createMarkerIcon(this->m_RecentColors[2],true));
  this->markerColorRecent4->setIcon(this->createMarkerIcon(this->m_RecentColors[3],true));
  this->markerColorRecent5->setIcon(this->createMarkerIcon(this->m_RecentColors[4],true));
};

void ModelDockWidget::refresh()
{
  this->mp_SelectAllMarkers->setEnabled(false);
  this->mp_SelectAllAnalogs->setEnabled(false);
  this->mp_SelectAllModelOutputs->setEnabled(false);
  this->mp_SelectAllAngles->setEnabled(false);
  this->mp_SelectAllForces->setEnabled(false);
  this->mp_SelectAllMoments->setEnabled(false);
  this->mp_SelectAllPowers->setEnabled(false);
  this->mp_SelectAllScalars->setEnabled(false);
    
  QTreeWidgetItem* markersRoot = this->modelTree->topLevelItem(0);
  QTreeWidgetItem* analogsRoot = this->modelTree->topLevelItem(1);
  QTreeWidgetItem* modelOutputsRoot = this->modelTree->topLevelItem(2);
  QTreeWidgetItem* modelOutputAngles = modelOutputsRoot->child(0);
  QTreeWidgetItem* modelOutputForces = modelOutputsRoot->child(1);
  QTreeWidgetItem* modelOutputMoments = modelOutputsRoot->child(2);
  QTreeWidgetItem* modelOutputPowers = modelOutputsRoot->child(3);
  QTreeWidgetItem* modelOutputScalars = modelOutputsRoot->child(4);
  
  markersRoot->setHidden(true);
  analogsRoot->setHidden(true);
  modelOutputsRoot->setHidden(true);
  modelOutputAngles->setHidden(true);
  modelOutputForces->setHidden(true);
  modelOutputMoments->setHidden(true);
  modelOutputPowers->setHidden(true);
  modelOutputScalars->setHidden(true);
  
  if ((markersRoot->childCount() != 0) && this->hasChildVisible(markersRoot))
  {
    this->mp_SelectAllMarkers->setEnabled(true);
    markersRoot->setHidden(false);
  }
  if ((analogsRoot->childCount() != 0) && this->hasChildVisible(analogsRoot))
  {
    this->mp_SelectAllAnalogs->setEnabled(true);
    analogsRoot->setHidden(false);
  }
  if ((modelOutputAngles->childCount() != 0) && this->hasChildVisible(modelOutputAngles))
  {
    this->mp_SelectAllAngles->setEnabled(true);
    this->mp_SelectAllModelOutputs->setEnabled(true);
    modelOutputAngles->setHidden(false);
    modelOutputsRoot->setHidden(false);
  }
  if ((modelOutputForces->childCount() != 0) && this->hasChildVisible(modelOutputForces))
  {
    this->mp_SelectAllForces->setEnabled(true);
    this->mp_SelectAllModelOutputs->setEnabled(true);
    modelOutputForces->setHidden(false);
    modelOutputsRoot->setHidden(false);
  }
  if ((modelOutputMoments->childCount() != 0) && this->hasChildVisible(modelOutputMoments))
  {
    this->mp_SelectAllMoments->setEnabled(true);
    this->mp_SelectAllModelOutputs->setEnabled(true);
    modelOutputMoments->setHidden(false);
    modelOutputsRoot->setHidden(false);
  }
  if ((modelOutputPowers->childCount() != 0) && this->hasChildVisible(modelOutputPowers))
  {
    this->mp_SelectAllPowers->setEnabled(true);
    this->mp_SelectAllModelOutputs->setEnabled(true);
    modelOutputPowers->setHidden(false);
    modelOutputsRoot->setHidden(false);
  }
  if ((modelOutputScalars->childCount() != 0) && this->hasChildVisible(modelOutputScalars))
  {
    this->mp_SelectAllScalars->setEnabled(true);
    this->mp_SelectAllModelOutputs->setEnabled(true);
    modelOutputScalars->setHidden(false);
    modelOutputsRoot->setHidden(false);
  }
};

QTreeWidgetItem* ModelDockWidget::treePointChild(QTreeWidgetItem* parent, int id) const
{
  for (int i = 0 ; i < parent->childCount() ; ++i)
  {
    QTreeWidgetItem* child = parent->child(i);
    if (child->data(0, pointId).toInt() == id)
      return child;
  }
  return 0;
};

bool ModelDockWidget::hasChildVisible(QTreeWidgetItem* parent) const
{
  for (int i = 0 ; i < parent->childCount() ; ++i)
  {
    if (!parent->child(i)->isHidden())
      return true;
  }
  return false;
};

bool ModelDockWidget::isOkToModifyAnalog()
{
  QMessageBox messageBox(QMessageBox::Question, 
                         trUtf8("Mokka"),
                         trUtf8("The modification of this parameter might corrupt the data when saved in a file.\nDo you want to continue?"), 
                         QMessageBox::Yes | QMessageBox::No,
                         this, Qt::Sheet);
  //messageBox.setWindowModality(Qt::WindowModal);
  messageBox.setDefaultButton(QMessageBox::Yes);
  messageBox.setEscapeButton(QMessageBox::No);
  if (messageBox.exec() == QMessageBox::Yes)
    return true;
  return false;
};