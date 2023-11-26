#pragma once


class CParticlesObject: public CCustomObject
{
    typedef CCustomObject inherited;

public:
					        CParticlesObject    (LPVOID data, LPCSTR name);
	virtual 		        ~CParticlesObject   ();
	void 			        Construct		    (LPVOID data);
    virtual bool	        CanAttach		    () {return true;}
  	virtual bool 	        LoadStream			(IReader&);
  	virtual bool 	        LoadLTX				(CInifile& ini, LPCSTR sect_name);
	void*                   QueryInterface      (ObjClassID InClassID) override;
    shared_str				m_RefName;
};