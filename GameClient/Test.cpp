
// 그래픽 파이프라인 문서
//https://learn.microsoft.com/ko-kr/windows/uwp/graphics-concepts/graphics-pipeline

	/*
	AMesh* p = new AMesh;
	Ptr<AMesh> pMesh = p;
	p = pMesh.Get();
	p = new AMesh;
	//p->AddRef();
	Ptr<AMesh> pMesh2 = nullptr;
	*pMesh2.GetAdressOf() = p;

	pMesh == pMesh2;
	pMesh == p;
	p == pMesh;
	*/

	// VertexShader
	// 컴파일할 VertexShader 함수가 작성 되어있는 파일의 절대 경로

	// 파이프라인 생성