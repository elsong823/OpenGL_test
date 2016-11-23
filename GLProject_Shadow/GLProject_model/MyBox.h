#pragma once
#include "MyCube.h"

class CMyBox :
	public CMyCube
{
public:
	CMyBox();
	~CMyBox();

public:
	static void SetDirectLight(D_LIGHT _dirLight){ directLight = _dirLight; }
	static void SetSpotLight(S_LIGHT _spotLight){ spotLight = _spotLight; }
	void AddPointLight(PT_LIGHT _light);

private:
	void Init()override;
	void SubsetDraw()override;

private:
	static D_LIGHT directLight;
	static MATERIAL material;
	static S_LIGHT spotLight;
	vector<PT_LIGHT> m_ptLights;
};

