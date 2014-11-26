#include "CVS_FbxImporter.h"
#include "CVS_Mesh.h"
#include "CVS_Bone.h"
#include "CVS_Skeleton.h"


bool CVS_FbxImporter::extractResource(FbxScene* _pFbxScene)
{
	// No constructor nor copy constructor, virtually a singleton 
	FbxGlobalSettings& gSettings = _pFbxScene->GetGlobalSettings();

	std::vector<CVS_Mesh*> meshes;

	auto pRootNode = _pFbxScene->GetRootNode();
	// import skeleton
	ImportAllNodes(pRootNode, EFbxImportMode::Skeleton);
	// import mesh
	ImportAllNodes(pRootNode, EFbxImportMode::Mesh);
	// import animation
	ImportAllNodes(pRootNode, EFbxImportMode::Animation);

	/*
	//GETTING ANIMAION DATA
	char pTimeString[256];

	auto timeMode = gSettings.GetTimeMode();
	printf("Time Mode : %s\n", FbxGetTimeModeName(timeMode));
	double fps = FrameRateToDouble(timeMode);
	FbxTimeSpan pTimeSpan;
	gSettings.GetTimelineDefaultTimeSpan(pTimeSpan);
	FbxTime     lStart, lEnd;
	lStart = pTimeSpan.GetStart();
	lEnd = pTimeSpan.GetStop();
	printf("Timeline default timespan: \n");
	printf("     Start: %s\n", lStart.GetTimeString(pTimeString, FbxUShort(256)));
	printf("     Stop : %s\n", lEnd.GetTimeString(pTimeString, FbxUShort(256)));

	// Get animation information
	// Now only supports one take
	for (int i = 0; i < _pFbxScene->GetSrcObjectCount<FbxAnimStack>(); ++i)
	{
	auto lAnimStack = _pFbxScene->GetSrcObject<FbxAnimStack>(i);

	auto stackName = lAnimStack->GetName();
	printf("Animation Stack %d Name: %s\n", i, stackName);
	auto start = lAnimStack->GetLocalTimeSpan().GetStart();
	auto stop = lAnimStack->GetLocalTimeSpan().GetStop();
	auto animationLength = stop.GetFrameCount(timeMode) - start.GetFrameCount(timeMode);// TODO: do we really need +1?;
	printf("     Start: %d\n", start.GetFrameCount(timeMode));
	printf("     Stop : %d\n", stop.GetFrameCount(timeMode));
	// For Each layer
	int numLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();
	for (int j = 0; j < numLayers; ++j)
	{
	FbxAnimLayer* pAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(j);

	auto layerName = pAnimLayer->GetName();
	printf("Animation Stack Name: %s\n", layerName);

	// Start with root node
	FbxNode* pNodeItr = _pFbxScene->GetRootNode();
	// Breadth first inline recursion
	std::queue<FbxNode*> nodes;
	while (pNodeItr != NULL)
	{
	FbxAnimCurve* lAnimCurve = pNodeItr->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	// For each node, extract all channels
	static const char channels[] = { FBXSDK_CURVENODE_TRANSLATION, FBXSDK_CURVENODE_ROTATION, FBXSDK_CURVENODE_SCALING };
	static const char components[] = { FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z };
	for (int i = 0; i < 3; i++)
	{
	// Extract Translation Curve
	auto lAnimCurve = pNodeItr->LclTranslation.GetCurve(pAnimLayer, components[i]);
	if (lAnimCurve)
	{
	printf("        %s%s\n", channels[0], components[i]);
	//ProcessCurve(lAnimCurve);
	}

	// Extract Rotation Curve
	lAnimCurve = pNodeItr->LclRotation.GetCurve(pAnimLayer, components[i]);
	if (lAnimCurve)
	{
	printf("        %s%s\n", channels[1], components[i]);
	//ProcessCurve(lAnimCurve);
	}

	// Extract Scaling Curve
	lAnimCurve = pNodeItr->LclScaling.GetCurve(pAnimLayer, components[i]);
	if (lAnimCurve)
	{
	printf("        %s%s\n", channels[2], components[i]);
	//ProcessCurve(lAnimCurve);
	}
	}

	for (int i = 0; i < pNodeItr->GetChildCount(false); ++i)
	{
	nodes.push(pNodeItr->GetChild(i));
	}

	if (nodes.size() > 0)
	{
	pNodeItr = nodes.front();
	nodes.pop();
	}
	else
	{
	pNodeItr = NULL;
	}
	}
	}
	}
	*/
	return true;
}

