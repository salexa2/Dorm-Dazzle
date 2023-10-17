#include "EntityManager.h"



EntityManager::EntityManager()
{
    size = 0; 
}


EntityID EntityManager::CreateEntity()
{
    
    used_entity_ids.insert(size);
    return size++;
}

void EntityManager::Destroy(EntityID e)
{
     for( const auto& [index, comps] : m_components ) { comps->Drop( e ); }
     size--;
     used_entity_ids.erase(e);
}
