/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QList>
#include <QTableWidgetSelectionRange>
#include <QModelIndexList>

// SlicerQt includes
#include "qSlicerOpenIGTLinkRemoteQueryWidget.h"
#include "ui_qSlicerOpenIGTLinkRemoteQueryWidget.h"

// Other includes
#include "vtkSlicerOpenIGTLinkIFLogic.h"

#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLIGTLQueryNode.h"
#include "vtkMRMLImageMetaListNode.h"
#include "vtkMRMLLabelMetaListNode.h"
#include "vtkMRMLPointMetaListNode.h"
#include "vtkMRMLAnnotationHierarchyNode.h"
#include "vtkMRMLAnnotationFiducialNode.h"


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_IGTLRemote
class qSlicerOpenIGTLinkRemoteQueryWidgetPrivate
  : public Ui_qSlicerOpenIGTLinkRemoteQueryWidget
{
  Q_DECLARE_PUBLIC(qSlicerOpenIGTLinkRemoteQueryWidget);

public:
  qSlicerOpenIGTLinkRemoteQueryWidgetPrivate(qSlicerOpenIGTLinkRemoteQueryWidget&);
  ~qSlicerOpenIGTLinkRemoteQueryWidgetPrivate();
  
public:
  QButtonGroup typeButtonGroup;
  
  void init();
  void updateResultBoxLabels(int id);
  void requestImage(std::string name);

  enum {
    TYPE_IMAGE,
    TYPE_LABEL,
    TYPE_POINT,
  };

  vtkMRMLIGTLConnectorNode * connectorNode;

  // TODO: we only have one query node.. it may cause some issue,
  // when there are multiple connectors using the single query Node.
  vtkMRMLIGTLQueryNode * queryNode;
  
protected:
  qSlicerOpenIGTLinkRemoteQueryWidget* const q_ptr;
};

//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkRemoteQueryWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::
  qSlicerOpenIGTLinkRemoteQueryWidgetPrivate(qSlicerOpenIGTLinkRemoteQueryWidget& object)
  : q_ptr(&object)
{
  this->connectorNode = NULL;
  this->queryNode = NULL;
}

qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::~qSlicerOpenIGTLinkRemoteQueryWidgetPrivate()
{
}


//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::init()
{
  Q_Q(qSlicerOpenIGTLinkRemoteQueryWidget);
  this->setupUi(q);

  this->typeButtonGroup.addButton(this->typeImageRadioButton, qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_IMAGE);
  this->typeButtonGroup.addButton(this->typeLabelRadioButton, qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_LABEL);
  this->typeButtonGroup.addButton(this->typePointRadioButton, qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_POINT);

  QObject::connect(this->connectorNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
                   q, SLOT(setConnectorNode(vtkMRMLNode*)));
  QObject::connect(this->updateButton, SIGNAL(clicked()),
                   q, SLOT(queryRemoteList()));
  QObject::connect(this->getSelectedItemButton, SIGNAL(clicked()),
                   q, SLOT(querySelectedItem()));
  QObject::connect(&typeButtonGroup, SIGNAL(buttonClicked(int)),
                   q, SLOT(onQueryTypeChanged(int)));
  QObject::connect(this->trackingSTTButton, SIGNAL(clicked()),
                   q, SLOT(startTracking()));
  QObject::connect(this->trackingSTPButton, SIGNAL(clicked()),
                   q, SLOT(stopTracking()));

  // set up table
  //this->remoteDataListTable->setRowCount(15);
  this->remoteDataListTable->setColumnCount(5);
  this->remoteDataListTable->verticalHeader()->hide();
  this->remoteDataListTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->remoteDataListTable->setSelectionMode(QAbstractItemView::MultiSelection);
  this->remoteDataListTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // set to default query type
  this->typeImageRadioButton->click();
}

//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkRemoteQueryWidget methods