void CVS_FbxImporter::ImportAllNodes(FbxNode* _pNode, EFbxImportMode _parseMode)
{
	// Skip RootNode by default
	// If we don't skip, RootNode will be parsed as a bone, NG
	for (int i = 0, e = _pNode->GetChildCount(); i < e; i++)
	{
		_processFbxNodeRecursive(_pNode->GetChild(i), _parseMode);
	}
	// Finally generate our Skeleton asset
	if (_parseMode == EFbxImportMode::Skeleton)
	{
		for (auto i : m_skeletonRootBones)
		{
			CVS_Skeleton* pSkeleton = new CVS_Skeleton;
			CVS_Bone* pBone = static_cast<CVS_Bone*>(i.second);
			pSkeleton->m_pRootBone = pBone;
			pSkeleton->m_name = pBone->m_name;
			pSkeleton->Init();
			m_pResourcePool->push_back(pSkeleton);
		}
	}
}

bool CVS_FbxImporter::_processFbxNodeRecursive(FbxNode* _pNode, EFbxImportMode _parseMode)
{
	FbxNodeAttribute* pNodeAttribute = _pNode->GetNodeAttribute();

	FbxMatrix globalTransform = _pNode->GetAnimationEvaluator()->GetNodeGlobalTransform(_pNode);
	glm::dvec4 c0 = glm::make_vec4((double*)globalTransform.GetColumn(0).Buffer());
	glm::dvec4 c1 = glm::make_vec4((double*)globalTransform.GetColumn(1).Buffer());
	glm::dvec4 c2 = glm::make_vec4((double*)globalTransform.GetColumn(2).Buffer());
	glm::dvec4 c3 = glm::make_vec4((double*)globalTransform.GetColumn(3).Buffer());
	cmat4 matGlobal = glm::inverse(cmat4(c0, c1, c2, c3));

	// Fill all attributes here
	double* pDouble = _pNode->PreRotation.Get().mData;
	auto PreRotation = glm::radians(cvec3(pDouble[0], pDouble[1], pDouble[2]));
	// Note: y and z are flipped here because we want Y-up translation
	pDouble = _pNode->LclTranslation.Get().mData;
	auto LclTranslation = cvec3(pDouble[0], pDouble[1], pDouble[2]);
	pDouble = _pNode->LclRotation.Get().mData;
	auto LclRotation = glm::radians(cvec3(pDouble[0], pDouble[1], pDouble[2]));
	pDouble = _pNode->LclScaling.Get().mData;
	auto LclScaling = cvec3(pDouble[0], pDouble[1], pDouble[2]);

	auto matTranslation = glm::translate(cmat4(), LclTranslation);
	auto matScaling = glm::scale(cmat4(), LclScaling);
	auto matRotation = glm::mat4_cast(cquat(LclRotation)) * glm::mat4_cast(cquat(PreRotation));

	pDouble = _pNode->GetGeometricTranslation(FbxNode::eSourcePivot).mData;
	//this->m_GOTranslation = cvec3(pDouble[0], pDouble[1], pDouble[2]);

	pDouble = _pNode->GetGeometricRotation(FbxNode::eSourcePivot).mData;
	//this->m_GORotation = cvec3(pDouble[0], pDouble[1], pDouble[2]);

	pDouble = _pNode->GetGeometricScaling(FbxNode::eSourcePivot).mData;
	//this->m_GOScaling = cvec3(pDouble[0], pDouble[1], pDouble[2]);

	switch (_parseMode)
	{
	case EFbxImportMode::Skeleton:
	{
									 // Again, we defer skeleton asset generation because we need all the bones to do so
									 CVS_Bone* pBone = nullptr;
									 if (pNodeAttribute)
									 {
										 // Blender specifies that Null and Skeleton are both parsed as bones
										 if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNull ||
											 pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
										 {
											 pBone = new CVS_Bone;
										 }
									 }
									 // An empty node, however we treat it as a bone too
									 else
									 {
										 pBone = new CVS_Bone;
									 }
									 // Init bone only if it is created
									 if (pBone)
									 {
										 if (pBone->initFromFbxNode(_pNode))
										 {
											 pBone->m_translation = LclTranslation;
											 pBone->m_rotation = glm::normalize(cquat(LclRotation) * cquat(PreRotation));
											 pDouble = _pNode->GetGeometricRotation(FbxNode::eSourcePivot).mData;
											 pBone->m_GORotation = cvec3(pDouble[0], pDouble[1], pDouble[2]);
											 pBone->m_scaling = LclScaling;
											 // Add bone to all bone list
											 m_allBones[_pNode] = pBone;
											 // Resolving the bone tree relies on correctly traversing the scene nodes
											 // Try to find its parent
											 auto pParentFbxNode = _pNode->GetParent();
											 if (pParentFbxNode)
											 {
												 CVS_Bone* pParentBone = static_cast<CVS_Bone*>(m_allBones[pParentFbxNode]);
												 // Find the respective CVS_Bone of parentNode
												 // Must check because it could be the root node which we don't want
												 if (pParentBone)
												 {
													 // Create Link between parent and our new bone
													 pParentBone->m_children.push_back(pBone);
													 pBone->m_pParent = pParentBone;
													 // Remove it from temp container because we only want root bones in this temp container
													 //m_boneMap.erase(_pNode);
												 }
												 else
												 {
													 // New root bone
													 m_skeletonRootBones[_pNode] = pBone;
												 }
											 }
										 }
									 }
	}
		break;
	case EFbxImportMode::Mesh:
	{
								 if (pNodeAttribute)
								 {
									 // Blender specifies that Null and Skeleton are both parsed as bones
									 if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
									 {
										 auto pMesh = new CVS_Mesh;
										 if (pMesh->initFromFbxNode(_pNode))
										 {
#pragma region
											 // Store all offset matrices
											 pMesh->m_offset *= matTranslation * matRotation * matScaling * matGlobal;
											 auto pFbxMesh = _pNode->GetMesh();
											 int skinCount = pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
											 if (skinCount)
											 {
												 for (int skinIndex = 0; skinIndex != skinCount; ++skinIndex)
												 {
													 auto pSkin = (FbxSkin *)pFbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin);
													 if (skinCount > 1)
													 {
														 printf("CVS_Mesh::initFromFbxNode: Mesh contains more than 1 skin.\n");
														 assert(0);
													 }

													 // Allocate memory for vertex bone data
													 pMesh->m_vertexBones.reserve(pMesh->m_vertices.size());
													 // for each skin, find the correct skeleton
													 // TODO: might be buggy when there's nore than 1 skeleton per mesh
													 CVS_Skeleton* pSkeleton = nullptr;
													 int clusterCount = (pSkin)->GetClusterCount();
													 for (int clusterIndex = 0; clusterIndex != clusterCount; ++clusterIndex)
													 {
														 auto pCluster = ((FbxSkin *)pFbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin))->GetCluster(clusterIndex);

														 _PrintClusterInfo(pCluster, clusterIndex);

														 // For each cluster, we check against the bone map to make sure
														 // everything comes from the same skeleton tree
														 auto pSkeletonNode = pCluster->GetLink();

														 CVS_Bone* pBone = nullptr;
														 pBone = static_cast<CVS_Bone*>(m_allBones[pSkeletonNode]);
														 if (!pBone)
														 {
															 printf("CVS_Mesh::initFromFbxNode: Current deformer's bone link not found in bonemap\n");
															 assert(0);
														 }

														 // Check if the parent is the same skeleton we currently have
														 // If no skeleton currently in place, use it directly
														 auto pParent = pBone->getSkeleton();
														 if (pSkeleton)
														 {
															 if (pSkeleton != pParent)
															 {
																 printf("CVS_Mesh::initFromFbxNode: Current mesh seems to have 2 rigs affecting it as the same time. Check your mesh.\n");
																 assert(0);
															 }
														 }
														 else
														 {
															 pSkeleton = pParent;
														 }

														 // Inject data into vertices
														 int* lIndices = pCluster->GetControlPointIndices();
														 double* lWeights = pCluster->GetControlPointWeights();

														 for (int j = 0, e = pCluster->GetControlPointIndicesCount(); j < e; j++)
														 {
															 int i = lIndices[j];
															 float f = lWeights[j];
															 pMesh->m_vertexBones[i].AddBoneData(pBone->m_index, f);
														 }

														 FbxAMatrix transformMatrix;
														 FbxAMatrix transformLinkMatrix;
														 FbxAMatrix globalBindposeInverseMatrix;

														 pCluster->GetTransformMatrix(transformMatrix); // The transformation of the mesh at binding time
														 pCluster->GetTransformLinkMatrix(transformLinkMatrix); // The transformation of the cluster(joint) at binding time from joint space to world space
														 globalBindposeInverseMatrix = transformLinkMatrix.Inverse();

														 auto pMat = cmat4();
														 auto pMatLink = cmat4();
														 auto pBindPoseInverse = cmat4();
														 FbxAMatrixToMat4(&transformMatrix, pMat);
														 FbxAMatrixToMat4(&transformLinkMatrix, pMatLink);
														 FbxAMatrixToMat4(&globalBindposeInverseMatrix, pBindPoseInverse);

														 pBone->m_offset *= pBindPoseInverse * pMat;
													 }
													 if (pMesh->m_pSkeleton)
													 {
														 printf("CVS_Mesh::initFromFbxNode: Current mesh seems to have 2 skins.\n");
														 assert(0);
													 }
													 else
													 {
														 pMesh->m_pSkeleton = pSkeleton;
													 }
												 }

											 }
											 else
											 {
												 // TODO: fix shader support for static mesh

												 // If there's no bone at all, we make a dummy
												 // to pass to shaders
												 /*
												 if (!this->m_pRootBone)
												 {
												 auto pDummyBone = new CVS_Bone;
												 pDummyBone->m_name = "DummyBone";
												 pDummyBone->m_color = cvec3(1.0f, 1.0f, 1.0f);
												 pDummyBone->m_index = 0;
												 this->m_pRootBone = pDummyBone;
												 // 			for (int i = 0, e = m_vertexBones.size(); i < e; i++)
												 // 			{
												 // 				m_vertexBones[i].AddBoneData(0, 1.0f);
												 // 			}
												 }
												 */
											 }
#pragma endregion Add Bones
											 pMesh->Init();
											 printf("Init mesh %s\n", pMesh->m_name.c_str());
											 m_pResourcePool->push_back(pMesh);
										 }
										 break;
									 }
								 }

	}
		break;
	case EFbxImportMode::Animation:
	{

	}
		break;
	default:
		assert(0);
		break;
	}

	// Process children
	for (int i = 0, e = _pNode->GetChildCount(); i < e; ++i)
	{
		_processFbxNodeRecursive(_pNode->GetChild(i), _parseMode);
	}

	return true;
}

