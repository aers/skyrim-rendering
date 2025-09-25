#include "DrawWorld.h"

namespace Draw::DrawWorld
{
    static std::uint32_t& GetSceneListAccumulationJobCount()
    {
        static REL::Relocation<std::uint32_t*> target{ REL::ID(415016) };
        return *target;
    }

    static RE::BSTArray<RE::BSTArray<RE::NiPointer<RE::NiAVObject>>>& GetSceneListAccumulationArrays()
    {
        static REL::Relocation<RE::BSTArray<RE::BSTArray<RE::NiPointer<RE::NiAVObject>>>*> target{ REL::ID(415017) };
        return *target;
    }

    static RE::BSTArray<RE::NiPointer<RE::NiAVObject>>& GetSceneListAccumulationMainArray()
    {
        static REL::Relocation<RE::BSTArray<RE::NiPointer<RE::NiAVObject>>*> target{ REL::ID(415020) };
        return *target;
    }

    static RE::BSTArray<RE::NiPointer<RE::BSGeometryListCullingProcess>>& GetSceneListAccumulationCullingProcesses()
    {
        static REL::Relocation<RE::BSTArray<RE::NiPointer<RE::BSGeometryListCullingProcess>>*> target{ REL::ID(415022) };
        return *target;
    }

    static void AddObjectIfExists(RE::BSTArray<RE::NiPointer<RE::NiAVObject>>& a_array, RE::NiAVObject* a_object)
    {
        if (a_object && a_object->flags.none(RE::NiAVObject::Flag::kHidden))
            a_array.emplace_back(a_object);
    }

    // World (SceneGraph)
    // - shadow scene node (ShadowSceneNode) (child count: 10)
    // -- Sky (BSMultiBoundNode)
    // -- Weather (NiNode)
    // -- LODRoot (BSClearZNode)
    // --- LandLOD (NiNode)
    // --- DistantRefLOD (NiNode)
    // --- WaterLOD (NiNode)
    // --- LOD Trees (NiNode)
    // -- ObjectLODRoot (NiNode)
    // --- TempNodeManager (BSTempNodeManager)
    // --- WaterManager node (NiNode)
    // --- loaded cell roots
    // -- ? (empty)
    // -- BSParticleSystemManager
    // -- GrassManager node (NiNode)
    // -- ? (empty)
    // -- PortalGraph MultiBoundNode Node (NiNode)
    // -- PortalGraph Portal-shared Node (NiNode)

