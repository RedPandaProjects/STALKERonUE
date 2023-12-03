#pragma once

class ESceneGroupTool: public FXRaySceneCustomOTool
{
	typedef FXRaySceneCustomOTool inherited;
 public:
				                ESceneGroupTool		    ():FXRaySceneCustomOTool(ERBMKSceneObjectType::Group){;}
	// definition
    IC LPCSTR			        ClassName			    (){return "group";}
    IC LPCSTR			        ClassDesc			    (){return "Group";}

    virtual void		        Clear				    (){inherited::Clear();}
    // IO
    virtual int			        SaveFileCount		    () const {return 2;}
    virtual bool   		        LoadStream              (IReader&);
    virtual bool   		        LoadLTX            	    (CInifile&);
    virtual bool		        LoadSelection      	    (IReader&);

    virtual FXRayCustomObject*  CreateObject			(LPVOID data, LPCSTR name);
private:
    xr_string			        m_CurrentObject;
    xr_vector<bool>		        m_stored_state;
};