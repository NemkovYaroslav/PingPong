#include "RenderComponent.h"

RenderComponent::RenderComponent()
{
	rastDesc = std::make_shared<CD3D11_RASTERIZER_DESC>();
	vertexBufDesc = std::make_shared<D3D11_BUFFER_DESC>();
	vertexData = std::make_shared<D3D11_SUBRESOURCE_DATA>();
	indexBufDesc = std::make_shared<D3D11_BUFFER_DESC>();
	indexData = std::make_shared<D3D11_SUBRESOURCE_DATA>();
	constBufDesc = std::make_shared<D3D11_BUFFER_DESC>();
	renderOffset = std::make_shared<DirectX::SimpleMath::Vector4>();
}

RenderComponent::RenderComponent(std::shared_ptr<DirectX::SimpleMath::Vector4> position)
{
	rastDesc = std::make_shared<CD3D11_RASTERIZER_DESC>();
	vertexBufDesc = std::make_shared<D3D11_BUFFER_DESC>();
	vertexData = std::make_shared<D3D11_SUBRESOURCE_DATA>();
	indexBufDesc = std::make_shared<D3D11_BUFFER_DESC>();
	indexData = std::make_shared<D3D11_SUBRESOURCE_DATA>();
	constBufDesc = std::make_shared<D3D11_BUFFER_DESC>();
	renderOffset = std::make_shared<DirectX::SimpleMath::Vector4>();
	this->renderOffset = position;
}

void RenderComponent::Initialize()
{
	ID3DBlob* errorPixelCode = nullptr;
	ID3DBlob* errorVertexCode = nullptr;

	Game::GetInstance()->res = D3DCompileFromFile(
		L"./Shaders/MyVeryFirstShader.hlsl",
		nullptr /*macros*/,
		nullptr /*include*/,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		pixelShaderByteCode.GetAddressOf(),
		&errorPixelCode
	);

	Game::GetInstance()->res = D3DCompileFromFile(
		L"./Shaders/MyVeryFirstShader.hlsl",
		nullptr /*macros*/,
		nullptr /*include*/,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		vertexShaderByteCode.GetAddressOf(),
		&errorVertexCode
	);

	if (FAILED(Game::GetInstance()->res))
	{
		if (errorVertexCode)
		{
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else
		{
			MessageBox(Game::GetInstance()->GetDisplay()->GetHWnd(), L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}
		return;
	}

	Game::GetInstance()->GetDevice()->CreatePixelShader(
		pixelShaderByteCode->GetBufferPointer(),
		pixelShaderByteCode->GetBufferSize(),
		nullptr,
		pixelShader.GetAddressOf()
	);

	Game::GetInstance()->GetDevice()->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		nullptr,
		vertexShader.GetAddressOf()
	);

	D3D11_INPUT_ELEMENT_DESC inputElements[] = 
	{
		D3D11_INPUT_ELEMENT_DESC 
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		D3D11_INPUT_ELEMENT_DESC 
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
	};

	Game::GetInstance()->GetDevice()->CreateInputLayout(
		inputElements,
		2,
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		layout.GetAddressOf()
	);

	vertexBufDesc->ByteWidth = sizeof(DirectX::XMFLOAT4) * points.size();
	vertexBufDesc->Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc->CPUAccessFlags = 0;
	vertexBufDesc->MiscFlags = 0;
	vertexBufDesc->StructureByteStride = 0;

	vertexData->pSysMem = points.data();
	vertexData->SysMemPitch = 0;
	vertexData->SysMemSlicePitch = 0;

	Game::GetInstance()->GetDevice()->CreateBuffer(vertexBufDesc.get(), vertexData.get(), vertexBuf.GetAddressOf());

	indexBufDesc->ByteWidth = sizeof(int) * indeces.size();
	indexBufDesc->Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc->BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc->CPUAccessFlags = 0;
	indexBufDesc->MiscFlags = 0;
	indexBufDesc->StructureByteStride = 0;

	indexData->pSysMem = indeces.data();
	indexData->SysMemPitch = 0;
	indexData->SysMemSlicePitch = 0;

	Game::GetInstance()->GetDevice()->CreateBuffer(indexBufDesc.get(), indexData.get(), indexBuf.GetAddressOf());

//MOVEMENT
	constBufDesc->ByteWidth = sizeof(DirectX::XMMATRIX);
	constBufDesc->Usage = D3D11_USAGE_DEFAULT;
	constBufDesc->BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc->CPUAccessFlags = 0;
	constBufDesc->MiscFlags = 0;
	constBufDesc->StructureByteStride = 0;
	Game::GetInstance()->GetDevice()->CreateBuffer(constBufDesc.get(), nullptr, constBuf.GetAddressOf());
//MOVEMENT

	rastDesc->CullMode = D3D11_CULL_NONE;
	rastDesc->FillMode = D3D11_FILL_SOLID;

	Game::GetInstance()->res = Game::GetInstance()->GetDevice()->CreateRasterizerState(rastDesc.get(), rastState.GetAddressOf());
	Game::GetInstance()->GetContext()->RSSetState(rastState.Get());

	strides[0] = 32;
	offsets[0] = 0;
}

void RenderComponent::Draw()
{
//MOVEMENT
	transform = DirectX::XMMatrixTranspose
	(
		DirectX::XMMatrixMultiply
		(
			DirectX::XMMatrixScaling((float)Game::GetInstance()->GetHeight()/(float)Game::GetInstance()->GetWidth(), 1.0f, 1.0f),
			DirectX::XMMatrixTranslation(renderOffset.get()->x, renderOffset.get()->y, renderOffset.get()->z)
		)
	);
//MOVEMENT

	Game::GetInstance()->GetContext()->IASetInputLayout(layout.Get());
	Game::GetInstance()->GetContext()->IASetPrimitiveTopology(topology);
	Game::GetInstance()->GetContext()->IASetIndexBuffer(indexBuf.Get(), DXGI_FORMAT_R32_UINT, 0);
	Game::GetInstance()->GetContext()->IASetVertexBuffers(0, 1, vertexBuf.GetAddressOf(), strides, offsets);
	Game::GetInstance()->GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	Game::GetInstance()->GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
	Game::GetInstance()->GetContext()->RSSetState(rastState.Get());

//MOVEMENT
	Game::GetInstance()->GetContext()->UpdateSubresource(constBuf.Get(), 0, nullptr, &transform, 0, 0);
	Game::GetInstance()->GetContext()->VSSetConstantBuffers(0, 1, constBuf.GetAddressOf());
//MOVEMENT

	Game::GetInstance()->GetContext()->DrawIndexed(indeces.size(), 0, 0);
}