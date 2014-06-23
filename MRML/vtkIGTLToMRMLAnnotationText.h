#ifndef __vtkIGTLToMRMLAnnotationText_h
#define __vtkIGTLToMRMLAnnotationText_h

#include "vtkObject.h"
#include "vtkMRMLNode.h"
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkRemoteModuleMRMLExport.h"

#include "igtlStringMessage.h"



class VTK_SLICER_OPENIGTLINKREMOTE_MODULE_MRML_EXPORT vtkIGTLToMRMLAnnotationText : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLAnnotationText *New();
  vtkTypeMacro(vtkIGTLToMRMLAnnotationText,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual int          GetConverterType() { return TYPE_NORMAL; };
  virtual const char*  GetIGTLName() { return "STRING"; };
  virtual const char*  GetMRMLName() { return "AnnotationText"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  // for TYPE_MULTI_IGTL_NAMES
  int                  GetNumberOfIGTLNames()   { return this->IGTLNames.size(); };
  const char*          GetIGTLName(int index)   { return this->IGTLNames[index].c_str(); };

  //BTX
  virtual int          IGTLToMRML( igtl::MessageBase::Pointer buffer, vtkMRMLNode* node );
  //ETX
  virtual int          MRMLToIGTL( unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg );

 
 protected:
  vtkIGTLToMRMLAnnotationText();
  ~vtkIGTLToMRMLAnnotationText();

 protected:
  //BTX
  igtl::StringMessage::Pointer StringMsg;
  //ETX
  
};


#endif