void CVS_FbxImporter::_PrintClusterInfo(FbxCluster* _pCluster, int _index)
{
	static const char* lClusterModes[] = { "Normalize", "Additive", "Total1" };

	printf("    Cluster %d\n", _index);

	printf("    Mode: %s\n", lClusterModes[_pCluster->GetLinkMode()]);

	if (_pCluster->GetLink() != NULL)
	{
		printf("        Name: %s\n", (char *)_pCluster->GetLink()->GetName());
	}

	FbxString lString1 = "        Link Indices: ";
	FbxString lString2 = "        Weight Values: ";

	int k, lIndexCount = _pCluster->GetControlPointIndicesCount();
	int* lIndices = _pCluster->GetControlPointIndices();
	double* lWeights = _pCluster->GetControlPointWeights();

	for (k = 0; k < lIndexCount; k++)
	{
		lString1 += lIndices[k];
		lString2 += (float)lWeights[k];

		if (k < lIndexCount - 1)
		{
			lString1 += ", ";
			lString2 += ", ";
		}
	}

	lString1 += "\n";
	lString2 += "\n";

	FBXSDK_printf(lString1);
	//FBXSDK_printf(lString2);

	printf("\n");

	FbxAMatrix lMatrix;

	lMatrix = _pCluster->GetTransformMatrix(lMatrix);
	printf("        Transform Translation     : %f, %f, %f\n", lMatrix.GetT()[0], lMatrix.GetT()[1], lMatrix.GetT()[2]);
	printf("        Transform Rotation        : %f, %f, %f\n", lMatrix.GetR()[0], lMatrix.GetR()[1], lMatrix.GetR()[2]);
	printf("        Transform Scaling         : %f, %f, %f\n", lMatrix.GetS()[0], lMatrix.GetS()[1], lMatrix.GetS()[2]);

	lMatrix = _pCluster->GetTransformLinkMatrix(lMatrix);
	printf("        Transform Link Translation: %f, %f, %f\n", lMatrix.GetT()[0], lMatrix.GetT()[1], lMatrix.GetT()[2]);
	printf("        Transform Link Rotation   : %f, %f, %f\n", lMatrix.GetR()[0], lMatrix.GetR()[1], lMatrix.GetR()[2]);
	printf("        Transform Link Scaling    : %f, %f, %f\n", lMatrix.GetS()[0], lMatrix.GetS()[1], lMatrix.GetS()[2]);

	if (_pCluster->GetAssociateModel() != NULL)
	{
		lMatrix = _pCluster->GetTransformAssociateModelMatrix(lMatrix);
		printf("        Associate Model            : %s\n", (char *)_pCluster->GetAssociateModel()->GetName());
		printf("        Associate Model Translation: %f, %f, %f\n", lMatrix.GetT()[0], lMatrix.GetT()[1], lMatrix.GetT()[2]);
		printf("        Associate Model Rotation   : %f, %f, %f\n", lMatrix.GetR()[0], lMatrix.GetR()[1], lMatrix.GetR()[2]);
		printf("        Associate Model Scaling    : %f, %f, %f\n", lMatrix.GetS()[0], lMatrix.GetS()[1], lMatrix.GetS()[2]);
	}

	printf("\n");
}

