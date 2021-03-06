#pragma once
#include "stdafx.h"

class GeneralShader: public AlignedAllocationPolicy<16>
{
public:
	GeneralShader();
	GeneralShader(const GeneralShader &other);
	~GeneralShader();

	virtual bool Init(ID3D11Device *pDevice, HWND hwnd);
	virtual void Shutdown();
	bool Render(ID3D11DeviceContext *pDeviceContext, int indexCount,
		void *pVSUniformVariables, void *pPSUniformVariables,
		ID3D11ShaderResourceView *texture);

	virtual size_t GetVSUniformVariableSize();
	virtual size_t GetPSUniformVariableSize();
	
	static bool CreateVSUniformVariable(void *&ptr);
	static void SetVSUniformVariable(void *ptr, XMMATRIX world, XMMATRIX view, XMMATRIX projection);

protected:
	struct VSUniformVariableType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	ID3D11VertexShader			*m_pVertexShader;
	ID3D11PixelShader			*m_pPixelShader;
	ID3D11InputLayout			*m_pLayout;
	ID3D11Buffer				*m_pVSUniformVariableBuffer;
	ID3D11Buffer				*m_pPSUniformVariableBuffer;
	D3D11_INPUT_ELEMENT_DESC	*m_pPolygonLayout;

	LPCSTR m_vsFilename;
	LPCSTR m_psFilename;
	LPCSTR m_vsEntryPoint;
	LPCSTR m_psEntryPoint;

	virtual bool InitShader(ID3D11Device *pDevice, HWND hwnd);
	virtual unsigned int SetPolygonLayout() = 0;
	virtual void ShutdownShader();
	virtual void OutputShaderErrorMsg(ID3D10Blob *errorMsg, HWND hwnd, LPCSTR shaderFilename);
	virtual void RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount);

	bool SetShaderParams(ID3D11DeviceContext *pDeviceContext,
		void *pVSUniformVariables, void *pPSUniformVariables,
		ID3D11ShaderResourceView *texture);
};

