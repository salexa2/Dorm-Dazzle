#include "EntityManager.h"



EntityManager::EntityManager()
{
}


EntityID EntityManager::CreateEntity()
{
    
    used_entity_ids.insert(++size);
    //printf("size:%d",size);
    return size;
}

EntityManager::Health &EntityManager::GetHealth(EntityID entityID)
{
     return ECS.Get<EntityManager::Health>(entityID);
}

void EntityManager::Destroy(EntityID e)
{
     for( const auto& [index, comps] : m_components ) { comps->Drop( e ); }
     size--;
     used_entity_ids.erase(e);
}