FbxScene* CVS_FbxImporter::_initFBXScene(const char* _filepath)
{
	// Create the importer.
	int fileFormat = -1;
	if (!m_pFbxManager->GetIOPluginRegistry()->DetectReaderFileFormat(_filepath, fileFormat))
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		fileFormat = m_pFbxManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
	}

	if (!m_pImporter->Initialize(_filepath, -1, m_pFbxManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", m_pImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	auto pScene = FbxScene::Create(m_pFbxManager, "My scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}

	if (m_pImporter->Import(pScene) == true)
	{
		auto pGlobalSettings = &pScene->GetGlobalSettings();

		// Convert Axis System to what is used in this example, if needed
		FbxAxisSystem SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
		if (SceneAxisSystem != OurAxisSystem)
		{
			OurAxisSystem.ConvertScene(pScene);
		}

		// Requires the complete fbxsdk library
		// Convert Unit System to what is used in this example, if needed
		FbxSystemUnit SceneSystemUnit = pScene->GetGlobalSettings().GetSystemUnit();
		if (SceneSystemUnit.GetScaleFactor() != 1.0)
		{
			//The unit in this example is centimeter.
			FbxSystemUnit::cm.ConvertScene(pScene);
		}

		printf("_loadFBXScene: Animation Time Mode : %s\n", FbxGetTimeModeName(pGlobalSettings->GetTimeMode()));

		// Animation Stack
		for (int i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
		{
			FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);
			int numAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();

			printf("Animation Stack Name: %s\n\n", pAnimStack->GetName());
			printf("Animation stack contains %d Animation Layer(s)\n", numAnimLayers);

			// Animation Layer
			for (int j = 0; j < numAnimLayers; j++)
			{
				FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(j);
				printf("AnimLayer %d\n", j);
				printf("     Node Name: %s\n\n", pScene->GetRootNode()->GetName());
			}
		}


		FbxArray<FbxString*> mAnimStackNameArray;
		// Get the list of all the animation stack.
		pScene->FillAnimStackNameArray(mAnimStackNameArray);

		// Convert mesh, NURBS and patch into triangle mesh
		FbxGeometryConverter lGeomConverter(m_pFbxManager);
		lGeomConverter.Triangulate(pScene, true);

		// Split meshes per material, so that we only have one material per mesh (for VBO support)
		lGeomConverter.SplitMeshesPerMaterial(pScene, true);
	}
	else
	{
		printf("Unable to import file %s.\n", _filepath);
		printf("Error reported: ", m_pImporter->GetStatus().GetErrorString());
	}
	m_pImporter->Destroy();
	m_pIoSettings->Destroy();

	return pScene;
}

bool CVS_FbxImporter::import(const char* _filePath)
{
	auto pFbxScene = _initFBXScene(_filePath);
	if (!pFbxScene)
	{
		return false;
	}
	return extractResource(pFbxScene);
}

bool CVS_FbxImporter::init()
{
	m_pFbxManager = FbxManager::Create();
	m_pIoSettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
	m_pFbxManager->SetIOSettings(m_pIoSettings);
	m_pImporter = FbxImporter::Create(m_pFbxManager, "");

	return true;
}

CVS_FbxImporter::~CVS_FbxImporter()
{
	m_pImporter->Destroy();
	m_pIoSettings->Destroy();
	m_pFbxManager->Destroy();
}

CVS_FbxImporter::CVS_FbxImporter()
{
	this->AddFormatSupport(".fbx");
}