    //
    void hk_BuildSceneLists([[maybe_unused]] RE::Main* a_self)
    {
        RE::BSGeometryListCullingProcess* cullingProcess = GetSceneListAccumulationCullingProcesses()[0].get();
        RE::ShadowSceneNode*              shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
        RE::BSPortalGraphEntry*           portalGraphEntry = cullingProcess->portalGraphEntry;

        const std::int16_t shadowSceneNodeChildCount = shadowSceneNode->children.free_idx();
        if (shadowSceneNodeChildCount) {
            RE::NiAVObject* sky = shadowSceneNode->children[0].get();
            AddObjectIfExists(GetSceneListAccumulationMainArray(), sky);
        }
        if (shadowSceneNodeChildCount > 1) {
            RE::NiAVObject* weather = shadowSceneNode->children[1].get();
            AddObjectIfExists(GetSceneListAccumulationMainArray(), weather);
        }
        if (shadowSceneNodeChildCount > 2) {
            RE::NiAVObject* lodRoot = shadowSceneNode->children[2].get();
            if (lodRoot && lodRoot->flags.none(RE::NiAVObject::Flag::kHidden)) {
                RE::NiNode* lodRootNode = lodRoot->AsNode();
                if (lodRootNode) {
                    const std::int16_t lodRootNodeChildCount = lodRootNode->children.free_idx();
                    if (lodRootNodeChildCount) {
                        RE::NiAVObject* landLod = lodRootNode->children[0].get();
                        AddObjectIfExists(GetSceneListAccumulationMainArray(), landLod);
                    }
                    if (lodRootNodeChildCount > 1) {
                        RE::NiAVObject* distantRefLod = lodRootNode->children[1].get();
                        AddObjectIfExists(GetSceneListAccumulationMainArray(), distantRefLod);
                    }
                    if (lodRootNodeChildCount > 2) {
                        RE::NiAVObject* waterLod = lodRootNode->children[2].get();
                        AddObjectIfExists(GetSceneListAccumulationMainArray(), waterLod);
                    }
                    if (lodRootNodeChildCount > 3) {
                        RE::NiAVObject* lodTrees = lodRootNode->children[3].get();
                        AddObjectIfExists(GetSceneListAccumulationMainArray(), lodTrees);
                    }
                }
            }
        }
        std::uint32_t currentAccumulationArrayIndex = 0;

        if (!portalGraphEntry->visibleUnboundSpace) {
            GetSceneListAccumulationArrays()[0].emplace_back(RE::TESWaterSystem::GetSingleton()->waterRoot.get());
        } else {
            if (shadowSceneNodeChildCount > 3) {
                RE::NiAVObject* objectLodRoot = shadowSceneNode->children[3].get();
                if (objectLodRoot) {
                    RE::NiNode* objectLodRootAsNiNode = objectLodRoot->AsNode();
                    if (objectLodRootAsNiNode) {
                        if (RE::TES::GetSingleton()->interiorCell) {
                            const std::int16_t objectLodRootChildCount = objectLodRootAsNiNode->children.free_idx();
                            if (objectLodRootChildCount) {
                                RE::NiAVObject* tempNodeManager = objectLodRootAsNiNode->children[0].get();
                                AddObjectIfExists(GetSceneListAccumulationArrays()[1], tempNodeManager);
                            }
                            if (objectLodRootChildCount > 1) {
                                RE::NiAVObject* waterManager = objectLodRootAsNiNode->children[1].get();
                                AddObjectIfExists(GetSceneListAccumulationArrays()[0], waterManager);
                            }
                            RE::NiNode* interiorRootNode = nullptr;
                            if (RE::TES::GetSingleton()->interiorCell->loadedData)
                                interiorRootNode = RE::TES::GetSingleton()->interiorCell->loadedData->cell3D.get()->AsNode();
                            if (interiorRootNode && interiorRootNode->flags.none(RE::NiAVObject::Flag::kHidden)) {
                                interiorRootNode->flags.set(true, RE::NiAVObject::Flag::kAccumulated);
                                for (std::int16_t i = 2; i < interiorRootNode->children.free_idx(); i++) {
                                    if (i == 4)
                                        continue;
                                    RE::NiAVObject* interiorRootNodeChild = interiorRootNode->children[i].get();
                                    if (interiorRootNodeChild) {
                                        RE::NiNode* interiorRootNodeChildAsNode = interiorRootNodeChild->AsNode();
                                        if (interiorRootNodeChildAsNode && interiorRootNodeChildAsNode->flags.none(RE::NiAVObject::Flag::kHidden)) {
                                            for (std::int16_t j = i == 6 ? 1 : 0; j < interiorRootNodeChildAsNode->children.free_idx(); j++) {
                                                RE::NiAVObject* interiorRootNodeChildChild = interiorRootNodeChildAsNode->children[j].get();
                                                AddObjectIfExists(GetSceneListAccumulationArrays()[++currentAccumulationArrayIndex % GetSceneListAccumulationJobCount()], interiorRootNodeChildChild);
                                            }
                                        }
                                    }
                                }
                            }
                        } else if (objectLodRootAsNiNode->flags.none(RE::NiAVObject::Flag::kHidden)) {
                            const std::int16_t objectLodRootChildCount = objectLodRootAsNiNode->children.free_idx();
                            const bool         worldSpaceFixedDimensions = RE::TES::GetSingleton()->worldSpace && RE::TES::GetSingleton()->worldSpace->flags.all(RE::TESWorldSpace::Flag::kFixedDimensions);
                            for (std::int16_t i = 0; i < objectLodRootChildCount; i++) {
                                RE::NiAVObject* lodRootChild = objectLodRootAsNiNode->children[i].get();
                                if (lodRootChild) {
                                    RE::NiNode* lodRootChildAsNode = lodRootChild->AsNode();
                                    if (lodRootChildAsNode && lodRootChildAsNode->flags.none(RE::NiAVObject::Flag::kHidden)) {
                                        lodRootChildAsNode->flags.set(true, RE::NiAVObject::Flag::kAccumulated);
                                        if (i < 2) {
                                            // 0 = TempNodeManager, 1 = WaterManager root node
                                            GetSceneListAccumulationArrays()[++currentAccumulationArrayIndex % GetSceneListAccumulationJobCount()].emplace_back(lodRootChildAsNode);
                                            continue;
                                        }

                                        for (std::int16_t j = 2; j < lodRootChildAsNode->children.free_idx(); j++) {
                                            if (j == 4)
                                                continue;
                                            if (j == 2 && worldSpaceFixedDimensions)
                                                continue;

                                            RE::NiAVObject* cellChild = lodRootChildAsNode->children[j].get();
                                            if (cellChild && cellChild->flags.none(RE::NiAVObject::Flag::kHidden)) {
                                                RE::NiNode* cellChildAsNode = cellChild->AsNode();
                                                if (cellChildAsNode) {
                                                    for (std::int16_t k = j == 6 ? 1 : 0; k < cellChildAsNode->children.free_idx(); k++) {
                                                        RE::NiAVObject* cellChildChild = cellChildAsNode->children[k].get();
                                                        AddObjectIfExists(GetSceneListAccumulationArrays()[++currentAccumulationArrayIndex % GetSceneListAccumulationJobCount()], cellChildChild);
                                                    }
                                                } else {
                                                    GetSceneListAccumulationArrays()[++currentAccumulationArrayIndex % GetSceneListAccumulationJobCount()].emplace_back(cellChild);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        RE::BSPortalGraph* portalGraph = shadowSceneNode->portalGraph;
        if (!portalGraph || !portalGraph->IsCompatibleEntry(portalGraphEntry)) {
            portalGraphEntry->SetPortalGraph(portalGraph);
            if (portalGraph)
                portalGraphEntry->cellId = portalGraph->cellID;
            else
                portalGraphEntry->cellId = 0;
            portalGraphEntry->ClearVisibility();
        }

        if (portalGraph && portalGraphEntry->enableAccumulation) {
            for (auto room : portalGraphEntry->multiBoundRoomAccumList) {
                if (!portalGraphEntry->visibleUnboundSpace || !RE::TES::GetSingleton()->interiorCell) {
                    GetSceneListAccumulationArrays()[++currentAccumulationArrayIndex % GetSceneListAccumulationJobCount()].emplace_back(room);
                }
                for (const auto portal : room->portalList) {
                    if (cullingProcess->AddShared(portal->sharedGeometry.get())) {
                        RE::BSPortalSharedNode* sharedGeometry = portal->sharedGeometry.get();
                        if (sharedGeometry) {
                            RE::BSCompoundFrustum* compoundFrustum = shadowSceneNode->BuildSharedCompoundFrustum(cullingProcess, portal);
                            if (compoundFrustum) {
                                portalGraphEntry->AddVisibilityToMap(sharedGeometry, compoundFrustum);
                                GetSceneListAccumulationArrays()[++currentAccumulationArrayIndex % GetSceneListAccumulationJobCount()].emplace_back(sharedGeometry);
                            }
                        }
                    }
                }
            }
            if (portalGraphEntry->visibleUnboundSpace) {
                for (auto& portal : portalGraph->portals) {
                    RE::BSPortalSharedNode* sharedGeometry = portal.sharedGeometry.get();
                    if (cullingProcess->AddShared(sharedGeometry) && sharedGeometry) {
                        GetSceneListAccumulationArrays()[++currentAccumulationArrayIndex % GetSceneListAccumulationJobCount()].emplace_back(sharedGeometry);
                    }
                }
            }
        }

        if (shadowSceneNodeChildCount > 6) {
            RE::NiAVObject* grass = shadowSceneNode->children[6].get();
            AddObjectIfExists(GetSceneListAccumulationMainArray(), grass);
        }
        if (shadowSceneNodeChildCount > 7) {
            RE::NiAVObject* unknown = shadowSceneNode->children[7].get();
            AddObjectIfExists(GetSceneListAccumulationMainArray(), unknown);
        }
    }
}