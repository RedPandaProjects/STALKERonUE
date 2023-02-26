#pragma once

class ESceneGroupTool: public ESceneCustomOTool
{
	typedef ESceneCustomOTool inherited;
 public:
				                ESceneGroupTool		    ():ESceneCustomOTool(OBJCLASS_GROUP){;}
	// definition
    IC LPCSTR			        ClassName			    (){return "group";}
    IC LPCSTR			        ClassDesc			    (){return "Group";}

    virtual void		        Clear				    (){inherited::Clear();}
    // IO
    virtual int			        SaveFileCount		    () const {return 2;}
    virtual bool   		        LoadStream              (IReader&);
    virtual bool   		        LoadLTX            	    (CInifile&);
    virtual bool		        LoadSelection      	    (IReader&);

    virtual CCustomObject*      CreateObject			(LPVOID data, LPCSTR name);
private:
    xr_string			        m_CurrentObject;
    xr_vector<bool>		        m_stored_state;
};