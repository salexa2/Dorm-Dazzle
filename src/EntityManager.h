#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include   <set>
#include <unordered_map>
#include <string>
#include <typeindex>
#include <utility>
#include <memory>
#include <functional>

//#include "SparseSetHolder.hpp"



//--------------------------------------------
typedef long EntityID;
typedef std::type_index ComponentIndex;

class SparseSetHolder {
public:
    // A virtual destructor, since subclasses need their destructors to run to free memory.
    inline virtual ~SparseSetHolder() = default;
    inline virtual bool Has( EntityID ) const = 0;
    inline virtual void Drop( EntityID ) = 0;
};
// Subclasses are templated on the component type they hold.
template< typename T > class SparseSet : public SparseSetHolder {
public:
    std::unordered_map< EntityID, T > data;
    inline bool Has( EntityID e ) const override { return data.count( e ) > 0; };
    inline void Drop( EntityID e ) override { data.erase( e ); };
};


class EntityManager{

public: 
    int size; 
    std::set<EntityID> used_entity_ids;
    std::unordered_map< ComponentIndex, std::unique_ptr< SparseSetHolder > > m_components;
    
    
//T is the type of component


//use ECS.GET

    struct Position { int x, y; }; // or: struct Position : public vec2 {};
    struct Velocity { int x, y; }; // or: struct Velocity : public vec2 {};
    struct Gravity { float meters_per_second; };
    struct Sprite { std::string image; int size; };
    struct Health { float percent; };
    struct Script { std::string name; };
    struct Money{float price;};
  
   


   


 //---------------FUNCTIONS--------------
    EntityManager();
    EntityID CreateEntity();
    template< typename T >
    T& Get( EntityID entity ){
          return GetAppropriateSparseSet<T>()[ entity ];
    }
  

    
    template< typename T >
    std::unordered_map< EntityID, T >& GetAppropriateSparseSet(){
            // Get the index for T's SparseSet
        const ComponentIndex index = std::type_index(typeid(T));
        // Create the actual sparse set if needed.
        if( m_components[ index ] == nullptr ) m_components[index] = std::make_unique< SparseSet<T> >();
        // It's safe to cast the SparseSetHolder to its subclass and return the std::unordered_map< EntityID, T > inside.
        return static_cast< SparseSet<T>& >( *m_components[ index ] ).data;
    };
    template< typename T >
    void Drop( EntityID e ){
          GetAppropriateSparseSet<T>().erase( e );
    }
    void Destroy( EntityID e );


  typedef std::function<void(EntityID)> ForEachCallback;

   template <typename EntitiesThatHaveThisComponent, typename... AndAlsoTheseComponents>
   void ForEach(const ForEachCallback& callback) {
    // Get the component index for the primary component type.
        const ComponentIndex primaryIndex = std::type_index(typeid(EntitiesThatHaveThisComponent));
        
        // Get the component indices for additional components.
        const std::vector<ComponentIndex> additionalIndices{ std::type_index(typeid(AndAlsoTheseComponents))... };
        
        // Iterate over entities in the primary component's SparseSet.
       for (const auto& [entity, value] : GetAppropriateSparseSet<EntitiesThatHaveThisComponent>()) {
            bool hasAllComponents = true;
            
            // Check if the entity has all the required components (primary and additional).
            if (!m_components[primaryIndex]->Has(entity)) {
                hasAllComponents = false;
            } else {
                for (const ComponentIndex& index : additionalIndices) {
                    if (!m_components[index]->Has(entity)) {
                        hasAllComponents = false;
                        break;
                    }
                }
            }
            
            // If the entity has all components, call the callback.
            if (hasAllComponents) {
                callback(entity);
            }
        }
   }
    
};
inline EntityManager ECS;





#endif