//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkRemoteQueryWidget::qSlicerOpenIGTLinkRemoteQueryWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr( new qSlicerOpenIGTLinkRemoteQueryWidgetPrivate(*this) )
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);
  d->init();
  d->queryNode = vtkMRMLIGTLQueryNode::New();
  d->queryNode->SetNoNameQuery(1);
  qvtkConnect(d->queryNode, vtkMRMLIGTLQueryNode::ResponseEvent,
              this, SLOT(onQueryResponseReceived()));
}


//-----------------------------------------------------------------------------
qSlicerOpenIGTLinkRemoteQueryWidget::~qSlicerOpenIGTLinkRemoteQueryWidget()
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);
  if( d->queryNode != NULL )
  {
    qvtkDisconnect(d->queryNode, vtkMRMLIGTLQueryNode::ResponseEvent,
                this, SLOT(onQueryResponseReceived()));
    d->queryNode->Delete();
  }
}


//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::setMRMLScene(vtkMRMLScene *newScene)
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);

  d->connectorNodeSelector->setMRMLScene(newScene);

  if (newScene)
    newScene->AddNode(d->queryNode);

  this->Superclass::setMRMLScene(newScene);
}


void qSlicerOpenIGTLinkRemoteQueryWidget::setIFLogic(vtkSlicerOpenIGTLinkIFLogic *ifLogic)
{
  vtkNotUsed(ifLogic);
}


//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::setConnectorNode(vtkMRMLNode* node)
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);

  vtkMRMLIGTLConnectorNode* cnode =
    vtkMRMLIGTLConnectorNode::SafeDownCast(node);
  if (cnode)
    {
    d->connectorNode = cnode;
    }
}


//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::queryRemoteList()
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);

  if (!this->mrmlScene() || !d->connectorNode || !d->queryNode)
    return;

  if (d->typeButtonGroup.checkedId() == qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_IMAGE)
    {
    d->queryNode->SetIGTLName("IMGMETA");
    }
  else if (d->typeButtonGroup.checkedId() == qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_LABEL)
    {
    d->queryNode->SetIGTLName("LBMETA");
    }
  else if (d->typeButtonGroup.checkedId() == qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_POINT)
    {
    d->queryNode->SetIGTLName("POINT");
    }
  d->queryNode->SetQueryStatus(vtkMRMLIGTLQueryNode::STATUS_PREPARED);
  d->queryNode->SetQueryType(vtkMRMLIGTLQueryNode::TYPE_GET);
  d->connectorNode->PushQuery((vtkMRMLIGTLQueryNode*)d->queryNode);
}


//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::startTracking()
{
  std::cerr << "qSlicerOpenIGTLinkRemoteQueryWidget::startTracking()" << std::endl;
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);

  if (!this->mrmlScene() || !d->connectorNode || !d->queryNode)
    return;

  d->queryNode->SetIGTLName("TDATA");
  d->queryNode->SetNoNameQuery(1);
  d->queryNode->SetQueryStatus(vtkMRMLIGTLQueryNode::STATUS_PREPARED);
  d->queryNode->SetQueryType(vtkMRMLIGTLQueryNode::TYPE_START);
  d->connectorNode->PushQuery((vtkMRMLIGTLQueryNode*)d->queryNode);
}


//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::stopTracking()
{
  std::cerr << "qSlicerOpenIGTLinkRemoteQueryWidget::stopTracking()" << std::endl;
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);

  if (!this->mrmlScene() || !d->connectorNode || !d->queryNode)
    return;

  d->queryNode->SetIGTLName("TDATA");
  d->queryNode->SetNoNameQuery(1);
  d->queryNode->SetQueryStatus(vtkMRMLIGTLQueryNode::STATUS_PREPARED);
  d->queryNode->SetQueryType(vtkMRMLIGTLQueryNode::TYPE_STOP);
  d->connectorNode->PushQuery((vtkMRMLIGTLQueryNode*)d->queryNode);
}


