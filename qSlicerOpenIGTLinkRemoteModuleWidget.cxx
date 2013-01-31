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
#include <QDebug>

// SlicerQt includes
#include "qSlicerOpenIGTLinkRemoteModuleWidget.h"
#include "ui_qSlicerOpenIGTLinkRemoteModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerOpenIGTLinkRemoteModuleWidgetPrivate: public Ui_qSlicerOpenIGTLinkRemoteModuleWidget
{
public:
  qSlicerOpenIGTLinkRemoteModuleWidgetPrivate();
};



//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkRemoteModuleWidgetPrivate methods



qSlicerOpenIGTLinkRemoteModuleWidgetPrivate::qSlicerOpenIGTLinkRemoteModuleWidgetPrivate()
{
}



//-----------------------------------------------------------------------------
// qSlicerOpenIGTLinkRemoteModuleWidget methods



qSlicerOpenIGTLinkRemoteModuleWidget::qSlicerOpenIGTLinkRemoteModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerOpenIGTLinkRemoteModuleWidgetPrivate )
{
}



qSlicerOpenIGTLinkRemoteModuleWidget::~qSlicerOpenIGTLinkRemoteModuleWidget()
{
}



void qSlicerOpenIGTLinkRemoteModuleWidget::setup()
{
  Q_D(qSlicerOpenIGTLinkRemoteModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
  
  connect( d->SendCommandButton, SIGNAL( clicked() ), this, SLOT( OnSendCommandClicked() ) );
}


void qSlicerOpenIGTLinkRemoteModuleWidget
::OnSendCommandClicked()
{
  Q_D(qSlicerOpenIGTLinkRemoteModuleWidget);
  
  vtkMRMLNode* node = d->ConnectorComboBox->currentNode();
  if ( node == NULL )
  {
    d->ReplyTextEdit->setPlainText( "Connector node not selected!" );
    return;
  }
  
  d->CommandTextEdit->setPlainText( "" );
  d->ReplyTextEdit->setPlainText( "" );
  
  // TODO: Call logic here.
 
}