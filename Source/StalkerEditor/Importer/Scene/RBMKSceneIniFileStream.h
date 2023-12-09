#pragma once
struct FRBMKSceneIniFileStream :public IIniFileStream
{
	void	  	move_begin		() override {Counter=0;}
	void	  	w_float			( float a) override;
	void 	  	w_vec3			( const Fvector& a) override;
	void 	  	w_vec4			( const Fvector4& a) override;
	void 	  	w_u64			( u64 a) override;				
	void 	  	w_s64			( s64 a) override;				
	void 	  	w_u32			( u32 a) override;				
	void 	  	w_s32			( s32 a) override;				
	void 	  	w_u16			( u16 a) override;				
	void 		w_s16			( s16 a) override;				
	void		w_u8			( u8 a) override;				
	void		w_s8			( s8 a) override;
	void		w_stringZ		( LPCSTR S) override;
	
	void		r_vec3			(Fvector&) override;			
	void		r_vec4			(Fvector4&) override;
	void		r_float			(float&) override;
	void		r_u8			(u8&)  override;					
	void		r_u16			(u16&) override;					
	void		r_u32			(u32&) override;					
	void		r_u64			(u64&) override;					
	void		r_s8			(s8&)  override;					
	void		r_s16			(s16&) override;					
	void		r_s32			(s32&) override;					
	void		r_s64			(s64&) override;					

	void		r_string		(char* Dest, uint32 DestSize)	override;
	void		r_string		(xr_string&name);
	void		skip_stringZ	() override;
	const char*	gen_name	();

	CInifile*	INIFile;
	shared_str	SectionName;
	string128	BufferStr;
	uint32		Counter;
};