//-----------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::querySelectedItem()
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);

  if (!this->mrmlScene()
      || !d->connectorNode)
    return;
    
    
  QList<QTableWidgetSelectionRange>
    selectRange(d->remoteDataListTable->selectedRanges());
  int i = 0, j = 0;
  while(i < selectRange.size())
    {
    j = selectRange.at(i).topRow();
    // Get the item identifier from the table
    std::string rowid( d->remoteDataListTable->item(j, 0)->text().toAscii() );
    switch (d->typeButtonGroup.checkedId()) 
      {
      case qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_IMAGE:
      case qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_LABEL:
        this->getImage( rowid );
        break;
      case qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_POINT:
        this->getPointList( rowid );
        break;
      }
      
    // iterate: within each range, then increment to the top of next range
    (j == selectRange.at(i).bottomRow()) ? i++ :
                                           j++;
    }
}

//------------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::onQueryResponseReceived()
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);

  vtkMRMLScene* scene = this->mrmlScene();
  if (!this->mrmlScene() || !d->queryNode)
    return;

  vtkMRMLNode* qNode = scene->GetNodeByID(d->queryNode->GetResponseDataNodeID());
  vtkMRMLImageMetaListNode* imgQueryNode; 
  vtkMRMLLabelMetaListNode* lbQueryNode; 
  vtkMRMLPointMetaListNode* ptQueryNode;

  if ( (imgQueryNode = vtkMRMLImageMetaListNode::SafeDownCast(qNode)) )
    {
    int numImages = imgQueryNode->GetNumberOfImageMetaElement();
    d->remoteDataListTable->setRowCount(numImages);

    for (int i = 0; i < numImages; i++)
      {
      vtkMRMLImageMetaListNode::ImageMetaElement element;

      imgQueryNode->GetImageMetaElement(i, &element);

      time_t timer = (time_t) element.TimeStamp;
      struct tm *tst = localtime(&timer);
      std::stringstream timess;
      timess << tst->tm_year+1900 << "-" << tst->tm_mon+1 << "-" << tst->tm_mday << " "
             << tst->tm_hour << ":" << tst->tm_min << ":" << tst->tm_sec;

      QTableWidgetItem *deviceItem = new QTableWidgetItem(element.DeviceName.c_str());
      QTableWidgetItem *idItem = new QTableWidgetItem(element.PatientID.c_str());
      QTableWidgetItem *nameItem = new QTableWidgetItem(element.PatientName.c_str());
      QTableWidgetItem *modalityItem = new QTableWidgetItem(element.Modality.c_str());
      QTableWidgetItem *timeItem = new QTableWidgetItem(timess.str().c_str());

      d->remoteDataListTable->setItem(i, 0, deviceItem);
      d->remoteDataListTable->setItem(i, 1, idItem);
      d->remoteDataListTable->setItem(i, 2, nameItem);
      d->remoteDataListTable->setItem(i, 3, modalityItem);
      d->remoteDataListTable->setItem(i, 4, timeItem);
      }
    }
  else if ( (lbQueryNode = vtkMRMLLabelMetaListNode::SafeDownCast(qNode)) )
    {
    int numLabels = lbQueryNode->GetNumberOfLabelMetaElement();
    d->remoteDataListTable->setRowCount(numLabels);

    for (int i = 0; i < numLabels; i++)
      {
      vtkMRMLLabelMetaListNode::LabelMetaElement element;

      lbQueryNode->GetLabelMetaElement(i, &element);

      QTableWidgetItem *deviceItem = new QTableWidgetItem(element.DeviceName.c_str());
      QTableWidgetItem *ownerItem = new QTableWidgetItem(element.Owner.c_str());

      d->remoteDataListTable->setItem(i, 0, deviceItem);
      d->remoteDataListTable->setItem(i, 1, ownerItem);
      }
    }
  else if( (ptQueryNode = vtkMRMLPointMetaListNode::SafeDownCast(qNode)) )
    {
    std::vector<std::string> ptGroupIds;
    ptQueryNode->GetPointGroupNames(ptGroupIds);
    d->remoteDataListTable->setRowCount(ptGroupIds.size());
    for (int i = 0; i < ptGroupIds.size(); i++)
      d->remoteDataListTable->setItem(i, 0, new QTableWidgetItem(ptGroupIds[i].c_str()));
    }
}


