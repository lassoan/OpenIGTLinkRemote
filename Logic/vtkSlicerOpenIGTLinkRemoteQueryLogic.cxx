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

// IGTLRemote includes
#include "vtkSlicerOpenIGTLinkRemoteQueryLogic.h"

// MRML includes

// VTK includes
#include <vtkNew.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------

class vtkSlicerOpenIGTLinkRemoteQueryLogic::vtkInternal
{
public:
  vtkInternal();

  vtkSlicerOpenIGTLinkIFLogic* IFLogic;
};

vtkSlicerOpenIGTLinkRemoteQueryLogic::vtkInternal::vtkInternal()
: IFLogic(NULL)
{
}

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerOpenIGTLinkRemoteQueryLogic);

//----------------------------------------------------------------------------
vtkSlicerOpenIGTLinkRemoteQueryLogic::vtkSlicerOpenIGTLinkRemoteQueryLogic()
{
  this->Internal = new vtkInternal;
}

//----------------------------------------------------------------------------
vtkSlicerOpenIGTLinkRemoteQueryLogic::~vtkSlicerOpenIGTLinkRemoteQueryLogic()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteQueryLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteQueryLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteQueryLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteQueryLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteQueryLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteQueryLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

void vtkSlicerOpenIGTLinkRemoteQueryLogic
::SetIFLogic(vtkSlicerOpenIGTLinkIFLogic *ifLogic)
{
  this->Internal->IFLogic = ifLogic;
}