//------------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::getImage(std::string name)
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);
  vtkMRMLIGTLQueryNode* node = vtkMRMLIGTLQueryNode::New();
  node->SetIGTLName("IMAGE");
  node->SetQueryStatus(vtkMRMLIGTLQueryNode::STATUS_PREPARED);
  node->SetQueryType(vtkMRMLIGTLQueryNode::TYPE_GET);
  node->SetName(name.c_str());
  this->mrmlScene()->AddNode(node);
  qvtkConnect(node, vtkMRMLIGTLQueryNode::ResponseEvent,
              this, SLOT(onQueryResponseReceived()));
  d->connectorNode->PushQuery(node);
  node->Delete();
}


//------------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::getPointList(std::string id)
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);
  typedef vtkMRMLPointMetaListNode::PointMetaElement ElemType;
  vtkMRMLNode* qNode = this->mrmlScene()->GetNodeByID(d->queryNode->GetResponseDataNodeID());
  vtkMRMLPointMetaListNode* ptQueryNode = vtkMRMLPointMetaListNode::SafeDownCast(qNode);
  
  if(!ptQueryNode)
    {
    std::cerr << "qSlicerOpenIGTLinkRemoteQueryWidget: Query result for " << id
              << " is no longer available!" << std::endl;
    return;
    }
  std::vector<ElemType>::iterator elemIter;
  std::vector<ElemType> elements;
  ptQueryNode->GetPointGroup(id, elements);
  if (elements.size() == 0)
    return;
  
  // annotation parent setup
  vtkMRMLAnnotationHierarchyNode* parent;
    {
    parent = vtkMRMLAnnotationHierarchyNode::New();
    parent->HideFromEditorsOff();
    parent->SetName(this->mrmlScene()->GetUniqueNameByString(id.c_str()));
    this->mrmlScene()->AddNode(parent);
    }

  for (elemIter = elements.begin(); elemIter != elements.end(); elemIter++)
    {
    vtkMRMLAnnotationHierarchyNode* fiduNode = vtkMRMLAnnotationHierarchyNode::New();
      {
      fiduNode->SetParentNodeID(parent->GetID());
      fiduNode->AllowMultipleChildrenOff();
      }
    
    vtkMRMLAnnotationFiducialNode* fidu = vtkMRMLAnnotationFiducialNode::New();
      {
      fidu->SetDisableModifiedEvent(1);
      fidu->SetName( this->mrmlScene()->GetUniqueNameByString(elemIter->Name.c_str()) );
      float* pos = elemIter->Position;
      fidu->SetFiducialCoordinates((double)pos[0], (double)pos[1], (double)pos[2]);
      fidu->Initialize(this->mrmlScene());
      fidu->SetDisableModifiedEvent(0);
      fiduNode->SetDisplayableNodeID(fidu->GetID());
      }
    this->mrmlScene()->InsertAfterNode(fidu, fiduNode);
    fidu->Delete();
    fiduNode->Delete();
    }
  parent->Delete();
}


//------------------------------------------------------------------------------
void qSlicerOpenIGTLinkRemoteQueryWidget::onQueryTypeChanged(int id)
{
  Q_D(qSlicerOpenIGTLinkRemoteQueryWidget);
  d->remoteDataListTable->clearContents();
  QStringList list;
  switch(id) {
    case qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_IMAGE:
      list << QObject::tr("Image ID") << QObject::tr("Patient ID") 
           << QObject::tr("Patient Name") << QObject::tr("Modality")
           << QObject::tr("Time");
      break;
    case qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_LABEL:
      list << QObject::tr("Image ID") << QObject::tr("Owner Image")
           << QObject::tr("") << QObject::tr("")
           << QObject::tr("");
      break;
    case qSlicerOpenIGTLinkRemoteQueryWidgetPrivate::TYPE_POINT:
      list << QObject::tr("Group ID") << QObject::tr("") 
           << QObject::tr("") << QObject::tr("")
           << QObject::tr("");
      break;
  }
  d->remoteDataListTable->setHorizontalHeaderLabels(list);
}
